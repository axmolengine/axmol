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
#if !defined(LUA_LIB)
#  define LUA_LIB
#endif

#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/yasio.hpp"
#include "yasio/bindings/lyasio.hpp"
#include "yasio/bindings/yasio_sol.hpp"
using namespace yasio;
using namespace yasio::inet;

namespace lyasio
{
template <typename _Stream> static void obstream_write_v(_Stream* obs, cxx17::string_view val, int length_field_bits)
{
  // default: Use variant length of length field, just like .net BinaryWriter.Write(String),
  // see:
  // https://github.com/mono/mono/blob/master/mcs/class/referencesource/mscorlib/system/io/binarywriter.cs
  switch (length_field_bits)
  {
    case -1:
    default:
      return obs->write_v(val);
    case 32:
      return obs->write_v32(val);
    case 16:
      return obs->write_v16(val);
    case 8:
      return obs->write_v8(val);
  }
};
template <typename _Stream> static cxx17::string_view ibstream_read_v(_Stream* ibs, int length_field_bits)
{
  // default: Use variant length of length field, just like .net BinaryReader.ReadString,
  // see:
  // https://github.com/mono/mono/blob/master/mcs/class/referencesource/mscorlib/system/io/binaryreader.cs
  switch (length_field_bits)
  {
    case -1:
    default:
      return ibs->read_v();
    case 32:
      return ibs->read_v32();
    case 16:
      return ibs->read_v16();
    case 8:
      return ibs->read_v8();
  }
};
} // namespace lyasio

#if YASIO__HAS_CXX14

namespace lyasio
{
template <typename _Stream> static void register_obstream(sol::table& lib, const char* usertype)
{
  lib.new_usertype<_Stream>(
      usertype, "push32", &_Stream::push32, "pop32",
      sol::overload(static_cast<void (_Stream ::*)()>(&_Stream::pop32), static_cast<void (_Stream ::*)(uint32_t)>(&_Stream::pop32)), "push16", &_Stream::push16,
      "pop16", sol::overload(static_cast<void (_Stream ::*)()>(&_Stream::pop16), static_cast<void (_Stream ::*)(uint16_t)>(&_Stream::pop16)), "push8",
      &_Stream::push8, "pop8", sol::overload(static_cast<void (_Stream ::*)()>(&_Stream::pop8), static_cast<void (_Stream ::*)(uint8_t)>(&_Stream::pop8)),
      "write_ix", &_Stream::template write_ix<int64_t>, "write_bool", &_Stream::template write<bool>, "write_i8", &_Stream::template write<int8_t>, "write_i16",
      &_Stream::template write<int16_t>, "write_i32", &_Stream::template write<int32_t>, "write_i64", &_Stream::template write<int64_t>, "write_u8",
      &_Stream::template write<uint8_t>, "write_u16", &_Stream::template write<uint16_t>, "write_u32", &_Stream::template write<uint32_t>, "write_u64",
      &_Stream::template write<uint64_t>,
#  if defined(YASIO_HAVE_HALF_FLOAT)
      "write_f16", &_Stream::template write<fp16_t>,
#  endif
      "write_f", &_Stream::template write<float>, "write_lf", &_Stream::template write<double>, "write_v",
      [](_Stream* obs, cxx17::string_view sv, sol::variadic_args args) {
        int lfl = -1;
        if (args.size() > 0)
          lfl = static_cast<int>(args[0]);
        return obstream_write_v<_Stream>(obs, sv, lfl);
      },
      "write_bytes", static_cast<void (_Stream::*)(cxx17::string_view)>(&_Stream::write_bytes), "length", &_Stream::length, "to_string",
      [](_Stream* obs) { return cxx17::string_view(obs->data(), obs->length()); }, "save", &_Stream::save);
}

template <typename _Stream, typename _StreamView, typename _OStream> static void register_ibstream(sol::table& lib, const char* usertype)
{
  lib.new_usertype<_Stream>(
      usertype, sol::constructors<_Stream(), _Stream(std::vector<char>), _Stream(const _OStream*)>(), "load", &_Stream::load, "read_ix",
      &_Stream::template read_ix<int64_t>, "read_bool", &_Stream::template read<bool>, "read_i8", &_Stream::template read<int8_t>, "read_i16",
      &_Stream::template read<int16_t>, "read_i32", &_Stream::template read<int32_t>, "read_i64", &_Stream::template read<int64_t>, "read_u8",
      &_Stream::template read<uint8_t>, "read_u16", &_Stream::template read<uint16_t>, "read_u32", &_Stream::template read<uint32_t>, "read_u64",
      &_Stream::template read<uint64_t>,
#  if defined(YASIO_HAVE_HALF_FLOAT)
      "read_f16", &_Stream::template read<fp16_t>,
#  endif
      "read_f", &_Stream::template read<float>, "read_lf", &_Stream::template read<double>, "read_v",
      [](_Stream* ibs, sol::variadic_args args) {
        int lfl = -1;
        if (args.size() > 0)
          lfl = static_cast<int>(args[0]);
        return ibstream_read_v<_Stream>(ibs, lfl);
      },
      "read_bytes", static_cast<cxx17::string_view (_Stream::*)(int)>(&_Stream::read_bytes), "seek", &_StreamView::seek, "length", &_StreamView::length,
      "to_string", [](_Stream* ibs) { return cxx17::string_view(ibs->data(), ibs->length()); });
}
} // namespace lyasio

