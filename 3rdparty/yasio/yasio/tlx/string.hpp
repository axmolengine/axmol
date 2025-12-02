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

The tlx dedicated string (API not 100% compatible with stl) concepts:
   a. no SSO, no COW, sizeof(tlx::string) = 24(x64), 12(x86)
   b. The resize behavior differrent stl, always allocate exactly
   c. By default resize without fill (uninitialized and for overwrite),
      use insert/append insetad if you want fill memory inside container
   d. Support release internal buffer ownership with `release_pointer`
   e. Transparent iterator
   f. The operations expand/append/insert/push_back/+= will trigger memory allocate growth strategy MSVC
   g. resize_and_overwrite (c++23)
   h. provide API replace_all, to_upper, to_lower which stl not provide
   i. More suitable for small text file (> SSO size) read API
*/

#pragma once

#include <ctype.h>
#include <utility>
#include <memory>
#include <iterator>
#include <limits>
#include <algorithm>
#include "yasio/tlx/buffer_alloc.hpp" // crt_buffer_allocator
#include "yasio/tlx/string_view.hpp"
#include "yasio/tlx/memory.hpp" // compressed_pair

namespace tlx
{
template <typename _Elem, typename _Alloc = _TLX crt_buffer_allocator<_Elem>, _TLX enable_if_t<is_char_type<_Elem>::value, int> = 0>
class basic_string {
  YASIO__CONSTEXPR _Alloc& _Getal() noexcept { return _Mypair._Get_first(); }

public:
  using pointer            = _Elem*;
  using const_pointer      = const _Elem*;
  using reference          = _Elem&;
  using const_reference    = const _Elem&;
  using value_type         = _Elem;
  using iterator           = _Elem*; // transparent iterator
  using const_iterator     = const _Elem*;
  using allocator_type     = _Alloc;
  using _Alloc_traits      = std::allocator_traits<_Alloc>;
  using size_type          = typename _Alloc_traits::size_type;
  using _Traits            = std::char_traits<_Elem>;
  using view_type          = std::basic_string_view<_Elem>;
  using my_type            = basic_string<_Elem, _Alloc>;
  static const size_t npos = static_cast<size_t>(-1);

  // compressed storage: allocator + 3 pointers (_Myfirst, _Mylast, _Myend)
  struct _Str_storage {
    pointer _Myfirst = nullptr; // begin
    pointer _Mylast  = nullptr; // one past last character
    pointer _Myend   = nullptr; // one past end of storage
  };
  __compressed_pair<allocator_type, _Str_storage> _Mypair;

  basic_string() : _Mypair(__zero_then_variadic_args_t{}) {}
  basic_string(nullptr_t) = delete;
  explicit basic_string(size_type count) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { resize(static_cast<size_type>(count)); }
  basic_string(size_type count, const_reference val) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { resize(static_cast<size_type>(count), val); }
  template <typename _Iter, _TLX enable_if_t<_TLX is_iterator<_Iter>::value, int> = 0>
  basic_string(_Iter first, _Iter last) : _Mypair(__one_then_variadic_args_t{}, _Alloc{})
  {
    assign(first, last);
  }
  basic_string(const basic_string& rhs) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { assign(rhs); }
  basic_string(basic_string&& rhs) YASIO__NOEXCEPT : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { _Assign_rv(std::move(rhs)); }
  basic_string(view_type rhs) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { assign(rhs); }
  basic_string(const_pointer ntcs) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { assign(ntcs); }
  basic_string(const_pointer ntcs, size_type count) : _Mypair(__one_then_variadic_args_t{}, _Alloc{}) { assign(ntcs, ntcs + count); }
  /*basic_string(std::initializer_list<value_type> rhs) { _Assign_range(rhs.begin(), rhs.end()); }*/
  ~basic_string() { _Tidy(); }

  operator view_type() const YASIO__NOEXCEPT { return this->view(); }
  view_type view() const YASIO__NOEXCEPT { return view_type(this->c_str(), this->size()); }

  basic_string& operator=(const basic_string& rhs)
  {
    assign(rhs);
    return *this;
  }
  basic_string& operator=(basic_string&& rhs) YASIO__NOEXCEPT
  {
    this->swap(rhs);
    return *this;
  }

