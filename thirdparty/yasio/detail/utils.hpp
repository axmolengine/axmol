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
#ifndef YASIO__UTILS_HPP
#define YASIO__UTILS_HPP
#include <assert.h>
#include <sys/stat.h>
#include <chrono>
#include <algorithm>
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
// typedefs
typedef long long highp_time_t;
typedef std::chrono::high_resolution_clock steady_clock_t;
typedef std::chrono::system_clock system_clock_t;

// The high precision nano seconds timestamp since epoch
template <typename _Ty = steady_clock_t>
inline highp_time_t xhighp_clock()
{
  auto duration = _Ty::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}
// The high precision micro seconds timestamp since epoch
template <typename _Ty = steady_clock_t>
inline highp_time_t highp_clock()
{
  return xhighp_clock<_Ty>() / std::milli::den;
}
// The normal precision milli seconds timestamp since epoch
template <typename _Ty = steady_clock_t>
inline highp_time_t clock()
{
  return xhighp_clock<_Ty>() / std::micro::den;
}
// The time now in seconds since epoch, better performance than chrono on win32
// see: win10 sdk ucrt/time/time.cpp:common_time
// https://docs.microsoft.com/en-us/windows/desktop/sysinfo/acquiring-high-resolution-time-stamps
inline highp_time_t time_now() { return ::time(nullptr); }

#if YASIO__HAS_CXX17
using std::clamp;
#else
template <typename _Ty>
const _Ty& clamp(const _Ty& v, const _Ty& lo, const _Ty& hi)
{
  assert(!(hi < lo));
  return v < lo ? lo : hi < v ? hi : v;
}
#endif

template <typename _Ty>
inline void invoke_dtor(_Ty* p)
{
  p->~_Ty();
}

inline bool is_regular_file(const char* path)
{
  struct stat st;
  return (::stat(path, &st) == 0) && (st.st_mode & S_IFREG);
}

} // namespace yasio

#endif
