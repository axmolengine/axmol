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
*/

/*
 * The standard split stub of yasio:
 *   The pred callback prototype: [](CStr first, CStr last) ->bool{ return true; }
 *   returns:
 *     true: want continue split
 *     false: abort split
 *
 */

#pragma once

#include "yasio/string_view.hpp"
#include <string.h>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4706)
#endif

namespace yasio
{
template <typename _CStr, typename _Pred>
inline void split_if(_CStr s, typename std::remove_pointer<_CStr>::type delim, _Pred&& pred)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  while ((_Ptr = strchr(_Ptr, delim)))
  {
    if (_Start <= _Ptr && !pred(_Start, _Ptr))
      return;
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  pred(_Start, nullptr); // last one, end is nullptr
}

template <typename _CStr, typename _Pred>
inline void split_if_n(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim, _Pred&& pred)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = strchr(_Ptr, delim)))
  {
    if (_Ptr >= _End)
      break;

    if (_Start <= _Ptr && !pred(_Start, _Ptr))
      return;
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start <= _End)
    pred(_Start, _End);
}

template <typename _CStr, typename _Func>
inline void split(_CStr s, typename std::remove_pointer<_CStr>::type delim, _Func&& func)
{
  split_if(s, delim, [func](_CStr first, _CStr last) {
    func(first, last);
    return true;
  });
}

template <typename _CStr, typename _Func>
inline void split_n(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim, _Func&& func)
{
  split_if_n(s, slen, delim, [func](_CStr first, _CStr last) {
    func(first, last);
    return true;
  });
}

struct split_term {
  split_term(char* end)
  {
    if (end)
    {
      this->val_ = *end;
      *end       = '\0';
      this->end_ = end;
    }
  }
  ~split_term()
  {
    if (this->end_)
      *this->end_ = this->val_;
  }

private:
  char* end_ = nullptr;
  char val_  = '\0';
};
} // namespace yasio

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
