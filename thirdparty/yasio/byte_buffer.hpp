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

Version: 4.2.0

The byte_buffer concepts:
   a. The memory model is similar to to std::vector<char>, std::string
   b. Support resize fit
   c. By default resize without fill (uninitialized and for overwrite)
   d. Support release internal buffer ownership with `release_pointer`
*/
#ifndef YASIO__BYTE_BUFFER_HPP
#define YASIO__BYTE_BUFFER_HPP
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
template <typename _Elem, enable_if_t<is_byte_type<_Elem>::value, int> = 0>
using default_byte_allocater = default_buffer_allocator<_Elem>;

template <typename _Elem, typename _Alloc = default_byte_allocater<_Elem>>
class basic_byte_buffer {
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
  explicit basic_byte_buffer(size_type count) { resize(count); }
  basic_byte_buffer(size_type count, std::true_type /*fit*/) { resize_fit(count); }
  basic_byte_buffer(size_type count, const_reference val) { resize(count, val); }
  basic_byte_buffer(size_type count, const_reference val, std::true_type /*fit*/) { resize_fit(count, val); }
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
  basic_byte_buffer(const basic_byte_buffer& rhs, std::true_type /*fit*/) { assign(rhs, std::true_type{}); }
  basic_byte_buffer(basic_byte_buffer&& rhs) YASIO__NOEXCEPT { assign(std::move(rhs)); }
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
  ~basic_byte_buffer() { _Tidy(); }
  basic_byte_buffer& operator=(const basic_byte_buffer& rhs)
  {
    assign(rhs);
    return *this;
  }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) YASIO__NOEXCEPT
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
  void assign(const basic_byte_buffer& rhs, std::true_type) { _Assign_range(rhs.begin(), rhs.end(), std::true_type{}); }
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
  void swap(basic_byte_buffer& rhs) YASIO__NOEXCEPT
  {
    std::swap(_Myfirst, rhs._Myfirst);
    std::swap(_Mylast, rhs._Mylast);
    std::swap(_Myend, rhs._Myend);
  }
  template <typename _Iter>
  iterator insert(iterator _Where, _Iter first, const _Iter last)
  {
    _YASIO_VERIFY_RANGE(_Where >= _Myfirst && _Where <= _Mylast && first <= last, "byte_buffer: out of range!");
    if (first != last)
    {
      auto ifirst        = std::addressof(*first);
      auto count         = std::distance(first, last) * sizeof(*ifirst);
      auto insertion_pos = std::distance(_Myfirst, _Where);
      if (_Where == _Mylast)
      {
        if (count > 1)
        {
          auto old_size = _Mylast - _Myfirst;
          resize(old_size + count);
          std::copy_n((iterator)ifirst, count, _Myfirst + old_size);
        }
        else if (count == 1)
          push_back(static_cast<value_type>(*(iterator)ifirst));
      }
      else
      {
        if (insertion_pos >= 0)
        {
          auto old_size = _Mylast - _Myfirst;
          resize(old_size + count);
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
    _YASIO_VERIFY_RANGE((first <= last) && first >= _Myfirst && last <= _Mylast, "byte_buffer: out of range!");
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
  static YASIO__CONSTEXPR size_type max_size() YASIO__NOEXCEPT { return (std::numeric_limits<ptrdiff_t>::max)(); }
  iterator begin() YASIO__NOEXCEPT { return _Myfirst; }
  iterator end() YASIO__NOEXCEPT { return _Mylast; }
  const_iterator begin() const YASIO__NOEXCEPT { return _Myfirst; }
  const_iterator end() const YASIO__NOEXCEPT { return _Mylast; }
  pointer data() YASIO__NOEXCEPT { return _Myfirst; }
  const_pointer data() const YASIO__NOEXCEPT { return _Myfirst; }
  size_type capacity() const YASIO__NOEXCEPT { return static_cast<size_type>(_Myend - _Myfirst); }
  size_type size() const YASIO__NOEXCEPT { return static_cast<size_type>(_Mylast - _Myfirst); }
  void clear() YASIO__NOEXCEPT { _Mylast = _Myfirst; }
  bool empty() const YASIO__NOEXCEPT { return _Mylast == _Myfirst; }

  const_reference operator[](size_type index) const { return this->at(index); }
  reference operator[](size_type index) { return this->at(index); }
  const_reference at(size_type index) const
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "byte_buffer: out of range!");
    return _Myfirst[index];
  }
  reference at(size_type index)
  {
    _YASIO_VERIFY_RANGE(index < this->size(), "byte_buffer: out of range!");
    return _Myfirst[index];
  }
  void resize(size_type new_size, const_reference val)
  {
    auto old_size = this->size();
    resize(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  void resize(size_type new_size)
  {
    auto old_cap = this->capacity();
    if (old_cap < new_size)
      _Resize_uninitialized(new_size, _Calculate_growth(new_size));
    else
      _Mylast = _Myfirst + new_size;
  }
  void resize_fit(size_type new_size, const_reference val)
  {
    auto old_size = this->size();
    resize_fit(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
  }
  void resize_fit(size_type new_size)
  {
    if (this->capacity() < new_size)
      _Resize_uninitialized(new_size, new_size);
    else
      _Mylast = _Myfirst + new_size;
  }
  void reserve(size_type new_cap)
  {
    if (this->capacity() < new_cap)
      _Resize_uninitialized(this->size(), new_cap);
  }
  void shrink_to_fit()
  { // reduce capacity to size, provide strong guarantee
    if (_Myfirst)
    {
      const auto _Count = this->size();
      _Resize_uninitialized(_Count, _Count);
    }
  }
  /** detach internal buffer ownership
   * Note: this is a unsafe operation, after take the internal buffer, you are responsible for
   * destroy it once you don't need it, i.e:
   *   yasio::byte_buffer buf;
   *   buf.push_back('I');
   *   auto rawbufCapacity = buf.capacity();
   *   auto rawbufLen = buf.size();
   *   auto rawbuf = buf.release_pointer();
   *   // use rawbuf to do something
   *   // ...
   *   // done, destroy the memory
   *   yasio::byte_buffer::allocator_type::deallocate(rawbuf, rawbufCapacity);
   *
   */
  template <typename _Intty>
  pointer detach(_Intty& len)
  {
    len      = static_cast<_Intty>(this->size());
    auto ptr = _Myfirst;
    _Myfirst = nullptr;
    _Mylast  = nullptr;
    _Myend   = nullptr;
    return ptr;
  }
  void attach(pointer ptr, size_t len)
  {
    _Tidy();
    _Myfirst = ptr;
    _Myend = _Mylast = ptr + len;
  }
  pointer release_pointer() YASIO__NOEXCEPT
  {
    size_t ignored_len;
    return this->detach(ignored_len);
  }

private:
  template <typename _Iter>
  void _Assign_range(_Iter first, _Iter last)
  {
    _Mylast = _Myfirst;
    if (last > first)
    {
      auto ifirst           = std::addressof(*first);
      const auto size_bytes = std::distance(first, last) * sizeof(*ifirst);
      resize(size_bytes);
      std::copy_n((iterator)ifirst, size_bytes, _Myfirst);
    }
  }
  template <typename _Iter>
  void _Assign_range(_Iter first, _Iter last, std::true_type)
  {
    _Mylast = _Myfirst;
    if (last > first)
    {
      auto ifirst           = std::addressof(*first);
      const auto size_bytes = std::distance(first, last) * sizeof(*ifirst);
      resize_fit(size_bytes);
      std::copy_n((iterator)ifirst, size_bytes, _Myfirst);
    }
  }
  void _Assign_rv(basic_byte_buffer&& rhs)
  {
    memcpy(this, &rhs, sizeof(rhs));
    memset(&rhs, 0, sizeof(rhs));
  }
  void _Resize_uninitialized(size_type new_size, size_type new_cap)
  {
    _Myfirst = _Alloc::reallocate(_Myfirst, static_cast<size_type>(_Myend - _Myfirst), new_cap);
    if (_Myfirst || !new_cap)
    {
      _Mylast = _Myfirst + new_size;
      _Myend  = _Myfirst + new_cap;
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
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<unsigned char>;
} // namespace yasio
#endif
