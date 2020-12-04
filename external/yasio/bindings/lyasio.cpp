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
#if !defined(LUA_LIB)
#  define LUA_LIB
#endif

#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/yasio.hpp"
#include "yasio/bindings/lyasio.h"
using namespace yasio;
using namespace yasio::inet;

namespace lyasio
{
static auto obstream_write_v = [](yasio::obstream* obs, cxx17::string_view val,
                                  int length_field_bits) {
  // default: Use variant length of length field, just like .net BinaryWriter.Write(String),
  // see:
  // https://github.com/mono/mono/blob/master/mcs/class/referencesource/mscorlib/system/io/binarywriter.cs
  switch (length_field_bits)
  {
    case -1:
      return obs->write_v(val);
    case 32:
      return obs->write_v32(val);
    case 16:
      return obs->write_v16(val);
    default:
      return obs->write_v8(val);
  }
};
static auto ibstream_read_v = [](yasio::ibstream* ibs, int length_field_bits) {
  // default: Use variant length of length field, just like .net BinaryReader.ReadString,
  // see:
  // https://github.com/mono/mono/blob/master/mcs/class/referencesource/mscorlib/system/io/binaryreader.cs
  switch (length_field_bits)
  {
    case -1:
      return ibs->read_v();
    case 32:
      return ibs->read_v32();
    case 16:
      return ibs->read_v16();
    default:
      return ibs->read_v8();
  }
};
} // namespace lyasio

#if YASIO__HAS_CXX14

#  if !YASIO__HAS_CXX20
#    include "sol/sol.hpp"
#  else
#    include "sol3/sol.hpp"
#  endif

#  if !YASIO__HAS_CXX17
namespace sol
{
namespace stack
{
template <> struct pusher<cxx17::string_view>
{
  static int push(lua_State* L, const cxx17::string_view& str)
  {
    lua_pushlstring(L, !str.empty() ? str.c_str() : "", str.length());
    return 1;
  }
};
template <> struct getter<cxx17::string_view>
{
  static cxx17::string_view get(lua_State* L, int index, record& tracking)
  {
    tracking.use(1); // THIS IS THE ONLY BIT THAT CHANGES
    size_t len    = 0;
    const char* s = lua_tolstring(L, index, &len);
    return cxx17::string_view(s, len);
  }
};
} // namespace stack
template <> struct lua_type_of<cxx17::string_view> : std::integral_constant<type, type::string>
{};
} // namespace sol
#  endif

