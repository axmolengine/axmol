//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2025 HALX99
Copyright (c) 2016 Matthew Rodusek(matthew.rodusek@gmail.com) <http://rodusek.me>

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
See: https://github.com/bitwizeshift/string_view-standalone
*/
#ifndef YASIO__STRING_VIEW_HPP
#define YASIO__STRING_VIEW_HPP
#include <ctype.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <string_view>
#include "yasio/impl/char_traits.hpp"
#include "yasio/compiler/feature_test.hpp"

/// wcsncasecmp workaround for android API level < 23, copy from msvc ucrt 10.0.18362.0 'wcsnicmp'
#if (defined(__ANDROID_API__) && __ANDROID_API__ < 23) || defined(__MINGW32__)
inline int wcsncasecmp(wchar_t const* const lhs, wchar_t const* const rhs, size_t const count)
{
  if (count == 0)
  {
    return 0;
  }

  wchar_t const* lhs_ptr = reinterpret_cast<wchar_t const*>(lhs);
  wchar_t const* rhs_ptr = reinterpret_cast<wchar_t const*>(rhs);

  int result;
  int lhs_value;
  int rhs_value;
  size_t remaining = count;
  do
  {
    lhs_value = ::towlower(*lhs_ptr++);
    rhs_value = ::towlower(*rhs_ptr++);
    result    = lhs_value - rhs_value;
  } while (result == 0 && lhs_value != 0 && --remaining != 0);

  return result;
}
#endif

namespace tlx
{
template <class T>
using decay_t = typename std::decay<T>::type;
template <class T>
using remove_const_t = typename std::remove_const<T>::type;
namespace char_ranges
{ // allow get char type from char*, wchar_t*, std::string, std::wstring
template <typename _Ty>
struct value_type {
  using type = typename _Ty::value_type;
};

template <typename _Ty>
struct value_type<_Ty&> {
  using type = remove_const_t<_Ty>;
};

template <typename _Ty>
struct value_type<_Ty*> {
  using type = remove_const_t<_Ty>;
};
} // namespace char_ranges

// starts_with(), since C++20:
template <typename _CharT>
inline bool starts_with(std::basic_string_view<_CharT> lhs,
                        std::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && lhs.compare(0, v.size(), v) == 0;
}

template <typename _T1, typename _T2>
inline bool starts_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return starts_with(std::basic_string_view<char_type>{lhs}, std::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool starts_with(std::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && lhs.front() == c;
}

template <typename _Ty>
inline bool starts_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return starts_with(std::basic_string_view<char_type>{lhs}, c);
}

// ends_with(), since C++20:
template <typename _CharT>
inline bool ends_with(std::basic_string_view<_CharT> lhs,
                      std::basic_string_view<_CharT> v) // (1)
{
  auto offset = lhs.size() - v.size();
  return lhs.size() >= v.size() && lhs.compare(offset, v.size(), v) == 0;
}

template <typename _T1, typename _T2>
inline bool ends_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return ends_with(std::basic_string_view<char_type>{lhs}, std::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool ends_with(std::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && lhs.back() == c;
}

template <typename _Ty>
inline bool ends_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return ends_with(std::basic_string_view<char_type>{lhs}, c);
}

/// The case insensitive implementation of starts_with, ends_with
namespace ic
{
template <typename _CharT>
inline bool iequals(std::basic_string_view<_CharT> lhs, std::basic_string_view<_CharT> v);
#if defined(_MSC_VER)
template <>
inline bool iequals<char>(std::basic_string_view<char> lhs, std::basic_string_view<char> v)
{
  return lhs.size() == v.size() && ::_strnicmp(lhs.data(), v.data(), v.size()) == 0;
}
template <>
inline bool iequals<wchar_t>(std::basic_string_view<wchar_t> lhs, std::basic_string_view<wchar_t> v)
{
  return lhs.size() == v.size() && ::_wcsnicmp(lhs.data(), v.data(), v.size()) == 0;
}
#else
template <>
inline bool iequals<char>(std::basic_string_view<char> lhs, std::basic_string_view<char> v)
{
  return lhs.size() == v.size() && ::strncasecmp(lhs.data(), v.data(), v.size()) == 0;
}
template <>
inline bool iequals<wchar_t>(std::basic_string_view<wchar_t> lhs, std::basic_string_view<wchar_t> v)
{
  return lhs.size() == v.size() && ::wcsncasecmp(lhs.data(), v.data(), v.size()) == 0;
}
#endif
template <typename _T1, typename _T2>
inline bool iequals(_T1&& lhs, _T2&& v)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return iequals(std::basic_string_view<char_type>{lhs}, std::basic_string_view<char_type>{v});
}
// starts_with(), since C++20:
template <typename _CharT>
inline bool starts_with(std::basic_string_view<_CharT> lhs,
                        std::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && iequals(lhs.substr(0, v.size()), v);
}

template <typename _T1, typename _T2>
inline bool starts_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return starts_with(std::basic_string_view<char_type>{lhs}, std::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool starts_with(std::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && ::tolower(lhs.front()) == ::tolower(c);
}

template <typename _Ty>
inline bool starts_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return starts_with(std::basic_string_view<char_type>{lhs}, c);
}

// ends_with(), since C++20:
template <typename _CharT>
inline bool ends_with(std::basic_string_view<_CharT> lhs,
                      std::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && iequals(lhs.substr(lhs.size() - v.size(), lhs.npos), v);
}

template <typename _T1, typename _T2>
inline bool ends_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return ends_with(std::basic_string_view<char_type>{lhs}, std::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool ends_with(std::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && ::tolower(lhs.back()) == ::tolower(c);
}

template <typename _Ty>
inline bool ends_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return ends_with(std::basic_string_view<char_type>{lhs}, c);
}
} // namespace ic
} // namespace tlx

#endif
