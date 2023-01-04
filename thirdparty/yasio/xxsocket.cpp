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
#ifndef YASIO__XXSOCKET_CPP
#define YASIO__XXSOCKET_CPP
#include <assert.h>
#ifdef _DEBUG
#  include <stdio.h>
#endif

#if !defined(YASIO_HEADER_ONLY)
#  include "yasio/xxsocket.hpp"
#endif

#include "yasio/detail/utils.hpp"

#if !defined(_WIN32)
#  include "yasio/detail/ifaddrs.hpp"
#endif

// For apple bsd socket implemention
#if !defined(TCP_KEEPIDLE)
#  define TCP_KEEPIDLE TCP_KEEPALIVE
#endif

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif

#if defined(_WIN32)
static LPFN_ACCEPTEX __accept_ex                           = nullptr;
static LPFN_GETACCEPTEXSOCKADDRS __get_accept_ex_sockaddrs = nullptr;
static LPFN_CONNECTEX __connect_ex                         = nullptr;
#endif

#if !YASIO__HAS_NTOP
namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
YASIO__NS_INLINE
namespace ip
{
namespace compat
{
#  include "yasio/detail/inet_compat.inl"
} // namespace compat
} // namespace ip
} // namespace inet
} // namespace yasio
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{

int xxsocket::xpconnect(const char* hostname, u_short port, u_short local_port)
{
  auto flags = getipsv();

  int error = -1;

  xxsocket::resolve_i(
      [&](const endpoint& ep) {
        switch (ep.af())
        {
          case AF_INET:
            if (flags & ipsv_ipv4)
            {
              error = pconnect(ep, local_port);
            }
            else if (flags & ipsv_ipv6)
            {
              xxsocket::resolve_i([&](const endpoint& ep6) { return 0 == (error = pconnect(ep6, local_port)); }, hostname, port, AF_INET6, AI_V4MAPPED);
            }
            break;
          case AF_INET6:
            if (flags & ipsv_ipv6)
              error = pconnect(ep, local_port);
            break;
        }

        return error == 0;
      },
      hostname, port, AF_UNSPEC, AI_ALL);

  return error;
}

int xxsocket::xpconnect_n(const char* hostname, u_short port, const std::chrono::microseconds& wtimeout, u_short local_port)
{
  auto flags = getipsv();
  int error  = -1;
  xxsocket::resolve_i(
      [&](const endpoint& ep) {
        switch (ep.af())
        {
          case AF_INET:
            if (flags & ipsv_ipv4)
              error = pconnect_n(ep, wtimeout, local_port);
            else if (flags & ipsv_ipv6)
            {
              xxsocket::resolve_i([&](const endpoint& ep6) { return 0 == (error = pconnect_n(ep6, wtimeout, local_port)); }, hostname, port, AF_INET6,
                                  AI_V4MAPPED);
            }
            break;
          case AF_INET6:
            if (flags & ipsv_ipv6)
              error = pconnect_n(ep, wtimeout, local_port);
            break;
        }

        return error == 0;
      },
      hostname, port, AF_UNSPEC, AI_ALL);

  return error;
}

int xxsocket::pconnect(const char* hostname, u_short port, u_short local_port)
{
  int error = -1;
  xxsocket::resolve_i([&](const endpoint& ep) { return 0 == (error = pconnect(ep, local_port)); }, hostname, port);
  return error;
}

int xxsocket::pconnect_n(const char* hostname, u_short port, const std::chrono::microseconds& wtimeout, u_short local_port)
{
  int error = -1;
  xxsocket::resolve_i([&](const endpoint& ep) { return 0 == (error = pconnect_n(ep, wtimeout, local_port)); }, hostname, port);
  return error;
}

int xxsocket::pconnect_n(const char* hostname, u_short port, u_short local_port)
{
  int error = -1;
  xxsocket::resolve_i(
      [&](const endpoint& ep) {
        (error = pconnect_n(ep, local_port));
        return true;
      },
      hostname, port);
  return error;
}

int xxsocket::pconnect(const endpoint& ep, u_short local_port)
{
  if (this->reopen(ep.af()))
  {
    if (local_port != 0)
      this->bind(YASIO_ADDR_ANY(ep.af()), local_port);
    return this->connect(ep);
  }
  return -1;
}

int xxsocket::pconnect_n(const endpoint& ep, const std::chrono::microseconds& wtimeout, u_short local_port)
{
  if (this->reopen(ep.af()))
  {
    if (local_port != 0)
      this->bind(YASIO_ADDR_ANY(ep.af()), local_port);
    return this->connect_n(ep, wtimeout);
  }
  return -1;
}

int xxsocket::pconnect_n(const endpoint& ep, u_short local_port)
{
  if (this->reopen(ep.af()))
  {
    if (local_port != 0)
      this->bind(YASIO_ADDR_ANY(ep.af()), local_port);
    return xxsocket::connect_n(this->fd, ep);
  }
  return -1;
}

int xxsocket::pserve(const char* addr, u_short port) { return this->pserve(endpoint{addr, port}); }
int xxsocket::pserve(const endpoint& ep)
{
  if (!this->reopen(ep.af()))
    return -1;

  set_optval(SOL_SOCKET, SO_REUSEADDR, 1);

  int n = this->bind(ep);
  if (n != 0)
    return n;

  return this->listen();
}

int xxsocket::resolve(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port, int socktype)
{
  return resolve_i(
      [&](const endpoint& ep) {
        endpoints.push_back(ep);
        return false;
      },
      hostname, port, AF_UNSPEC, AI_ALL, socktype);
}
int xxsocket::resolve_v4(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port, int socktype)
{
  return resolve_i(
      [&](const endpoint& ep) {
        endpoints.push_back(ep);
        return false;
      },
      hostname, port, AF_INET, 0, socktype);
}
int xxsocket::resolve_v6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port, int socktype)
{
  return resolve_i(
      [&](const endpoint& ep) {
        endpoints.push_back(ep);
        return false;
      },
      hostname, port, AF_INET6, 0, socktype);
}
int xxsocket::resolve_v4to6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port, int socktype)
{
  return xxsocket::resolve_i(
      [&](const endpoint& ep) {
        endpoints.push_back(ep);
        return false;
      },
      hostname, port, AF_INET6, AI_V4MAPPED, socktype);
}
int xxsocket::resolve_tov6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port, int socktype)
{
  return resolve_i(
      [&](const endpoint& ep) {
        endpoints.push_back(ep);
        return false;
      },
      hostname, port, AF_INET6, AI_ALL | AI_V4MAPPED, socktype);
}

