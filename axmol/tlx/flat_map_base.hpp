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
class flat_map_base
{
public:
    using key_type         = typename _Traits::key_type;
    using mapped_type      = typename _Traits::mapped_type;
    using allocator_type   = typename _Traits::allocator_type;
    using key_container    = typename _Traits::key_container;
    using mapped_container = typename _Traits::mapped_container;

protected:
    using key_alloc    = typename std::allocator_traits<allocator_type>::template rebind_alloc<key_type>;
    using mapped_alloc = typename std::allocator_traits<allocator_type>::template rebind_alloc<mapped_type>;

public:
    using key_compare = typename _Traits::key_compare;

    using size_type       = typename std::allocator_traits<key_alloc>::size_type;
    using difference_type = typename std::allocator_traits<key_alloc>::difference_type;

#pragma region value and iterator proxy
    struct value_proxy
    {
        const key_type& first;
        mapped_type& second;

        value_proxy* operator->() { return this; }
        const value_proxy* operator->() const { return this; }
    };

    class iterator
    {
    public:
        using key_iter    = typename key_container::iterator;
        using mapped_iter = typename mapped_container::iterator;
        key_iter _k_it;
        mapped_iter _v_it;

        using difference_type   = typename key_container::difference_type;
        using value_type        = value_proxy;
        using reference         = value_proxy;
        using pointer           = value_proxy*;
        using iterator_category = std::random_access_iterator_tag;

        iterator(key_iter k_it, mapped_iter v_it) : _k_it(k_it), _v_it(v_it) {}
        reference operator*() const { return value_proxy{*_k_it, *_v_it}; }
        value_proxy operator->() const { return {*_k_it, *_v_it}; }

