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
#include "axmol/tlx/sorted_vector.hpp"

namespace axstd
{
// flat_set traits
template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
struct flat_set_traits
{
    using key_type       = Key;
    using value_type     = Key;
    using key_compare    = Compare;
    using value_compare  = Compare;
    using allocator_type = Alloc;
    using key_extractor  = detail::identity<Key>;

    static constexpr bool allow_duplicates = false;
};

/// flat_set
template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
class flat_set : public detail::sorted_vector<flat_set_traits<Key, Compare, Alloc>>
{
    using impl_type = detail::sorted_vector<flat_set_traits<Key, Compare, Alloc>>;

public:
    using impl_type::impl_type;
};
}  // namespace axstd
