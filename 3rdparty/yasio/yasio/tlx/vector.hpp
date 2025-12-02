//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2025 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Version: 5.0.0
Inspired by Microsoft STL, tailored for dedicated use cases.

The vector aka array_buffer concepts:
   a. resize(void)/extend(void) doesn't fill default constructor (uninitialized for overwrite on POD path)
   b. Transparent iterator
   c. Extensions APIs:
      - operator+=
      - extend: extend size
      - resize_and_overwrite
      - detach_abi: release ownership
      - attach_abi: take owership
*/
#pragma once

#include <utility>
#include <memory>
#include <iterator>
#include <limits>
#include <algorithm>
#include "yasio/compiler/feature_test.hpp"
#include "yasio/tlx/type_traits.hpp"
#include "yasio/tlx/memory.hpp" // for compressed_pair and ::construct_at

namespace tlx
{

// Writable iterator for sequential containers
template <class _Myvec>
class sequence_const_iterator {
public:
  using iterator_concept  = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = typename _Myvec::value_type;
  using difference_type   = typename _Myvec::difference_type;
  using pointer           = typename _Myvec::const_pointer;
  using reference         = const value_type&;

  using _Tptr = typename _Myvec::pointer;

  constexpr sequence_const_iterator() noexcept : _Ptr() {}

  constexpr sequence_const_iterator(_Tptr _Parg) noexcept : _Ptr(_Parg) {}

  constexpr reference operator*() const noexcept { return *_Ptr; }

  constexpr pointer operator->() const noexcept { return _Ptr; }

  constexpr sequence_const_iterator& operator++() noexcept
  {
    ++_Ptr;
    return *this;
  }

  constexpr sequence_const_iterator operator++(int) noexcept
  {
    sequence_const_iterator _Tmp = *this;
    ++*this;
    return _Tmp;
  }

  constexpr sequence_const_iterator& operator--() noexcept
  {
    --_Ptr;
    return *this;
  }

  constexpr sequence_const_iterator operator--(int) noexcept
  {
    sequence_const_iterator _Tmp = *this;
    --*this;
    return _Tmp;
  }

  constexpr sequence_const_iterator& operator+=(const difference_type _Off) noexcept
  {
    _Ptr += _Off;
    return *this;
  }

  constexpr sequence_const_iterator operator+(const difference_type _Off) const noexcept
  {
    sequence_const_iterator _Tmp = *this;
    _Tmp += _Off;
    return _Tmp;
  }

  friend constexpr sequence_const_iterator operator+(const difference_type _Off, sequence_const_iterator _Next) noexcept
  {
    _Next += _Off;
    return _Next;
  }

  constexpr sequence_const_iterator& operator-=(const difference_type _Off) noexcept { return *this += -_Off; }

  constexpr sequence_const_iterator operator-(const difference_type _Off) const noexcept
  {
    sequence_const_iterator _Tmp = *this;
    _Tmp -= _Off;
    return _Tmp;
  }

  constexpr difference_type operator-(const sequence_const_iterator& _Right) const noexcept { return static_cast<difference_type>(_Ptr - _Right._Ptr); }

  constexpr reference operator[](const difference_type _Off) const noexcept { return *(*this + _Off); }

  constexpr bool operator==(const sequence_const_iterator& _Right) const noexcept { return _Ptr == _Right._Ptr; }

  bool operator!=(const sequence_const_iterator& _Right) const noexcept { return !(*this == _Right); }

  bool operator<(const sequence_const_iterator& _Right) const noexcept { return _Ptr < _Right._Ptr; }

  bool operator>(const sequence_const_iterator& _Right) const noexcept { return _Right < *this; }

  bool operator<=(const sequence_const_iterator& _Right) const noexcept { return !(_Right < *this); }

  bool operator>=(const sequence_const_iterator& _Right) const noexcept { return !(*this < _Right); }

  _Tptr _Ptr; // pointer to element in vector
};

template <class _Myvec>
class sequence_iterator : public sequence_const_iterator<_Myvec> {
public:
  using _Mybase = sequence_const_iterator<_Myvec>;

  using iterator_concept  = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = typename _Myvec::value_type;
  using difference_type   = typename _Myvec::difference_type;
  using pointer           = typename _Myvec::pointer;
  using reference         = value_type&;

  using _Mybase::_Mybase;

  constexpr reference operator*() const noexcept { return const_cast<reference>(_Mybase::operator*()); }

  constexpr pointer operator->() const noexcept { return this->_Ptr; }

  constexpr sequence_iterator& operator++() noexcept
  {
    _Mybase::operator++();
    return *this;
  }

  constexpr sequence_iterator operator++(int) noexcept
  {
    sequence_iterator _Tmp = *this;
    _Mybase::operator++();
    return _Tmp;
  }

  constexpr sequence_iterator& operator--() noexcept
  {
    _Mybase::operator--();
    return *this;
  }

  constexpr sequence_iterator operator--(int) noexcept
  {
    sequence_iterator _Tmp = *this;
    _Mybase::operator--();
    return _Tmp;
  }

  constexpr sequence_iterator& operator+=(const difference_type _Off) noexcept
  {
    _Mybase::operator+=(_Off);
    return *this;
  }

  constexpr sequence_iterator operator+(const difference_type _Off) const noexcept
  {
    sequence_iterator _Tmp = *this;
    _Tmp += _Off;
    return _Tmp;
  }

  friend constexpr sequence_iterator operator+(const difference_type _Off, sequence_iterator _Next) noexcept
  {
    _Next += _Off;
    return _Next;
  }

  constexpr sequence_iterator& operator-=(const difference_type _Off) noexcept
  {
    _Mybase::operator-=(_Off);
    return *this;
  }

  using _Mybase::operator-;

  constexpr sequence_iterator operator-(const difference_type _Off) const noexcept
  {
    sequence_iterator _Tmp = *this;
    _Tmp -= _Off;
    return _Tmp;
  }

  constexpr reference operator[](const difference_type _Off) const noexcept { return const_cast<reference>(_Mybase::operator[](_Off)); }
};

template <class _Value_type, class _Size_type, class _Difference_type, class _Pointer, class _Const_pointer>
struct _Vec_iter_types {
  using value_type      = _Value_type;
  using size_type       = _Size_type;
  using difference_type = _Difference_type;
  using pointer         = _Pointer;
  using const_pointer   = _Const_pointer;
};

template <typename _Val_types>
struct _Vector_val {
  using value_type      = typename _Val_types::value_type;
  using size_type       = typename _Val_types::size_type;
  using difference_type = typename _Val_types::difference_type;
  using pointer         = typename _Val_types::pointer;
  using const_pointer   = typename _Val_types::const_pointer;
  using reference       = value_type&;
  using const_reference = const value_type&;

  constexpr _Vector_val() noexcept : _Myfirst(), _Mylast(), _Myend() {}
  constexpr _Vector_val(pointer _First, pointer _Last, pointer _End) noexcept : _Myfirst(_First), _Mylast(_Last), _Myend(_End) {}

