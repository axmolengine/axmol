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
#include "yasio/tlx/memory.hpp"

namespace tlx
{
template <typename _Ty, size_t _Initial, typename _Alloc = std::allocator<_Ty>>
class inlined_vector
{
    using _Alty        = _TLX rebind_alloc_t<_Alloc, _Ty>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    struct _Vec_val
    {
        _Vec_val() { reset(); }

        void reset()
        {
            _Myfirst = reinterpret_cast<_Ty*>(_Mybuf);
            _Mylast  = _Myfirst;
            _Myend   = _Myfirst + _Initial;

            ::memset(_Mybuf, 0, sizeof(_Mybuf));
        }

        _Ty* inlined_data() { return reinterpret_cast<_Ty*>(_Mybuf); }

        bool is_inlined() const { return _Myfirst == reinterpret_cast<const _Ty*>(_Mybuf); }

        _Ty* _Myfirst;
        _Ty* _Mylast;
        _Ty* _Myend;

        alignas(_Ty) unsigned char _Mybuf[sizeof(_Ty) * _Initial];
    };

public:
    using value_type      = _Ty;
    using allocator_type  = _Alloc;
    using pointer         = typename _Alty_traits::pointer;
    using const_pointer   = typename _Alty_traits::const_pointer;
    using reference       = _Ty&;
    using const_reference = const _Ty&;
    using size_type       = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using iterator        = pointer;
    using const_iterator  = const_pointer;

    constexpr inlined_vector() noexcept : _Mypair(_TLX __zero_then_variadic_args_t{}) {}

    constexpr inlined_vector(const inlined_vector& other) : _Mypair(_TLX __zero_then_variadic_args_t{})
    {
        _Assign(other);
    }

    constexpr inlined_vector(inlined_vector&& other) : _Mypair(_TLX __zero_then_variadic_args_t{})
    {
        _Assign_rv(std::move(other));
    }

    constexpr inlined_vector(std::initializer_list<_Ty> ilist) : _Mypair(_TLX __zero_then_variadic_args_t{})
    {
        _Assign_ilist(ilist);
    }

    ~inlined_vector() noexcept
    {
        clear();
        _Tidy();
    }

    inlined_vector& operator=(const inlined_vector& other)
    {
        if (this != &other)
        {
            clear();
            _Assign(other);
        }
        return *this;
    }

    inlined_vector& operator=(inlined_vector&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            _Tidy();
            _Assign_rv(std::forward<inlined_vector>(other));
        }
        return *this;
    }

    // basic access
    constexpr bool empty() const noexcept
    {
        auto& _My_data = _Mypair._Myval2;
        return _My_data._Myfirst == _My_data._Mylast;
    }

