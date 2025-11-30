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
#include "axmol/tlx/flat_set_base.hpp"
#include <vector>

namespace tlx
{
template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
struct flat_set_traits
{
    using key_type                         = Key;
    using value_type                       = Key;
    using key_compare                      = Compare;
    using allocator_type                   = Alloc;
    using container_type                   = std::vector<Key, Alloc>;
    static constexpr bool allow_duplicates = false;
};

template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
using flat_set = tlx::detail::flat_set_base<flat_set_traits<Key, Compare, Alloc>>;
}  // namespace tlx
