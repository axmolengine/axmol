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
#include "axmol/tlx/type_traits.hpp"

namespace tlx
{
struct sorted_unique_t
{
    explicit sorted_unique_t() = default;
};
inline constexpr sorted_unique_t sorted_unique{};

struct sorted_equivalent_t
{
    explicit sorted_equivalent_t() = default;
};
inline constexpr sorted_equivalent_t sorted_equivalent{};

template <bool _IsUnique, class _Kty, class _Pred, class _Container>
class flat_set_base
{
    using _Sorted_t = std::conditional_t<_IsUnique, sorted_unique_t, sorted_equivalent_t>;

public:
    using key_type       = _Kty;
    using value_type     = _Kty;
    using key_compare    = _Pred;
    using container_type = _Container;

    using size_type       = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using pointer         = typename container_type::pointer;
    using const_pointer   = typename container_type::const_pointer;
    using reference       = key_type&;
    using const_reference = const key_type&;
    using iterator        = typename container_type::iterator;
    using const_iterator  = typename container_type::const_iterator;

    flat_set_base() : _Mycont(), _Mycomp() {}
    explicit flat_set_base(container_type cont, const key_compare& pred = key_compare())
        : _Mycont(std::move(cont)), _Mycomp(pred)
    {}
    flat_set_base(_Sorted_t, container_type cont, const key_compare& pred = key_compare())
        : _Mycont(std::move(cont)), _Mycomp(pred)
    {
    }
    flat_set_base(_Sorted_t, const container_type& cont, const key_compare& pred = key_compare())
        : _Mycont(std::move(cont)), _Mycomp(pred)
    {}

    void reserve(size_t capacity) { _Mycont.reserve(capacity); }
    void clear() { _Mycont.clear(); }

    bool empty() const noexcept { return _Mycont.empty(); }
    size_t size() const noexcept { return _Mycont.size(); }

    iterator begin() noexcept { return _Mycont.begin(); }
    iterator end() noexcept { return _Mycont.end(); }
    const_iterator begin() const noexcept { return _Mycont.begin(); }
    const_iterator end() const noexcept { return _Mycont.end(); }

    const_iterator cbegin() const noexcept { return _Mycont.begin(); }
    const_iterator cend() const noexcept { return _Mycont.end(); }

    iterator find(const key_type& key)
    {
        auto it = this->lower_bound(key);
        if (it != _Mycont.end() && !_Mycomp(key, *it) && !_Mycomp(*it, key))
            return it;
        return _Mycont.end();
    }

    const_iterator find(const key_type& key) const
    {
        auto it = this->lower_bound(key);
        if (it != _Mycont.end() && !_Mycomp(key, *it) && !_Mycomp(*it, key))
            return it;
        return _Mycont.end();
    }

    bool contains(const key_type& key) const
    {
        auto it = this->lower_bound(key);
        return it != _Mycont.end() && !_Mycomp(key, *it) && !_Mycomp(*it, key);
    }

    iterator lower_bound(const key_type& key) { return std::lower_bound(_Mycont.begin(), _Mycont.end(), key, _Mycomp); }

    const_iterator lower_bound(const key_type& key) const
    {
        return std::lower_bound(_Mycont.begin(), _Mycont.end(), key, _Mycomp);
    }

    iterator upper_bound(const key_type& key) { return std::upper_bound(_Mycont.begin(), _Mycont.end(), key, _Mycomp); }

    const_iterator upper_bound(const key_type& key) const
    {
        return std::upper_bound(_Mycont.begin(), _Mycont.end(), key, _Mycomp);
    }

    std::pair<iterator, iterator> equal_range(const key_type& key) { return {lower_bound(key), upper_bound(key)}; }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return {this->lower_bound(key), this->upper_bound(key)};
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last)
    {
        _Mycont.insert(_Mycont.end(), first, last);

        std::sort(_Mycont.begin(), _Mycont.end(), _Mycomp);

        if constexpr (_IsUnique)
        {
            _Mycont.erase(
                std::unique(_Mycont.begin(), _Mycont.end(),
                            [&](const key_type& a, const key_type& b) { return !_Mycomp(a, b) && !_Mycomp(b, a); }),
                _Mycont.end());
        }
    }

    std::pair<iterator, bool> insert(const key_type& k) { return emplace(k); }

    iterator insert(const_iterator hint, const key_type& k) { return emplace_hint(hint, k); }

    template <typename... _Args>
    std::pair<iterator, bool> emplace(_Args&&... _Vals)
    {
        if constexpr (std::is_same_v<_Kty, tlx::select1st_t<std::remove_cvref_t<_Args>...>>)
        {
            return _Emplace(std::forward<_Args>(_Vals)...);
        }
        else
        {
            return _Emplace(_Kty(std::forward<_Args>(_Vals)...));
        }
    }

