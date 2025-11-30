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
#include <algorithm>
#include <functional>
#include <utility>
#include <type_traits>
#include "yasio/tlx/memory.hpp"

namespace tlx
{
namespace detail
{

template <typename _Traits>
class flat_set_base
{
public:
    using key_type       = typename _Traits::key_type;
    using value_type     = typename _Traits::key_type;
    using allocator_type = typename _Traits::allocator_type;
    using container_type = typename _Traits::container_type;

protected:
    using _Alty        = _TLX rebind_alloc_t<allocator_type, key_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using key_compare = typename _Traits::key_compare;

    using size_type       = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using pointer         = typename _Alty_traits::pointer;
    using const_pointer   = typename _Alty_traits::const_pointer;
    using reference       = key_type&;
    using const_reference = const key_type&;
    using iterator        = typename container_type::iterator;
    using const_iterator  = typename container_type::const_iterator;

    explicit flat_set_base(const key_compare& pred = key_compare(), const allocator_type& alloc = allocator_type())
        : _Mypair(_TLX __one_then_variadic_args_t{}, pred, container_type(alloc))
    {}

    void reserve(size_t capacity) { _Mypair._Myval2.reserve(capacity); }
    void clear() { _Mypair._Myval2.clear(); }

    bool empty() const noexcept { return _Mypair._Myval2.empty(); }
    size_t size() const noexcept { return _Mypair._Myval2.size(); }

    iterator begin() noexcept { return _Mypair._Myval2.begin(); }
    iterator end() noexcept { return _Mypair._Myval2.end(); }
    const_iterator begin() const noexcept { return _Mypair._Myval2.begin(); }
    const_iterator end() const noexcept { return _Mypair._Myval2.end(); }

    iterator find(const key_type& key)
    {
        auto& cont = _Mypair._Myval2;
        auto& pred = _Mypair._Get_first();
        auto it    = lower_bound(key);
        if (it != cont.end() && !pred(key, *it) && !pred(*it, key))
            return it;
        return cont.end();
    }

    const_iterator find(const key_type& key) const
    {
        const auto& cont = _Mypair._Myval2;
        const auto& pred = _Mypair._Get_first();
        auto it          = lower_bound(key);
        if (it != cont.end() && !pred(key, *it) && !pred(*it, key))
            return it;
        return cont.end();
    }

    iterator lower_bound(const key_type& key)
    {
        auto& cont = _Mypair._Myval2;
        auto& pred = _Mypair._Get_first();
        return std::lower_bound(cont.begin(), cont.end(), key, pred);
    }

    const_iterator lower_bound(const key_type& key) const
    {
        const auto& cont = _Mypair._Myval2;
        const auto& pred = _Mypair._Get_first();
        return std::lower_bound(cont.begin(), cont.end(), key, pred);
    }

    iterator upper_bound(const key_type& key)
    {
        auto& cont = _Mypair._Myval2;
        auto& pred = _Mypair._Get_first();
        return std::upper_bound(cont.begin(), cont.end(), key, pred);
    }

    const_iterator upper_bound(const key_type& key) const
    {
        const auto& cont = _Mypair._Myval2;
        const auto& pred = _Mypair._Get_first();
        return std::upper_bound(cont.begin(), cont.end(), key, pred);
    }

    std::pair<iterator, iterator> equal_range(const key_type& key) { return {lower_bound(key), upper_bound(key)}; }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return {lower_bound(key), upper_bound(key)};
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto& cont = _Mypair._Myval2;
        auto& pred = _Mypair._Get_first();

        cont.insert(cont.end(), first, last);

        std::sort(cont.begin(), cont.end(), pred);

        if constexpr (!_Traits::allow_duplicates)
        {
            cont.erase(std::unique(cont.begin(), cont.end(),
                                   [&](const key_type& a, const key_type& b) { return !pred(a, b) && !pred(b, a); }),
                       cont.end());
        }
    }

    std::pair<iterator, bool> insert(const key_type& k) { return emplace(k); }

    iterator insert(const_iterator hint, const key_type& k) { return emplace_hint(hint, k); }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        key_type k(std::forward<Args>(args)...);
        auto& cont = _Mypair._Myval2;
        auto& comp = _Mypair._Get_first();

        if constexpr (_Traits::allow_duplicates)
        {
            auto it = upper_bound(k);
            it      = cont.insert(it, std::move(k));
            return {it, true};
        }
        else
        {
            auto it = lower_bound(k);
            if (it == cont.end() || comp(k, *it))
            {
                it = cont.insert(it, std::move(k));
                return {it, true};
            }
            return {it, false};
        }
    }

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        key_type k(std::forward<Args>(args)...);
        auto& cont = _Mypair._Myval2;
        auto& comp = _Mypair._Get_first();

        if constexpr (_Traits::allow_duplicates)
        {
            return cont.insert(hint, std::move(k));
        }
        else
        {
            if (hint == cont.end() || comp(k, *hint))
            {
                return cont.insert(hint, std::move(k));
            }
            else
            {
                return emplace(std::move(k)).first;
            }
        }
    }

    size_type erase(const key_type& key)
    {
        auto range      = equal_range(key);
        size_type count = std::distance(range.first, range.second);
        _Mypair._Myval2.erase(range.first, range.second);
        return count;
    }

    iterator erase(const_iterator it) { return _Mypair._Myval2.erase(it); }

    const auto& keys() const { return _Mypair._Myval2; }

protected:
    _TLX __compressed_pair<key_compare, container_type> _Mypair;
};

}  // namespace detail
}  // namespace tlx
