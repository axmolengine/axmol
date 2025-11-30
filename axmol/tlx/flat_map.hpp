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
#include <stdint.h>
#include <algorithm>
#include <functional>
#include <utility>
#include <type_traits>
#include <vector>
#include "yasio/tlx/memory.hpp"

namespace tlx
{

/// CLASS template flat_map_base
template <bool _IsUnique,
          typename _Key,
          typename _Mapped,
          typename _Compare,
          typename _KeyContainer,
          typename _MappedContainer>
class flat_map_base
{
public:
    using key_type         = _Key;
    using mapped_type      = _Mapped;
    using key_container    = _KeyContainer;
    using mapped_container = _MappedContainer;
    using key_compare      = _Compare;

    using size_type       = size_t;
    using difference_type = ptrdiff_t;

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
        key_iter _Key_it;
        mapped_iter _Mapped_it;

        using difference_type   = typename key_container::difference_type;
        using value_type        = value_proxy;
        using reference         = value_proxy;
        using pointer           = value_proxy*;
        using iterator_category = std::random_access_iterator_tag;

        iterator(key_iter k_it, mapped_iter v_it) : _Key_it(k_it), _Mapped_it(v_it) {}
        reference operator*() const { return value_proxy{*_Key_it, *_Mapped_it}; }
        value_proxy operator->() const { return {*_Key_it, *_Mapped_it}; }

        iterator& operator++()
        {
            ++_Key_it;
            ++_Mapped_it;
            return *this;
        }
        iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const iterator& other) const { return _Key_it == other._Key_it; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    class const_iterator
    {
    public:
        using key_iter    = typename key_container::const_iterator;
        using mapped_iter = typename mapped_container::const_iterator;
        key_iter _Key_it;
        mapped_iter _Mapped_it;

        using difference_type   = typename key_container::difference_type;
        using value_type        = value_proxy;
        using reference         = value_proxy;
        using pointer           = const value_proxy*;
        using iterator_category = std::random_access_iterator_tag;

        const_iterator(key_iter k_it, mapped_iter v_it) : _Key_it(k_it), _Mapped_it(v_it) {}
        reference operator*() const { return value_proxy{*_Key_it, const_cast<mapped_type&>(*_Mapped_it)}; }
        value_proxy operator->() const { return {*_Key_it, const_cast<mapped_type&>(*_Mapped_it)}; }

        const_iterator& operator++()
        {
            ++_Key_it;
            ++_Mapped_it;
            return *this;
        }
        const_iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return _Key_it == other._Key_it; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };
#pragma endregion

    flat_map_base() : flat_map_base(key_compare()) {}
    explicit flat_map_base(const key_compare& pred) : _Mykeys(), _Myvals(), _Mycomp(pred) {}

    void reserve(size_t capacity)
    {
        _Mykeys.reserve(capacity);
        _Myvals.reserve(capacity);
    }
    void clear()
    {
        _Mykeys.clear();
        _Myvals.clear();
    }

    bool empty() const noexcept { return _Mykeys.empty(); }
    size_t size() const noexcept { return _Mykeys.size(); }

    iterator begin() noexcept { return iterator(_Mykeys.begin(), _Myvals.begin()); }
    iterator end() noexcept { return iterator(_Mykeys.end(), _Myvals.end()); }
    const_iterator begin() const noexcept { return const_iterator(_Mykeys.begin(), _Myvals.begin()); }
    const_iterator end() const noexcept { return const_iterator(_Mykeys.end(), _Myvals.end()); }
    const_iterator cbegin() const noexcept { return const_iterator{std::cbegin(_Mykeys), std::cbegin(_Myvals)}; }
    const_iterator cend() const noexcept { return const_iterator{std::cend(_Mykeys), std::cend(_Myvals)}; }

    iterator find(const key_type& key)
    {
        auto it = this->lower_bound(key);
        if (it != end() && !_Mycomp(key, (*it).first) && !_Mycomp((*it).first, key))
            return it;
        return end();
    }

    const_iterator find(const key_type& key) const
    {
        auto it = lower_bound(key);
        if (it != end() && !_Mycomp(key, (*it).first) && !_Mycomp((*it).first, key))
            return it;
        return end();
    }

    iterator lower_bound(const key_type& key)
    {
        auto k_it = std::lower_bound(_Mykeys.begin(), _Mykeys.end(), key, _Mycomp);
        auto v_it = _Myvals.begin() + (k_it - _Mykeys.begin());
        return iterator(k_it, v_it);
    }

    const_iterator lower_bound(const key_type& key) const
    {
        auto k_it = std::lower_bound(_Mykeys.begin(), _Mykeys.end(), key, _Mycomp);
        auto v_it = _Myvals.begin() + (k_it - _Mykeys.begin());
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
        auto key_it    = std::lower_bound(_Mykeys.begin(), _Mykeys.end(), key, _Mycomp);
        auto mapped_it = _Myvals.begin() + (key_it - _Mykeys.begin());

        if constexpr (_IsUnique)
        {
            // map: only insert if key not found
            if (key_it == _Mykeys.end() || _Mycomp(key, *key_it))
            {
                key_it    = _Mykeys.insert(key_it, std::forward<_KeyArg>(key));
                mapped_it = _Myvals.insert(mapped_it, mapped_type(std::forward<_MArgs>(mapped_args)...));
                return {iterator(key_it, mapped_it), true};
            }
            else
            {
                return {iterator(key_it, mapped_it), false};
            }
        }
        else
        {
            // multimap: always insert at upper_bound
            key_it    = _Mykeys.insert(key_it, std::forward<_KeyArg>(key));
            mapped_it = _Myvals.insert(mapped_it, mapped_type(std::forward<_MArgs>(mapped_args)...));
            return {iterator(key_it, mapped_it), true};
        }
    }

    // erase by iterator
    iterator erase(iterator pos)
    {
        auto key_it    = _Mykeys.erase(pos._Key_it);
        auto mapped_it = _Myvals.erase(pos._Mapped_it);
        return iterator(key_it, mapped_it);
    }

    // erase by key
    size_type erase(const key_type& key)
    {
        auto k_it = std::lower_bound(_Mykeys.begin(), _Mykeys.end(), key, _Mycomp);
        auto v_it = _Myvals.begin() + (k_it - _Mykeys.begin());

        if (k_it == _Mykeys.end() || _Mycomp(key, *k_it) || _Mycomp(*k_it, key))
            return 0;  // not found

        if constexpr (_IsUnique)
        {
            // map: erase single element
            _Mykeys.erase(k_it);
            _Myvals.erase(v_it);
            return 1;
        }
        else
        {
            // multimap: erase all equal keys
            auto k_end = std::upper_bound(k_it, _Mykeys.end(), key, _Mycomp);
            auto v_end = _Myvals.begin() + (k_end - _Mykeys.begin());

            size_type count = static_cast<size_type>(k_end - k_it);
            _Mykeys.erase(k_it, k_end);
            _Myvals.erase(v_it, v_end);
            return count;
        }
    }

    const auto& keys() const { return _Mykeys; }

protected:
    key_container _Mykeys;
    mapped_container _Myvals;
    [[no_unique_address]] key_compare _Mycomp;
};

/// CLASS template flat_map
template <typename _Key,
          typename _Mapped,
          typename _Compare         = std::less<_Key>,
          typename _KeyContainer    = std::vector<_Key>,
          typename _MappedContainer = std::vector<_Mapped>>
class flat_map : public _TLX flat_map_base<true, _Key, _Mapped, _Compare, _KeyContainer, _MappedContainer>
{
    using impl_type = _TLX flat_map_base<true, _Key, _Mapped, _Compare, _KeyContainer, _MappedContainer>;

public:
    using impl_type::impl_type;

