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
#ifndef YASIO__CORE_HPP
#define YASIO__CORE_HPP

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#if defined(_WIN32)
#  include <map>
#endif
#include "yasio/detail/sz.hpp"
#include "yasio/detail/config.hpp"
#include "yasio/detail/object_pool.hpp"
#include "yasio/detail/singleton.hpp"
#include "yasio/detail/select_interrupter.hpp"
#include "yasio/detail/concurrent_queue.hpp"
#include "yasio/detail/utils.hpp"
#include "yasio/detail/errc.hpp"
#include "yasio/detail/byte_buffer.hpp"
#include "yasio/detail/fd_set_adapter.hpp"
#include "yasio/stl/memory.hpp"
#include "yasio/stl/string_view.hpp"
#include "yasio/xxsocket.hpp"

#if !defined(YASIO_HAVE_CARES)
#  include "yasio/stl/shared_mutex.hpp"
#endif

#if defined(YASIO_HAVE_KCP)
typedef struct IKCPCB ikcpcb;
#endif

#if defined(YASIO_SSL_BACKEND)
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_st SSL;
#endif

#if defined(YASIO_HAVE_CARES)
typedef struct ares_channeldata* ares_channel;
typedef struct ares_addrinfo ares_addrinfo;
#endif

#define yasio__find(cont, v) ::std::find(cont.begin(), cont.end(), v)
#define yasio__find_if(cont, pred) ::std::find_if(cont.begin(), cont.end(), pred)

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
// options
enum
{ // lgtm [cpp/irregular-enum-init]

  // Set whether deferred dispatch event.
  // params: deferred_event:int(1)
  YOPT_S_DEFERRED_EVENT = 1,

  // Set custom resolve function, native C++ ONLY.
  // params: func:resolv_fn_t*
  // remarks: you must ensure thread safe of it.
  YOPT_S_RESOLV_FN,

  // Set custom print function, native C++ ONLY.
  // parmas: func:print_fn_t
  // remarks: you must ensure thread safe of it.
  YOPT_S_PRINT_FN,

  // Set custom print function, native C++ ONLY.
  // parmas: func:print_fn2_t
  // remarks: you must ensure thread safe of it.
  YOPT_S_PRINT_FN2,

  // Set event callback
  // params: func:event_cb_t*
  // remarks: this callback will be invoke at io_service::dispatch caller thread
  YOPT_S_EVENT_CB,

  // Sets callback before defer dispatch event.
  // params: func:defer_event_cb_t*
  // remarks: this callback invoke at io_service thread
  YOPT_S_DEFER_EVENT_CB,

  // Set tcp keepalive in seconds, probes is tries.
  // params: idle:int(7200), interal:int(75), probes:int(10)
  YOPT_S_TCP_KEEPALIVE,

  // Don't start a new thread to run event loop
  // params: value:int(0)
  YOPT_S_NO_NEW_THREAD,

  // Sets ssl verification cert, if empty, don't verify
  // params: path:const char*
  YOPT_S_SSL_CACERT,

  // Set connect timeout in seconds
  // params: connect_timeout:int(10)
  YOPT_S_CONNECT_TIMEOUT,

  // Set connect timeout in milliseconds
  // params: connect_timeout : int(10000),
  YOPT_S_CONNECT_TIMEOUTMS,

  // Set dns cache timeout in seconds
  // params: dns_cache_timeout : int(600),
  YOPT_S_DNS_CACHE_TIMEOUT,

  // Set dns cache timeout in milliseconds
  // params: dns_cache_timeout : int(600000),
  YOPT_S_DNS_CACHE_TIMEOUTMS,

  // Set dns queries timeout in seconds, default is: 5
  // params: dns_queries_timeout : int(5)
  // remarks:
  //         a. this option must be set before 'io_service::start'
  //         b. only works when have c-ares
  //         c. the timeout algorithm of c-ares is complicated, usually, by default, dns queries
  //         will failed with timeout after more than 75 seconds.
  //         d. for more detail, please see:
  //         https://c-ares.haxx.se/ares_init_options.html
  YOPT_S_DNS_QUERIES_TIMEOUT,

  // Set dns queries timeout in milliseconds, default is: 5000
  // see also: YOPT_S_DNS_QUERIES_TIMEOUT
  YOPT_S_DNS_QUERIES_TIMEOUTMS,

  // Set dns queries tries when timeout reached, default is: 4
  // params: dns_queries_tries : int(4)
  // remarks:
  //        a. this option must be set before 'io_service::start'
  //        b. relative option: YOPT_S_DNS_QUERIES_TIMEOUT
  YOPT_S_DNS_QUERIES_TRIES,

  // Set dns server dirty
  // params: reserved : int(1)
  // remarks: you should set this option after your device network changed
  YOPT_S_DNS_DIRTY,

  // Set custom dns servers
  // params: servers: const char*
  // remarks:
  //  a. IPv4 address is 8.8.8.8 or 8.8.8.8:53, the port is optional
  //  b. IPv6 addresses with ports require square brackets [fe80::1%lo0]:53
  YOPT_S_DNS_LIST,

  // Set whether forward event without GC alloc
  // params: forward: int(0)
  // reamrks:
  //   when forward event enabled, the option YOPT_S_DEFERRED_EVENT was ignored
  YOPT_S_FORWARD_EVENT,

  // Set ssl server cert and private key file
  // params:
  //   crtfile: const char*
  //   keyfile: const char*
  YOPT_S_SSL_CERT,

  // Sets channel length field based frame decode function, native C++ ONLY
  // params: index:int, func:decode_len_fn_t*
  YOPT_C_UNPACK_FN = 101,
  YOPT_C_LFBFD_FN  = YOPT_C_UNPACK_FN,

