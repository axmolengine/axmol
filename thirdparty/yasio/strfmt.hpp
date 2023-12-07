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
*/
#ifndef YASIO__SFMT_HPP
#define YASIO__SFMT_HPP

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
// yasio::format_traits: the vsnprintf/vswprintf wrapper
// !!! if '_BufferCount' had been sufficiently large, not counting the terminating null character.
template <typename _Elem> struct format_traits {};
template <> struct format_traits<char> {
  static int format(char* const _Buffer, size_t const _BufferCount, char const* const _Format, va_list _ArgList)
  {
    return vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
  }
  static const char* report_error() { return "yasio::_strfmt: an error is encountered!"; }
};
template <> struct format_traits<wchar_t> {
  static int format(wchar_t* const _Buffer, size_t const _BufferCount, wchar_t const* const _Format, va_list _ArgList)
  {
    return vswprintf(_Buffer, _BufferCount, _Format, _ArgList);
  }
  static const wchar_t* report_error() { return L"yasio::_strfmt: an error is encountered!"; }
};

template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline bool _vstrfmt(std::basic_string<_Elem, _Traits, _Alloc>& buf, size_t initial_buf_size, const _Elem* format, va_list initialized_args)
{
  bool ok = true;
  va_list args;

  auto offset = buf.size();
  buf.resize(offset + initial_buf_size);

  va_copy(args, initialized_args);
  int nret = format_traits<_Elem>::format(&buf.front() + offset, buf.length() - offset + 1, format, args);
  va_end(args);

  if (nret >= 0)
  {
    if ((unsigned int)nret < (buf.length() - offset))
    {
      buf.resize(offset + nret);
    }
    else if ((unsigned int)nret > (buf.length() - offset))
    { // handle return required length when buffer insufficient
      buf.resize(offset + nret);

      va_copy(args, initialized_args);
      nret = format_traits<_Elem>::format(&buf.front() + offset, buf.length() - offset + 1, format, args);
      va_end(args);
    }
    // else equals, do nothing.
  }
  else
  { // handle return -1 when buffer insufficient
    /*
    return -1 when the output was truncated:
      - vsnprintf: vs2013/older & glibc <= 2.0.6
      - vswprintf: all standard implemetations
    references:
      - https://man7.org/linux/man-pages/man3/vsnprintf.3.html
      - https://www.cplusplus.com/reference/cstdio/vsnprintf/
      - https://www.cplusplus.com/reference/cwchar/vswprintf/
      - https://stackoverflow.com/questions/51134188/why-vsnwprintf-missing
      - https://stackoverflow.com/questions/10446754/vsnwprintf-alternative-on-linux

    */
    enum : size_t
    {
      enlarge_limits = (1 << 20), // limits the buffer cost memory less than 2MB
    };
    do
    {
      buf.resize(offset + ((buf.length() - offset) << 1));

      va_copy(args, initialized_args);
      nret = format_traits<_Elem>::format(&buf.front() + offset, buf.length() - offset + 1, format, args);
      va_end(args);

    } while (nret < 0 && (buf.length() - offset) <= enlarge_limits);
    if (nret > 0)
      buf.resize(offset + nret);
    else
      ok = false;
  }

  return ok;
}

template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> _vstrfmt(size_t initial_buf_size, const _Elem* format, va_list initialized_args)
{
  std::basic_string<_Elem, _Traits, _Alloc> buf;
  if (!_vstrfmt(buf, initial_buf_size, format, initialized_args))
  {
    buf.clear();
    buf.shrink_to_fit();
    buf = format_traits<_Elem>::report_error();
  }
  return buf;
}

template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> basic_strfmt(size_t n, const _Elem* format, ...)
{
  va_list initialized_args;

  va_start(initialized_args, format);
  auto buf = _vstrfmt(n, format, initialized_args);
  va_end(initialized_args);

  return buf;
}

static auto YASIO__CONSTEXPR strfmt = basic_strfmt<char>;
static auto YASIO__CONSTEXPR wcsfmt = basic_strfmt<wchar_t>;

} // namespace yasio

#endif