extern "C" {

YASIO_LUA_API int luaopen_yasio(lua_State* L)
{
  sol::state_view state_view(L);

#  if !YASIO_LUA_ENABLE_GLOBAL
  auto lyasio = state_view.create_table();
#  else
  auto lyasio = state_view.create_named_table("yasio");
#  endif
  lyasio.new_usertype<io_event>(
      "io_event", "kind", &io_event::kind, "status", &io_event::status, "packet",
      [](io_event* ev, sol::variadic_args args) {
        bool copy = false;
        if (args.size() >= 2)
          copy = args[1];
        return std::unique_ptr<yasio::ibstream>(!copy ? new yasio::ibstream(std::move(ev->packet()))
                                                      : new yasio::ibstream(ev->packet()));
      },
      "cindex", &io_event::cindex, "transport", &io_event::transport
#  if !defined(YASIO_MINIFY_EVENT)
      ,
      "timestamp", &io_event::timestamp
#  endif
  );

  lyasio.new_usertype<io_service>(
      "io_service", "new",
      sol::initializers(
          [](io_service& uninitialized_memory) { return new (&uninitialized_memory) io_service(); },
          [](io_service& uninitialized_memory, int n) {
            return new (&uninitialized_memory) io_service(n);
          },
          [](io_service& uninitialized_memory, sol::table channel_eps) {
            std::vector<io_hostent> hosts;
            auto host = channel_eps["host"];
            if (host.valid())
              hosts.push_back(io_hostent(host, channel_eps["port"]));
            else
            {
              for (auto item : channel_eps)
              {
                auto ep = item.second.as<sol::table>();
                hosts.push_back(io_hostent(ep["host"], ep["port"]));
              }
            }
            return new (&uninitialized_memory)
                io_service(!hosts.empty() ? &hosts.front() : nullptr,
                           (std::max)(static_cast<int>(hosts.size()), 1));
          }),
      sol::meta_function::garbage_collect,
      sol::destructor([](io_service& memory_from_lua) { memory_from_lua.~io_service(); }), "start",
      [](io_service* service, sol::function cb) {
        service->start([=](event_ptr ev) { cb(std::move(ev)); });
      },
      "stop", &io_service::stop, "set_option",
      [](io_service* service, int opt, sol::variadic_args va) {
        switch (opt)
        {
          case YOPT_C_LOCAL_HOST:
          case YOPT_C_REMOTE_HOST:
            service->set_option(opt, static_cast<int>(va[0]), va[1].as<const char*>());
            break;
#  if YASIO_VERSION_NUM >= 0x033100
          case YOPT_C_LFBFD_IBTS:
#  endif
          case YOPT_C_LOCAL_PORT:
          case YOPT_C_REMOTE_PORT:
          case YOPT_C_KCP_CONV:
            service->set_option(opt, static_cast<int>(va[0]), static_cast<int>(va[1]));
            break;
          case YOPT_C_ENABLE_MCAST:
          case YOPT_C_LOCAL_ENDPOINT:
          case YOPT_C_REMOTE_ENDPOINT:
          case YOPT_C_MOD_FLAGS:
            service->set_option(opt, static_cast<int>(va[0]), va[1].as<const char*>(),
                                static_cast<int>(va[2]));
            break;
          case YOPT_S_TCP_KEEPALIVE:
            service->set_option(opt, static_cast<int>(va[0]), static_cast<int>(va[1]),
                                static_cast<int>(va[2]));
            break;
          case YOPT_C_LFBFD_PARAMS:
            service->set_option(opt, static_cast<int>(va[0]), static_cast<int>(va[1]),
                                static_cast<int>(va[2]), static_cast<int>(va[3]),
                                static_cast<int>(va[4]));
            break;
          case YOPT_S_EVENT_CB:
            (void)0;
            {
              sol::function fn     = va[0];
              io_event_cb_t fnwrap = [=](event_ptr e) mutable -> void { fn(std::move(e)); };
              service->set_option(opt, std::addressof(fnwrap));
            }
            break;
          default:
            service->set_option(opt, static_cast<int>(va[0]));
        }
      },
      "dispatch", &io_service::dispatch, "open", &io_service::open, "is_open",
      sol::overload(
          static_cast<bool (io_service::*)(int) const>(&io_service::is_open),
          static_cast<bool (io_service::*)(transport_handle_t) const>(&io_service::is_open)),
      "close",
      sol::overload(static_cast<void (io_service::*)(transport_handle_t)>(&io_service::close),
                    static_cast<void (io_service::*)(int)>(&io_service::close)),
      "write",
      sol::overload(
          [](io_service* service, transport_handle_t transport, cxx17::string_view s) {
            return service->write(transport, std::vector<char>(s.data(), s.data() + s.length()));
          },
          [](io_service* service, transport_handle_t transport, yasio::obstream* obs) {
            return service->write(transport, std::move(obs->buffer()));
          }),
      "write_to",
      sol::overload(
          [](io_service* service, transport_handle_t transport, cxx17::string_view s,
             cxx17::string_view ip, u_short port) {
            return service->write_to(transport, std::vector<char>(s.data(), s.data() + s.length()),
                                     ip::endpoint{ip.data(), port});
          },
          [](io_service* service, transport_handle_t transport, yasio::obstream* obs,
             cxx17::string_view ip, u_short port) {
            return service->write_to(transport, std::move(obs->buffer()),
                                     ip::endpoint{ip.data(), port});
          }),
      "native_ptr", [](io_service* service) { return (void*)service; });

  // ##-- obstream
  lyasio.new_usertype<yasio::obstream>(
      "obstream", "push32", &yasio::obstream::push32, "pop32",
      sol::overload(static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop32),
                    static_cast<void (yasio::obstream ::*)(uint32_t)>(&yasio::obstream::pop32)),
      "push24", &yasio::obstream::push24, "pop24",
      sol::overload(static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop24),
                    static_cast<void (yasio::obstream ::*)(uint32_t)>(&yasio::obstream::pop24)),
      "push16", &yasio::obstream::push16, "pop16",
      sol::overload(static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop16),
                    static_cast<void (yasio::obstream ::*)(uint16_t)>(&yasio::obstream::pop16)),
      "push8", &yasio::obstream::push8, "pop8",
      sol::overload(static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop8),
                    static_cast<void (yasio::obstream ::*)(uint8_t)>(&yasio::obstream::pop8)),
      "write_ix", &yasio::obstream::write_ix<int64_t>,
      "write_bool", &yasio::obstream::write<bool>, "write_i8", &yasio::obstream::write<int8_t>,
      "write_i16", &yasio::obstream::write<int16_t>, "write_i24", &yasio::obstream::write_i24,
      "write_i32", &yasio::obstream::write<int32_t>, "write_i64",
      &yasio::obstream::write<int64_t>, "write_u8", &yasio::obstream::write<uint8_t>,
      "write_u16", &yasio::obstream::write<uint16_t>, "write_u32",
      &yasio::obstream::write<uint32_t>, "write_u64", &yasio::obstream::write<uint64_t>,
      "write_f", &yasio::obstream::write<float>, "write_lf", &yasio::obstream::write<double>,
      "write_v",
      [](yasio::obstream* obs, cxx17::string_view sv, sol::variadic_args args) {
        int lfl = -1;
        if (args.size() > 0)
          lfl = static_cast<int>(args[0]);
        return lyasio::obstream_write_v(obs, sv, lfl);
      },
      "write_bytes",
      static_cast<void (yasio::obstream::*)(cxx17::string_view)>(&yasio::obstream::write_bytes),
      "length", &yasio::obstream::length, "to_string",
      [](yasio::obstream* obs) { return cxx17::string_view(obs->data(), obs->length()); });

  // ##-- yasio::ibstream
  lyasio.new_usertype<yasio::ibstream>(
      "ibstream",
      sol::constructors<yasio::ibstream(std::vector<char>),
                        yasio::ibstream(const yasio::obstream*)>(),
      "read_ix", &yasio::ibstream::read_ix<int64_t>,
      "read_bool", &yasio::ibstream::read<bool>, "read_i8", &yasio::ibstream::read<int8_t>,
      "read_i16", &yasio::ibstream::read<int16_t>, "read_i24", &yasio::ibstream::read_i24,
      "read_i32", &yasio::ibstream::read<int32_t>, "read_i64", &yasio::ibstream::read<int64_t>,
      "read_u8", &yasio::ibstream::read<uint8_t>, "read_u16", &yasio::ibstream::read<uint16_t>,
      "read_u24", &yasio::ibstream::read_u24, "read_u32", &yasio::ibstream::read<uint32_t>,
      "read_u64", &yasio::ibstream::read<uint64_t>, "read_f", &yasio::ibstream::read<float>,
      "read_lf", &yasio::ibstream::read<double>, "read_v",
      [](yasio::ibstream* ibs, sol::variadic_args args) {
        int lfl = -1;
        if (args.size() > 0)
          lfl = static_cast<int>(args[0]);
        return lyasio::ibstream_read_v(ibs, lfl);
      },
      "read_bytes",
      static_cast<cxx17::string_view (yasio::ibstream::*)(int)>(&yasio::ibstream::read_bytes),
      "seek", &yasio::ibstream_view::seek, "length", &yasio::ibstream_view::length, "to_string",
      [](yasio::ibstream* ibs) { return cxx17::string_view(ibs->data(), ibs->length()); });

  lyasio["highp_clock"] = &highp_clock<steady_clock_t>;
  lyasio["highp_time"]  = &highp_clock<system_clock_t>;

  // ##-- yasio enums