  // Sets channel length field based frame decode params
  // params:
  //     index:int,
  //     max_frame_length:int(10MBytes),
  //     length_field_offset:int(-1),
  //     length_field_length:int(4),
  //     length_adjustment:int(0),
  YOPT_C_UNPACK_PARAMS,
  YOPT_C_LFBFD_PARAMS = YOPT_C_UNPACK_PARAMS,

  // Sets channel length field based frame decode initial bytes to strip
  // params:
  //     index:int,
  //     initial_bytes_to_strip:int(0)
  YOPT_C_UNPACK_STRIP,
  YOPT_C_LFBFD_IBTS = YOPT_C_UNPACK_STRIP,

  // Sets channel remote host
  // params: index:int, ip:const char*
  YOPT_C_REMOTE_HOST,

  // Sets channel remote port
  // params: index:int, port:int
  YOPT_C_REMOTE_PORT,

  // Sets channel remote endpoint
  // params: index:int, ip:const char*, port:int
  YOPT_C_REMOTE_ENDPOINT,

  // Sets local host for client channel only
  // params: index:int, ip:const char*
  YOPT_C_LOCAL_HOST,

  // Sets local port for client channel only
  // params: index:int, port:int
  YOPT_C_LOCAL_PORT,

  // Sets local endpoint for client channel only
  // params: index:int, ip:const char*, port:int
  YOPT_C_LOCAL_ENDPOINT,

  // Mods channl flags
  // params: index:int, flagsToAdd:int, flagsToRemove:int
  YOPT_C_MOD_FLAGS,

  // Sets channel multicast interface, required on BSD-like system
  // params: index:int, multi_ifaddr:const char*
  // remarks:
  //   a. On BSD-like(APPLE, etc...) system: ipv6 addr must be "::1%lo0" or "::%en0"
  YOPT_C_MCAST_IF,

  // Enable channel multicast mode
  // params: index:int, multi_addr:const char*, loopback:int,
  // remarks:
  //   a. On BSD-like(APPLE, etc...) system: ipv6 addr must be: "ff02::1%lo0" or "ff02::1%en0"
  // refer to: https://www.tldp.org/HOWTO/Multicast-HOWTO-2.html
  YOPT_C_ENABLE_MCAST,

  // Disable channel multicast mode
  // params: index:int
  YOPT_C_DISABLE_MCAST,

  // The kcp conv id, must equal in two endpoint from the same connection
  // params: index:int, conv:int
  YOPT_C_KCP_CONV,

  // Whether never perform bswap for length field
  // params: index:int, no_bswap:int(0)
  YOPT_C_UNPACK_NO_BSWAP,

  // Change 4-tuple association for io_transport_udp
  // params: transport:transport_handle_t
  // remarks: only works for udp client transport
  YOPT_T_CONNECT,

  // Dissolve 4-tuple association for io_transport_udp
  // params: transport:transport_handle_t
  // remarks: only works for udp client transport
  YOPT_T_DISCONNECT,

  // Sets io_base sockopt
  // params: io_base*,level:int,optname:int,optval:int,optlen:int
  YOPT_B_SOCKOPT = 201,
};

// channel masks: only for internal use, not for user
enum
{
  YCM_CLIENT = 1,
  YCM_SERVER = 1 << 1,
  YCM_TCP    = 1 << 2,
  YCM_UDP    = 1 << 3,
  YCM_KCP    = 1 << 4,
  YCM_SSL    = 1 << 5,
  YCM_UDS    = 1 << 6, // IPC: posix domain socket
};

// channel kinds: for user to call io_service::open
enum
{
  YCK_TCP_CLIENT = YCM_TCP | YCM_CLIENT,
  YCK_TCP_SERVER = YCM_TCP | YCM_SERVER,
  YCK_UDP_CLIENT = YCM_UDP | YCM_CLIENT,
  YCK_UDP_SERVER = YCM_UDP | YCM_SERVER,
  YCK_KCP_CLIENT = YCM_KCP | YCM_CLIENT | YCM_UDP,
  YCK_KCP_SERVER = YCM_KCP | YCM_SERVER | YCM_UDP,
  YCK_SSL_CLIENT = YCK_TCP_CLIENT | YCM_SSL,
  YCK_SSL_SERVER = YCK_TCP_SERVER | YCM_SSL,
};

// channel flags
enum
{
  /* Whether setsockopt SO_REUSEADDR and SO_REUSEPORT */
  YCF_REUSEADDR = 1 << 9,

  /* For winsock security issue, see:
     https://docs.microsoft.com/en-us/windows/win32/winsock/using-so-reuseaddr-and-so-exclusiveaddruse
  */
  YCF_EXCLUSIVEADDRUSE = 1 << 10,
};

// event kinds
enum
{
  YEK_ON_OPEN = 1,
  YEK_ON_CLOSE,
  YEK_ON_PACKET,
  YEK_CONNECT_RESPONSE = YEK_ON_OPEN,   // implicit deprecated alias
  YEK_CONNECTION_LOST  = YEK_ON_CLOSE,  // implicit deprecated alias
  YEK_PACKET           = YEK_ON_PACKET, // implicit deprecated alias
};

// the network core service log level
enum
{
  YLOG_V,
  YLOG_D,
  YLOG_I,
  YLOG_W,
  YLOG_E,
};

// class fwds
class highp_timer;
class io_send_op;
class io_sendto_op;
class io_event;
class io_channel;
class io_transport;
class io_transport_tcp; // tcp client/server
class io_transport_ssl; // ssl client
class io_transport_udp; // udp client/server
class io_transport_kcp; // kcp client/server
class io_service;

