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

/*
 * The standard split stub of tlx:
 *   The pred callback prototype: [](CStr first, CStr last) ->bool{ return true; }
 *   returns:
 *     true: want continue split
 *     false: want end split
 *
 */

#pragma once

#include <type_traits>
#include <string_view>
#include <span>
#include <cstring>  // strchr, strpbrk
#include <cwchar>   // wcschr, wcspbrk
#include <iterator> // std::distance
#include <memory>   // std::to_address

#if defined(__cpp_lib_ranges)
#include <ranges>
#endif

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4706)
#endif

namespace tlx
{

template <typename T, typename = void>
struct has_random_access_iterator_category : std::false_type {};

template <typename T>
struct has_random_access_iterator_category<T, std::void_t<typename std::iterator_traits<typename T::iterator>::iterator_category>> {
  using cat                   = typename std::iterator_traits<typename T::iterator>::iterator_category;
  static constexpr bool value = std::is_base_of_v<std::random_access_iterator_tag, cat>;
};

template <typename T>
inline constexpr bool has_random_access_iterator_category_v = has_random_access_iterator_category<std::remove_cvref_t<T>>::value;

template <typename T, typename = void>
struct has_data_and_size : std::false_type {};

template <typename T>
struct has_data_and_size<T, std::void_t<decltype(std::declval<T>().data()), decltype(std::declval<T>().size())>> : std::true_type {};

template <typename T>
inline constexpr bool has_data_and_size_v = has_data_and_size<std::remove_cvref_t<T>>::value;

template <typename T>
inline constexpr bool is_contiguous_like_v =
#if defined(__cpp_lib_ranges)
    std::ranges::contiguous_range<std::remove_cvref_t<T>> ||
#endif
    (has_data_and_size_v<T> && has_random_access_iterator_category_v<T>);

// -----------------------------
// element_of trait (robust)
// -----------------------------
template <typename _Ty, typename = void>
struct element_of; // primary undefined

// If _Ty has value_type (containers, span, iterators with value_type), use it
template <typename _Ty>
struct element_of<_Ty, std::void_t<typename std::remove_cvref_t<_Ty>::value_type>> {
  using type = typename std::remove_cvref_t<_Ty>::value_type;
};

// Pointer specialization: keep cv of the pointee (so const char* -> const char)
template <typename _Ty>
struct element_of<_Ty*, void> {
  using type = std::remove_cv_t<_Ty>; // keep cv on pointer handled by pointer type itself
};

// Special-case: string_view should map to const char (preserve const)
template <>
struct element_of<std::string_view, void> {
  using type = const char;
};

template <>
struct element_of<std::wstring_view, void> {
  using type = const wchar_t;
};

// Helper aliases
template <typename _Ty>
using element_of_t = typename element_of<_Ty>::type;

// element_of_nocvref_t: evaluate element_of on remove_cvref_t<_Ty>
// useful for deducing delim/delims types from an input type
template <typename _Ty>
using element_of_nocvref_t = typename element_of<std::remove_cvref_t<_Ty>>::type;

// -----------------------------
// is_span_of detection
// -----------------------------
template <typename _Ty>
struct is_span_of : std::false_type {};

template <typename E>
struct is_span_of<std::span<E>> : std::true_type {};

template <typename _Ty>
inline constexpr bool is_span_of_v = is_span_of<std::remove_cvref_t<_Ty>>::value;

// -----------------------------
// detail implementations
// -----------------------------
namespace detail
{

template <typename Elem>
struct split_traits_base;

// narrow char
template <>
struct split_traits_base<char> {
  static const char* find_char(const char* s, char delim) { return std::strchr(s, delim); }
  static const char* find_any(const char* s, const char* delims) { return std::strpbrk(s, delims); }

  static char* find_char(char* s, char delim) { return std::strchr(s, delim); }
  static char* find_any(char* s, const char* delims) { return std::strpbrk(s, delims); }
};

// wide char
template <>
struct split_traits_base<wchar_t> {
  static const wchar_t* find_char(const wchar_t* s, wchar_t delim) { return std::wcschr(s, delim); }
  static const wchar_t* find_any(const wchar_t* s, const wchar_t* delims) { return std::wcspbrk(s, delims); }