int xxsocket::getipsv(void)
{
  int flags = 0;
  xxsocket::traverse_local_address([&](const ip::endpoint& ep) -> bool {
    switch (ep.af())
    {
      case AF_INET:
        flags |= ipsv_ipv4;
        break;
      case AF_INET6:
        flags |= ipsv_ipv6;
        break;
    }
    return (flags == ipsv_dual_stack);
  });
  YASIO_LOG("xxsocket::getipsv: flags=%d", flags);
  return flags;
}

void xxsocket::traverse_local_address(std::function<bool(const ip::endpoint&)> handler)
{
  /* Only windows support use getaddrinfo to get local ip address(not loopback or linklocal),
    Because nullptr same as "localhost": always return loopback address and at unix/linux the
    gethostname always return "localhost"
    */
#if defined(_WIN32)
  char hostname[256] = {0};
  ::gethostname(hostname, sizeof(hostname));
#  if defined(_DEBUG)
  YASIO_LOG("xxsocket::traverse_local_address: localhost=%s", hostname);
#  endif

  // ipv4 & ipv6
  addrinfo hint, *ailist = nullptr;
  ::memset(&hint, 0x0, sizeof(hint));

  endpoint ep;
  int iret = getaddrinfo(hostname, nullptr, &hint, &ailist);
  (void)iret;
  if (ailist != nullptr)
  {
    for (auto aip = ailist; aip != nullptr; aip = aip->ai_next)
    {
      if (ep.as_is(aip))
      {
        YASIO_LOGV("xxsocket::traverse_local_address: ip=%s", ep.ip().c_str());
        if (ep.is_global())
        {
          if (handler(ep))
            break;
        }
      }
    }
    freeaddrinfo(ailist);
  }
  else
    YASIO_LOGV("xxsocket::traverse_local_address fail with %s", xxsocket::gai_strerror(iret));
#else // unix like systems with <ifaddrs.h>
  struct ifaddrs *ifaddr, *ifa;
  /*
  The value of ifa->ifa_name:
   Android:
    wifi: "w"
    cellular: "r"
   iOS:
    wifi: "en0"
    cellular: "pdp_ip0"
  */

  if (yasio::getifaddrs(&ifaddr) == -1)
  {
    YASIO_LOG("xxsocket::traverse_local_address: getifaddrs fail!");
    return;
  }

  endpoint ep;
  /* Walk through linked list*/
  for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == nullptr)
      continue;
    if (ep.as_is(ifa->ifa_addr))
    {
      YASIO_LOGV("xxsocket::traverse_local_address: ip=%s", ep.ip().c_str());
      if (ep.is_global())
      {
        if (handler(ep))
          break;
      }
    }
  }

  yasio::freeifaddrs(ifaddr);