  template <typename _Cont>
  basic_string& operator+=(const _Cont& rhs)
  {
    return this->append(std::begin(rhs), std::end(rhs));
  }
  basic_string& operator+=(const_reference rhs)
  {
    this->push_back(rhs);
    return *this;
  }

  template <typename _Iter, _TLX enable_if_t<_TLX is_iterator<_Iter>::value, int> = 0>
  void assign(_Iter first, _Iter last)
  {
    _Assign_range(first, last);
  }
  void assign(view_type rhs) { _Assign_range(rhs.begin(), rhs.end()); }
  void assign(const_pointer ntcs) { this->assign(ntcs, static_cast<size_type>(_Traits::length(ntcs))); }
  void assign(const_pointer ntcs, size_type count) { _Assign_range(ntcs, ntcs + count); }
  void assign(const basic_string& rhs) { _Assign_range(rhs.begin(), rhs.end()); }
  void assign(basic_string&& rhs) { _Assign_rv(std::move(rhs)); }

  void swap(basic_string& rhs) YASIO__NOEXCEPT
  {
    std::swap(_Getal(), rhs._Getal());

    auto& a = _Mypair._Myval2;
    auto& b = rhs._Mypair._Myval2;
    std::swap(a._Myfirst, b._Myfirst);
    std::swap(a._Mylast, b._Mylast);
    std::swap(a._Myend, b._Myend);
  }

  template <typename _Iter, _TLX enable_if_t<_TLX is_iterator<_Iter>::value, int> = 0>
  iterator insert(iterator _Where, _Iter first, _Iter last)
  {
    auto& st     = _Mypair._Myval2;
    auto _Mylast = st._Mylast;
    _TLX_VERIFY(_Where >= st._Myfirst && _Where <= _Mylast && first <= last, "basic_string: out of range!");
    if (first != last)
    {
      auto insertion_pos = static_cast<size_type>(std::distance(st._Myfirst, _Where));
      if (_Where == _Mylast)
        append(first, last);
      else
      {
        auto ifirst = std::addressof(*first);
        static_assert(sizeof(*ifirst) == sizeof(value_type), "basic_string: iterator type incompatible!");
        auto count = static_cast<size_type>(std::distance(first, last));
        if (insertion_pos >= 0)
        {
          auto old_size = static_cast<size_type>(_Mylast - st._Myfirst);
          expand(count);
          _Where       = st._Myfirst + insertion_pos;
          _Mylast      = st._Mylast;
          auto move_to = _Where + count;
          std::copy_n(_Where, static_cast<size_type>(_Mylast - move_to), move_to);
          std::copy_n((iterator)ifirst, count, _Where);
        }
      }
      return st._Myfirst + insertion_pos;
    }
    return _Where;
  }

  iterator insert(iterator _Where, size_type count, const_reference val)
  {
    auto& st     = _Mypair._Myval2;
    auto _Mylast = st._Mylast;
    _TLX_VERIFY(_Where >= st._Myfirst && _Where <= _Mylast, "basic_string: out of range!");
    if (count)
    {
      auto insertion_pos = std::distance(st._Myfirst, _Where);
      if (_Where == _Mylast)
        append(count, val);
      else
      {
        if (insertion_pos >= 0)
        {
          const auto old_size = size();
          expand(count);
          _Where       = st._Myfirst + insertion_pos;
          _Mylast      = st._Mylast;
          auto move_to = _Where + count;
          std::copy_n(_Where, static_cast<size_type>(_Mylast - move_to), move_to);
          std::fill_n(_Where, count, val);
        }
      }
      return st._Myfirst + insertion_pos;
    }
    return _Where;
  }

  basic_string& append(view_type value) { return this->append(value.begin(), value.end()); }
  template <typename _Iter, _TLX enable_if_t<_TLX is_iterator<_Iter>::value, int> = 0>
  basic_string& append(_Iter first, const _Iter last)
  {
    if (first != last)
    {
      auto& st    = _Mypair._Myval2;
      auto ifirst = std::addressof(*first);
      static_assert(sizeof(*ifirst) == sizeof(value_type), "basic_string: iterator type incompatible!");
      auto count = static_cast<size_type>(std::distance(first, last));
      if (count > 1)
      {
        const auto old_size = size();
        expand(count);
        std::copy_n((iterator)ifirst, count, st._Myfirst + old_size);
      }
      else if (count == 1)
        push_back(static_cast<value_type>(*(iterator)ifirst));
    }
    return *this;
  }

