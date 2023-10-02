//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

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

The pod_vector aka array_buffer concepts:
   a. The memory model is similar to to std::vector, but only accept trivially_copyable(no destructor & no custom copy constructor) types
   b. The resize behavior differrent stl, always allocate exactly
   c. By default resize without fill (uninitialized and for overwrite)
   d. Support release internal buffer ownership with `release_pointer`
   e. Transparent iterator
   f. expand/append/insert/push_back will trigger memory allocate growth strategy MSVC
   g. resize_and_overwrite (c++23)
*/
#ifndef YASIO__POD_VECTOR_HPP
#define YASIO__POD_VECTOR_HPP
#include <utility>
#include <memory>
#include <iterator>
#include <limits>
#include <algorithm>
#include <initializer_list>
#include "yasio/buffer_alloc.hpp"
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
template <typename _Elem, typename _Alloc = default_buffer_allocator<_Elem>>
class pod_vector {
public:
  using pointer         = _Elem*;
  using const_pointer   = const _Elem*;
  using reference       = _Elem&;
  using const_reference = const _Elem&;
  using size_type       = size_t;
  using value_type      = _Elem;
  using iterator        = _Elem*; // transparent iterator
  using const_iterator  = const _Elem*;
  using allocator_type  = _Alloc;
  pod_vector() {}
  explicit pod_vector(size_type count) { resize(count); }
  pod_vector(size_type count, const_reference val) { resize(count, val); }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  pod_vector(_Iter first, _Iter last)
  {
    assign(first, last);
  }
  pod_vector(const pod_vector& rhs) { assign(rhs); };
  pod_vector(pod_vector&& rhs) YASIO__NOEXCEPT { assign(std::move(rhs)); }
  /*pod_vector(std::initializer_list<value_type> rhs) { _Assign_range(rhs.begin(), rhs.end()); }*/
  ~pod_vector() { _Tidy(); }
  pod_vector& operator=(const pod_vector& rhs)
  {
    assign(rhs);
    return *this;
  }
  pod_vector& operator=(pod_vector&& rhs) YASIO__NOEXCEPT
  {
    this->swap(rhs);
    return *this;
  }
  template <typename _Cont>
  pod_vector& operator+=(const _Cont& rhs)
  {
    return this->append(std::begin(rhs), std::end(rhs));
  }
  pod_vector& operator+=(const_reference rhs)
  {
    this->push_back(rhs);
    return *this;
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  void assign(_Iter first, _Iter last)
  {
    _Assign_range(first, last);
  }
  void assign(const pod_vector& rhs) { _Assign_range(rhs.begin(), rhs.end()); }
  void assign(pod_vector&& rhs) { _Assign_rv(std::move(rhs)); }
  void swap(pod_vector& rhs) YASIO__NOEXCEPT
  {
    std::swap(_Myfirst, rhs._Myfirst);
    std::swap(_Mylast, rhs._Mylast);
    std::swap(_Myend, rhs._Myend);
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  iterator insert(iterator _Where, _Iter first, _Iter last)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where <= _Mylast && first <= last, "pod_vector: out of range!");
    if (first != last)
    {
      auto insertion_pos = std::distance(_Myfirst, _Where);
      if (_Where == _Mylast)
        append(first, last);
      else
      {
        auto ifirst = std::addressof(*first);
        static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
        auto count = std::distance(first, last);
        if (insertion_pos >= 0)
        {
          auto old_size = _Mylast - _Myfirst;
          expand(count);
          _Where       = _Myfirst + insertion_pos;
          auto move_to = _Where + count;
          std::copy_n(_Where, _Mylast - move_to, move_to);
          std::copy_n((iterator)ifirst, count, _Where);
        }
      }
      return _Myfirst + insertion_pos;
    }
    return _Where;
  }
  iterator insert(iterator _Where, size_type count, const_reference val)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where <= _Mylast, "pod_vector: out of range!");
    if (count)
    {
      auto insertion_pos = std::distance(_Myfirst, _Where);
      if (_Where == _Mylast)
        append(count, val);
      else
      {
        if (insertion_pos >= 0)
        {
          auto old_size = _Mylast - _Myfirst;
          expand(count);
          _Where       = _Myfirst + insertion_pos;
          auto move_to = _Where + count;
          std::copy_n(_Where, _Mylast - move_to, move_to);
          std::fill_n(_Where, count, val);
        }
      }
      return _Myfirst + insertion_pos;
    }
    return _Where;
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  pod_vector& append(_Iter first, const _Iter last)
  {
    if (first != last)
    {
      auto ifirst = std::addressof(*first);
      static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
      auto count = std::distance(first, last);
      if (count > 1)
      {
        auto old_size = _Mylast - _Myfirst;
        expand(count);
        std::copy_n((iterator)ifirst, count, _Myfirst + old_size);
      }
      else if (count == 1)
        push_back(static_cast<value_type>(*(iterator)ifirst));
    }
    return *this;
  }
  pod_vector& append(size_t count, const_reference val)
  {
    expand(count, val);
    return *this;
  }
  void push_back(value_type&& v) { push_back(v); }
  void push_back(const value_type& v)
  {
    expand(1);
    _Mylast[-1] = v;
  }
  template <typename... _Valty>
  inline value_type& emplace_back(_Valty&&... _Val)
  {
    if (_Mylast != _Myend)
      return *construct_helper<value_type>::construct_at(_Mylast++, std::forward<_Valty>(_Val)...);
    return *_Emplace_back_reallocate(std::forward<_Valty>(_Val)...);
  }
  iterator erase(iterator _Where)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where < _Mylast, "pod_vector: out of range!");
    _Mylast = std::move(_Where + 1, _Mylast, _Where);
    return _Where;
  }
  iterator erase(iterator first, iterator last)
  {
    _YASIO_VERIFY_RANGE((first <= last) && first >= _Myfirst && last <= _Mylast, "pod_vector: out of range!");
    _Mylast = std::move(last, _Mylast, first);
    return first;
  }
  value_type& front()
  {
    _YASIO_VERIFY_RANGE(_Myfirst < _Mylast, "pod_vector: out of range!");
    return *_Myfirst;
  }
  value_type& back()
  {
    _YASIO_VERIFY_RANGE(_Myfirst < _Mylast, "pod_vector: out of range!");
    return _Mylast[-1];
  }
  static YASIO__CONSTEXPR size_type max_size() YASIO__NOEXCEPT { return (std::numeric_limits<ptrdiff_t>::max)() / sizeof(value_type); }
  iterator begin() YASIO__NOEXCEPT { return _Myfirst; }
  iterator end() YASIO__NOEXCEPT { return _Mylast; }
  const_iterator begin() const YASIO__NOEXCEPT { return _Myfirst; }
  const_iterator end() const YASIO__NOEXCEPT { return _Mylast; }
  pointer data() YASIO__NOEXCEPT { return _Myfirst; }
  const_pointer data() const YASIO__NOEXCEPT { return _Myfirst; }
  size_type capacity() const YASIO__NOEXCEPT { return static_cast<size_type>(_Myend - _Myfirst); }
  size_type size() const YASIO__NOEXCEPT { return static_cast<size_type>(_Mylast - _Myfirst); }
  size_type length() const YASIO__NOEXCEPT { return static_cast<size_type>(_Mylast - _Myfirst); }
  void clear() YASIO__NOEXCEPT { _Mylast = _Myfirst; }
  bool empty() const YASIO__NOEXCEPT { return _Mylast == _Myfirst; }

  const_reference operator[](size_type index) const { return this->at(index); }
  reference operator[](size_type index) { return this->at(index); }
  const_reference at(size_type index) const
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "pod_vector: out of range!");
    return _Myfirst[index];
  }
  reference at(size_type index)
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "pod_vector: out of range!");
    return _Myfirst[index];
  }
