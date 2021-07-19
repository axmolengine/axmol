/****************************************************************************
 Copyright (c) 2020-2021 Simdsoft Limited.

 https://simdsoft.com

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
 ****************************************************************************/
#ifndef SIMDSOFT_FAST_SPLIT_HPP
#define SIMDSOFT_FAST_SPLIT_HPP
#include <string.h>
#include <string>
#include <vector>

namespace xsbase
{
namespace internal
{
#define _REDIRECT_DELIM_INTRI(_From, _To, _CStr, _Delim)                                           \
  inline _CStr _To(_CStr s, _Delim d) { return _From(s, d); }

_REDIRECT_DELIM_INTRI(strchr, xstrchr, const char*, int)
_REDIRECT_DELIM_INTRI(strchr, xstrchr, char*, int)
_REDIRECT_DELIM_INTRI(wcschr, xstrchr, const wchar_t*, wchar_t)
_REDIRECT_DELIM_INTRI(wcschr, xstrchr, wchar_t*, wchar_t)

_REDIRECT_DELIM_INTRI(strstr, xstrstr, const char*, const char*)
_REDIRECT_DELIM_INTRI(strstr, xstrstr, char*, const char*)
_REDIRECT_DELIM_INTRI(wcsstr, xstrstr, const wchar_t*, const wchar_t*)
_REDIRECT_DELIM_INTRI(wcsstr, xstrstr, wchar_t*, const wchar_t*)

_REDIRECT_DELIM_INTRI(strpbrk, xstrpbrk, const char*, const char*)
_REDIRECT_DELIM_INTRI(strpbrk, xstrpbrk, char*, const char*)
_REDIRECT_DELIM_INTRI(wcspbrk, xstrpbrk, const wchar_t*, const wchar_t*)
_REDIRECT_DELIM_INTRI(wcspbrk, xstrpbrk, wchar_t*, const wchar_t*)
} // namespace internal

/*
 * nsc::fast_split API (override+3)
 * op prototype: [](const char* start, const char* end){}
 */
template <typename _CStr, typename _Fn>
inline void fast_split(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim,
                       _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrchr(_Ptr, delim)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
      func(_Start, _Ptr);
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start <= _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split(_CStr s, size_t slen,
                       const typename std::remove_pointer<_CStr>::type* delims, size_t dlen,
                       _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrstr(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
      func(_Start, _Ptr);

    _Start = _Ptr + dlen;
    _Ptr += dlen;
  }
  if (_Start <= _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_of(_CStr s, size_t slen,
                          const typename std::remove_pointer<_CStr>::type* delims, _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  auto _Delim = delims[0];
  while ((_Ptr = internal::xstrpbrk(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
    {
      func(_Start, _Ptr, _Delim);
      _Delim = *_Ptr;
    }
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start <= _End)
    func(_Start, _End, _Delim);
}

/*
 * nsc::fast_split_brkif API (override+3)
 * op prototype: [](const char* start, const char* end){}
 */
template <typename _CStr, typename _Fn>
inline void fast_split_brkif(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim,
                             _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrchr(_Ptr, delim)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
      if (func(_Start, _Ptr))
        return;
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start <= _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_brkif(_CStr s, size_t slen,
                             const typename std::remove_pointer<_CStr>::type* delims, size_t dlen,
                             _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrstr(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
    {
      if (func(_Start, _Ptr))
        return;
    }
    _Start = _Ptr + dlen;
    _Ptr += dlen;
  }
  if (_Start <= _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_of_brkif(_CStr s, size_t slen,
                                const typename std::remove_pointer<_CStr>::type* delims, _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  auto _Delim = delims[0];
  while ((_Ptr = internal::xstrpbrk(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr)
    {
      if (func(_Start, _Ptr, _Delim))
        return;
      _Delim = *_Ptr;
    }
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start <= _End)
    func(_Start, _End, _Delim);
}

/// C++ basic_string wrappers
template <typename _StringCont, typename _Fn>
inline void fast_split(_StringCont& s, typename _StringCont::value_type delim, _Fn func)
{
  typename _StringCont::value_type dummy[1] = {0};
  return fast_split(!s.empty() ? &s.front() : &dummy[0], s.length(), delim, func);
}

#if _HAS_CXX17
/// C++17 basic_string_view wrappers
template <typename _Elem, typename _Fn>
inline void fast_split(std::basic_string_view<_Elem> s, _Elem delim, _Fn func)
{
  return fast_split(s.data(), s.length(), delim, func);
}
template <typename _Elem, typename _Fn>
inline void fast_split(std::basic_string_view<_Elem> s, std::basic_string_view<_Elem> delims,
                       _Fn func)
{
  return fast_split(s.data(), s.length(), delims.data(), delims.length(), func);
}
template <typename _Elem, typename _Fn>
inline void fast_split_of(std::basic_string_view<_Elem> s, const _Elem* delims, _Fn func)
{
  return fast_split_of(s.data(), s.length(), delims, func);
}
#endif

/// simple retun a vector<basic_string> as string array
#if _HAS_CXX17
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(std::basic_string_view<_Elem> s, _Elem delim)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split(s, delim, [&](const _Elem* s, const _Elem* e) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(const std::basic_string<_Elem>& s, _Elem delim)
{
  return split(std::basic_string_view<_Elem>(s.c_str(), s.length()), delim);
}

template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(std::basic_string_view<_Elem> s,
                                                   std::basic_string_view<_Elem> delims)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split(s, delims, [&](const _Elem* s, const _Elem* e) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(const std::basic_string<_Elem>& s,
                                                   std::basic_string_view<_Elem> delims)
{
  return split(std::basic_string_view<_Elem>(s.c_str(), s.length()), delims);
}

template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split_of(std::basic_string_view<_Elem> s,
                                                      const _Elem* delims)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split_of(s, delims, [&](const _Elem* s, const _Elem* e, _Elem) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split_of(const std::basic_string<_Elem>& s,
                                                      const _Elem* delims)
{
  return split_of(std::basic_string_view<_Elem>(s.c_str(), s.length()), delims);
}
#endif

namespace nzls
{
/*
 * nsc::fast_split_nzls API (override+3) ignore empty string
 * op prototype: [](const char* start, const char* end){}
 */
template <typename _CStr, typename _Fn>
inline void fast_split(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim,
                       _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrchr(_Ptr, delim)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
      func(_Start, _Ptr);
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start < _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split(_CStr s, size_t slen,
                       const typename std::remove_pointer<_CStr>::type* delims, size_t dlen,
                       _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrstr(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
      func(_Start, _Ptr);

    _Start = _Ptr + dlen;
    _Ptr += dlen;
  }
  if (_Start < _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_of(_CStr s, size_t slen,
                          const typename std::remove_pointer<_CStr>::type* delims, _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  auto _Delim = delims[0];
  while ((_Ptr = internal::xstrpbrk(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
    {
      func(_Start, _Ptr, _Delim);
      _Delim = *_Ptr;
    }
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start < _End)
    func(_Start, _End, _Delim);
}

/*
 * nsc::fast_split_nzls_brkif API (override+3) ignore empty string
 * op prototype: [](const char* start, const char* end){}
 */
template <typename _CStr, typename _Fn>
inline void fast_split_brkif(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim,
                             _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrchr(_Ptr, delim)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
      if (func(_Start, _Ptr))
        return;
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start < _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_brkif(_CStr s, size_t slen,
                             const typename std::remove_pointer<_CStr>::type* delims, size_t dlen,
                             _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = internal::xstrstr(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
    {
      if (func(_Start, _Ptr))
        return;
    }
    _Start = _Ptr + dlen;
    _Ptr += dlen;
  }
  if (_Start < _End)
    func(_Start, _End);
}

template <typename _CStr, typename _Fn>
inline void fast_split_of_brkif(_CStr s, size_t slen,
                                const typename std::remove_pointer<_CStr>::type* delims, _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  auto _Delim = delims[0];
  while ((_Ptr = internal::xstrpbrk(_Ptr, delims)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start < _Ptr)
    {
      if (func(_Start, _Ptr, _Delim))
        return;
      _Delim = *_Ptr;
    }
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start < _End)
    func(_Start, _End, _Delim);
}

/// C++ basic_string wrappers
template <typename _StringCont, typename _Fn>
inline void fast_split(_StringCont& s, typename _StringCont::value_type delim, _Fn func)
{
  typename _StringCont::value_type dummy[1] = {0};
  return fast_split(!s.empty() ? &s.front() : &dummy[0], s.length(), delim, func);
}

#if _HAS_CXX17
/// C++17 basic_string_view wrappers
template <typename _Elem, typename _Fn>
inline void fast_split(std::basic_string_view<_Elem> s, _Elem delim, _Fn func)
{
  return fast_split(s.data(), s.length(), delim, func);
}
template <typename _Elem, typename _Fn>
inline void fast_split(std::basic_string_view<_Elem> s, std::basic_string_view<_Elem> delims,
                       _Fn func)
{
  return fast_split(s.data(), s.length(), delims.data(), delims.length(), func);
}
template <typename _Elem, typename _Fn>
inline void fast_split_of(std::basic_string_view<_Elem> s, const _Elem* delims, _Fn func)
{
  return fast_split_of(s.data(), s.length(), delims, func);
}
#endif

/// simple retun a vector<basic_string> as string array
#if _HAS_CXX17
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(std::basic_string_view<_Elem> s, _Elem delim)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split(s, delim, [&](const _Elem* s, const _Elem* e) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(const std::basic_string<_Elem>& s, _Elem delim)
{
  return split(std::basic_string_view<_Elem>(s.c_str(), s.length()), delim);
}

template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(std::basic_string_view<_Elem> s,
                                                   std::basic_string_view<_Elem> delims)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split(s, delims, [&](const _Elem* s, const _Elem* e) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split(const std::basic_string<_Elem>& s,
                                                   std::basic_string_view<_Elem> delims)
{
  return split(std::basic_string_view<_Elem>(s.c_str(), s.length()), delims);
}

template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split_of(std::basic_string_view<_Elem> s,
                                                      const _Elem* delims)
{
  std::vector<std::basic_string<_Elem>> results;
  fast_split_of(s, delims, [&](const _Elem* s, const _Elem* e, _Elem) {
    results.push_back(std::basic_string<_Elem>(s, e));
  });
  return results;
}
template <typename _Elem>
inline std::vector<std::basic_string<_Elem>> split_of(const std::basic_string<_Elem>& s,
                                                      const _Elem* delims)
{
  return split_of(std::basic_string_view<_Elem>(s.c_str(), s.length()), delims);
}
#endif
} // namespace nzls

template <typename _Elem, typename _Pr, typename _Fn>
inline void _splitpath(_Elem* s, _Pr _Pred, _Fn func) // will convert '\\' to '/'
{
  _Elem* _Start = s; // the start of every string
  _Elem* _Ptr   = s; // source string iterator
  while (_Pred(_Ptr))
  {
    if ('\\' == *_Ptr || '/' == *_Ptr)
    {
      if (_Ptr != _Start)
      {
        auto _Ch        = *_Ptr;
        *_Ptr           = '\0';
        bool should_brk = func(s);
#if defined(_WIN32)
        *_Ptr = '\\';
#else // For unix linux like system.
        *_Ptr = '/';
#endif
        if (should_brk)
          return;
      }
      _Start = _Ptr + 1;
    }
    ++_Ptr;
  }
  if (_Start < _Ptr)
  {
    func(s);
  }
}

template <typename _Elem, typename _Fn>
inline void splitpath(_Elem* s, size_t size, _Fn func) // will convert '\\' to '/'
{
  auto _Endptr = s + size;
  _splitpath(
      s, [=](_Elem* _Ptr) { return _Ptr < _Endptr; }, func);
}

template <typename _Elem, typename _Fn>
inline void splitpath(_Elem* s, _Fn func) // will convert '\\' to '/'
{
  _splitpath(
      s, [=](_Elem* _Ptr) { return *_Ptr != '\0'; }, func);
}

template <typename _Elem, typename _Fn>
inline void splitpath(std::basic_string<_Elem>& s, _Fn func) // will convert '\\' to '/'
{
  _Elem dummy[1] = {0};
  splitpath(!s.empty() ? &s.front() : &dummy[0], func);
}
} // namespace xsbase
#endif