  basic_string& append(size_type count, const_reference val)
  {
    expand(count, val);
    return *this;
  }

  void push_back(value_type&& v) { push_back(v); }
  void push_back(const value_type& v)
  {
    expand(1);
    back() = v;
  }

  iterator erase(iterator _Where)
  {
    auto& st           = _Mypair._Myval2;
    const auto _Mylast = st._Mylast;
    _TLX_VERIFY(_Where >= st._Myfirst && _Where < _Mylast, "basic_string: out of range!");
    st._Mylast = std::move(_Where + 1, _Mylast, _Where);
    // keep null terminator
    *_Mylast_ptr() = value_type(0);
    return _Where;
  }

  iterator erase(iterator first, iterator last)
  {
    auto& st           = _Mypair._Myval2;
    const auto _Mylast = st._Mylast;
    _TLX_VERIFY((first <= last) && first >= st._Myfirst && last <= _Mylast, "basic_string: out of range!");
    st._Mylast = std::move(last, _Mylast, first);
    // keep null terminator
    *_Mylast_ptr() = value_type(0);
    return first;
  }

  value_type& front()
  {
    _TLX_VERIFY(!empty(), "basic_string: out of range!");
    return *_Mypair._Myval2._Myfirst;
  }

  value_type& back()
  {
    _TLX_VERIFY(!empty(), "basic_string: out of range!");
    return *(_Mypair._Myval2._Mylast - 1);
  }

  static YASIO__CONSTEXPR size_type max_size() YASIO__NOEXCEPT
  {
    // static form to avoid needing allocator instance
    return static_cast<size_type>(-1) / sizeof(value_type);
  }

#pragma region Iterators
  iterator begin() YASIO__NOEXCEPT { return this->data(); }
  iterator end() YASIO__NOEXCEPT { return _Mypair._Myval2._Mylast; }
  const_iterator begin() const YASIO__NOEXCEPT { return this->data(); }
  const_iterator end() const YASIO__NOEXCEPT { return _Mypair._Myval2._Mylast; }
#pragma endregion

  pointer data() YASIO__NOEXCEPT { return _Mypair._Myval2._Myfirst; }
  const_pointer data() const YASIO__NOEXCEPT { return _Mypair._Myval2._Myfirst; }

  const_pointer c_str() const YASIO__NOEXCEPT
  {
    auto& st = _Mypair._Myval2;
    return st._Myfirst ? st._Myfirst : reinterpret_cast<pointer>(&st._Myfirst);
  }

  const_reference operator[](size_type index) const { return this->at(index); }
  reference operator[](size_type index) { return this->at(index); }
  const_reference at(size_type index) const
  {
    _TLX_VERIFY(index < this->size(), "basic_string: out of range!");
    return _Mypair._Myval2._Myfirst[index];
  }
  reference at(size_type index)
  {
    _TLX_VERIFY(index < this->size(), "basic_string: out of range!");
    return _Mypair._Myval2._Myfirst[index];
  }

#pragma region Capacity
  size_type capacity() const YASIO__NOEXCEPT
  {
    auto& st = _Mypair._Myval2;
    return static_cast<size_type>(st._Myend - st._Myfirst);
  }
  size_type size() const YASIO__NOEXCEPT
  {
    auto& st = _Mypair._Myval2;
    return static_cast<size_type>(st._Mylast - st._Myfirst);
  }
  size_type length() const YASIO__NOEXCEPT { return size(); }
  void clear() YASIO__NOEXCEPT
  {
    auto& st   = _Mypair._Myval2;
    st._Mylast = st._Myfirst;
    if (st._Myfirst)
      *st._Mylast = value_type(0);
  }
  bool empty() const YASIO__NOEXCEPT { return _Mypair._Myval2._Mylast == _Mypair._Myval2._Myfirst; }

  void resize(size_type new_size)
  {
    if (this->capacity() < new_size)
      _Resize_reallocate<_Reallocation_policy::_Exactly>(new_size);
    else
      _Eos(new_size);
  }

  void expand(size_type count)
  {
    const auto new_size = this->size() + count;
    if (this->capacity() < new_size)
      _Resize_reallocate<_Reallocation_policy::_At_least>(new_size);
    else
      _Eos(new_size);
  }