    constexpr pointer data() noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "data() called on empty inlined_vector");
        return _My_data._Myfirst;
    }

    constexpr const_pointer data() const noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "data() called on empty inlined_vector");
        return _My_data._Myfirst;
    }

    constexpr size_type size() const noexcept
    {
        auto& _My_data = _Mypair._Myval2;
        return static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst);
    }

    constexpr size_type max_size() const noexcept
    {
        return (std::min)(static_cast<size_type>((std::numeric_limits<difference_type>::max)()),
                          _Alty_traits::max_size(_Getal()));
    }

    constexpr size_type capacity() const noexcept
    {
        auto& _My_data = _Mypair._Myval2;
        return static_cast<size_type>(_My_data._Myend - _My_data._Myfirst);
    }

    constexpr _Ty& operator[](const size_type _Pos) noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_Pos < static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst),
                    "vector subscript out of range");
        return _My_data._Myfirst[_Pos];
    }

    constexpr const _Ty& operator[](const size_type _Pos) const noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_Pos < static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst),
                    "vector subscript out of range");
        return _My_data._Myfirst[_Pos];
    }

    constexpr _Ty& at(const size_type _Pos)
    {
        auto& _My_data = _Mypair._Myval2;
        if (static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst) <= _Pos)
            _Xrange();

        return _My_data._Myfirst[_Pos];
    }

    constexpr const _Ty& at(const size_type _Pos) const
    {
        auto& _My_data = _Mypair._Myval2;
        if (static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst) <= _Pos)
            _Xrange();

        return _My_data._Myfirst[_Pos];
    }

    constexpr _Ty& front() noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "front() called on empty inlined_vector");
        return *_My_data._Myfirst;
    }

    constexpr const _Ty& front() const noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "front() called on empty inlined_vector");
        return *_My_data._Myfirst;
    }

    constexpr _Ty& back() noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "back() called on empty inlined_vector");
        return _My_data._Mylast[-1];
    }

    constexpr const _Ty& back() const noexcept /* strengthened */
    {
        auto& _My_data = _Mypair._Myval2;
        _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "back() called on empty inlined_vector");
        return _My_data._Mylast[-1];
    }

    iterator begin() noexcept { return _Mypair._Myval2._Myfirst; }
    iterator end() noexcept { return _Mypair._Myval2._Mylast; }
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cbegin() const noexcept { return _Mypair._Myval2._Myfirst; }
    const_iterator cend() const noexcept { return _Mypair._Myval2._Mylast; }

    // modifiers
    void clear() noexcept
    {
        auto& _My_data = _Mypair._Myval2;
        if constexpr (!std::is_trivially_destructible_v<_Ty>)
        {
            for (_Ty* p = _My_data._Mylast; p != _My_data._Myfirst;)
                tlx::invoke_dtor(--p);
        }
        _My_data._Mylast = _My_data._Myfirst;
    }

    void push_back(const _Ty& v) { emplace_back(v); }

    void push_back(_Ty&& v) { emplace_back(std::forward<_Ty>(v)); }

    template <typename... _Valty>
    _Ty& emplace_back(_Valty&&... vals)
    {
        _Resize(size() + 1, _TLX no_init);
        return *tlx::construct_at(_Mypair._Myval2._Mylast - 1, std::forward<_Valty>(vals)...);
    }

    constexpr void resize(size_t _Newsize) { _Resize(_Newsize, _TLX value_init); }
    constexpr void resize(size_t _Newsize, const value_type& _Val) { _Resize(_Newsize, _Val); }

    void pop_back()
    {
        auto& _My_data   = _Mypair._Myval2;
        pointer& _Mylast = _My_data._Mylast;

        _TLX_VERIFY(_My_data._Myfirst != _Mylast, "pop_back() called on empty vector");

        _Alty_traits::destroy(_Getal(), std::to_address(_Mylast - 1));
        --_Mylast;
    }

    void swap(inlined_vector& other)
    {
        if (this == std::addressof(other))
            return;

        auto& lhs = _Mypair._Myval2;
        auto& rhs = other._Mypair._Myval2;

        const bool a_inlined = lhs.is_inlined();
        const bool b_inlined = rhs.is_inlined();

        // ------------------------------------------------------------
        // Case 1: both vectors use heap storage
        // ------------------------------------------------------------
        if (!a_inlined && !b_inlined)
        {
            std::swap(lhs._Myfirst, rhs._Myfirst);
            std::swap(lhs._Mylast, rhs._Mylast);
            std::swap(lhs._Myend, rhs._Myend);
        }
        // ------------------------------------------------------------
        // Case 2: both vectors use inline storage
        // ------------------------------------------------------------
        else if (a_inlined && b_inlined)
        {
            const size_type as = static_cast<size_type>(lhs._Mylast - lhs._Myfirst);
            const size_type bs = static_cast<size_type>(rhs._Mylast - rhs._Myfirst);
            const size_type m  = (std::min)(as, bs);

            // 2.1 swap overlapping elements
            for (size_type i = 0; i < m; ++i)
                std::swap(lhs._Myfirst[i], rhs._Myfirst[i]);

            // 2.2 move remaining elements (lhs -> rhs)
            if (as > bs)
            {
                const size_type extra = as - bs;

                if constexpr (std::is_trivially_copyable_v<_Ty>)
                {
                    // For trivially copyable types, moving is equivalent to memcpy
                    ::memcpy(rhs._Myfirst + bs, lhs._Myfirst + bs, extra * sizeof(_Ty));
                }
                else
                {
                    size_type i = bs;
                    try
                    {
                        for (; i < as; ++i)
                            tlx::construct_at(rhs._Myfirst + i, std::move(lhs._Myfirst[i]));
                    }
                    catch (...)
                    {
                        // roll back constructed elements
                        if constexpr (!std::is_trivially_destructible_v<_Ty>)
                        {
                            for (size_type j = bs; j < i; ++j)
                                tlx::invoke_dtor(rhs._Myfirst + j);
                        }
                        throw;
                    }

                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type i2 = bs; i2 < as; ++i2)
                            tlx::invoke_dtor(lhs._Myfirst + i2);
                    }
                }

                rhs._Mylast = rhs._Myfirst + as;
                lhs._Mylast = lhs._Myfirst + bs;
            }
            // 2.3 move remaining elements (rhs -> lhs)
            else if (bs > as)
            {
                const size_type extra = bs - as;

                if constexpr (std::is_trivially_copyable_v<_Ty>)
                {
                    // For trivially copyable types, moving is equivalent to memcpy
                    ::memcpy(lhs._Myfirst + as, rhs._Myfirst + as, extra * sizeof(_Ty));
                }
                else
                {
                    size_type i = as;
                    try
                    {
                        for (; i < bs; ++i)
                            tlx::construct_at(lhs._Myfirst + i, std::move(rhs._Myfirst[i]));
                    }
                    catch (...)
                    {
                        if constexpr (!std::is_trivially_destructible_v<_Ty>)
                        {
                            for (size_type j = as; j < i; ++j)
                                tlx::invoke_dtor(lhs._Myfirst + j);
                        }
                        throw;
                    }

                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type i2 = as; i2 < bs; ++i2)
                            tlx::invoke_dtor(rhs._Myfirst + i2);
                    }
                }

                lhs._Mylast = lhs._Myfirst + bs;
                rhs._Mylast = rhs._Myfirst + as;
            }
        }
        // ------------------------------------------------------------
        // Case 3: one inline, one heap
        // ------------------------------------------------------------
        else
        {
            // Normalize roles:
            // inline_side = inline storage
            // heap_side   = heap storage
            _Vec_val* inline_side = a_inlined ? &lhs : &rhs;
            _Vec_val* heap_side   = a_inlined ? &rhs : &lhs;

            // Sizes on both sides
            const size_type heap_size   = static_cast<size_type>(heap_side->_Mylast - heap_side->_Myfirst);
            const size_type inline_size = static_cast<size_type>(inline_side->_Mylast - inline_side->_Myfirst);

            // Save heap_side's heap allocation
            _Ty* heap_first = heap_side->_Myfirst;
            _Ty* heap_last  = heap_side->_Mylast;
            _Ty* heap_end   = heap_side->_Myend;

            // 1) Move inline_side's inline elements into heap_side's inline buffer.
            _Ty* heap_inline_first = heap_side->inlined_data();  // address of heap_side's embedded inline buffer

            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                ::memcpy(heap_inline_first, inline_side->_Myfirst, inline_size * sizeof(_Ty));
            }
            else
            {
                size_type constructed = 0;
                try
                {
                    for (; constructed < inline_size; ++constructed)
                        tlx::construct_at(heap_inline_first + constructed,
                                          std::move(inline_side->_Myfirst[constructed]));
                }
                catch (...)
                {
                    // destroy any successfully constructed elements
                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type j = 0; j < constructed; ++j)
                            tlx::invoke_dtor(heap_inline_first + j);
                    }
                    throw;
                }

                // now destroy source inline elements
                if constexpr (!std::is_trivially_destructible_v<_Ty>)
                {
                    for (size_type i = 0; i < inline_size; ++i)
                        tlx::invoke_dtor(inline_side->_Myfirst + i);
                }
            }

            // 2) inline_side takes over heap_side's heap allocation and becomes heap-based
            inline_side->_Myfirst = heap_first;
            inline_side->_Mylast  = heap_first + heap_size;
            inline_side->_Myend   = heap_end;

            // 3) heap_side is reset to use its inline buffer
            heap_side->_Myfirst = heap_inline_first;
            heap_side->_Mylast  = heap_inline_first + inline_size;
            heap_side->_Myend   = heap_inline_first + _Initial;
        }

        // Swap allocators (allocator propagation rules are honored here)
        if constexpr (std::allocator_traits<_Alty>::propagate_on_container_swap::value)
            std::swap(_Mypair._Get_first(), other._Mypair._Get_first());
    }