#pragma region modify size and capacity
  void resize(size_type new_size)
  {
    auto old_cap = this->capacity();
    if (old_cap < new_size)
      _Reallocate_exactly(new_size);
    _Eos(new_size);
  }
  void expand(size_type count)
  {
    const auto new_size = this->size() + count;
    auto old_cap        = this->capacity();
    if (old_cap < new_size)
      _Reallocate_exactly(_Calculate_growth(new_size));
    _Eos(new_size);
  }
  void shrink_to_fit()
  { // reduce capacity to size, provide strong guarantee
    const pointer _Oldlast = _Mylast;
    if (_Oldlast != _Myend)
    { // something to do
      const pointer _Oldfirst = _Myfirst;
      if (_Oldfirst == _Oldlast)
        _Tidy();
      else
      {
        const auto count = static_cast<size_type>(_Oldlast - _Oldfirst);
        _Reallocate_exactly(count);
        _Eos(count);
      }
    }
  }
  void reserve(size_type new_cap)
  {
    if (this->capacity() < new_cap)
    {
      const auto count = this->size();
      _Reallocate_exactly(new_cap);
      _Eos(count);
    }
  }
  template <typename _Operation>
  void resize_and_overwrite(const size_type _New_size, _Operation _Op)
  {
    _Reallocate_exactly(_New_size);
    _Eos(std::move(_Op)(_Myfirst, _New_size));
  }