  void shrink_to_fit()
  { // reduce capacity to size, provide strong guarantee
    auto& st = _Mypair._Myval2;
    if (st._Mylast != st._Myend)
    { // something to do
      if (st._Mylast == st._Myfirst)
        _Tidy();
      else
        _Reallocate<_Reallocation_policy::_Exactly>(size());
    }
  }

  void reserve(size_type new_cap)
  {
    if (this->capacity() < new_cap)
      _Reallocate<_Reallocation_policy::_Exactly>(new_cap);
  }

  template <typename _Operation>
  void resize_and_overwrite(const size_type _New_size, _Operation _Op)
  {
    _Reallocate<_Reallocation_policy::_Exactly>(_New_size);
    auto& st = _Mypair._Myval2;
    // _Op writes up to _New_size and returns new size
    size_type written = std::move(_Op)(st._Myfirst, _New_size);
    _Eos(written);
  }
#pragma endregion

  void resize(size_type new_size, const_reference val)
  {
    auto old_size = this->size();
    if (old_size != new_size)
    {
      resize(new_size);
      if (old_size < new_size)
        std::fill_n(_Mypair._Myval2._Myfirst + old_size, new_size - old_size, val);
    }
  }

  void expand(size_type count, const_reference val)
  {
    if (count)
    {
      auto old_size = this->size();
      expand(count);
      if (count)
        std::fill_n(_Mypair._Myval2._Myfirst + old_size, count, val);
    }
  }

  template <typename _Intty>
  pointer detach_abi(_Intty& len) YASIO__NOEXCEPT
  {
    auto& st    = _Mypair._Myval2;
    len         = static_cast<_Intty>(this->size());
    auto ptr    = st._Myfirst;
    st._Myfirst = st._Mylast = st._Myend = nullptr;
    return ptr;
  }

  pointer detach_abi() YASIO__NOEXCEPT
  {
    size_type ignored_len;
    return this->detach_abi(ignored_len);
  }

  void attach_abi(pointer ptr, size_type len)
  {
    _Tidy();
    auto& st    = _Mypair._Myval2;
    st._Myfirst = ptr;
    st._Mylast  = ptr + len;
    st._Myend   = ptr + len;
  }

  pointer release_pointer() YASIO__NOEXCEPT { return detach_abi(); }

#pragma region find stubs, String operations
  size_t find(value_type c, size_t pos = 0) const YASIO__NOEXCEPT { return view().find(c, pos); }
  size_t find(view_type str, size_t pos = 0) const YASIO__NOEXCEPT { return view().find(str, pos); }

  size_t rfind(value_type c, size_t pos = npos) const YASIO__NOEXCEPT { return view().rfind(c, pos); }
  size_t rfind(view_type str, size_t pos = npos) const YASIO__NOEXCEPT { return view().rfind(str, pos); }

  size_t find_first_of(value_type c, size_t pos = 0) const YASIO__NOEXCEPT { return view().find_first_of(c, pos); }
  size_t find_first_of(view_type str, size_t pos = 0) const YASIO__NOEXCEPT { return view().find_first_of(str, pos); }

  size_t find_last_of(value_type c, size_t pos = npos) const YASIO__NOEXCEPT { return view().find_last_of(c, pos); }
  size_t find_last_of(view_type str, size_t pos = npos) const YASIO__NOEXCEPT { return view().find_last_of(str, pos); }

  size_t find_first_not_of(value_type c, size_t pos = 0) const YASIO__NOEXCEPT { return view().find_first_not_of(c, pos); }
  size_t find_first_not_of(view_type str, size_t pos = 0) const YASIO__NOEXCEPT { return view().find_first_not_of(str, pos); }

  int compare(view_type str) const YASIO__NOEXCEPT { return view().compare(str); }

  my_type substr(size_t pos = 0, size_t len = npos) const { return my_type{view().substr(pos, len)}; }