  constexpr void _Swap_val(_Vector_val& _Right) noexcept
  {
    std::swap(_Myfirst, _Right._Myfirst); // intentional ADL
    std::swap(_Mylast, _Right._Mylast);   // intentional ADL
    std::swap(_Myend, _Right._Myend);     // intentional ADL
  }

  constexpr void _Take_contents(_Vector_val& _Right) noexcept
  {
    _Myfirst = _Right._Myfirst;
    _Mylast  = _Right._Mylast;
    _Myend   = _Right._Myend;

    _Right._Myfirst = nullptr;
    _Right._Mylast  = nullptr;
    _Right._Myend   = nullptr;
  }

  pointer _Myfirst;
  pointer _Mylast;
  pointer _Myend;
};

template <class _Ty, class _Alloc = std::allocator<_Ty>, fill_policy _FillPolicy = fill_policy::always>
class vector { // varying size array of values
private:
  template <class>
  friend class _Vb_val;
  friend __tidy_guard<vector>;

  using _Alty        = _TLX rebind_alloc_t<_Alloc, _Ty>;
  using _Alty_traits = std::allocator_traits<_Alty>;

public:
  static constexpr bool allow_auto_fill = __allow_auto_fill_v<_Ty, _FillPolicy>;

  static_assert(std::is_object_v<_Ty>, "The C++ Standard forbids containers of non-object types "
                                       "because of [container.requirements].");

  using value_type      = _Ty;
  using allocator_type  = _Alloc;
  using pointer         = typename _Alty_traits::pointer;
  using const_pointer   = typename _Alty_traits::const_pointer;
  using reference       = _Ty&;
  using const_reference = const _Ty&;
  using size_type       = typename _Alty_traits::size_type;
  using difference_type = typename _Alty_traits::difference_type;

private:
  // _Vector_val<conditional_t<_Is_simple_alloc_v<_Alty>, _Simple_types<_Ty>, _Vec_iter_types<_Ty, size_type, difference_type, pointer, const_pointer>>>
  using _Scary_val = _Vector_val<_Vec_iter_types<_Ty, size_type, difference_type, pointer, const_pointer>>;

  struct _Reallocation_guard {
    _Alloc& _Al;
    pointer _New_begin;
    size_type _New_capacity;
    pointer _Constructed_first;
    pointer _Constructed_last;

    _Reallocation_guard& operator=(const _Reallocation_guard&) = delete;
    _Reallocation_guard& operator=(_Reallocation_guard&&)      = delete;

    constexpr ~_Reallocation_guard() noexcept
    {
      if (_New_begin != nullptr)
      {
        _TLX destroy_range(_Constructed_first, _Constructed_last, _Al);
        _Al.deallocate(_New_begin, _New_capacity);
      }
    }
  };

  struct _Simple_reallocation_guard {
    _Alloc& _Al;
    pointer _New_begin;
    size_type _New_capacity;

    _Simple_reallocation_guard& operator=(const _Simple_reallocation_guard&) = delete;
    _Simple_reallocation_guard& operator=(_Simple_reallocation_guard&&)      = delete;

    constexpr ~_Simple_reallocation_guard() noexcept
    {
      if (_New_begin != nullptr)
      {
        _Al.deallocate(_New_begin, _New_capacity);
      }
    }
  };

  struct _Vaporization_guard { // vaporize the detached piece
    vector* _Target;
    pointer _Vaporized_first;
    pointer _Vaporized_last;
    pointer _Destroyed_first;

    _Vaporization_guard& operator=(const _Vaporization_guard&) = delete;
    _Vaporization_guard& operator=(_Vaporization_guard&&)      = delete;

    ~_Vaporization_guard() noexcept
    {
      if (_Target != nullptr)
      {
        auto& _Al     = _Target->_Getal();
        auto& _Mylast = _Target->_Mypair._Myval2._Mylast;
        _TLX destroy_range(_Destroyed_first, _Mylast, _Al);
        _Mylast = _Vaporized_first;
      }
    }
  };

public:
  using iterator       = sequence_iterator<_Scary_val>;
  using const_iterator = sequence_const_iterator<_Scary_val>;

  constexpr vector() noexcept : _Mypair(_TLX __zero_then_variadic_args_t{}) {}

  constexpr explicit vector(const _Alloc& _Al) noexcept : _Mypair(_TLX __one_then_variadic_args_t{}, _Al) {}

  constexpr explicit vector(const size_type _Count, const _Alloc& _Al = _Alloc()) : _Mypair(_TLX __one_then_variadic_args_t{}, _Al) { _Construct_n(_Count); }

  constexpr vector(const size_type _Count, const _Ty& _Val, const _Alloc& _Al = _Alloc()) : _Mypair(_TLX __one_then_variadic_args_t{}, _Al)
  {
    _Construct_n(_Count, _Val);
  }

  template <class _Iter, enable_if_t<is_iterator<_Iter>::value, int> = 0>
  constexpr vector(_Iter _First, _Iter _Last, const _Alloc& _Al = _Alloc()) : _Mypair(_TLX __one_then_variadic_args_t{}, _Al)
  {
    auto _UFirst = _First;
    auto _ULast  = _Last;

    // since we only care about pointer-like iterators, we can always compute length
    const auto _Length = static_cast<size_t>(_ULast - _UFirst);
    const auto _Count  = static_cast<size_type>(_Length);

    _Construct_n(_Count, _UFirst, _ULast);
  }

  constexpr vector(std::initializer_list<_Ty> _Ilist, const _Alloc& _Al = _Alloc()) : _Mypair(_TLX __one_then_variadic_args_t{}, _Al)
  {
    _Construct_n(static_cast<size_type>(_Ilist.size()), _Ilist.begin(), _Ilist.end());
  }

  constexpr vector(const vector& _Right) : _Mypair(_TLX __one_then_variadic_args_t{}, _Alty_traits::select_on_container_copy_construction(_Right._Getal()))
  {
    const auto& _Right_data = _Right._Mypair._Myval2;
    const auto _Count       = static_cast<size_type>(_Right_data._Mylast - _Right_data._Myfirst);
    _Construct_n(_Count, _Right_data._Myfirst, _Right_data._Mylast);
  }

  constexpr vector(const vector& _Right, const _TLX identity_t<_Alloc>& _Al) : _Mypair(_TLX __one_then_variadic_args_t{}, _Al)
  {
    const auto& _Right_data = _Right._Mypair._Myval2;
    const auto _Count       = static_cast<size_type>(_Right_data._Mylast - _Right_data._Myfirst);
    _Construct_n(_Count, _Right_data._Myfirst, _Right_data._Mylast);
  }

  constexpr vector(vector&& _Right) noexcept
      : _Mypair(_TLX __one_then_variadic_args_t{}, std::move(_Right._Getal()), std::exchange(_Right._Mypair._Myval2._Myfirst, nullptr),
                std::exchange(_Right._Mypair._Myval2._Mylast, nullptr), std::exchange(_Right._Mypair._Myval2._Myend, nullptr))
  {}