extern "C" {

YASIO_LUA_API int luaopen_yasio(lua_State* L)
{
  sol::state_view state_view(L);

#  if !YASIO_LUA_ENABLE_GLOBAL
  auto yasio_lib = state_view.create_table();
#  else
  auto yasio_lib = state_view.create_named_table("yasio");
#  endif
  yasio_lib.new_usertype<io_event>(
      "io_event", "kind", &io_event::kind, "status", &io_event::status, "passive", [](io_event* e) { return !!e->passive(); }, "packet",
      [](io_event* ev, sol::variadic_args args) {
        bool copy = false;
        if (args.size() >= 2)
          copy = args[1];
        auto& pkt = ev->packet();
        return !is_packet_empty(pkt)
                   ? std::unique_ptr<yasio::ibstream>(!copy ? new yasio::ibstream(forward_packet((packet_t &&) pkt)) : new yasio::ibstream(forward_packet(pkt)))
                   : std::unique_ptr<yasio::ibstream>{};
      },
      "cindex", &io_event::cindex, "transport", &io_event::transport
#  if !defined(YASIO_MINIFY_EVENT)
      ,
      "timestamp", &io_event::timestamp
#  endif
  );

  yasio_lib.new_usertype<io_service>(
      "io_service", "new",
      sol::initializers([](io_service& uninitialized_memory) { return new (&uninitialized_memory) io_service(); },
                        [](io_service& uninitialized_memory, int n) { return new (&uninitialized_memory) io_service(n); },
                        [](io_service& uninitialized_memory, sol::table channel_eps) {
                          std::vector<io_hostent> hosts;
                          auto host = channel_eps["host"];
                          if (host.valid())
                            hosts.push_back(io_hostent(host.get<cxx17::string_view>(), channel_eps["port"]));
                          else
                          {
                            for (auto item : channel_eps)
                            {
                              auto ep = item.second.as<sol::table>();
                              hosts.push_back(io_hostent(ep["host"].get<cxx17::string_view>(), ep["port"]));
                            }
                          }
                          return new (&uninitialized_memory)
                              io_service(!hosts.empty() ? &hosts.front() : nullptr, (std::max)(static_cast<int>(hosts.size()), 1));
                        }),
      sol::meta_function::garbage_collect, sol::destructor([](io_service& memory_from_lua) { memory_from_lua.~io_service(); }), "start",
      [](io_service* service, sol::function cb) { service->start([=](event_ptr ev) { cb(std::move(ev)); }); }, "stop", &io_service::stop, "set_option",
      [](io_service* service, int opt, sol::variadic_args args) {
        switch (opt)
        {
          case YOPT_C_LOCAL_HOST:
          case YOPT_C_REMOTE_HOST:
            service->set_option(opt, static_cast<int>(args[0]), args[1].as<const char*>());
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
            service->set_option(opt, static_cast<int>(args[0]), args[1].as<const char*>(), static_cast<int>(args[2]));
            break;
          case YOPT_C_MOD_FLAGS:
            service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]));
            break;
          case YOPT_S_TCP_KEEPALIVE:
            service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]));
            break;
          case YOPT_C_LFBFD_PARAMS:
            service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]), static_cast<int>(args[3]),
                                static_cast<int>(args[4]));
            break;
          case YOPT_S_EVENT_CB:
            (void)0;
            {
              sol::function fn     = args[0];
              io_event_cb_t fnwrap = [=](event_ptr e) mutable -> void { fn(std::move(e)); };
              service->set_option(opt, std::addressof(fnwrap));
            }
            break;
          default:
            service->set_option(opt, static_cast<int>(args[0]));
        }
      },
      "dispatch", &io_service::dispatch, "open", &io_service::open, "is_open",
      sol::overload(static_cast<bool (io_service::*)(int) const>(&io_service::is_open),
                    static_cast<bool (io_service::*)(transport_handle_t) const>(&io_service::is_open)),
      "close",
      sol::overload(static_cast<void (io_service::*)(transport_handle_t)>(&io_service::close), static_cast<void (io_service::*)(int)>(&io_service::close)),
      "write",
      sol::overload(
          [](io_service* service, transport_handle_t transport, cxx17::string_view s) {
            return service->write(transport, std::vector<char>(s.data(), s.data() + s.length()));
          },
          [](io_service* service, transport_handle_t transport, yasio::obstream* obs) { return service->write(transport, std::move(obs->buffer())); }),
      "write_to",
      sol::overload(
          [](io_service* service, transport_handle_t transport, cxx17::string_view s, cxx17::string_view ip, u_short port) {
            return service->write_to(transport, std::vector<char>(s.data(), s.data() + s.length()), ip::endpoint{ip.data(), port});
          },
          [](io_service* service, transport_handle_t transport, yasio::obstream* obs, cxx17::string_view ip, u_short port) {
            return service->write_to(transport, std::move(obs->buffer()), ip::endpoint{ip.data(), port});
          }),
      "native_ptr", [](io_service* service) { return (void*)service; });

  // ##-- obstream
  lyasio::register_obstream<obstream>(yasio_lib, "obstream");
  lyasio::register_obstream<fast_obstream>(yasio_lib, "fast_obstream");

  // ##-- yasio::ibstream
  lyasio::register_ibstream<ibstream, ibstream_view, obstream>(yasio_lib, "ibstream");
  lyasio::register_ibstream<fast_ibstream, fast_ibstream_view, fast_obstream>(yasio_lib, "fast_ibstream");

  yasio_lib["highp_clock"] = &highp_clock<steady_clock_t>;
  yasio_lib["highp_time"]  = &highp_clock<system_clock_t>;

  yasio_lib["unwrap_ptr"] = [](lua_State* L) -> int {
    auto& pkt  = *(packet_t*)lua_touserdata(L, 1);
    int offset = lua_isinteger(L, 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    lua_pushlightuserdata(L, packet_data(pkt) + offset);
    return 1;
  };

  yasio_lib["unwrap_len"] = [](lua_State* L) -> int {
    auto& pkt  = *(packet_t*)lua_touserdata(L, 1);
    int offset = lua_isinteger(L, 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    lua_pushinteger(L, packet_len(pkt) - offset);
    return 1;
  };

  // ##-- yasio enums
#  define YASIO_EXPORT_ENUM(v) yasio_lib[#  v] = v
  YASIO_EXPORT_ENUM(YCK_TCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_TCP_SERVER);
  YASIO_EXPORT_ENUM(YCK_UDP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_UDP_SERVER);
#  if defined(YASIO_HAVE_KCP)
  YASIO_EXPORT_ENUM(YCK_KCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_KCP_SERVER);
#  endif
#  if defined(YASIO_SSL_BACKEND)
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

  YASIO_EXPORT_ENUM(YEK_ON_OPEN);
  YASIO_EXPORT_ENUM(YEK_ON_CLOSE);
  YASIO_EXPORT_ENUM(YEK_ON_PACKET);
  YASIO_EXPORT_ENUM(YEK_CONNECT_RESPONSE);
  YASIO_EXPORT_ENUM(YEK_CONNECTION_LOST);
  YASIO_EXPORT_ENUM(YEK_PACKET);

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);

  return yasio_lib.push(); /* return 'yasio' table */
}

} /* extern "C" */

