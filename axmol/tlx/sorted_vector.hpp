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
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <type_traits>
#include <memory>
#include "axmol/tlx/memory.hpp"

namespace axstd
{
namespace detail
{
template <class _Alloc, class _Value_type>
using _Rebind_alloc_t = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Value_type>;

/****************************************************************************
 * sorted_vector: generic flat sorted container implementation base on std::vector
 *
 * This class template provides the core implementation for all flat
 * associative containers (flat_map, flat_multimap, flat_set, flat_multiset).
 *
 * Design:
 * - Storage is backed by a contiguous sequence (std::vector).
 * - Elements are kept sorted according to the key comparator (_Traits::key_compare).
 * - Lookup operations (find, lower_bound, upper_bound, equal_range) use
 *   binary search on the vector, simulating the semantics of a balanced tree.
 *
 * Traits:
 * - _Traits defines key_type, value_type, allocator_type, key_extractor,
 *   key_compare, and a constexpr bool allow_duplicates.
 * - allow_duplicates = false → map/set semantics (unique keys).
 * - allow_duplicates = true  → multimap/multiset semantics (duplicate keys allowed).
 *
 * Complexity:
 * - Lookup: O(log N) (binary search).
 * - Insert/Erase: O(N) (vector reallocation/shifting).
 * - Iteration: O(N), cache-friendly due to contiguous storage.
 *
 * Purpose:
 * - Serves as the common base for flat_map/flat_set variants.
 * - Provides STL-compatible interface (begin/end, find, lower_bound, upper_bound, equal_range).
 * - Mimics tree-based associative containers but with flat storage for better
 *   cache locality and reduced memory overhead.
 ****************************************************************************/
template <typename _Traits>
class sorted_vector
{
public:
    using key_type       = typename _Traits::key_type;
    using value_type     = typename _Traits::value_type;
    using allocator_type = typename _Traits::allocator_type;

protected:
    using _Alty        = _Rebind_alloc_t<allocator_type, value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_compare = typename _Traits::value_compare;
    using key_compare   = typename _Traits::key_compare;
    using key_extractor = typename _Traits::key_extractor;

    using size_type       = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using pointer         = typename _Alty_traits::pointer;
    using const_pointer   = typename _Alty_traits::const_pointer;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using container_type  = std::vector<value_type, allocator_type>;
    using iterator        = typename container_type::iterator;
    using const_iterator  = typename container_type::const_iterator;

    explicit sorted_vector(const key_compare& pred = key_compare(), const allocator_type& alloc = allocator_type())
        : _Mypair(pred, container_type(alloc))
    {}

    void reserve(size_t capacity) { _Mypair.second().reserve(capacity); }
    void clear() { _Mypair.second().clear(); }

    bool empty() const noexcept { return _Mypair.second().empty(); }
    size_t size() const noexcept { return _Mypair.second().size(); }

    iterator begin() noexcept { return _Mypair.second().begin(); }
    iterator end() noexcept { return _Mypair.second().end(); }
    const_iterator begin() const noexcept { return _Mypair.second().begin(); }
    const_iterator end() const noexcept { return _Mypair.second().end(); }

    iterator find(const key_type& key)
    {
        auto& cont = _Mypair.second();
        auto& pred = _Mypair.first();
        auto it    = lower_bound(key);
        if (it != cont.end() && !pred(key, key_extractor{}(*it)) && !pred(key_extractor{}(*it), key))
            return it;
        return cont.end();
    }

    const_iterator find(const key_type& key) const
    {
        const auto& cont = _Mypair.second();
        const auto& pred = _Mypair.first();
        auto it          = lower_bound(key);
        if (it != cont.end() && !pred(key, key_extractor{}(*it)) && !pred(key_extractor{}(*it), key))
            return it;
        return cont.end();
    }

    iterator lower_bound(const key_type& key)
    {
        auto& cont = _Mypair.second();
        auto& pred = _Mypair.first();
        return std::lower_bound(cont.begin(), cont.end(), key,
                                [&](const value_type& v, const key_type& k) { return pred(key_extractor{}(v), k); });
    }

    const_iterator lower_bound(const key_type& key) const
    {
        const auto& cont = _Mypair.second();
        const auto& pred = _Mypair.first();
        return std::lower_bound(cont.begin(), cont.end(), key,
                                [&](const value_type& v, const key_type& k) { return pred(key_extractor{}(v), k); });
    }

    iterator upper_bound(const key_type& key)
    {
        auto& cont = _Mypair.second();
        auto& pred = _Mypair.first();
        return std::upper_bound(cont.begin(), cont.end(), key,
                                [&](const key_type& k, const value_type& v) { return pred(k, key_extractor{}(v)); });
    }

    const_iterator upper_bound(const key_type& key) const
    {
        const auto& cont = _Mypair.second();
        const auto& pred = _Mypair.first();
        return std::upper_bound(cont.begin(), cont.end(), key,
                                [&](const key_type& k, const value_type& v) { return pred(k, key_extractor{}(v)); });
    }

    std::pair<iterator, iterator> equal_range(const key_type& key) { return {lower_bound(key), upper_bound(key)}; }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return {lower_bound(key), upper_bound(key)};
    }

    iterator insert(iterator /*hint*/, const value_type& v)
    {
        // ignore hint, do sorted insertion
        return insert(v).first;
    }

    std::pair<iterator, bool> insert(const value_type& v)
    {
        auto& cont        = _Mypair.second();
        auto& comp        = _Mypair.first();
        const key_type& k = key_extractor{}(v);

        if constexpr (_Traits::allow_duplicates)
        {
            auto it = upper_bound(k);
            it      = cont.insert(it, v);
            return {it, true};
        }
        else
        {
            auto it = lower_bound(k);
            if (it == cont.end() || comp(k, key_extractor{}(*it)))
            {
                it = cont.insert(it, v);
                return {it, true};
            }
            return {it, false};
        }
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        value_type v(std::forward<Args>(args)...);
        return insert(v);
    }

    size_type erase(const key_type& key)
    {
        auto range      = equal_range(key);
        size_type count = std::distance(range.first, range.second);
        _Mypair.second().erase(range.first, range.second);
        return count;
    }

    iterator erase(const_iterator it) { return _Mypair.second().erase(it); }

protected:
    ::axstd::compressed_pair<key_compare, container_type> _Mypair;
};

/// identity extractor: for flat_set
template <class Key>
struct identity
{
    const Key& operator()(const Key& k) const noexcept { return k; }
};

/// select1st extractor: for flat_map
template <class _Pair>
struct select1st
{
    using Key = typename _Pair::first_type;
    const Key& operator()(const _Pair& p) const noexcept { return p.first; }
};

}  // namespace detail
}  // namespace axstd