  my_type& replace(const size_type _Off, size_type _Nx, view_type value) { return this->replace(_Off, _Nx, value.data(), value.length()); }
  my_type& replace(const size_type _Off, size_type _Nx, const _Elem* const _Ptr, const size_type _Count)
  { // replace port from https://github.com/microsoft/stl
    auto& st = _Mypair._Myval2;
    _TLX_VERIFY(_Off < size(), "basic_string: out of range!");
    _Nx = (std::min)(_Nx, size() - _Off);
    if (_Nx == _Count)
    { // size doesn't change, so a single move does the trick
      _Traits::move(st._Myfirst + _Off, _Ptr, _Count);
      return *this;
    }

    const size_type _Old_size    = size();
    const size_type _Suffix_size = _Old_size - _Nx - _Off + 1;
    if (_Count < _Nx)
    { // suffix shifts backwards; we don't have to move anything out of the way
      _Elem* const _Old_ptr   = st._Myfirst;
      _Elem* const _Insert_at = _Old_ptr + _Off;
      _Traits::move(_Insert_at, _Ptr, _Count);
      _Traits::move(_Insert_at + _Count, _Insert_at + _Nx, _Suffix_size);

      const auto _New_size = _Old_size - (_Nx - _Count);
      _Eos(_New_size);
      return *this;
    }

    const size_type _Growth = static_cast<size_type>(_Count - _Nx);

    if (!std::is_constant_evaluated())
    {
      const size_type _Old_capacity = capacity();
      if (_Growth <= _Old_capacity - _Old_size)
      { // growth fits
        _Eos(_Old_size + _Growth);
        _Elem* const _Old_ptr   = st._Myfirst;
        _Elem* const _Insert_at = _Old_ptr + _Off;
        _Elem* const _Suffix_at = _Insert_at + _Nx;

        size_type _Ptr_shifted_after; // see rationale in insert
        if (_Ptr + _Count <= _Insert_at || _Ptr > _Old_ptr + _Old_size)
        {
          _Ptr_shifted_after = _Count;
        }
        else if (_Suffix_at <= _Ptr)
        {
          _Ptr_shifted_after = 0;
        }
        else
        {
          _Ptr_shifted_after = static_cast<size_type>(_Suffix_at - _Ptr);
        }

        _Traits::move(_Suffix_at + _Growth, _Suffix_at, _Suffix_size);
        _Traits::move(_Insert_at, _Ptr, _Ptr_shifted_after);
        _Traits::copy(_Insert_at + _Ptr_shifted_after, _Ptr + _Growth + _Ptr_shifted_after, _Count - _Ptr_shifted_after);
        return *this;
      }
    }

    return _Reallocate_grow_by(
        _Growth,
        [](_Elem* const _New_ptr, const size_type _Old_size, const size_type _Off, const size_type _Nx, const _Elem* const _Ptr, const size_type _Count) {
          _Traits::copy(_New_ptr + _Off + _Count, _New_ptr + _Off + _Nx, _Old_size - _Nx - _Off + 1);
          _Traits::copy(_New_ptr + _Off, _Ptr, _Count);
        },
        _Off, _Nx, _Ptr, _Count);
  }

  template <class _Fty, class... _ArgTys>
  my_type& _Reallocate_grow_by(const size_type _Size_increase, _Fty _Fn, _ArgTys... _Args)
  {
    const size_type _Old_size = size();
    if (max_size() - _Old_size < _Size_increase)
      throw std::length_error("string too long");

    const size_type _New_size     = _Old_size + _Size_increase;
    const size_type _New_capacity = _Calculate_growth(_New_size);
    auto& st                      = _Mypair._Myval2;
    auto& alloc                   = _Getal();
    pointer _New_ptr              = alloc.reallocate(st._Myfirst, static_cast<size_type>(st._Myend - st._Myfirst), _New_capacity + 1); // throws

    _Eos(_New_size);
    st._Myend = _New_ptr + (_New_capacity + 1);

    const pointer _Old_ptr = st._Myfirst;
    _Fn(_New_ptr, _Old_size, _Args...);
    st._Myfirst = _New_ptr;

    return *this;
  }
#pragma endregion

#pragma region replace all stubs, yasio string spec
  size_t replace_all(view_type from, view_type to)
  {
    if (from == to)
      return 0;
    int hints              = 0;
    size_t pos             = 0;
    const size_t predicate = !from.empty() ? 0 : 1;
    while ((pos = this->find(from, pos)) != my_type::npos)
    {
      (void)this->replace(pos, from.length(), to);
      pos += (to.length() + predicate);
      ++hints;
    }
    return hints;
  }
  void replace_all(value_type from, value_type to) YASIO__NOEXCEPT { std::replace(this->begin(), this->end(), from, to); }
#pragma endregion

#pragma region to_lower, to_upper
  my_type& to_lower() YASIO__NOEXCEPT
  {
    std::transform(this->begin(), this->end(), this->begin(), ::tolower);
    return *this;
  }
  my_type& to_upper() YASIO__NOEXCEPT
  {
    std::transform(this->begin(), this->end(), this->begin(), ::toupper);
    return *this;
  }
#pragma endregion

private:
  void _Eos(size_type size) YASIO__NOEXCEPT
  {
    auto& st   = _Mypair._Myval2;
    st._Mylast = st._Myfirst + size;
    if (st._Myfirst)
      *st._Mylast = static_cast<value_type>(0);
  }

