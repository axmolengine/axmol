//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

HAL: Hardware Abstraction Layer
X99: Intel X99 Mainboard Platform

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
#ifndef YASIO__CORE_CPP
#define YASIO__CORE_CPP
#if !defined(YASIO_HEADER_ONLY)
#  include "yasio/yasio.hpp"
#endif
#include <limits>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "yasio/detail/thread_name.hpp"

#if defined(YASIO_SSL_BACKEND)
#  include "yasio/detail/ssl.hpp"
#endif

#if defined(YASIO_HAVE_KCP)
#  include "kcp/ikcp.h"
#endif

#if defined(YASIO_HAVE_CARES)
#  include "yasio/detail/ares.hpp"
#endif

// clang-format off
#define YASIO_KLOG_CP(level, format, ...)                                                                                      \
  do                                                                                                                           \
  {                                                                                                                            \
    auto& __cprint = __get_cprint();                                                                                           \
    auto __msg           = ::yasio::strfmt(127, "[yasio][%lld]" format "\n", ::yasio::clock<system_clock_t>(), ##__VA_ARGS__); \
    if (__cprint)                                                                                                              \
      __cprint(level, __msg.c_str());                                                                                          \
    else                                                                                                                       \
      YASIO_LOG_TAG("", "%s", __msg.c_str());                                                                                  \
  } while (false)
// clang-format on

#define YASIO_KLOGD(format, ...) YASIO_KLOG_CP(YLOG_D, format, ##__VA_ARGS__)
#define YASIO_KLOGI(format, ...) YASIO_KLOG_CP(YLOG_I, format, ##__VA_ARGS__)
#define YASIO_KLOGE(format, ...) YASIO_KLOG_CP(YLOG_E, format, ##__VA_ARGS__)

#if !defined(YASIO_VERBOSE_LOG)
#  define YASIO_KLOGV(fmt, ...) (void)0
#else
#  define YASIO_KLOGV(format, ...) YASIO_KLOG_CP(YLOG_V, format, ##__VA_ARGS__)
#endif

#define yasio__setbits(x, m) ((x) |= (m))
#define yasio__clearbits(x, m) ((x) &= ~(m))
#define yasio__testbits(x, m) ((x) & (m))

#define yasio__testlobyte(x, v) (((x) & (uint16_t)0x00ff) == (v))
#define yasio__setlobyte(x, v) ((x) = ((x) & ~((decltype(x))0xff)) | (v))
#define yasio__lobyte(x) ((x) & (uint16_t)0x00ff)

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 6320 6322 4996)
#endif

namespace yasio
{
namespace errc
{
enum
{
  no_error              = 0,   // No error.
  invalid_packet        = -27, // Invalid packet.
  resolve_host_failed   = -26, // Resolve host failed.
  no_available_address  = -25, // No available address to connect.
  shutdown_by_localhost = -24, // Local shutdown the connection.
  ssl_handshake_failed  = -23, // SSL handshake failed.
  ssl_write_failed      = -22, // SSL write failed.
  ssl_read_failed       = -21, // SSL read failed.
  eof                   = -20, // end of file.
};
}
namespace inet
{
namespace
{
enum
{ // event mask
  YEM_POLLIN  = 1,
  YEM_POLLOUT = 2,
  YEM_POLLERR = 4,
};

enum
{ // op mask
  YOPM_OPEN  = 1,
  YOPM_CLOSE = 1 << 1,
};

enum : u_short
{ // dns queries state
  YDQS_READY = 1,
  YDQS_DIRTY,
  YDQS_INPROGRESS,
  YDQS_FAILED,
};

enum
{
  /* whether udp server enable multicast service */
  YCPF_MCAST = 1 << 17,

  /* whether multicast loopback, if 1, local machine can recv self multicast packet */
  YCPF_MCAST_LOOPBACK = 1 << 18,

  /* whether host modified */
  YCPF_HOST_MOD = 1 << 19,

  /* whether port modified */
  YCPF_PORT_MOD = 1 << 20,

  /* whether need dns queries */
  YCPF_NEEDS_QUERIES = 1 << 21,

  /// below is byte2 of private flags (25~32)
  /* whether ssl client in handshaking */
  YCPF_SSL_HANDSHAKING = 1 << 25,
};

namespace
{
// the minimal wait duration for select
static highp_time_t yasio__min_wait_duration = 0LL;
// the max transport alloc size
static const size_t yasio__max_tsize = (std::max)({sizeof(io_transport_tcp), sizeof(io_transport_udp), sizeof(io_transport_ssl), sizeof(io_transport_kcp)});
} // namespace
struct yasio__global_state {
  enum
  {
    INITF_SSL   = 1,
    INITF_CARES = 2,
  };
  yasio__global_state(const print_fn2_t& custom_print)
  {
    auto __get_cprint = [&]() -> const print_fn2_t& { return custom_print; };
    // for single core CPU, we set minimal wait duration to 10us by default
    yasio__min_wait_duration = std::thread::hardware_concurrency() > 1 ? 0LL : YASIO_MIN_WAIT_DURATION;
#if defined(YASIO_SSL_BACKEND) && YASIO_SSL_BACKEND == 1
    if (OPENSSL_init_ssl(0, NULL) == 1)
      yasio__setbits(this->init_flags_, INITF_SSL);
#endif
#if defined(YASIO_HAVE_CARES)
    int ares_status = ::ares_library_init(ARES_LIB_INIT_ALL);
    if (ares_status == 0)
      yasio__setbits(init_flags_, INITF_CARES);
    else
      YASIO_KLOGI("[global] c-ares library init failed, status=%d, detail:%s", ares_status, ::ares_strerror(ares_status));
#  if defined(__ANDROID__)
    ares_status = ::yasio__ares_init_android();
    if (ares_status != 0)
      YASIO_KLOGI("[global] c-ares library init android failed, status=%d, detail:%s", ares_status, ::ares_strerror(ares_status));
#  endif
#endif
    // print version & transport alloc size
    YASIO_KLOGI("[global] the yasio-%x.%x.%x is initialized, the size of per transport is %d when object_pool enabled.", (YASIO_VERSION_NUM >> 16) & 0xff,
                (YASIO_VERSION_NUM >> 8) & 0xff, YASIO_VERSION_NUM & 0xff, yasio__max_tsize);
    YASIO_KLOGI("[global] sizeof(io_event)=%d", sizeof(io_event));
  }
  ~yasio__global_state()
  {
#if defined(YASIO_HAVE_CARES)
    if (yasio__testbits(this->init_flags_, INITF_CARES))
      ::ares_library_cleanup();
#endif
  }