    // operator[] for map semantics
    _Mapped& operator[](const _Key& key)
    {
        auto it = this->lower_bound(key);
        if (it == this->end() || this->_Mycomp(key, (*it).first))
        {
            auto pos = it._Key_it;
            auto idx = pos - this->_Mykeys.begin();
            this->_Mykeys.insert(pos, key);
            this->_Myvals.emplace(this->_Myvals.begin() + idx);
            it = typename impl_type::iterator(this->_Mykeys.begin() + idx, this->_Myvals.begin() + idx);
        }
        return (*it).second;
    }

    // at() for map semantics
    _Mapped& at(const _Key& key)
    {
        auto it = this->lower_bound(key);
        if (it == this->end() || this->_Mycomp(key, (*it).first))
            throw std::out_of_range("tlx::flat_map::at: key not found");
        return (*it).second;
    }

    const _Mapped& at(const _Key& key) const
    {
        auto it = this->lower_bound(key);
        if (it == this->end() || this->_Mycomp(key, (*it).first))
            throw std::out_of_range("tlx::flat_map::at: key not found");
        return (*it).second;
    }
};

/// CLASS template flat_multimap
template <typename _Key,
          typename _Mapped,
          typename _Compare         = std::less<_Key>,
          typename _KeyContainer    = std::vector<_Key>,
          typename _MappedContainer = std::vector<_Mapped>>
using flat_multimap = _TLX flat_map_base<false, _Key, _Mapped, _Compare, _KeyContainer, _MappedContainer>;

}  // namespace tlx