// recommand user always use transport_handle_t, in the future, it's maybe void* or intptr_t
typedef io_transport* transport_handle_t;

// typedefs
typedef std::shared_ptr<xxsocket> xxsocket_ptr;

typedef std::unique_ptr<io_send_op> send_op_ptr;
typedef std::unique_ptr<io_event> event_ptr;
typedef std::shared_ptr<highp_timer> highp_timer_ptr;

typedef std::function<bool(io_service&)> timer_cb_t;
typedef std::function<void(io_service&)> timerv_cb_t;
typedef std::function<void(event_ptr&&)> event_cb_t;
typedef std::function<bool(event_ptr&)> defer_event_cb_t;
typedef std::function<void(int, size_t)> completion_cb_t;
typedef std::function<int(void* d, int n)> decode_len_fn_t;
typedef std::function<int(std::vector<ip::endpoint>&, const char*, unsigned short)> resolv_fn_t;
typedef std::function<void(const char*)> print_fn_t;
typedef std::function<void(int level, const char*)> print_fn2_t;

typedef std::pair<highp_timer*, timer_cb_t> timer_impl_t;

// alias, for compatible purpose only
typedef highp_timer deadline_timer;
typedef highp_timer_ptr deadline_timer_ptr;
typedef event_cb_t io_event_cb_t;
typedef completion_cb_t io_completion_cb_t;

// the ssl role
enum ssl_role
{
  YSSL_CLIENT,
  YSSL_SERVER,
};

struct io_hostent {
  io_hostent() = default;
  io_hostent(cxx17::string_view ip, u_short port) : host_(cxx17::svtos(ip)), port_(port) {}
  io_hostent(io_hostent&& rhs) YASIO__NOEXCEPT : host_(std::move(rhs.host_)), port_(rhs.port_) {}
  io_hostent(const io_hostent& rhs) : host_(rhs.host_), port_(rhs.port_) {}
  void set_ip(cxx17::string_view ip) { cxx17::assign(host_, ip); }
  const std::string& get_ip() const { return host_; }
  void set_port(u_short port) { port_ = port; }
  u_short get_port() const { return port_; }
  std::string host_;
  u_short port_ = 0;
};

class YASIO_API highp_timer {
public:
  highp_timer()                   = default;
  highp_timer(const highp_timer&) = delete;
  highp_timer(highp_timer&&)      = delete;
  void expires_from_now(const std::chrono::microseconds& duration)
  {
    this->duration_    = duration;
    this->expire_time_ = steady_clock_t::now() + this->duration_;
  }

  void expires_from_now() { this->expire_time_ = steady_clock_t::now() + this->duration_; }

