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
#ifndef YASIO__XXSOCKET_HPP
#define YASIO__XXSOCKET_HPP

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <chrono>
#include <functional>
#include <memory>
#include "yasio/detail/socket.hpp"
#include "yasio/detail/logging.hpp"

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{

// #define _make_dotted_decimal(b1,b2,b3,b4) ( ( ((uint32_t)(b4) << 24) & 0xff000000 ) | (
// ((uint32_t)(b3) << 16) & 0x00ff0000 ) | ( ((uint32_t)(b2) << 8) & 0x0000ff00 ) | ( (uint32_t)(b1)
// & 0x000000ff ) )
static const socket_native_type invalid_socket = (socket_native_type)-1;

YASIO__NS_INLINE
namespace ip
{
#pragma pack(push, 1)
// ip packet
struct ip_hdr_st {
  // header size; 5+
  unsigned char header_length : 4;

  // IP version: 0100/0x04(IPv4), 0110/0x05(IPv6)
  unsigned char version : 4;

  // type of service:
  union {
    unsigned char value;
    struct {
      unsigned char priority : 3;
      unsigned char D : 1;        // delay: 0(normal), 1(as little as possible)
      unsigned char T : 1;        // throughput: 0(normal), 1(as big as possible)
      unsigned char R : 1;        // reliability: 0(normal), 1(as big as possible)
      unsigned char C : 1;        // transmission cost: 0(normal), 1(as little as possible)
      unsigned char reserved : 1; // always be zero
    } detail;
  } TOS;

  // total size, header + data; MAX length is: 65535
  unsigned short total_length;

  // identifier: all split small packet set as the same value.
  unsigned short identifier;

  // flags and frag
  unsigned short flags : 3;
  unsigned short frag : 13;

  // time of living, decreased by route, if zero, this packet will by dropped
  // avoid foward looply.
  unsigned char TTL;

  // protocol
  // 1: ICMP
  // 2: IGMP
  // 6: TCP
  // 0x11/17: UDP
  // 0x58/88: IGRP
  // 0x59/89: OSPF
  unsigned char protocol; // TCP / UDP / Other

  // check header of IP-PACKET 's correctness.
  unsigned short sum;

  typedef union {
    unsigned int value;
    struct {
      unsigned int B1 : 8, B2 : 8, B3 : 8, B4 : 8;
    } detail;
  } dotted_decimal_t;

  // source ip address
  dotted_decimal_t src_ip;

  // destination ip address
  dotted_decimal_t dst_ip;
};

struct psd_hdr_st {
  unsigned long src_addr;
  unsigned long dst_addr;
  char mbz;
  char protocol;
  unsigned short tcp_length;
};

struct tcp_hdr_st {
  unsigned short src_port; // lgtm [cpp/class-many-fields]
  unsigned short dst_port;
  unsigned int seqno;
  unsigned int ackno;
  unsigned char header_length : 4;
  unsigned char reserved : 4;
  unsigned char flg_fin : 1, flg_syn : 1, flg_rst : 1, flg_psh : 1, flg_ack : 1, flg_urg : 1, flg_reserved : 2;
  unsigned short win_length;
  unsigned short sum;
  unsigned short urp;
};

struct udp_hdr_st {
  unsigned short src_port;
  unsigned short dst_port;
  unsigned short length;
  unsigned short sum;
};

struct icmp_hdr_st {
  unsigned char type;   // 8bit type
  unsigned char code;   // 8bit code
  unsigned short sum;   // 16bit check sum
  unsigned short id;    // identifier: usually use process id
  unsigned short seqno; // message sequence NO.
};

struct eth_hdr_st {
  unsigned dst_eth[6];
  unsigned src_eth[6];
  unsigned eth_type;
};

struct arp_hdr_st {
  unsigned short arp_hw;    // format of hardware address
  unsigned short arp_pro;   // format of protocol address
  unsigned char arp_hlen;   // length of hardware address
  unsigned char arp_plen;   // length of protocol address
  unsigned short arp_op;    // arp operation
  unsigned char arp_oha[6]; // sender hardware address
  unsigned long arp_opa;    // sender protocol address
  unsigned char arp_tha;    // target hardware address
  unsigned long arp_tpa;    // target protocol address;
};

struct arp_packet_st {
  eth_hdr_st ethhdr;
  arp_hdr_st arphdr;
};
#pragma pack(pop)

namespace compat
{
#if YASIO__HAS_NTOP
using ::inet_ntop;
using ::inet_pton;
#else
YASIO__DECL const char* inet_ntop(int af, const void* src, char* dst, size_t);
YASIO__DECL int inet_pton(int af, const char* src, void* dst);
#endif
} // namespace compat

inline bool is_global_in4_addr(const in_addr* addr) { return !IN4_IS_ADDR_LOOPBACK(addr) && !IN4_IS_ADDR_LINKLOCAL(addr); };
inline bool is_global_in6_addr(const in6_addr* addr) { return !!IN6_IS_ADDR_GLOBAL(addr); };

struct endpoint {
public:
  enum
  {
    fmt_no_local   = 1,
    fmt_no_port    = 1 << 1,
    fmt_no_port_0  = 1 << 2,
    fmt_no_un_path = 1 << 3,
    fmt_default    = fmt_no_port_0 | fmt_no_un_path,
  };
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
  static const size_t max_fmt_len = (std::max)(IN_MAX_ADDRSTRLEN + 2 /*[]*/ + sizeof("65535") /*:port*/, sizeof(sockaddr_un::sun_path));
#else
  static const size_t max_fmt_len = IN_MAX_ADDRSTRLEN + 2 /*[]*/ + sizeof("65535") /*:port*/;
#endif

  endpoint() { this->zeroset(); }
  endpoint(const endpoint& rhs) { this->as_is(rhs); }
  explicit endpoint(const addrinfo* info) { as_is(info); }
  explicit endpoint(const sockaddr* info) { as_is(info); }
  explicit endpoint(const char* str_ep) { as_is(str_ep); }
  endpoint(const char* addr, unsigned short port) { as_in(addr, port); }
  endpoint(uint32_t addr, unsigned short port) { as_in(addr, port); }
  endpoint(int family, const void* addr, unsigned short port) { as_in(family, addr, port); }

  explicit operator bool() const { return this->af() != AF_UNSPEC; }

  endpoint& operator=(const endpoint& rhs) { return this->as_is(rhs); }
  endpoint& as_is(const endpoint& rhs)
  {
    this->zeroset();
    memcpy(this, &rhs, sizeof(rhs));
    return *this;
  }
  endpoint& as_is(const addrinfo* info) { return this->as_is_raw(info->ai_addr, info->ai_addrlen); }
  endpoint& as_is(const sockaddr* addr)
  {
    this->zeroset();
    switch (addr->sa_family)
    {
      case AF_INET:
        ::memcpy(&in4_, addr, sizeof(sockaddr_in));
        this->len(sizeof(sockaddr_in));
        break;
      case AF_INET6:
        ::memcpy(&in6_, addr, sizeof(sockaddr_in6));
        this->len(sizeof(sockaddr_in6));
        break;
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
      case AF_UNIX:
        as_un(((sockaddr_un*)addr)->sun_path);
        break;
#endif
    }
    return *this;
  }
  /*
   * IPv4: 127.0.0.1:2022
   * IPv6: [fe80::1]:2022
   */
  endpoint& as_is(const char* str_ep)
  {
    char addr_part[IN_MAX_ADDRSTRLEN];
    if (str_ep[0] == '[')
    { // ipv6
      const char* ip = str_ep + 1;
      auto rbracket  = strchr(ip, ']');
      if (rbracket && rbracket[1] == ':')
      {
        auto zone_value = parse_in6_zone(ip, addr_part, sizeof(addr_part), rbracket);
        as_in6(ip, static_cast<u_short>(atoi(rbracket + 2)), zone_value);
      }
    }
    else
    { // ipv4
      auto colon = strchr(str_ep, ':');
      if (colon)
      {
        auto n = colon - str_ep;
        memcpy(addr_part, str_ep, n);
        addr_part[n] = '\0';
        as_in4(addr_part, static_cast<u_short>(atoi(colon + 1)));
      }
    }
    return *this;
  }
  endpoint& as_in(int family, const void* addr_in, u_short port)
  {
    this->zeroset();
    this->af(family);
    this->port(port);
    switch (family)
    {
      case AF_INET:
        ::memcpy(&in4_.sin_addr, addr_in, sizeof(in_addr));
        this->len(sizeof(sockaddr_in));
        break;
      case AF_INET6:
        ::memcpy(&in6_.sin6_addr, addr_in, sizeof(in6_addr));
        this->len(sizeof(sockaddr_in6));
        break;
    }
    return *this;
  }
  endpoint& as_in(const char* ip, unsigned short port)
  {
    this->zeroset();

    if (strchr(ip, ':'))
    { // ipv6
      char addr_part[IN_MAX_ADDRSTRLEN];
      auto zone_value = parse_in6_zone(ip, addr_part, sizeof(addr_part), nullptr);
      as_in6(ip, port, zone_value);
    }
    else
    { // ipv4
      as_in4(ip, port);
    }

    return *this;
  }
  void as_in6(const char* ip, unsigned short port, unsigned int scope_id)
  {
    if (compat::inet_pton(AF_INET6, ip, &this->in6_.sin6_addr) == 1)
    {
      this->in6_.sin6_family   = AF_INET6;
      this->in6_.sin6_port     = host_to_network(port);
      this->in6_.sin6_scope_id = scope_id;
      this->len(sizeof(sockaddr_in6));
    }
  }
  void as_in4(const char* ip, unsigned short port)
  {
    if (compat::inet_pton(AF_INET, ip, &this->in4_.sin_addr) == 1)
    {
      this->in4_.sin_family = AF_INET;
      this->in4_.sin_port   = host_to_network(port);
      this->len(sizeof(sockaddr_in));
    }
  }
  endpoint& as_in(uint32_t addr, u_short port)
  {
    this->zeroset();

    this->addr_v4(addr);
    this->port(port);
    return *this;
  }

#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
  endpoint& as_un(const char* name)
  {
    int n = snprintf(un_.sun_path, sizeof(un_.sun_path) - 1, "%s", name);
    if (n > 0)
    {
      un_.sun_family = AF_UNIX;
      this->len(offsetof(struct sockaddr_un, sun_path) + n + 1);
    }
    else
    {
      un_.sun_family = AF_UNSPEC;
      this->len(0);
    }
    return *this;
  }
#endif

  endpoint& as_is_raw(const void* ai_addr, size_t ai_addrlen)
  {
    this->zeroset();
    ::memcpy(this, ai_addr, ai_addrlen);
    this->len(ai_addrlen);
    return *this;
  }

  void zeroset() { ::memset(this, 0x0, sizeof(*this)); }

  void af(int v) { sa_.sa_family = static_cast<u_short>(v); }
  int af() const { return sa_.sa_family; }

  void ip(const char* addr)
  {
    if (strchr(addr, ':'))
    { // ipv6
      this->in6_.sin6_family = AF_INET6;
      compat::inet_pton(AF_INET6, addr, &this->in6_.sin6_addr);
      this->len(sizeof(sockaddr_in6));
    }
    else
    { // ipv4
      this->in4_.sin_family = AF_INET;
      compat::inet_pton(AF_INET, addr, &this->in4_.sin_addr);
      this->len(sizeof(sockaddr_in));
    }
  }
  std::string ip() const { return this->to_string(fmt_default | fmt_no_port); }
  std::string to_string(int flags = fmt_default) const
  {
    std::string ret;
    this->format_to(ret, flags);
    return ret;
  }

  unsigned short port() const { return network_to_host(in4_.sin_port); }
  void port(unsigned short value) { in4_.sin_port = host_to_network(value); }

  // for ipv6 only
  unsigned int scope_id() const { return af() == AF_INET6 ? static_cast<unsigned int>(this->in6_.sin6_scope_id) : 0u; }
  void scope_id(unsigned int v)
  {
    if (af() == AF_INET6)
      this->in6_.sin6_scope_id = v;
  }

  void addr_v4(uint32_t addr)
  {
    this->af(AF_INET);
    in4_.sin_addr.s_addr = host_to_network(addr);
    this->len(sizeof(sockaddr_in));
  }
  uint32_t addr_v4() const { return af() == AF_INET ? network_to_host(in4_.sin_addr.s_addr) : 0u; }

  // check does endpoint is global address, not linklocal or loopback
  bool is_global() const
  {
    switch (af())
    {
      case AF_INET:
        return is_global_in4_addr(&in4_.sin_addr);
      case AF_INET6:
        return is_global_in6_addr(&in6_.sin6_addr);
    }
    return false;
  }

  void len(size_t n)
  {
#if !YASIO__HAS_SA_LEN
    len_ = static_cast<uint8_t>(n);
#else
    sa_.sa_len = static_cast<uint8_t>(n);
#endif
  }
  socklen_t len() const
  {
#if !YASIO__HAS_SA_LEN
    return len_;
#else
    return sa_.sa_len;
#endif
  }

  // format to buffer
  size_t format_to(std::string& buf, int flags = 0) const
  {
    char str[endpoint::max_fmt_len];
    size_t n = this->format_to(str, endpoint::max_fmt_len, flags);
    if (n > 0)
    {
      buf.append(str, n);
      return n;
    }
    return 0;
  }

  /* format to
   *
   * @params:
   *    buf: the buffer to output
   *    buf_len: the buffer len, must be at least endpoint::max_fmt_len
   * @returns:
   *    the number of character written to the buf without null-termianted charactor
   *
   * @remark:
   *   The buffer result should be
   *    ipv4: xxx.xxx.xxx.xxx<:port> 127.0.0.1:2021
   *    ipv6: [xxx....xxx]<:port> [fe80::1]:53
   */
  size_t format_to(char* buf, size_t buf_len, int flags) const
  {
    if (!(fmt_no_local & flags) || is_global())
    {
      size_t n = 0;
      switch (af())
      {
        case AF_INET:
          n = strlen(compat::inet_ntop(AF_INET, &in4_.sin_addr, buf, buf_len));
          break;
        case AF_INET6:
          buf[n++] = '[';
          n += strlen(compat::inet_ntop(AF_INET6, &in6_.sin6_addr, buf + n, buf_len - n));
          buf[n++] = ']';
          break;
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
        case AF_UNIX:
          if (!(flags & fmt_no_un_path))
          {
            n = (std::min)(static_cast<size_t>(this->len()), buf_len);
            memcpy(buf, un_.sun_path, n);
          }
          return n;
#endif
      }
      if (n > 0)
      {
        if (!(flags & fmt_no_port))
        {
          u_short p = this->port();
          if (!(flags & fmt_no_port_0) || p != 0)
            n += sprintf(buf + n, ":%u", (unsigned int)p);
        }
      }
      return n;
    }
    return 0;
  }

  /* format ipv4 only
   %N: s_net   127
   %H: s_host  0
   %L: s_lh    0
   %M: s_impno 1
   %l: low byte of port
   %h: high byte of port
  */
  std::string format_v4(const char* foramt)
  {
    static const char* const _SIN_FORMATS[] = {"%N", "%H", "%L", "%M", "%l", "%h"};

    unsigned char addr_bytes[sizeof(in4_.sin_addr.s_addr) + sizeof(u_short)];
    memcpy(addr_bytes, &in4_.sin_addr.s_addr, sizeof(in4_.sin_addr.s_addr));
    memcpy(addr_bytes + sizeof(in4_.sin_addr.s_addr), &in4_.sin_port, sizeof(in4_.sin_port));

    char snum[sizeof("255")] = {0};
    const size_t _N0         = sizeof("%N") - 1;
    std::string s            = foramt;
    for (size_t idx = 0; idx < YASIO_ARRAYSIZE(_SIN_FORMATS); ++idx)
    {
      auto fmt   = _SIN_FORMATS[idx];
      auto offst = s.find(fmt);
      if (offst != std::string::npos)
      {
        sprintf(snum, "%u", addr_bytes[idx]);
        s.replace(offst, _N0, snum);
      }
    }

    return s;
  }

  sockaddr* operator&() { return &sa_; }
  const sockaddr* operator&() const { return &sa_; }

  union {
    sockaddr sa_;
    sockaddr_in in4_;
    sockaddr_in6 in6_;
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
    sockaddr_un un_;
#endif
  };
#if !YASIO__HAS_SA_LEN
  uint8_t len_;
#endif

private:
  unsigned int parse_in6_zone(const char*& ip, char* addr_part, size_t addr_part_maxn, const char* endptr)
  {
    unsigned int zone_value = 0;
    auto zoneptr            = strchr(ip, '%');
    if (zoneptr)
    {

#if defined(_WIN32)
      zone_value = atoi(zoneptr + 1);
#else
      zone_value = ::if_nametoindex(zoneptr + 1);
#endif
      endptr = zoneptr;
    }

    if (endptr)
    {
      size_t n = (std::min)(static_cast<size_t>(endptr - ip), addr_part_maxn - 1);
      memcpy(addr_part, ip, n);
      addr_part[n] = '\0';
      ip           = addr_part;
    }
    return zone_value;
  }
};

// supported internet protocol flags
enum : u_short
{
  ipsv_unavailable = 0,
  ipsv_ipv4        = 1,
  ipsv_ipv6        = 2,
  ipsv_dual_stack  = ipsv_ipv4 | ipsv_ipv6,
};
} // namespace ip

#if !YASIO__HAS_CXX11
using namespace yasio::inet::ip;
#endif

/*
** CLASS xxsocket: a posix socket wrapper
*/
class YASIO_API xxsocket {
public: /// portable connect APIs
  // easy to connect a server ipv4 or ipv6 with local ip protocol version detect
  // for support ipv6 ONLY network.
  YASIO__DECL int xpconnect(const char* hostname, u_short port, u_short local_port = 0);
  YASIO__DECL int xpconnect_n(const char* hostname, u_short port, const std::chrono::microseconds& wtimeout, u_short local_port = 0);

  // easy to connect a server ipv4 or ipv6.
  YASIO__DECL int pconnect(const char* hostname, u_short port, u_short local_port = 0);
  YASIO__DECL int pconnect_n(const char* hostname, u_short port, const std::chrono::microseconds& wtimeout, u_short local_port = 0);
  YASIO__DECL int pconnect_n(const char* hostname, u_short port, u_short local_port = 0);

  // easy to connect a server ipv4 or ipv6.
  YASIO__DECL int pconnect(const endpoint& ep, u_short local_port = 0);
  YASIO__DECL int pconnect_n(const endpoint& ep, const std::chrono::microseconds& wtimeout, u_short local_port = 0);
  YASIO__DECL int pconnect_n(const endpoint& ep, u_short local_port = 0);

  // easy to create a tcp ipv4 or ipv6 server socket.
  YASIO__DECL int pserve(const char* addr, u_short port);
  YASIO__DECL int pserve(const endpoint& ep);

public:
  // Construct a empty socket object
  YASIO__DECL xxsocket();

  // Construct with a exist socket handle
  YASIO__DECL xxsocket(socket_native_type handle);
  // Disable copy constructor
  YASIO__DECL xxsocket(const xxsocket&) = delete;
  // Construct with a exist socket, it will replace the source
  YASIO__DECL xxsocket(xxsocket&&) YASIO__NOEXCEPT;

  YASIO__DECL xxsocket& operator=(socket_native_type handle) YASIO__NOEXCEPT;
  // Disable copy assign operator
  YASIO__DECL xxsocket& operator=(const xxsocket&) = delete;
  // Construct with a exist socket, it will replace the source
  YASIO__DECL xxsocket& operator=(xxsocket&&) YASIO__NOEXCEPT;

  // See also as function: open
  YASIO__DECL xxsocket(int af, int type, int protocol);

  YASIO__DECL ~xxsocket();

  // swap with other when this fd is closed.
  YASIO__DECL xxsocket& swap(xxsocket& who);

  /* @brief: Open new socket
  ** @params:
  **        af      : Usually is [AF_INET]
  **        type    : [SOCK_STREAM-->TCP] and [SOCK_DGRAM-->UDP]
  **        protocol: Usually is [0]
  ** @returns: false: check reason by errno
  */
  YASIO__DECL bool open(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
  YASIO__DECL bool reopen(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
#ifdef _WIN32
  YASIO__DECL bool open_ex(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);

  YASIO__DECL static bool accept_ex(SOCKET sockfd_listened, SOCKET sockfd_prepared, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength,
                                    DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped);

  YASIO__DECL static bool connect_ex(SOCKET s, const struct sockaddr* name, int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength, LPDWORD lpdwBytesSent,
                                     LPOVERLAPPED lpOverlapped);

  YASIO__DECL static void translate_sockaddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
                                              sockaddr** LocalSockaddr, LPINT LocalSockaddrLength, sockaddr** RemoteSockaddr, LPINT RemoteSockaddrLength);
#endif

  /** Is this socket opened **/
  YASIO__DECL bool is_open() const;

  /** Gets the socket fd value **/
  YASIO__DECL socket_native_type native_handle() const;

  /** Release ownership of the underlying native socket handle **/
  YASIO__DECL socket_native_type release_handle();

  /* @brief: Set this socket io mode to nonblocking
  ** @params:
  **
  ** @returns: [0] succeed, otherwise, a value of SOCKET_ERROR is returned.
  */
  YASIO__DECL int set_nonblocking(bool nonblocking) const;
  YASIO__DECL static int set_nonblocking(socket_native_type s, bool nonblocking);

  /* @brief: Test whether the socket has nonblocking flag
  ** @params:
  **
  ** @returns: [1] yes. [0] no
  ** @pitfall: for wsock2, will return [-1] when it's a unconnected SOCK_STREAM
  */
  YASIO__DECL int test_nonblocking() const;
  YASIO__DECL static int test_nonblocking(socket_native_type s);

  /* @brief: Associates a local address with this socket
  ** @params:
  **        addr: four point address, if set "0.0.0.0" ipv4, "::" ipv6, the socket will listen at
  *any.
  **        port: @$$#s
  ** @returns:
  **         If no error occurs, bind returns [0]. Otherwise, it returns SOCKET_ERROR
  */
  YASIO__DECL int bind(const char* addr, unsigned short port) const;
  YASIO__DECL int bind(const endpoint&) const;
  YASIO__DECL int bind_any(bool ipv6 = false) const;

  /* @brief: Places this socket in a state in which it is listening for an incoming connection
  ** @params:
  **        Ommit
  **
  ** @returns:
  **         If no error occurs, bind returns [0]. Otherwise, it returns SOCKET_ERROR
  */
  YASIO__DECL int listen(int backlog = SOMAXCONN) const;

  /* @brief: Permits an incoming connection attempt on this socket
  ** @returns:
  **        If no error occurs, accept returns a new socket on which
  **        the actual connection is made.
  **        Otherwise, a value of [invalid_socket] is returned
  */
  YASIO__DECL xxsocket accept() const;

  /* @brief: Permits an incoming connection attempt on this socket
  ** @params:
  ** @returns:
  **        If no error occurs, return 0, and the new_sock will be the actual connection is made.
  **        Otherwise, a EWOULDBLOCK,EAGAIN or other value is returned
  */
  YASIO__DECL int accept_n(socket_native_type& new_sock) const;

  /* @brief: Establishes a connection to a specified this socket
  ** @params:
  **        addr: Usually is a IPV4 address
  **        port: Server Listenning Port
  **
  ** @returns:
  **         If no error occurs, returns [0].
  **         Otherwise, it returns SOCKET_ERROR
  */
  YASIO__DECL int connect(const char* addr, u_short port);
  YASIO__DECL int connect(const endpoint& ep);
  YASIO__DECL static int connect(socket_native_type s, const char* addr, u_short port);
  YASIO__DECL static int connect(socket_native_type s, const endpoint& ep);

  /* @brief: Establishes a connection to a specified this socket with nonblocking
  ** @params:
  **        timeout: connection timeout, millseconds
  **
  ** @returns: [0].succeed, [-1].failed
  ** @remark: Because on win32, there is no way to test whether the socket is non-blocking,
  ** so, after this function called, the socket will be always set to blocking mode.
  */
  YASIO__DECL int connect_n(const char* addr, u_short port, const std::chrono::microseconds& wtimeout);
  YASIO__DECL int connect_n(const endpoint& ep, const std::chrono::microseconds& wtimeout);
  YASIO__DECL static int connect_n(socket_native_type s, const endpoint& ep, const std::chrono::microseconds& wtimeout);

  /* @brief: Establishes a connection to a specified this socket with nonblocking
  ** @params:
  **
  ** @returns: [0].succeed, [-1].failed
  ** @remark: this function will return immediately, for tcp, you should detect whether the
  ** handshake complete by handle_write_ready.
  */
  YASIO__DECL int connect_n(const endpoint& ep);
  YASIO__DECL static int connect_n(socket_native_type s, const endpoint& ep);

  /* @brief: Disconnect a connectionless socket (such as SOCK_DGRAM)
  **
  */
  YASIO__DECL int disconnect() const;
  YASIO__DECL static int disconnect(socket_native_type s);

  /* @brief: nonblock send
   ** @params: omit
   **
   ** @returns:
   **         If no error occurs, retval == len,
   **         Oterwise, If retval < len && not_recv_error(get_last_errno()), should close socket.
   */
  YASIO__DECL int send_n(const void* buf, int len, const std::chrono::microseconds& wtimeout, int flags = 0);
  YASIO__DECL static int send_n(socket_native_type s, const void* buf, int len, std::chrono::microseconds wtimeout, int flags = 0);

  /* @brief: nonblock recv
  ** @params:
  **       The timeout is in microseconds
  ** @returns:
  **         If no error occurs, retval == len,
  **         Oterwise, If retval < len && not_recv_error(get_last_errno()), should close socket.
  */
  YASIO__DECL int recv_n(void* buf, int len, const std::chrono::microseconds& wtimeout, int flags = 0) const;
  YASIO__DECL static int recv_n(socket_native_type s, void* buf, int len, std::chrono::microseconds wtimeout, int flags = 0);

  /* @brief: Sends data on this connected socket
  ** @params: omit
  **
  ** @returns:
  **         If no error occurs, send returns the total number of bytes sent,
  **         which can be less than the number requested to be sent in the len parameter.
  **         Otherwise, a value of SOCKET_ERROR is returned.
  */
  YASIO__DECL int send(const void* buf, int len, int flags = 0) const;
  YASIO__DECL static int send(socket_native_type fd, const void* buf, int len, int flags = 0);

  /* @brief: Receives data from this connected socket or a bound connectionless socket.
  ** @params: omit
  **
  ** @returns:
  **         If no error occurs, recv returns the number of bytes received and
  **         the buffer pointed to by the buf parameter will contain this data received.
  **         If the connection has been gracefully closed, the return value is [0].
  */
  YASIO__DECL int recv(void* buf, int len, int flags = 0) const;
  YASIO__DECL static int recv(socket_native_type s, void* buf, int len, int flags);

  /* @brief: Sends data on this connected socket
  ** @params: omit
  **
  ** @returns:
  **         If no error occurs, send returns the total number of bytes sent,
  **         which can be less than the number requested to be sent in the len parameter.
  **         Otherwise, a value of SOCKET_ERROR is returned.
  */
  YASIO__DECL int sendto(const void* buf, int len, const endpoint& to, int flags = 0) const;

  /* @brief: Receives a datagram and stores the source address
  ** @params: omit
  **
  ** @returns:
  **         If no error occurs, recv returns the number of bytes received and
  **         the buffer pointed to by the buf parameter will contain this data received.
  **         If the connection has been gracefully closed, the return value is [0].
  */
  YASIO__DECL int recvfrom(void* buf, int len, endpoint& peer, int flags = 0) const;

  YASIO__DECL int handle_write_ready(const std::chrono::microseconds& wtimeout) const;
  YASIO__DECL static int handle_write_ready(socket_native_type s, const std::chrono::microseconds& wtimeout);

  YASIO__DECL int handle_read_ready(const std::chrono::microseconds& wtimeout) const;
  YASIO__DECL static int handle_read_ready(socket_native_type s, const std::chrono::microseconds& wtimeout);

  /* @brief: Get local address info
  ** @params : None
  **
  ** @returns:
  */
  YASIO__DECL endpoint local_endpoint() const;
  YASIO__DECL static endpoint local_endpoint(socket_native_type);

  /* @brief: Get peer address info
  ** @params : None
  **
  ** @returns:
  *  @remark: if this a listening socket fd, will return "0.0.0.0:0"
  */
  YASIO__DECL endpoint peer_endpoint() const;
  YASIO__DECL static endpoint peer_endpoint(socket_native_type);

  /* @brief: Configure TCP keepalive
  ** @params : flag:     1.on, 0.off
  **           idle:     time(secs) to send keepalive when no data interaction
  **           interval: keepalive send interval(secs)
  **           probes:   count to try when no response
  **
  ** @returns: [0].successfully
  **          [<0].one or more errors occured
  */
  YASIO__DECL int set_keepalive(int flag = 1, int idle = 7200, int interval = 75, int probes = 10);
  YASIO__DECL static int set_keepalive(socket_native_type s, int flag, int idle, int interval, int probes);

  YASIO__DECL void reuse_address(bool reuse);

  YASIO__DECL void exclusive_address(bool exclusive);

  /* @brief: Sets the socket option
  ** @params :
  **        level: The level at which the option is defined (for example, SOL_SOCKET).
  **      optname: The socket option for which the value is to be set (for example, SO_BROADCAST).
  **               The optname parameter must be a socket option defined within the specified level,
  **               or behavior is undefined.
  **       optval: The option value.
  ** @examples:
  **       set_optval(SOL_SOCKET, SO_SNDBUF, 4096);
  **       set_optval(SOL_SOCKET, SO_RCVBUF, 4096);
  **
  ** @remark: for more detail, please see:
  **       windows: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
  **       linux: https://linux.die.net/man/3/setsockopt
  **
  ** @returns: If no error occurs, set_optval returns zero. Otherwise, a value of SOCKET_ERROR is
  **       returned
  */
  template <typename _Ty>
  inline int set_optval(int level, int optname, const _Ty& optval)
  {
    return set_optval(this->fd, level, optname, optval);
  }
  template <typename _Ty>
  inline static int set_optval(socket_native_type sockfd, int level, int optname, const _Ty& optval)
  {
    return set_optval(sockfd, level, optname, &optval, static_cast<socklen_t>(sizeof(_Ty)));
  }

  int set_optval(int level, int optname, const void* optval, socklen_t optlen) { return set_optval(this->fd, level, optname, optval, optlen); }
  static int set_optval(socket_native_type sockfd, int level, int optname, const void* optval, socklen_t optlen)
  {
    return ::setsockopt(sockfd, level, optname, static_cast<const char*>(optval), optlen);
  }

  /* @brief: Retrieves a socket option
  ** @params :
  **     level: The level at which the option is defined. Example: SOL_SOCKET.
  **   optname: The socket option for which the value is to be retrieved.
  **            Example: SO_ACCEPTCONN. The optname value must be a socket option defined within the
  **            specified level, or behavior is undefined.
  **    optval: A variable to the buffer in which the value for the requested option is to be
  **            returned.
  **
  ** @returns: If no error occurs, get_optval returns zero. Otherwise, a value of SOCKET_ERROR is
  *returned
  */
  template <typename _Ty>
  inline _Ty get_optval(int level, int optname) const
  {
    _Ty optval = {};
    get_optval(this->fd, level, optname, optval);
    return optval;
  }
  template <typename _Ty>
  inline int get_optval(int level, int optname, _Ty& optval) const
  {
    return get_optval(this->fd, level, optname, optval);
  }
  template <typename _Ty>
  inline static int get_optval(socket_native_type sockfd, int level, int optname, _Ty& optval)
  {
    socklen_t optlen = static_cast<socklen_t>(sizeof(_Ty));
    return get_optval(sockfd, level, optname, &optval, &optlen);
  }
  static int get_optval(socket_native_type sockfd, int level, int optname, void* optval, socklen_t* optlen)
  {
    return ::getsockopt(sockfd, level, optname, static_cast<char*>(optval), optlen);
  }

  /* @brief: control the socket
  ** @params :
  **          see MSDN or man page
  ** @returns: If no error occurs, ioctl returns zero. Otherwise, a value of SOCKET_ERROR is
  **           returned
  **
  **
  **
  */
  template <typename _Ty>
  inline int ioctl(long cmd, const _Ty& value) const
  {
    return xxsocket::ioctl(this->fd, cmd, value);
  }
  template <typename _Ty>
  inline static int ioctl(socket_native_type s, long cmd, const _Ty& value)
  {
    u_long argp = static_cast<u_long>(value);
    return ::ioctlsocket(s, cmd, &argp);
  }

  /* @brief: wrapper system select, hide signal EINTR
  ** @params:
  **          s: the socket fd, it's different with system
  **          see MSDN or man page
  ** @returns: If no error occurs, returns >= 0. Otherwise, a value of -1 is
  **          returned
  */
  int select(fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const std::chrono::microseconds& wtimeout) const
  {
    return xxsocket::select(this->fd, readfds, writefds, exceptfds, wtimeout);
  }
  YASIO__DECL static int select(socket_native_type s, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, std::chrono::microseconds wtimeout);

  /* @brief: Disables sends or receives on this socket
  ** @params:
  **        how: [SD_SEND] or [SD_RECEIVE] or [SD_BOTH]
  **
  ** @returns: [0] succeed, otherwise, a value of SOCKET_ERROR is returned.
  */
  YASIO__DECL int shutdown(int how = SD_BOTH) const;

  /* @brief: close sends
   ** @params:
   **        shut_how: [SD_SEND] or [SD_RECEIVE] or [SD_BOTH] or [SD_NONE]
   **
   ** @returns: [0] succeed, otherwise, a value of SOCKET_ERROR is returned.
   */
  YASIO__DECL void close(int shut_how = SD_BOTH);

  /* @brief: Retrive tcp socket rtt in microseconds
   ** @params:
   **        non
   **
   ** @returns: > 0 succeed, 0 failed.
   */
  YASIO__DECL unsigned int tcp_rtt() const;
  YASIO__DECL static unsigned int tcp_rtt(socket_native_type s);

  YASIO__DECL operator socket_native_type() const;

  /// <summary>
  /// this function just for windows platform
  /// </summary>
  YASIO__DECL static void init_ws32_lib();

  YASIO__DECL static int get_last_errno();
  YASIO__DECL static void set_last_errno(int error);

  YASIO__DECL static bool not_send_error(int error);
  YASIO__DECL static bool not_recv_error(int error);

  YASIO__DECL static const char* strerror(int error);
  YASIO__DECL static const char* strerror_r(int error, char* buf, size_t buflen);
  YASIO__DECL static const char* gai_strerror(int error);

  /// <summary>
  /// Resolve all as ipv4 or ipv6 endpoints
  /// </summary>
  YASIO__DECL static int resolve(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port = 0, int socktype = SOCK_STREAM);

  /// <summary>
  /// Resolve as ipv4 address only.
  /// </summary>
  YASIO__DECL static int resolve_v4(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port = 0, int socktype = SOCK_STREAM);

  /// <summary>
  /// Resolve as ipv6 address only.
  /// </summary>
  YASIO__DECL static int resolve_v6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port = 0, int socktype = SOCK_STREAM);

  /// <summary>
  /// Resolve as ipv4 address only and convert to V4MAPPED format.
  /// </summary>
  YASIO__DECL static int resolve_v4to6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port = 0, int socktype = SOCK_STREAM);

  /// <summary>
  /// Force resolve all addres to ipv6 endpoints, IP4 with AI_V4MAPPED
  /// </summary>
  YASIO__DECL static int resolve_tov6(std::vector<endpoint>& endpoints, const char* hostname, unsigned short port = 0, int socktype = SOCK_STREAM);

  /// <summary>
  /// Resolve as ipv4 or ipv6 endpoints with callback
  /// </summary>
  template <typename _Fty>
  inline static int resolve_i(const _Fty& callback, const char* hostname, unsigned short port = 0, int af = 0, int flags = 0, int socktype = SOCK_STREAM)
  {
    addrinfo hint;
    memset(&hint, 0x0, sizeof(hint));
    hint.ai_flags    = flags;
    hint.ai_family   = af;
    hint.ai_socktype = socktype;

    addrinfo* answerlist        = nullptr;
    char buffer[sizeof "65535"] = {'\0'};
    const char* service         = nullptr;
    if (port > 0)
    {
      sprintf(buffer, "%u", port); // It's enough for unsigned short, so use sprintf ok.
      service = buffer;
    }
    int error = getaddrinfo(hostname, service, &hint, &answerlist);
    if (nullptr == answerlist)
      return error;

    for (auto ai = answerlist; ai != nullptr; ai = ai->ai_next)
    {
      if (ai->ai_family == AF_INET6 || ai->ai_family == AF_INET)
      {
        if (callback(endpoint(ai)))
          break;
      }
    }

    freeaddrinfo(answerlist);

    return error;
  }

  /*
  ** @brief:: Gets supported internet protocol versions of localhost
  ** @returns:
  **  ipsv_unavailable: The network unavailable.
  **  ipsv_ipv4: Support ipv4 only.
  **  ipsv_ipv6: Support ipv6 only.
  **  ipsv_dual_stack:
  **    Support ipv4 or ipv6, but for multi network adapters device, you should always
  **    use ipv4 preferred, such as smart phone with wifi & cellular network. The smart phone's os
  **    will choose wifi when it is available to avoid consume user's cash, when the cellular
  *support
  **    ipv6/ipv4 but the wifi only support ipv4, then use ipv6 will cause network issue.
  **    For more detail, see: https://github.com/halx99/yasio/issues/130
  */
  YASIO__DECL static int getipsv();

  /*
  ** @brief: Traverse local device network adapter address with valid ip
  ** @params:
  **  handler: prototype is [](const ip::endpoint& ep)->bool
  */
  YASIO__DECL static void traverse_local_address(std::function<bool(const ip::endpoint&)> handler);

protected:
  YASIO__DECL static void reregister_descriptor(socket_native_type s, fd_set* fds);

private:
  socket_native_type fd;
}; // namespace inet

} // namespace inet
#if !YASIO__HAS_CXX11
using namespace yasio::inet;
#endif
} // namespace yasio

namespace std
{ // VS2013 the operator must be at namespace std
inline bool operator<(const yasio::inet::ip::endpoint& lhs, const yasio::inet::ip::endpoint& rhs)
{ // apply operator < to operands
  if (lhs.af() == AF_INET)
    return (static_cast<uint64_t>(lhs.in4_.sin_addr.s_addr) + lhs.in4_.sin_port) < (static_cast<uint64_t>(rhs.in4_.sin_addr.s_addr) + rhs.in4_.sin_port);
  return ::memcmp(&lhs, &rhs, sizeof(rhs)) < 0;
}
inline bool operator==(const yasio::inet::ip::endpoint& lhs, const yasio::inet::ip::endpoint& rhs)
{ // apply operator == to operands
  return !(lhs < rhs) && !(rhs < lhs);
}
} // namespace std

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#if defined(YASIO_HEADER_ONLY)
#  include "yasio/xxsocket.cpp" // lgtm [cpp/include-non-header]
#endif

#endif
