//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

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
** yasio_ni v2: The yasio native interface for interop, match OpenNSM2(The Unity C# wrapper of yasio)
*/
#include <array>
#include <string.h>
#include "yasio/yasio.hpp"
#include "yasio/obstream.hpp"

#if defined(_WINDLL)
#  define YASIO_NI_API __declspec(dllexport)
#else
#  define YASIO_NI_API
#endif

#define YASIO_MAX_OPTION_ARGC 5

using namespace yasio;
using namespace yasio::inet;

namespace
{
template <typename _CStr, typename _Fn> inline void fast_split(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim, _Fn func)
{
  auto _Start = s; // the start of every string
  auto _Ptr   = s; // source string iterator
  auto _End   = s + slen;
  while ((_Ptr = strchr(_Ptr, delim)))
  {
    if (_Start < _Ptr)
      if (func(_Start, _Ptr))
        return;
    _Start = _Ptr + 1;
    ++_Ptr;
  }
  if (_Start < _End)
  {
    func(_Start, _End);
  }
}
inline int svtoi(cxx17::string_view& sv) { return !sv.empty() ? atoi(sv.data()) : 0; }
inline const char* svtoa(cxx17::string_view& sv) { return !sv.empty() ? sv.data() : ""; }
} // namespace

extern "C" {

YASIO_NI_API void yasio_init_globals(void(YASIO_INTEROP_DECL* pfn)(int level, const char*))
{
  yasio::inet::print_fn2_t custom_print = pfn;
  io_service::init_globals(custom_print);
}
YASIO_NI_API void yasio_cleanup_globals() { io_service::cleanup_globals(); }
YASIO_NI_API void* yasio_create_service(int channel_count, void(YASIO_INTEROP_DECL* event_cb)(int kind, int status, int cidx, void* t, void* opaque))
{
  assert(!!event_cb);
  io_service* service = new io_service(channel_count);
  service->start([=](event_ptr e) {
    auto& pkt = e->packet();
    event_cb(e->kind(), e->status(), e->cindex(), e->transport(), !is_packet_empty(pkt) ? &pkt : nullptr);
  });
  return service;
}
YASIO_NI_API void* yasio_unwrap_ptr(void* opaque, int offset)
{
  auto& pkt = *(packet_t*)opaque;
  return packet_data(pkt) + offset;
}
YASIO_NI_API int yasio_unwrap_len(void* opaque, int offset)
{
  auto& pkt = *(packet_t*)opaque;
  return static_cast<int>(packet_len(pkt) - offset);
}
YASIO_NI_API void yasio_destroy_service(void* service_ptr)
{
  io_service* service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    service->stop();
    delete service;
  }
}
YASIO_NI_API void yasio_set_resolv_fn(void* service_ptr, int(YASIO_INTEROP_DECL* resolv)(const char* host, void* sbuf))
{
  io_service* service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    resolv_fn_t fn = [resolv](std::vector<ip::endpoint>& eps, const char* host, unsigned short port) {
      char buffer[128] = {0};
      int ret          = resolv(host, &buffer[0]);
      if (0 == ret)
      {
        eps.push_back(ip::endpoint(buffer, port));
      }
      return ret;
    };

    service->set_option(YOPT_S_RESOLV_FN, &fn);
  }
}
/*
  Because, unity c# Marshal call C language vardic paremeter function will crash,
  so we provide this API.
  @params:
    opt: the opt value
    pszArgs: split by ';'
  */
