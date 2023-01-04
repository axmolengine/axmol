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
#ifndef YASIO__SOCKET_HPP
#define YASIO__SOCKET_HPP

#include "yasio/detail/config.hpp"

#ifdef _WIN32
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <WinSock2.h>
#  include <Windows.h>
#  if defined(YASIO_INSIDE_UNREAL)
#    if !defined(TRUE)
#      define TRUE 1
#    endif
#    if !defined(FALSE)
#      define FALSE 0
#    endif
#  endif
#  include <Mswsock.h>
#  include <Mstcpip.h>
#  include <Ws2tcpip.h>
#  if defined(YASIO_NT_COMPAT_GAI)
#    include <Wspiapi.h>
#  endif
#  if YASIO__HAS_UDS
#    include <afunix.h>
#  endif
typedef SOCKET socket_native_type;
typedef int socklen_t;
#  define poll WSAPoll
#  pragma comment(lib, "ws2_32.lib")

#  undef gai_strerror
#else
#  include <unistd.h>
#  include <signal.h>
#  include <sys/ioctl.h>
#  include <netdb.h>
#  include <sys/types.h>
#  include <sys/poll.h>
#  if defined(__linux__)
#    include <sys/epoll.h>
#  endif
#  include <sys/select.h>
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <net/if.h>
#  include <arpa/inet.h>
#  if !defined(SD_RECEIVE)
#    define SD_RECEIVE SHUT_RD
#  endif
#  if !defined(SD_SEND)
#    define SD_SEND SHUT_WR
#  endif
#  if !defined(SD_BOTH)
#    define SD_BOTH SHUT_RDWR
#  endif
#  if !defined(closesocket)
#    define closesocket close
#  endif
#  if !defined(ioctlsocket)
#    define ioctlsocket ioctl
#  endif
#  if defined(__linux__)
#    define SO_NOSIGPIPE MSG_NOSIGNAL
#  endif
typedef int socket_native_type;
#  undef socket
#endif
#define SD_NONE -1

#include <fcntl.h> // common platform header

#include "yasio/detail/endian_portable.hpp"

// redefine socket error code for posix api
#ifdef _WIN32
#  undef EWOULDBLOCK
#  undef EINPROGRESS
#  undef EALREADY
#  undef ENOTSOCK
#  undef EDESTADDRREQ
#  undef EMSGSIZE
#  undef EPROTOTYPE
#  undef ENOPROTOOPT
#  undef EPROTONOSUPPORT
#  undef ESOCKTNOSUPPORT
#  undef EOPNOTSUPP
#  undef EPFNOSUPPORT
#  undef EAFNOSUPPORT
#  undef EADDRINUSE
#  undef EADDRNOTAVAIL
#  undef ENETDOWN
#  undef ENETUNREACH
#  undef ENETRESET
#  undef ECONNABORTED
#  undef ECONNRESET
#  undef ENOBUFS
#  undef EISCONN
#  undef ENOTCONN
#  undef ESHUTDOWN
#  undef ETOOMANYREFS
#  undef ETIMEDOUT
#  undef ECONNREFUSED
#  undef ELOOP
#  undef ENAMETOOLONG
#  undef EHOSTDOWN
#  undef EHOSTUNREACH
#  undef ENOTEMPTY
#  undef EPROCLIM
#  undef EUSERS
#  undef EDQUOT
#  undef ESTALE
#  undef EREMOTE
#  undef EBADF
#  undef EFAULT
#  undef EAGAIN

#  define EWOULDBLOCK WSAEWOULDBLOCK
#  define EINPROGRESS WSAEINPROGRESS
#  define EALREADY WSAEALREADY
#  define ENOTSOCK WSAENOTSOCK
#  define EDESTADDRREQ WSAEDESTADDRREQ
#  define EMSGSIZE WSAEMSGSIZE
#  define EPROTOTYPE WSAEPROTOTYPE
#  define ENOPROTOOPT WSAENOPROTOOPT
#  define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#  define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#  define EOPNOTSUPP WSAEOPNOTSUPP
#  define EPFNOSUPPORT WSAEPFNOSUPPORT
#  define EAFNOSUPPORT WSAEAFNOSUPPORT
#  define EADDRINUSE WSAEADDRINUSE
#  define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#  define ENETDOWN WSAENETDOWN
#  define ENETUNREACH WSAENETUNREACH
#  define ENETRESET WSAENETRESET
#  define ECONNABORTED WSAECONNABORTED
#  define ECONNRESET WSAECONNRESET
#  define ENOBUFS WSAENOBUFS
#  define EISCONN WSAEISCONN
#  define ENOTCONN WSAENOTCONN
#  define ESHUTDOWN WSAESHUTDOWN
#  define ETOOMANYREFS WSAETOOMANYREFS
#  define ETIMEDOUT WSAETIMEDOUT
#  define ECONNREFUSED WSAECONNREFUSED
#  define ELOOP WSAELOOP
#  define ENAMETOOLONG WSAENAMETOOLONG
#  define EHOSTDOWN WSAEHOSTDOWN
#  define EHOSTUNREACH WSAEHOSTUNREACH
#  define ENOTEMPTY WSAENOTEMPTY
#  define EPROCLIM WSAEPROCLIM
#  define EUSERS WSAEUSERS
#  define EDQUOT WSAEDQUOT
#  define ESTALE WSAESTALE
#  define EREMOTE WSAEREMOTE
#  define EBADF WSAEBADF
#  define EFAULT WSAEFAULT
#  define EAGAIN WSATRY_AGAIN
#endif

#if !defined(MAXNS)
#  define MAXNS 3
#endif

#define IN_MAX_ADDRSTRLEN INET6_ADDRSTRLEN

// Workaround for older MinGW missing AI_XXX macros
#if defined(__MINGW32__)
#  if !defined(AI_ALL)
#    define AI_ALL 0x00000100
#  endif
#  if !defined(AI_V4MAPPED)
#    define AI_V4MAPPED 0x00000800
#  endif
#endif

#if !defined(_WS2IPDEF_) || defined(__MINGW32__)
inline bool IN4_IS_ADDR_LOOPBACK(const in_addr* a)
{
  return ((a->s_addr & 0xff) == 0x7f); // 127/8
}
inline bool IN4_IS_ADDR_LINKLOCAL(const in_addr* a)
{
  return ((a->s_addr & 0xffff) == 0xfea9); // 169.254/16
}
inline bool IN6_IS_ADDR_GLOBAL(const in6_addr* a)
{
  //
  // Check the format prefix and exclude addresses
  // whose high 4 bits are all zero or all one.
  // This is a cheap way of excluding v4-compatible,
  // v4-mapped, loopback, multicast, link-local, site-local.
  //
  unsigned int High = (a->s6_addr[0] & 0xf0);
  return ((High != 0) && (High != 0xf0));
}
#endif

#define YASIO_ADDR_ANY(af) (af == AF_INET ? "0.0.0.0" : "::")

#define yasio__setbits(x, m) ((x) |= (m))
#define yasio__clearbits(x, m) ((x) &= ~(m))
#define yasio__testbits(x, m) ((x) & (m))
#define yasio__setlobyte(x, v) ((x) = ((x) & ~((decltype(x))0xff)) | (v))

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
struct socket_event {
  enum
  { // event mask
    null      = 0,
    read      = 1,
    write     = 2,
    error     = 4,
    readwrite = read | write,
  };
};
} // namespace inet
} // namespace yasio
#endif