  constexpr vector(vector&& _Right, const _TLX identity_t<_Alloc>& _Al_) noexcept(std::allocator_traits<_Alloc>::is_always_equal::value &&
                                                                                  std::is_nothrow_move_constructible_v<_Ty>)
      : _Mypair(_TLX __one_then_variadic_args_t{}, _Al_)
  {
    _Alty& _Al        = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    auto& _Right_data = _Right._Mypair._Myval2;

    if constexpr (!_Alty_traits::is_always_equal::value)
    {
      if (_Al != _Right._Getal())
      {
        const auto _Count = static_cast<size_type>(_Right_data._Mylast - _Right_data._Myfirst);
        if (_Count != 0)
        {
          _Buy_raw(_Count);
          _My_data._Mylast = uninitialized_move(_Right_data._Myfirst, _Right_data._Mylast, _My_data._Myfirst, _Al);
        }
        return;
      }
    }

    _My_data._Take_contents(_Right_data);
  }

  constexpr vector& operator=(vector&& _Right) noexcept
  {
    if (this == std::addressof(_Right))
    {
      return *this;
    }

    _Tidy();
    _Mypair._Myval2._Take_contents(_Right._Mypair._Myval2);
    return *this;
  }

  constexpr ~vector() noexcept { _Tidy(); }

private:
  template <class... _Valty>
  constexpr _Ty& _Emplace_one_at_back(_Valty&&... _Val)
  {
    // insert by perfectly forwarding into element at end, provide strong guarantee
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    if (_Mylast != _My_data._Myend)
    {
      return _Emplace_back_with_unused_capacity(std::forward<_Valty>(_Val)...);
    }

    return *_Emplace_reallocate(_Mylast, std::forward<_Valty>(_Val)...);
  }

  template <class... Valty>
  constexpr _Ty& _Emplace_back_with_unused_capacity(Valty&&... val)
  {
    auto& data    = _Mypair._Myval2;
    pointer& last = data._Mylast;
    _TLX_INTERNAL_CHECK(last != data._Myend);

    if constexpr (std::is_nothrow_constructible_v<_Ty, Valty...>)
    {
      tlx::construct_at(last, std::forward<Valty>(val)...);
    }
    else
    {
      _Alty_traits::construct(_Getal(), std::to_address(last), std::forward<Valty>(val)...);
    }

    _Ty& result = *last;
    ++last;
    return result;
  }

  template <class... _Valty>
  constexpr pointer _Emplace_reallocate(const pointer _Whereptr, _Valty&&... _Val)
  {
    // reallocate and insert by perfectly forwarding _Val at _Whereptr
    _Alty& _Al        = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    _TLX_INTERNAL_CHECK(_Mylast == _My_data._Myend); // check that we have no unused capacity

    const auto _Whereoff = static_cast<size_type>(_Whereptr - _Myfirst);
    const auto _Oldsize  = static_cast<size_type>(_Mylast - _Myfirst);

    if (_Oldsize == max_size())
    {
      _Xlength();
    }

    const size_type _Newsize = _Oldsize + 1;
    size_type _Newcapacity   = _Calculate_growth(_Newsize);

    const pointer _Newvec           = _Al.allocate(_Newcapacity);
    const pointer _Constructed_last = _Newvec + _Whereoff + 1;

    _Reallocation_guard _Guard{_Al, _Newvec, _Newcapacity, _Constructed_last, _Constructed_last};
    auto& _Constructed_first = _Guard._Constructed_first;

    _Alty_traits::construct(_Al, _Newvec + _Whereoff, std::forward<_Valty>(_Val)...);
    _Constructed_first = _Newvec + _Whereoff;

    if (_Whereptr == _Mylast)
    { // at back, provide strong guarantee
      if constexpr (std::is_nothrow_move_constructible_v<_Ty> || !std::is_copy_constructible_v<_Ty>)
      {
        _TLX uninitialized_move(_Myfirst, _Mylast, _Newvec, _Al);
      }
      else
      {
        _TLX uninitialized_copy(_Myfirst, _Mylast, _Newvec, _Al);
      }
    }
    else
    { // provide basic guarantee
      _TLX uninitialized_move(_Myfirst, _Whereptr, _Newvec, _Al);
      _Constructed_first = _Newvec;
      _TLX uninitialized_move(_Whereptr, _Mylast, _Newvec + _Whereoff + 1, _Al);
    }

    _Guard._New_begin = nullptr;
    _Change_array(_Newvec, _Newsize, _Newcapacity);
    return _Newvec + _Whereoff;
  }

public:
  template <class... _Valty>
  constexpr _Ty& emplace_back(_Valty&&... _Val)
  {
    // insert by perfectly forwarding into element at end, provide strong guarantee
    return _Emplace_one_at_back(std::forward<_Valty>(_Val)...);
  }

  constexpr void push_back(const _Ty& _Val)
  { // insert element at end, provide strong guarantee
    _Emplace_one_at_back(_Val);
  }

  constexpr void push_back(_Ty&& _Val)
  {
    // insert by moving into element at end, provide strong guarantee
    _Emplace_one_at_back(std::move(_Val));
  }

#pragma region extension APIs

  constexpr size_type size_bytes() const { return size() * sizeof(value_type); }

  constexpr vector& operator+=(const value_type& val)
  {
    push_back(val);
    return *this;
  }

  template <class _U = value_type, typename = std::enable_if_t<std::is_pointer<_U>::value>>
  constexpr void resize(const size_type _Newsize, std::nullptr_t)
  {
    _Resize(_Newsize, _TLX value_init);
  }

  constexpr void resize(const size_type _Newsize, value_init_t)
  {
    // trim or append value-initialized elements, provide strong guarantee
    _Resize(_Newsize, _TLX value_init);
  }

  template <typename _Cont>
  constexpr vector& operator+=(const _Cont& rhs)
  {
    return this->extend(std::begin(rhs), std::end(rhs));
  }

  template <typename _Iter>
  constexpr vector& extend(_Iter first, _Iter last)
  {
    insert(end(), first, last);
    return *this;
  }

  constexpr vector& extend(size_type count)
  {
    resize(size() + count);
    return *this;
  }

  constexpr vector& extend(size_type count, const value_type& val)
  {
    resize(size() + count, val);
    return *this;
  }

  template <typename _Operation>
  void resize_and_overwrite(const size_type new_size, _Operation op)
  {
    _Reallocate<_Reallocation_policy::_Exactly>(new_size);

    auto& _My_data   = _Mypair._Myval2;
    _My_data._Mylast = _My_data._Myfirst + (std::move(op)(_My_data._Myfirst, new_size));
  }

  pointer detach_abi() noexcept
  {
    auto& _My_data    = _Mypair._Myval2;
    pointer p         = _My_data._Myfirst;
    _My_data._Myfirst = nullptr;
    _My_data._Mylast  = nullptr;
    _My_data._Myend   = nullptr;
    return p;
  }

