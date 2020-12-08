//////////////////////////////////////////////////////////////////////////////////////////
// A cross platform socket APIs, support ios & android & wp8 & window store universal app
//
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

/*
** yasio_ni.cpp: The yasio native interface for interop.
*/
#include <array>
#include <string.h>
#include "yasio/yasio.hpp"

#if defined(_WINDLL)
#  define YASIO_NI_API __declspec(dllexport)
#else
#  define YASIO_NI_API
#endif

#if !defined(_WIN32) || YASIO__64BITS
#  define YASIO_INTEROP_DECL
#else
#  define YASIO_INTEROP_DECL __stdcall
#endif

#define YASIO_MAX_OPTION_ARGC 5

using namespace yasio;
using namespace yasio::inet;

namespace
{
template <typename _CStr, typename _Fn>
inline void fast_split(_CStr s, size_t slen, typename std::remove_pointer<_CStr>::type delim,
                       _Fn func)
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
YASIO_NI_API void yasio_cleanup_globals()
{
  io_service::cleanup_globals();
}
YASIO_NI_API void yasio_start(int channel_count,
                              void(YASIO_INTEROP_DECL* event_cb)(uint32_t emask, int cidx,
                                                                 intptr_t sid, intptr_t bytes,
                                                                 int len))
{
  yasio_shared_service(channel_count)->start([=](event_ptr e) {
    uint32_t emask = ((e->kind() << 16) & 0xffff0000) | (e->status() & 0xffff);
    event_cb(emask, e->cindex(), reinterpret_cast<intptr_t>(e->transport()),
             reinterpret_cast<intptr_t>(!e->packet().empty() ? e->packet().data() : nullptr),
             static_cast<int>(e->packet().size()));
  });
}
YASIO_NI_API void yasio_set_resolv_fn(int(YASIO_INTEROP_DECL* resolv)(const char* host,
                                                                      intptr_t sbuf))
{
  resolv_fn_t fn = [resolv](std::vector<ip::endpoint>& eps, const char* host, unsigned short port) {
    char buffer[128] = {0};
    int ret          = resolv(host, (intptr_t)&buffer[0]);
    if (0 == ret)
    {
      eps.push_back(ip::endpoint(buffer, port));
    }
    return ret;
  };
  yasio_shared_service()->set_option(YOPT_S_RESOLV_FN, &fn);
}
/*
  Because, unity c# Marshal call C language vardic paremeter function will crash,
  so we provide this API.
  @params:
    opt: the opt value
    pszArgs: split by ';'
  */
YASIO_NI_API void yasio_set_option(int opt, const char* pszArgs)
{
  auto service = yasio_shared_service();

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
      service->set_option(opt, svtoi(args[0]), svtoi(args[1]), svtoi(args[2]), svtoi(args[3]),
                          svtoi(args[4]));
      break;
    default:
      YASIO_LOG("The option: %d unsupported by yasio_set_option!", opt);
  }
}
YASIO_NI_API void yasio_set_option_vp(int opt, ...)
{
  auto service = yasio_shared_service();

  va_list ap;
  va_start(ap, opt);

  if (opt != YOPT_S_RESOLV_FN && opt != YOPT_S_PRINT_FN)
    service->set_option_internal(opt, ap);

  va_end(ap);
}
YASIO_NI_API void yasio_open(int cindex, int kind) { yasio_shared_service()->open(cindex, kind); }
YASIO_NI_API void yasio_close(int cindex) { yasio_shared_service()->close(cindex); }
YASIO_NI_API void yasio_close_handle(intptr_t thandle)
{
  auto p = reinterpret_cast<transport_handle_t>(thandle);
  yasio_shared_service()->close(p);
}
YASIO_NI_API int yasio_write(intptr_t thandle, const unsigned char* bytes, int len)
{
  std::vector<char> buf(bytes, bytes + len);
  auto p = reinterpret_cast<transport_handle_t>(thandle);
  return yasio_shared_service()->write(p, std::move(buf));
}
YASIO_NI_API uint32_t yasio_tcp_rtt(intptr_t thandle)
{
  auto p = reinterpret_cast<transport_handle_t>(thandle);
  return io_service::tcp_rtt(p);
}
YASIO_NI_API void yasio_dispatch(int count) { yasio_shared_service()->dispatch(count); }
YASIO_NI_API void yasio_stop() { yasio_shared_service()->stop(); }
YASIO_NI_API long long yasio_highp_time(void) { return highp_clock<system_clock_t>(); }
YASIO_NI_API long long yasio_highp_clock(void) { return highp_clock<steady_clock_t>(); }
YASIO_NI_API void yasio_set_print_fn(void(YASIO_INTEROP_DECL* pfn)(int, const char*))
{
  yasio::inet::print_fn2_t custom_print = pfn;
  yasio_shared_service()->set_option(YOPT_S_PRINT_FN2, &custom_print);
}
YASIO_NI_API void yasio_memcpy(void* dst, const void* src, unsigned int len)
{
  ::memcpy(dst, src, len);
}
}