  // convenience: get address of current null-terminator within capacity
  pointer _Mylast_ptr() YASIO__NOEXCEPT
  {
    auto& st = _Mypair._Myval2;
    return st._Mylast; // points to null terminator slot
  }

  template <typename _Iter, _TLX enable_if_t<_TLX is_iterator<_Iter>::value, int> = 0>
  void _Assign_range(_Iter first, _Iter last)
  {
    auto& st    = _Mypair._Myval2;
    auto ifirst = std::addressof(*first);
    static_assert(sizeof(*ifirst) == sizeof(value_type), "basic_string: iterator type incompatible!");
    if (ifirst != st._Myfirst)
    {
      st._Mylast = st._Myfirst; // size = 0
      if (last > first)
      {
        const auto count = static_cast<size_type>(std::distance(first, last));
        resize(count);
        std::copy_n((iterator)ifirst, count, st._Myfirst);
      }
    }
  }

  void _Assign_rv(basic_string&& rhs)
  {
    // move allocator and storage
    std::swap(_Getal(), rhs._Getal());
    _Mypair._Myval2     = rhs._Mypair._Myval2;
    rhs._Mypair._Myval2 = _Str_storage{};
  }

  enum class _Reallocation_policy
  {
    _At_least,
    _Exactly
  };

  template <_Reallocation_policy _Policy>
  void _Resize_reallocate(size_type size)
  {
    _Reallocate<_Policy>(size);
    _Eos(size);
  }

  template <_Reallocation_policy _Policy>
  void _Reallocate(size_type size)
  {
    auto& st    = _Mypair._Myval2;
    auto& alloc = _Getal();
    size_type new_cap;
    if YASIO__CONSTEXPR (_Policy == _Reallocation_policy::_Exactly)
      new_cap = size + 1;
    else
      new_cap = (std::max)(_Calculate_growth(size), size) + 1;

    pointer _Newvec = alloc.reallocate(st._Myfirst, static_cast<size_type>(st._Myend - st._Myfirst), new_cap);
    if (_Newvec)
    {
      st._Myend           = _Newvec + new_cap;
      const auto cur_size = size; // caller will set exact size via _Eos
      st._Myfirst         = _Newvec;
      st._Mylast          = _Newvec + cur_size;
    }
    else
      throw std::bad_alloc{};
  }

  size_type _Calculate_growth(const size_type _Newsize) const
  {
    // given _Oldcapacity and _Newsize, calculate geometric growth
    const size_type _Oldcapacity = capacity();
    const size_type _Max         = max_size();

    if (_Oldcapacity > _Max - _Oldcapacity / 2)
      return _Max; // geometric growth would overflow

    const size_type _Geometric = _Oldcapacity + (_Oldcapacity >> 1);

    if (_Geometric < _Newsize)
      return _Newsize; // geometric growth would be insufficient

    return _Geometric; // geometric growth is sufficient
  }

  void _Tidy() YASIO__NOEXCEPT
  { // free all storage
    auto& st    = _Mypair._Myval2;
    auto& alloc = _Getal();
    if (st._Myfirst)
    {
      alloc.deallocate(st._Myfirst, static_cast<size_type>(st._Myend - st._Myfirst));
      st._Myfirst = st._Mylast = st._Myend = nullptr;
    }
  }
};

// aliases
using string = basic_string<char>;
#if defined(__cpp_lib_char8_t)
using u8string = basic_string<char8_t>;
#endif
using wstring   = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
} // namespace tlx
