/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "caf/message_builder.hpp"

namespace caf {

namespace {

using namespace detail;

enum to_msg_policy {
  move_msg,
  copy_msg,
};

template <to_msg_policy Policy, class TypeListBuilder, class ElementVector>
message to_message_impl(size_t storage_size, TypeListBuilder& types,
                        ElementVector& elements) {
  if (storage_size == 0)
    return message{};
  auto vptr = malloc(sizeof(message_data) + storage_size);
  if (vptr == nullptr)
    throw std::bad_alloc();
  message_data* raw_ptr;
  if constexpr (Policy == move_msg)
    raw_ptr = new (vptr) message_data(types.move_to_list());
  else
    raw_ptr = new (vptr) message_data(types.copy_to_list());
  intrusive_cow_ptr<message_data> ptr{raw_ptr, false};
  auto storage = raw_ptr->storage();
  for (auto& element : elements)
    if constexpr (Policy == move_msg)
      storage = element->move_init(storage);
    else
      storage = element->copy_init(storage);
  return message{std::move(ptr)};
}

} // namespace

void message_builder::clear() noexcept {
  storage_size_ = 0;
  types_.clear();
  elements_.clear();
}

message message_builder::to_message() const {
  return to_message_impl<copy_msg>(storage_size_, types_, elements_);
}

message message_builder::move_to_message() {
  return to_message_impl<move_msg>(storage_size_, types_, elements_);
}

} // namespace caf
