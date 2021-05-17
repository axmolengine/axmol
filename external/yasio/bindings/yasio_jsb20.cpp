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
#define YASIO_HEADER_ONLY 1
#define YASIO_HAVE_KCP 1

#include "yasio/bindings/yasio_jsb20.hpp"
#include "yasio/yasio.hpp"
#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/detail/ref_ptr.hpp"

#if __has_include(<cocos/bindings/jswrapper/SeApi.h>) || defined(YASIO_CREATOR_30_OR_LATER)
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "cocos/platform/Application.h"
#include "cocos/base/Scheduler.h"
#include "cocos/base/StringUtil.h"
using namespace cc;
#else
#include "cocos2d.h"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/platform/CCApplication.h"
#include "cocos/base/CCScheduler.h"
using namespace cocos2d;
using StringUtil = StringUtils;
#define CC_LOG_DEBUG CCLOG
#endif

using namespace yasio;
using namespace yasio::inet;


namespace yasio_jsb
{

#define YASIO_DEFINE_REFERENCE_CLASS                                                               \
private:                                                                                           \
  unsigned int referenceCount_;                                                                    \
                                                                                                   \
public:                                                                                            \
  void retain() { ++referenceCount_; }                                                             \
  void release()                                                                                   \
  {                                                                                                \
    --referenceCount_;                                                                             \
    if (referenceCount_ == 0)                                                                      \
      delete this;                                                                                 \
  }                                                                                                \
                                                                                                   \
private:

namespace stimer
{
// The STIMER fake target: 0xfffffffe, well, any system's malloc never return a object address
// so it's always works well.
#define STIMER_TARGET_VALUE reinterpret_cast<void*>(~static_cast<uintptr_t>(0) - 1)

typedef void* TIMER_ID;
typedef std::function<void(void)> vcallback_t;

struct TimerObject
{
  TimerObject(vcallback_t&& callback) : callback_(std::move(callback)), referenceCount_(1) {}

  vcallback_t callback_;
  static uintptr_t s_timerId;

  DEFINE_OBJECT_POOL_ALLOCATION(TimerObject, 128)
  YASIO_DEFINE_REFERENCE_CLASS
};
uintptr_t TimerObject::s_timerId = 0;

TIMER_ID loop(unsigned int n, float interval, vcallback_t callback)
{
  if (n > 0 && interval >= 0)
  {
    yasio::gc::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));

    auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

    std::string key = StringUtil::format("STMR#%p", timerId);

    Application::getInstance()->getScheduler()->schedule(
        [timerObj](
            float /*dt*/) { // lambda expression hold the reference of timerObj automatically.
          timerObj->callback_();
        },
        STIMER_TARGET_VALUE, interval, n - 1, 0, false, key);

    return timerId;
  }
  return nullptr;
}

TIMER_ID delay(float delay, vcallback_t callback)
{
  if (delay > 0)
  {
    yasio::gc::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));
    auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

    std::string key = StringUtil::format("STMR#%p", timerId);
    Application::getInstance()->getScheduler()->schedule(
        [timerObj](
            float /*dt*/) { // lambda expression hold the reference of timerObj automatically.
          timerObj->callback_();
        },
        STIMER_TARGET_VALUE, 0, 0, delay, false, key);

    return timerId;
  }
  return nullptr;
}

void kill(TIMER_ID timerId)
{
  std::string key = StringUtil::format("STMR#%p", timerId);
  Application::getInstance()->getScheduler()->unschedule(key, STIMER_TARGET_VALUE);
}
void clear()
{
  Application::getInstance()->getScheduler()->unscheduleAllForTarget(STIMER_TARGET_VALUE);
}
} // namespace stimer
} // namespace yasio_jsb