private:
    // ------------------------------------------------------------
    // helper: move from other
    // ------------------------------------------------------------
    constexpr void _Assign_rv(inlined_vector&& other) { this->swap(other); }

    // ------------------------------------------------------------
    // helper: deep copy from other
    // ------------------------------------------------------------
    constexpr void _Assign(const inlined_vector& other)
    {
        auto& dst         = _Mypair._Myval2;
        const auto& src   = other._Mypair._Myval2;
        const size_type n = other.size();

        if (n <= _Initial)
        {
            // inline copy
            dst._Myfirst = reinterpret_cast<_Ty*>(dst._Mybuf);
            dst._Myend   = dst._Myfirst + _Initial;
            dst._Mylast  = dst._Myfirst;

            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                ::memcpy(dst._Myfirst, src._Myfirst, n * sizeof(_Ty));
            }
            else
            {
                size_type i = 0;
                try
                {
                    for (; i < n; ++i)
                        tlx::construct_at(dst._Myfirst + i, src._Myfirst[i]);
                }
                catch (...)
                {
                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type j = 0; j < i; ++j)
                            tlx::invoke_dtor(dst._Myfirst + j);
                    }
                    throw;
                }
            }
            dst._Mylast = dst._Myfirst + n;
        }
        else
        {
            // heap copy
            _Ty* new_first = _Getal().allocate(n);
            _Ty* new_last  = new_first;

            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                ::memcpy(new_first, src._Myfirst, n * sizeof(_Ty));
                new_last = new_first + n;
            }
            else
            {
                size_type i = 0;
                try
                {
                    for (; i < n; ++i, ++new_last)
                        tlx::construct_at(new_first + i, src._Myfirst[i]);
                }
                catch (...)
                {
                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type j = 0; j < i; ++j)
                            tlx::invoke_dtor(new_first + j);
                    }
                    _Getal().deallocate(new_first, n);
                    throw;
                }
            }

            dst._Myfirst = new_first;
            dst._Mylast  = new_last;
            dst._Myend   = new_first + n;
        }
    }

    // helper: deep copy from initializer_list
    constexpr void _Assign_ilist(std::initializer_list<_Ty> init)
    {
        auto& dst         = _Mypair._Myval2;
        const size_type n = init.size();

        if (n <= _Initial)
        {
            // inline copy
            dst._Myfirst = reinterpret_cast<_Ty*>(dst._Mybuf);
            dst._Myend   = dst._Myfirst + _Initial;
            dst._Mylast  = dst._Myfirst;

            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                ::memcpy(dst._Myfirst, init.begin(), n * sizeof(_Ty));
            }
            else
            {
                size_type i = 0;
                try
                {
                    for (auto it = init.begin(); it != init.end(); ++it, ++i)
                        tlx::construct_at(dst._Myfirst + i, *it);
                }
                catch (...)
                {
                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type j = 0; j < i; ++j)
                            tlx::invoke_dtor(dst._Myfirst + j);
                    }
                    throw;
                }
            }
            dst._Mylast = dst._Myfirst + n;
        }
        else
        {
            // heap copy
            _Ty* new_first = _Getal().allocate(n);
            _Ty* new_last  = new_first;

            if constexpr (std::is_trivially_copyable_v<_Ty>)
            {
                ::memcpy(new_first, init.begin(), n * sizeof(_Ty));
                new_last = new_first + n;
            }
            else
            {
                size_type i = 0;
                try
                {
                    for (auto it = init.begin(); it != init.end(); ++it, ++i, ++new_last)
                        tlx::construct_at(new_first + i, *it);
                }
                catch (...)
                {
                    if constexpr (!std::is_trivially_destructible_v<_Ty>)
                    {
                        for (size_type j = 0; j < i; ++j)
                            tlx::invoke_dtor(new_first + j);
                    }
                    _Getal().deallocate(new_first, n);
                    throw;
                }
            }

            dst._Myfirst = new_first;
            dst._Mylast  = new_last;
            dst._Myend   = new_first + n;
        }
    }

    template <typename _Ty2>
    constexpr void _Resize(const size_type _Newsize, const _Ty2& _Val)
    {
        if (_Newsize > max_size())
        {
            _Xlength();
        }

        auto& _Al          = _Getal();
        auto& _My_data     = _Mypair._Myval2;
        auto& _Myfirst     = _My_data._Myfirst;
        auto& _Mylast      = _My_data._Mylast;
        auto& _Myend       = _My_data._Myend;
        size_type _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);
        if (_Newsize < _Oldsize)
        {
            // trim
            const pointer _Newlast = _Myfirst + _Newsize;
            _TLX destroy_range(_Newlast, _Mylast, _Al);
            _Mylast = _Newlast;
            return;
        }

        if (_Newsize > _Oldsize)
        {
            const auto _Oldcap = static_cast<size_type>(_Myend - _Myfirst);
            size_type _Newcap  = _Newsize > _Oldcap ? _Calculate_growth(_Newsize) : _Oldcap;

            if (_Newcap > _Oldcap)
            {
                auto _Newvec = _Getal().allocate(_Newcap);

                // move or copy old elements into new storage
                if constexpr (!std::is_trivially_copyable_v<_Ty>)
                {
                    size_type i = 0;
                    try
                    {
                        for (; i < _Oldsize; ++i)
                            tlx::construct_at(_Newvec + i, std::move(_Myfirst[i]));
                    }
                    catch (...)
                    {
                        if constexpr (!std::is_trivially_destructible_v<_Ty>)
                        {
                            for (size_type j = 0; j < i; ++j)
                                tlx::invoke_dtor(_Newvec + j);
                        }
                        _Getal().deallocate(_Newvec, _Newcap);
                        throw;
                    }
                }
                else
                {
                    ::memcpy(_Newvec, _Myfirst, _Oldsize * sizeof(_Ty));
                }

                // destroy old elements in old storage
                if constexpr (!std::is_trivially_destructible_v<_Ty>)
                {
                    for (size_type i = 0; i < _Oldsize; ++i)
                        tlx::invoke_dtor(_Myfirst + i);
                }

                // free old storage if it was heap
                if (!_My_data.is_inlined())
                {
                    _Getal().deallocate(_Myfirst, _Oldcap);
                }

                // install new storage
                _Myfirst = _Newvec;
                _Mylast  = _Newvec + _Oldsize;
                _Myend   = _Newvec + _Newcap;
            }

            if constexpr (std::is_same_v<_Ty2, _Ty>)
            {  // Fill with user value: use memset for 1‑byte POD, construct otherwise
                _Mylast = _TLX uninitialized_fill_n(_Mylast, _Newsize - _Oldsize, _Val, _Al);
            }
            else if constexpr (std::is_same_v<_Ty2, _TLX value_init_t>)
            {  // Fill with value init: fast-pass
                _Mylast = _TLX uninitialized_value_construct_n(_Mylast, _Newsize - _Oldsize, _Al);
            }
            else
            {  // No fill: blazing fast
                _Mylast = _Myfirst + _Newsize;
            }
        }
        // if _Newsize == _Oldsize, do nothing
    }

    constexpr size_type _Calculate_growth(const size_type _Newsize) const
    {
        // given _Oldcapacity and _Newsize, calculate geometric growth
        const size_type _Oldcapacity = capacity();
        const auto _Max              = max_size();

        if (_Oldcapacity > _Max - _Oldcapacity / 2)
        {
            return _Max;  // geometric growth would overflow
        }

        const size_type _Geometric = _Oldcapacity + _Oldcapacity / 2;

        if (_Geometric < _Newsize)
        {
            return _Newsize;  // geometric growth would be insufficient
        }

        return _Geometric;  // geometric growth is sufficient
    }

    void _Tidy() noexcept
    {
        auto& _My_data = _Mypair._Myval2;
        if (!_My_data.is_inlined())
            _Getal().deallocate(_My_data._Myfirst, static_cast<size_type>(_My_data._Myend - _My_data._Myfirst));
        _My_data.reset();
    }

    static void _Xlength() { _TLX __xlength_error("vector too long"); }
    static void _Xrange() { _TLX __xout_of_range("invalid vector subscript"); }

    constexpr _Alty& _Getal() noexcept { return _Mypair._Get_first(); }
    constexpr const _Alty& _Getal() const noexcept { return _Mypair._Get_first(); }

    __compressed_pair<_Alty, _Vec_val> _Mypair;
};
}  // namespace tlx