#  define YASIO_EXPORT_ENUM(v) lyasio[#  v] = v
  YASIO_EXPORT_ENUM(YCK_TCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_TCP_SERVER);
  YASIO_EXPORT_ENUM(YCK_UDP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_UDP_SERVER);
#  if defined(YASIO_HAVE_KCP)
  YASIO_EXPORT_ENUM(YCK_KCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_KCP_SERVER);
#  endif
#  if defined(YASIO_HAVE_SSL)
  YASIO_EXPORT_ENUM(YCK_SSL_CLIENT);
#  endif

  YASIO_EXPORT_ENUM(YOPT_S_CONNECT_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_DNS_CACHE_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_DNS_QUERIES_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_TCP_KEEPALIVE);
  YASIO_EXPORT_ENUM(YOPT_S_EVENT_CB);
  YASIO_EXPORT_ENUM(YOPT_C_LFBFD_PARAMS);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_HOST);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_PORT);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_ENDPOINT);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_HOST);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_PORT);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_ENDPOINT);
  YASIO_EXPORT_ENUM(YOPT_C_ENABLE_MCAST);
  YASIO_EXPORT_ENUM(YOPT_C_DISABLE_MCAST);
  YASIO_EXPORT_ENUM(YOPT_C_KCP_CONV);
  YASIO_EXPORT_ENUM(YOPT_C_MOD_FLAGS);

  YASIO_EXPORT_ENUM(YCF_REUSEADDR);
  YASIO_EXPORT_ENUM(YCF_EXCLUSIVEADDRUSE);

  YASIO_EXPORT_ENUM(YEK_CONNECT_RESPONSE);
  YASIO_EXPORT_ENUM(YEK_CONNECTION_LOST);
  YASIO_EXPORT_ENUM(YEK_PACKET);

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);

  return lyasio.push(); /* return 'yasio' table */
}

} /* extern "C" */

