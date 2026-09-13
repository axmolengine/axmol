/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "yasio/tlx/vector.hpp"
#include "yasio/tlx/buffer_alloc.hpp"

namespace tlx
{
// alias: pod_vector
// Usage restriction: only valid for types that are BOTH:
// 1. Trivially destructible
// 2. Trivially copyable
//
// This ensures that raw memory operations (e.g. memcpy, memset) and
// skipping value-initialization are safe. In practice, this alias
// should be used only with POD or standard-layout types.
template <typename _Ty, typename _Alloc = tlx::crt_buffer_allocator<_Ty>>
using pod_vector = typename std::enable_if<std::is_trivially_copyable<_Ty>::value,
                                           ::tlx::vector<_Ty, _Alloc, fill_policy::nontrivial_dtor>>::type;

}  // namespace tlx
