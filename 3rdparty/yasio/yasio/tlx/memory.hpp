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
*/
#pragma once
#include <memory>
#include <stdexcept>
#include <utility>
#include <string.h>
#include <iterator>
#include <type_traits>
#include <assert.h>

#include "yasio/compiler/feature_test.hpp"

namespace tlx
{
template <typename _Ty, bool = true>
struct construct_helper {
  template <typename... Args>
  static _Ty* construct_at(_Ty* p, Args&&... args)
  {
    return ::new (static_cast<void*>(p)) _Ty(std::forward<Args>(args)...);
  }
};
template <typename _Ty>
struct construct_helper<_Ty, false> {
  template <typename... Args>
  static _Ty* construct_at(_Ty* p, Args&&... args)
  {
    return ::new (static_cast<void*>(p)) _Ty{std::forward<Args>(args)...};
  }
};

template <typename _Ty, typename... Args>
inline _Ty* construct_at(_Ty* p, Args&&... args)
{
  return construct_helper<_Ty, std::is_constructible<_Ty, Args&&...>::value>::construct_at(p, std::forward<Args>(args)...);
}

template <typename _Ty>
inline void invoke_dtor(_Ty* p)
{
  p->~_Ty();
}

struct __zero_then_variadic_args_t {
  explicit __zero_then_variadic_args_t() = default;
}; // tag type for value-initializing first, constructing second from remaining args

struct __one_then_variadic_args_t {
  explicit __one_then_variadic_args_t() = default;
}; // tag type for constructing first from one arg, constructing second from remaining args

template <class _Ty1, class _Ty2, bool = std::is_empty_v<_Ty1> && !std::is_final_v<_Ty1>>
class __compressed_pair final : private _Ty1 { // store a pair of values, deriving from empty first
public:
  _Ty2 _Myval2;

  using _Mybase = _Ty1; // for visualization

  template <class... _Other2>
  constexpr explicit __compressed_pair(__zero_then_variadic_args_t, _Other2&&... _Val2) noexcept : _Ty1(), _Myval2(std::forward<_Other2>(_Val2)...)
  {}

  template <class _Other1, class... _Other2>
  constexpr __compressed_pair(__one_then_variadic_args_t, _Other1&& _Val1, _Other2&&... _Val2) noexcept
      : _Ty1(std::forward<_Other1>(_Val1)), _Myval2(std::forward<_Other2>(_Val2)...)
  {}

  constexpr _Ty1& _Get_first() noexcept { return *this; }

  constexpr const _Ty1& _Get_first() const noexcept { return *this; }
};

template <class _Ty1, class _Ty2>
class __compressed_pair<_Ty1, _Ty2, false> final { // store a pair of values, not deriving from first
public:
  _Ty1 _Myval1;
  _Ty2 _Myval2;

  template <class... _Other2>
  constexpr explicit __compressed_pair(__zero_then_variadic_args_t, _Other2&&... _Val2) noexcept : _Myval1(), _Myval2(std::forward<_Other2>(_Val2)...)
  {}

  template <class _Other1, class... _Other2>
  constexpr __compressed_pair(__one_then_variadic_args_t, _Other1&& _Val1, _Other2&&... _Val2) noexcept
      : _Myval1(std::forward<_Other1>(_Val1)), _Myval2(std::forward<_Other2>(_Val2)...)
  {}

  constexpr _Ty1& _Get_first() noexcept { return _Myval1; }

  constexpr const _Ty1& _Get_first() const noexcept { return _Myval1; }
};

template <class _Alloc, class _Value_type>
using rebind_alloc_t = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Value_type>;

template <class _Ty>
struct __tidy_guard { // class with destructor that calls _Tidy
  _Ty* _Target;
  YASIO__CONSTEXPR ~__tidy_guard()
  {
    if (_Target)
    {
      _Target->_Tidy();
    }
  }
};

template <class _Ty>
struct _Identity {
  using type = _Ty;
};
template <class _Ty>
using identity_t = typename _Identity<_Ty>::type;

template <class _Alloc>
class __uninitialized_backout_al {
public:
  using allocator_traits = typename std::allocator_traits<_Alloc>;
  using pointer          = typename allocator_traits::pointer;
  using value_type       = typename allocator_traits::value_type;