        iterator& operator++()
        {
            ++_k_it;
            ++_v_it;
            return *this;
        }
        iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const iterator& other) const { return _k_it == other._k_it; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    class const_iterator
    {
    public:
        using key_iter    = typename key_container::const_iterator;
        using mapped_iter = typename mapped_container::const_iterator;
        key_iter _k_it;
        mapped_iter _v_it;

        using difference_type   = typename key_container::difference_type;
        using value_type        = value_proxy;
        using reference         = value_proxy;
        using pointer           = const value_proxy*;
        using iterator_category = std::random_access_iterator_tag;

        const_iterator(key_iter k_it, mapped_iter v_it) : _k_it(k_it), _v_it(v_it) {}
        reference operator*() const { return value_proxy{*_k_it, const_cast<mapped_type&>(*_v_it)}; }
        value_proxy operator->() const { return {*_k_it, const_cast<mapped_type&>(*_v_it)}; }

        const_iterator& operator++()
        {
            ++_k_it;
            ++_v_it;
            return *this;
        }
        const_iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return _k_it == other._k_it; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };
#pragma endregion

    explicit flat_map_base(const key_compare& pred = key_compare(), const allocator_type& alloc = allocator_type())
        : _Mykeys(_TLX __one_then_variadic_args_t{}, pred, key_container(alloc)), _Myvals(alloc)
    {}

    void reserve(size_t capacity)
    {
        _Mykeys._Myval2.reserve(capacity);
        _Myvals.reserve(capacity);
    }
    void clear()
    {
        _Mykeys._Myval2.clear();
        _Myvals.clear();
    }

    bool empty() const noexcept { return _Mykeys._Myval2.empty(); }
    size_t size() const noexcept { return _Mykeys._Myval2.size(); }

    iterator begin() noexcept { return iterator(_Mykeys._Myval2.begin(), _Myvals.begin()); }
    iterator end() noexcept { return iterator(_Mykeys._Myval2.end(), _Myvals.end()); }
    const_iterator begin() const noexcept { return const_iterator(_Mykeys._Myval2.begin(), _Myvals.begin()); }
    const_iterator end() const noexcept { return const_iterator(_Mykeys._Myval2.end(), _Myvals.end()); }

    iterator find(const key_type& key)
    {
        const auto& pred = _Mykeys._Get_first();
        auto it          = lower_bound(key);
        if (it != end() && !pred(key, (*it).first) && !pred((*it).first, key))
            return it;
        return end();
    }

    const_iterator find(const key_type& key) const
    {
        const auto& pred = _Mykeys._Get_first();
        auto it          = lower_bound(key);
        if (it != end() && !pred(key, (*it).first) && !pred((*it).first, key))
            return it;
        return end();
    }

    iterator lower_bound(const key_type& key)
    {
        const auto& pred = _Mykeys._Get_first();
        auto k_it        = std::lower_bound(_Mykeys._Myval2.begin(), _Mykeys._Myval2.end(), key, pred);
        auto v_it        = _Myvals.begin() + (k_it - _Mykeys._Myval2.begin());
        return iterator(k_it, v_it);
    }

    const_iterator lower_bound(const key_type& key) const
    {
        const auto& pred = _Mykeys._Get_first();
        auto k_it        = std::lower_bound(_Mykeys._Myval2.begin(), _Mykeys._Myval2.end(), key, pred);
        auto v_it        = _Myvals.begin() + (k_it - _Mykeys._Myval2.begin());
        return const_iterator(k_it, v_it);
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (; first != last; ++first)
        {
            insert(first->first, first->second);
        }
    }

    std::pair<iterator, bool> insert(const key_type& k, const mapped_type& v)
    {
        return emplace(std::forward<key_type>(k), std::forward<mapped_type>(v));
    }

    template <class _KeyArg, class... _MArgs>
    std::pair<iterator, bool> emplace(_KeyArg&& key, _MArgs&&... mapped_args)
    {
        const auto& pred = _Mykeys._Get_first();
        auto key_it      = std::lower_bound(_Mykeys._Myval2.begin(), _Mykeys._Myval2.end(), key, pred);
        auto mapped_it   = _Myvals.begin() + (key_it - _Mykeys._Myval2.begin());

        if constexpr (_Traits::allow_duplicates)
        {
            // multimap: always insert at upper_bound
            key_it    = _Mykeys._Myval2.insert(key_it, std::forward<_KeyArg>(key));
            mapped_it = _Myvals.insert(mapped_it, mapped_type(std::forward<_MArgs>(mapped_args)...));
            return {iterator(key_it, mapped_it), true};
        }
        else
        {
            // map: only insert if key not found
            if (key_it == _Mykeys._Myval2.end() || pred(key, *key_it))
            {
                key_it    = _Mykeys._Myval2.insert(key_it, std::forward<_KeyArg>(key));
                mapped_it = _Myvals.insert(mapped_it, mapped_type(std::forward<_MArgs>(mapped_args)...));
                return {iterator(key_it, mapped_it), true};
            }
            else
            {
                return {iterator(key_it, mapped_it), false};
            }
        }
    }

    // erase by iterator
    iterator erase(iterator pos)
    {
        auto k_it = pos._k_it;
        auto v_it = pos._v_it;
        _Mykeys._Myval2.erase(k_it);
        _Myvals.erase(v_it);
        return iterator(k_it, v_it);
    }

    // erase by key
    size_type erase(const key_type& key)
    {
        const auto& pred = _Mykeys._Get_first();
        auto k_it        = std::lower_bound(_Mykeys._Myval2.begin(), _Mykeys._Myval2.end(), key, pred);
        auto v_it        = _Myvals.begin() + (k_it - _Mykeys._Myval2.begin());

        if (k_it == _Mykeys._Myval2.end() || pred(key, *k_it) || pred(*k_it, key))
            return 0;  // not found

        if constexpr (_Traits::allow_duplicates)
        {
            // multimap: erase all equal keys
            auto k_end = std::upper_bound(k_it, _Mykeys._Myval2.end(), key, pred);
            auto v_end = _Myvals.begin() + (k_end - _Mykeys._Myval2.begin());

            size_type count = static_cast<size_type>(k_end - k_it);
            _Mykeys._Myval2.erase(k_it, k_end);
            _Myvals.erase(v_it, v_end);
            return count;
        }
        else
        {
            // map: erase single element
            _Mykeys._Myval2.erase(k_it);
            _Myvals.erase(v_it);
            return 1;
        }
    }

    const auto& keys() const { return _Mykeys; }

protected:
    _TLX __compressed_pair<key_compare, key_container> _Mykeys;
    mapped_container _Myvals;
};

}  // namespace detail
}  // namespace tlx