  static wchar_t* find_char(wchar_t* s, wchar_t delim) { return std::wcschr(s, delim); }
  static wchar_t* find_any(wchar_t* s, const wchar_t* delims) { return std::wcspbrk(s, delims); }
};

template <typename Elem>
using split_traits = split_traits_base<std::remove_cv_t<Elem>>;

// -----------------------------
// split_if implementations
// -----------------------------

// pointer version: keep nullptr as end sentinel to avoid strlen
template <typename Elem, typename Pred>
inline void split_if(Elem* s, Elem delim, Pred&& pred)
{
  Elem* start = s;
  Elem* ptr   = s;
  while ((ptr = split_traits<Elem>::find_char(ptr, delim)))
  {
    if (start <= ptr && !pred(start, ptr))
      return;
    start = ptr + 1;
    ++ptr;
  }
  // end sentinel: nullptr (caller wrapper must accept auto last)
  pred(start, nullptr);
}

// span version: Elem may be const-qualified; delim uses remove_cv_t<Elem>
template <typename Elem, typename Pred>
inline void split_if(std::span<Elem> s, std::remove_cv_t<Elem> delim, Pred&& pred)
{
  Elem* start = s.data();
  Elem* ptr   = start;
  Elem* end   = start + s.size();
  while ((ptr = split_traits<Elem>::find_char(ptr, delim)))
  {
    if (ptr >= end)
      break;
    if (start <= ptr && !pred(start, ptr))
      return;
    start = ptr + 1;
    ++ptr;
  }
  if (start <= end)
    pred(start, end);
}

// convenience span wrapper (callback style)
template <typename Elem, typename Fn>
inline void split(std::span<Elem> s, std::remove_cv_t<Elem> delim, Fn&& func)
{
  split_if(s, delim, [func = std::forward<Fn>(func)](Elem* f, Elem* l) {
    func(f, l);
    return true;
  });
}

// -----------------------------
// split_of_if implementations
// -----------------------------

// pointer version: delims is pointer to (possibly const) char type; end sentinel nullptr
template <typename Elem, typename Pred>
inline void split_of_if(Elem* s, const std::remove_cv_t<Elem>* delims, Pred&& pred)
{
  Elem* start = s;
  Elem* ptr   = s;
  Elem delim  = *delims;
  while ((ptr = split_traits<Elem>::find_any(ptr, delims)))
  {
    if (start <= ptr)
    {
      if (!pred(start, ptr, delim))
        return;
      delim = *ptr;
    }
    start = ptr + 1;
    ++ptr;
  }
  pred(start, nullptr, delim);
}

// span version: delims is pointer to remove_cv_t<Elem>
template <typename Elem, typename Pred>
inline void split_of_if(std::span<Elem> s, const std::remove_cv_t<Elem>* delims, Pred&& pred)
{
  Elem* start = s.data();
  Elem* ptr   = start;
  Elem* end   = start + s.size();
  Elem delim  = *delims;
  while ((ptr = split_traits<Elem>::find_any(ptr, delims)))
  {
    if (ptr >= end)
      break;
    if (start <= ptr)
    {
      if (!pred(start, ptr, delim))
        return;
      delim = *ptr;
    }
    start = ptr + 1;
    ++ptr;
  }
  if (start <= end)
    pred(start, end, delim);
}

// convenience span wrapper (callback style)
template <typename Elem, typename Fn>
inline void split_of(std::span<Elem> s, const std::remove_cv_t<Elem>* delims, Fn&& func)
{
  split_of_if(s, delims, [func = std::forward<Fn>(func)](Elem* f, Elem* l, Elem d) {
    func(f, l, d);
    return true;
  });
}

} // namespace detail

// -----------------------------
// external dispatching helpers
// -----------------------------

// split_if: accept string-like types, pointers, spans
template <typename Str, typename Pred>
inline void split_if(Str&& s, const element_of_nocvref_t<Str> delim, Pred&& pred)
{
  using Tp = std::remove_cvref_t<Str>;

  if constexpr (std::is_pointer_v<Tp>)
  {
    // pointer (char*/wchar_t*)
    detail::split_if(s, delim, std::forward<Pred>(pred));
  }
  else if constexpr (is_span_of_v<Tp>)
  {
    detail::split_if(s, delim, std::forward<Pred>(pred));
  }
  else if constexpr (is_contiguous_like_v<Tp>)
  {
    // Any type that exposes data() and size() (std::string, std::string_view,
    // std::basic_string<...>, std::span<...> etc.)
    using ElemPtr = decltype(std::declval<Tp>().data()); // e.g. const char*
    using Elem    = std::remove_pointer_t<ElemPtr>;      // preserves const
    detail::split_if(std::span<Elem>(s.data(), s.size()), delim, std::forward<Pred>(pred));
  }
  else
  {
    static_assert(sizeof(Tp) == 0, "Unsupported type for tlx::split_if");
  }
}

// split_of_if: similar dispatch
template <typename Str, typename Pred>
inline void split_of_if(Str&& s, const element_of_nocvref_t<Str>* delims, Pred&& pred)
{
  using Tp = std::remove_cvref_t<Str>;

  if constexpr (std::is_pointer_v<Tp>)
  {
    detail::split_of_if(s, delims, std::forward<Pred>(pred));
  }
  else if constexpr (is_span_of_v<Tp>)
  {
    detail::split_of_if(s, delims, std::forward<Pred>(pred));
  }
  else if constexpr (is_contiguous_like_v<Tp>)
  {
    // Any type that exposes data() and size() (std::string, std::string_view,
    // std::basic_string<...>, std::span<...> etc.)
    using ElemPtr = decltype(std::declval<Tp>().data()); // e.g. const char*
    using Elem    = std::remove_pointer_t<ElemPtr>;      // preserves const
    detail::split_of_if(std::span<Elem>(s.data(), s.size()), delims, std::forward<Pred>(pred));
  }
  else
  {
    static_assert(sizeof(Tp) == 0, "Unsupported type for tlx::split_of_if");
  }
}

// -----------------------------
// non-_if wrappers (callback style)
// note: wrapper lambdas accept `auto last` (not auto*) so they can receive nullptr
// -----------------------------
template <typename Str, typename Fn>
inline void split(Str&& s, const element_of_nocvref_t<Str> delim, Fn&& func)
{
  split_if(std::forward<Str>(s), delim, [func = std::forward<Fn>(func)](auto* first, auto last) {
    func(first, last);
    return true;
  });
}

template <typename Str, typename Fn>
inline void split_of(Str&& s, const element_of_nocvref_t<Str>* delims, Fn&& func)
{
  split_of_if(std::forward<Str>(s), delims, [func = std::forward<Fn>(func)](auto* first, auto last, auto delim) {
    func(first, last, delim);
    return true;
  });
}

// -----------------------------
// iterator/iterator-range overloads
// support arbitrary iterators; construct span from to_address(first)
// ensure element type matches the pointer returned by to_address
// -----------------------------
template <typename Iter, typename Fn>
inline void split(Iter first, Iter last, const element_of_nocvref_t<Iter> delim, Fn&& func)
{
  // deduce pointer type from std::to_address(first)
  using Ptr      = decltype(std::to_address(first));
  using ElemSpan = std::remove_pointer_t<std::remove_cv_t<Ptr>>; // element type for span
  auto n         = std::distance(first, last);
  // construct span with element type matching pointer returned by to_address
  std::span<ElemSpan> sp(std::to_address(first), static_cast<size_t>(n));
  detail::split_if(sp, delim, [func = std::forward<Fn>(func)](ElemSpan* f, ElemSpan* l) {
    func(f, l);
    return true;
  });
}

template <typename Iter, typename Fn>
inline void split_of(Iter first, Iter last, const element_of_nocvref_t<Iter>* delims, Fn&& func)
{
  using Ptr      = decltype(std::to_address(first));
  using ElemSpan = std::remove_pointer_t<std::remove_cv_t<Ptr>>;
  auto n         = std::distance(first, last);
  std::span<ElemSpan> sp(std::to_address(first), static_cast<size_t>(n));
  detail::split_of_if(sp, delims, [func = std::forward<Fn>(func)](ElemSpan* f, ElemSpan* l, ElemSpan d) {
    func(f, l, d);
    return true;
  });
}

} // namespace tlx

