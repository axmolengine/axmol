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

// flat_impl: generic flat sorted container implementation
template <typename _Traits>
class flat_impl
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

    explicit flat_impl(const key_compare& pred = key_compare(), const allocator_type& alloc = allocator_type())
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

    std::pair<iterator, bool> insert(const value_type& v)
    {
        auto& cont        = _Mypair.second();
        auto& comp        = _Mypair.first();
        const key_type& k = key_extractor{}(v);
        auto it           = lower_bound(k);
        if (it == cont.end() || comp(k, key_extractor{}(*it)))
        {
            it = cont.insert(it, v);
            return {it, true};
        }
        return {it, false};
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        value_type v(std::forward<Args>(args)...);
        return insert(v);
    }

    size_type erase(const key_type& key)
    {
        auto it = find(key);
        if (it != end())
        {
            _Mypair.second().erase(it);
            return 1;
        }
        return 0;
    }

    iterator erase(const_iterator it) { return _Mypair.second().erase(it); }

protected:
    ::axstd::compressed_pair<key_compare, container_type> _Mypair;
};
}  // namespace detail
}  // namespace axstd
