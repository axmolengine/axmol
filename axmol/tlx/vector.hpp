/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
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
                                           ::tlx::vector<_Ty, _Alloc, fill_policy::no_fill_trivial_dtor>>::type;

}  // namespace tlx