  void attach_abi(pointer ptr, size_type len, size_type capacity = (size_type)-1)
  {
    _Tidy();
    auto& _My_data    = _Mypair._Myval2;
    _My_data._Myfirst = ptr;
    _My_data._Mylast  = ptr + len;
    _My_data._Myend   = ptr + (capacity != (size_type)-1 ? capacity : len);
  }

#pragma endregion

public:
  template <class... _Valty>
  constexpr iterator emplace(const_iterator _Where, _Valty&&... _Val)
  {
    auto _Whereptr = _Where._Ptr;
    auto& _My_data = _Mypair._Myval2;
    auto _Oldlast  = _My_data._Mylast;
    _TLX_VERIFY(_Whereptr >= _My_data._Myfirst && _Oldlast >= _Whereptr, "vector emplace iterator outside range");
    if (_Oldlast != _My_data._Myend)
    {
      if (_Whereptr == _Oldlast)
      { // at back, strong guarantee
        _Emplace_back_with_unused_capacity(std::forward<_Valty>(_Val)...);
      }
      else
      {
        auto& _Al = _Getal();
        // construct a copy of the last element at the new end
        _Alty_traits::construct(_Al, std::to_address(_Oldlast), std::move(_Oldlast[-1]));
        ++_My_data._Mylast;

        // shift elements one position to the right
        _TLX move_backward_unchecked(_Whereptr, _Oldlast - 1, _Oldlast);

        // construct new element at the insertion position
        _Alty_traits::construct(_Al, std::to_address(_Whereptr), std::forward<_Valty>(_Val)...);
      }

      return iterator(_Whereptr);
    }

    // no capacity left, reallocate and emplace
    return iterator(_Emplace_reallocate(_Whereptr, std::forward<_Valty>(_Val)...));
  }

  constexpr iterator insert(const_iterator _Where, const _Ty& _Val)
  { // insert _Val at _Where
    return emplace(_Where, _Val);
  }

  constexpr iterator insert(const_iterator _Where, _Ty&& _Val)
  { // insert by moving _Val at _Where
    return emplace(_Where, std::move(_Val));
  }

  template <class... _Valty>
  constexpr iterator insert(const_iterator _Where, const size_type _Count, const _Ty& _Val)
  {
    // insert _Count copies of _Val at _Where
    const pointer _Whereptr = _Where._Ptr;

    auto& _Al        = _Getal();
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    const pointer _Oldfirst = _My_data._Myfirst;
    const pointer _Oldlast  = _Mylast;

    _TLX_VERIFY(_Whereptr >= _Oldfirst && _Oldlast >= _Whereptr, "vector insert iterator outside range");

    const auto _Whereoff        = static_cast<size_type>(_Whereptr - _Oldfirst);
    const auto _Unused_capacity = static_cast<size_type>(_My_data._Myend - _Oldlast);
    const bool _One_at_back     = _Count == 1 && _Whereptr == _Oldlast;

    if (_Count == 0)
    { // nothing to do, avoid invalidating iterators
    }
    else if (_Count > _Unused_capacity)
    { // reallocate
      const auto _Oldsize = static_cast<size_type>(_Oldlast - _Oldfirst);

      if (_Count > max_size() - _Oldsize)
      {
        _Xlength();
      }

      const size_type _Newsize = _Oldsize + _Count;
      size_type _Newcapacity   = _Calculate_growth(_Newsize);

      const pointer _Newvec           = _Al.allocate(_Newcapacity);
      const pointer _Constructed_last = _Newvec + _Whereoff + _Count;

      _Reallocation_guard _Guard{_Al, _Newvec, _Newcapacity, _Constructed_last, _Constructed_last};
      auto& _Constructed_first = _Guard._Constructed_first;

      _TLX uninitialized_fill_n(_Newvec + _Whereoff, _Count, _Val, _Al);
      _Constructed_first = _Newvec + _Whereoff;

      if (_One_at_back)
      { // strong guarantee
        if constexpr (std::is_nothrow_move_constructible_v<_Ty> || !std::is_copy_constructible_v<_Ty>)
        {
          _TLX uninitialized_move(_Oldfirst, _Oldlast, _Newvec, _Al);
        }
        else
        {
          _TLX uninitialized_copy(_Oldfirst, _Oldlast, _Newvec, _Al);
        }
      }
      else
      { // basic guarantee
        _TLX uninitialized_move(_Oldfirst, _Whereptr, _Newvec, _Al);
        _Constructed_first = _Newvec;
        _TLX uninitialized_move(_Whereptr, _Oldlast, _Newvec + _Whereoff + _Count, _Al);
      }

      _Guard._New_begin = nullptr;
      _Change_array(_Newvec, _Newsize, _Newcapacity);
    }
    else if (_One_at_back)
    { // strong guarantee
      _Emplace_back_with_unused_capacity(_Val);
    }
    else
    { // basic guarantee
      const auto _Affected_elements = static_cast<size_type>(_Oldlast - _Whereptr);

      if (_Count > _Affected_elements)
      { // new stuff spills off end
        _Mylast = _TLX uninitialized_fill_n(_Oldlast, _Count - _Affected_elements, _Val, _Al);
        _Mylast = _TLX uninitialized_move(_Whereptr, _Oldlast, _Mylast, _Al);
        std::fill(_Whereptr, _Oldlast, _Val);
      }
      else
      { // new stuff can all be assigned
        _Mylast = _TLX uninitialized_move(_Oldlast - _Count, _Oldlast, _Oldlast, _Al);
        _TLX move_backward_unchecked(_Whereptr, _Oldlast - _Count, _Oldlast);
        std::fill_n(_Whereptr, _Count, _Val);
      }
    }

    return iterator(_My_data._Myfirst + _Whereoff);
  }

private:
  template <class _Iter>
  constexpr void _Insert_counted_range(const_iterator _Where, _Iter _First, const size_type _Count)
  {
    // insert counted range _First + [0, _Count) at _Where
    auto _Whereptr = _Where._Ptr;

    auto& _Al        = _Getal();
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    const pointer _Oldfirst     = _My_data._Myfirst;
    const pointer _Oldlast      = _Mylast;
    const auto _Unused_capacity = static_cast<size_type>(_My_data._Myend - _Oldlast);

    if (_Count == 0)
    { // nothing to do, avoid invalidating iterators
    }
    else if (_Count > _Unused_capacity)
    { // reallocate
      const auto _Oldsize = static_cast<size_type>(_Oldlast - _Oldfirst);

      if (_Count > max_size() - _Oldsize)
      {
        _Xlength();
      }

      const size_type _Newsize = _Oldsize + _Count;
      size_type _Newcapacity   = _Calculate_growth(_Newsize);

      const pointer _Newvec           = _Al.allocate(_Newcapacity);
      const auto _Whereoff            = static_cast<size_type>(_Whereptr - _Oldfirst);
      const pointer _Constructed_last = _Newvec + _Whereoff + _Count;

      _Reallocation_guard _Guard{_Al, _Newvec, _Newcapacity, _Constructed_last, _Constructed_last};
      auto& _Constructed_first = _Guard._Constructed_first;

      _TLX uninitialized_copy_n(std::move(_First), _Count, _Newvec + _Whereoff, _Al);
      _Constructed_first = _Newvec + _Whereoff;

      if (_Count == 1 && _Whereptr == _Oldlast)
      { // one at back, provide strong guarantee
        if constexpr (std::is_nothrow_move_constructible_v<_Ty> || !std::is_copy_constructible_v<_Ty>)
        {
          _TLX uninitialized_move(_Oldfirst, _Oldlast, _Newvec, _Al);
        }
        else
        {
          _TLX uninitialized_copy(_Oldfirst, _Oldlast, _Newvec, _Al);
        }
      }
      else
      { // provide basic guarantee
        _TLX uninitialized_move(_Oldfirst, _Whereptr, _Newvec, _Al);
        _Constructed_first = _Newvec;
        _TLX uninitialized_move(_Whereptr, _Oldlast, _Newvec + _Whereoff + _Count, _Al);
      }

      _Guard._New_begin = nullptr;
      _Change_array(_Newvec, _Newsize, _Newcapacity);
    }
    else
    { // Attempt to provide the strong guarantee for EmplaceConstructible failure.
      // If we encounter copy/move construction/assignment failure, provide the basic guarantee.
      // (For one-at-back, this provides the strong guarantee.)

      const auto _Affected_elements = static_cast<size_type>(_Oldlast - _Whereptr);

      if (_Count < _Affected_elements)
      { // some affected elements must be assigned
        _Mylast = _TLX uninitialized_move(_Oldlast - _Count, _Oldlast, _Oldlast, _Al);
        _TLX move_backward_unchecked(_Whereptr, _Oldlast - _Count, _Oldlast);
        _TLX destroy_range(_Whereptr, _Whereptr + _Count, _Al);

        try
        {
          _TLX uninitialized_copy_n(std::move(_First), _Count, _Whereptr, _Al);
        }
        catch (...)
        {
          // glue the broken pieces back together

          _Vaporization_guard _Guard{this, _Whereptr, _Oldlast, _Whereptr + _Count};
          _TLX uninitialized_move(_Whereptr + _Count, _Whereptr + 2 * _Count, _Whereptr, _Al);
          _Guard._Target = nullptr;

          _TLX move_unchecked(_Whereptr + 2 * _Count, _Mylast, _Whereptr + _Count);
          _TLX destroy_range(_Oldlast, _Mylast, _Al);
          _Mylast = _Oldlast;
          throw;
        }
      }
      else
      { // affected elements don't overlap before/after
        const pointer _Relocated = _Whereptr + _Count;
        _Mylast                  = _TLX uninitialized_move(_Whereptr, _Oldlast, _Relocated, _Al);
        _TLX destroy_range(_Whereptr, _Oldlast, _Al);
        try
        {
          _TLX uninitialized_copy_n(std::move(_First), _Count, _Whereptr, _Al);
          // glue the broken pieces back together
        }
        catch (...)
        {
          _Vaporization_guard _Guard{this, _Whereptr, _Oldlast, _Relocated};
          _TLX uninitialized_move(_Relocated, _Mylast, _Whereptr, _Al);
          _Guard._Target = nullptr;

          _TLX destroy_range(_Relocated, _Mylast, _Al);
          _Mylast = _Oldlast;
          throw;
        }
      }
    }
  }

public:
  template <class _Iter, enable_if_t<tlx::is_iterator_v<_Iter>, int> = 0>
  constexpr iterator insert(const_iterator _Where, _Iter _First, _Iter _Last)
  {
    auto _Whereptr          = _Where._Ptr;
    auto& _My_data          = _Mypair._Myval2;
    const pointer _Oldfirst = _My_data._Myfirst;

    const auto _Whereoff = static_cast<size_type>(_Whereptr - _Oldfirst);
    const auto _Count    = static_cast<size_type>(_Last - _First);

    _Insert_counted_range(_Where, _First, _Count);

    return iterator(_My_data._Myfirst + _Whereoff);
  }