#pragma endregion
  void resize(size_type new_size, const_reference val)
  {
    auto old_size = this->size();
    resize(new_size);
    if (old_size < new_size)
      std::fill(_Myfirst + old_size, _Mylast, val);
  }
  void expand(size_type count, const_reference val)
  {
    auto old_size = this->size();
    expand(count);
    if (count)
      std::fill(_Myfirst + old_size, _Mylast, val);
  }
  ptrdiff_t index_of(const_reference val) const YASIO__NOEXCEPT
  {
    auto it = std::find(begin(), end(), val);
    if (it != this->end())
      return std::distance(begin(), it);
    return -1;
  }
  void reset(size_t new_size)
  {
    resize(new_size);
    memset(_Myfirst, 0x0, size_bytes());
  }
  size_t size_bytes() const YASIO__NOEXCEPT { return this->size() * sizeof(value_type); }
  template <typename _Intty>
  pointer detach_abi(_Intty& len) YASIO__NOEXCEPT
  {
    len      = static_cast<_Intty>(this->size());
    auto ptr = _Myfirst;
    _Myfirst = nullptr;
    _Mylast  = nullptr;
    _Myend   = nullptr;
    return ptr;
  }
  pointer detach_abi() YASIO__NOEXCEPT
  {
    size_t ignored_len;
    return this->detach_abi(ignored_len);
  }
  void attach_abi(pointer ptr, size_t len)
  {
    _Tidy();
    _Myfirst = ptr;
    _Myend = _Mylast = ptr + len;
  }
  pointer release_pointer() YASIO__NOEXCEPT { return detach_abi(); }
private:
  void _Eos(size_t size) YASIO__NOEXCEPT { _Mylast = _Myfirst + size; }
  template <typename... _Valty>
  pointer _Emplace_back_reallocate(_Valty&&... _Val)
  {
    const auto _Oldsize = static_cast<size_type>(_Mylast - _Myfirst);

    if (_Oldsize == max_size())
      throw std::length_error("pod_vector too long");

    const size_type _Newsize     = _Oldsize + 1;
    const size_type _Newcapacity = _Calculate_growth(_Newsize);

    _Reallocate_exactly(_Newcapacity);
    const pointer _Newptr = construct_helper<value_type>::construct_at(_Myfirst + _Oldsize, std::forward<_Valty>(_Val)...);
    _Eos(_Newsize);
    return _Newptr;
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  void _Assign_range(_Iter first, _Iter last)
  {
    auto ifirst = std::addressof(*first);
    static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
    if (ifirst != _Myfirst)
    {
      _Mylast = _Myfirst;
      if (last > first)
      {
        const auto count = std::distance(first, last);
        resize(count);
        std::copy_n((iterator)ifirst, count, _Myfirst);
      }
    }
  }
  void _Assign_rv(pod_vector&& rhs)
  {
    memcpy(this, &rhs, sizeof(rhs));
    memset(&rhs, 0, sizeof(rhs));
  }
  void _Reallocate_exactly(size_type new_cap)
  {
    auto _Newvec = _Alloc::reallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst), new_cap);
    if (_Newvec || !new_cap)
    {
      _Myfirst = _Newvec;
      _Myend   = _Newvec + new_cap;
    }
    else
      throw std::bad_alloc{};
  }
  size_type _Calculate_growth(const size_type _Newsize) const
  {
    // given _Oldcapacity and _Newsize, calculate geometric growth
    const size_type _Oldcapacity = capacity();
    YASIO__CONSTEXPR auto _Max   = max_size();

    if (_Oldcapacity > _Max - _Oldcapacity / 2)
      return _Max; // geometric growth would overflow

    const size_type _Geometric = _Oldcapacity + (_Oldcapacity >> 1);

    if (_Geometric < _Newsize)
      return _Newsize; // geometric growth would be insufficient

    return _Geometric; // geometric growth is sufficient
  }
  void _Tidy() YASIO__NOEXCEPT
  { // free all storage
    if (_Myfirst)
    {
      _Alloc::reallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst), 0);
      _Myfirst = _Mylast = _Myend = nullptr;
    }
  }

  pointer _Myfirst = nullptr;
  pointer _Mylast  = nullptr;
  pointer _Myend   = nullptr;
};

template <typename _Elem, typename _Alloc = default_buffer_allocator<_Elem>>
using array_buffer = pod_vector<_Elem, _Alloc>;
} // namespace yasio
#endif