#else

#  include "kaguya/kaguya.hpp"
/// customize the type conversion from/to lua
namespace kaguya
{
// cxx17::string_view
template <> struct lua_type_traits<cxx17::string_view>
{
  typedef cxx17::string_view get_type;
  typedef cxx17::string_view push_type;

  static bool strictCheckType(lua_State* l, int index) { return lua_type(l, index) == LUA_TSTRING; }
  static bool checkType(lua_State* l, int index) { return lua_isstring(l, index) != 0; }
  static get_type get(lua_State* l, int index)
  {
    size_t size        = 0;
    const char* buffer = lua_tolstring(l, index, &size);
    return cxx17::string_view(buffer, size);
  }
  static int push(lua_State* l, push_type s)
  {
    lua_pushlstring(l, s.data(), s.size());
    return 1;
  }
};
// std::vector<char>
template <> struct lua_type_traits<std::vector<char>>
{
  typedef std::vector<char> get_type;
  typedef const std::vector<char>& push_type;

  static bool strictCheckType(lua_State* l, int index) { return lua_type(l, index) == LUA_TSTRING; }
  static bool checkType(lua_State* l, int index) { return lua_isstring(l, index) != 0; }
  static get_type get(lua_State* l, int index)
  {
    size_t size        = 0;
    const char* buffer = lua_tolstring(l, index, &size);
    return std::vector<char>(buffer, buffer + size);
  }
  static int push(lua_State* l, push_type s)
  {
    lua_pushlstring(l, s.data(), s.size());
    return 1;
  }
};
// transport_handle_t
template <> struct lua_type_traits<yasio::inet::transport_handle_t>
{
  typedef yasio::inet::transport_handle_t get_type;
  typedef yasio::inet::transport_handle_t push_type;

  static bool strictCheckType(lua_State* l, int index)
  {
    return lua_type(l, index) == LUA_TLIGHTUSERDATA;
  }
  static bool checkType(lua_State* l, int index) { return lua_islightuserdata(l, index) != 0; }
  static get_type get(lua_State* l, int index)
  {
    return reinterpret_cast<get_type>(lua_touserdata(l, index));
  }
  static int push(lua_State* l, push_type s)
  {
    lua_pushlightuserdata(l, s);
    return 1;
  }
};

template <> struct lua_type_traits<std::vector<yasio::inet::io_hostent>>
{
  typedef std::vector<yasio::inet::io_hostent> get_type;
  typedef std::vector<yasio::inet::io_hostent> push_type;

  static bool strictCheckType(lua_State* l, int index) { return lua_type(l, index) == LUA_TTABLE; }
  static bool checkType(lua_State* l, int index) { return lua_istable(l, index) != 0; }
  static get_type get(lua_State* l, int index)
  {
    lua_pushvalue(l, index);

    kaguya::LuaTable channel_eps(l, kaguya::StackTop{});
    std::vector<yasio::inet::io_hostent> hosts;
    auto host = channel_eps["host"];
    if (host)
      hosts.push_back(yasio::inet::io_hostent(host, channel_eps["port"]));
    else
    {
      channel_eps.foreach_table<int, kaguya::LuaTable>([&](int, kaguya::LuaTable ep) {
        hosts.push_back(yasio::inet::io_hostent(ep["host"], ep["port"]));
      });
    }

    lua_pop(l, 1);

    return hosts;
  }
  static int push(lua_State* l, push_type s)
  { // don't need push yasio::inet::io_hostent to lua
    assert(false);
    return 1;
  }
};
}; // namespace kaguya