  constexpr iterator insert(const_iterator _Where, std::initializer_list<_Ty> _Ilist)
  {
    const pointer _Whereptr = _Where._Ptr;
    auto& _My_data          = _Mypair._Myval2;
    const pointer _Oldfirst = _My_data._Myfirst;
    const auto _Whereoff    = static_cast<size_type>(_Whereptr - _Oldfirst);

    const auto _Count = static_cast<size_type>(_Ilist.size());
    _Insert_counted_range(_Where, _Ilist.begin(), _Count);
    return iterator(_My_data._Myfirst + _Whereoff);
  }

  constexpr void assign(const size_type _Newsize, const _Ty& _Val)
  {
    // assign _Newsize * _Val
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    const auto _Oldcapacity = static_cast<size_type>(_My_data._Myend - _Myfirst);
    if (_Newsize > _Oldcapacity)
    { // reallocate
      _Clear_and_reserve_geometric(_Newsize);
      _Mylast = _TLX uninitialized_fill_n(_Myfirst, _Newsize, _Val, _Al);
      return;
    }

    const auto _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);
    if (_Newsize > _Oldsize)
    {
      std::fill(_Myfirst, _Mylast, _Val);
      _Mylast = _TLX uninitialized_fill_n(_Mylast, _Newsize - _Oldsize, _Val, _Al);
    }
    else
    {
      const pointer _Newlast = _Myfirst + _Newsize;
      std::fill(_Myfirst, _Newlast, _Val);
      _TLX destroy_range(_Newlast, _Mylast, _Al);
      _Mylast = _Newlast;
    }
  }

private:
  template <class _Iter>
  constexpr void _Assign_counted_range(_Iter _First, const size_type _Newsize)
  {
    // assign elements from counted range _First + [0, _Newsize)
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;

    const auto _Oldcapacity = static_cast<size_type>(_Myend - _Myfirst);
    if (_Newsize > _Oldcapacity)
    {
      _Clear_and_reserve_geometric(_Newsize);
      _Mylast = _TLX uninitialized_copy_n(std::move(_First), _Newsize, _Myfirst, _Al);
      return;
    }

    const auto _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);
    if (_Newsize > _Oldsize)
    {
      bool _Copied = false;
      if constexpr (_TLX bitcopy_assignable_v<_Iter, pointer>)
      {
        if (!std::is_constant_evaluated())
        {
          copy_memmove_n(_First, static_cast<size_t>(_Oldsize), _Myfirst);
          _First += _Oldsize;
          _Copied = true;
        }
      }

      if (!_Copied)
      {
        for (auto _Mid = _Myfirst; _Mid != _Mylast; ++_Mid, (void)++_First)
        {
          *_Mid = *_First;
        }
      }

      _Mylast = _TLX uninitialized_copy_n(std::move(_First), _Newsize - _Oldsize, _Mylast, _Al);
    }
    else
    {
      const pointer _Newlast = _Myfirst + _Newsize;
      _TLX copy_n_unchecked4(std::move(_First), _Newsize, _Myfirst);
      _TLX destroy_range(_Newlast, _Mylast, _Al);
      _Mylast = _Newlast;
    }
  }

