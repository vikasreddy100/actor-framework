// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#include "caf/flow/subscription.hpp"

#include "caf/flow/coordinator.hpp"

namespace caf::flow {

subscription::impl::~impl() {
  // nop
}

void subscription::impl_base::ref_disposable() const noexcept {
  this->ref();
}

void subscription::impl_base::deref_disposable() const noexcept {
  this->deref();
}

void subscription::impl_base::ref_coordinated() const noexcept {
  this->ref();
}

void subscription::impl_base::deref_coordinated() const noexcept {
  this->deref();
}

subscription::listener::~listener() {
  // nop
}

bool subscription::fwd_impl::disposed() const noexcept {
  return src_ == nullptr;
}

void subscription::fwd_impl::request(size_t n) {
  if (src_)
    parent()->delay_fn([src = src_, snk = snk_, n] { //
      src->on_request(snk.get(), n);
    });
}

void subscription::fwd_impl::dispose() {
  if (src_) {
    parent()->delay_fn([src = src_, snk = snk_] { //
      src->on_cancel(snk.get());
    });
    src_.reset();
    snk_.reset();
  }
}

coordinator* subscription::trivial_impl::parent() const noexcept {
  return parent_;
}

bool subscription::trivial_impl::disposed() const noexcept {
  return disposed_;
}

void subscription::trivial_impl::request(size_t) {
  // nop
}

void subscription::trivial_impl::dispose() {
  disposed_ = true;
}

} // namespace caf::flow
