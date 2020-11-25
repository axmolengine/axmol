//////////////////////////////////////////////////////////////////////////////////////////
// A cross platform socket APIs, support ios & android & wp8 & window store
// universal app
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2020 HALX99

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
#ifndef YASIO__CONFIG_HPP
#define YASIO__CONFIG_HPP
#include "yasio/compiler/feature_test.hpp"

/*
** Uncomment or add compiler flag -DYASIO_HEADER_ONLY to enable yasio core implementation header
** only
*/
#define YASIO_HEADER_ONLY 1

/*
** Uncomment or add compiler flag -DYASIO_VERBOSE_LOG to enable verbose log
*/
// #define YASIO_VERBOSE_LOG 1

/*
** Uncomment or add compiler flag -DYASIO_USE_SPSC_QUEUE to use SPSC queue in io_service
** Remark: By default, yasio use std's queue + mutex to ensure thread safe, If you want
**         more fast messege queue and only have one thread to call io_service write APIs,
**         you may need uncomment it.
*/
// #define YASIO_USE_SPSC_QUEUE 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_OBJECT_POOL to disable object_pool for allocating
** protocol data unit
*/
// #define YASIO_DISABLE_OBJECT_POOL 1

/*
** Uncomment or add compiler flag -DYASIO_ENABLE_ARES_PROFILER to test async resolve performance
*/
// #define YASIO_ENABLE_ARES_PROFILER 1

/*
** Uncomment or add compiler flag -DYASIO_HAVE_CARES to use c-ares to perform async resolve
*/
// #define YASIO_HAVE_CARES 1

/*
** Uncomment or add compiler flag -DYASIO_HAVE_KCP for kcp support
** Remember, before thus, please ensure:
** 1. Execute: `git submodule update --init --recursive` to clone the kcp sources.
** 2. Add yasio/kcp/ikcp.c to your build system, even through the `YASIO_HEADER_ONLY` was defined.
** pitfall: yasio kcp support is experimental currently.
*/
// #define YASIO_HAVE_KCP 1

/*
** Uncomment or add compiler flag -DYASIO_HAVE_SSL for SSL support
*/
#define YASIO_HAVE_SSL 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_CONCURRENT_SINGLETON to disable concurrent
*singleton
*/
// #define YASIO_DISABLE_CONCURRENT_SINGLETON 1

/*
** Uncomment or add compiler flag -DYASIO_ENABLE_UDS to enable unix domain socket via SOCK_STREAM
*/
// #define YASIO_ENABLE_UDS 1

/*
** Workaround for 'vs2013 without full c++11 support', in the future, drop vs2013 support and
** follow 3 lines code will be removed
*/
#if !YASIO__HAS_FULL_CXX11 && !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
#  define YASIO_DISABLE_CONCURRENT_SINGLETON 1
#endif

/*
** Uncomment or add compiler flag -DYASIO_NT_COMPAT_GAI for earlier versions of Windows XP
** see: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
*/
// #define YASIO_NT_COMPAT_GAI 1

/*
** Uncomment or add compiler flag -DYASIO_MINIFY_EVENT to minfy size of io_event 
*/
// #define YASIO_MINIFY_EVENT 1

/*
** Uncomment or add compiler flag -DYASIO_NO_EXCEPTIONS to disable exceptions
*/
// #define YASIO_NO_EXCEPTIONS 1

#if defined(YASIO_HEADER_ONLY)
#  define YASIO__DECL inline
#else
#  define YASIO__DECL
#endif

#if defined(_WIN32)
#  define YASIO_LOG_TAG(tag, format, ...)                                                          \
    OutputDebugStringA(::yasio::strfmt(127, (tag format "\n"), ##__VA_ARGS__).c_str())
#elif defined(ANDROID) || defined(__ANDROID__)
#  include <android/log.h>
#  include <jni.h>
#  define YASIO_LOG_TAG(tag, format, ...)                                                          \
    __android_log_print(ANDROID_LOG_INFO, "yasio", (tag format), ##__VA_ARGS__)
#else
#  define YASIO_LOG_TAG(tag, format, ...) printf((tag format "\n"), ##__VA_ARGS__)
#endif

#define YASIO_LOG(format, ...) YASIO_LOG_TAG("[yasio]", format, ##__VA_ARGS__)

#if !defined(YASIO_VERBOSE_LOG)
#  define YASIO_LOGV(fmt, ...) (void)0
#else
#  define YASIO_LOGV YASIO_LOG
#endif

#if !defined(YASIO_NO_EXCEPTIONS)
#  define YASIO__THROW(x, retval) throw(x)
#else
#  define YASIO__THROW(x, retval) return (retval)
#endif

#define YASIO_ARRAYSIZE(A) (sizeof(A) / sizeof((A)[0]))

/*
** YASIO_OBSOLETE_DEPRECATE
*/
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement) __attribute__((deprecated))
#elif _MSC_VER >= 1400 // vs 2005 or higher
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement)                                                   \
    __declspec(deprecated(                                                                         \
        "This function will be removed in the future. Consider using " #_Replacement " instead."))
#else
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement)
#endif

/*
**  The yasio version macros
*/
#define YASIO_VERSION_NUM 0x033400

/*
** The macros used by io_service.
*/
// The default max listen count of tcp server.
#define YASIO_SOMAXCONN 19

// The max wait duration in macroseconds when io_service nothing to do.
#define YASIO_MAX_WAIT_DURATION (5LL * 60LL * 1000LL * 1000LL)

// The min wait duration in macroseconds when io_service have outstanding work to do.
// !!!Only affects Single Core CPU
#define YASIO_MIN_WAIT_DURATION 10LL

// The default ttl of multicast
#define YASIO_DEFAULT_MULTICAST_TTL (int)128

#define YASIO_INET_BUFFER_SIZE 65536

/* max pdu buffer length, avoid large memory allocation when application layer decode a huge length
 * field. */
#define YASIO_MAX_PDU_BUFFER_SIZE static_cast<int>(1 * 1024 * 1024)

// The max Initial Bytes To Strip for length field based frame decode mechanism
#define YASIO_MAX_IBTS 32

// The fallback name servers when c-ares can't get name servers from system config,
// For Android 8 or later, yasio will try to retrive through jni automitically,
// For iOS, c-ares doesn't retrive system dns, yasio will try through API 'res_getservers' provided by libresov
// please see:
//   https://github.com/c-ares/c-ares/issues/276
//   https://github.com/c-ares/c-ares/pull/148
//   https://github.com/c-ares/c-ares/pull/29
#define YASIO_CARES_FALLBACK_DNS "8.8.8.8,223.5.5.5,114.114.114.114"

#include "strfmt.hpp"

#endif