namespace tlx
{
/// split_path
template <typename _Elem, typename _Pred, typename _Fn>
inline void split_path(_Elem* s, _Pred&& pred, _Fn&& func)
{
  _Elem* _Start = s;
  _Elem* _Ptr   = s;
  while (pred(_Ptr))
  {
    if (*_Ptr == _Elem('\\') || *_Ptr == _Elem('/'))
    {
      if (_Ptr != _Start)
      {
        auto _Ch = *_Ptr;
        *_Ptr    = _Elem('\0');
        bool brk = func(s);
#if defined(_WIN32)
        *_Ptr = _Elem('\\');
#else
        *_Ptr = _Elem('/');
#endif
        if (brk)
          return;
      }
      _Start = _Ptr + 1;
    }
    ++_Ptr;
  }
  if (_Start < _Ptr)
    func(s);
}
} // namespace tlx

namespace tlx
{
//  CLASS split_term_guard
struct split_term_guard {
  split_term_guard(char* end)
  {
    if (end)
    {
      this->val_ = *end;
      *end       = '\0';
      this->end_ = end;
    }
  }
  ~split_term_guard()
  {
    if (this->end_)
      *this->end_ = this->val_;
  }

private:
  char* end_ = nullptr;
  char val_  = '\0';
};
} // namespace tlx

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
