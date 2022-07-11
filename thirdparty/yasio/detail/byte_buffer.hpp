//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2022 HALX99

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

The byte_buffer concepts:
   a. The memory model is similar to to std::vector<char>, std::string
   b. Use c realloc to manage memory
   c. Implemented operations: resize(without fill), resize_fit, attach/detach(stl not support)
*/
#ifndef YASIO__BYTE_BUFFER_HPP
#define YASIO__BYTE_BUFFER_HPP
#include <stddef.h>
#include <string.h>
#include <utility>
#include <memory>
#include <iterator>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include <initializer_list>
#include <type_traits>
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
#define _YASIO_VERIFY_RANGE(cond, mesg)                 \
  do                                                    \
  {                                                     \
    if (cond)                                           \
      ; /* contextually convertible to bool paranoia */ \
    else                                                \
    {                                                   \
      throw std::out_of_range(mesg);                    \
    }                                                   \
                                                        \
  } while (false)

template <bool _Test, class _Ty = void>
using enable_if_t = typename ::std::enable_if<_Test, _Ty>::type;

struct default_allocator {
  static void* reallocate(void* old_block, size_t /*old_size*/, size_t new_size)
  {
    return ::realloc(old_block, new_size);
  }
};
template <typename _Elem, typename _Alloc = default_allocator>
class basic_byte_buffer {
  static_assert(std::is_same<_Elem, char>::value || std::is_same<_Elem, unsigned char>::value,
                "The basic_byte_buffer only accept type which is char or unsigned char!");

public:
  using pointer         = _Elem*;
  using const_pointer   = const _Elem*;
  using reference       = _Elem&;
  using const_reference = const _Elem&;
  using size_type       = size_t;
  using value_type      = _Elem;
  using iterator        = _Elem*; // byte_buffer only needs transparent iterator
  using const_iterator  = const _Elem*;
  using allocator_type  = _Alloc;
  basic_byte_buffer() {}
  explicit basic_byte_buffer(size_t count) { resize(count); }
  basic_byte_buffer(size_t count, std::true_type /*fit*/) { resize_fit(count); }
  basic_byte_buffer(size_t count, const_reference val) { resize(count, val); }
  basic_byte_buffer(size_t count, const_reference val, std::true_type /*fit*/)
  {
    resize_fit(count, val);
  }
  template <typename _Iter>
  basic_byte_buffer(_Iter first, _Iter last)
  {
    assign(first, last);
  }
  template <typename _Iter>
  basic_byte_buffer(_Iter first, _Iter last, std::true_type /*fit*/)
  {
    assign(first, last, std::true_type{});
  }
  basic_byte_buffer(const basic_byte_buffer& rhs) { assign(rhs); };
  basic_byte_buffer(const basic_byte_buffer& rhs, std::true_type /*fit*/)
  {
    assign(rhs, std::true_type{});
  }
  basic_byte_buffer(basic_byte_buffer&& rhs) noexcept { assign(std::move(rhs)); }
  template <typename _Ty, enable_if_t<std::is_integral<_Ty>::value, int> = 0>
  basic_byte_buffer(std::initializer_list<_Ty> rhs)
  {
    assign(rhs);
  }
  template <typename _Ty, enable_if_t<std::is_integral<_Ty>::value, int> = 0>
  basic_byte_buffer(std::initializer_list<_Ty> rhs, std::true_type /*fit*/)
  {
    assign(rhs, std::true_type{});
  }
  ~basic_byte_buffer() { shrink_to_fit(0); }
  basic_byte_buffer& operator=(const basic_byte_buffer& rhs)
  {
    assign(rhs);
    return *this;
  }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) noexcept
  {
    this->swap(rhs);
    return *this;
  }
  template <typename _Cont>
  basic_byte_buffer& operator+=(const _Cont& rhs)
  {
    return this->append(std::begin(rhs), std::end(rhs));
  }
  basic_byte_buffer& operator+=(const_reference rhs)
  {
    this->push_back(rhs);
    return *this;
  }
  template <typename _Iter>
  void assign(const _Iter first, const _Iter last)
  {
    _Assign_range(first, last);
  }
  template <typename _Iter>
  void assign(const _Iter first, const _Iter last, std::true_type /*fit*/)
  {
    _Assign_range(first, last, std::true_type{});
  }
  void assign(const basic_byte_buffer& rhs) { _Assign_range(rhs.begin(), rhs.end()); }
  void assign(const basic_byte_buffer& rhs, std::true_type)
  {
    _Assign_range(rhs.begin(), rhs.end(), std::true_type{});
  }
  void assign(basic_byte_buffer&& rhs) { _Assign_rv(std::move(rhs)); }
  template <typename _Ty, enable_if_t<std::is_integral<_Ty>::value, int> = 0>
  void assign(std::initializer_list<_Ty> rhs)
  {
    _Assign_range((iterator)rhs.begin(), (iterator)rhs.end());
  }
  template <typename _Ty, enable_if_t<std::is_integral<_Ty>::value, int> = 0>
  void assign(std::initializer_list<_Ty> rhs, std::true_type /*fit*/)
  {
    _Assign_range((iterator)rhs.begin(), (iterator)rhs.end(), std::true_type{});
  }
  void swap(basic_byte_buffer& rhs) noexcept
  {
    char _Tmp[sizeof(rhs)];
    memcpy(_Tmp, &rhs, sizeof(rhs));
    memcpy(&rhs, this, sizeof(rhs));
    memcpy(this, _Tmp, sizeof(_Tmp));
  }
  template <typename _Iter>
  iterator insert(iterator _Where, _Iter first, const _Iter last)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where <= _Mylast && first <= last,
                        "byte_buffer: out of range!");
    auto ifirst = (iterator)std::addressof(*first);
    auto ilast  = (iterator)std::addressof(*last);
    auto count  = std::distance(ifirst, ilast);
    if (count > 0)
    {
      auto insertion_pos = std::distance(_Myfirst, _Where);
      if (_Where == _Mylast)
      {
        if (count > 1)
        {
          auto old_size = _Mylast - _Myfirst;
          std::copy_n(ifirst, count, resize(old_size + count) + old_size);
        }
        else if (count == 1)
          push_back(static_cast<value_type>(*ifirst));
      }
      else
      {
        if (insertion_pos >= 0)
        {
          auto old_size = _Mylast - _Myfirst;
          _Where        = resize(old_size + count) + insertion_pos;
          auto move_to  = _Where + count;
          std::copy_n(_Where, _Mylast - move_to, move_to);
          std::copy_n(ifirst, count, _Where);
        }
      }
      return _Myfirst + insertion_pos;
    }
    return _Where;
  }
  template <typename _Iter>
  basic_byte_buffer& append(_Iter first, const _Iter last)
  {
    insert(end(), first, last);
    return *this;
  }
  void push_back(value_type v)
  {
    resize(this->size() + 1);
    *(_Mylast - 1) = v;
  }
  iterator erase(iterator _Where)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where < _Mylast, "byte_buffer: out of range!");
    _Mylast = std::move(_Where + 1, _Mylast, _Where);
    return _Where;
  }
  iterator erase(iterator first, iterator last)
  {
    _YASIO_VERIFY_RANGE((first <= last) && first >= _Myfirst && last <= _Mylast,
                        "byte_buffer: out of range!");
    _Mylast = std::move(last, _Mylast, first);
    return first;
  }
  value_type& front()
  {
    _YASIO_VERIFY_RANGE(_Myfirst < _Mylast, "byte_buffer: out of range!");
    return *_Myfirst;
  }
  value_type& back()
  {
    _YASIO_VERIFY_RANGE(_Myfirst < _Mylast, "byte_buffer: out of range!");
    return *(_Mylast - 1);
  }
  static constexpr size_t max_size() noexcept { return (std::numeric_limits<ptrdiff_t>::max)(); }
  iterator begin() noexcept { return _Myfirst; }
  iterator end() noexcept { return _Mylast; }
  const_iterator begin() const noexcept { return _Myfirst; }
  const_iterator end() const noexcept { return _Mylast; }
  pointer data() noexcept { return _Myfirst; }
  const_pointer data() const noexcept { return _Myfirst; }
  size_t capacity() const noexcept { return _Myend - _Myfirst; }
  size_t size() const noexcept { return _Mylast - _Myfirst; }
  void clear() noexcept { _Mylast = _Myfirst; }
  void shrink_to_fit() { shrink_to_fit(this->size()); }
  bool empty() const noexcept { return _Mylast == _Myfirst; }
  void resize(size_t new_size, const_reference val)
  {
    auto old_size = this->size();
    resize(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  pointer resize(size_t new_size)
  {
    auto old_cap = this->capacity();
    if (old_cap < new_size)
      _Reallocate_exactly((std::max)(old_cap + old_cap / 2, new_size));
    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  void resize_fit(size_t new_size, const_reference val)
  {
    auto old_size = this->size();
    resize_fit(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  pointer resize_fit(size_t new_size)
  {
    if (this->capacity() < new_size)
      _Reallocate_exactly(new_size);
    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  void shrink_to_fit(size_t new_size)
  {
    if (this->capacity() != new_size)
      _Reallocate_exactly(new_size);
    _Mylast = _Myfirst + new_size;
  }
  void reserve(size_t new_cap)
  {
    if (this->capacity() < new_cap)
    {
      auto cur_size = this->size();
      _Reallocate_exactly(new_cap);
      _Mylast = _Myfirst + cur_size;
    }
  }
  const_reference operator[](size_t index) const { return this->at(index); }
  reference operator[](size_t index) { return this->at(index); }
  const_reference at(size_t index) const
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "byte_buffer: out of range!");
    return _Myfirst[index];
  }
  reference at(size_t index)
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "byte_buffer: out of range!");
    return _Myfirst[index];
  }
  void attach(void* ptr, size_t len) noexcept
  {
    if (ptr)
    {
      shrink_to_fit(0);
      _Myfirst = (pointer)ptr;
      _Myend = _Mylast = _Myfirst + len;
    }
  }
  template <typename _TSIZE>
  pointer detach(_TSIZE& len) noexcept
  {
    auto ptr = _Myfirst;
    len      = static_cast<_TSIZE>(this->size());
    memset(this, 0, sizeof(*this));
    return ptr;
  }

private:
  template <typename _Iter>
  void _Assign_range(_Iter first, _Iter last)
  {
    _Mylast = _Myfirst;
    if (last > first)
    {
      auto ifirst = (iterator)std::addressof(*first);
      auto ilast  = (iterator)std::addressof(*last);
      std::copy(ifirst, ilast, resize(std::distance(ifirst, ilast)));
    }
  }
  template <typename _Iter>
  void _Assign_range(_Iter first, _Iter last, std::true_type)
  {
    _Mylast = _Myfirst;
    if (last > first)
      std::copy(first, last, resize_fit(std::distance(first, last)));
  }
  void _Assign_rv(basic_byte_buffer&& rhs)
  {
    memcpy(this, &rhs, sizeof(rhs));
    memset(&rhs, 0, sizeof(rhs));
  }
  void _Reallocate_exactly(size_t new_cap)
  {
    auto new_block = (pointer)_Alloc::reallocate(_Myfirst, _Myend - _Myfirst, new_cap);
    if (new_block || 0 == new_cap)
    {
      _Myfirst = new_block;
      _Myend   = _Myfirst + new_cap;
    }
    else
      throw std::bad_alloc{};
  }
  pointer _Myfirst = nullptr;
  pointer _Mylast  = nullptr;
  pointer _Myend   = nullptr;
};
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<unsigned char>;
} // namespace yasio
#endif
