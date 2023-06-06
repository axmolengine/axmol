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
#ifndef YASIO__THREAD_NAME_HPP
#define YASIO__THREAD_NAME_HPP
#if defined(WIN32)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Windows.h>
#else
#  include <thread>
#  if defined(__FreeBSD__) || defined(__OpenBSD__)
#    include <pthread_np.h> // For pthread_getthreadid_np() / pthread_set_name_np()
#  endif
#endif

#if defined(_WIN32)
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#  pragma pack(push, 8)
typedef struct _yasio__thread_info {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} yasio__thread_info;
#  pragma pack(pop)
static void yasio__set_thread_name(const char* threadName)
{
  yasio__thread_info info;
  info.dwType     = 0x1000;
  info.szName     = threadName;
  info.dwThreadID = GetCurrentThreadId();
  info.dwFlags    = 0;
#  if !defined(__MINGW64__) && !defined(__MINGW32__)
  __try
  {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {}
#  endif
}
#elif defined(__linux__)
#  if (defined(__GLIBC__) && defined(_GNU_SOURCE)) || defined(__ANDROID__)
#    define yasio__set_thread_name(name) pthread_setname_np(pthread_self(), name)
#  else
#    define yasio__set_thread_name(name)
#  endif
#elif defined(__APPLE__)
#  define yasio__set_thread_name(name) pthread_setname_np(name)
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
#  define yasio__set_thread_name(name) pthread_set_name_np(pthread_self(), name)
#elif defined(__NetBSD__)
#  define yasio__set_thread_name(name) pthread_setname_np(pthread_self(), "%s", (void*)name);
#else
#  define yasio__set_thread_name(name)
#endif

namespace yasio
{
inline void set_thread_name(const char* name) { yasio__set_thread_name(name); }
} // namespace yasio
#endif