public:
  template <class _Iter, enable_if_t<std::is_pointer_v<_Iter>, int> = 0>
  constexpr void assign(_Iter _First, _Iter _Last)
  {
    const auto _Length = static_cast<size_t>(_Last - _First); // pointer difference
    const auto _Count  = static_cast<size_type>(_Length);

    _Assign_counted_range(_First, _Count);
  }

  constexpr void assign(const std::initializer_list<_Ty> _Ilist)
  {
    const auto _Count = static_cast<size_type>(_Ilist.size());
    _Assign_counted_range(_Ilist.begin(), _Count);
  }

  constexpr vector& operator=(const vector& _Right)
  {
    if (this == std::addressof(_Right))
    {
      return *this; // self-assignment, nothing to do
    }

    _Tidy(); // clear current contents

    auto& _Right_data      = _Right._Mypair._Myval2;
    const size_type _Count = static_cast<size_type>(_Right_data._Mylast - _Right_data._Myfirst);

    _Assign_counted_range(_Right_data._Myfirst, _Count); // copy elements from _Right

    return *this;
  }

  constexpr vector& operator=(std::initializer_list<_Ty> _Ilist)
  {
    const auto _Count = static_cast<size_type>(_Ilist.size());
    _Assign_counted_range(_Ilist.begin(), _Count);
    return *this;
  }

private:
  template <class _Ty2>
  constexpr void _Resize_reallocate(const size_type _Newsize, const _Ty2& _Val)
  {
    if (_Newsize > max_size())
    {
      _Xlength();
    }

    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    const auto _Oldsize    = static_cast<size_type>(_Mylast - _Myfirst);
    size_type _Newcapacity = _Calculate_growth(_Newsize);

    const pointer _Newvec         = _Al.allocate(_Newcapacity);
    const pointer _Appended_first = _Newvec + _Oldsize;

    _Reallocation_guard _Guard{_Al, _Newvec, _Newcapacity, _Appended_first, _Appended_first};
    auto& _Appended_last = _Guard._Constructed_last;

    if constexpr (std::is_same_v<_Ty2, _Ty>)
    { // Fill with user value: use memset for 1‑byte POD, construct otherwise
      _Appended_last = _TLX uninitialized_fill_n(_Appended_first, _Newsize - _Oldsize, _Val, _Al);
    }
    else
    { // Fill with value init: fast-pass
      if constexpr ((std::is_same_v<_Ty2, __auto_value_init_t> && allow_auto_fill) || std::is_same_v<_Ty2, value_init_t>)
        _Appended_last = _TLX uninitialized_value_construct_n(_Appended_first, _Newsize - _Oldsize, _Al);
      else // No fill: blazing fast
        _Appended_last = _Appended_first + (_Newsize - _Oldsize);
    }

    if constexpr (std::is_nothrow_move_constructible_v<_Ty> || !std::is_copy_constructible_v<_Ty>)
    {
      _TLX uninitialized_move(_Myfirst, _Mylast, _Newvec, _Al);
    }
    else
    {
      _TLX uninitialized_copy(_Myfirst, _Mylast, _Newvec, _Al);
    }

    _Guard._New_begin = nullptr;
    _Change_array(_Newvec, _Newsize, _Newcapacity);
  }

  template <class _Ty2>
  constexpr void _Resize(const size_type _Newsize, const _Ty2& _Val)
  {
    // trim or append elements, provide strong guarantee
    auto& _Al           = _Getal();
    auto& _My_data      = _Mypair._Myval2;
    pointer& _Myfirst   = _My_data._Myfirst;
    pointer& _Mylast    = _My_data._Mylast;
    const auto _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);
    if (_Newsize < _Oldsize)
    { // trim
      const pointer _Newlast = _Myfirst + _Newsize;
      _TLX destroy_range(_Newlast, _Mylast, _Al);
      _Mylast = _Newlast;
      return;
    }

    if (_Newsize > _Oldsize)
    { // append
      const auto _Oldcapacity = static_cast<size_type>(_My_data._Myend - _Myfirst);
      if (_Newsize > _Oldcapacity)
      { // reallocate
        _Resize_reallocate(_Newsize, _Val);
        return;
      }

      if constexpr (std::is_same_v<_Ty2, _Ty>)
      { // Fill with user value: use memset for 1‑byte POD, construct otherwise
        _Mylast = _TLX uninitialized_fill_n(_Mylast, _Newsize - _Oldsize, _Val, _Al);
      }
      else
      {
        // Fill with value init: fast-pass
        if constexpr ((std::is_same_v<_Ty2, __auto_value_init_t> && allow_auto_fill) || std::is_same_v<_Ty2, value_init_t>)
          _Mylast = _TLX uninitialized_value_construct_n(_Mylast, _Newsize - _Oldsize, _Al);
        else // No fill: blazing fast
          _Mylast = _Myfirst + _Newsize;
      }
    }
    // if _Newsize == _Oldsize, do nothing; avoid invalidating iterators
  }

public:
  constexpr void resize(const size_type _Newsize)
  {
    // trim or append value-initialized elements, provide strong guarantee
    _Resize(_Newsize, __auto_value_init_t{});
  }

  constexpr void resize(const size_type _Newsize, const _Ty& _Val)
  {
    // trim or append copies of _Val, provide strong guarantee
    _Resize(_Newsize, _Val);
  }

private:
  enum class _Reallocation_policy
  {
    _At_least,
    _Exactly
  };

  template <_Reallocation_policy _Policy>
  constexpr void _Reallocate(const size_type& _Newcapacity)
  {
    // set capacity to _Newcapacity (without geometric growth), provide strong guarantee
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    const auto _Size = static_cast<size_type>(_Mylast - _Myfirst);

    pointer _Newvec;
    if constexpr (_Policy == _Reallocation_policy::_At_least)
    {
      _Newvec = _Al.allocate(_Newcapacity);
    }
    else
    {
      _Newvec = _Al.allocate(_Newcapacity);
    }

    _Simple_reallocation_guard _Guard{_Al, _Newvec, _Newcapacity};

    if constexpr (std::is_nothrow_move_constructible_v<_Ty> || !std::is_copy_constructible_v<_Ty>)
    {
      uninitialized_move(_Myfirst, _Mylast, _Newvec, _Al);
    }
    else
    {
      uninitialized_copy(_Myfirst, _Mylast, _Newvec, _Al);
    }

    _Guard._New_begin = nullptr;
    _Change_array(_Newvec, _Size, _Newcapacity);
  }

  constexpr void _Clear_and_reserve_geometric(const size_type _Newsize)
  {
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;

    if (_Newsize > max_size())
    {
      _Xlength();
    }

    const size_type _Newcapacity = _Calculate_growth(_Newsize);

    if (_Myfirst)
    { // destroy and deallocate old array
      _TLX destroy_range(_Myfirst, _Mylast, _Al);
      _Al.deallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst));

      _Myfirst = nullptr;
      _Mylast  = nullptr;
      _Myend   = nullptr;
    }

    _Buy_raw(_Newcapacity);
  }