YASIO_NI_API void yasio_set_option(void* service_ptr, int opt, const char* pszArgs)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (!service)
    return;

  // process one arg
  switch (opt)
  {
    case YOPT_C_DISABLE_MCAST:
    case YOPT_S_CONNECT_TIMEOUT:
    case YOPT_S_DNS_CACHE_TIMEOUT:
    case YOPT_S_DNS_QUERIES_TIMEOUT:
    case YOPT_S_DNS_DIRTY:
      service->set_option(opt, atoi(pszArgs));
      return;
  }

  // split args
  std::string strArgs = pszArgs;
  std::array<cxx17::string_view, YASIO_MAX_OPTION_ARGC> args;
  int argc = 0;
  fast_split(&strArgs.front(), strArgs.length(), ';', [&](char* s, char* e) {
    *e         = '\0'; // to c style string
    args[argc] = cxx17::string_view(s, e - s);
    return (++argc == YASIO_MAX_OPTION_ARGC);
  });

  switch (opt)
  {
    case YOPT_C_REMOTE_HOST:
    case YOPT_C_LOCAL_HOST:
      service->set_option(opt, svtoi(args[0]), svtoa(args[1]));
      break;
    case YOPT_C_LFBFD_IBTS:
    case YOPT_C_LOCAL_PORT:
    case YOPT_C_REMOTE_PORT:
    case YOPT_C_KCP_CONV:
      service->set_option(opt, svtoi(args[0]), svtoi(args[1]));
      break;
    case YOPT_C_ENABLE_MCAST:
    case YOPT_C_LOCAL_ENDPOINT:
    case YOPT_C_REMOTE_ENDPOINT:
      service->set_option(opt, svtoi(args[0]), svtoa(args[1]), svtoi(args[2]));
      break;
    case YOPT_C_MOD_FLAGS:
      service->set_option(opt, svtoi(args[0]), svtoi(args[1]), svtoi(args[2]));
      break;
    case YOPT_S_TCP_KEEPALIVE:
      service->set_option(opt, svtoi(args[0]), svtoi(args[1]), svtoi(args[2]), svtoi(args[3]));
      break;
    case YOPT_C_LFBFD_PARAMS:
      service->set_option(opt, svtoi(args[0]), svtoi(args[1]), svtoi(args[2]), svtoi(args[3]), svtoi(args[4]));
      break;
    default:
      YASIO_LOG("The option: %d unsupported by yasio_set_option!", opt);
  }
}
YASIO_NI_API void yasio_open(void* service_ptr, int cindex, int kind)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
    service->open(cindex, kind);
}
YASIO_NI_API void yasio_close(void* service_ptr, int cindex)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
    service->close(cindex);
}
YASIO_NI_API void yasio_close_handle(void* service_ptr, void* thandle)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
    service->close(reinterpret_cast<transport_handle_t>(thandle));
}
YASIO_NI_API int yasio_write(void* service_ptr, void* thandle, const unsigned char* bytes, int len)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    std::vector<char> buf(bytes, bytes + len);
    return service->write(reinterpret_cast<transport_handle_t>(thandle), std::move(buf));
  }
  return -1;
}
YASIO_NI_API int yasio_write_ob(void* service_ptr, void* thandle, void* obs_ptr)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    auto obs = reinterpret_cast<obstream*>(obs_ptr);
    return service->write(reinterpret_cast<transport_handle_t>(thandle), std::move(obs->buffer()));
  }
  return -1;
}
YASIO_NI_API unsigned int yasio_tcp_rtt(void* thandle)
{
  auto p = reinterpret_cast<transport_handle_t>(thandle);
  return io_service::tcp_rtt(p);
}
YASIO_NI_API void yasio_dispatch(void* service_ptr, int count)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
    service->dispatch(count);
}
YASIO_NI_API long long yasio_bytes_transferred(void* service_ptr, int cindex)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    auto channel = service->channel_at(cindex);
    if (channel)
      return channel->bytes_transferred();
  }
  return 0;
}
YASIO_NI_API unsigned int yasio_connect_id(void* service_ptr, int cindex)
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    auto channel = service->channel_at(cindex);
    if (channel)
      return channel->connect_id();
  }
  return 0;
}
YASIO_NI_API void yasio_set_print_fn(void* service_ptr, void(YASIO_INTEROP_DECL* pfn)(int, const char*))
{
  auto service = reinterpret_cast<io_service*>(service_ptr);
  if (service)
  {
    yasio::inet::print_fn2_t custom_print = pfn;
    service->set_option(YOPT_S_PRINT_FN2, &custom_print);
  }
}
YASIO_NI_API void* yasio_ob_new(int capacity) { return new obstream(capacity); }
YASIO_NI_API void yasio_ob_release(void* obs_ptr)
{
  auto obs = reinterpret_cast<obstream*>(obs_ptr);
  delete obs;
}
YASIO_NI_API void yasio_ob_write_short(void* obs_ptr, short value)
{
  auto obs = reinterpret_cast<obstream*>(obs_ptr);
  obs->write(value);
}
YASIO_NI_API void yasio_ob_write_int(void* obs_ptr, int value)
{
  auto obs = reinterpret_cast<obstream*>(obs_ptr);
  obs->write(value);
}
YASIO_NI_API void yasio_ob_write_bytes(void* obs_ptr, void* bytes, int len)
{
  auto obs = reinterpret_cast<obstream*>(obs_ptr);
  obs->write_bytes(bytes, len);
}

YASIO_NI_API long long yasio_highp_time(void) { return highp_clock<system_clock_t>(); }
YASIO_NI_API long long yasio_highp_clock(void) { return highp_clock<steady_clock_t>(); }
}