  // Wait timer timeout once.
  void async_wait_once(io_service& service, timerv_cb_t cb)
  {
#if YASIO__HAS_CXX14
    this->async_wait(service, [cb = std::move(cb)](io_service& service) {
#else
    this->async_wait(service, [cb](io_service& service) {
#endif
      cb(service);
      return true;
    });
  }

  // Wait timer timeout
  // @retval of timer_cb_t:
  //        true: wait once
  //        false: wait again after expired
  YASIO__DECL void async_wait(io_service& service, timer_cb_t);

  // Cancel the timer
  YASIO__DECL void cancel(io_service& service);

  // Check if timer is expired?
  bool expired() const { return wait_duration().count() <= 0; }

  // Gets wait duration of timer.
  std::chrono::microseconds wait_duration() const { return std::chrono::duration_cast<std::chrono::microseconds>(this->expire_time_ - steady_clock_t::now()); }

  std::chrono::microseconds duration_                  = {};
  std::chrono::time_point<steady_clock_t> expire_time_ = {};
};

struct YASIO_API io_base {
  enum class state : uint8_t
  {
    CLOSED,
    RESOLVING,  // for client only
    CONNECTING, // for client only
    OPENED,
  };
  enum class error_stage : uint8_t
  {
    NONE,
    READ,
    WRITE,
    OPEN_SOCKET,
    BIND_SOCKET,
    LISTEN_SOCKET
  };
  io_base() : error_(0), state_(state::CLOSED), opmask_(0)
  {
    static unsigned int s_object_id = 0;
    this->id_                       = ++s_object_id;
  }
  virtual ~io_base() {}

  void set_last_errno(int error, error_stage stage = error_stage::NONE)
  {
    error_       = error;
    error_stage_ = stage;
  }

#if !defined(YASIO_MINIFY_EVENT)
  // usedata
  union {
    void* ptr;
    int ival;
  } ud_{};
#endif

  xxsocket_ptr socket_;
  int error_; // socket error(>= -1), application error(< -1)
  // 0: none, 1: read, 2: write
  error_stage error_stage_ = error_stage::NONE;

  // mark whether pollout event registerred.
  bool pollout_registerred_ = false;
  std::atomic<state> state_;
  uint8_t opmask_;
  unsigned int id_;

public:
  unsigned int id() const { return id_; }
};

class YASIO_API io_channel : public io_base {
  friend class io_service;
  friend class io_transport;
  friend class io_transport_tcp;
  friend class io_transport_ssl;
  friend class io_transport_udp;
  friend class io_transport_kcp;

public:
  io_service& get_service() const { return service_; }
#if defined(YASIO_SSL_BACKEND)
  YASIO__DECL SSL_CTX* get_ssl_context(bool client) const;
#endif
  int index() const { return index_; }
  u_short remote_port() const { return remote_port_; }
  YASIO__DECL std::string format_destination() const;

  long long bytes_transferred() const { return bytes_transferred_; }
  unsigned int connect_id() const { return connect_id_; }

#if !defined(YASIO_NO_USER_TIMER)
  highp_timer& get_user_timer() { return this->user_timer_; }
#endif
protected:
  YASIO__DECL void enable_multicast(const char* addr, int loopback);
  YASIO__DECL void disable_multicast();
  YASIO__DECL void join_multicast_group();
  YASIO__DECL int configure_multicast_group(bool onoff);
  // For log macro only
  YASIO__DECL const print_fn2_t& __get_cprint() const;

private:
  YASIO__DECL io_channel(io_service& service, int index);

  void set_address(cxx17::string_view host, u_short port)
  {
    set_host(host);
    set_port(port);
  }

  YASIO__DECL void set_host(cxx17::string_view host);
  YASIO__DECL void set_port(u_short port);

  void clear_mutable_flags() { properties_ &= 0x00ffffff; }

  // -1 indicate failed, connection will be closed
  YASIO__DECL int __builtin_decode_len(void* d, int n);

  io_service& service_;

  /* Since v3.33.0 mask,kind,flags,private_flags are stored to this field
  ** bit[1-8] mask & kinds
  ** bit[9-16] flags
  ** bit[17-24] byte1 of private flags
  ** bit[25~32] byte2 of private flags (mutable)
  */
  uint32_t properties_ = 0;

  /*
  ** !!! tcp/udp client only, if not zero, will use it as fixed port,
  ** !!! otherwise system will generate a random port for local socket.
  */
  u_short local_port_ = 0;

  /*
  ** !!! tcp/udp client, the port to connect
  ** !!! tcp/udp server, the port to listening
  */
  u_short remote_port_ = 0;

  // The last query success time in microseconds for dns cache support
  highp_time_t query_success_time_ = 0;

#if defined(YASIO_ENABLE_ARES_PROFILER)
  highp_time_t query_start_time_;
#endif

  int index_;
  int socktype_ = 0;

  // The timer for check resolve & connect timeout
  highp_timer timer_;

#if !defined(YASIO_NO_USER_TIMER)
  // The timer for user
  highp_timer user_timer_;
#endif
  // The stream mode application protocol (based on tcp/udp/kcp) unpack params
  struct __unnamed01 {
    int max_frame_length       = YASIO_SZ(10, M); // 10MBytes
    int length_field_offset    = -1;              // -1: directly, >= 0: store as 1~4bytes integer
    int length_field_length    = 4;               // 1,2,3,4
    int length_adjustment      = 0;
    int initial_bytes_to_strip = 0;
    int no_bswap               = 0;
  } uparams_;
  decode_len_fn_t decode_len_;

  /*
  !!! for tcp/udp client to bind local specific network adapter, empty for any
  */
  std::string local_host_;

  /*
  !!! for tcp/udp client to connect remote host.
  !!! for tcp/udp server to bind local specific network adapter, empty for any
  !!! for multicast, it's used as multicast address,
      doesn't connect even through recvfrom on packet from remote
  */
  std::string remote_host_;
  std::vector<ip::endpoint> remote_eps_;

  ip::endpoint multiaddr_, multiif_;

  // Current it's only for UDP
  sbyte_buffer buffer_;

  // The bytes transferred from socket low layer, currently, only works for client channel
  long long bytes_transferred_ = 0;

  unsigned int connect_id_ = 0;

#if defined(YASIO_HAVE_KCP)
  int kcp_conv_ = 0;
#endif
};

// for tcp transport only
class YASIO_API io_send_op {
public:
  io_send_op(sbyte_buffer&& buffer, completion_cb_t&& handler) : offset_(0), buffer_(std::move(buffer)), handler_(std::move(handler)) {}
  virtual ~io_send_op() {}

  size_t offset_;       // read pos from sending buffer
  sbyte_buffer buffer_; // sending data buffer
  completion_cb_t handler_;

  YASIO__DECL virtual int perform(transport_handle_t transport, const void* buf, int n, int& error);

#if !defined(YASIO_DISABLE_OBJECT_POOL)
  DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(io_send_op, 512)
#endif
};

// for udp transport only
class YASIO_API io_sendto_op : public io_send_op {
public:
  io_sendto_op(sbyte_buffer&& buffer, completion_cb_t&& handler, const ip::endpoint& destination)
      : io_send_op(std::move(buffer), std::move(handler)), destination_(destination)
  {}

  YASIO__DECL int perform(transport_handle_t transport, const void* buf, int n, int& error) override;
#if !defined(YASIO_DISABLE_OBJECT_POOL)
  DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(io_sendto_op, 512)
#endif
  ip::endpoint destination_;
};

class io_transport : public io_base {
  friend class io_service;
  friend class io_send_op;
  friend class io_sendto_op;
  friend class io_event;

  io_transport(const io_transport&) = delete;

public:
  int cindex() const { return ctx_->index_; }

  ip::endpoint local_endpoint() const { return socket_->local_endpoint(); }
  virtual ip::endpoint remote_endpoint() const { return socket_->peer_endpoint(); }

  io_channel* get_context() const { return ctx_; }

  virtual ~io_transport()
  {
    ctx_ = nullptr;
    send_queue_.clear();
  }

protected:
  io_service& get_service() const { return ctx_->get_service(); }
  bool is_open() const { return state_ == state::OPENED && socket_ && socket_->is_open(); }
  sbyte_buffer fetch_packet()
  {
    expected_size_ = -1;
    return std::move(expected_packet_);
  }

  // For log macro only
  YASIO__DECL const print_fn2_t& __get_cprint() const;

  // Call at user thread
  YASIO__DECL virtual int write(sbyte_buffer&&, completion_cb_t&&);

  // Call at user thread
  virtual int write_to(sbyte_buffer&&, const ip::endpoint&, completion_cb_t&&)
  {
    YASIO_LOG("[warning] io_transport doesn't support 'write_to' operation!");
    return 0;
  }

  YASIO__DECL int call_read(void* data, int size, int revent, int& error);
  YASIO__DECL int call_write(io_send_op*, int& error);
  YASIO__DECL void complete_op(io_send_op*, int error);

  // Call at io_service
  YASIO__DECL virtual int do_read(int revent, int& error, highp_time_t& wait_duration);

  // Call at io_service, try flush pending packet
  YASIO__DECL virtual bool do_write(highp_time_t& wait_duration);

  // Sets the underlying layer socket io primitives.
  YASIO__DECL virtual void set_primitives();

  YASIO__DECL io_transport(io_channel* ctx, xxsocket_ptr&& s);

  bool is_valid() const { return ctx_ != nullptr; }

  char buffer_[YASIO_INET_BUFFER_SIZE]; // recv buffer, 64K
  int offset_ = 0;                      // recv buffer offset

  int expected_size_ = -1;
  sbyte_buffer expected_packet_;

  io_channel* ctx_;

  std::function<int(const void*, int, const ip::endpoint*, int&)> write_cb_;
  std::function<int(void*, int, int, int&)> read_cb_;

  privacy::concurrent_queue<send_op_ptr> send_queue_;
};

class YASIO_API io_transport_tcp : public io_transport {
  friend class io_service;

public:
  io_transport_tcp(io_channel* ctx, xxsocket_ptr&& s);
};
#if defined(YASIO_SSL_BACKEND)
class io_transport_ssl : public io_transport_tcp {
public:
  YASIO__DECL io_transport_ssl(io_channel* ctx, xxsocket_ptr&& s);
  YASIO__DECL void set_primitives() override;

  YASIO__DECL void do_ssl_shutdown();

protected:
  YASIO__DECL int do_ssl_handshake(int& error); // always invoke at do_read
  SSL* ssl_ = nullptr;
};
#else
class io_transport_ssl {};
#endif
class YASIO_API io_transport_udp : public io_transport {
  friend class io_service;

public:
  YASIO__DECL io_transport_udp(io_channel* ctx, xxsocket_ptr&& s);
  YASIO__DECL ~io_transport_udp();

  YASIO__DECL ip::endpoint remote_endpoint() const override;

protected:
  YASIO__DECL void connect();
  YASIO__DECL void disconnect();

  YASIO__DECL int write(sbyte_buffer&&, completion_cb_t&&) override;
  YASIO__DECL int write_to(sbyte_buffer&&, const ip::endpoint&, completion_cb_t&&) override;

  YASIO__DECL void set_primitives() override;

  // ensure destination for sendto valid, if not, assign from ctx_->remote_eps_[0]
  YASIO__DECL const ip::endpoint& ensure_destination() const;

  // configure remote with specific endpoint
  YASIO__DECL void confgure_remote(const ip::endpoint& peer);

  // process received data from low level
  YASIO__DECL virtual int handle_input(const char* data, int bytes_transferred, int& error, highp_time_t& wait_duration);

  ip::endpoint peer_;                // for recv only, unstable
  mutable ip::endpoint destination_; // for sendto only, stable
  bool connected_ = false;
};
#if defined(YASIO_HAVE_KCP)
class io_transport_kcp : public io_transport_udp {
public:
  YASIO__DECL io_transport_kcp(io_channel* ctx, xxsocket_ptr&& s);
  YASIO__DECL ~io_transport_kcp();
  ikcpcb* internal_object() { return kcp_; }

protected:
  YASIO__DECL int write(sbyte_buffer&&, completion_cb_t&&) override;

  YASIO__DECL int do_read(int revent, int& error, highp_time_t& wait_duration) override;
  YASIO__DECL bool do_write(highp_time_t& wait_duration) override;

  YASIO__DECL int handle_input(const char* buf, int len, int& error, highp_time_t& wait_duration) override;

  YASIO__DECL void check_timeout(highp_time_t& wait_duration) const;

  sbyte_buffer rawbuf_; // the low level raw buffer
  ikcpcb* kcp_;
  std::recursive_mutex send_mtx_;
};
#else
class io_transport_kcp {};
#endif

using io_packet = sbyte_buffer;
#if !defined(YASIO_USE_SHARED_PACKET)
using packet_t = io_packet;
inline packet_t wrap_packet(io_packet& raw_packet) { return std::move(raw_packet); }
inline bool is_packet_empty(packet_t& pkt) { return pkt.empty(); }
inline io_packet& forward_packet(packet_t& pkt) { return pkt; }
inline io_packet&& forward_packet(packet_t&& pkt) { return std::move(pkt); }
inline io_packet::pointer packet_data(packet_t& pkt) { return pkt.data(); }
inline io_packet::size_type packet_len(packet_t& pkt) { return pkt.size(); }
#else
using packet_t = std::shared_ptr<io_packet>;
inline packet_t wrap_packet(io_packet& raw_packet) { return std::make_shared<io_packet>(std::move(raw_packet)); }
inline bool is_packet_empty(packet_t& pkt) { return !pkt; }
inline io_packet& forward_packet(packet_t& pkt) { return *pkt; }
inline io_packet&& forward_packet(packet_t&& pkt) { return std::move(*pkt); }
inline io_packet::pointer packet_data(packet_t& pkt) { return pkt->data(); }
inline io_packet::size_type packet_len(packet_t& pkt) { return pkt->size(); }
#endif

class io_packet_view {
public:
  io_packet_view() = default;
  io_packet_view(char* d, int n) : data_(d), size_(n) {}
  char* data() { return this->data_; }
  const char* data() const { return this->data_; }
  size_t size() const { return this->size_; }

private:
  char* data_  = nullptr;
  size_t size_ = 0;
};

/*
 * Notes: store some properties of event source to make sure user can safe get them deferred
 */
class io_event final {
public:
  io_event(int cidx, int kind, int status, io_channel* source /*not nullable*/, int passive = 0)
      : kind_(kind), writable_(0), passive_(passive), status_(status), cindex_(cidx), source_id_(source->id_), source_(source)
  {
#if !defined(YASIO_MINIFY_EVENT)
    source_ud_ = source_->ud_.ptr;
#endif
  }
  io_event(int cidx, int kind, int status, io_transport* source /*not nullable*/)
      : kind_(kind), writable_(1), passive_(0), status_(status), cindex_(cidx), source_id_(source->id_), source_(source)
  {
#if !defined(YASIO_MINIFY_EVENT)
    source_ud_ = source_->ud_.ptr;
#endif
  }
  io_event(int cidx, io_packet&& pkt, io_transport* source /*not nullable*/)
      : kind_(YEK_ON_PACKET), writable_(1), passive_(0), status_(0), cindex_(cidx), source_id_(source->id_), source_(source), packet_(wrap_packet(pkt))
  {
#if !defined(YASIO_MINIFY_EVENT)
    source_ud_ = source_->ud_.ptr;
#endif
  }
  io_event(int cidx, io_packet_view pkt, io_transport* source /*not nullable*/)
      : kind_(YEK_ON_PACKET), writable_(1), passive_(0), status_(0), cindex_(cidx), source_id_(source->id_), source_(source), packet_view_(pkt)
  {
#if !defined(YASIO_MINIFY_EVENT)
    source_ud_ = source_->ud_.ptr;
#endif
  }
  io_event(const io_event&) = delete;
  io_event(io_event&& rhs)  = delete;
  ~io_event() {}

public:
  int cindex() const { return cindex_; }
  int status() const { return status_; }

  int kind() const { return kind_; }

  // whether the event triggered by server channel
  int passive() const { return passive_; }

  packet_t& packet() { return packet_; }

  io_packet_view packet_view() const { return packet_view_; }

  /*[nullable]*/ transport_handle_t transport() const { return writable_ ? static_cast<transport_handle_t>(source_) : nullptr; }

  io_base* source() const { return source_; }
  unsigned int source_id() const { return source_id_; }

#if !defined(YASIO_MINIFY_EVENT)
  /* Gets to transport user data when process this event */
  template <typename _Uty = void*>
  _Uty transport_ud() const
  {
    return (_Uty)(uintptr_t)source_ud_;
  }
  /* Sets trasnport user data when process this event */
  template <typename _Uty = void*>
  void transport_ud(_Uty uval)
  {
    source_ud_ = (void*)(uintptr_t)uval;

    auto t = this->transport();
    if (t)
      t->ud_.ptr = (void*)(uintptr_t)uval;
  }
  highp_time_t timestamp() const { return timestamp_; }
#endif
#if !defined(YASIO_DISABLE_OBJECT_POOL)
  DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(io_event, 128)
#endif
private:
  unsigned int kind_ : 30;
  unsigned int writable_ : 1;
  unsigned int passive_ : 1;

  int status_;

  int cindex_;
  unsigned int source_id_;

  io_base* source_;
  packet_t packet_;
  io_packet_view packet_view_;
#if !defined(YASIO_MINIFY_EVENT)
  void* source_ud_;
  highp_time_t timestamp_ = highp_clock();
#endif
};

class YASIO_API io_service // lgtm [cpp/class-many-fields]
{
  friend class highp_timer;
  friend class io_transport;
  friend class io_transport_tcp;
  friend class io_transport_udp;
  friend class io_transport_kcp;
#if defined(YASIO_SSL_BACKEND)
  friend class io_transport_ssl;
#endif
  friend class io_channel;

public:
  enum class state
  {
    UNINITIALIZED,
    IDLE,
    RUNNING,
    AT_EXITING,
  };

  /*
  ** summary: init global state with custom print function, you must ensure thread safe of it.
  ** remark:
  **   a. this function is not required, if you don't want print init log to custom console.
  **   b. this function only works once
  **   c. you should call once before call any 'io_servic::start'
  */
  YASIO__DECL static void init_globals(const yasio::inet::print_fn2_t&);
  YASIO__DECL static void cleanup_globals();

  // the additional API to get rtt of tcp transport
  YASIO__DECL static unsigned int tcp_rtt(transport_handle_t);

public:
  YASIO__DECL io_service();
  YASIO__DECL io_service(int channel_count);
  YASIO__DECL io_service(const io_hostent& channel_eps);
  YASIO__DECL io_service(const std::vector<io_hostent>& channel_eps);
  YASIO__DECL io_service(const io_hostent* channel_eps, int channel_count);
  YASIO__DECL ~io_service();

  YASIO__DECL void start(event_cb_t cb);
  /* summary: stop the io_service
  **
  ** remark:
  ** a. IF caller thread isn't service worker thread, the service state will be IDLE.
  ** b. IF caller thread is service worker thread, the service state will be STOPPING,
  **    then you needs to invoke this API again at other thread
  ** c. Strong recommend invoke this API at your event dispatch thread.
  */
  YASIO__DECL void stop();

  bool is_running() const { return this->state_ == io_service::state::RUNNING; }
  bool is_stopping() const { return !!this->stop_flag_; }

  // should call at the thread who care about async io
  // events(CONNECT_RESPONSE,CONNECTION_LOST,PACKET), such cocos2d-x opengl or
  // any other game engines' render thread.
  YASIO__DECL void dispatch(int max_count = 128);

  // set option, see enum YOPT_XXX
  YASIO__DECL void set_option(int opt, ...);
  YASIO__DECL void set_option_internal(int opt, va_list args);

  // open a channel, default: YCK_TCP_CLIENT
  YASIO__DECL bool open(size_t index, int kind = YCK_TCP_CLIENT);

  // check whether the channel is open
  YASIO__DECL bool is_open(int index) const;
  // check whether the transport is open
  YASIO__DECL bool is_open(transport_handle_t) const;

  // close transport
  YASIO__DECL void close(transport_handle_t);
  // close channel
  YASIO__DECL void close(int index);

  /*
  ** summary: Write data to a TCP or connected UDP transport with last peer address
  ** retval: < 0: failed
  ** params:
  **        'thandle': the transport to write, could be tcp/udp/kcp
  **        'buf': the data to write
  **        'len': the data len
  **        'handler': send finish callback, only works for TCP transport
  ** remark:
  **        + TCP/UDP: Use queue to store user message, flush at io_service thread
  **        + KCP: Use queue provided by kcp internal, flush at io_service thread
  */
  int write(transport_handle_t thandle, const void* buf, size_t len, completion_cb_t completion_handler = nullptr)
  {
    return write(thandle, sbyte_buffer{(const char*)buf, (const char*)buf + len, std::true_type{}}, std::move(completion_handler));
  }
  YASIO__DECL int write(transport_handle_t thandle, sbyte_buffer buffer, completion_cb_t completion_handler = nullptr);

  /*
   ** Summary: Write data to unconnected UDP transport with specified address.
   ** retval: < 0: failed
   ** remark: This function only for UDP like transport (UDP or KCP)
   **        + UDP: Use queue to store user message, flush at io_service thread
   **        + KCP: Use the queue provided by kcp internal, flush at io_service thread
   */
  int write_to(transport_handle_t thandle, const void* buf, size_t len, const ip::endpoint& to, completion_cb_t completion_handler = nullptr)
  {
    return write_to(thandle, sbyte_buffer{(const char*)buf, (const char*)buf + len, std::true_type{}}, to, std::move(completion_handler));
  }
  YASIO__DECL int write_to(transport_handle_t thandle, sbyte_buffer buffer, const ip::endpoint& to, completion_cb_t completion_handler = nullptr);

  // The highp_timer support, !important, the callback is called on the thread of io_service
  YASIO__DECL highp_timer_ptr schedule(const std::chrono::microseconds& duration, timer_cb_t);

  YASIO__DECL int resolve(std::vector<ip::endpoint>& endpoints, const char* hostname, unsigned short port = 0);

  // Gets channel by index
  YASIO__DECL io_channel* channel_at(size_t index) const;

private:
  YASIO__DECL void do_stop(uint8_t flags);
  YASIO__DECL void schedule_timer(highp_timer*, timer_cb_t&&);
  YASIO__DECL void remove_timer(highp_timer*);

  std::vector<timer_impl_t>::iterator find_timer(highp_timer* key)
  {
    return yasio__find_if(timer_queue_, [=](const timer_impl_t& timer) { return timer.first == key; });
  }
  void sort_timers()
  {
    std::sort(this->timer_queue_.begin(), this->timer_queue_.end(),
              [](const timer_impl_t& lhs, const timer_impl_t& rhs) { return lhs.first->wait_duration() > rhs.first->wait_duration(); });
  }

  // Start a async domain name query
  YASIO__DECL void start_query(io_channel*);

  YASIO__DECL void initialize(const io_hostent* channel_eps /* could be nullptr */, int channel_count);
  YASIO__DECL void finalize();

  // Try to dispose thread and other resources, service state will be IDLE when succeed
  YASIO__DECL void handle_stop();

  YASIO__DECL bool open_internal(io_channel*);

  YASIO__DECL void process_transports(fd_set_adapter& fd_set);
  YASIO__DECL void process_channels(fd_set_adapter& fd_set);
  YASIO__DECL void process_timers();

  YASIO__DECL void interrupt();

  YASIO__DECL highp_time_t get_timeout(highp_time_t usec);

  YASIO__DECL int do_resolve(io_channel* ctx);
  YASIO__DECL void do_connect(io_channel*);
  YASIO__DECL void do_connect_completion(io_channel*, fd_set_adapter& fd_set);

#if defined(YASIO_SSL_BACKEND)
  YASIO__DECL SSL_CTX* init_ssl_context(ssl_role role);
  YASIO__DECL void cleanup_ssl_context(ssl_role role);
#endif

#if defined(YASIO_HAVE_CARES)
  YASIO__DECL static void ares_getaddrinfo_cb(void* arg, int status, int timeouts, ares_addrinfo* answerlist);
  YASIO__DECL void ares_work_started();
  YASIO__DECL void ares_work_finished();
  YASIO__DECL int do_ares_fds(socket_native_type* socks, fd_set_adapter& fd_set, timeval& waitd_tv);
  YASIO__DECL void do_ares_process_fds(socket_native_type* socks, int count, fd_set_adapter& fd_set);
  YASIO__DECL void recreate_ares_channel();
  YASIO__DECL void config_ares_name_servers();
  YASIO__DECL void destroy_ares_channel();
#endif

  void handle_connect_succeed(io_channel* ctx, xxsocket_ptr s) { handle_connect_succeed(allocate_transport(ctx, std::move(s))); }
  YASIO__DECL void handle_connect_succeed(transport_handle_t);
  YASIO__DECL void handle_connect_failed(io_channel*, int ec);
  YASIO__DECL void active_transport(transport_handle_t);

  YASIO__DECL transport_handle_t allocate_transport(io_channel*, xxsocket_ptr&&);
  YASIO__DECL void deallocate_transport(transport_handle_t);

  YASIO__DECL void register_descriptor(const socket_native_type fd, int events);
  YASIO__DECL void deregister_descriptor(const socket_native_type fd, int events);

  // The major non-blocking event-loop
  YASIO__DECL void run(void);

  YASIO__DECL bool do_read(transport_handle_t, fd_set_adapter& fd_set);
  bool do_write(transport_handle_t transport) { return transport->do_write(this->wait_duration_); }
  YASIO__DECL void unpack(transport_handle_t, int bytes_expected, int bytes_transferred, int bytes_to_strip);

  YASIO__DECL bool cleanup_channel(io_channel* channel, bool clear_mask = true);
  YASIO__DECL bool cleanup_io(io_base* obj, bool clear_mask = true);

  YASIO__DECL void handle_close(transport_handle_t);

  template <typename... _Types>
  inline void fire_event(_Types&&... args)
  {
    auto event = cxx14::make_unique<io_event>(std::forward<_Types>(args)...);
    if (options_.deferred_event_ && !options_.forward_event_)
    {
      if (options_.on_defer_event_ && !options_.on_defer_event_(event))
        return;
      events_.emplace(std::move(event));
    }
    else
      options_.on_event_(std::move(event));
  }

  // new/delete client socket connection channel
  // please call this at initialization, don't new channel at runtime
  // dynmaically: because this API is not thread safe.
  YASIO__DECL void create_channels(const io_hostent* eps, int count);
  YASIO__DECL void destroy_channels(); // destroy all channels
  YASIO__DECL void clear_transports(); // clear all transports
  YASIO__DECL bool close_internal(io_channel*);

  // supporting server
  YASIO__DECL void do_accept(io_channel*);
  YASIO__DECL void do_accept_completion(io_channel*, fd_set_adapter& fd_set);

  YASIO__DECL static const char* strerror(int error);

  /*
  ** summary: For udp-server only, make dgram handle to communicate with client
  */
  YASIO__DECL transport_handle_t do_dgram_accept(io_channel*, const ip::endpoint& peer, int& error);

  int local_address_family() const { return ((ipsv_ & ipsv_ipv4) || !ipsv_) ? AF_INET : AF_INET6; }

  YASIO__DECL void update_dns_status();

  bool address_expired(io_channel* ctx) const { return (highp_clock() - ctx->query_success_time_) > options_.dns_cache_timeout_; }

  /* For log macro only */
  inline const print_fn2_t& __get_cprint() const { return options_.print_; }

private:
  state state_ = state::UNINITIALIZED; // The service state
  std::thread worker_;
  std::thread::id worker_id_;

  privacy::concurrent_queue<event_ptr, true> events_;

  std::vector<io_channel*> channels_;

  std::recursive_mutex channel_ops_mtx_;
  std::vector<io_channel*> channel_ops_;

  std::vector<transport_handle_t> transports_;
  std::vector<transport_handle_t> tpool_;

  // select interrupter
  select_interrupter interrupter_;

  // timer support timer_pair
  std::vector<timer_impl_t> timer_queue_;
  std::recursive_mutex timer_queue_mtx_;

  // the next wait duration for socket.select
  highp_time_t wait_duration_;

  fd_set_adapter fd_set_;

  // options
  struct __unnamed_options {
    highp_time_t connect_timeout_     = 10LL * std::micro::den;
    highp_time_t dns_cache_timeout_   = 600LL * std::micro::den;
    highp_time_t dns_queries_timeout_ = 5LL * std::micro::den;
    int dns_queries_tries_            = 5;

    bool dns_dirty_ = false;

    bool deferred_event_ = true;
    defer_event_cb_t on_defer_event_;

    bool forward_event_ = false; // since v3.39.7

    // tcp keepalive settings
    struct __unnamed01 {
      int onoff    = 0;
      int idle     = 7200;
      int interval = 75;
      int probs    = 10;
    } tcp_keepalive_;

    bool no_new_thread_ = false;

    // The resolve function
    resolv_fn_t resolv_;
    // the event callback
    event_cb_t on_event_;
    // The custom debug print function
    print_fn2_t print_;

#if defined(YASIO_SSL_BACKEND)
    // SSL client, the full path cacert(.pem) file for ssl verifaction
    std::string cafile_;

    // SSL server
    std::string crtfile_;
    std::string keyfile_;
#endif

#if defined(YASIO_HAVE_CARES)
    std::string name_servers_;
#endif
  } options_;

  // The ip stack version supported by localhost
  u_short ipsv_ = 0;
  // The stop flag to notify all transports needs close
  uint8_t stop_flag_ = 0;
#if defined(YASIO_SSL_BACKEND)
  SSL_CTX* ssl_roles_[2];
#endif
#if defined(YASIO_HAVE_CARES)
  ares_channel ares_         = nullptr; // the ares handle for non blocking io dns resolve support
  int ares_outstanding_work_ = 0;
#else
  // we need life_token + life_mutex
  struct life_token {};
  std::shared_ptr<life_token> life_token_;
  std::shared_ptr<cxx17::shared_mutex> life_mutex_;
#endif
}; // io_service

} // namespace inet
#if !YASIO__HAS_CXX11
using namespace yasio::inet;
#endif
} /* namespace yasio */

#define yasio_shared_service yasio::gc::singleton<yasio::inet::io_service>::instance

#if defined(YASIO_HEADER_ONLY)
#  include "yasio/yasio.cpp" // lgtm [cpp/include-non-header]
#endif

#endif