  __uninitialized_backout_al(pointer dest, _Alloc& al) : _dest(dest), _cur(dest), _alloc(al), _released(false) {}

  ~__uninitialized_backout_al()
  {
    if (!_released)
    {
      for (auto p = _dest; p != _cur; ++p)
      {
        std::allocator_traits<_Alloc>::destroy(_alloc, p);
      }
    }
  }

  template <class... Args>
  void _Emplace_back(Args&&... args)
  {
    std::allocator_traits<_Alloc>::construct(_alloc, _cur, std::forward<Args>(args)...);
    ++_cur;
  }

  pointer _Release()
  {
    _released = true;
    return _cur;
  }

private:
  pointer _dest;
  pointer _cur;
  _Alloc& _alloc;
  bool _released;
};

template <typename _Iter>
using iter_ref_t = typename std::iterator_traits<_Iter>::reference;

inline void __xlength_error(const char* what) { throw ::std::length_error(what); }

inline void __xout_of_range(const char* what) { throw ::std::out_of_range(what); }

static_assert(std::is_same_v<typename std::iterator_traits<int*>::value_type, int>);

template <typename _Iter>
inline constexpr bool is_pod_iterator_v = std::is_trivially_copyable_v<typename std::iterator_traits<_Iter>::value_type>;

template <typename _Iter, typename _Ptr>
inline constexpr bool bitcopy_assignable_v = std::is_trivially_copyable_v<typename std::iterator_traits<_Iter>::value_type> &&
                                             std::is_same_v<typename std::iterator_traits<_Iter>::value_type, typename std::pointer_traits<_Ptr>::element_type>;

template <typename _Ptr, typename _Alloc>
_Ptr uninitialized_fill_n(_Ptr first, size_t count, const typename _Alloc::value_type& val, _Alloc& alloc)
{
  using value_type = typename _Alloc::value_type;

  if constexpr (std::is_trivially_copyable_v<value_type>)
  {
    if constexpr (sizeof(value_type) == 1)
    {
      ::memset(first, static_cast<unsigned char>(val), count);
    }
    else
    {
      for (size_t i = 0; i < count; ++i)
      {
        first[i] = val;
      }
    }
    return first + count;
  }
  else
  {
    for (size_t i = 0; i < count; ++i)
    {
      std::allocator_traits<_Alloc>::construct(alloc, first + i, val);
    }
    return first + count;
  }
}

template <typename _InIt, typename _OutPtr, typename _Alloc>
constexpr _OutPtr uninitialized_move(_InIt first, _InIt last, _OutPtr dest, _Alloc& alloc)
{
  using T          = typename _Alloc::value_type;
  const auto count = static_cast<size_t>(last - first);

  if constexpr (std::is_trivially_copyable_v<T>)
  {
    if (!std::is_constant_evaluated())
    {
      ::memmove(std::to_address(dest), std::to_address(first), count * sizeof(T));
      return dest + count;
    }
  }

  __uninitialized_backout_al<_Alloc> backout{dest, alloc};
  for (; first != last; ++first)
  {
    backout._Emplace_back(std::move(*first));
  }
  return backout._Release();
}

template <typename _InIt, typename _Alloc, typename _OutPtr>
constexpr _OutPtr uninitialized_copy_n(_InIt first, size_t count, _OutPtr dest, _Alloc& alloc)
{
  using T = typename _Alloc::value_type;

  if constexpr (std::is_trivially_copyable_v<T>)
  {
    ::memmove(std::to_address(dest), std::to_address(first), count * sizeof(T));
    return dest + count;
  }
  else
  {
    __uninitialized_backout_al<_Alloc> backout{dest, alloc};
    for (; count != 0; ++first, --count)
    {
      backout._Emplace_back(*first);
    }
    return backout._Release();
  }
}

template <typename _InIt, typename _Ptr, typename _Alloc>
_Ptr uninitialized_copy(_InIt first, _InIt last, _Ptr dest, _Alloc& alloc)
{
  using T = typename _Alloc::value_type;

  const auto count = static_cast<size_t>(last - first);

  if constexpr (std::is_trivially_copyable_v<T>)
  {
    ::memmove(dest, &*first, count * sizeof(T));
    return dest + count;
  }
  else
  {
    __uninitialized_backout_al<_Alloc> backout{dest, alloc};
    for (; first != last; ++first)
    {
      backout._Emplace_back(*first);
    }
    return backout._Release();
  }
}

template <typename _Alloc, typename Ptr>
constexpr void destroy_range(Ptr first, Ptr last, _Alloc& alloc) noexcept
{
  using T = typename _Alloc::value_type;

  if constexpr (!std::is_trivially_destructible_v<T>)
  {
    for (; first != last; ++first)
    {
      std::allocator_traits<_Alloc>::destroy(alloc, std::to_address(first));
    }
  }
}

/*
 * trivially_constructible types: Use memset to zero for
 * non-trivially_constructible types: default constructor
 */
template <typename _Ptr, typename _Alloc>
_Ptr uninitialized_value_construct_n(_Ptr first, size_t count, _Alloc& alloc)
{
  using T = typename _Alloc::value_type;

  if constexpr (std::is_trivially_constructible_v<T>)
  {
    ::memset(first, 0, count * sizeof(T));
    return first + count;
  }
  else
  {
    __uninitialized_backout_al<_Alloc> backout{first, alloc};
    for (; count > 0; --count)
    {
      backout._Emplace_back();
    }
    return backout._Release();
  }
}

template <typename InCtgIt, typename OutCtgIt>
OutCtgIt copy_memmove_n(InCtgIt first, size_t count, OutCtgIt dest)
{
  using T = typename std::iterator_traits<InCtgIt>::value_type;

  const auto byte_count = count * sizeof(T);

  const auto src_ptr  = std::to_address(first);
  const auto dest_ptr = std::to_address(dest);

  ::memmove(dest_ptr, src_ptr, byte_count);

  if constexpr (std::is_pointer_v<OutCtgIt>)
  {
    return dest_ptr + count;
  }
  else
  {
    return dest + static_cast<typename std::iterator_traits<OutCtgIt>::difference_type>(count);
  }
}

template <typename _InIt, typename _OutIt>
constexpr _OutIt move_unchecked(_InIt first, _InIt last, _OutIt dest)
{
  using T          = typename std::iterator_traits<_InIt>::value_type;
  const auto count = static_cast<size_t>(last - first);

  if constexpr (std::is_trivially_copyable_v<T>)
  {
    ::memmove(std::to_address(dest), std::to_address(first), count * sizeof(T));
    return dest + count;
  }
  else
  {
    for (; first != last; ++first, ++dest)
    {
      *dest = std::move(*first);
    }
    return dest;
  }
}

template <typename _CtgIt1, typename _CtgIt2>
_CtgIt2 copy_backward_memmove(_CtgIt1 _First, _CtgIt1 _Last, _CtgIt2 _Dest)
{
  // implement copy_backward-like function as memmove
  const auto _First_ptr = std::to_address(_First);
  const auto _Last_ptr  = std::to_address(_Last);
  const auto _Dest_ptr  = std::to_address(_Dest);

  const auto _First_ch = const_cast<const char*>(reinterpret_cast<const volatile char*>(_First_ptr));
  const auto _Last_ch  = const_cast<const char*>(reinterpret_cast<const volatile char*>(_Last_ptr));
  const auto _Dest_ch  = const_cast<char*>(reinterpret_cast<const volatile char*>(_Dest_ptr));

  const auto _Count  = static_cast<size_t>(_Last_ch - _First_ch);
  const auto _Result = ::memmove(_Dest_ch - _Count, _First_ch, _Count);

  if constexpr (std::is_pointer_v<_CtgIt2>)
  {
    return static_cast<_CtgIt2>(_Result);
  }
  else
  {
    using diff_t = typename std::iterator_traits<_CtgIt2>::difference_type;
    return _Dest - static_cast<diff_t>(_Last_ptr - _First_ptr);
  }
}

template <typename _BidIt1, typename _BidIt2>
_BidIt2 copy_backward_memmove(std::move_iterator<_BidIt1> _First, std::move_iterator<_BidIt1> _Last, _BidIt2 _Dest)
{
  return copy_backward_memmove(_First.base(), _Last.base(), _Dest);
}

template <typename _BidIt1, typename _BidIt2>
_BidIt2 move_backward_unchecked(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
{
  // move [_First, _Last) backwards to [..., _Dest)
  // note: move_backward_unchecked has callers other than the move_backward family
  if constexpr (is_pod_iterator_v<_BidIt2>)
  {
    if (!std::is_constant_evaluated())
    {
      return copy_backward_memmove(_First, _Last, _Dest);
    }
  }

  while (_First != _Last)
  {
    *--_Dest = std::move(*--_Last);
  }

  return _Dest;
}

template <typename _InIt, typename _SizeTy, typename _OutIt>
constexpr _OutIt copy_n_unchecked4(_InIt first, _SizeTy count, _OutIt dest)
{
  using T = typename std::iterator_traits<_InIt>::value_type;

  static_assert(std::is_integral_v<_SizeTy>, "must be integer-like");
  if (count < 0)
    return dest;

  if constexpr (std::is_trivially_copyable_v<T>)
  {
    if (!std::is_constant_evaluated())
    {
      ::memmove(std::to_address(dest), std::to_address(first), count * sizeof(T));
      return dest + count;
    }
  }

  for (; count != 0; ++dest, ++first, --count)
  {
    *dest = *first;
  }
  return dest;
}

// Enumeration for fill policies with positive semantics
enum class fill_policy
{
  always,          // Always perform value initialization
  nontrivial_dtor, // Fill if destructor is non-trivial
  nontrivial,      // Fill if either ctor or dtor is non-trivial
};

// Trait to determine whether filling is required under a given policy
template <class _Ty, fill_policy policy>
struct __allow_auto_fill {
  static constexpr bool value = true; // Default: always fill
};

// Specialization: fill only if destructor is non-trivial
template <class _Ty>
struct __allow_auto_fill<_Ty, fill_policy::nontrivial_dtor> {
  static constexpr bool value = !std::is_trivially_destructible_v<_Ty>;
};

// Specialization: fill if either ctor or dtor is non-trivial
template <class _Ty>
struct __allow_auto_fill<_Ty, fill_policy::nontrivial> {
  static constexpr bool value = !(std::is_trivially_default_constructible_v<_Ty> && std::is_trivially_destructible_v<_Ty>);
};

template <typename _Ty, fill_policy policy>
inline constexpr bool __allow_auto_fill_v = __allow_auto_fill<_Ty, policy>::value;

struct __auto_value_init_t { // tag to request value-initialization
  explicit __auto_value_init_t() = default;
};

struct value_init_t { // tag to request value-initialization
  explicit value_init_t() = default;
};
inline constexpr value_init_t value_init = value_init_t{};

struct no_init_t { // tag to request no-initialization
  explicit no_init_t() = default;
};
inline constexpr no_init_t no_init = no_init_t{};

} // namespace tlx

#define _TLX_VERIFY(cond, mesg) assert(cond&& mesg)

#define _TLX_INTERNAL_CHECK(cond) assert(cond)