  int init_flags_ = 0;
  print_fn2_t cprint_;
};
static yasio__global_state& yasio__shared_globals(const print_fn2_t& prt = nullptr)
{
  static yasio__global_state __global_state(prt);
  return __global_state;
}
} // namespace

/// highp_timer
void highp_timer::async_wait(io_service& service, timer_cb_t cb) { service.schedule_timer(this, std::move(cb)); }
void highp_timer::cancel(io_service& service)
{
  if (!expired())
    service.remove_timer(this);
}

/// io_send_op
int io_send_op::perform(io_transport* transport, const void* buf, int n) { return transport->write_cb_(buf, n, nullptr); }

/// io_sendto_op
int io_sendto_op::perform(io_transport* transport, const void* buf, int n) { return transport->write_cb_(buf, n, &destination_); }

#if defined(YASIO_SSL_BACKEND)
void ssl_auto_handle::destroy()
{
  if (ssl_)
  {
#  if YASIO_SSL_BACKEND == 1
    ::SSL_shutdown(ssl_);
    ::SSL_free(ssl_);
#  elif YASIO_SSL_BACKEND == 2
    ::mbedtls_ssl_free(ssl_);
    delete ssl_;
#  endif
    ssl_ = nullptr;
  }
}
#endif

/// io_channel
io_channel::io_channel(io_service& service, int index) : io_base(), service_(service)
{
  socket_            = std::make_shared<xxsocket>();
  state_             = io_base::state::CLOSED;
  dns_queries_state_ = YDQS_FAILED;
  index_             = index;
  decode_len_        = [=](void* ptr, int len) { return this->__builtin_decode_len(ptr, len); };
}
std::string io_channel::format_destination() const
{
  if (yasio__testbits(properties_, YCPF_NEEDS_QUERIES))
    return yasio::strfmt(127, "%s(%s):%u", remote_host_.c_str(), !remote_eps_.empty() ? remote_eps_[0].ip().c_str() : "undefined", remote_port_);

  return yasio::strfmt(127, "%s:%u", remote_host_.c_str(), remote_port_);
}
void io_channel::enable_multicast_group(const ip::endpoint& ep, int loopback)
{
  yasio__setbits(properties_, YCPF_MCAST);
  if (loopback)
    yasio__setbits(properties_, YCPF_MCAST_LOOPBACK);

  multiaddr_ = ep;
}
int io_channel::join_multicast_group()
{
  if (socket_->is_open())
  {
    int loopback = yasio__testbits(properties_, YCPF_MCAST_LOOPBACK) ? 1 : 0;
    socket_->set_optval(multiaddr_.af() == AF_INET ? IPPROTO_IP : IPPROTO_IPV6, multiaddr_.af() == AF_INET ? IP_MULTICAST_LOOP : IPV6_MULTICAST_LOOP, loopback);
    // ttl
    socket_->set_optval(multiaddr_.af() == AF_INET ? IPPROTO_IP : IPPROTO_IPV6, multiaddr_.af() == AF_INET ? IP_MULTICAST_TTL : IPV6_MULTICAST_HOPS,
                        YASIO_DEFAULT_MULTICAST_TTL);

    return configure_multicast_group(true);
  }
  return -1;
}
void io_channel::disable_multicast_group()
{
  yasio__clearbits(properties_, YCPF_MCAST);
  yasio__clearbits(properties_, YCPF_MCAST_LOOPBACK);

  if (socket_->is_open())
    configure_multicast_group(false);
}
int io_channel::configure_multicast_group(bool onoff)
{
  if (multiaddr_.af() == AF_INET)
  { // ipv4
    struct ip_mreq mreq;
    mreq.imr_interface.s_addr = 0;
    mreq.imr_multiaddr        = multiaddr_.in4_.sin_addr;
    return socket_->set_optval(IPPROTO_IP, onoff ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &mreq, (int)sizeof(mreq));
  }
  else
  { // ipv6
    struct ipv6_mreq mreq_v6;
    mreq_v6.ipv6mr_interface = 0;
    mreq_v6.ipv6mr_multiaddr = multiaddr_.in6_.sin6_addr;
    return socket_->set_optval(IPPROTO_IPV6, onoff ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, &mreq_v6, (int)sizeof(mreq_v6));
  }
}
void io_channel::set_host(cxx17::string_view host)
{
  if (this->remote_host_ != host)
  {
    cxx17::assign(this->remote_host_, host);
    yasio__setbits(properties_, YCPF_HOST_MOD);
  }
}
void io_channel::set_port(u_short port)
{
  if (port == 0)
    return;
  if (this->remote_port_ != port)
  {
    this->remote_port_ = port;
    yasio__setbits(properties_, YCPF_PORT_MOD);
  }
}
void io_channel::configure_address()
{
  if (yasio__testbits(properties_, YCPF_HOST_MOD))
  {
    yasio__clearbits(properties_, YCPF_HOST_MOD);
    this->remote_eps_.clear();
    ip::endpoint ep;
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
    if (yasio__unlikely(yasio__testbits(properties_, YCM_UDS)))
    {
      ep.as_un(this->remote_host_.c_str());
      this->remote_eps_.push_back(ep);
      this->dns_queries_state_ = YDQS_READY;
      return;
    }
#endif
    if (ep.as_in(this->remote_host_.c_str(), this->remote_port_))
    {
      this->remote_eps_.push_back(ep);
      this->dns_queries_state_ = YDQS_READY;
    }
    else
    {
      yasio__setbits(properties_, YCPF_NEEDS_QUERIES);
      this->dns_queries_state_ = YDQS_DIRTY;
    }
  }

  if (yasio__testbits(properties_, YCPF_PORT_MOD))
  {
    yasio__clearbits(properties_, YCPF_PORT_MOD);
    if (!this->remote_eps_.empty())
      for (auto& ep : this->remote_eps_)
        ep.port(this->remote_port_);
  }
}
int io_channel::__builtin_decode_len(void* d, int n)
{
  int loffset = uparams_.length_field_offset;
  int lsize   = uparams_.length_field_length;
  if (loffset >= 0)
  {
    assert(lsize >= 1 && lsize <= YASIO_SSIZEOF(int));
    int len = 0;
    if (n >= (loffset + lsize))
    {
      ::memcpy(&len, (uint8_t*)d + loffset, lsize);
      len = yasio::network_to_host(len, lsize);
      len += uparams_.length_adjustment;
      if (len > uparams_.max_frame_length)
        len = -1;
    }
    return len;
  }
  return n;
}
// -------------------- io_transport ---------------------
io_transport::io_transport(io_channel* ctx, std::shared_ptr<xxsocket>& s) : ctx_(ctx)
{
  this->state_  = io_base::state::OPEN;
  this->socket_ = s;
#if !defined(YASIO_MINIFY_EVENT)
  this->ud_.ptr = nullptr;
#endif
}
const print_fn2_t& io_transport::__get_cprint() const { return ctx_->get_service().options_.print_; }
int io_transport::write(std::vector<char>&& buffer, completion_cb_t&& handler)
{
  int n = static_cast<int>(buffer.size());
  send_queue_.emplace(cxx14::make_unique<io_send_op>(std::move(buffer), std::move(handler)));
  get_service().interrupt();
  return n;
}
int io_transport::do_read(int revent, int& error, highp_time_t&) { return revent ? this->call_read(buffer_ + offset_, sizeof(buffer_) - offset_, error) : 0; }
bool io_transport::do_write(highp_time_t& wait_duration)
{
  bool ret = false;
  do
  {
    if (!socket_->is_open())
      break;

    int error = 0;
    auto wrap = send_queue_.peek();
    if (wrap)
    {
      auto& v = *wrap;
      if (call_write(v.get(), error) < 0)
      {
        this->set_last_errno(error, yasio::net::io_base::error_stage::WRITE);
        break;
      }
    }

    bool no_wevent = send_queue_.empty();
    if (yasio__unlikely(!no_wevent))
    { // still have work to do
      no_wevent = (error != EWOULDBLOCK && error != EAGAIN && error != ENOBUFS);
      if (!no_wevent)
      { // system kernel buffer full
        if (!pollout_registerred_)
        {
          get_service().register_descriptor(socket_->native_handle(), YEM_POLLOUT);
          pollout_registerred_ = true;
        }
      }
      else
        wait_duration = yasio__min_wait_duration;
    }
    if (no_wevent && pollout_registerred_)
    {
      get_service().unregister_descriptor(socket_->native_handle(), YEM_POLLOUT);
      pollout_registerred_ = false;
    }
    ret = true;
  } while (false);

  return ret;
}
int io_transport::call_read(void* data, int size, int& error)
{
  int n = read_cb_(data, size);
  if (n > 0)
  {
    ctx_->bytes_transferred_ += n;
    return n;
  }
  if (n < 0)
  {
    error = xxsocket::get_last_errno();
    if (xxsocket::not_recv_error(error))
      return (error = 0); // status ok, clear error
    return n;
  }
  if (yasio__testbits(ctx_->properties_, YCM_TCP))
  {
    error = yasio::errc::eof;
    return -1;
  }
  return 0;
}
int io_transport::call_write(io_send_op* op, int& error)
{
  int n = op->perform(this, op->buffer_.data() + op->offset_, static_cast<int>(op->buffer_.size() - op->offset_));
  if (n > 0)
  {
    // #performance: change offset only, remain data will be send at next frame.
    op->offset_ += n;
    if (op->offset_ == op->buffer_.size())
      this->complete_op(op, 0);
  }
  else if (n < 0)
  {
    error = xxsocket::get_last_errno();
    if (xxsocket::not_send_error(error))
      n = 0;
    else if (yasio__testbits(ctx_->properties_, YCM_UDP))
    { // !!! For udp, simply drop the op instead trigger handle close,
      // on android device, the error will be 'EPERM' when app in background.
      this->complete_op(op, error);
      n = 0;
    }
  }
  return n;
}
void io_transport::complete_op(io_send_op* op, int error)
{
  YASIO_KLOGV("[index: %d] write complete, bytes transferred: %d/%d", this->cindex(), static_cast<int>(op->offset_), static_cast<int>(op->buffer_.size()));
  if (op->handler_)
    op->handler_(error, op->offset_);
  send_queue_.pop();
}
void io_transport::set_primitives()
{
  this->write_cb_ = [=](const void* data, int len, const ip::endpoint*) { return socket_->send(data, len); };
  this->read_cb_  = [=](void* data, int len) { return socket_->recv(data, len, 0); };
}
// -------------------- io_transport_tcp ---------------------
inline io_transport_tcp::io_transport_tcp(io_channel* ctx, std::shared_ptr<xxsocket>& s) : io_transport(ctx, s) {}
// ----------------------- io_transport_ssl ----------------
#if defined(YASIO_SSL_BACKEND)
io_transport_ssl::io_transport_ssl(io_channel* ctx, std::shared_ptr<xxsocket>& s) : io_transport_tcp(ctx, s), ssl_(std::move(ctx->ssl_))
{
  yasio__clearbits(ctx->properties_, YCPF_SSL_HANDSHAKING);
}
void io_transport_ssl::set_primitives()
{
  this->read_cb_ = [=](void* data, int len) {
#  if YASIO_SSL_BACKEND == 1
    ERR_clear_error();
    int n = ::SSL_read(ssl_, data, len);
    if (n > 0)
      return n;
    int error = SSL_get_error(ssl_, n);
    switch (error)
    {
      case SSL_ERROR_ZERO_RETURN: // n=0, the upper caller will regards as eof
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        /* The operation did not complete; the same TLS/SSL I/O function
           should be called again later. This is basically an EWOULDBLOCK
           equivalent. */
        if (xxsocket::get_last_errno() != EWOULDBLOCK)
          xxsocket::set_last_errno(EWOULDBLOCK);
        break;
      default:
        xxsocket::set_last_errno(yasio::errc::ssl_read_failed);
    }
    return n;
#  elif YASIO_SSL_BACKEND == 2
    auto ssl = static_cast<SSL*>(ssl_);
    int n    = ::mbedtls_ssl_read(ssl, static_cast<uint8_t*>(data), len);
    if (n > 0)
      return n;
    switch (n)
    {
      case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: // n=0, the upper caller will regards as eof
        n = 0;
      case 0:
        ::mbedtls_ssl_close_notify(ssl);
        break;
      case MBEDTLS_ERR_SSL_WANT_READ:
      case MBEDTLS_ERR_SSL_WANT_WRITE:
        if (xxsocket::get_last_errno() != EWOULDBLOCK)
          xxsocket::set_last_errno(EWOULDBLOCK);
        break;
      default:
        xxsocket::set_last_errno(yasio::errc::ssl_read_failed);
    }
    return n;
#  endif
  };
  this->write_cb_ = [=](const void* data, int len, const ip::endpoint*) {
#  if YASIO_SSL_BACKEND == 1
    ERR_clear_error();
    int n = ::SSL_write(ssl_, data, len);
    if (n > 0)
      return n;

    int error = SSL_get_error(ssl_, n);
    switch (error)
    {
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (xxsocket::get_last_errno() != EWOULDBLOCK)
          xxsocket::set_last_errno(EWOULDBLOCK);
        break;
      default:
        xxsocket::set_last_errno(yasio::errc::ssl_write_failed);
    }
#  elif YASIO_SSL_BACKEND == 2
    int n = ::mbedtls_ssl_write(static_cast<SSL*>(ssl_), static_cast<const uint8_t*>(data), len);
    if (n > 0)
      return n;
    switch (n)
    {
      case MBEDTLS_ERR_SSL_WANT_READ:
      case MBEDTLS_ERR_SSL_WANT_WRITE:
        if (xxsocket::get_last_errno() != EWOULDBLOCK)
          xxsocket::set_last_errno(EWOULDBLOCK);
        break;
      default:
        xxsocket::set_last_errno(yasio::errc::ssl_write_failed);
    }
#  endif
    return -1;
  };
}
#endif
// ----------------------- io_transport_udp ----------------
io_transport_udp::io_transport_udp(io_channel* ctx, std::shared_ptr<xxsocket>& s) : io_transport(ctx, s) {}
io_transport_udp::~io_transport_udp() {}
ip::endpoint io_transport_udp::remote_endpoint() const { return !connected_ ? this->peer_ : socket_->peer_endpoint(); }
const ip::endpoint& io_transport_udp::ensure_destination() const
{
  if (this->destination_.af() != AF_UNSPEC)
    return this->destination_;
  return (this->destination_ = this->peer_);
}
int io_transport_udp::confgure_remote(const ip::endpoint& peer)
{
  if (connected_) // connected, update peer is pointless and useless
    return -1;
  this->peer_ = peer;
  return this->connect();
}
int io_transport_udp::connect()
{
  if (connected_)
    return 0;

  if (this->peer_.af() == AF_UNSPEC)
  {
    if (ctx_->remote_eps_.empty())
      return -1;
    this->peer_ = ctx_->remote_eps_[0];
  }

  int retval = this->socket_->connect_n(this->peer_);
  connected_ = (retval == 0);

  set_primitives();
  return retval;
}
int io_transport_udp::disconnect()
{
  const int retval = this->socket_->disconnect();
  if (retval == 0)
  {
    connected_ = false;
    set_primitives();
  }
  return retval;
}
int io_transport_udp::write(std::vector<char>&& buffer, completion_cb_t&& handler)
{
  return connected_ ? io_transport::write(std::move(buffer), std::move(handler)) : write_to(std::move(buffer), ensure_destination(), std::move(handler));
}
int io_transport_udp::write_to(std::vector<char>&& buffer, const ip::endpoint& to, completion_cb_t&& handler)
{
  int n = static_cast<int>(buffer.size());
  send_queue_.emplace(cxx14::make_unique<io_sendto_op>(std::move(buffer), std::move(handler), to));
  get_service().interrupt();
  return n;
}
void io_transport_udp::set_primitives()
{
  if (connected_)
    io_transport::set_primitives();
  else
  {
    this->write_cb_ = [=](const void* data, int len, const ip::endpoint* destination) {
      assert(destination);
      int n = socket_->sendto(data, len, *destination);
      if (n < 0)
      {
        auto error = xxsocket::get_last_errno();
        if (!xxsocket::not_send_error(error))
          YASIO_KLOGI("[index: %d] write udp socket failed, ec=%d, detail:%s", this->cindex(), error, io_service::strerror(error));
      }
      return n;
    };
    this->read_cb_ = [=](void* data, int len) {
      ip::endpoint peer;
      int n = socket_->recvfrom(data, len, peer);
      if (n > 0)
        this->peer_ = peer;
      return n;
    };
  }
}
int io_transport_udp::handle_input(const char* buf, int bytes_transferred, int& /*error*/, highp_time_t&)
{ // pure udp, dispatch to upper layer directly
  get_service().handle_event(cxx14::make_unique<io_event>(this->cindex(), io_packet{buf, buf + bytes_transferred}, this));
  return bytes_transferred;
}

#if defined(YASIO_HAVE_KCP)
// ----------------------- io_transport_kcp ------------------
io_transport_kcp::io_transport_kcp(io_channel* ctx, std::shared_ptr<xxsocket>& s) : io_transport_udp(ctx, s)
{
  this->kcp_ = ::ikcp_create(static_cast<IUINT32>(ctx->kcp_conv_), this);
  this->rawbuf_.resize(YASIO_INET_BUFFER_SIZE);
  ::ikcp_nodelay(this->kcp_, 1, 5000 /*kcp max interval is 5000(ms)*/, 2, 1);
  ::ikcp_setoutput(this->kcp_, [](const char* buf, int len, ::ikcpcb* /*kcp*/, void* user) {
    auto t = (io_transport_kcp*)user;
    if (yasio__min_wait_duration == 0)
      return t->write_cb_(buf, len, &t->ensure_destination());
    // Enqueue to transport queue
    return t->io_transport_udp::write(std::vector<char>(buf, buf + len), nullptr);
  });
}
io_transport_kcp::~io_transport_kcp() { ::ikcp_release(this->kcp_); }

int io_transport_kcp::write(std::vector<char>&& buffer, completion_cb_t&& /*handler*/)
{
  std::lock_guard<std::recursive_mutex> lck(send_mtx_);
  int len    = static_cast<int>(buffer.size());
  int retval = ::ikcp_send(kcp_, buffer.data(), len);
  get_service().interrupt();
  return retval == 0 ? len : retval;
}
int io_transport_kcp::do_read(int revent, int& error, highp_time_t& wait_duration)
{
  int n = revent ? this->call_read(&rawbuf_.front(), static_cast<int>(rawbuf_.size()), error) : 0;
  if (n > 0)
    this->handle_input(rawbuf_.data(), n, error, wait_duration);
  if (!error)
  { // !important, should always try to call ikcp_recv when no error occured.
    n = ::ikcp_recv(kcp_, buffer_ + offset_, sizeof(buffer_) - offset_);
    if (n > 0) // If got data from kcp, don't wait
      wait_duration = yasio__min_wait_duration;
    else if (n < 0)
      n = 0; // EAGAIN/EWOULDBLOCK
  }
  return n;
}
int io_transport_kcp::handle_input(const char* buf, int len, int& error, highp_time_t& wait_duration)
{
  // ikcp in event always in service thread, so no need to lock
  if (0 == ::ikcp_input(kcp_, buf, len))
  {
    this->check_timeout(wait_duration); // call ikcp_check
    return len;
  }

  // simply regards -1,-2,-3 as error and trigger connection lost event.
  error = yasio::errc::invalid_packet;
  return -1;
}
bool io_transport_kcp::do_write(highp_time_t& wait_duration)
{
  std::lock_guard<std::recursive_mutex> lck(send_mtx_);

  ::ikcp_update(kcp_, static_cast<IUINT32>(::yasio::clock()));
  ::ikcp_flush(kcp_);
  this->check_timeout(wait_duration); // call ikcp_check
  if (yasio__min_wait_duration == 0)
    return true;
  // Call super do_write to perform low layer socket.send
  // benefit of transport queue:
  // a. cache udp data if kernel buffer full
  // b. lower packet lose, but may reduce transfer performance and large memory use
  return io_transport_udp::do_write(wait_duration);
}
void io_transport_kcp::check_timeout(highp_time_t& wait_duration) const
{
  auto current          = static_cast<IUINT32>(::yasio::clock());
  auto expire_time      = ::ikcp_check(kcp_, current);
  highp_time_t duration = static_cast<highp_time_t>(expire_time - current) * std::milli::den;
  if (duration < 0)
    duration = yasio__min_wait_duration;
  if (wait_duration > duration)
    wait_duration = duration;
}
#endif

// ------------------------ io_service ------------------------
void io_service::init_globals(const yasio::inet::print_fn2_t& prt) { yasio__shared_globals(prt).cprint_ = prt; }
void io_service::cleanup_globals() { yasio__shared_globals().cprint_ = nullptr; }
unsigned int io_service::tcp_rtt(transport_handle_t transport) { return transport->is_open() ? transport->socket_->tcp_rtt() : 0; }
io_service::io_service() { this->init(nullptr, 1); }
io_service::io_service(int channel_count) { this->init(nullptr, channel_count); }
io_service::io_service(const io_hostent& channel_ep) { this->init(&channel_ep, 1); }
io_service::io_service(const std::vector<io_hostent>& channel_eps)
{
  this->init(!channel_eps.empty() ? channel_eps.data() : nullptr, static_cast<int>(channel_eps.size()));
}
io_service::io_service(const io_hostent* channel_eps, int channel_count) { this->init(channel_eps, channel_count); }
io_service::~io_service()
{
  this->stop();
  this->cleanup();
}
void io_service::start(event_cb_t cb)
{
  if (state_ == io_service::state::IDLE)
  {
    auto& global_state = yasio__shared_globals();
    if (!this->options_.print_)
      this->options_.print_ = global_state.cprint_;

    if (cb)
      options_.on_event_ = std::move(cb);
    this->state_ = io_service::state::RUNNING;
    if (!options_.no_new_thread_)
    {
      this->worker_    = std::thread(&io_service::run, this);
      this->worker_id_ = worker_.get_id();
    }
    else
    {
      this->worker_id_               = std::this_thread::get_id();
      this->options_.deferred_event_ = false;
      run();
      handle_stop();
    }
  }
}
void io_service::stop()
{
  if (this->state_ == io_service::state::RUNNING)
  {
    this->state_ = io_service::state::STOPPING;
    this->interrupt();
    this->join();
  }
  else if (this->state_ == io_service::state::STOPPING)
    this->join();
}
void io_service::join()
{
  if (this->worker_.joinable())
  {
    if (std::this_thread::get_id() != this->worker_id_)
    {
      this->worker_.join();
      handle_stop();
    }
    else
      xxsocket::set_last_errno(EAGAIN);
  }
}
void io_service::handle_stop()
{
  clear_transports();
  this->state_ = io_service::state::IDLE;
}
void io_service::init(const io_hostent* channel_eps, int channel_count)
{
  // at least one channel
  if (channel_count < 1)
    channel_count = 1;

  FD_ZERO(&fds_array_[read_op]);
  FD_ZERO(&fds_array_[write_op]);
  FD_ZERO(&fds_array_[except_op]);

  this->max_nfds_  = 0;
  options_.resolv_ = [=](std::vector<ip::endpoint>& eps, const char* host, unsigned short port) { return this->resolve(eps, host, port); };
  register_descriptor(interrupter_.read_descriptor(), YEM_POLLIN);

  // Create channels
  create_channels(channel_eps, channel_count);

#if !defined(YASIO_HAVE_CARES)
  life_mutex_ = std::make_shared<cxx17::shared_mutex>();
  life_token_ = std::make_shared<life_token>();
#endif
  this->state_ = io_service::state::IDLE;
}
void io_service::cleanup()
{
  if (this->state_ == io_service::state::IDLE)
  {
#if !defined(YASIO_HAVE_CARES)
    std::unique_lock<cxx17::shared_mutex> lck(*life_mutex_);
    life_token_.reset();
#endif

    clear_channels();
    this->events_.clear();
    this->timer_queue_.clear();

    unregister_descriptor(interrupter_.read_descriptor(), YEM_POLLIN);

    options_.on_event_ = nullptr;
    options_.resolv_   = nullptr;

    /// purge transport pool memory
    for (auto o : tpool_)
      ::operator delete(o);
    tpool_.clear();

    this->state_ = io_service::state::UNINITIALIZED;
  }
}
void io_service::create_channels(const io_hostent* channel_eps, int channel_count)
{
  for (auto i = 0; i < channel_count; ++i)
  {
    auto channel = new io_channel(*this, i);
    if (channel_eps != nullptr)
      channel->set_address(channel_eps[i].host_, channel_eps[i].port_);
    channels_.push_back(channel);
  }
}
void io_service::clear_channels()
{
  this->channel_ops_.clear();
  for (auto channel : channels_)
  {
    channel->timer_.cancel(*this);
    cleanup_io(channel);
    delete channel;
  }
  channels_.clear();
}
void io_service::clear_transports()
{
  for (auto transport : transports_)
  {
    cleanup_io(transport);
    transport->~io_transport();
    this->tpool_.push_back(transport);
  }
  transports_.clear();
}
void io_service::dispatch(int max_count)
{
  if (options_.on_event_)
    this->events_.consume(max_count, options_.on_event_);
}
void io_service::run()
{
  yasio::set_thread_name("yasio");

#if defined(YASIO_SSL_BACKEND)
  init_ssl_context();
#endif
#if defined(YASIO_HAVE_CARES)
  recreate_ares_channel();
#endif

  // Call once at startup
  this->ipsv_ = static_cast<u_short>(xxsocket::getipsv());

  // The core event loop
  fd_set fds_array[max_ops];
  this->wait_duration_ = YASIO_MAX_WAIT_DURATION;
  for (; this->state_ == io_service::state::RUNNING;)
  {
    auto wait_duration   = get_timeout(this->wait_duration_); // Gets current wait duration
    this->wait_duration_ = YASIO_MAX_WAIT_DURATION;           // Reset next wait duration
    if (wait_duration > 0)
    {
      int retval = do_select(fds_array, wait_duration);
      if (this->state_ != io_service::state::RUNNING)
        break;

      if (retval < 0)
      {
        int ec = xxsocket::get_last_errno();
        YASIO_KLOGD("[core] do_select failed, ec=%d, detail:%s\n", ec, io_service::strerror(ec));
        if (ec != EBADF)
          continue; // Try again.
        goto _L_end;
      }

      if (retval == 0)
        YASIO_KLOGV("[core] %s", "do_select is timeout, process_timers()");
      else if (FD_ISSET(this->interrupter_.read_descriptor(), &(fds_array[read_op])))
      { // Reset the interrupter.
        if (!interrupter_.reset())
          interrupter_.recreate();
        --retval;
      }
    }

#if defined(YASIO_HAVE_CARES)
    // process possible async resolve requests.
    process_ares_requests(fds_array);
#endif

    // process active transports
    process_transports(fds_array);

    // process active channels
    process_channels(fds_array);

    // process timeout timers
    process_timers();
  }

_L_end:
  (void)0; // ONLY for xcode compiler happy.
#if defined(YASIO_HAVE_CARES)
  destroy_ares_channel();
#endif
#if defined(YASIO_SSL_BACKEND)
  cleanup_ssl_context();
#endif
}
void io_service::process_transports(fd_set* fds_array)
{
  // preform transports
  for (auto iter = transports_.begin(); iter != transports_.end();)
  {
    auto transport = *iter;
    bool ok        = (do_read(transport, fds_array) && do_write(transport));
    if (ok)
    {
      int opm = transport->opmask_ | transport->ctx_->opmask_;
      if (0 == opm)
      { // no open/close operations request
        ++iter;
        continue;
      }
      shutdown_internal(transport);
    }

    handle_close(transport);
    iter = transports_.erase(iter);
  }
}
void io_service::process_channels(fd_set* fds_array)
{
  if (!this->channel_ops_.empty())
  {
    // perform active channels
    std::lock_guard<std::recursive_mutex> lck(this->channel_ops_mtx_);
    for (auto iter = this->channel_ops_.begin(); iter != this->channel_ops_.end();)
    {
      auto ctx    = *iter;
      bool finish = true;
      ctx->configure_address();
      if (yasio__testbits(ctx->properties_, YCM_CLIENT))
      { // resolving, opening
        if (yasio__testbits(ctx->opmask_, YOPM_OPEN))
        {
          switch (this->query_ares_state(ctx))
          {
            case YDQS_READY:
              do_nonblocking_connect(ctx);
              break;
            case YDQS_FAILED:
              handle_connect_failed(ctx, yasio::errc::resolve_host_failed);
              break;
            default:; // YDQS_INPRROGRESS
          }
        }
        else if (ctx->state_ == io_base::state::OPENING)
          do_nonblocking_connect_completion(ctx, fds_array);

        finish = ctx->error_ != EINPROGRESS && !yasio__testbits(ctx->opmask_, YOPM_OPEN);
      }
      else if (yasio__testbits(ctx->properties_, YCM_SERVER))
      {
        auto opmask = ctx->opmask_;
        if (yasio__testbits(opmask, YOPM_OPEN))
          do_nonblocking_accept(ctx);
        else if (yasio__testbits(opmask, YOPM_CLOSE))
          cleanup_channel(ctx);

        finish = (ctx->state_ != io_base::state::OPEN);
        if (!finish)
          do_nonblocking_accept_completion(ctx, fds_array);
        else
          ctx->bytes_transferred_ = 0;
      }

      if (finish)
        iter = this->channel_ops_.erase(iter);
      else
        ++iter;
    }
  }
}
void io_service::close(int index)
{
  // Gets channel context
  auto channel = channel_at(index);
  if (!channel)
    return;

  if (!yasio__testbits(channel->opmask_, YOPM_CLOSE))
  {
    yasio__clearbits(channel->opmask_, YOPM_OPEN);
    if (channel->socket_->is_open())
    {
      yasio__setbits(channel->opmask_, YOPM_CLOSE);
      this->interrupt();
    }
  }
}
void io_service::close(transport_handle_t transport)
{
  if (!yasio__testbits(transport->opmask_, YOPM_CLOSE))
  {
    yasio__setbits(transport->opmask_, YOPM_CLOSE);
    this->interrupt();
  }
}
bool io_service::is_open(transport_handle_t transport) const { return transport->is_open(); }
bool io_service::is_open(int index) const
{
  auto ctx = channel_at(index);
  return ctx != nullptr && ctx->state_ == io_base::state::OPEN;
}
void io_service::open(size_t index, int kind)
{
  assert((kind > 0 && kind <= 0xff) && ((kind & (kind - 1)) != 0));

  auto ctx = channel_at(index);
  if (ctx != nullptr)
  {
    yasio__setlobyte(ctx->properties_, kind & 0xff);
    if (yasio__testbits(kind, YCM_TCP))
      ctx->socktype_ = SOCK_STREAM;
    else if (yasio__testbits(kind, YCM_UDP))
      ctx->socktype_ = SOCK_DGRAM;

    open_internal(ctx);
  }
}
io_channel* io_service::channel_at(size_t index) const { return (index < channels_.size()) ? channels_[index] : nullptr; }
void io_service::handle_close(transport_handle_t thandle)
{
  auto ctx = thandle->ctx_;
  auto ec  = thandle->error_;

  // @Because we can't retrive peer endpoint when connect reset by peer, so use id to trace.
  YASIO_KLOGD("[index: %d] the connection #%u(%p) is lost, ec=%d, where=%d, detail:%s", ctx->index_, thandle->id_, thandle, ec, (int)thandle->error_stage_,
              io_service::strerror(ec));

  handle_event(cxx14::make_unique<io_event>(thandle->cindex(), YEK_ON_CLOSE, ec, thandle));
  cleanup_io(thandle, false);
  deallocate_transport(thandle);

  if (yasio__testbits(ctx->properties_, YCM_CLIENT))
  {
    ctx->error_ = 0;
    yasio__clearbits(ctx->opmask_, YOPM_CLOSE);
    ctx->state_ = io_base::state::CLOSED;
    ctx->properties_ &= 0xffffff; // clear highest byte flags
  }
}
void io_service::register_descriptor(const socket_native_type fd, int flags)
{
  if (yasio__testbits(flags, YEM_POLLIN))
    FD_SET(fd, &(fds_array_[read_op]));

  if (yasio__testbits(flags, YEM_POLLOUT))
    FD_SET(fd, &(fds_array_[write_op]));

  if (yasio__testbits(flags, YEM_POLLERR))
    FD_SET(fd, &(fds_array_[except_op]));

  if (max_nfds_ < static_cast<int>(fd) + 1)
    max_nfds_ = static_cast<int>(fd) + 1;
}
void io_service::unregister_descriptor(const socket_native_type fd, int flags)
{
  if (yasio__testbits(flags, YEM_POLLIN))
    FD_CLR(fd, &(fds_array_[read_op]));

  if (yasio__testbits(flags, YEM_POLLOUT))
    FD_CLR(fd, &(fds_array_[write_op]));

  if (yasio__testbits(flags, YEM_POLLERR))
    FD_CLR(fd, &(fds_array_[except_op]));
}
int io_service::write(transport_handle_t transport, std::vector<char> buffer, completion_cb_t handler)
{
  if (transport && transport->is_open())
    return !buffer.empty() ? transport->write(std::move(buffer), std::move(handler)) : 0;
  else
  {
    YASIO_KLOGE("[transport: %p] send failed, the connection not ok!", (void*)transport);
    return -1;
  }
}
int io_service::write_to(transport_handle_t transport, std::vector<char> buffer, const ip::endpoint& to, completion_cb_t handler)
{
  if (transport && transport->is_open())
    return !buffer.empty() ? transport->write_to(std::move(buffer), to, std::move(handler)) : 0;
  else
  {
    YASIO_KLOGE("[transport: %p] send failed, the connection not ok!", (void*)transport);
    return -1;
  }
}
void io_service::handle_event(event_ptr event)
{
  if (options_.deferred_event_)
  {
    if (options_.on_defer_event_ && !options_.on_defer_event_(event))
      return;
    events_.emplace(std::move(event));
  }
  else
    options_.on_event_(std::move(event));
}
void io_service::do_nonblocking_connect(io_channel* ctx)
{
  assert(ctx->dns_queries_state_ == YDQS_READY);
  if (this->ipsv_ == 0)
    this->ipsv_ = static_cast<u_short>(xxsocket::getipsv());
  if (ctx->socket_->is_open())
    cleanup_io(ctx);

  yasio__clearbits(ctx->opmask_, YOPM_OPEN);
  if (ctx->remote_eps_.empty())
  {
    this->handle_connect_failed(ctx, yasio::errc::no_available_address);
    return;
  }

  ctx->state_ = io_base::state::OPENING;
  auto& ep    = ctx->remote_eps_[0];
  YASIO_KLOGD("[index: %d] connecting server %s(%s):%u...", ctx->index_, ctx->remote_host_.c_str(), ep.ip().c_str(), ctx->remote_port_);
  if (ctx->socket_->open(ep.af(), ctx->socktype_))
  {
    int ret = 0;
    if (yasio__testbits(ctx->properties_, YCF_REUSEADDR))
      ctx->socket_->reuse_address(true);
    if (yasio__testbits(ctx->properties_, YCF_EXCLUSIVEADDRUSE))
      ctx->socket_->exclusive_address(true);
    if (ctx->local_port_ != 0 || !ctx->local_host_.empty() || yasio__testbits(ctx->properties_, YCM_UDP))
    {
      if (yasio__likely(!yasio__testbits(ctx->properties_, YCM_UDS)))
      {
        auto ifaddr = ctx->local_host_.empty() ? YASIO_ADDR_ANY(ep.af()) : ctx->local_host_.c_str();
        ret         = ctx->socket_->bind(ifaddr, ctx->local_port_);
      }
    }

    if (ret == 0)
    {
      // tcp connect directly, for udp do not need to connect.
      if (yasio__testbits(ctx->properties_, YCM_TCP))
        ret = xxsocket::connect_n(ctx->socket_->native_handle(), ep);
      else // udp, we should set non-blocking mode manually
        ctx->socket_->set_nonblocking(true);

      // join the multicast group for udp
      if (yasio__testbits(ctx->properties_, YCPF_MCAST))
        ctx->join_multicast_group();
    }

    if (ret < 0)
    { // setup non-blocking connect
      int error = xxsocket::get_last_errno();
      if (error != EINPROGRESS && error != EWOULDBLOCK)
        this->handle_connect_failed(ctx, error);
      else
      {
        ctx->set_last_errno(EINPROGRESS);
        register_descriptor(ctx->socket_->native_handle(), YEM_POLLIN | YEM_POLLOUT);
        ctx->timer_.expires_from_now(std::chrono::microseconds(options_.connect_timeout_));
        ctx->timer_.async_wait_once(*this, [ctx](io_service& thiz) {
          if (ctx->state_ != io_base::state::OPEN)
            thiz.handle_connect_failed(ctx, ETIMEDOUT);
        });
      }
    }
    else if (ret == 0)
    { // connect server successful immediately.
      register_descriptor(ctx->socket_->native_handle(), YEM_POLLIN);
      handle_connect_succeed(ctx, ctx->socket_);
    } // !!!NEVER GO HERE
  }
  else
    this->handle_connect_failed(ctx, xxsocket::get_last_errno());
}

void io_service::do_nonblocking_connect_completion(io_channel* ctx, fd_set* fds_array)
{
  assert(ctx->state_ == io_base::state::OPENING && yasio__testbits(ctx->properties_, YCM_TCP) && yasio__testbits(ctx->properties_, YCM_CLIENT));
  if (ctx->state_ == io_base::state::OPENING)
  {
#if !defined(YASIO_SSL_BACKEND)
    int error = -1;
    if (FD_ISSET(ctx->socket_->native_handle(), &fds_array[write_op]) || FD_ISSET(ctx->socket_->native_handle(), &fds_array[read_op]))
    {
      if (ctx->socket_->get_optval(SOL_SOCKET, SO_ERROR, error) >= 0 && error == 0)
      {
        // The nonblocking tcp handshake complete, remove write event avoid high-CPU occupation
        unregister_descriptor(ctx->socket_->native_handle(), YEM_POLLOUT);
        handle_connect_succeed(ctx, ctx->socket_);
      }
      else
        handle_connect_failed(ctx, error);

      ctx->timer_.cancel(*this);
    }
#else
    if (!yasio__testbits(ctx->properties_, YCPF_SSL_HANDSHAKING))
    {
      int error = -1;
      if (FD_ISSET(ctx->socket_->native_handle(), &fds_array[write_op]) || FD_ISSET(ctx->socket_->native_handle(), &fds_array[read_op]))
      {
        if (ctx->socket_->get_optval(SOL_SOCKET, SO_ERROR, error) >= 0 && error == 0)
        {
          // The nonblocking tcp handshake complete, remove write event avoid high-CPU occupation
          unregister_descriptor(ctx->socket_->native_handle(), YEM_POLLOUT);
          if (!yasio__testbits(ctx->properties_, YCM_SSL))
            handle_connect_succeed(ctx, ctx->socket_);
          else
            do_ssl_handshake(ctx);
        }
        else
          handle_connect_failed(ctx, error);
      }
    }
    else
      do_ssl_handshake(ctx);

    if (ctx->state_ != io_base::state::OPENING)
      ctx->timer_.cancel(*this);
#endif
  }
}
#if defined(YASIO_SSL_BACKEND)
void io_service::init_ssl_context()
{
#  if YASIO_SSL_BACKEND == 1
#    if (OPENSSL_VERSION_NUMBER >= 0x10100000L)
  auto req_method = ::TLS_client_method();
#    else
  auto req_method = ::SSLv23_client_method();
#    endif
  ssl_ctx_ = ::SSL_CTX_new(req_method);

#    if defined(SSL_MODE_RELEASE_BUFFERS)
  ::SSL_CTX_set_mode(ssl_ctx_, SSL_MODE_RELEASE_BUFFERS);
#    endif

  ::SSL_CTX_set_mode(ssl_ctx_, SSL_MODE_ENABLE_PARTIAL_WRITE);
  if (!this->options_.cafile_.empty())
  {
    if (::SSL_CTX_load_verify_locations(ssl_ctx_, this->options_.cafile_.c_str(), nullptr) == 1)
    {
      ::SSL_CTX_set_verify(ssl_ctx_, SSL_VERIFY_PEER, ::SSL_CTX_get_verify_callback(ssl_ctx_));
#    if OPENSSL_VERSION_NUMBER >= 0x10101000L
      ::SSL_CTX_set_post_handshake_auth(ssl_ctx_, 1);
#    endif
#    if defined(X509_V_FLAG_PARTIAL_CHAIN)
      /* Have intermediate certificates in the trust store be treated as
         trust-anchors, in the same way as self-signed root CA certificates
         are. This allows users to verify servers using the intermediate cert
         only, instead of needing the whole chain. */
      X509_STORE_set_flags(SSL_CTX_get_cert_store(ssl_ctx_), X509_V_FLAG_PARTIAL_CHAIN);
#    endif
    }
    else
      YASIO_KLOGE("[global] load ca certifaction file failed!");
  }
  else
    SSL_CTX_set_verify(ssl_ctx_, SSL_VERIFY_NONE, nullptr);
#  elif YASIO_SSL_BACKEND == 2
  const char* pers = "yasio_ssl_client";
  ssl_ctx_ = new SSL_CTX();
  ::mbedtls_ssl_config_init(&ssl_ctx_->conf);
  ::mbedtls_x509_crt_init(&ssl_ctx_->cacert);
  ::mbedtls_ctr_drbg_init(&ssl_ctx_->ctr_drbg);
  ::mbedtls_entropy_init(&ssl_ctx_->entropy);
  int ret = ::mbedtls_ctr_drbg_seed(&ssl_ctx_->ctr_drbg, ::mbedtls_entropy_func, &ssl_ctx_->entropy, (const unsigned char*)pers, strlen(pers));
  if (ret != 0)
    YASIO_KLOGE("mbedtls_ctr_drbg_seed fail with ret=%d", ret);

  int authmode = MBEDTLS_SSL_VERIFY_OPTIONAL;
  if (!this->options_.cafile_.empty()) // the cafile_ must be full path
  {
    if ((ret = ::mbedtls_x509_crt_parse_file(&ssl_ctx_->cacert, this->options_.cafile_.c_str())) == 0)
      authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
    else
      YASIO_KLOGE("mbedtls_x509_crt_parse_file with ret=-0x%x", (unsigned int)-ret);
  }

  if ((ret = ::mbedtls_ssl_config_defaults(&ssl_ctx_->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    YASIO_KLOGE("mbedtls_ssl_config_defaults fail with ret=%d", ret);

  ::mbedtls_ssl_conf_authmode(&ssl_ctx_->conf, authmode);
  ::mbedtls_ssl_conf_ca_chain(&ssl_ctx_->conf, &ssl_ctx_->cacert, nullptr);
  ::mbedtls_ssl_conf_rng(&ssl_ctx_->conf, ::mbedtls_ctr_drbg_random, &ssl_ctx_->ctr_drbg);
#  endif
}
void io_service::cleanup_ssl_context()
{
  if (ssl_ctx_)
  {
#  if YASIO_SSL_BACKEND == 1
    SSL_CTX_free((SSL_CTX*)ssl_ctx_);
#  elif YASIO_SSL_BACKEND == 2
    ::mbedtls_x509_crt_free(&ssl_ctx_->cacert);
    ::mbedtls_ssl_config_free(&ssl_ctx_->conf);
    ::mbedtls_ctr_drbg_free(&ssl_ctx_->ctr_drbg);
    ::mbedtls_entropy_free(&ssl_ctx_->entropy);
    delete ssl_ctx_;
#  endif
    ssl_ctx_ = nullptr;
  }
}
void io_service::do_ssl_handshake(io_channel* ctx)
{
  if (!ctx->ssl_)
  {
#  if YASIO_SSL_BACKEND == 1
    auto ssl = ::SSL_new(ssl_ctx_);
    ::SSL_set_fd(ssl, static_cast<int>(ctx->socket_->native_handle()));
    ::SSL_set_connect_state(ssl);
    ::SSL_set_tlsext_host_name(ssl, ctx->remote_host_.c_str());
#  elif YASIO_SSL_BACKEND == 2
    auto ssl = ::mbedtls_ssl_new(ssl_ctx_);
    ::mbedtls_ssl_set_fd(ssl, static_cast<int>(ctx->socket_->native_handle()));
    ::mbedtls_ssl_set_hostname(ssl, ctx->remote_host_.c_str());
#  endif
    yasio__setbits(ctx->properties_, YCPF_SSL_HANDSHAKING); // start ssl handshake
    ctx->ssl_.reset(ssl);
  }

#  if YASIO_SSL_BACKEND == 1
  int ret = ::SSL_do_handshake(ctx->ssl_);
  if (ret != 1)
  {
    int status = ::SSL_get_error(ctx->ssl_, ret);
    /*
    When using a non-blocking socket, nothing is to be done, but select() can be used to check for
    the required condition: https://www.openssl.org/docs/manmaster/man3/SSL_do_handshake.html
    */
    if (status == SSL_ERROR_WANT_READ || status == SSL_ERROR_WANT_WRITE || status == SSL_ERROR_WANT_ASYNC)
      ; // Nothing need to do
    else
    {
      int error = static_cast<int>(ERR_get_error());
      if (error)
      {
        char errstring[256] = {0};
        ERR_error_string_n(error, errstring, sizeof(errstring));
        YASIO_KLOGE("[index: %d] SSL_do_handshake fail with ret=%d,error=%X, detail:%s", ctx->index_, ret, error, errstring);
      }
      else
      {
        error = xxsocket::get_last_errno();
        YASIO_KLOGE("[index: %d] SSL_do_handshake fail with ret=%d,status=%d, error=%d, detail:%s", ctx->index_, ret, status, error, xxsocket::strerror(error));
      }
      ctx->ssl_.destroy();
      handle_connect_failed(ctx, yasio::errc::ssl_handshake_failed);
    }
  }
  else
    handle_connect_succeed(ctx, ctx->socket_);
#  elif YASIO_SSL_BACKEND == 2
  auto ssl = static_cast<SSL*>(ctx->ssl_);
  int ret = ::mbedtls_ssl_handshake_step(ssl);
  if (ret == 0)
  {
    if (ssl->state != MBEDTLS_SSL_HANDSHAKE_OVER)
      interrupt();
    else // mbedtls_ssl_get_verify_result return 0 when valid cacert provided
      handle_connect_succeed(ctx, ctx->socket_);
  }
  else
  {
    char errstring[256] = {0};
    switch (ret)
    {
      case MBEDTLS_ERR_SSL_WANT_READ:
      case MBEDTLS_ERR_SSL_WANT_WRITE:
        break; // Nothing need to do
      default:
        ::mbedtls_strerror(ret, errstring, sizeof(errstring));
        YASIO_KLOGE("[index: %d] mbedtls_ssl_handshake_step fail with ret=%d, detail:%s", ctx->index_, ret, errstring);
        ctx->ssl_.destroy();
        handle_connect_failed(ctx, yasio::errc::ssl_handshake_failed);
    }
  }
#  endif
}
#endif
#if defined(YASIO_HAVE_CARES)
void io_service::ares_getaddrinfo_cb(void* arg, int status, int timeouts, ares_addrinfo* answerlist)
{
  auto ctx              = (io_channel*)arg;
  auto& current_service = ctx->get_service();
  current_service.ares_work_finished();

  if (status == ARES_SUCCESS && answerlist != nullptr)
  {
    for (auto ai = answerlist->nodes; ai != nullptr; ai = ai->ai_next)
    {
      if (ai->ai_family == AF_INET6 || ai->ai_family == AF_INET)
      {
        ctx->remote_eps_.push_back(ip::endpoint(ai->ai_addr));
        break;
      }
    }
  }

  auto __get_cprint = [&]() -> const print_fn2_t& { return current_service.options_.print_; };
  if (!ctx->remote_eps_.empty())
  {
    ctx->dns_queries_state_     = YDQS_READY;
    ctx->dns_queries_timestamp_ = highp_clock();
#  if defined(YASIO_ENABLE_ARES_PROFILER)
    YASIO_KLOGD("[index: %d] ares_getaddrinfo_cb: resolve %s succeed, cost:%g(ms)", ctx->index_, ctx->remote_host_.c_str(),
                (ctx->dns_queries_timestamp_ - ctx->ares_start_time_) / 1000.0);
#  endif
  }
  else
  {
    ctx->set_last_errno(yasio::errc::resolve_host_failed);
    ctx->dns_queries_state_ = YDQS_FAILED;
    YASIO_KLOGE("[index: %d] ares_getaddrinfo_cb: resolve %s failed, status=%d, detail:%s", ctx->index_, ctx->remote_host_.c_str(), status,
                ::ares_strerror(status));
  }

  current_service.interrupt();
}
void io_service::process_ares_requests(fd_set* fds_array)
{
  if (this->ares_outstanding_work_ > 0)
  {
    ares_socket_t socks[ARES_GETSOCK_MAXNUM] = {0};
    int bitmask                              = ::ares_getsock(this->ares_, socks, ARES_GETSOCK_MAXNUM);

    for (int i = 0; i < ARES_GETSOCK_MAXNUM; ++i)
    {
      if (ARES_GETSOCK_READABLE(bitmask, i) || ARES_GETSOCK_WRITABLE(bitmask, i))
      {
        auto fd = socks[i];
        ::ares_process_fd(this->ares_, FD_ISSET(fd, &(fds_array[read_op])) ? fd : ARES_SOCKET_BAD, FD_ISSET(fd, &(fds_array[write_op])) ? fd : ARES_SOCKET_BAD);
      }
      else
        break;
    }
  }
}
void io_service::recreate_ares_channel()
{
  this->options_.dns_dirty_ = false;
  if (ares_)
    destroy_ares_channel();

  ares_options options = {};
  options.timeout      = static_cast<int>(this->options_.dns_queries_timeout_ / std::milli::den);
  options.tries        = this->options_.dns_queries_tries_;
  int status           = ::ares_init_options(&ares_, &options, ARES_OPT_TIMEOUTMS | ARES_OPT_TRIES /* | ARES_OPT_LOOKUPS*/);
  if (status == ARES_SUCCESS)
  {
    YASIO_KLOGD("[c-ares] create channel succeed");
    config_ares_name_servers();
  }
  else
    YASIO_KLOGE("[c-ares] create channel failed, status=%d, detail:%s", status, ::ares_strerror(status));
}
void io_service::config_ares_name_servers()
{
  std::string nscsv;
  // list all dns servers for resov problem diagnosis
  ares_addr_node* name_servers = nullptr;
  int status                   = ::ares_get_servers(ares_, &name_servers);
  if (status == ARES_SUCCESS)
  {
    for (auto name_server = name_servers; name_server != nullptr; name_server = name_server->next)
      endpoint::inaddr_to_csv_nl(name_server->family, &name_server->addr, nscsv);

    if (!nscsv.empty()) // if no valid name server, use predefined fallback dns
      YASIO_KLOGD("[c-ares] use system dns: %s", nscsv.c_str());
    else
    {
      status = ::ares_set_servers_csv(ares_, YASIO_CARES_FALLBACK_DNS);
      if (status == 0)
        YASIO_KLOGD("[c-ares] set fallback dns: '%s' succeed", YASIO_CARES_FALLBACK_DNS);
      else
        YASIO_KLOGE("[c-ares] set fallback dns: '%s' failed, detail: %s", YASIO_CARES_FALLBACK_DNS, ::ares_strerror(status));
    }
    ::ares_free_data(name_servers);
  }
}
void io_service::destroy_ares_channel()
{
  if (ares_ != nullptr)
  {
    ::ares_cancel(this->ares_);
    ::ares_destroy(this->ares_);
    this->ares_ = nullptr;
  }
}
#endif
void io_service::do_nonblocking_accept(io_channel* ctx)
{ // channel is server
  cleanup_channel(ctx);

  ip::endpoint ep;
  if (yasio__likely(!yasio__testbits(ctx->properties_, YCM_UDS)))
  {
    // server: don't need resolve, don't use remote_eps_
    auto ifaddr = ctx->remote_host_.empty() ? YASIO_ADDR_ANY(local_address_family()) : ctx->remote_host_.c_str();
    ep.as_in(ifaddr, ctx->remote_port_);
  }
#if defined(YASIO_ENABLE_UDS) && YASIO__HAS_UDS
  else
  {
    ep.as_un(ctx->remote_host_.c_str());
    ::unlink(ctx->remote_host_.c_str());
  }
#endif
  int error                  = -1;
  io_base::error_stage where = io_base::error_stage::NONE;
  do
  {
    xxsocket::set_last_errno(0);
    if (!ctx->socket_->open(ep.af(), ctx->socktype_))
    {
      where = io_base::error_stage::OPEN_SOCKET;
      break;
    }

    if (yasio__testbits(ctx->properties_, YCF_REUSEADDR))
      ctx->socket_->reuse_address(true);
    if (yasio__testbits(ctx->properties_, YCF_EXCLUSIVEADDRUSE))
      ctx->socket_->exclusive_address(false);
    if (ctx->socket_->bind(ep) != 0)
    {
      where = io_base::error_stage::BIND_SOCKET;
      break;
    }

    if (yasio__testbits(ctx->properties_, YCM_TCP) && ctx->socket_->listen(YASIO_SOMAXCONN) != 0)
    {
      where = io_base::error_stage::LISTEN_SOCKET;
      break;
    }

    ctx->socket_->set_nonblocking(true);
    ctx->state_ = io_base::state::OPEN;
    if (yasio__testbits(ctx->properties_, YCM_UDP))
    {
      if (yasio__testbits(ctx->properties_, YCPF_MCAST))
        ctx->join_multicast_group();
      ctx->buffer_.resize(YASIO_INET_BUFFER_SIZE);
    }
    register_descriptor(ctx->socket_->native_handle(), YEM_POLLIN);
    YASIO_KLOGD("[index: %d] open server succeed, socket.fd=%d listening at %s...", ctx->index_, (int)ctx->socket_->native_handle(), ep.to_string().c_str());
    error = 0;
  } while (false);

  if (error < 0)
  {
    error = xxsocket::get_last_errno();
    YASIO_KLOGE("[index: %d] open server failed during stage %d, ec=%d, detail:%s", where, ctx->index_, error, io_service::strerror(error));
    ctx->socket_->close();
    ctx->state_ = io_base::state::CLOSED;
  }
#if defined(YASIO_ENABLE_PASSIVE_EVENT)
  handle_event(cxx14::make_unique<io_event>(ctx->index_, YEK_ON_OPEN, error, ctx, 1));
#endif
}
void io_service::do_nonblocking_accept_completion(io_channel* ctx, fd_set* fds_array)
{
  if (ctx->state_ == io_base::state::OPEN)
  {
    int error = 0;
    if (FD_ISSET(ctx->socket_->native_handle(), &fds_array[read_op]) && ctx->socket_->get_optval(SOL_SOCKET, SO_ERROR, error) >= 0 && error == 0)
    {
      if (yasio__testbits(ctx->properties_, YCM_TCP))
      {
        socket_native_type sockfd{invalid_socket};
        error = ctx->socket_->accept_n(sockfd);
        if (error == 0)
          handle_connect_succeed(ctx, std::make_shared<xxsocket>(sockfd));
        else // The non-blocking tcp accept failed can be ignored.
          YASIO_KLOGV("[index: %d] socket.fd=%d, accept failed, ec=%u", ctx->index_, (int)ctx->socket_->native_handle(), error);
      }
      else // YCM_UDP
      {
        ip::endpoint peer;
        int n = ctx->socket_->recvfrom(&ctx->buffer_.front(), static_cast<int>(ctx->buffer_.size()), peer);
        if (n > 0)
        {
          YASIO_KLOGV("[index: %d] recvfrom peer: %s succeed.", ctx->index_, peer.to_string().c_str());
#if !defined(_WIN32)
          auto transport = static_cast<io_transport_udp*>(do_dgram_accept(ctx, peer, error));
#else
          /*
           Because Bind() the client socket to the socket address of the listening socket.  On
           Linux this essentially passes the responsibility for receiving data for the client
           session from the well-known listening socket, to the newly allocated client socket.  It
           is important to note that this behavior is not the same on other platforms, like
           Windows (unfortunately), detail see:
           https://blog.grijjy.com/2018/08/29/creating-high-performance-udp-servers-on-windows-and-linux
           https://cloud.tencent.com/developer/article/1004555
           So we emulate thus by ourself, don't care the performance, just a workaround implementation.
         */
          // for win32, we check exists udp clients by ourself, and only write operation can be
          // perform on transports, the read operation still dispatch by channel.
          auto it        = yasio__find_if(this->transports_, [&peer](const io_transport* transport) {
            using namespace std;
            return yasio__testbits(transport->ctx_->properties_, YCM_UDP) && static_cast<const io_transport_udp*>(transport)->remote_endpoint() == peer;
          });
          auto transport = static_cast<io_transport_udp*>(it != this->transports_.end() ? *it : do_dgram_accept(ctx, peer, error));
#endif
          if (transport)
          {
            if (transport->handle_input(ctx->buffer_.data(), n, error, this->wait_duration_) < 0)
            {
              transport->error_ = error;
              close(transport);
            }
          }
          else
            YASIO_KLOGE("[index: %d] do_dgram_accept failed, ec=%d, detail:%s", ctx->index_, error, this->strerror(error));
        }
        else if (n < 0)
        {
          error = xxsocket::get_last_errno();
          if (!xxsocket::not_recv_error(error))
            YASIO_KLOGE("[index: %d] recvfrom failed, ec=%d, detail:%s", ctx->index_, error, this->strerror(error));
        }
      }
    }
  }
}
transport_handle_t io_service::do_dgram_accept(io_channel* ctx, const ip::endpoint& peer, int& error)
{
  auto new_sock = std::make_shared<xxsocket>();
  if (new_sock->open(peer.af(), SOCK_DGRAM))
  {
    if (yasio__testbits(ctx->properties_, YCF_REUSEADDR))
      new_sock->reuse_address(true);
    if (yasio__testbits(ctx->properties_, YCF_EXCLUSIVEADDRUSE))
      new_sock->exclusive_address(false);
    if (new_sock->bind(YASIO_ADDR_ANY(peer.af()), ctx->remote_port_) == 0)
    {
      new_sock->set_nonblocking(true);
      auto transport = static_cast<io_transport_udp*>(allocate_transport(ctx, std::move(new_sock)));
      // We always establish 4 tuple with clients
      transport->confgure_remote(peer);
#if !defined(_WIN32)
      handle_connect_succeed(transport);
#else
      notify_connect_succeed(transport);
#endif
      return transport;
    }
  }

  // unhandled, get error from system.
  error = xxsocket::get_last_errno();
  return nullptr;
}
void io_service::handle_connect_succeed(transport_handle_t transport)
{
  auto ctx = transport->ctx_;
  ctx->set_last_errno(0); // clear errno, value may be EINPROGRESS
  auto& connection = transport->socket_;
  if (yasio__testbits(ctx->properties_, YCM_CLIENT))
  {
    // Reset client channel bytes transferred when a new connection established
    ctx->bytes_transferred_ = 0;
    ctx->state_             = io_base::state::OPEN;
    if (yasio__testbits(ctx->properties_, YCM_UDP))
      static_cast<io_transport_udp*>(transport)->confgure_remote(ctx->remote_eps_[0]);
  }
  else
    register_descriptor(connection->native_handle(), YEM_POLLIN);
  if (yasio__testbits(ctx->properties_, YCM_TCP))
  {
#if defined(__APPLE__) || defined(__linux__)
    connection->set_optval(SOL_SOCKET, SO_NOSIGPIPE, (int)1);
#endif
    // apply tcp keepalive options
    if (options_.tcp_keepalive_.onoff)
      connection->set_keepalive(options_.tcp_keepalive_.onoff, options_.tcp_keepalive_.idle, options_.tcp_keepalive_.interval, options_.tcp_keepalive_.probs);
  }

  notify_connect_succeed(transport);
}
void io_service::notify_connect_succeed(transport_handle_t t)
{
  auto ctx = t->ctx_;
  auto& s  = t->socket_;
  this->transports_.push_back(t);
  YASIO_KLOGV("[index: %d] sndbuf=%d, rcvbuf=%d", ctx->index_, s->get_optval<int>(SOL_SOCKET, SO_SNDBUF), s->get_optval<int>(SOL_SOCKET, SO_RCVBUF));
  YASIO_KLOGD("[index: %d] the connection #%u(%p) [%s] --> [%s] is established.", ctx->index_, t->id_, t, t->local_endpoint().to_string().c_str(),
              t->remote_endpoint().to_string().c_str());
  handle_event(cxx14::make_unique<io_event>(ctx->index_, YEK_ON_OPEN, 0, t));
}
transport_handle_t io_service::allocate_transport(io_channel* ctx, std::shared_ptr<xxsocket> socket)
{
  transport_handle_t transport;
  void* vp;
  if (!tpool_.empty())
  { // allocate from pool
    vp = tpool_.back();
    tpool_.pop_back();
  }
  else
    vp = ::operator new(yasio__max_tsize);
  do
  {
    if (yasio__testbits(ctx->properties_, YCM_TCP))
    { // tcp like transport
#if defined(YASIO_SSL_BACKEND)
      if (yasio__unlikely(yasio__testbits(ctx->properties_, YCM_SSL)))
      {
        transport = new (vp) io_transport_ssl(ctx, socket);
        break;
      }
#endif
      transport = new (vp) io_transport_tcp(ctx, socket);
    }
    else // udp like transport
    {
#if defined(YASIO_HAVE_KCP)
      if (yasio__unlikely(yasio__testbits(ctx->properties_, YCM_KCP)))
      {
        transport = new (vp) io_transport_kcp(ctx, socket);
        break;
      }
#endif
      transport = new (vp) io_transport_udp(ctx, socket);
    }
  } while (false);
  transport->set_primitives();
  return transport;
}
void io_service::deallocate_transport(transport_handle_t t)
{
  if (t && t->is_valid())
  {
    t->invalid();
    yasio::invoke_dtor(t);
    this->tpool_.push_back(t);
  }
}
void io_service::handle_connect_failed(io_channel* ctx, int error)
{
  ctx->properties_ &= 0xffffff; // clear highest byte flags
  cleanup_io(ctx);
  YASIO_KLOGE("[index: %d] connect server %s failed, ec=%d, detail:%s", ctx->index_, ctx->format_destination().c_str(), error, io_service::strerror(error));
  handle_event(cxx14::make_unique<io_event>(ctx->index_, YEK_ON_OPEN, error, ctx));
}
bool io_service::do_read(transport_handle_t transport, fd_set* fds_array)
{
  bool ret = false;
  do
  {
    if (!transport->socket_->is_open())
      break;
    int error  = 0;
    int revent = FD_ISSET(transport->socket_->native_handle(), &(fds_array[read_op]));
    int n      = transport->do_read(revent, error, this->wait_duration_);
    if (n >= 0)
    {
      YASIO_KLOGV("[index: %d] do_read status ok, bytes transferred: %d, buffer used: %d", transport->cindex(), n, n + transport->offset_);
      if (transport->expected_size_ == -1)
      { // decode length
        int length = transport->ctx_->decode_len_(transport->buffer_, transport->offset_ + n);
        if (length > 0)
        {
          int bytes_to_strip        = ::yasio::clamp(transport->ctx_->uparams_.initial_bytes_to_strip, 0, length - 1);
          transport->expected_size_ = length;
          transport->expected_packet_.reserve((std::min)(length - bytes_to_strip,
                                                         YASIO_MAX_PDU_BUFFER_SIZE)); // #perfomance, avoid memory reallocte.
          unpack(transport, transport->expected_size_, n, bytes_to_strip);
        }
        else if (length == 0) // header insufficient, wait readfd ready at next event frame.
          transport->offset_ += n;
        else
        {
          transport->set_last_errno(yasio::errc::invalid_packet, yasio::net::io_base::error_stage::READ);
          break;
        }
      }
      else
      { // process incompleted pdu
        unpack(transport, transport->expected_size_ - static_cast<int>(transport->expected_packet_.size()), n, 0);
      }
    }
    else
    { // n < 0, regard as connection should close
      transport->set_last_errno(error, yasio::net::io_base::error_stage::READ);
      break;
    }
    ret = true;
  } while (false);
  return ret;
}
void io_service::unpack(transport_handle_t transport, int bytes_expected, int bytes_transferred, int bytes_to_strip)
{
  auto& offset         = transport->offset_;
  auto bytes_available = bytes_transferred + offset;
  transport->expected_packet_.insert(transport->expected_packet_.end(), transport->buffer_ + bytes_to_strip,
                                     transport->buffer_ + (std::min)(bytes_expected, bytes_available));

  // set 'offset' to bytes of remain buffer
  offset = bytes_available - bytes_expected;
  if (offset >= 0)
  { /* pdu received properly */
    if (offset > 0)
    { /* move remain data to head of buffer and hold 'offset'. */
      ::memmove(transport->buffer_, transport->buffer_ + bytes_expected, offset);
      this->wait_duration_ = yasio__min_wait_duration;
    }
    // move properly pdu to ready queue, the other thread who care about will retrieve it.
    YASIO_KLOGV("[index: %d] received a properly packet from peer, packet size:%d", transport->cindex(), transport->expected_size_);
    this->handle_event(cxx14::make_unique<io_event>(transport->cindex(), transport->fetch_packet(), transport));
  }
  else /* all buffer consumed, set 'offset' to ZERO, pdu incomplete, continue recv remain data. */
    offset = 0;
}
highp_timer_ptr io_service::schedule(const std::chrono::microseconds& duration, timer_cb_t cb)
{
  auto timer = std::make_shared<highp_timer>();
  timer->expires_from_now(duration);
  /*!important, hold on `timer` by lambda expression */
#if YASIO__HAS_CXX14
  timer->async_wait(*this, [timer, cb = std::move(cb)](io_service& service) { return cb(service); });
#else
  timer->async_wait(*this, [timer, cb](io_service& service) { return cb(service); });
#endif
  return timer;
}
void io_service::schedule_timer(highp_timer* timer_ctl, timer_cb_t&& timer_cb)
{
  if (timer_ctl == nullptr)
    return;

  std::lock_guard<std::recursive_mutex> lck(this->timer_queue_mtx_);
  auto timer_it = this->find_timer(timer_ctl);
  if (timer_it == timer_queue_.end())
  {
    this->timer_queue_.emplace_back(timer_ctl, std::move(timer_cb));
    this->sort_timers();
    // If the new timer is earliest, wakup
    if (timer_ctl == this->timer_queue_.rbegin()->first)
      this->interrupt();
  }
  else // always replace timer_cb
    timer_it->second = std::move(timer_cb);
}
void io_service::remove_timer(highp_timer* timer)
{
  std::lock_guard<std::recursive_mutex> lck(this->timer_queue_mtx_);
  auto iter = this->find_timer(timer);
  if (iter != timer_queue_.end())
  {
    timer_queue_.erase(iter);
    if (!timer_queue_.empty())
    {
      this->sort_timers();
      this->interrupt();
    }
  }
}
void io_service::open_internal(io_channel* ctx)
{
  if (ctx->state_ == io_base::state::OPENING)
  { // in-opening, do nothing
    YASIO_KLOGD("[index: %d] the channel is in opening!", ctx->index_);
    return;
  }

  yasio__clearbits(ctx->opmask_, YOPM_CLOSE);
  yasio__setbits(ctx->opmask_, YOPM_OPEN);

  ++ctx->connect_id_;

  this->channel_ops_mtx_.lock();
  if (yasio__find(this->channel_ops_, ctx) == this->channel_ops_.end())
    this->channel_ops_.push_back(ctx);
  this->channel_ops_mtx_.unlock();

  this->interrupt();
}
bool io_service::shutdown_internal(transport_handle_t transport)
{
  if (transport->error_ == 0)
    transport->error_ = yasio::errc::shutdown_by_localhost;
  if (yasio__testbits(transport->ctx_->properties_, YCM_TCP))
    return transport->socket_->shutdown() == 0;
  return false;
}
bool io_service::close_internal(io_channel* ctx)
{
  yasio__clearbits(ctx->opmask_, YOPM_OPEN);
  if (ctx->socket_->is_open())
  {
    yasio__setbits(ctx->opmask_, YOPM_CLOSE);
    return true;
  }
  return false;
}
void io_service::process_timers()
{
  if (this->timer_queue_.empty())
    return;

  std::lock_guard<std::recursive_mutex> lck(this->timer_queue_mtx_);

  unsigned int n = 0; // the count expired loop timers
  while (!this->timer_queue_.empty())
  {
    auto timer_ctl = timer_queue_.back().first;
    if (timer_ctl->expired())
    {
      // fetch timer
      auto timer_impl = std::move(timer_queue_.back());
      timer_queue_.pop_back();

      if (!timer_impl.second(*this))
      { // reschedule if the timer want wait again
        timer_ctl->expires_from_now();
        timer_queue_.push_back(std::move(timer_impl));
        ++n;
      }
    }
    else
      break;
  }
  if (n)
    sort_timers();
}
int io_service::do_select(fd_set* fdsa, highp_time_t wait_duration)
{
  ::memcpy(fdsa, this->fds_array_, sizeof(this->fds_array_));
  timeval waitd_tv = {(decltype(timeval::tv_sec))(wait_duration / 1000000), (decltype(timeval::tv_usec))(wait_duration % 1000000)};
#if defined(YASIO_HAVE_CARES)
  int nfds = -1;
  if (this->ares_outstanding_work_ > 0 && (nfds = ::ares_fds(this->ares_, &fdsa[read_op], &fdsa[write_op])) > 0)
  {
    ::ares_timeout(this->ares_, &waitd_tv, &waitd_tv);
    if (this->max_nfds_ < nfds)
      this->max_nfds_ = nfds;
  }
#endif
  YASIO_KLOGV("[core] socket.select max_nfds_:%d waiting... %ld milliseconds", max_nfds_, waitd_tv.tv_sec * 1000 + waitd_tv.tv_usec / 1000);
  int retval = ::select(this->max_nfds_, &(fdsa[read_op]), &(fdsa[write_op]), nullptr, &waitd_tv);
  YASIO_KLOGV("[core] socket.select waked up, retval=%d", retval);

  return retval;
}
highp_time_t io_service::get_timeout(highp_time_t usec)
{
  if (this->timer_queue_.empty())
    return usec;

  std::lock_guard<std::recursive_mutex> lck(this->timer_queue_mtx_);
  auto earliest = timer_queue_.back().first;

  // microseconds
  auto duration = earliest->wait_duration();
  if (std::chrono::microseconds(usec) > duration)
    return duration.count();
  else
    return usec;
}
bool io_service::cleanup_channel(io_channel* ctx, bool clear_state)
{
  bool bret = cleanup_io(ctx, clear_state);
#if defined(YAISO_ENABLE_PASSIVE_EVENT)
  if (bret && yasio__testbits(ctx->properties_, YCM_SERVER))
    handle_event(cxx14::make_unique<io_event>(ctx->index_, YEK_ON_CLOSE, 0, ctx, 1));
#endif
  return bret;
}
bool io_service::cleanup_io(io_base* obj, bool clear_state)
{
  obj->error_  = 0;
  obj->opmask_ = 0;
  if (clear_state)
    obj->state_ = io_base::state::CLOSED;
  if (obj->socket_->is_open())
  {
    unregister_descriptor(obj->socket_->native_handle(), YEM_POLLIN | YEM_POLLOUT);
    obj->socket_->close();
    return true;
  }
  return false;
}
u_short io_service::query_ares_state(io_channel* ctx)
{
  if (yasio__testbits(ctx->properties_, YCPF_NEEDS_QUERIES))
  {
    switch (static_cast<u_short>(ctx->dns_queries_state_))
    {
      case YDQS_INPROGRESS:
        break;
      case YDQS_READY:
        if ((highp_clock() - ctx->dns_queries_timestamp_) < options_.dns_cache_timeout_)
          break;
        // dns cache timeout, change state to dirty and start resolve
        ctx->dns_queries_state_ = YDQS_DIRTY;
      case YDQS_DIRTY:
        start_resolve(ctx);
        break;
    }
  }
  return ctx->dns_queries_state_;
}
void io_service::start_resolve(io_channel* ctx)
{ // Only call at event-loop thread, so
  // no need to consider thread safe.
  assert(ctx->dns_queries_state_ == YDQS_DIRTY);
  ctx->set_last_errno(EINPROGRESS);
  ctx->dns_queries_state_ = YDQS_INPROGRESS;
  YASIO_KLOGD("[index: %d] resolving %s", ctx->index_, ctx->remote_host_.c_str());
  ctx->remote_eps_.clear();
#if defined(YASIO_ENABLE_ARES_PROFILER)
  ctx->ares_start_time_ = highp_clock();
#endif
#if !defined(YASIO_HAVE_CARES)
  // init async resolve thread state
  std::string resolving_host                    = ctx->remote_host_;
  u_short resolving_port                        = ctx->remote_port_;
  std::weak_ptr<cxx17::shared_mutex> weak_mutex = life_mutex_;
  std::weak_ptr<life_token> life_token          = life_token_;
  std::thread async_resolv_thread([=] {
    // check life token
    if (life_token.use_count() < 1)
      return;

    // preform blocking resolving safe
    std::vector<ip::endpoint> remote_eps;
    int error = options_.resolv_(remote_eps, resolving_host.c_str(), resolving_port);

    // lock perform update dns state of the channel
    auto pmtx = weak_mutex.lock();
    if (!pmtx)
      return;
    cxx17::shared_lock<cxx17::shared_mutex> lck(*pmtx);

    // check life token again, when io_service cleanup done, life_token's use_count will be 0,
    // otherwise, we can safe to do follow assignments.
    if (life_token.use_count() < 1)
      return;
    if (error == 0)
    {
      ctx->dns_queries_state_     = YDQS_READY;
      ctx->remote_eps_            = std::move(remote_eps);
      ctx->dns_queries_timestamp_ = highp_clock();
#  if defined(YASIO_ENABLE_ARES_PROFILER)
      YASIO_KLOGD("[index: %d] resolve %s succeed, cost: %g(ms)", ctx->index_, ctx->remote_host_.c_str(),
                  (ctx->dns_queries_timestamp_ - ctx->ares_start_time_) / 1000.0);
#  endif
    }
    else
    {
      ctx->dns_queries_state_ = YDQS_FAILED;
      YASIO_KLOGE("[index: %d] resolve %s failed, ec=%d, detail:%s", ctx->index_, ctx->remote_host_.c_str(), error, xxsocket::gai_strerror(error));
    }
    this->interrupt();
  });
  async_resolv_thread.detach();
#else
  if (this->options_.dns_dirty_)
    recreate_ares_channel();

  ares_addrinfo_hints hint;
  memset(&hint, 0x0, sizeof(hint));
  hint.ai_family = local_address_family();
  char sport[sizeof "65535"] = {'\0'};
  const char* service = nullptr;
  if (ctx->remote_port_ > 0)
  {
    sprintf(sport, "%u", ctx->remote_port_); // It's enough for unsigned short, so use sprintf ok.
    service = sport;
  }
  ares_work_started();
  ::ares_getaddrinfo(this->ares_, ctx->remote_host_.c_str(), service, &hint, io_service::ares_getaddrinfo_cb, ctx);
#endif
}
int io_service::resolve(std::vector<ip::endpoint>& endpoints, const char* hostname, unsigned short port)
{
  if (yasio__testbits(this->ipsv_, ipsv_ipv4))
    return xxsocket::resolve_v4(endpoints, hostname, port);
  else if (yasio__testbits(this->ipsv_, ipsv_ipv6)) // localhost is IPv6_only network
    return xxsocket::resolve_v6(endpoints, hostname, port) != 0 ? xxsocket::resolve_v4to6(endpoints, hostname, port) : 0;
  return -1;
}
void io_service::interrupt() { interrupter_.interrupt(); }
const char* io_service::strerror(int error)
{
  switch (error)
  {
    case 0:
      return "No error.";
    case yasio::errc::resolve_host_failed:
      return "Resolve host failed!";
    case yasio::errc::no_available_address:
      return "No available address!";
    case yasio::errc::shutdown_by_localhost:
      return "An existing connection was shutdown by local host!";
    case yasio::errc::invalid_packet:
      return "Invalid packet!";
    case yasio::errc::ssl_handshake_failed:
      return "SSL handshake failed!";
    case yasio::errc::ssl_write_failed:
      return "SSL write failed!";
    case yasio::errc::ssl_read_failed:
      return "SSL read failed!";
    case yasio::errc::eof:
      return "End of file.";
    case -1:
      return "Unknown error!";
    default:
      return xxsocket::strerror(error);
  }
}
void io_service::set_option(int opt, ...)
{
  va_list ap;
  va_start(ap, opt);
  set_option_internal(opt, ap);
  va_end(ap);
}
void io_service::set_option_internal(int opt, va_list ap) // lgtm [cpp/poorly-documented-function]
{
  switch (opt)
  {
    case YOPT_S_DEFERRED_EVENT:
      options_.deferred_event_ = !!va_arg(ap, int);
      break;
    case YOPT_S_TCP_KEEPALIVE:
      options_.tcp_keepalive_.onoff    = 1;
      options_.tcp_keepalive_.idle     = va_arg(ap, int);
      options_.tcp_keepalive_.interval = va_arg(ap, int);
      options_.tcp_keepalive_.probs    = va_arg(ap, int);
      break;
    case YOPT_S_RESOLV_FN:
      options_.resolv_ = *va_arg(ap, resolv_fn_t*);
      break;
    case YOPT_S_PRINT_FN: {
      auto ncb = *va_arg(ap, print_fn_t*);
      if (ncb)
        options_.print_ = [=](int, const char* msg) { ncb(msg); };
      else
        options_.print_ = nullptr;
    }
    break;
    case YOPT_S_PRINT_FN2:
      options_.print_ = *va_arg(ap, print_fn2_t*);
      break;
    case YOPT_S_NO_NEW_THREAD:
      this->options_.no_new_thread_ = !!va_arg(ap, int);
      break;
#if defined(YASIO_SSL_BACKEND)
    case YOPT_S_SSL_CACERT:
      this->options_.cafile_ = va_arg(ap, const char*);
      break;
#endif
    case YOPT_S_CONNECT_TIMEOUT:
      options_.connect_timeout_ = static_cast<highp_time_t>(va_arg(ap, int)) * std::micro::den;
      break;
    case YOPT_S_DNS_CACHE_TIMEOUT:
      options_.dns_cache_timeout_ = static_cast<highp_time_t>(va_arg(ap, int)) * std::micro::den;
      break;
    case YOPT_S_DNS_QUERIES_TIMEOUT:
    case YOPT_S_DNS_QUERIES_TIMEOUTMS:
      options_.dns_queries_timeout_ = static_cast<highp_time_t>(va_arg(ap, int)) * std::milli::den;
      break;
    case YOPT_S_DNS_QUERIES_TRIES:
      options_.dns_queries_tries_ = va_arg(ap, int);
      break;
    case YOPT_S_DNS_DIRTY:
      options_.dns_dirty_ = true;
      break;
    case YOPT_C_UNPACK_PARAMS: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
      {
        channel->uparams_.max_frame_length    = va_arg(ap, int);
        channel->uparams_.length_field_offset = va_arg(ap, int);
        channel->uparams_.length_field_length = yasio::clamp(va_arg(ap, int), YASIO_SSIZEOF(int8_t), YASIO_SSIZEOF(int));
        channel->uparams_.length_adjustment   = va_arg(ap, int);
      }
      break;
    }
    case YOPT_C_UNPACK_STRIP: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->uparams_.initial_bytes_to_strip = yasio::clamp(va_arg(ap, int), 0, YASIO_UNPACK_MAX_STRIP);
      break;
    }
    case YOPT_S_EVENT_CB:
      options_.on_event_ = *va_arg(ap, event_cb_t*);
      break;
    case YOPT_S_DEFER_EVENT_CB:
      options_.on_defer_event_ = *va_arg(ap, defer_event_cb_t*);
      break;
    case YOPT_C_LFBFD_FN: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->decode_len_ = *va_arg(ap, decode_len_fn_t*);
      break;
    }
    case YOPT_C_LOCAL_HOST: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->local_host_ = va_arg(ap, const char*);
      break;
    }
    case YOPT_C_LOCAL_PORT: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->local_port_ = (u_short)va_arg(ap, int);
      break;
    }
    case YOPT_C_LOCAL_ENDPOINT: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
      {
        channel->local_host_ = va_arg(ap, const char*);
        channel->local_port_ = ((u_short)va_arg(ap, int));
      }
      break;
    }
    case YOPT_C_REMOTE_HOST: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->set_host(va_arg(ap, const char*));
      break;
    }
    case YOPT_C_REMOTE_PORT: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->set_port((u_short)va_arg(ap, int));
      break;
    }
    case YOPT_C_REMOTE_ENDPOINT: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
      {
        channel->set_host(va_arg(ap, const char*));
        channel->set_port((u_short)va_arg(ap, int));
      }
      break;
    }
    case YOPT_C_ENABLE_MCAST: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
      {
        const char* addr = va_arg(ap, const char*);
        int loopback     = va_arg(ap, int);
        channel->enable_multicast_group(ip::endpoint(addr, 0), loopback);
        if (channel->socket_->is_open())
        { // client join directly
          channel->join_multicast_group();
        }
      }
      break;
    }
    case YOPT_C_DISABLE_MCAST: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->disable_multicast_group();
      break;
    }
    case YOPT_C_MOD_FLAGS: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
      {
        yasio__setbits(channel->properties_, (uint32_t)va_arg(ap, int));
        yasio__clearbits(channel->properties_, (uint32_t)va_arg(ap, int));
      }
      break;
    }
