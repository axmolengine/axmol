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
#include "axmol/tlx/flat_map_base.hpp"
#include <vector>

namespace tlx
{

// flat_map traits
template <class Key,
          class T,
          class Compare     = std::less<Key>,
          class KeyAlloc    = std::allocator<Key>,
          class MappedAlloc = std::allocator<T>>
struct flat_multimap_traits
{
    using key_type         = Key;
    using mapped_type      = T;
    using key_compare      = Compare;
    using allocator_type   = KeyAlloc;
    using key_container    = std::vector<Key, KeyAlloc>;
    using mapped_container = std::vector<T, MappedAlloc>;

    static constexpr bool allow_duplicates = true;
};

template <class Key,
          class T,
          class Compare     = std::less<Key>,
          class KeyAlloc    = std::allocator<Key>,
          class MappedAlloc = std::allocator<T>>
class flat_multimap : public detail::flat_map_base<flat_multimap_traits<Key, T, Compare, KeyAlloc, MappedAlloc>>
{
    using traits    = flat_multimap_traits<Key, T, Compare, KeyAlloc, MappedAlloc>;
    using impl_type = detail::flat_map_base<traits>;

public:
    using impl_type::impl_type;
};

}  // namespace tlx
