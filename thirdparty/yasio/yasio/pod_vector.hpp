//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2024 HALX99

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

Version: 4.1.1

The pod_vector aka array_buffer concepts:
   a. The memory model is similar to to std::vector, but only accept trivially_copyable(no destructor & no custom copy constructor) types
   b. The resize behavior differrent stl, always allocate exactly
   c. By default resize without fill (uninitialized and for overwrite),
      use insert/append insetad if you want fill memory inside container
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
#include "yasio/buffer_alloc.hpp"
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
template <typename _Ty, typename _Alloc = buffer_allocator<_Ty>>
class pod_vector {
public:
  using pointer         = _Ty*;
  using const_pointer   = const _Ty*;
  using reference       = _Ty&;
  using const_reference = const _Ty&;
  using _Alloc_traits   = buffer_allocator_traits<_Alloc>;
  using size_type       = typename _Alloc_traits::size_type;
  using value_type      = _Ty;
  using iterator        = _Ty*; // transparent iterator
  using const_iterator  = const _Ty*;
  using allocator_type  = _Alloc;
  pod_vector() {}
  explicit pod_vector(size_type count) { resize(static_cast<size_type>(count)); }
  pod_vector(size_type count, const_reference val) { resize(static_cast<size_type>(count), val); }
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
    std::swap(_Mysize, rhs._Mysize);
    std::swap(_Myres, rhs._Myres);
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  iterator insert(iterator pos, _Iter first, _Iter last)
  {
    auto mlast = _Myfirst + _Mysize;
    _YASIO_VERIFY_RANGE(pos >= _Myfirst && pos <= mlast && first <= last, "pod_vector: out of range!");
    if (first != last)
    {
      auto insertion_off = static_cast<size_type>(std::distance(_Myfirst, pos));
      if (pos == mlast)
        append(first, last);
      else
      {
        auto ifirst = std::addressof(*first);
        static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
        auto count = static_cast<size_type>(std::distance(first, last));
        if (insertion_off >= 0)
        {
          expand(count);
          pos          = _Myfirst + insertion_off;
          mlast      = _Myfirst + _Mysize;
          auto move_to = pos + count;
          std::copy_n(pos, mlast - move_to, move_to);
          std::copy_n((iterator)ifirst, count, pos);
        }
      }
      return _Myfirst + insertion_off;
    }
    return pos;
  }
  iterator insert(iterator pos, size_type count, const_reference val)
  {
    auto mlast = _Myfirst + _Mysize;
    _YASIO_VERIFY_RANGE(pos >= _Myfirst && pos <= mlast, "pod_vector: out of range!");
    if (count)
    {
      auto insertion_off = std::distance(_Myfirst, pos);
      if (pos == mlast)
        append(count, val);
      else
      {
        if (insertion_off >= 0)
        {
          const auto old_size = _Mysize;
          expand(count);
          pos          = _Myfirst + insertion_off;
          mlast      = _Myfirst + _Mysize;
          auto move_to = pos + count;
          std::copy_n(pos, mlast - move_to, move_to);
          std::fill_n(pos, count, val);
        }
      }
      return _Myfirst + insertion_off;
    }
    return pos;
  }
  iterator insert(iterator pos, const value_type& val)
  { // insert val at pos
    return emplace(pos, val);
  }
  iterator insert(iterator pos, value_type&& val)
  { // insert by moving val at pos
    return emplace(pos, std::move(val));
  }
  template <typename... _Valty>
  iterator emplace(iterator pos, _Valty&&... val)
  {
    auto insertion_off = std::distance(_Myfirst, pos);
    _YASIO_VERIFY_RANGE(insertion_off <= _Mysize, "pod_vector: out of range!");
#if YASIO__HAS_CXX20
    emplace_back(std::forward<_Valty>(val)...);
    std::rotate(begin() + insertion_off, end() - 1, end());
    return (begin() + insertion_off);
#else
    auto mlast = _Myfirst + _Mysize;
    if (pos == mlast)
      emplace_back(std::forward<_Valty>(val)...);
    else
    {
      if (insertion_off >= 0)
      {
        expand(1);
        pos          = _Myfirst + insertion_off;
        mlast        = _Myfirst + _Mysize;
        auto move_to = pos + 1;
        std::copy_n(pos, mlast - move_to, move_to);
        ::yasio::construct_at(pos, std::forward<_Valty>(val)...);
      }
    }
    return _Myfirst + insertion_off;
#endif
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  pod_vector& append(_Iter first, const _Iter last)
  {
    if (first != last)
    {
      auto ifirst = std::addressof(*first);
      static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
      auto count = static_cast<size_type>(std::distance(first, last));
      if (count > 1)
      {
        const auto old_size = _Mysize;
        expand(count);
        std::copy_n((iterator)ifirst, count, _Myfirst + old_size);
      }
      else if (count == 1)
        push_back(static_cast<value_type>(*(iterator)ifirst));
    }
    return *this;
  }
  pod_vector& append(size_type count, const_reference val)
  {
    expand(count, val);
    return *this;
  }
  void push_back(value_type&& val) { push_back(val); }
  void push_back(const value_type& val) { emplace_back(val); }
  template <typename... _Valty>
  inline value_type& emplace_back(_Valty&&... val)
  {
    if (_Mysize < _Myres)
      return *::yasio::construct_at(_Myfirst + _Mysize++, std::forward<_Valty>(val)...);
    return *_Emplace_back_reallocate(std::forward<_Valty>(val)...);
  }
  iterator erase(iterator pos)
  {
    const auto mlast = _Myfirst + _Mysize;
    _YASIO_VERIFY_RANGE(pos >= _Myfirst && pos < mlast, "pod_vector: out of range!");
    _Mysize = static_cast<size_type>(std::move(pos + 1, mlast, pos) - _Myfirst);
    return pos;
  }
  iterator erase(iterator first, iterator last)
  {
    const auto mlast = _Myfirst + _Mysize;
    _YASIO_VERIFY_RANGE((first <= last) && first >= _Myfirst && last <= mlast, "pod_vector: out of range!");
    _Mysize = static_cast<size_type>(std::move(last, mlast, first) - _Myfirst);
    return first;
  }
  value_type& front()
  {
    _YASIO_VERIFY_RANGE(!empty(), "pod_vector: out of range!");
    return *_Myfirst;
  }
  value_type& back()
  {
    _YASIO_VERIFY_RANGE(!empty(), "pod_vector: out of range!");
    return _Myfirst[_Mysize - 1];
  }
  static YASIO__CONSTEXPR size_type max_size() YASIO__NOEXCEPT { return _Alloc_traits::max_size(); }
  iterator begin() YASIO__NOEXCEPT { return _Myfirst; }
  iterator end() YASIO__NOEXCEPT { return _Myfirst + _Mysize; }
  const_iterator begin() const YASIO__NOEXCEPT { return _Myfirst; }
  const_iterator end() const YASIO__NOEXCEPT { return _Myfirst + _Mysize; }
  pointer data() YASIO__NOEXCEPT { return _Myfirst; }
  const_pointer data() const YASIO__NOEXCEPT { return _Myfirst; }
  size_type capacity() const YASIO__NOEXCEPT { return _Myres; }
  size_type size() const YASIO__NOEXCEPT { return _Mysize; }
  size_type length() const YASIO__NOEXCEPT { return _Mysize; }
  void clear() YASIO__NOEXCEPT { _Mysize = 0; }
  bool empty() const YASIO__NOEXCEPT { return _Mysize == 0; }

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
    if (_Mysize != _Myres)
    { // something to do
      if (!_Mysize)
        _Tidy();
      else
        _Reallocate<_Reallocation_policy::_Exactly>(_Mysize);
    }
  }
  void reserve(size_type new_cap)
  {
    if (this->capacity() < new_cap)
      _Reallocate<_Reallocation_policy::_Exactly>(new_cap);
  }
  template <typename _Operation>
  void resize_and_overwrite(const size_type new_size, _Operation op)
  {
    _Reallocate<_Reallocation_policy::_Exactly>(new_size);
    _Eos(std::move(op)(_Myfirst, new_size));
  }
#pragma endregion
  void resize(size_type new_size, const_reference val)
  {
    auto old_size = this->size();
    if (old_size != new_size)
    {
      resize(new_size);
      if (old_size < new_size)
        std::fill_n(_Myfirst + old_size, new_size - old_size, val);
    }
  }
  void expand(size_type count, const_reference val)
  {
    if (count)
    {
      auto old_size = this->size();
      expand(count);
      if (count)
        std::fill_n(_Myfirst + old_size, count, val);
    }
  }
  ptrdiff_t index_of(const_reference val) const YASIO__NOEXCEPT
  {
    auto it = std::find(begin(), end(), val);
    if (it != this->end())
      return std::distance(begin(), it);
    return -1;
  }
  void reset(size_type new_size)
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
    _Mysize = _Myres = 0;
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
    _Myfirst = ptr;
    _Mysize = _Myres = len;
  }
  pointer release_pointer() YASIO__NOEXCEPT { return detach_abi(); }

