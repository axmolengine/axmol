//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

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
#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>
#include <exception>
#include <type_traits>
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
template <typename _Elem>
class basic_byte_buffer final {
  static_assert(sizeof(_Elem) == 1, "The basic_byte_buffer only accept type which is char or unsigned char!");

public:
  using pointer       = _Elem*;
  using const_pointer = const _Elem*;
  using size_type     = size_t;
  basic_byte_buffer() {}
  explicit basic_byte_buffer(size_t capacity) { reserve(capacity); }
  basic_byte_buffer(size_t size, _Elem val) { resize(size, val); }
  basic_byte_buffer(const void* first, const void* last) { assign(first, last); }
  basic_byte_buffer(const basic_byte_buffer& rhs) { assign(rhs.begin(), rhs.end()); };
  basic_byte_buffer(basic_byte_buffer&& rhs) noexcept
  {
    memcpy(this, &rhs, sizeof(rhs));
    memset(&rhs, 0, sizeof(rhs));
  }
  basic_byte_buffer(const std::vector<_Elem>& rhs) { assign(rhs.data(), rhs.data() + rhs.size()); }
  ~basic_byte_buffer() { shrink_to_fit(0); }
  basic_byte_buffer& operator=(const basic_byte_buffer& rhs) { return assign(rhs.begin(), rhs.end()); }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) noexcept { return this->swap(rhs); }
  basic_byte_buffer& assign(const void* first, const void* last)
  {
    clear();
    insert(this->end(), first, last);
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
  void insert(_Elem* where, const void* first, const void* last)
  {
    auto count = (const _Elem*)last - (const _Elem*)first;
    if (count > 0)
    {
      auto insertion_pos = where - _Myfirst;
      auto old_size      = _Mylast - _Myfirst;
      resize(old_size + count);
      if (insertion_pos >= old_size)
        memcpy(_Myfirst + old_size, first, count);
      else if (insertion_pos >= 0)
      {
        where        = _Myfirst + insertion_pos;
        auto move_to = where + count;
        memmove(move_to, where, _Mylast - move_to);
        memcpy(where, first, count);
      }
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
    else
      throw std::out_of_range("byte_buffer: out of range!");
  }
  _Elem& back()
  {
    if (!this->empty())
      return *(_Mylast - 1);
    else
      throw std::out_of_range("byte_buffer: out of range!");
  }
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
  void resize(size_t new_size)
  {
    if (this->capacity() < new_size)
      _Reallocate_exactly(new_size * 3 / 2);
    _Mylast = _Myfirst + new_size;
  }
  void resize_fit(size_t new_size)
  {
    if (this->capacity() < new_size)
      _Reallocate_exactly(new_size);
    _Mylast = _Myfirst + new_size;
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
  void attach(void* ptr, size_t len) noexcept
  {
    if (ptr)
    {
      shrink_to_fit(0);
      _Myfirst = (_Elem*)ptr;
      _Myend = _Mylast = _Myfirst + len;
    }
  }
  template <typename _TSIZE>
  _Elem* detach(_TSIZE& len) noexcept
  {
    auto ptr = _Myfirst;
    len      = static_cast<_TSIZE>(this->size());
    memset(this, 0, sizeof(*this));
    return ptr;
  }

private:
  void _Reallocate_exactly(size_t new_cap)
  {
    auto new_block = (_Elem*)realloc(_Myfirst, new_cap);
    if (new_block || 0 == new_cap)
      _Myfirst = new_block;
    else
      throw std::bad_alloc{};
    _Myend = _Myfirst + new_cap;
  }

  _Elem* _Myfirst = nullptr;
  _Elem* _Mylast  = nullptr;
  _Elem* _Myend   = nullptr;
};
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<uint8_t>;
} // namespace yasio
#endif