    template <typename _Ty>
    std::pair<iterator, bool> _Emplace(_Ty&& _Val)
    {
        if constexpr (_IsUnique)
        {
            auto _Where = lower_bound(_Val);
            if (_Where != cend() && !_Mycomp(_Val, *_Where))
            {
                return std::pair{_Where, false};
            }

            return std::pair{_Mycont.emplace(_Where, std::forward<_Ty>(_Val)), true};
        }
        else
        {
            return std::pair{_Mycont.emplace(upper_bound(_Val), std::forward<_Ty>(_Val)), true};
        }
    }

    template <typename... _Args>
    iterator emplace_hint(const_iterator _Hint, _Args&&... _Vals)
    {
        if constexpr (std::is_same_v<_Kty, tlx::select1st_t<std::remove_cvref_t<_Args>...>>)
        {
            return _Emplace_hint(_Hint, std::forward<_Args>(_Vals)...);
        }
        else
        {
            return _Emplace_hint(_Hint, _Kty(std::forward<_Args>(_Vals)...));
        }
    }

    template <typename _Ty>
    iterator _Emplace_hint(const_iterator _Where, _Ty&& _Val)
    {
        const const_iterator _Begin = cbegin();
        const const_iterator _End   = cend();
        if constexpr (_IsUnique)
        {
            // look for lower_bound(_Val)
            if (_Where == _End || !_Mycomp(*_Where, _Val))
            {
                // _Val <= *_Where
                if (_Where == _Begin || _Mycomp(*(_Where - 1), _Val))
                {
                    // _Val > *(_Where-1) ~ lower_bound is _Where
                }
                else
                {
                    // _Val <= *(_Where-1) ~ lower_bound is in [_Begin,_Where-1]
                    _Where = std::lower_bound(_Begin, _Where - 1, _Val, _Mycomp);
                }
            }
            else
            {
                // _Val > *_Where ~ lower_bound is in [_Where+1,_End]
                _Where = std::lower_bound(_Where + 1, _End, _Val, _Mycomp);
            }

            if (_Where != _End && !_Mycomp(_Val, *_Where))
            {
                return begin() + std::distance(cbegin(), _Where);
            }

            if constexpr (std::is_same_v<std::remove_cvref_t<_Ty>, _Kty>)
            {
                return _Mycont.emplace(_Where, std::forward<_Ty>(_Val));
            }
            else
            {
                _Kty _Keyval(std::forward<_Ty>(_Val));
                return _Mycont.emplace(_Where, std::move(_Keyval));
            }
        }
        else
        {
            // look for closest position just prior to _Where, respecting ordering
            if (_Where == _End || _Compare(_Val, *_Where))
            {
                // _Val < *_Where
                if (_Where == _Begin || !_Compare(_Val, *(_Where - 1)))
                {
                    // _Val >= *(_Where-1) ~ closest valid position is _Where
                }
                else
                {
                    // _Val < *(_Where-1) ~ closest valid position is upper_bound(_Val) located in [_Begin,_Where-1]
                    _Where = std::upper_bound(_Begin, _Where - 1, _Val, _Mycomp);
                }
            }
            else
            {
                // _Val >= *_Where ~ search for lower_bound in [_Where,_End] to place _Val "as close as possible"
                _Where = std::lower_bound(_Where, _End, _Val, _Mycomp);
            }

            return _Mycont.emplace(_Where, std::forward<_Ty>(_Val));
        }
    }

    size_type erase(const key_type& key)
    {
        if constexpr (_IsUnique)
        {
            const const_iterator _Where = lower_bound(key);
            if (_Where != cend() && !_Mycomp(key, *_Where))
            {
                _Mycont.erase(_Where);
                return 1;
            }
            return 0;
        }
        else
        {
            const auto [_First, _Last] = equal_range(key);

            const auto _Removed = static_cast<size_type>(_Last - _First);
            _Mycont.erase(_First, _Last);
            return _Removed;
        }
    }

    iterator erase(const_iterator it) { return _Mycont.erase(it); }

    const auto& keys() const { return _Mycont; }

protected:
    container_type _Mycont;
    [[no_unique_address]] key_compare _Mycomp;
};

template <class _Kty, class _Pred = std::less<_Kty>, class _Container = std::vector<_Kty>>
using flat_set = _TLX flat_set_base<true, _Kty, _Pred, _Container>;

template <class _Kty, class _Pred = std::less<_Kty>, class _Container = std::vector<_Kty>>
using flat_multiset = _TLX flat_set_base<false, _Kty, _Pred, _Container>;

}  // namespace tlx