#else

#  include "kaguya/kaguya.hpp"
/// customize the type conversion from/to lua
namespace kaguya
{
// cxx17::string_view
template <> struct lua_type_traits<cxx17::string_view> {
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
template <> struct lua_type_traits<std::vector<char>> {
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
template <> struct lua_type_traits<yasio::inet::transport_handle_t> {
  typedef yasio::inet::transport_handle_t get_type;
  typedef yasio::inet::transport_handle_t push_type;

  static bool strictCheckType(lua_State* l, int index) { return lua_type(l, index) == LUA_TLIGHTUSERDATA; }
  static bool checkType(lua_State* l, int index) { return lua_islightuserdata(l, index) != 0; }
  static get_type get(lua_State* l, int index) { return reinterpret_cast<get_type>(lua_touserdata(l, index)); }
  static int push(lua_State* l, push_type s)
  {
    lua_pushlightuserdata(l, s);
    return 1;
  }
};

template <> struct lua_type_traits<std::vector<yasio::inet::io_hostent>> {
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
      channel_eps.foreach_table<int, kaguya::LuaTable>([&](int, kaguya::LuaTable ep) { hosts.push_back(yasio::inet::io_hostent(ep["host"], ep["port"])); });
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
#  if defined(YASIO_HAVE_HALF_FLOAT)
template <> struct lua_type_traits<fp16_t> {
  typedef fp16_t get_type;
  typedef fp16_t push_type;

  static bool strictCheckType(lua_State* l, int index) { return lua_type(l, index) == LUA_TNUMBER; }
  static bool checkType(lua_State* l, int index) { return lua_isnumber(l, index) != 0; }
  static get_type get(lua_State* l, int index) { return fp16_t{static_cast<float>(lua_tonumber(l, index))}; }
  static int push(lua_State* l, push_type value)
  {
    lua_pushnumber(l, static_cast<float>(value));
    return 1;
  }
};
#  endif
}; // namespace kaguya

namespace lyasio
{
#  define kaguya_obstream_class(_Stream) kaguya::UserdataMetatable<_Stream>().setConstructors<_Stream(), _Stream(int)>()
#  define kaguya_ibstream_view_class(_StreamView, _OStream)                                                                                                    \
    kaguya::UserdataMetatable<_StreamView>().setConstructors<_StreamView(), _StreamView(const void*, int), _StreamView(const _OStream*)>()
#  define kaguya_ibstream_class(_Stream, _StreamView, _OStream)                                                                                                \
    kaguya::UserdataMetatable<_Stream, _StreamView>().setConstructors<_Stream(std::vector<char>), _Stream(const _OStream*)>()

template <typename _Stream> static void register_obstream(kaguya::LuaTable& lib, const char* usertype, kaguya::UserdataMetatable<_Stream>& userclass)
{
  lib[usertype].setClass(
      userclass.addFunction("push32", &_Stream::push32)
          .addOverloadedFunctions("pop32", static_cast<void (_Stream ::*)()>(&_Stream::pop32), static_cast<void (_Stream ::*)(uint32_t)>(&_Stream::pop32))
          .addFunction("push16", &_Stream::push16)
          .addOverloadedFunctions("pop16", static_cast<void (_Stream ::*)()>(&_Stream::pop16), static_cast<void (_Stream ::*)(uint16_t)>(&_Stream::pop16))
          .addFunction("push8", &_Stream::push8)
          .addOverloadedFunctions("pop8", static_cast<void (_Stream ::*)()>(&_Stream::pop8), static_cast<void (_Stream ::*)(uint8_t)>(&_Stream::pop8))
          .addFunction("write_ix", &_Stream::template write_ix<int64_t>)
          .addFunction("write_bool", &_Stream::template write<bool>)
          .addFunction("write_i8", &_Stream::template write<int8_t>)
          .addFunction("write_i16", &_Stream::template write<int16_t>)
          .addFunction("write_i32", &_Stream::template write<int32_t>)
          .addFunction("write_i64", &_Stream::template write<int64_t>)
          .addFunction("write_u8", &_Stream::template write<uint8_t>)
          .addFunction("write_u16", &_Stream::template write<uint16_t>)
          .addFunction("write_u32", &_Stream::template write<uint32_t>)
          .addFunction("write_u64", &_Stream::template write<uint64_t>)
#  if defined(YASIO_HAVE_HALF_FLOAT)
          .addFunction("write_f16", &_Stream::template write<fp16_t>)
#  endif
          .addFunction("write_f", &_Stream::template write<float>)
          .addFunction("write_lf", &_Stream::template write<double>)
          .addStaticFunction("write_v",
                             [](_Stream* obs, cxx17::string_view sv, kaguya::VariadicArgType args) {
                               int lfl = -1;
                               if (args.size() > 0)
                                 lfl = static_cast<int>(args[0]);
                               return lyasio::obstream_write_v(obs, sv, lfl);
                             })
          .addFunction("write_bytes", static_cast<void (_Stream::*)(cxx17::string_view)>(&_Stream::write_bytes))
          .addFunction("length", &_Stream::length)
          .addStaticFunction("to_string", [](_Stream* obs) { return cxx17::string_view(obs->data(), obs->length()); }));
}
template <typename _Stream, typename _StreamView, typename _OStream>
static void register_ibstream(kaguya::LuaTable& lib, const char* usertype, const char* basetype, kaguya::UserdataMetatable<_Stream, _StreamView>& userclass,
                              kaguya::UserdataMetatable<_StreamView>& baseclass)
{
  lib[basetype].setClass(baseclass.addFunction("read_ix", &_StreamView::template read_ix<int64_t>)
                             .addFunction("read_bool", &_StreamView::template read<bool>)
                             .addFunction("read_i8", &_StreamView::template read<int8_t>)
                             .addFunction("read_i16", &_StreamView::template read<int16_t>)
                             .addFunction("read_i32", &_StreamView::template read<int32_t>)
                             .addFunction("read_i64", &_StreamView::template read<int64_t>)
                             .addFunction("read_u8", &_StreamView::template read<uint8_t>)
                             .addFunction("read_u16", &_StreamView::template read<uint16_t>)
                             .addFunction("read_u32", &_StreamView::template read<uint32_t>)
                             .addFunction("read_u64", &_StreamView::template read<uint64_t>)
#  if defined(YASIO_HAVE_HALF_FLOAT)
                             .addFunction("read_f16", &_Stream::template read<fp16_t>)
#  endif
                             .addFunction("read_f", &_StreamView::template read<float>)
                             .addFunction("read_lf", &_StreamView::template read<double>)
                             .addStaticFunction("read_v",
                                                [](_StreamView* ibs, kaguya::VariadicArgType args) {
                                                  int length_field_bits = -1;
                                                  if (args.size() > 0)
                                                    length_field_bits = static_cast<int>(args[0]);
                                                  return lyasio::ibstream_read_v(ibs, length_field_bits);
                                                })
                             .addFunction("read_bytes", static_cast<cxx17::string_view (_StreamView::*)(int)>(&_StreamView::read_bytes))
                             .addFunction("seek", &_StreamView::seek)
                             .addFunction("length", &_StreamView::length)
                             .addStaticFunction("to_string", [](_StreamView* ibs) { return cxx17::string_view(ibs->data(), ibs->length()); }));

  // ##-- ibstream
  lib[usertype].setClass(userclass);
}
} // namespace lyasio

extern "C" {

YASIO_LUA_API int luaopen_yasio(lua_State* L)
{
  kaguya::State state(L);

  auto yasio_lib = state.newTable();
#  if YASIO_LUA_ENABLE_GLOBAL
  state["yasio"] = yasio_lib;
#  endif

  yasio_lib["io_event"].setClass(kaguya::UserdataMetatable<io_event>()

                                     .addFunction("kind", &io_event::kind)
                                     .addFunction("status", &io_event::status)
                                     .addStaticFunction("passive", [](io_event* ev) { return !!ev->passive(); })
                                     .addStaticFunction("packet",
                                                        [](io_event* ev, bool /*raw*/, bool copy) {
                                                          auto& pkt = ev->packet();
                                                          return !is_packet_empty(pkt) ? std::unique_ptr<yasio::ibstream>(
                                                                                             !copy ? new yasio::ibstream(forward_packet((packet_t &&) pkt))
                                                                                                   : new yasio::ibstream(forward_packet(pkt)))
                                                                                       : std::unique_ptr<yasio::ibstream>{};
                                                        })
                                     .addFunction("cindex", &io_event::cindex)
                                     .addFunction("transport", &io_event::transport)
                                     .addFunction("timestamp", &io_event::timestamp));

  yasio_lib["io_service"].setClass(
      kaguya::UserdataMetatable<io_service>()
          .setConstructors<io_service(), io_service(int), io_service(const std::vector<io_hostent>&)>()
          .addStaticFunction("start",
                             [](io_service* service, kaguya::LuaFunction cb) {
                               io_event_cb_t fnwrap = [=](event_ptr e) mutable -> void { cb(e.get()); };
                               service->start(std::move(fnwrap));
                             })
          .addFunction("stop", &io_service::stop)
          .addFunction("dispatch", &io_service::dispatch)
          .addFunction("open", &io_service::open)
          .addOverloadedFunctions("is_open", static_cast<bool (io_service::*)(int) const>(&io_service::is_open),
                                  static_cast<bool (io_service::*)(transport_handle_t) const>(&io_service::is_open))
          .addOverloadedFunctions("close", static_cast<void (io_service::*)(transport_handle_t)>(&io_service::close),
                                  static_cast<void (io_service::*)(int)>(&io_service::close))
          .addOverloadedFunctions(
              "write",
              [](io_service* service, transport_handle_t transport, cxx17::string_view s) {
                return service->write(transport, std::vector<char>(s.data(), s.data() + s.length()));
              },
              [](io_service* service, transport_handle_t transport, yasio::obstream* obs) { return service->write(transport, std::move(obs->buffer())); })
          .addOverloadedFunctions(
              "write_to",
              [](io_service* service, transport_handle_t transport, cxx17::string_view s, cxx17::string_view ip, u_short port) {
                return service->write_to(transport, std::vector<char>(s.data(), s.data() + s.length()), ip::endpoint{ip.data(), port});
              },
              [](io_service* service, transport_handle_t transport, yasio::obstream* obs, cxx17::string_view ip, u_short port) {
                return service->write_to(transport, std::move(obs->buffer()), ip::endpoint{ip.data(), port});
              })
          .addStaticFunction("set_option",
                             [](io_service* service, int opt, kaguya::VariadicArgType args) {
                               switch (opt)
                               {
                                 case YOPT_C_LOCAL_HOST:
                                 case YOPT_C_REMOTE_HOST:
                                   service->set_option(opt, static_cast<int>(args[0]), static_cast<const char*>(args[1]));
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
                                   service->set_option(opt, static_cast<int>(args[0]), static_cast<const char*>(args[1]), static_cast<int>(args[2]));
                                   break;
                                 case YOPT_C_MOD_FLAGS:
                                   service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]));
                                   break;
                                 case YOPT_S_TCP_KEEPALIVE:
                                   service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]));
                                   break;
                                 case YOPT_C_LFBFD_PARAMS:
                                   service->set_option(opt, static_cast<int>(args[0]), static_cast<int>(args[1]), static_cast<int>(args[2]),
                                                       static_cast<int>(args[3]), static_cast<int>(args[4]));
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

  // ##-- obstream
  lyasio::register_obstream<obstream>(yasio_lib, "obstream", kaguya_obstream_class(obstream));
  lyasio::register_obstream<fast_obstream>(yasio_lib, "fast_obstream", kaguya_obstream_class(fast_obstream));

  // ##-- yasio::ibstream
  lyasio::register_ibstream<ibstream, ibstream_view, obstream>(yasio_lib, "ibstream", "ibstream_view", kaguya_ibstream_class(ibstream, ibstream_view, obstream),
                                                               kaguya_ibstream_view_class(ibstream_view, obstream));
  lyasio::register_ibstream<fast_ibstream, fast_ibstream_view, fast_obstream>(yasio_lib, "fast_ibstream", "fast_ibstream_view",
                                                                              kaguya_ibstream_class(fast_ibstream, fast_ibstream_view, fast_obstream),
                                                                              kaguya_ibstream_view_class(fast_ibstream_view, fast_obstream));

  yasio_lib.setField("highp_clock", &highp_clock<steady_clock_t>);
  yasio_lib.setField("highp_time", &highp_clock<system_clock_t>);

  yasio_lib.setField("unwrap_ptr", [](lua_State* L) -> int {
    auto& pkt  = *(packet_t*)lua_touserdata(L, 1);
    int offset = lua_isinteger(L, 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    lua_pushlightuserdata(L, packet_data(pkt) + offset);
    return 1;
  });

  yasio_lib.setField("unwrap_len", [](lua_State* L) -> int {
    auto& pkt  = *(packet_t*)lua_touserdata(L, 1);
    int offset = lua_isinteger(L, 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    lua_pushinteger(L, packet_len(pkt) - offset);
    return 1;
  });

  // ##-- yasio enums
#  define YASIO_EXPORT_ENUM(v) yasio_lib[#  v] = v
  YASIO_EXPORT_ENUM(YCK_TCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_TCP_SERVER);
  YASIO_EXPORT_ENUM(YCK_UDP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_UDP_SERVER);
#  if defined(YASIO_HAVE_KCP)
  YASIO_EXPORT_ENUM(YCK_KCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_KCP_SERVER);
#  endif
#  if YASIO_SSL_BACKEND
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

  YASIO_EXPORT_ENUM(YEK_ON_OPEN);
  YASIO_EXPORT_ENUM(YEK_ON_CLOSE);
  YASIO_EXPORT_ENUM(YEK_ON_PACKET);
  YASIO_EXPORT_ENUM(YEK_CONNECT_RESPONSE);
  YASIO_EXPORT_ENUM(YEK_CONNECTION_LOST);
  YASIO_EXPORT_ENUM(YEK_PACKET);

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);

  return yasio_lib.push(); /* return 'yasio' table */
}

} /* extern "C" */

#endif /* YASIO__HAS_CXX17 */

extern "C" {
YASIO_LUA_API void luaregister_yasio(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");

  lua_pushcfunction(L, luaopen_yasio);
  lua_setfield(L, -2, "yasio");

  lua_pop(L, 2);
}
}