/////////////// javascript like setTimeout, clearTimeout setInterval, clearInterval ///////////
bool jsb_yasio_setTimeout(se::State& s)
{
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 2)
    {
      auto arg0 = args[0];
      auto arg1 = args[1];
      CC_BREAK_IF(!arg0.toObject()->isFunction());

      yasio_jsb::stimer::vcallback_t callback = [=]() {
        se::ScriptEngine::getInstance()->clearException();
        se::AutoHandleScope hs;

        se::Value rval;
        se::Object* funcObj = arg0.toObject();
        bool succeed        = funcObj->call(args, nullptr, &rval);
        if (!succeed)
        {
          se::ScriptEngine::getInstance()->clearException();
        }
      };

      float timeout = 0;
      seval_to_float(arg1, &timeout);
      auto timerId = yasio_jsb::stimer::delay(timeout, std::move(callback));

      s.rval().setNumber((double)(int64_t)timerId);
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(jsb_yasio_setTimeout)

bool jsb_yasio_setInterval(se::State& s)
{
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 2)
    {
      auto arg0 = args[0];
      auto arg1 = args[1];
      CC_BREAK_IF(!arg0.toObject()->isFunction());

      yasio_jsb::stimer::vcallback_t callback = [=]() {
        se::ScriptEngine::getInstance()->clearException();
        se::AutoHandleScope hs;

        se::Value rval;
        se::Object* funcObj = arg0.toObject();
        bool succeed        = funcObj->call(args, nullptr, &rval);
        if (!succeed)
        {
          se::ScriptEngine::getInstance()->clearException();
        }
      };

      float interval = 0;
      seval_to_float(arg1, &interval);
      auto timerId = yasio_jsb::stimer::loop((std::numeric_limits<unsigned int>::max)(), interval,
                                             std::move(callback));

      s.rval().setNumber((double)(int64_t)timerId);
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(jsb_yasio_setInterval)

bool jsb_yasio_killTimer(se::State& s)
{
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 1)
    {
      auto arg0 = args[0];
      void* id  = (void*)(int64_t)arg0.toNumber();

      yasio_jsb::stimer::kill(id);

      s.rval().setUndefined();
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
  return false;
}
SE_BIND_FUNC(jsb_yasio_killTimer)

bool jsb_yasio_highp_clock(se::State& s)
{
  s.rval().setNumber(yasio::highp_clock());
  return true;
}
SE_BIND_FUNC(jsb_yasio_highp_clock)

bool jsb_yasio_highp_time(se::State& s)
{
  s.rval().setNumber(yasio::highp_clock<yasio::system_clock_t>());
  return true;
}
SE_BIND_FUNC(jsb_yasio_highp_time)

static bool seval_to_hostent(const se::Value& v, inet::io_hostent* ret)
{
  assert(v.isObject() && ret != nullptr);
  se::Object* obj = v.toObject();
  se::Value host;
  se::Value port;
  bool ok = obj->getProperty("host", &host);
  SE_PRECONDITION3(ok && host.isString(), false, );
  ok = obj->getProperty("port", &port);
  SE_PRECONDITION3(ok && port.isNumber(), false, );

  ret->host_ = host.toString();
  ret->port_ = port.toUint16();
  return true;
}

static bool seval_to_std_vector_hostent(const se::Value& v, std::vector<inet::io_hostent>* ret)
{
  assert(ret != nullptr);
  assert(v.isObject());
  se::Object* obj = v.toObject();
  assert(obj->isArray());
  uint32_t len = 0;
  if (obj->getArrayLength(&len))
  {
    se::Value value;
    for (uint32_t i = 0; i < len; ++i)
    {
      SE_PRECONDITION3(obj->getArrayElement(i, &value), false, ret->clear());
      assert(value.isObject());
      io_hostent ioh;
      seval_to_hostent(value, &ioh);
      ret->push_back(std::move(ioh));
    }
    return true;
  }

  ret->clear();
  return false;
}

cxx17::string_view seval_to_string_view(const se::Value& v, bool* unrecognized_object = nullptr)
{
  if (v.isString())
  {
    return cxx17::string_view(v.toString());
  }
  else if (v.isObject())
  {
    uint8_t* data = nullptr;
    size_t size   = 0;
    auto obj      = v.toObject();
    if (obj->isArrayBuffer())
      obj->getArrayBufferData(&data, &size);
    else if (obj->isTypedArray())
      obj->getTypedArrayData(&data, &size);
    else if (unrecognized_object)
      *unrecognized_object = true;
    if (data != nullptr)
      return cxx17::string_view((const char*)data, size);
  }
  return {};
}

//////////////////// common template functions //////////////

template <typename T> static bool jsb_yasio__ctor(se::State& s)
{
  auto cobj = new (std::nothrow) T();
  s.thisObject()->setPrivateData(cobj);
  se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
  return true;
}

template <typename T> static bool jsb_yasio__dtor(se::State& s)
{
  auto iter = se::NonRefNativePtrCreatedByCtorMap::find(s.nativeThisObject());
  if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
  {
    CC_LOG_DEBUG("jsbindings: finalizing JS object(created by ctor) %p(%s)", s.nativeThisObject(),
          typeid(T).name());
    se::NonRefNativePtrCreatedByCtorMap::erase(iter);
    T* cobj = reinterpret_cast<T*>(s.nativeThisObject());
    delete cobj;
  }
  else
  { // finalize not created by ctor
    auto iter2 = se::NativePtrToObjectMap::find(s.nativeThisObject());
    if (iter2 != se::NativePtrToObjectMap::end())
    {
      CC_LOG_DEBUG("jsbindings: finalizing JS object(created by native) %p(%s)", s.nativeThisObject(),
            typeid(T).name());
      T* cobj = reinterpret_cast<T*>(s.nativeThisObject());
      delete cobj;
    }
  }
  return true;
}

///////////////////////// ibstream /////////////////////////////////

static auto js_yasio_ibstream__dtor = jsb_yasio__dtor<yasio::ibstream>;
SE_BIND_FINALIZE_FUNC(js_yasio_ibstream__dtor)

static bool js_yasio_ibstream_read_bool(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setBoolean(cobj->read<bool>());

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_read_bool)

static bool js_yasio_ibstream_read_ix(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setInt32(cobj->read_ix<int>());

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_read_ix)

// for int8_t, int16_t, int32_t
template <typename T> static bool js_yasio_ibstream_read(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setInt32(cobj->read<T>());

  return true;
}
static auto js_yasio_ibstream_read_i8  = js_yasio_ibstream_read<int8_t>;
static auto js_yasio_ibstream_read_i16 = js_yasio_ibstream_read<int16_t>;
static auto js_yasio_ibstream_read_i32 = js_yasio_ibstream_read<int32_t>;
SE_BIND_FUNC(js_yasio_ibstream_read_i8)
SE_BIND_FUNC(js_yasio_ibstream_read_i16)
SE_BIND_FUNC(js_yasio_ibstream_read_i32)

// for uint8_t, uint16_t, uint32_t
template <typename T> static bool js_yasio_ibstream_read_ux(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setUint32(cobj->read<T>());

  return true;
}
static auto js_yasio_ibstream_read_u8  = js_yasio_ibstream_read_ux<uint8_t>;
static auto js_yasio_ibstream_read_u16 = js_yasio_ibstream_read_ux<uint16_t>;
static auto js_yasio_ibstream_read_u32 = js_yasio_ibstream_read_ux<uint32_t>;
SE_BIND_FUNC(js_yasio_ibstream_read_u8)
SE_BIND_FUNC(js_yasio_ibstream_read_u16)
SE_BIND_FUNC(js_yasio_ibstream_read_u32)

// for int64_t, float, double
template <typename T> static bool js_yasio_ibstream_read_dx(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setNumber(double(cobj->read<T>()));

  return true;
}
static auto js_yasio_ibstream_read_i64 = js_yasio_ibstream_read_dx<int64_t>;
static auto js_yasio_ibstream_read_f   = js_yasio_ibstream_read_dx<float>;
static auto js_yasio_ibstream_read_lf  = js_yasio_ibstream_read_dx<double>;
SE_BIND_FUNC(js_yasio_ibstream_read_i64)
SE_BIND_FUNC(js_yasio_ibstream_read_f)
SE_BIND_FUNC(js_yasio_ibstream_read_lf)

static bool js_yasio_ibstream_read_v(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  int length_field_bits = -1; // default: use variant length of length field
  bool raw              = false;
  if (argc >= 1)
    length_field_bits = args[0].toInt32();
  if (argc >= 2)
    raw = args[1].toBoolean();

  cxx17::string_view sv;
  switch (length_field_bits)
  {
    case -1: // variant bits
    default:
      sv = cobj->read_v();
      break;
    case 32: // 32bits
      sv = cobj->read_v32();
      break;
    case 16: // 16bits
      sv = cobj->read_v16();
      break;
  }

  if (!raw)
    s.rval().setString(std::string(sv.data(), sv.length()));
  else
  {
    se::HandleObject dataObj(se::Object::createArrayBufferObject((void*)sv.data(), sv.size()));
    s.rval().setObject(dataObj);
  }

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_read_v)

static bool js_yasio_ibstream_read_bytes(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();
  int n            = 0;
  if (argc >= 1)
    n = args[0].toInt32();
  if (n > 0)
  {
    auto sv = cobj->read_bytes(n);

    se::HandleObject dataObj(se::Object::createArrayBufferObject((void*)sv.data(), sv.size()));
    s.rval().setObject(dataObj);
  }
  else
    s.rval().setNull();
  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_read_bytes)

static bool js_yasio_ibstream_length(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");

  s.rval().setInt32(cobj->length());

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_length)

static bool js_yasio_ibstream_seek(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();
  if (argc >= 2)
    s.rval().setInt32(cobj->seek(args[0].toInt32(), args[1].toInt32()));
  else
    s.rval().setInt32(-1);

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_seek)

static bool js_yasio_ibstream_to_string(se::State& s)
{
  yasio::ibstream* cobj = (yasio::ibstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");

  s.rval().setString(std::string(cobj->data(), cobj->length()));

  return true;
}
SE_BIND_FUNC(js_yasio_ibstream_to_string)

void js_register_yasio_ibstream(se::Object* obj)
{
  auto cls = se::Class::create("ibstream", obj, nullptr, nullptr);

#define DEFINE_IBSTREAM_FUNC(funcName)                                                             \
  cls->defineFunction(#funcName, _SE(js_yasio_ibstream_##funcName))

  DEFINE_IBSTREAM_FUNC(read_bool);
  DEFINE_IBSTREAM_FUNC(read_ix);
  DEFINE_IBSTREAM_FUNC(read_i8);
  DEFINE_IBSTREAM_FUNC(read_i16);
  DEFINE_IBSTREAM_FUNC(read_i32);
  DEFINE_IBSTREAM_FUNC(read_i64);
  DEFINE_IBSTREAM_FUNC(read_u8);
  DEFINE_IBSTREAM_FUNC(read_u16);
  DEFINE_IBSTREAM_FUNC(read_u32);
  DEFINE_IBSTREAM_FUNC(read_f);
  DEFINE_IBSTREAM_FUNC(read_lf);
  DEFINE_IBSTREAM_FUNC(read_v);
  DEFINE_IBSTREAM_FUNC(read_bytes);
  DEFINE_IBSTREAM_FUNC(length);
  DEFINE_IBSTREAM_FUNC(seek);
  DEFINE_IBSTREAM_FUNC(to_string);
  cls->defineFinalizeFunction(_SE(js_yasio_ibstream__dtor));
  cls->install();
  JSBClassType::registerClass<yasio::ibstream>(cls);

  se::ScriptEngine::getInstance()->clearException();
}

///////////////////////// obstream /////////////////////////////////
se::Object* __jsb_yasio_obstream_proto = nullptr;
se::Class* __jsb_yasio_obstream_class  = nullptr;
static auto js_yasio_obstream__dtor    = jsb_yasio__dtor<yasio::obstream>;
SE_BIND_FINALIZE_FUNC(js_yasio_obstream__dtor)

static bool jsb_yasio_obstream__ctor(se::State& s)
{
  const auto& args = s.args();
  size_t argc      = args.size();

  yasio::obstream* cobj = nullptr;
  if (argc == 0)
  {
    cobj = new (std::nothrow) yasio::obstream();
  }
  else
  {
    auto arg0 = args[0];
    if (arg0.isNumber())
      cobj = new (std::nothrow) yasio::obstream(arg0.toUint32());
    else
      cobj = new (std::nothrow) yasio::obstream();
  }

  s.thisObject()->setPrivateData(cobj);
  se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);

  return true;
}
SE_BIND_CTOR(jsb_yasio_obstream__ctor, __jsb_yasio_obstream_class, js_yasio_obstream__dtor)

static bool js_yasio_obstream_push32(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->push32();

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_push32)

static bool js_yasio_obstream_pop32(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  if (argc == 0)
  {
    cobj->pop32();
  }
  else if (argc == 1)
  {
    cobj->pop32(args[0].toInt32());
  }

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_pop32)

static bool js_yasio_obstream_push16(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->push16();

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_push16)

static bool js_yasio_obstream_pop16(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  if (argc == 0)
  {
    cobj->pop16();
  }
  else if (argc == 1)
  {
    cobj->pop16(args[0].toInt32());
  }

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_pop16)

static bool js_yasio_obstream_push8(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->push8();

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_push8)

static bool js_yasio_obstream_pop8(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  if (argc == 0)
  {
    cobj->pop8();
  }
  else if (argc == 1)
  {
    cobj->pop8(args[0].toInt32());
  }

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_pop8)

static bool js_yasio_obstream_write_bool(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->write<bool>(args[0].toBoolean());

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_write_bool)

static bool js_yasio_obstream_write_ix(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->write_ix(args[0].toInt32());

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_write_ix)

template <typename T> static bool js_yasio_obstream_write(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  cobj->write<T>(args[0].toUint32());

  s.rval().setUndefined();

  return true;
}
static auto js_yasio_obstream_write_i8  = js_yasio_obstream_write<uint8_t>;
static auto js_yasio_obstream_write_i16 = js_yasio_obstream_write<uint16_t>;
static auto js_yasio_obstream_write_i32 = js_yasio_obstream_write<uint32_t>;
SE_BIND_FUNC(js_yasio_obstream_write_i8)
SE_BIND_FUNC(js_yasio_obstream_write_i16)
SE_BIND_FUNC(js_yasio_obstream_write_i32)

template <typename T> static bool js_yasio_obstream_write_dx(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  double argval = 0;
  seval_to_double(args[0], &argval);
  cobj->write<T>(static_cast<T>(argval));

  s.rval().setUndefined();

  return true;
}
static auto js_yasio_obstream_write_i64 = js_yasio_obstream_write_dx<int64_t>;
static auto js_yasio_obstream_write_f   = js_yasio_obstream_write_dx<float>;
static auto js_yasio_obstream_write_lf  = js_yasio_obstream_write_dx<double>;
SE_BIND_FUNC(js_yasio_obstream_write_i64)
SE_BIND_FUNC(js_yasio_obstream_write_f)
SE_BIND_FUNC(js_yasio_obstream_write_lf)

bool js_yasio_obstream_write_v(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  auto sv = seval_to_string_view(args[0]);

  int length_field_bits = -1; // default: use variant length of length field
  if (argc >= 2)
    length_field_bits = args[1].toInt32();
  switch (length_field_bits)
  {
    case -1: // variant bits
    default:
      cobj->write_v(sv);
      break;
    case 32: // 32bits
      cobj->write_v32(sv);
      break;
    case 16: // 16bits
      cobj->write_v16(sv);
      break;
  }

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_write_v)

bool js_yasio_obstream_write_bytes(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  auto sv = seval_to_string_view(args[0]);

  cobj->write_bytes(sv);

  s.rval().setUndefined();

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_write_bytes)

bool js_yasio_obstream_length(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  auto length = cobj->length();
  s.rval().setUint32(length);

  return true;
}
SE_BIND_FUNC(js_yasio_obstream_length)

bool js_yasio_obstream_sub(se::State& s)
{
  auto cobj = (yasio::obstream*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    size_t offset = 0;
    size_t count  = -1;
    CC_BREAK_IF(argc == 0);

    if (argc >= 1)
    {
      offset = args[0].toInt32();
    }
    if (argc >= 2)
    {
      count = args[1].toInt32();
    }

    auto subobj = new (std::nothrow) yasio::obstream(cobj->sub(offset, count));
    native_ptr_to_seval<yasio::obstream>(subobj, &s.rval());

    return true;
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d", (int)argc);
  return false;
}
SE_BIND_FUNC(js_yasio_obstream_sub)

void js_register_yasio_obstream(se::Object* obj)
{
#define DEFINE_OBSTREAM_FUNC(funcName)                                                             \
  cls->defineFunction(#funcName, _SE(js_yasio_obstream_##funcName))
  auto cls = se::Class::create("obstream", obj, nullptr, _SE(jsb_yasio_obstream__ctor));

  DEFINE_OBSTREAM_FUNC(push32);
  DEFINE_OBSTREAM_FUNC(push16);
  DEFINE_OBSTREAM_FUNC(push8);
  DEFINE_OBSTREAM_FUNC(pop32);
  DEFINE_OBSTREAM_FUNC(pop16);
  DEFINE_OBSTREAM_FUNC(pop8);
  DEFINE_OBSTREAM_FUNC(write_bool);
  DEFINE_OBSTREAM_FUNC(write_ix);
  DEFINE_OBSTREAM_FUNC(write_i8);
  DEFINE_OBSTREAM_FUNC(write_i16);
  DEFINE_OBSTREAM_FUNC(write_i32);
  DEFINE_OBSTREAM_FUNC(write_i64);
  DEFINE_OBSTREAM_FUNC(write_f);
  DEFINE_OBSTREAM_FUNC(write_lf);
  DEFINE_OBSTREAM_FUNC(write_v);
  DEFINE_OBSTREAM_FUNC(write_bytes);
  DEFINE_OBSTREAM_FUNC(length);
  DEFINE_OBSTREAM_FUNC(sub);

  cls->defineFinalizeFunction(_SE(js_yasio_obstream__dtor));
  cls->install();
  JSBClassType::registerClass<yasio::obstream>(cls);
  __jsb_yasio_obstream_proto = cls->getProto();
  __jsb_yasio_obstream_class = cls;

  se::ScriptEngine::getInstance()->clearException();
}

///////////////////////// transport ///////////////////////////////
void js_register_yasio_transport(se::Object* obj)
{ // since the transport is managed by native, don't need gc, we just register a dummy for pointer
  // passing
  auto cls = se::Class::create("transport", obj, nullptr, nullptr);

  cls->install();
  JSBClassType::registerClass<io_transport>(cls);

  se::ScriptEngine::getInstance()->clearException();
}

///////////////////////// io_event /////////////////////////////////

static auto jsb_yasio_io_event__dtor = jsb_yasio__dtor<io_event>;
SE_BIND_FINALIZE_FUNC(jsb_yasio_io_event__dtor)

bool js_yasio_io_event_kind(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setInt32(cobj->kind());

  return true;
}
SE_BIND_FUNC(js_yasio_io_event_kind)

bool js_yasio_io_event_status(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setInt32(cobj->status());

  return true;
}
SE_BIND_FUNC(js_yasio_io_event_status)

bool js_yasio_io_event_packet(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  auto& packet = cobj->packet();

  if (!packet.empty())
  {
    bool raw  = false;
    bool copy = false;
    if (argc >= 1)
      raw = args[0].toBoolean();
    if (argc >= 2)
      copy = args[1].toBoolean();
    if (!raw)
      native_ptr_to_seval<yasio::ibstream>(
          !copy ? new yasio::ibstream(std::move(packet)) : new yasio::ibstream(packet), &s.rval());
    else
    {
      se::HandleObject dataObj(se::Object::createArrayBufferObject(packet.data(), packet.size()));
      s.rval().setObject(dataObj);
      if (!copy)
      {
        packet.clear();
        packet.shrink_to_fit();
      }
    }
  }
  else
  {
    s.rval().setNull();
  }

  return true;
}
SE_BIND_FUNC(js_yasio_io_event_packet)

bool js_yasio_io_event_transport(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  native_ptr_to_seval<io_transport>(cobj->transport(), &s.rval());
  return true;
}
SE_BIND_FUNC(js_yasio_io_event_transport)

bool js_yasio_io_event_cindex(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  s.rval().setInt32(cobj->cindex());

  return true;
}
SE_BIND_FUNC(js_yasio_io_event_cindex)

#if !defined(YASIO_MINIFY_EVENT)
bool js_yasio_io_event_timestamp(se::State& s)
{
  auto cobj = (io_event*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  s.rval().setNumber(cobj->timestamp());
  return true;
}
SE_BIND_FUNC(js_yasio_io_event_timestamp)
#endif

void js_register_yasio_io_event(se::Object* obj)
{
#define DEFINE_IO_EVENT_FUNC(funcName)                                                             \
  cls->defineFunction(#funcName, _SE(js_yasio_io_event_##funcName))

  auto cls = se::Class::create("io_event", obj, nullptr, nullptr);

  DEFINE_IO_EVENT_FUNC(kind);
  DEFINE_IO_EVENT_FUNC(status);
  DEFINE_IO_EVENT_FUNC(packet);
  DEFINE_IO_EVENT_FUNC(cindex);
  DEFINE_IO_EVENT_FUNC(transport);
#if !defined(YASIO_MINIFY_EVENT)
  DEFINE_IO_EVENT_FUNC(timestamp);
#endif
  cls->defineFinalizeFunction(_SE(jsb_yasio_io_event__dtor));
  cls->install();
  JSBClassType::registerClass<io_event>(cls);

  se::ScriptEngine::getInstance()->clearException();
}

/////////////// io_service //////////////////////

se::Object* __jsb_yasio_io_service_proto = nullptr;
se::Class* __jsb_yasio_io_service_class  = nullptr;

static bool jsb_yasio_io_service__ctor(se::State& s)
{
  io_service* cobj = nullptr;
  const auto& args = s.args();
  size_t argc      = args.size();

  if (argc == 1)
  {
    auto& arg0 = args[0]; // hostent or hostents
    if (arg0.isObject())
    {
      if (arg0.toObject()->isArray())
      {
        std::vector<inet::io_hostent> hostents;
        seval_to_std_vector_hostent(arg0, &hostents);
        cobj = new io_service(!hostents.empty() ? &hostents.front() : nullptr,
                              (std::max)((int)hostents.size(), 1));
      }
      else
      {
        inet::io_hostent ioh;
        seval_to_hostent(arg0, &ioh);
        cobj = new io_service(&ioh, 1);
      }
    }
    else if (arg0.isNumber())
      cobj = new io_service(arg0.toInt32());
  }
  else
    cobj = new (std::nothrow) io_service();

  if (cobj != nullptr)
  {
    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
  }
  else
    s.rval().setNull();
  return true;
}

static auto jsb_yasio_io_service__dtor = jsb_yasio__dtor<io_service>;
SE_BIND_FINALIZE_FUNC(jsb_yasio_io_service__dtor)
SE_BIND_CTOR(jsb_yasio_io_service__ctor, __jsb_yasio_io_service_class, jsb_yasio_io_service__dtor)

bool js_yasio_io_service_start(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 1)
    {
      auto& jsFunc = args[0]; // io_event cb
      CC_BREAK_IF(!jsFunc.toObject()->isFunction());

      se::Value jsThis(s.thisObject());
      jsThis.toObject()->attachObject(jsFunc.toObject());
      io_event_cb_t fnwrap = [=](inet::event_ptr event) {
        se::ValueArray invokeArgs;
        invokeArgs.resize(1);
        native_ptr_to_seval<io_event>(event.release(), &invokeArgs[0]);
        se::Object* thisObj = jsThis.isObject() ? jsThis.toObject() : nullptr;
        se::Object* funcObj = jsFunc.toObject();
        bool succeed        = funcObj->call(invokeArgs, thisObj);
        if (!succeed)
        {
          se::ScriptEngine::getInstance()->clearException();
        }
      };

      cobj->start(std::move(fnwrap));

      s.rval().setUndefined();
      return true;
    }
  } while (0);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_start)

bool js_yasio_io_service_stop(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");

  cobj->stop();
  return true;
}
SE_BIND_FUNC(js_yasio_io_service_stop)

bool js_yasio_io_service_open(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 2)
    {
      cobj->open(args[0].toInt32(), args[1].toInt32());
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_open)

bool js_yasio_io_service_is_open(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 1)
    {
      bool opened = false;
      auto& arg0  = args[0];
      if (arg0.isNumber())
      {
        opened = cobj->is_open(arg0.toInt32());
      }
      else if (arg0.isObject())
      {
        io_transport* transport = nullptr;
        seval_to_native_ptr<io_transport*>(arg0, &transport);
        if (transport != nullptr)
          opened = cobj->is_open(transport);
      }
      s.rval().setBoolean(opened);
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_is_open)

bool js_yasio_io_service_close(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 1)
    {
      auto& arg0 = args[0];
      if (arg0.isNumber())
      {
        cobj->close(arg0.toInt32());
      }
      else if (arg0.isObject())
      {
        io_transport* transport = nullptr;
        seval_to_native_ptr<io_transport*>(arg0, &transport);
        if (transport != nullptr)
          cobj->close(transport);
      }
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_close)

bool js_yasio_io_service_dispatch(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 1)
    {
      cobj->dispatch(args[0].toInt32());
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_dispatch)

bool js_yasio_io_service_set_option(se::State& s)
{
  auto service = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(service, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc >= 2)
    {
      auto& arg0 = args[0];
      auto opt   = arg0.toInt32();
      switch (opt)
      {
        case YOPT_C_REMOTE_HOST:
        case YOPT_C_LOCAL_HOST:
          service->set_option(opt, args[1].toInt32(), args[2].toString().c_str());
          break;
#if YASIO_VERSION_NUM >= 0x033100
        case YOPT_C_LFBFD_IBTS:
#endif
        case YOPT_C_LOCAL_PORT:
        case YOPT_C_REMOTE_PORT:
        case YOPT_C_KCP_CONV:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32());
          break;
        case YOPT_C_ENABLE_MCAST:
        case YOPT_C_LOCAL_ENDPOINT:
        case YOPT_C_REMOTE_ENDPOINT:
          service->set_option(opt, args[1].toInt32(), args[2].toString().c_str(),
                              args[3].toInt32());
          break;
        case YOPT_C_MOD_FLAGS:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(),
                              args[3].toInt32());
          break;
        case YOPT_S_TCP_KEEPALIVE:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(), args[3].toInt32());
          break;
        case YOPT_C_LFBFD_PARAMS:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(), args[3].toInt32(),
                              args[4].toInt32(), args[5].toInt32());
          break;
        case YOPT_S_EVENT_CB: {
          se::Value jsThis(s.thisObject());
          se::Value jsFunc(args[1]);
          jsThis.toObject()->attachObject(jsFunc.toObject());
          io_event_cb_t callback = [=](inet::event_ptr event) {
            se::ValueArray invokeArgs;
            invokeArgs.resize(1);
            native_ptr_to_seval<io_event>(event.release(), &invokeArgs[0]);
            se::Object* thisObj = jsThis.isObject() ? jsThis.toObject() : nullptr;
            se::Object* funcObj = jsFunc.toObject();
            bool succeed        = funcObj->call(invokeArgs, thisObj);
            if (!succeed)
            {
              se::ScriptEngine::getInstance()->clearException();
            }
          };
          service->set_option(opt, std::addressof(callback));
          break;
        }
        default:
          service->set_option(opt, args[1].toInt32());
      }
      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_set_option)

bool js_yasio_io_service_write(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 2)
    {
      auto& arg0 = args[0];
      auto& arg1 = args[1];

      io_transport* transport = nullptr;
      seval_to_native_ptr<io_transport*>(arg0, &transport);

      if (transport != nullptr)
      {
        bool unrecognized_object = false;
        auto data                = seval_to_string_view(arg1, &unrecognized_object);
        if (!data.empty())
          cobj->write(transport, std::vector<char>(data.data(), data.data() + data.size()));
        else if (unrecognized_object)
        {
          yasio::obstream* obs = nullptr;
          seval_to_native_ptr(arg1, &obs);
          if (obs)
            cobj->write(transport, obs->buffer());
        }
      }

      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_write)

bool js_yasio_io_service_write_to(se::State& s)
{
  auto cobj = (io_service*)s.nativeThisObject();
  SE_PRECONDITION2(cobj, false, ": Invalid Native Object");
  const auto& args = s.args();
  size_t argc      = args.size();

  do
  {
    if (argc == 4)
    {
      auto& arg0   = args[0];
      auto& arg1   = args[1];
      auto ip      = seval_to_string_view(args[2]);
      u_short port = args[3].toUint16();

      io_transport* transport = nullptr;
      seval_to_native_ptr<io_transport*>(arg0, &transport);

      if (transport != nullptr)
      {
        bool unrecognized_object = false;
        auto data                = seval_to_string_view(arg1, &unrecognized_object);
        if (!data.empty())
          cobj->write_to(transport, std::vector<char>(data.data(), data.data() + data.size()),
                         ip::endpoint{ip.data(), port});
        else if (unrecognized_object)
        {
          yasio::obstream* obs = nullptr;
          seval_to_native_ptr(arg1, &obs);
          if (obs)
            cobj->write_to(transport, obs->buffer(), ip::endpoint{ip.data(), port});
        }
      }

      return true;
    }
  } while (false);

  SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
  return false;
}
SE_BIND_FUNC(js_yasio_io_service_write_to)

void js_register_yasio_io_service(se::Object* obj)
{
#define DEFINE_IO_SERVICE_FUNC(funcName)                                                           \
  cls->defineFunction(#funcName, _SE(js_yasio_io_service_##funcName))
  auto cls = se::Class::create("io_service", obj, nullptr, _SE(jsb_yasio_io_service__ctor));

  DEFINE_IO_SERVICE_FUNC(set_option);
  DEFINE_IO_SERVICE_FUNC(start);
  DEFINE_IO_SERVICE_FUNC(stop);
  DEFINE_IO_SERVICE_FUNC(open);
  DEFINE_IO_SERVICE_FUNC(close);
  DEFINE_IO_SERVICE_FUNC(is_open);
  DEFINE_IO_SERVICE_FUNC(dispatch);
  DEFINE_IO_SERVICE_FUNC(write);
  DEFINE_IO_SERVICE_FUNC(write_to);

  cls->defineFinalizeFunction(_SE(jsb_yasio_io_service__dtor));

  cls->install();
  JSBClassType::registerClass<io_service>(cls);

  __jsb_yasio_io_service_proto = cls->getProto();
  __jsb_yasio_io_service_class = cls;

  se::ScriptEngine::getInstance()->clearException();
}

bool jsb_register_yasio(se::Object* obj)
{
  // Get the ns
  se::Value nsVal;
  if (!obj->getProperty("yasio", &nsVal))
  {
    se::HandleObject jsobj(se::Object::createPlainObject());
    nsVal.setObject(jsobj);
    obj->setProperty("yasio", nsVal);
  }
  se::Object* yasio = nsVal.toObject();
  yasio->defineFunction("setTimeout", _SE(jsb_yasio_setTimeout));
  yasio->defineFunction("setInterval", _SE(jsb_yasio_setInterval));
  yasio->defineFunction("clearTimeout", _SE(jsb_yasio_killTimer));
  yasio->defineFunction("clearInterval", _SE(jsb_yasio_killTimer));
  yasio->defineFunction("highp_clock", _SE(jsb_yasio_highp_clock));
  yasio->defineFunction("highp_time", _SE(jsb_yasio_highp_time));

  js_register_yasio_ibstream(yasio);
  js_register_yasio_obstream(yasio);
  js_register_yasio_transport(yasio);
  js_register_yasio_io_event(yasio);
  js_register_yasio_io_service(yasio);

  // ##-- yasio enums
  se::Value __jsvalIntVal;
#define YASIO_SET_INT_PROP(name, value)                                                            \
  __jsvalIntVal.setInt32(value);                                                                   \
  yasio->setProperty(name, __jsvalIntVal)
#define YASIO_EXPORT_ENUM(v) YASIO_SET_INT_PROP(#v, v)
  YASIO_EXPORT_ENUM(YCK_TCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_TCP_SERVER);
  YASIO_EXPORT_ENUM(YCK_UDP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_UDP_SERVER);
#if defined(YASIO_HAVE_KCP)
  YASIO_EXPORT_ENUM(YCK_KCP_CLIENT);
  YASIO_EXPORT_ENUM(YCK_KCP_SERVER);
#endif
#if defined(YASIO_SSL_BACKEND)
  YASIO_EXPORT_ENUM(YCK_SSL_CLIENT);
#endif

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

  YASIO_EXPORT_ENUM(YEK_ON_OPEN);
  YASIO_EXPORT_ENUM(YEK_ON_CLOSE);
  YASIO_EXPORT_ENUM(YEK_ON_PACKET);

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);
  return true;
}
