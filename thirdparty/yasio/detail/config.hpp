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
** Uncomment or add compiler flag -DYASIO_USE_SHARED_PACKET to use std::shared_ptr wrap network packet.
*/
// #define YASIO_USE_SHARED_PACKET 1

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
** Uncomment or add compiler flag -DYASIO_USE_CARES to use c-ares to perform async resolve
*/
// #define YASIO_USE_CARES 1

/*
** Uncomment or add compiler flag -DYASIO_SSL_BACKEND=1 for SSL support with OpenSSL
** 1. -DYASIO_SSL_BACKEND=1: OpenSSL
** 2. -DYASIO_SSL_BACKEND=2: mbedtls
*/
#define YASIO_SSL_BACKEND 1

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
** Uncomment or add compiler flag -DYASIO_NT_COMPAT_GAI for compatible with Windows XP
** see: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
*/
// #define YASIO_NT_COMPAT_GAI 1

/*
** Uncomment or add compiler flag -DYASIO_MINIFY_EVENT to minfy size of io_event
*/
// #define YASIO_MINIFY_EVENT 1

/*
** Uncomment or add compiler flag -DYASIO_ENABLE_HALF_FLOAT to enable half-precision floating-point support
*/
// #define YASIO_ENABLE_HALF_FLOAT 1

/*
** Uncomment or add compiler flag -DYASIO_ENABLE_PASSIVE_EVENT to enable server channel open/close event
*/
// #define YASIO_ENABLE_PASSIVE_EVENT 1

/*
** Uncomment or add compiler flag -DYASIO_NO_USER_TIMER to disable io_channel user_timer
*/
// #define YASIO_NO_USER_TIMER 1

/*
** Uncomment or add compiler flag -DYASIO_OBS_BUILTIN_STACK to enable obstream builtin stack
** for push/pop operations, by default disabled for performance purpose
*/
// #define YASIO_OBS_BUILTIN_STACK 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_POLL to use socket.select for all platforms
** If you need support Windows XP, you need disable poll
*/
// #define YASIO_DISABLE_POLL 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_EPOLL to disable epoll
*/
// #define YASIO_DISABLE_EPOLL 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_KQUEUE to disable kqueue for bsd-like OS
*/
// #define YASIO_DISABLE_KQUEUE 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_EVPORT for SunOS 5.10+
*/
// #define YASIO_DISABLE_EVPORT 1

/*
** Uncomment or add compiler flag -DYASIO_DISABLE_IOCP for Windows
*/
// #define YASIO_DISABLE_IOCP 1

/*
** Uncomment or add compiler flag -DYASIO_ENABLE_WEPOLL for windows
*/
// #define YASIO_ENABLE_WEPOLL 1

/*
** Uncomment or add compiler flag -DYASIO_USE_OPENSSL_BIO to use openssl bio when YASIO_SSL_BACKEND=1
*/
// #define YASIO_USE_OPENSSL_BIO 1

#if defined(__EMSCRIPTEN__) && !defined(YASIO_USE_OPENSSL_BIO)
#  define YASIO_USE_OPENSSL_BIO 1
#endif

#if YASIO__HAS_EPOLL
#  define epoll_close close
typedef int epoll_handle_t;
#elif defined(_WIN32) && defined(YASIO_ENABLE_WEPOLL)
#  include "wepoll/wepoll.h"
#  undef YASIO__HAS_EPOLL
#  define YASIO__HAS_EPOLL 1
typedef HANDLE epoll_handle_t;
#endif

/*
** Workaround for 'vs2013 without full c++11 support', in the future, drop vs2013 support and
** follow 3 lines code will be removed
*/
#if !YASIO__HAS_CXX11 && !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
#  define YASIO_DISABLE_CONCURRENT_SINGLETON 1
#endif

#if defined(YASIO_HEADER_ONLY)
#  define YASIO__DECL inline
#else
#  define YASIO__DECL
#endif

/*
** The interop decl, it's useful for store managed c# function as c++ function pointer properly.
*/
#if !defined(_WIN32) || YASIO__64BITS
#  define YASIO_INTEROP_DECL
#else
#  define YASIO_INTEROP_DECL __stdcall
#endif