#if defined(YASIO_HAVE_KCP)
    case YOPT_C_KCP_CONV: {
      auto channel = channel_at(static_cast<size_t>(va_arg(ap, int)));
      if (channel)
        channel->kcp_conv_ = va_arg(ap, int);
      break;
    }
#endif
    case YOPT_T_CONNECT: {
      auto transport = va_arg(ap, transport_handle_t);
      if (transport && transport->is_open() && (transport->ctx_->properties_ & 0xff) == YCK_UDP_CLIENT)
        static_cast<io_transport_udp*>(transport)->connect();
      break;
    }
    case YOPT_T_DISCONNECT: {
      auto transport = va_arg(ap, transport_handle_t);
      if (transport && transport->is_open() && (transport->ctx_->properties_ & 0xff) == YCK_UDP_CLIENT)
        static_cast<io_transport_udp*>(transport)->disconnect();
      break;
    }
    case YOPT_B_SOCKOPT: {
      auto obj = va_arg(ap, io_base*);
      if (obj && obj->socket_ && obj->socket_->is_open())
      {
        auto optlevel = va_arg(ap, int);
        auto optname  = va_arg(ap, int);
        auto optval   = va_arg(ap, void*);
        auto optlen   = va_arg(ap, int);
        obj->socket_->set_optval(optlevel, optname, optval, optlen);
      }
      break;
    }
  }
}
} // namespace inet
} // namespace yasio

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