public:
  constexpr void reserve(size_type _Newcapacity)
  {
    // increase capacity to _Newcapacity (without geometric growth), provide strong guarantee
    if (_Newcapacity > capacity())
    { // something to do (reserve() never shrinks)
      if (_Newcapacity > max_size())
      {
        _Xlength();
      }

      _Reallocate<_Reallocation_policy::_At_least>(_Newcapacity);
    }
  }

  constexpr void shrink_to_fit()
  { // reduce capacity to size, provide strong guarantee
    auto& _My_data         = _Mypair._Myval2;
    const pointer _Oldlast = _My_data._Mylast;
    if (_Oldlast != _My_data._Myend)
    { // something to do
      const pointer _Oldfirst = _My_data._Myfirst;
      if (_Oldfirst == _Oldlast)
      {
        _Tidy();
      }
      else
      {
        size_type _Newcapacity = static_cast<size_type>(_Oldlast - _Oldfirst);
        _Reallocate<_Reallocation_policy::_Exactly>(_Newcapacity);
      }
    }
  }

  constexpr void pop_back() noexcept /* strengthened */
  {
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    _Alty_traits::destroy(_Getal(), std::to_address(_Mylast - 1));
    --_Mylast;
  }

  constexpr iterator erase(const_iterator _Where) noexcept /* strengthened */
  {
    auto _Whereptr   = _Where._Ptr;
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    move_unchecked(_Whereptr + 1, _Mylast, _Whereptr);
    _Alty_traits::destroy(_Getal(), std::to_address(_Mylast - 1));
    --_Mylast;
    return iterator(_Whereptr);
  }

  constexpr iterator erase(const_iterator _First, const_iterator _Last) noexcept /* strengthened */
  {
    auto _Firstptr   = _First._Ptr;
    auto _Lastptr    = _Last._Ptr;
    auto& _My_data   = _Mypair._Myval2;
    pointer& _Mylast = _My_data._Mylast;

    if (_Firstptr != _Lastptr)
    { // something to do, invalidate iterators
      const pointer _Newlast = move_unchecked(_Lastptr, _Mylast, _Firstptr);
      _TLX destroy_range(_Newlast, _Mylast, _Getal());
      _Mylast = _Newlast;
    }

    return iterator(_Firstptr);
  }

  constexpr void clear() noexcept
  { // erase all
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    if (_Myfirst == _Mylast)
    { // already empty, nothing to do
      // This is an optimization for debug mode: we can avoid taking the debug lock to invalidate iterators.
      // Note that when clearing an empty vector, this will preserve past-the-end iterators, which is allowed by
      // N4950 [sequence.reqmts]/54 "a.clear() [...] may invalidate the past-the-end iterator".
      return;
    }

    _TLX destroy_range(_Myfirst, _Mylast, _Getal());
    _Mylast = _Myfirst;
  }

  constexpr void swap(vector& _Right) noexcept /* strengthened */
  {
    if (this != std::addressof(_Right))
    {
      std::swap(_Getal(), _Right._Getal());
      _Mypair._Myval2._Swap_val(_Right._Mypair._Myval2);
    }
  }

  constexpr _Ty* data() noexcept { return _Mypair._Myval2._Myfirst; }

  constexpr const _Ty* data() const noexcept { return _Mypair._Myval2._Myfirst; }

  constexpr iterator begin() noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return iterator(_My_data._Myfirst);
  }

  constexpr const_iterator begin() const noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return const_iterator(_My_data._Myfirst);
  }

  constexpr iterator end() noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return iterator(_My_data._Mylast);
  }

  constexpr const_iterator end() const noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return const_iterator(_My_data._Mylast);
  }

  constexpr const_iterator cbegin() const noexcept { return begin(); }

  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr bool empty() const noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return _My_data._Myfirst == _My_data._Mylast;
  }

  constexpr size_type size() const noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst);
  }

  constexpr size_type max_size() const noexcept
  {
    return (std::min)(static_cast<size_type>((std::numeric_limits<difference_type>::max)()), _Alty_traits::max_size(_Getal()));
  }

  constexpr size_type capacity() const noexcept
  {
    auto& _My_data = _Mypair._Myval2;
    return static_cast<size_type>(_My_data._Myend - _My_data._Myfirst);
  }

  constexpr _Ty& operator[](const size_type _Pos) noexcept /* strengthened */
  {
    auto& _My_data = _Mypair._Myval2;
    _TLX_VERIFY(_Pos < static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst), "vector subscript out of range");
    return _My_data._Myfirst[_Pos];
  }

  constexpr const _Ty& operator[](const size_type _Pos) const noexcept /* strengthened */
  {
    auto& _My_data = _Mypair._Myval2;
    _TLX_VERIFY(_Pos < static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst), "vector subscript out of range");
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
    _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "front() called on empty vector");
    return *_My_data._Myfirst;
  }

  constexpr const _Ty& front() const noexcept /* strengthened */
  {
    auto& _My_data = _Mypair._Myval2;
    _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "front() called on empty vector");
    return *_My_data._Myfirst;
  }

  constexpr _Ty& back() noexcept /* strengthened */
  {
    auto& _My_data = _Mypair._Myval2;
    _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "back() called on empty vector");
    return _My_data._Mylast[-1];
  }

  constexpr const _Ty& back() const noexcept /* strengthened */
  {
    auto& _My_data = _Mypair._Myval2;
    _TLX_VERIFY(_My_data._Myfirst != _My_data._Mylast, "back() called on empty vector");
    return _My_data._Mylast[-1];
  }

  constexpr allocator_type get_allocator() const noexcept { return static_cast<allocator_type>(_Getal()); }

