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
*/

/* The byte_buffer concepts
   a. The memory model is similar to to std::vector<char>, std::string
   b. use c realloc/free to manage memory
   c. implemented operations:
      - resize(without fill)
      - resize_fit
      - attach/detach(stl not support)
      - stl likes: insert, reserve, front, begin, end, push_back and etc.
*/
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>
#include <exception>

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
  ~basic_byte_buffer() { _Tidy(); }
  basic_byte_buffer& operator=(const basic_byte_buffer& rhs) { return assign(rhs.begin(), rhs.end()); }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) noexcept { return this->swap(rhs); }
  basic_byte_buffer& assign(const void* first, const void* last)
  {
    ptrdiff_t count = (const _Elem*)last - (const _Elem*)first;
    if (count > 0)
      memcpy(resize(count), first, count);
    else
      clear();
    return *this;
  }
  basic_byte_buffer& swap(basic_byte_buffer& rhs) noexcept
  {
    std::swap(_Myfirst, rhs._Myfirst);
    std::swap(_Mylast, rhs._Mylast);
    std::swap(_Myend, rhs._Myend);
    return *this;
  }
  void insert(_Elem* where, const void* first, const void* last)
  {
    ptrdiff_t count = (const _Elem*)last - (const _Elem*)first;
    if (count > 0)
    {
      auto insertion_pos = where - this->begin();
      auto cur_size      = this->size();
      resize(cur_size + count);
      if (insertion_pos >= static_cast<ptrdiff_t>(cur_size))
        memcpy(this->begin() + cur_size, first, count);
      else if (insertion_pos >= 0)
      {
        where        = this->begin() + insertion_pos;
        auto move_to = where + count;
        memmove(move_to, where, this->end() - move_to);
        memcpy(where, first, count);
      }
    }
  }
  void push_back(_Elem v)
  {
    auto cur_size                  = this->size();
    resize(cur_size + 1)[cur_size] = v;
  }
  _Elem& front()
  {
    if (!this->empty())
      return *_Myfirst;
    else
      throw std::out_of_range("byte_buffer: out of range!");
  }
  _Elem* begin() noexcept { return _Myfirst; }
  _Elem* end() noexcept { return _Mylast; }
  const _Elem* begin() const noexcept { return _Myfirst; }
  const _Elem* end() const noexcept { return _Mylast; }
  pointer data() noexcept { return _Myfirst; }
  const_pointer data() const noexcept { return _Myfirst; }
  void reserve(size_t new_cap)
  {
    if (this->capacity() < new_cap)
    {
      auto cur_size = this->size();
      _Reset_cap(new_cap);
      _Mylast = _Myfirst + cur_size;
    }
  }
  _Elem* resize(size_t new_size, _Elem val)
  {
    auto cur_size = this->size();
    auto ptr      = resize(new_size);
    if (cur_size < new_size)
      memset(ptr + cur_size, val, new_size - cur_size);
    return ptr;
  }
  _Elem* resize(size_t new_size)
  {
    _Ensure_cap(new_size * 3 / 2);
    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  _Elem* resize_fit(size_t new_size)
  {
    _Ensure_cap(new_size);
    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  size_t capacity() const noexcept { return _Myend - _Myfirst; }
  size_t size() const noexcept { return _Mylast - _Myfirst; }
  void clear() noexcept { _Mylast = _Myfirst; }
  bool empty() const noexcept { return _Mylast == _Myfirst; }
  void shrink_to_fit() { _Reset_cap(this->size()); }
  void attach(void* ptr, size_t len) noexcept
  {
    if (ptr)
    {
      _Tidy();
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
  void _Tidy()
  {
    clear();
    shrink_to_fit();
  }
  void _Ensure_cap(size_t new_cap)
  {
    if (this->capacity() < new_cap)
      _Reset_cap(new_cap);
  }
  void _Reset_cap(size_t new_cap)
  {
    if (new_cap > 0)
    {
      auto new_blk = (_Elem*)realloc(_Myfirst, new_cap);
      if (new_blk)
      {
        _Myfirst = new_blk;
        _Myend   = _Myfirst + new_cap;
      }
      else
        throw std::bad_alloc{};
    }
    else
    {
      if (_Myfirst != nullptr)
      {
        free(_Myfirst);
        _Myfirst = nullptr;
      }
      _Myend = _Myfirst;
    }
  }

private:
  _Elem* _Myfirst = nullptr;
  _Elem* _Mylast  = nullptr;
  _Elem* _Myend   = nullptr;
};
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<uint8_t>;
} // namespace yasio