#if !defined(YASIO_API)
#  if defined(YASIO_BUILD_SHARED_LIBS) && !defined(YASIO_HEADER_ONLY)
#    if defined(_WIN32) && defined(YASIO_EXPORT_DLL)
#      if defined(YASIO_LIB)
#        define YASIO_API __declspec(dllexport)
#      else
#        define YASIO_API __declspec(dllimport)
#      endif
#    else
#      define YASIO_API
#    endif
#  else
#    define YASIO_API
#  endif
#endif

#define YASIO_ARRAYSIZE(A) (sizeof(A) / sizeof((A)[0]))

#define YASIO_SSIZEOF(T) static_cast<int>(sizeof(T))

// clang-format off
/*
** YASIO_OBSOLETE_DEPRECATE
*/
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement) __attribute__((deprecated))
#elif _MSC_VER >= 1400 // vs 2005 or higher
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement) \
    __declspec(deprecated("This function will be removed in the future. Consider using " #_Replacement " instead."))
#else
#  define YASIO_OBSOLETE_DEPRECATE(_Replacement)
#endif
// clang-format on

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_TEST) || defined(UE_BUILD_SHIPPING) || defined(UE_SERVER)
#  define YASIO_INSIDE_UNREAL 1
#endif // Unreal Engine 4 bullshit

/*
**  The yasio version macros
*/
#define YASIO_VERSION_NUM 0x040000

/*
** The macros used by io_service.
*/
// The default max listen count of tcp server.
#define YASIO_SOMAXCONN 19

// The default ttl of multicast
#define YASIO_DEFAULT_MULTICAST_TTL (int)128

// The max internet buffer size
#define YASIO_INET_BUFFER_SIZE 65536

// The max pdu buffer length, avoid large memory allocation when application decode a huge length.
#define YASIO_MAX_PDU_BUFFER_SIZE static_cast<int>(1 * 1024 * 1024)

// The max Initial Bytes To Strip for unpack.
#define YASIO_UNPACK_MAX_STRIP 32

// The fallback name servers when c-ares can't get name servers from system config,
// For Android 8 or later, yasio will try to retrive through jni automitically,
// For iOS, since c-ares-1.16.1, it will use libresolv for retrieving DNS servers.
// please see:
//   https://c-ares.haxx.se/changelog.html
//   https://github.com/c-ares/c-ares/issues/276
//   https://github.com/c-ares/c-ares/pull/148
#define YASIO_FALLBACK_NAME_SERVERS "8.8.8.8,223.5.5.5,114.114.114.114"

// Since ubuntu 16.04, the /etc/resolv.conf control by systemd-resolved service,
// and the preferred non loopback name server was store to /run/systemd/resolve/resolv.conf
// refer to: https://unix.stackexchange.com/questions/612416/why-does-etc-resolv-conf-point-at-127-0-0-53
#define YASIO_SYSTEMD_RESOLV_PATH "/run/systemd/resolve/resolv.conf"
#define YASIO_SYSTEMD_RESOLV_PATH_LEN (sizeof(YASIO_SYSTEMD_RESOLV_PATH) - 1)

// The yasio ssl client PIN for server to recognize
#define YASIO_SSL_PIN "yasio_ssl_client"
#define YASIO_SSL_PIN_LEN (sizeof(YASIO_SSL_PIN) - 1)

#define YASIO_SSL_PON "yasio_ssl_server"
#define YASIO_SSL_PON_LEN (sizeof(YASIO_SSL_PON) - 1)

// The msg flag for socket.send
// Linux: MSG_NOSIGNAL as to socket.send flag to ignore SIGPIPE
// BSDs: use setsockopt SO_NOSIGPIPE to ignore SIGPIPE
// Refer to: https://linux.die.net/man/2/send
#if defined(__linux__)
#  define YASIO_MSG_FLAG MSG_NOSIGNAL
#else
#  define YASIO_MSG_FLAG 0
#endif

#endif