#endif
}

xxsocket::xxsocket(void) : fd(invalid_socket) {}
xxsocket::xxsocket(socket_native_type h) : fd(h) {}
xxsocket::xxsocket(xxsocket&& right) YASIO__NOEXCEPT : fd(invalid_socket) { swap(right); }
xxsocket::xxsocket(int af, int type, int protocol) : fd(invalid_socket) { open(af, type, protocol); }
xxsocket::~xxsocket(void) { close(); }

xxsocket& xxsocket::operator=(socket_native_type handle) YASIO__NOEXCEPT
{
  if (!this->is_open())
    this->fd = handle;
  return *this;
}
xxsocket& xxsocket::operator=(xxsocket&& right) YASIO__NOEXCEPT { return swap(right); }

xxsocket& xxsocket::swap(xxsocket& rhs)
{
  std::swap(this->fd, rhs.fd);
  return *this;
}

bool xxsocket::open(int af, int type, int protocol)
{
  if (invalid_socket == this->fd)
    this->fd = ::socket(af, type, protocol);
  return is_open();
}

bool xxsocket::reopen(int af, int type, int protocol)
{
  this->close();
  return this->open(af, type, protocol);
}

#if defined(_WIN32)
bool xxsocket::open_ex(int af, int type, int protocol)
{
  if (invalid_socket == this->fd)
  {
    this->fd = ::WSASocket(af, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);

    DWORD dwBytes = 0;
    if (nullptr == __accept_ex)
    {
      GUID guidAcceptEx = WSAID_ACCEPTEX;
      (void)WSAIoctl(this->fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &__accept_ex, sizeof(__accept_ex), &dwBytes, nullptr,
                     nullptr);
    }

    if (nullptr == __connect_ex)
    {
      GUID guidConnectEx = WSAID_CONNECTEX;
      (void)WSAIoctl(this->fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &__connect_ex, sizeof(__connect_ex), &dwBytes,
                     nullptr, nullptr);
    }

    if (nullptr == __get_accept_ex_sockaddrs)
    {
      GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
      (void)WSAIoctl(this->fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs), &__get_accept_ex_sockaddrs,
                     sizeof(__get_accept_ex_sockaddrs), &dwBytes, nullptr, nullptr);
    }
  }
  return is_open();
}

bool xxsocket::accept_ex(SOCKET sockfd_listened, SOCKET sockfd_prepared, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength,
                         DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped)
{
  return !!__accept_ex(sockfd_listened, sockfd_prepared, lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived,
                       lpOverlapped);
}