private:
  void _Eos(size_type size) YASIO__NOEXCEPT { _Mysize = size; }
  template <typename... _Valty>
  pointer _Emplace_back_reallocate(_Valty&&... val)
  {
    const auto _Oldsize = _Mysize;

    if (_Oldsize == max_size())
      throw std::length_error("pod_vector too long");

    const size_type _Newsize = _Oldsize + 1;
    _Resize_reallocate<_Reallocation_policy::_At_least>(_Newsize);
    const pointer _Newptr = ::yasio::construct_at(_Myfirst + _Oldsize, std::forward<_Valty>(val)...);
    return _Newptr;
  }
  template <typename _Iter, ::yasio::enable_if_t<::yasio::is_iterator<_Iter>::value, int> = 0>
  void _Assign_range(_Iter first, _Iter last)
  {
    auto ifirst = std::addressof(*first);
    static_assert(sizeof(*ifirst) == sizeof(value_type), "pod_vector: iterator type incompatible!");
    if (ifirst != _Myfirst)
    {
      _Mysize = 0;
      if (last > first)
      {
        const auto count = static_cast<size_type>(std::distance(first, last));
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
    size_type new_cap;
    if YASIO__CONSTEXPR (_Policy == _Reallocation_policy::_Exactly)
      new_cap = size;
    else
      new_cap = _Calculate_growth(size);
    auto _Newvec = _Alloc::reallocate(_Myfirst, _Myres, new_cap);
    if (_Newvec)
    {
      _Myfirst = _Newvec;
      _Myres   = new_cap;
    }
    else
      throw std::bad_alloc{};
  }
  size_type _Calculate_growth(const size_type new_size) const
  {
    // given _Oldcapacity and _Newsize, calculate geometric growth
    const size_type old_cap = capacity();
    YASIO__CONSTEXPR auto max_cap   = max_size();

    if (old_cap > max_cap - old_cap / 2)
      return max_cap; // geometric growth would overflow

    const size_type geometric = old_cap + (old_cap >> 1);

    if (geometric < new_size)
      return new_size; // geometric growth would be insufficient

    return geometric; // geometric growth is sufficient
  }
  void _Tidy() YASIO__NOEXCEPT
  { // free all storage
    if (_Myfirst)
    {
      _Alloc::deallocate(_Myfirst, _Myres);
      _Myfirst = nullptr;
      _Mysize = _Myres = 0;
    }
  }

  pointer _Myfirst  = nullptr;
  size_type _Mysize = 0;
  size_type _Myres  = 0;
};

#pragma region c++20 like std::erase
template <typename _Ty, typename _Alloc>
void erase(pod_vector<_Ty, _Alloc>& cont, const _Ty& val)
{
  cont.erase(std::remove(cont.begin(), cont.end(), val), cont.end());
}
template <typename _Ty, typename _Alloc, typename _Pr>
void erase_if(pod_vector<_Ty, _Alloc>& cont, _Pr pred)
{
  cont.erase(std::remove_if(cont.begin(), cont.end(), pred), cont.end());
}
#pragma endregion

template <typename _Cont>
inline typename _Cont::iterator insert_sorted(_Cont& vec, typename _Cont::value_type const& val)
{
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), val), val);
}

template <typename _Cont, typename _Pred>
inline typename _Cont::iterator insert_sorted(_Cont& vec, typename _Cont::value_type const& val, _Pred pred)
{
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), val, pred), val);
}

// alias: array_buffer
template <typename _Ty, typename _Alloc = buffer_allocator<_Ty>>
using array_buffer = pod_vector<_Ty, _Alloc>;

} // namespace yasio
#endif