private:
  constexpr size_type _Calculate_growth(const size_type _Newsize) const
  {
    // given _Oldcapacity and _Newsize, calculate geometric growth
    const size_type _Oldcapacity = capacity();
    const auto _Max              = max_size();

    if (_Oldcapacity > _Max - _Oldcapacity / 2)
    {
      return _Max; // geometric growth would overflow
    }

    const size_type _Geometric = _Oldcapacity + _Oldcapacity / 2;

    if (_Geometric < _Newsize)
    {
      return _Newsize; // geometric growth would be insufficient
    }

    return _Geometric; // geometric growth is sufficient
  }

  constexpr void _Buy_raw(size_type _Newcapacity)
  {
    // allocate array with _Newcapacity elements
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;

    _TLX_INTERNAL_CHECK(!_Myfirst && !_Mylast && !_Myend); // check that *this is tidy
    _TLX_INTERNAL_CHECK(0 < _Newcapacity && _Newcapacity <= max_size());

    const pointer _Newvec = _Getal().allocate(_Newcapacity);
    _Myfirst              = _Newvec;
    _Mylast               = _Newvec;
    _Myend                = _Newvec + _Newcapacity;
  }

  constexpr void _Buy_nonzero(const size_type _Newcapacity)
  {
    // allocate array with _Newcapacity elements
#ifdef _ENABLE_TLX_INTERNAL_CHECK
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;
    _TLX_INTERNAL_CHECK(!_Myfirst && !_Mylast && !_Myend); // check that *this is tidy
    _TLX_INTERNAL_CHECK(0 < _Newcapacity);
#endif // _ENABLE_TLX_INTERNAL_CHECK

    if (_Newcapacity > max_size())
    {
      _Xlength();
    }

    _Buy_raw(_Newcapacity);
  }

  constexpr void _Change_array(const pointer _Newvec, const size_type _Newsize, const size_type _Newcapacity) noexcept
  {
    // orphan all iterators, discard old array, acquire new array
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;

    if (_Myfirst)
    { // destroy and deallocate old array
      _TLX destroy_range(_Myfirst, _Mylast, _Al);
      _Al.deallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst));
    }

    _Myfirst = _Newvec;
    _Mylast  = _Newvec + _Newsize;
    _Myend   = _Newvec + _Newcapacity;
  }

  constexpr void _Tidy() noexcept
  { // free all storage
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;
    pointer& _Myend   = _My_data._Myend;

    if (_Myfirst)
    { // destroy and deallocate old array
      _TLX destroy_range(_Myfirst, _Mylast, _Al);
      _Al.deallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst));

      _Myfirst = nullptr;
      _Mylast  = nullptr;
      _Myend   = nullptr;
    }
  }

  template <class... _Valty>
  constexpr void _Construct_n(const size_type _Count, _Valty&&... _Val)
  {
    // Dispatch between the three sized constructions:
    // 1-arg -> value-construction, e.g. vector(5)
    // 2-arg -> fill, e.g. vector(5, "meow")
    // 3-arg -> sized range construction, e.g. vector{"Hello", "Fluffy", "World"}

    auto& _Al      = _Getal();
    auto& _My_data = _Mypair._Myval2;

    if (_Count != 0)
    {
      _Buy_nonzero(_Count);

      if constexpr (sizeof...(_Val) == 0)
      {
        // value-construction
        _My_data._Mylast = uninitialized_value_construct_n(_My_data._Myfirst, _Count, _Al);
      }
      else if constexpr (sizeof...(_Val) == 1)
      {
        // fill construction
        _My_data._Mylast = uninitialized_fill_n(_My_data._Myfirst, _Count, _Val..., _Al);
      }
      else if constexpr (sizeof...(_Val) == 2)
      {
        // range construction
        _My_data._Mylast = uninitialized_copy(std::forward<_Valty>(_Val)..., _My_data._Myfirst, _Al);
      }
      else
      {
        // unsupported overload
      }
    }
  }

  constexpr void _Move_assign_unequal_alloc(vector& _Right)
  {
    auto& _Al         = _Getal();
    auto& _My_data    = _Mypair._Myval2;
    auto& _Right_data = _Right._Mypair._Myval2;

    const pointer _First = _Right_data._Myfirst;
    const pointer _Last  = _Right_data._Mylast;
    const auto _Newsize  = static_cast<size_type>(_Last - _First);

    pointer& _Myfirst = _My_data._Myfirst;
    pointer& _Mylast  = _My_data._Mylast;

    const auto _Oldcapacity = static_cast<size_type>(_My_data._Myend - _Myfirst);
    if (_Newsize > _Oldcapacity)
    {
      _Clear_and_reserve_geometric(_Newsize);
      _Mylast = uninitialized_move(_First, _Last, _Myfirst, _Al);
      return;
    }

    const auto _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);
    if (_Newsize > _Oldsize)
    {
      const pointer _Mid = _First + _Oldsize;
      move_unchecked(_First, _Mid, _Myfirst);
      _Mylast = uninitialized_move(_Mid, _Last, _Mylast, _Al);
    }
    else
    {
      const pointer _Newlast = _Myfirst + _Newsize;
      move_unchecked(_First, _Last, _Myfirst);
      _Mylast = _Newlast;
    }
  }

  static void _Xlength() { _TLX __xlength_error("vector too long"); }

  static void _Xrange() { _TLX __xout_of_range("invalid vector subscript"); }

  constexpr _Alty& _Getal() noexcept { return _Mypair._Get_first(); }

  constexpr const _Alty& _Getal() const noexcept { return _Mypair._Get_first(); }

  __compressed_pair<_Alty, _Scary_val> _Mypair;
};

#pragma region operator==
template <typename T, typename Alloc, fill_policy Policy>
inline constexpr bool operator==(const tlx::vector<T, Alloc, Policy>& lhs, const tlx::vector<T, Alloc, Policy>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc, fill_policy Policy, typename OtherContainer,
          typename = std::enable_if_t<std::is_same<T, typename OtherContainer::value_type>::value &&
                                      !std::is_same<OtherContainer, tlx::vector<T, Alloc, Policy>>::value>>
inline constexpr bool operator==(const tlx::vector<T, Alloc, Policy>& lhs, const OtherContainer& rhs)
{
  if (lhs.size() != rhs.size())
    return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc, fill_policy Policy, typename OtherContainer,
          typename = std::enable_if_t<std::is_same<T, typename OtherContainer::value_type>::value &&
                                      !std::is_same<OtherContainer, tlx::vector<T, Alloc, Policy>>::value>>
inline constexpr bool operator==(const OtherContainer& lhs, const tlx::vector<T, Alloc, Policy>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
#pragma endregion

#pragma region c++20 like std::erase
template <typename _Ty, typename _Alloc, fill_policy _Policy>
void erase(vector<_Ty, _Alloc, _Policy>& cont, const _Ty& val)
{
  cont.erase(std::remove(cont.begin(), cont.end(), val), cont.end());
}
template <typename _Ty, typename _Alloc, fill_policy _Policy, typename _Pr>
void erase_if(vector<_Ty, _Alloc, _Policy>& cont, _Pr pred)
{
  cont.erase(std::remove_if(cont.begin(), cont.end(), pred), cont.end());
}
#pragma endregion

#pragma region ordered insert, for flat container emulating
template <typename _Cont>
inline typename _Cont::iterator ordered_insert(_Cont& vec, typename _Cont::value_type const& val)
{
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), val), val);
}

template <typename _Cont, typename _Pred>
inline typename _Cont::iterator ordered_insert(_Cont& vec, typename _Cont::value_type const& val, _Pred pred)
{
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), val, pred), val);
}
#pragma endregion

} // namespace tlx

namespace std
{
template <class _Myvec>
struct pointer_traits<_TLX sequence_const_iterator<_Myvec>> {
  using pointer         = _TLX sequence_const_iterator<_Myvec>;
  using element_type    = const pointer::value_type;
  using difference_type = pointer::difference_type;

  static constexpr element_type* to_address(const pointer _Iter) noexcept { return std::to_address(_Iter._Ptr); }
};
template <class _Myvec>
struct pointer_traits<_TLX sequence_iterator<_Myvec>> {
  using pointer         = _TLX sequence_iterator<_Myvec>;
  using element_type    = pointer::value_type;
  using difference_type = pointer::difference_type;

  static constexpr element_type* to_address(const pointer _Iter) noexcept { return std::to_address(_Iter._Ptr); }
};
} // namespace std