bool xxsocket::connect_ex(SOCKET s, const struct sockaddr* name, int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength, LPDWORD lpdwBytesSent,
                          LPOVERLAPPED lpOverlapped)
{
  return !!__connect_ex(s, name, namelen, lpSendBuffer, dwSendDataLength, lpdwBytesSent, lpOverlapped);
}

void xxsocket::translate_sockaddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
                                   sockaddr** LocalSockaddr, LPINT LocalSockaddrLength, sockaddr** RemoteSockaddr, LPINT RemoteSockaddrLength)
{
  __get_accept_ex_sockaddrs(lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, LocalSockaddr, LocalSockaddrLength,
                            RemoteSockaddr, RemoteSockaddrLength);
}
#endif

bool xxsocket::is_open(void) const { return this->fd != invalid_socket; }

socket_native_type xxsocket::native_handle(void) const { return this->fd; }
socket_native_type xxsocket::release_handle(void)
{
  socket_native_type result = this->fd;
  this->fd                  = invalid_socket;
  return result;
}

int xxsocket::set_nonblocking(bool nonblocking) const { return set_nonblocking(this->fd, nonblocking); }
int xxsocket::set_nonblocking(socket_native_type s, bool nonblocking)
{
#if defined(_WIN32)
  u_long argp = nonblocking;
  return ::ioctlsocket(s, FIONBIO, &argp);
#else
  int flags = ::fcntl(s, F_GETFL, 0);
  return ::fcntl(s, F_SETFL, nonblocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
#endif
}

int xxsocket::test_nonblocking() const { return xxsocket::test_nonblocking(this->fd); }
int xxsocket::test_nonblocking(socket_native_type s)
{
#if defined(_WIN32)
  int r = 0;
  unsigned char b[1];
  r = xxsocket::recv(s, b, 0, 0);
  if (r == 0)
    return 1;
  else if (r == -1 && GetLastError() == WSAEWOULDBLOCK)
    return 0;
  return -1; /* In  case it is a connection socket (TCP) and it is not in connected state you will get here 10060 */
#else
  int flags = ::fcntl(s, F_GETFL, 0);
  return flags & O_NONBLOCK;
#endif
}

int xxsocket::bind(const char* addr, unsigned short port) const { return this->bind(endpoint(addr, port)); }
int xxsocket::bind(const endpoint& ep) const { return ::bind(this->fd, &ep, ep.len()); }
int xxsocket::bind_any(bool ipv6) const { return this->bind(endpoint(!ipv6 ? "0.0.0.0" : "::", 0)); }

int xxsocket::listen(int backlog) const { return ::listen(this->fd, backlog); }

xxsocket xxsocket::accept() const { return ::accept(this->fd, nullptr, nullptr); }
int xxsocket::accept_n(socket_native_type& new_sock) const
{
  for (;;)
  {
    // Accept the waiting connection.
    new_sock = ::accept(this->fd, nullptr, nullptr);

    // Check if operation succeeded.
    if (new_sock != invalid_socket)
    {
      xxsocket::set_nonblocking(new_sock, true);
      return 0;
    }

    auto error = get_last_errno();
    // Retry operation if interrupted by signal.
    if (error == EINTR)
      continue;

    /* Operation failed.
    ** The error maybe EWOULDBLOCK, EAGAIN, ECONNABORTED, EPROTO,
    ** Simply Fall through to retry operation.
    */
    return error;
  }
}

int xxsocket::connect(const char* addr, u_short port) { return connect(endpoint(addr, port)); }
int xxsocket::connect(const endpoint& ep) { return xxsocket::connect(fd, ep); }
int xxsocket::connect(socket_native_type s, const char* addr, u_short port)
{
  endpoint peer(addr, port);

  return xxsocket::connect(s, peer);
}
int xxsocket::connect(socket_native_type s, const endpoint& ep) { return ::connect(s, &ep, ep.len()); }

int xxsocket::connect_n(const char* addr, u_short port, const std::chrono::microseconds& wtimeout) { return connect_n(ip::endpoint(addr, port), wtimeout); }
int xxsocket::connect_n(const endpoint& ep, const std::chrono::microseconds& wtimeout) { return this->connect_n(this->fd, ep, wtimeout); }
int xxsocket::connect_n(socket_native_type s, const endpoint& ep, const std::chrono::microseconds& wtimeout)
{
  fd_set rset, wset;
  int ret, error = 0;

  set_nonblocking(s, true);

  if ((ret = xxsocket::connect(s, ep)) < 0)
  {
    error = xxsocket::get_last_errno();
    if (error != EINPROGRESS && error != EWOULDBLOCK)
      return -1;
  }

  /* Do whatever we want while the connect is taking place. */
  if (ret == 0)
    goto done; /* connect completed immediately */

  if (xxsocket::select(s, &rset, &wset, nullptr, wtimeout) <= 0)
    error = xxsocket::get_last_errno();
  else if ((FD_ISSET(s, &rset) || FD_ISSET(s, &wset)))
  { /* Everythings are ok */
    socklen_t len = sizeof(error);
    if (::getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
      return (-1); /* Solaris pending error */
  }

done:
  if (error != 0)
  {
    ::closesocket(s); /* just in case */
    return (-1);
  }

  /* Since v3.31.2, we don't restore file status flags for unify behavior for all platforms */
  // pitfall: because on win32, there is no way to test whether the s is non-blocking
  // so, can't restore properly
  return (0);
}

int xxsocket::connect_n(const endpoint& ep) { return xxsocket::connect_n(this->fd, ep); }
int xxsocket::connect_n(socket_native_type s, const endpoint& ep)
{
  set_nonblocking(s, true);

  return xxsocket::connect(s, ep);
}

int xxsocket::disconnect() const { return xxsocket::disconnect(this->fd); }
int xxsocket::disconnect(socket_native_type s)
{
#if defined(_WIN32)
  sockaddr_storage addr_unspec{0};
  return ::connect(s, (sockaddr*)&addr_unspec, sizeof(addr_unspec));
#else
#  if defined(__MVS__)
  sockaddr_storage addr_unspec{0};
#  else
  sockaddr addr_unspec{0};
  addr_unspec.sa_family = AF_UNSPEC;
#  endif
  int ret, error;
  for (;;)
  {
    ret = ::connect(s, &addr_unspec, sizeof(addr_unspec));
    if (ret == 0)
      return 0;
    if ((error = xxsocket::get_last_errno()) == EINTR)
      continue;
#  if YASIO__OS_BSD_LIKE
    /*
     * From kernel source code of FreeBSD,NetBSD,OpenBSD,etc.
     * The udp socket will be success disconnected by kernel function: `sodisconnect(upic_socket.c)`, then in the kernel, will continue try to
     * connect with new sockaddr, but will failed with follow errno:
     * a. EINVAL: addrlen mismatch
     * b. EAFNOSUPPORT: family mismatch
     * So, we just simply ignore them for the disconnect behavior.
     */
    return (error == EAFNOSUPPORT || error == EINVAL) ? 0 : -1;
#  else
    return ret;
#  endif
  }
#endif
}

int xxsocket::send_n(const void* buf, int len, const std::chrono::microseconds& wtimeout, int flags)
{
  return this->send_n(this->fd, buf, len, wtimeout, flags);
}
int xxsocket::send_n(socket_native_type s, const void* buf, int len, std::chrono::microseconds wtimeout, int flags)
{
  int bytes_transferred = 0;
  int n;
  int error = 0;

  xxsocket::set_nonblocking(s, true);

  for (; bytes_transferred < len;)
  {
    // Try to transfer as much of the remaining data as possible.
    // Since the socket is in non-blocking mode, this call will not
    // block.
    n = xxsocket::send(s, (const char*)buf + bytes_transferred, len - bytes_transferred, flags);
    if (n > 0)
    {
      bytes_transferred += n;
      continue;
    }

    // Check for possible blocking.
    error = xxsocket::get_last_errno();
    if (n == -1 && xxsocket::not_send_error(error))
    {
      // Wait upto <timeout> for the blocking to subside.
      auto start    = yasio::highp_clock();
      int const rtn = handle_write_ready(s, wtimeout);
      wtimeout -= std::chrono::microseconds(yasio::highp_clock() - start);

      // Did select() succeed?
      if (rtn != -1 && wtimeout.count() > 0)
      {
        // Blocking subsided in <timeout> period.  Continue
        // data transfer.
        continue;
      }
    }

    // Wait in select() timed out or other data transfer or
    // select() failures.
    break;
  }

  return bytes_transferred;
}

int xxsocket::recv_n(void* buf, int len, const std::chrono::microseconds& wtimeout, int flags) const
{
  return this->recv_n(this->fd, buf, len, wtimeout, flags);
}
int xxsocket::recv_n(socket_native_type s, void* buf, int len, std::chrono::microseconds wtimeout, int flags)
{
  int bytes_transferred = 0;
  int n;
  int error = 0;

  xxsocket::set_nonblocking(s, true);

  for (; bytes_transferred < len;)
  {
    // Try to transfer as much of the remaining data as possible.
    // Since the socket is in non-blocking mode, this call will not
    // block.
    n = xxsocket::recv(s, static_cast<char*>(buf) + bytes_transferred, len - bytes_transferred, flags);
    if (n > 0)
    {
      bytes_transferred += n;
      continue;
    }

    // Check for possible blocking.
    error = xxsocket::get_last_errno();
    if (n == -1 && xxsocket::not_recv_error(error))
    {
      // Wait upto <timeout> for the blocking to subside.
      auto start    = yasio::highp_clock();
      int const rtn = handle_read_ready(s, wtimeout);
      wtimeout -= std::chrono::microseconds(yasio::highp_clock() - start);

      // Did select() succeed?
      if (rtn != -1 && wtimeout.count() > 0)
      {
        // Blocking subsided in <timeout> period.  Continue
        // data transfer.
        continue;
      }
    }

    // Wait in select() timed out or other data transfer or
    // select() failures.
    break;
  }

  return bytes_transferred;
}

int xxsocket::send(const void* buf, int len, int flags) const { return static_cast<int>(::send(this->fd, (const char*)buf, len, flags)); }
int xxsocket::send(socket_native_type s, const void* buf, int len, int flags) { return static_cast<int>(::send(s, (const char*)buf, len, flags)); }

int xxsocket::recv(void* buf, int len, int flags) const { return static_cast<int>(this->recv(this->fd, buf, len, flags)); }
int xxsocket::recv(socket_native_type s, void* buf, int len, int flags) { return static_cast<int>(::recv(s, (char*)buf, len, flags)); }

int xxsocket::sendto(const void* buf, int len, const endpoint& to, int flags) const
{
  return static_cast<int>(::sendto(this->fd, (const char*)buf, len, flags, &to, to.len()));
}

int xxsocket::recvfrom(void* buf, int len, endpoint& from, int flags) const
{
  socklen_t addrlen{sizeof(from)};
  int n = static_cast<int>(::recvfrom(this->fd, (char*)buf, len, flags, &from, &addrlen));
  from.len(addrlen);
  return n;
}

int xxsocket::handle_write_ready(const std::chrono::microseconds& wtimeout) const { return handle_write_ready(this->fd, wtimeout); }
int xxsocket::handle_write_ready(socket_native_type s, const std::chrono::microseconds& wtimeout)
{
  fd_set writefds;
  return xxsocket::select(s, nullptr, &writefds, nullptr, wtimeout);
}

int xxsocket::handle_read_ready(const std::chrono::microseconds& wtimeout) const { return handle_read_ready(this->fd, wtimeout); }
int xxsocket::handle_read_ready(socket_native_type s, const std::chrono::microseconds& wtimeout)
{
  fd_set readfds;
  return xxsocket::select(s, &readfds, nullptr, nullptr, wtimeout);
}

int xxsocket::select(socket_native_type s, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, std::chrono::microseconds wtimeout)
{
  int n = 0;

  for (;;)
  {
    reregister_descriptor(s, readfds);
    reregister_descriptor(s, writefds);
    reregister_descriptor(s, exceptfds);

    timeval waitd_tv = {static_cast<decltype(timeval::tv_sec)>(wtimeout.count() / std::micro::den),
                        static_cast<decltype(timeval::tv_usec)>(wtimeout.count() % std::micro::den)};
    long long start  = highp_clock();
    n                = ::select(static_cast<int>(s + 1), readfds, writefds, exceptfds, &waitd_tv);
    wtimeout -= std::chrono::microseconds(highp_clock() - start);

    if (n < 0 && xxsocket::get_last_errno() == EINTR)
    {
      if (wtimeout.count() > 0)
        continue;
      n = 0;
    }

    if (n == 0)
      xxsocket::set_last_errno(ETIMEDOUT);
    break;
  }

  return n;
}

void xxsocket::reregister_descriptor(socket_native_type s, fd_set* fds)
{
  if (fds)
  {
    FD_ZERO(fds);
    FD_SET(s, fds);
  }
}

endpoint xxsocket::local_endpoint(void) const { return local_endpoint(this->fd); }
endpoint xxsocket::local_endpoint(socket_native_type fd)
{
  endpoint ep;
  socklen_t socklen = sizeof(ep);
  getsockname(fd, &ep, &socklen);
  ep.len(socklen);
  return ep;
}

endpoint xxsocket::peer_endpoint(void) const { return peer_endpoint(this->fd); }
endpoint xxsocket::peer_endpoint(socket_native_type fd)
{
  endpoint ep;
  socklen_t socklen = sizeof(ep);
  getpeername(fd, &ep, &socklen);
  ep.len(socklen);
  return ep;
}

int xxsocket::set_keepalive(int flag, int idle, int interval, int probes) { return set_keepalive(this->fd, flag, idle, interval, probes); }
int xxsocket::set_keepalive(socket_native_type s, int flag, int idle, int interval, int probes)
{
#if defined(_WIN32)
  tcp_keepalive buffer_in;
  buffer_in.onoff             = flag;
  buffer_in.keepalivetime     = idle * 1000;
  buffer_in.keepaliveinterval = interval * 1000;
  return WSAIoctl(s, SIO_KEEPALIVE_VALS, &buffer_in, sizeof(buffer_in), nullptr, 0, (DWORD*)&probes, nullptr, nullptr);
#else
  if (set_optval(s, SOL_SOCKET, SO_KEEPALIVE, flag) != 0)
    return -1;
  if (set_optval(s, IPPROTO_TCP, TCP_KEEPIDLE, idle) != 0)
    return -1;
  if (set_optval(s, IPPROTO_TCP, TCP_KEEPINTVL, interval) != 0)
    return -1;
  return set_optval(s, IPPROTO_TCP, TCP_KEEPCNT, probes);
#endif
}

void xxsocket::reuse_address(bool reuse)
{
  int optval = reuse ? 1 : 0;

  // All operating systems have 'SO_REUSEADDR'
  this->set_optval(SOL_SOCKET, SO_REUSEADDR, optval);
#if defined(SO_REUSEPORT) // macos,ios,linux,android
  this->set_optval(SOL_SOCKET, SO_REUSEPORT, optval);
#endif
}

void xxsocket::exclusive_address(bool exclusive)
{
#if defined(SO_EXCLUSIVEADDRUSE)
  this->set_optval(SOL_SOCKET, SO_EXCLUSIVEADDRUSE, exclusive ? 1 : 0);
#elif defined(SO_EXCLBIND)
  this->set_optval(SOL_SOCKET, SO_EXCLBIND, exclusive ? 1 : 0);
#endif
}

xxsocket::operator socket_native_type(void) const { return this->fd; }

int xxsocket::shutdown(int how) const { return ::shutdown(this->fd, how); }

void xxsocket::close(int shut_how)
{
  if (is_open())
  {
    if (shut_how >= 0)
      ::shutdown(this->fd, shut_how);
    ::closesocket(this->fd);
    this->fd = invalid_socket;
  }
}

unsigned int xxsocket::tcp_rtt() const { return xxsocket::tcp_rtt(this->fd); }
unsigned int xxsocket::tcp_rtt(socket_native_type s)
{
#if defined(_WIN32)
#  if defined(NTDDI_WIN10_RS2) && NTDDI_VERSION >= NTDDI_WIN10_RS2
  TCP_INFO_v0 info;
  DWORD tcpi_ver = 0, bytes_transferred = 0;
  int status = WSAIoctl(s, SIO_TCP_INFO,
                        (LPVOID)&tcpi_ver,           // lpvInBuffer pointer to a DWORD, version of tcp info
                        (DWORD)sizeof(tcpi_ver),     // size, in bytes, of the input buffer
                        (LPVOID)&info,               // pointer to a TCP_INFO_v0 structure
                        (DWORD)sizeof(info),         // size of the output buffer
                        (LPDWORD)&bytes_transferred, // number of bytes returned
                        (LPWSAOVERLAPPED) nullptr,   // OVERLAPPED structure
                        (LPWSAOVERLAPPED_COMPLETION_ROUTINE) nullptr);
  /*
  info.RttUs: The current estimated round-trip time for the connection, in microseconds.
  info.MinRttUs: The minimum sampled round trip time, in microseconds.
  */
  if (status == 0)
    return info.RttUs;
#  endif
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__)
  struct tcp_info info;
  if (0 == xxsocket::get_optval(s, IPPROTO_TCP, TCP_INFO, info))
    return info.tcpi_rtt;
#elif defined(__APPLE__)
  struct tcp_connection_info info;
  /*
  info.tcpi_srtt: average RTT in ms
  info.tcpi_rttcur: most recent RTT in ms
  */
  if (0 == xxsocket::get_optval(s, IPPROTO_TCP, TCP_CONNECTION_INFO, info))
    return info.tcpi_srtt * std::milli::den;
#endif
  return 0;
}

void xxsocket::init_ws32_lib(void) {}

int xxsocket::get_last_errno(void)
{
#if defined(_WIN32)
  return ::WSAGetLastError();
#else
  return errno;
#endif
}
void xxsocket::set_last_errno(int error)
{
#if defined(_WIN32)
  ::WSASetLastError(error);
#else
  errno = error;
#endif
}

bool xxsocket::not_send_error(int error) { return (error == EWOULDBLOCK || error == EAGAIN || error == EINTR || error == ENOBUFS); }
bool xxsocket::not_recv_error(int error) { return (error == EWOULDBLOCK || error == EAGAIN || error == EINTR); }

const char* xxsocket::strerror(int error)
{
#if defined(_WIN32)
  static char error_msg[256];
  return xxsocket::strerror_r(error, error_msg, sizeof(error_msg));
#else
  return ::strerror(error);
#endif
}

const char* xxsocket::strerror_r(int error, char* buf, size_t buflen)
{
#if defined(_WIN32)
  ZeroMemory(buf, buflen);
  ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK /* remove line-end charactors \r\n */, NULL,
                   error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // english language
                   buf, static_cast<DWORD>(buflen), nullptr);

  return buf;
#else
  (void)::strerror_r(error, buf, buflen);
  return buf;
#endif
}

const char* xxsocket::gai_strerror(int error)
{
#if defined(_WIN32)
  return xxsocket::strerror(error);
#else
  return ::gai_strerror(error);
#endif
}
} // namespace inet
} // namespace yasio

// initialize win32 socket library
#ifdef _WIN32
namespace
{
struct ws2_32_gc {
  ws2_32_gc(void)
  {
    WSADATA dat = {0};
    (void)WSAStartup(0x0202, &dat);
  }
  ~ws2_32_gc(void) { WSACleanup(); }
};

ws2_32_gc __ws32_lib_gc;
} // namespace
#endif

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