extern "C" {

YASIO_LUA_API int luaopen_yasio(lua_State* L)
{
  kaguya::State state(L);

  auto lyasio    = state.newTable();
#  if YASIO_LUA_ENABLE_GLOBAL
  state["yasio"] = lyasio;
#  endif
  // No any interface need export, only for holder
  // lyasio["io_transport"].setClass(kaguya::UserdataMetatable<io_transport>());

  lyasio["io_event"].setClass(
      kaguya::UserdataMetatable<io_event>()

          .addFunction("kind", &io_event::kind)
          .addFunction("status", &io_event::status)
          .addStaticFunction("packet",
                             [](io_event* ev, bool /*raw*/, bool copy) {
                               return std::unique_ptr<yasio::ibstream>(
                                   !copy ? new yasio::ibstream(std::move(ev->packet()))
                                         : new yasio::ibstream(ev->packet()));
                             })
          .addFunction("cindex", &io_event::cindex)
          .addFunction("transport", &io_event::transport)
          .addFunction("timestamp", &io_event::timestamp));

  lyasio["io_service"].setClass(
      kaguya::UserdataMetatable<io_service>()
          .setConstructors<io_service(), io_service(int),
                           io_service(const std::vector<io_hostent>&)>()
          .addStaticFunction("start",
                             [](io_service* service, kaguya::LuaFunction cb) {
                               io_event_cb_t fnwrap = [=](event_ptr e) mutable -> void {
                                 cb(e.get());
                               };
                               service->start(std::move(fnwrap));
                             })
          .addFunction("stop", &io_service::stop)
          .addFunction("dispatch", &io_service::dispatch)
          .addFunction("open", &io_service::open)
          .addOverloadedFunctions(
              "is_open", static_cast<bool (io_service::*)(int) const>(&io_service::is_open),
              static_cast<bool (io_service::*)(transport_handle_t) const>(&io_service::is_open))
          .addOverloadedFunctions(
              "close", static_cast<void (io_service::*)(transport_handle_t)>(&io_service::close),
              static_cast<void (io_service::*)(int)>(&io_service::close))
          .addOverloadedFunctions(
              "write",
              [](io_service* service, transport_handle_t transport, cxx17::string_view s) {
                return service->write(transport,
                                      std::vector<char>(s.data(), s.data() + s.length()));
              },
              [](io_service* service, transport_handle_t transport, yasio::obstream* obs) {
                return service->write(transport, std::move(obs->buffer()));
              })
          .addOverloadedFunctions(
              "write_to",
              [](io_service* service, transport_handle_t transport, cxx17::string_view s,
                 cxx17::string_view ip, u_short port) {
                return service->write_to(transport,
                                         std::vector<char>(s.data(), s.data() + s.length()),
                                         ip::endpoint{ip.data(), port});
              },
              [](io_service* service, transport_handle_t transport, yasio::obstream* obs,
                 cxx17::string_view ip, u_short port) {
                return service->write_to(transport, std::move(obs->buffer()),
                                         ip::endpoint{ip.data(), port});
              })
          .addStaticFunction(
              "set_option",
              [](io_service* service, int opt, kaguya::VariadicArgType args) {
                switch (opt)
                {
                  case YOPT_C_LOCAL_HOST:
                  case YOPT_C_REMOTE_HOST:
                    service->set_option(opt, static_cast<int>(args[0]),
                                        static_cast<const char*>(args[1]));
                    break;

#  if YASIO_VERSION_NUM >= 0x033100
                  case YOPT_C_LFBFD_IBTS:
#  endif
                  case YOPT_C_LOCAL_PORT:
                  case YOPT_C_REMOTE_PORT:
                  case YOPT_C_KCP_CONV:
                    service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]));
                    break;
                  case YOPT_C_ENABLE_MCAST:
                  case YOPT_C_LOCAL_ENDPOINT:
                  case YOPT_C_REMOTE_ENDPOINT:
                  case YOPT_C_MOD_FLAGS:
                    service->set_option(opt, static_cast<int>(args[0]),
                                        static_cast<const char*>(args[1]),
                                        static_cast<int>(args[2]));
                    break;
                  case YOPT_S_TCP_KEEPALIVE:
                    service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]),
                                        static_cast<int>(args[2]));
                    break;
                  case YOPT_C_LFBFD_PARAMS:
                    service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]),
                                        static_cast<int>(args[2]), static_cast<int>(args[3]),
                                        static_cast<int>(args[4]));
                    break;
                  case YOPT_S_EVENT_CB:
                    (void)0;
                    {
                      kaguya::LuaFunction fn = args[0];
                      io_event_cb_t fnwrap   = [=](event_ptr e) mutable -> void { fn(e.get()); };
                      service->set_option(opt, std::addressof(fnwrap));
                    }
                    break;
                  default:
                    service->set_option(opt, static_cast<int>(args[0]));
                }
              })
          .addStaticFunction("native_ptr", [](io_service* service) { return (void*)service; }));

  // ##-- yasio::obstream
  lyasio["obstream"].setClass(
      kaguya::UserdataMetatable<yasio::obstream>()
          .setConstructors<yasio::obstream(), yasio::obstream(int)>()
          .addFunction("push32", &yasio::obstream::push32)
          .addOverloadedFunctions(
              "pop32", static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop32),
              static_cast<void (yasio::obstream ::*)(uint32_t)>(&yasio::obstream::pop32))
          .addFunction("push24", &yasio::obstream::push24)
          .addOverloadedFunctions(
              "pop24", static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop24),
              static_cast<void (yasio::obstream ::*)(uint32_t)>(&yasio::obstream::pop24))
          .addFunction("push16", &yasio::obstream::push16)
          .addOverloadedFunctions(
              "pop16", static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop16),
              static_cast<void (yasio::obstream ::*)(uint16_t)>(&yasio::obstream::pop16))
          .addFunction("push8", &yasio::obstream::push8)
          .addOverloadedFunctions(
              "pop8", static_cast<void (yasio::obstream ::*)()>(&yasio::obstream::pop8),
              static_cast<void (yasio::obstream ::*)(uint8_t)>(&yasio::obstream::pop8))
          .addFunction("write_ix", &yasio::obstream::write_ix<int64_t>)
          .addFunction("write_bool", &yasio::obstream::write<bool>)
          .addFunction("write_i8", &yasio::obstream::write<int8_t>)
          .addFunction("write_i16", &yasio::obstream::write<int16_t>)
          .addFunction("write_i24", &yasio::obstream::write_i24)
          .addFunction("write_i32", &yasio::obstream::write<int32_t>)
          .addFunction("write_i64", &yasio::obstream::write<int64_t>)
          .addFunction("write_u8", &yasio::obstream::write<uint8_t>)
          .addFunction("write_u16", &yasio::obstream::write<uint16_t>)
          .addFunction("write_u32", &yasio::obstream::write<uint32_t>)
          .addFunction("write_u64", &yasio::obstream::write<uint64_t>)
          .addFunction("write_f", &yasio::obstream::write<float>)
          .addFunction("write_lf", &yasio::obstream::write<double>)
          .addStaticFunction(
              "write_v",
              [](yasio::obstream* obs, cxx17::string_view sv, kaguya::VariadicArgType args) {
                int lfl = -1;
                if (args.size() > 0)
                  lfl = static_cast<int>(args[0]);
                return lyasio::obstream_write_v(obs, sv, lfl);
              })
          .addFunction("write_bytes", static_cast<void (yasio::obstream::*)(cxx17::string_view)>(
                                          &yasio::obstream::write_bytes))
          .addFunction("length", &yasio::obstream::length)
          .addStaticFunction("to_string", [](yasio::obstream* obs) {
            return cxx17::string_view(obs->data(), obs->length());
          }));

  // ##-- yasio::ibstream_view
  lyasio["ibstream_view"].setClass(
      kaguya::UserdataMetatable<yasio::ibstream_view>()
          .setConstructors<yasio::ibstream_view(), yasio::ibstream_view(const void*, int),
                           yasio::ibstream_view(const yasio::obstream*)>()
          .addFunction("read_ix", &yasio::ibstream_view::read_ix<int64_t>)
          .addFunction("read_bool", &yasio::ibstream_view::read<bool>)
          .addFunction("read_i8", &yasio::ibstream_view::read<int8_t>)
          .addFunction("read_i16", &yasio::ibstream_view::read<int16_t>)
          .addFunction("read_i24", &yasio::ibstream_view::read_i24)
          .addFunction("read_i32", &yasio::ibstream_view::read<int32_t>)
          .addFunction("read_i64", &yasio::ibstream_view::read<int64_t>)
          .addFunction("read_u8", &yasio::ibstream_view::read<uint8_t>)
          .addFunction("read_u16", &yasio::ibstream_view::read<uint16_t>)
          .addFunction("read_u24", &yasio::ibstream_view::read_u24)
          .addFunction("read_u32", &yasio::ibstream_view::read<uint32_t>)
          .addFunction("read_u64", &yasio::ibstream_view::read<uint64_t>)
          .addFunction("read_f", &yasio::ibstream_view::read<float>)
          .addFunction("read_lf", &yasio::ibstream_view::read<double>)
          .addStaticFunction("read_v",
                             [](yasio::ibstream* ibs, kaguya::VariadicArgType args) {
                               int length_field_bits = -1;
                               if (args.size() > 0)
                                 length_field_bits = static_cast<int>(args[0]);
                               return lyasio::ibstream_read_v(ibs, length_field_bits);
                             })
          .addFunction("read_bytes", static_cast<cxx17::string_view (yasio::ibstream_view::*)(int)>(
                                         &yasio::ibstream_view::read_bytes))
          .addFunction("seek", &yasio::ibstream_view::seek)
          .addFunction("length", &yasio::ibstream_view::length)
          .addStaticFunction("to_string", [](yasio::ibstream_view* ibs) {
            return cxx17::string_view(ibs->data(), ibs->length());
          }));

  // ##-- ibstream
  lyasio["ibstream"].setClass(kaguya::UserdataMetatable<yasio::ibstream, yasio::ibstream_view>()
                                  .setConstructors<yasio::ibstream(std::vector<char>),
                                                   yasio::ibstream(const yasio::obstream*)>());

  lyasio.setField("highp_clock", &highp_clock<steady_clock_t>);
  lyasio.setField("highp_time", &highp_clock<system_clock_t>);

  // ##-- yasio enums
