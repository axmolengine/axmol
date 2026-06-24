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

#include <type_traits>
#include <utility>
#include <iterator>
#include <new>
#include <stdexcept>
#include <algorithm>
#include <string.h>

namespace tlx
{

template <typename _Ty, size_t _Capacity>
class static_vector
{
public:
    static_assert(_Capacity > 0, "static_vector capacity must be > 0");

    using value_type      = _Ty;
    using size_type       = size_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = value_type*;
    using const_iterator  = const value_type*;

    constexpr static_vector() noexcept : _size(0) {}
    static_vector(const static_vector& other)
    {
        if constexpr (std::is_trivially_copyable_v<_Ty>)
        {
            _size = other._size;
            ::memcpy(data(), other.data(), _size * sizeof(_Ty));
        }
        else
        {
            _size = 0;
            for (size_type i = 0; i < other._size; ++i)
                emplace_back(other[i]);
        }
    }

    static_vector(static_vector&& other)
    {
        if constexpr (std::is_trivially_copyable_v<_Ty>)
        {
            _size = other._size;
            ::memcpy(data(), other.data(), _size * sizeof(_Ty));
            other._size = 0;
        }
        else
        {
            _size = 0;
            for (size_type i = 0; i < other._size; ++i)
                emplace_back(std::move(other[i]));
            other.clear();
        }
    }

    static_vector& operator=(const static_vector& other)
    {
        if (this != &other)
        {
            clear();
            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                _size = other._size;
                ::memcpy(data(), other.data(), _size * sizeof(_Ty));
            }
            else
            {
                for (size_type i = 0; i < other._size; ++i)
                    emplace_back(other[i]);
            }
        }
        return *this;
    }

    static_vector& operator=(static_vector&& other)
    {
        if (this != &other)
        {
            clear();
            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                _size = other._size;
                ::memcpy(data(), other.data(), _size * sizeof(_Ty));

                other._size = 0;
            }
            else
            {
                for (size_type i = 0; i < other._size; ++i)
                    emplace_back(std::move(other[i]));
                other.clear();
            }
        }
        return *this;
    }
    ~static_vector() noexcept { clear(); }
    constexpr size_type capacity() const noexcept { return _Capacity; }
    constexpr size_type size() const noexcept { return _size; }
    constexpr bool empty() const noexcept { return _size == 0; }
    pointer data() noexcept { return reinterpret_cast<pointer>(_buffer); }
    const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(_buffer); }

    reference operator[](size_type idx) { return *element_at(idx); }
    const_reference operator[](size_type idx) const { return *element_at(idx); }

    reference at(size_type idx)
    {
        if (idx >= _size)
            throw std::out_of_range("static_vector::at");
        return *element_at(idx);
    }

    const_reference at(size_type idx) const
    {
        if (idx >= _size)
            throw std::out_of_range("static_vector::at");
        return *element_at(idx);
    }

    iterator begin() noexcept { return data(); }
    iterator end() noexcept { return data() + _size; }
    const_iterator begin() const noexcept { return data(); }
    const_iterator end() const noexcept { return data() + _size; }
    const_iterator cbegin() const noexcept { return data(); }
    const_iterator cend() const noexcept { return data() + _size; }

    void clear() noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<_Ty>)
        {
            for (size_type i = _size; i > 0; --i)
                std::destroy_at(data() + i - 1);
        }
        _size = 0;
    }

    template <typename... Args>
    _Ty& emplace_back(Args&&... args)
    {
        if (_size >= _Capacity)
            throw std::length_error("static_vector capacity exceeded");
        _Ty* obj = ::new (static_cast<void*>(data() + _size)) _Ty(std::forward<Args>(args)...);
        ++_size;
        return *obj;
    }

    void push_back(const _Ty& v) { emplace_back(v); }
    void push_back(_Ty&& v) { emplace_back(std::move(v)); }

    void pop_back()
    {
        if (_size == 0)
            return;
        if constexpr (!std::is_trivially_destructible_v<_Ty>)
            std::destroy_at(data() + _size - 1);
        --_size;
    }

    void resize(size_type newSize)
    {
        if (newSize > _Capacity)
            throw std::length_error("static_vector capacity exceeded");
        if constexpr (std::is_trivially_copyable_v<_Ty> && std::is_trivially_default_constructible_v<_Ty>)
        {
            if (_size < newSize)
                memset(data() + _size, 0, (newSize - _size) * sizeof(_Ty));
            _size = newSize;
        }
        else
        {
            while (_size < newSize)
                emplace_back();
            while (_size > newSize)
                pop_back();
        }
    }

    void swap(static_vector& other)
    {
        if constexpr (std::is_trivially_copyable_v<_Ty>)
        {
            const size_type minsz = std::min(_size, other._size);
            for (size_type i = 0; i < minsz; ++i)
                std::swap(data()[i], other.data()[i]);
            if (_size > other._size)
                ::memcpy(other.data() + minsz, data() + minsz, (_size - other._size) * sizeof(_Ty));
            else if (other._size > _size)
                ::memcpy(data() + minsz, other.data() + minsz, (other._size - _size) * sizeof(_Ty));
            std::swap(_size, other._size);
        }
        else
        {
            const size_type minsz = std::min(_size, other._size);
            for (size_type i = 0; i < minsz; ++i)
                std::swap((*this)[i], other[i]);
            if (_size > other._size)
            {
                for (size_type i = minsz; i < _size; ++i)
                    other.emplace_back(std::move((*this)[i]));
                for (size_type i = _size; i > other._size; --i)
                    pop_back();
            }
            else if (other._size > _size)
            {
                for (size_type i = minsz; i < other._size; ++i)
                    emplace_back(std::move(other[i]));
                for (size_type i = other._size; i > _size; --i)
                    other.pop_back();
            }
        }
    }

protected:
    pointer element_at(size_type idx) noexcept
    {
        return std::launder(reinterpret_cast<pointer>(_buffer + sizeof(_Ty) * idx));
    }

    const_pointer element_at(size_type idx) const noexcept
    {
        return std::launder(reinterpret_cast<const_pointer>(_buffer + sizeof(_Ty) * idx));
    }

    alignas(alignof(_Ty)) unsigned char _buffer[sizeof(_Ty) * _Capacity];
    size_type _size;
};

}  // namespace tlx
