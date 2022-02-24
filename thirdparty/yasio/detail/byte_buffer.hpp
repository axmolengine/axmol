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
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
struct default_allocator {
  static void* reallocate(void* old_block, size_t /*old_size*/, size_t new_size) { return ::realloc(old_block, new_size); }
};
template <typename _Elem, typename _Alloc = default_allocator> class basic_byte_buffer final {
  static_assert(std::is_same<_Elem, char>::value || std::is_same<_Elem, unsigned char>::value,
                "The basic_byte_buffer only accept type which is char or unsigned char!");
public:
  using pointer       = _Elem*;
  using const_pointer = const _Elem*;
  using size_type     = size_t;
  using value_type    = _Elem;
  basic_byte_buffer() {}
  explicit basic_byte_buffer(size_t count) { resize(count); }
  basic_byte_buffer(size_t count, std::true_type /*fit*/) { resize_fit(count); }
  basic_byte_buffer(size_t count, _Elem val) { resize(count, val); }
  basic_byte_buffer(size_t count, _Elem val, std::true_type /*fit*/) { resize_fit(count, val); }
  template <typename _Iter> basic_byte_buffer(_Iter first, _Iter last) { _Assign_range(first, last); }
  template <typename _Iter> basic_byte_buffer(_Iter first, _Iter last, std::true_type /*fit*/) { _Assign_range(first, last, std::true_type{}); }
  basic_byte_buffer(const basic_byte_buffer& rhs) { _Assign_range(rhs.begin(), rhs.end()); };
  basic_byte_buffer(const basic_byte_buffer& rhs, std::true_type /*fit*/) { _Assign_range(rhs.begin(), rhs.end(), std::true_type{}); };
  basic_byte_buffer(basic_byte_buffer&& rhs) noexcept { _Assign_rv(std::move(rhs)); }
  ~basic_byte_buffer() { shrink_to_fit(0); }
  basic_byte_buffer& operator=(const basic_byte_buffer& rhs) { return assign(rhs.begin(), rhs.end()); }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) noexcept { return this->swap(rhs); }
  template <typename _Iter> basic_byte_buffer& assign(const _Iter first, const _Iter last)
  {
    clear();
    _Assign_range(first, last);
    return *this;
  }
  basic_byte_buffer& swap(basic_byte_buffer& rhs) noexcept
  {
    char _Tmp[sizeof(rhs)];
    memcpy(_Tmp, &rhs, sizeof(rhs));
    memcpy(&rhs, this, sizeof(rhs));
    memcpy(this, _Tmp, sizeof(_Tmp));
    return *this;
  }
  template <typename _Iter> void insert(_Elem* where, _Iter first, const _Iter last)
  {
    if (where == _Mylast)
      append(first, last);
    else
    {
      auto count         = std::distance(first, last);
      auto insertion_pos = where - _Myfirst;
      if (count > 0 && insertion_pos >= 0)
      {
        auto old_size = _Mylast - _Myfirst;
        resize(old_size + count);
        where        = _Myfirst + insertion_pos;
        auto move_to = where + count;
        std::copy_n(where, _Mylast - move_to, move_to);
        std::copy_n(first, count, where);
      }
    }
  }
  template <typename _Iter> void append(_Iter first, const _Iter last) { append_n(first, std::distance(first, last)); }
  template <typename _Iter> void append_n(_Iter first, ptrdiff_t count)
  {
    if (count > 0)
    {
      auto old_size = _Mylast - _Myfirst;
      resize(old_size + count);
      std::copy_n(first, count, _Myfirst + old_size);
    }
  }
  void push_back(_Elem v)
  {
    resize(this->size() + 1);
    *(_Mylast - 1) = v;
  }
  _Elem& front()
  {
    if (!this->empty())
      return *_Myfirst;
    throw std::out_of_range("byte_buffer: out of range!");
  }
  _Elem& back()
  {
    if (!this->empty())
      return *(_Mylast - 1);
    throw std::out_of_range("byte_buffer: out of range!");
  }
  static constexpr size_t max_size() noexcept { return (std::numeric_limits<ptrdiff_t>::max)(); }
  _Elem* begin() noexcept { return _Myfirst; }
  _Elem* end() noexcept { return _Mylast; }
  const _Elem* begin() const noexcept { return _Myfirst; }
  const _Elem* end() const noexcept { return _Mylast; }
  pointer data() noexcept { return _Myfirst; }
  const_pointer data() const noexcept { return _Myfirst; }
  size_t capacity() const noexcept { return _Myend - _Myfirst; }
  size_t size() const noexcept { return _Mylast - _Myfirst; }
  void clear() noexcept { _Mylast = _Myfirst; }
  void shrink_to_fit() { shrink_to_fit(this->size()); }
  bool empty() const noexcept { return _Mylast == _Myfirst; }
  void resize(size_t new_size, _Elem val)
  {
    auto old_size = this->size();
    resize(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  _Elem* resize(size_t new_size)
  {
    auto old_cap = this->capacity();
    if (old_cap < new_size)
      _Reallocate_exactly((std::max)(old_cap + old_cap / 2, new_size));
    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  void resize_fit(size_t new_size, _Elem val)
  {
    auto old_size = this->size();
    resize_fit(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  _Elem* resize_fit(size_t new_size)
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
  const _Elem& operator[](size_t index) const
  {
    if (index < this->size())
      return _Myfirst[index];
    throw std::out_of_range("byte_buffer: out of range!");
  }
  _Elem& operator[](size_t index)
  {
    if (index < this->size())
      return _Myfirst[index];
    throw std::out_of_range("byte_buffer: out of range!");
  }
  void attach(void* ptr, size_t len) noexcept
  {
    if (ptr)
    {
      shrink_to_fit(0);
      _Myfirst = (_Elem*)ptr;
      _Myend = _Mylast = _Myfirst + len;
    }
  }
  template <typename _TSIZE> _Elem* detach(_TSIZE& len) noexcept
  {
    auto ptr = _Myfirst;
    len      = static_cast<_TSIZE>(this->size());
    memset(this, 0, sizeof(*this));
    return ptr;
  }

private:
  template <typename _Iter> void _Assign_range(_Iter first, _Iter last)
  {
    if (last > first)
      std::copy(first, last, resize(std::distance(first, last)));
  }
  template <typename _Iter> void _Assign_range(_Iter first, _Iter last, std::true_type)
  {
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
    auto new_block = (_Elem*)_Alloc::reallocate(_Myfirst, _Myend - _Myfirst, new_cap);
    if (new_block || 0 == new_cap)
    {
      _Myfirst = new_block;
      _Myend   = _Myfirst + new_cap;
    }
    else
      throw std::bad_alloc{};
  }
  _Elem* _Myfirst = nullptr;
  _Elem* _Mylast  = nullptr;
  _Elem* _Myend   = nullptr;
};
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<unsigned char>;
} // namespace yasio
#endif