#  define YASIO_EXPORT_ENUM(v) lyasio[#  v] = v
  YASIO_EXPORT_ENUM(YCK_TCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_TCP_SERVER);
  YASIO_EXPORT_ENUM(YCK_UDP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_UDP_SERVER);
#  if defined(YASIO_HAVE_KCP)
  YASIO_EXPORT_ENUM(YCK_KCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_KCP_SERVER);
#  endif
#  if defined(YASIO_HAVE_SSL)
  YASIO_EXPORT_ENUM(YCK_SSL_CLIENT);
#  endif

  YASIO_EXPORT_ENUM(YOPT_S_CONNECT_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_DNS_CACHE_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_DNS_QUERIES_TIMEOUT);
  YASIO_EXPORT_ENUM(YOPT_S_TCP_KEEPALIVE);
  YASIO_EXPORT_ENUM(YOPT_S_EVENT_CB);
  YASIO_EXPORT_ENUM(YOPT_C_LFBFD_PARAMS);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_HOST);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_PORT);
  YASIO_EXPORT_ENUM(YOPT_C_LOCAL_ENDPOINT);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_HOST);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_PORT);
  YASIO_EXPORT_ENUM(YOPT_C_REMOTE_ENDPOINT);
  YASIO_EXPORT_ENUM(YOPT_C_ENABLE_MCAST);
  YASIO_EXPORT_ENUM(YOPT_C_DISABLE_MCAST);
  YASIO_EXPORT_ENUM(YOPT_C_KCP_CONV);
  YASIO_EXPORT_ENUM(YOPT_C_MOD_FLAGS);

  YASIO_EXPORT_ENUM(YCF_REUSEADDR);
  YASIO_EXPORT_ENUM(YCF_EXCLUSIVEADDRUSE);

  YASIO_EXPORT_ENUM(YEK_CONNECT_RESPONSE);
  YASIO_EXPORT_ENUM(YEK_CONNECTION_LOST);
  YASIO_EXPORT_ENUM(YEK_PACKET);

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);

  return lyasio.push(); /* return 'yasio' table */
}

} /* extern "C" */

#endif /* YASIO__HAS_CXX17 */

extern "C" {
YASIO_LUA_API void lyasio_set_print_fn(void* inst, void (*pfn)(const char*))
{
  if (inst)
  {
    auto service            = (io_service*)inst;
    print_fn_t custom_print = pfn;
    service->set_option(YOPT_S_PRINT_FN, &custom_print);
  }
}
}
