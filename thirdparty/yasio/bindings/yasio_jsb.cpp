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

#include "yasio/bindings/yasio_jsb.hpp"
#include "yasio/yasio.hpp"
#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/detail/ref_ptr.hpp"

#include "cocos2d.h"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
using namespace yasio;
using namespace yasio::inet;
using namespace cocos2d;

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
typedef std::function<void()> vcallback_t;

struct TimerObject
{
  TimerObject(vcallback_t&& callback) : callback_(std::move(callback)), referenceCount_(1) {}

  vcallback_t callback_;
  static uintptr_t s_timerId;

  DEFINE_OBJECT_POOL_ALLOCATION(TimerObject, 128)
  YASIO_DEFINE_REFERENCE_CLASS
};
uintptr_t TimerObject::s_timerId = 0;

static TIMER_ID loop(unsigned int n, float interval, vcallback_t callback)
{
  if (n > 0 && interval >= 0)
  {
    yasio::gc::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));

    auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

    std::string key = StringUtils::format("STMR#%p", timerId);

    Director::getInstance()->getScheduler()->schedule(
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

    std::string key = StringUtils::format("STMR#%p", timerId);
    Director::getInstance()->getScheduler()->schedule(
        [timerObj](
            float /*dt*/) { // lambda expression hold the reference of timerObj automatically.
          timerObj->callback_();
        },
        STIMER_TARGET_VALUE, 0, 0, delay, false, key);

    return timerId;
  }
  return nullptr;
}

static void kill(TIMER_ID timerId)
{
  std::string key = StringUtils::format("STMR#%p", timerId);
  Director::getInstance()->getScheduler()->unschedule(key, STIMER_TARGET_VALUE);
}
void clear()
{
  Director::getInstance()->getScheduler()->unscheduleAllForTarget(STIMER_TARGET_VALUE);
}
} // namespace stimer

class string_view_adapter
{
public:
  string_view_adapter() : _data(nullptr), _size(0), _need_free(false) {}
  ~string_view_adapter()
  {
    if (_need_free)
      JS_free(ScriptingCore::getInstance()->getGlobalContext(), (void*)_data);
  }

  void set(JS::HandleValue val, JSContext* ctx, bool* unrecognized_object = nullptr)
  {
    clear();
    if (val.isString())
    {
      this->set(val.toString(), ctx);
    }
    else if (val.isObject())
    {
      JS::RootedObject jsobj(ctx, val.toObjectOrNull());
      if (JS_IsArrayBufferObject(jsobj))
      {
        _data = (char*)JS_GetArrayBufferData(jsobj);
        _size = JS_GetArrayBufferByteLength(jsobj);
      }
      else if (JS_IsArrayBufferViewObject(jsobj))
      {
        _data = (char*)JS_GetArrayBufferViewData(jsobj);
        _size = JS_GetArrayBufferViewByteLength(jsobj);
      }
      else if (unrecognized_object)
        *unrecognized_object = true;
    }
  }

  const char* data() { return _data; }
  size_t size() { return _size; }

  bool empty() const { return _data != nullptr && _size > 0; }

  operator cxx17::string_view() const
  {
    return cxx17::string_view(_data != nullptr ? _data : "", _size);
  }

  void clear()
  {
    if (_data && _need_free)
      JS_free(ScriptingCore::getInstance()->getGlobalContext(), (void*)_data);
    _data      = nullptr;
    _size      = 0;
    _need_free = false;
  }

protected:
  void set(JSString* str, JSContext* ctx)
  {
    if (!ctx)
    {
      ctx = ScriptingCore::getInstance()->getGlobalContext();
    }
    JS::RootedString jsstr(ctx, str);
    _data      = JS_EncodeStringToUTF8(ctx, jsstr);
    _size      = JS_GetStringEncodingLength(ctx, jsstr);
    _need_free = true;
  }

protected:
  char* _data;
  size_t _size;
  bool _need_free;
};
jsval createJSArrayBuffer(JSContext* ctx, const void* data, size_t size)
{
  JS::RootedObject buffer(ctx, JS_NewArrayBuffer(ctx, static_cast<uint32_t>(size)));
  uint8_t* bufdata = JS_GetArrayBufferData(buffer);
  memcpy((void*)bufdata, (void*)data, size);
  return OBJECT_TO_JSVAL(buffer);
}
} // namespace yasio_jsb

bool jsval_to_hostent(JSContext* ctx, JS::HandleValue vp, inet::io_hostent* ret)
{
  JS::RootedObject tmp(ctx);
  JS::RootedValue jsport(ctx);
  JS::RootedValue jsip(ctx);
  uint16_t port;
  bool ok = vp.isObject() && JS_ValueToObject(ctx, vp, &tmp) &&
            JS_GetProperty(ctx, tmp, "port", &jsport) && JS_GetProperty(ctx, tmp, "host", &jsip) &&
            JS::ToUint16(ctx, jsport, &port);

  JSB_PRECONDITION3(ok, ctx, false, "Error processing arguments");

  ret->port_ = port;
  jsval_to_std_string(ctx, jsip, &ret->host_);
  return true;
}

bool jsval_to_std_vector_hostent(JSContext* ctx, JS::HandleValue vp,
                                 std::vector<inet::io_hostent>* ret)
{
  JS::RootedObject jsobj(ctx);
  bool ok = vp.isObject() && JS_ValueToObject(ctx, vp, &jsobj);
  JSB_PRECONDITION3(ok, ctx, false, "Error converting value to object");
  JSB_PRECONDITION3(jsobj && JS_IsArrayObject(ctx, jsobj), ctx, false, "Object must be an array");

  uint32_t len = 0;
  JS_GetArrayLength(ctx, jsobj, &len);
  ret->reserve(len);
  for (uint32_t i = 0; i < len; i++)
  {
    JS::RootedValue value(ctx);
    if (JS_GetElement(ctx, jsobj, i, &value))
    {
      if (value.isObject())
      {
        inet::io_hostent ioh;
        jsval_to_hostent(ctx, value, &ioh);
        ret->push_back(std::move(ioh));
      }
      else
      {
        JS_ReportError(ctx, "not supported type in array");
        return false;
      }
    }
  }

  return true;
}

template <typename T> static bool jsb_yasio__ctor(JSContext* ctx, uint32_t argc, jsval* vp)
{
  JS::CallArgs args          = JS::CallArgsFromVp(argc, vp);
  bool ok                    = true;
  auto cobj                  = new (std::nothrow) T();
  js_type_class_t* typeClass = js_get_type_from_native<T>(cobj);

  // link the native object with the javascript object
  JS::RootedObject jsobj(ctx,
                         jsb_create_weak_jsobject(ctx, cobj, typeClass, TypeTest<T>::s_name()));
  args.rval().set(OBJECT_TO_JSVAL(jsobj));
  if (JS_HasProperty(ctx, jsobj, "_ctor", &ok) && ok)
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(jsobj), "_ctor", args);
  return true;
}

template <typename T> static void jsb_yasio__dtor(JSFreeOp* fop, JSObject* obj)
{
  CCLOG("jsbindings: finalizing JS object %p(%s)", obj, TypeTest<T>::s_name());
  js_proxy_t* nproxy;
  js_proxy_t* jsproxy;
  JSContext* ctx = ScriptingCore::getInstance()->getGlobalContext();
  JS::RootedObject jsobj(ctx, obj);
  jsproxy = jsb_get_js_proxy(jsobj);
  if (jsproxy)
  {
    auto nobj = static_cast<T*>(jsproxy->ptr);
    nproxy    = jsb_get_native_proxy(jsproxy->ptr);

    if (nobj)
    {
      jsb_remove_proxy(nproxy, jsproxy);
      JS::RootedValue flagValue(ctx);
      JS_GetProperty(ctx, jsobj, "__cppCreated", &flagValue);
      if (flagValue.isNullOrUndefined())
      {
        delete nobj;
      }
    }
    else
      jsb_remove_proxy(nullptr, jsproxy);
  }
}

template <typename T> static jsval jsb_yasio_to_jsval(JSContext* ctx, std::unique_ptr<T> value)
{
  auto cobj                  = value.release();
  js_type_class_t* typeClass = js_get_type_from_native<T>(cobj);

  // link the native object with the javascript object
  JS::RootedObject jsobj(ctx,
                         jsb_create_weak_jsobject(ctx, cobj, typeClass, TypeTest<T>::s_name()));

  return OBJECT_TO_JSVAL(jsobj);
}

static jsval jsb_yasio_to_jsval(JSContext* ctx, io_transport* value)
{
  js_type_class_t* typeClass = js_get_type_from_native<io_transport>(value);

  // link the native object with the javascript object
  JS::RootedObject jsobj(
      ctx, jsb_create_weak_jsobject(ctx, value, typeClass, TypeTest<io_transport>::s_name()));

  return OBJECT_TO_JSVAL(jsobj);
}

static io_transport* jsb_yasio_jsval_to_io_transport(JSContext* ctx, const JS::HandleValue& vp)
{
  JS::RootedObject jsobj(ctx);
  jsobj.set(vp.toObjectOrNull());
  auto proxy = jsb_get_js_proxy(jsobj);
  return (io_transport*)(proxy ? proxy->ptr : nullptr);
}

static yasio::obstream* jsb_yasio_jsval_to_obstram(JSContext* ctx, const JS::HandleValue& vp)
{
  JS::RootedObject jsobj(ctx);
  jsobj.set(vp.toObjectOrNull());
  auto proxy = jsb_get_js_proxy(jsobj);
  return (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
}

/////////////// javascript like setTimeout, clearTimeout setInterval, clearInterval ///////////
bool jsb_yasio_setTimeout(JSContext* ctx, uint32_t argc, jsval* vp)
{
  do
  {
    if (argc == 2)
    {
      auto args = JS::CallArgsFromVp(argc, vp);
      auto arg0 = args.get(0);
      auto arg1 = args.get(1);
      CC_BREAK_IF((JS_TypeOfValue(ctx, arg0) != JSTYPE_FUNCTION));

      JS::RootedObject jstarget(ctx, args.thisv().toObjectOrNull());
      auto func = std::make_shared<JSFunctionWrapper>(ctx, jstarget, arg0, args.thisv());
      yasio_jsb::stimer::vcallback_t callback = [=]() {
        JS::RootedValue rval(ctx);
        bool succeed = func->invoke(0, nullptr, &rval);
        if (!succeed && JS_IsExceptionPending(ctx))
        {
          JS_ReportPendingException(ctx);
        }
      };

      double timeout = 0;
      JS::ToNumber(ctx, arg1, &timeout);
      auto timerId = yasio_jsb::stimer::delay(timeout, std::move(callback));

      args.rval().set(PRIVATE_TO_JSVAL(timerId));
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "jsb_yasio_setTimeout: wrong number of arguments: %d, was expecting %d", argc,
                 0);
  return false;
}

bool jsb_yasio_setInterval(JSContext* ctx, uint32_t argc, jsval* vp)
{
  do
  {
    if (argc == 2)
    {
      auto args = JS::CallArgsFromVp(argc, vp);
      auto arg0 = args.get(0);
      auto arg1 = args.get(1);
      CC_BREAK_IF((JS_TypeOfValue(ctx, arg0) != JSTYPE_FUNCTION));

      JS::RootedObject jstarget(ctx, args.thisv().toObjectOrNull());
      auto func = std::make_shared<JSFunctionWrapper>(ctx, jstarget, arg0, args.thisv());
      yasio_jsb::stimer::vcallback_t callback = [=]() {
        JS::RootedValue rval(ctx);
        bool succeed = func->invoke(0, nullptr, &rval);
        if (!succeed && JS_IsExceptionPending(ctx))
        {
          JS_ReportPendingException(ctx);
        }
      };

      double interval = 0;
      JS::ToNumber(ctx, arg1, &interval);
      auto timerId = yasio_jsb::stimer::loop((std::numeric_limits<unsigned int>::max)(), interval,
                                             std::move(callback));

      args.rval().set(PRIVATE_TO_JSVAL(timerId));
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "jsb_yasio_setInterval: wrong number of arguments: %d, was expecting %d",
                 argc, 0);
  return false;
}

bool jsb_yasio_killTimer(JSContext* ctx, uint32_t argc, jsval* vp)
{
  do
  {
    if (argc == 1)
    {
      auto args = JS::CallArgsFromVp(argc, vp);
      auto arg0 = args.get(0);
      void* id  = arg0.get().toPrivate();

      yasio_jsb::stimer::kill(id);

      args.rval().setUndefined();
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "jsb_yasio_setTimeout: wrong number of arguments: %d, was expecting %d", argc,
                 0);
  return false;
}

bool jsb_yasio_highp_clock(JSContext* ctx, uint32_t argc, jsval* vp)
{
  auto args = JS::CallArgsFromVp(argc, vp);

  args.rval().setDouble(yasio::highp_clock());
  return true;
}

bool jsb_yasio_highp_time(JSContext* ctx, uint32_t argc, jsval* vp)
{
  auto args = JS::CallArgsFromVp(argc, vp);

  args.rval().setDouble(yasio::highp_clock<yasio::system_clock_t>());
  return true;
}

///////////////////////// ibstream /////////////////////////////////
JSClass* jsb_ibstream_class;
JSObject* jsb_ibstream_prototype;

static bool js_yasio_ibstream_read_bool(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_bool : Invalid Native Object");

  args.rval().set(BOOLEAN_TO_JSVAL(cobj->read<bool>()));

  return true;
}

// for variant int
static bool js_yasio_ibstream_read_ix(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_ix : Invalid Native Object");

  args.rval().set(INT_TO_JSVAL(cobj->read_ix<int>()));

  return true;
}

// for int8_t, int16_t, int32_t
template <typename T>
static bool js_yasio_ibstream_read(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read : Invalid Native Object");

  args.rval().set(INT_TO_JSVAL(cobj->read<T>()));

  return true;
}

template <typename T>
static bool js_yasio_ibstream_read_ux(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_ux : Invalid Native Object");

  args.rval().set(UINT_TO_JSVAL(cobj->read<T>()));

  return true;
}

// for int64_t, float, double
template <typename T>
static bool js_yasio_ibstream_read_dx(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_dx : Invalid Native Object");

  args.rval().set(DOUBLE_TO_JSVAL(cobj->read<T>()));

  return true;
}

static bool js_yasio_ibstream_read_v(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_v : Invalid Native Object");

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
    args.rval().set(c_string_to_jsval(ctx, sv.data(), sv.size()));
  else
    args.rval().set(yasio_jsb::createJSArrayBuffer(ctx, sv.data(), sv.size()));

  return true;
}

static bool js_yasio_ibstream_read_bytes(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_read_bytes : Invalid Native Object");

  int n = 0;
  if (argc >= 1)
    n = args[0].toInt32();
  if (n > 0)
  {
    auto sv = cobj->read_bytes(n);
    args.rval().set(yasio_jsb::createJSArrayBuffer(ctx, sv.data(), sv.size()));
  }
  else
    args.rval().setNull();

  return true;
}

static bool js_yasio_ibstream_seek(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_seek : Invalid Native Object");

  int ret = -1;
  if (argc >= 2)
    ret = cobj->seek(args[0].toInt32(), args[1].toInt32());

  args.rval().set(uint32_to_jsval(ctx, ret));

  return true;
}

static bool js_yasio_ibstream_length(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::ibstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::ibstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_ibstream_length : Invalid Native Object");

  args.rval().set(uint32_to_jsval(ctx, cobj->length()));

  return true;
}

void js_register_yasio_ibstream(JSContext* ctx, JS::HandleObject global)
{
  jsb_ibstream_class              = (JSClass*)calloc(1, sizeof(JSClass));
  jsb_ibstream_class->name        = "ibstream";
  jsb_ibstream_class->addProperty = JS_PropertyStub;
  jsb_ibstream_class->delProperty = JS_DeletePropertyStub;
  jsb_ibstream_class->getProperty = JS_PropertyStub;
  jsb_ibstream_class->setProperty = JS_StrictPropertyStub;
  jsb_ibstream_class->enumerate   = JS_EnumerateStub;
  jsb_ibstream_class->resolve     = JS_ResolveStub;
  jsb_ibstream_class->convert     = JS_ConvertStub;
  jsb_ibstream_class->finalize    = jsb_yasio__dtor<yasio::ibstream>;
  jsb_ibstream_class->flags       = JSCLASS_HAS_RESERVED_SLOTS(2);

  static JSPropertySpec properties[] = {JS_PS_END};

  static JSFunctionSpec funcs[] = {
      JS_FN("read_bool", js_yasio_ibstream_read_bool, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_ix", js_yasio_ibstream_read_ix, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_i8", js_yasio_ibstream_read<int8_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_i16", js_yasio_ibstream_read<int16_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_i32", js_yasio_ibstream_read<int32_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_u8", js_yasio_ibstream_read_ux<uint8_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_u16", js_yasio_ibstream_read_ux<uint16_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_u32", js_yasio_ibstream_read_ux<uint32_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_i64", js_yasio_ibstream_read_dx<int64_t>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_f", js_yasio_ibstream_read_dx<float>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_lf", js_yasio_ibstream_read_dx<double>, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_v", js_yasio_ibstream_read_v, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("read_bytes", js_yasio_ibstream_read_bytes, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("seek", js_yasio_ibstream_seek, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("length", js_yasio_ibstream_length, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FS_END};

  static JSFunctionSpec st_funcs[] = {JS_FS_END};

  jsb_ibstream_prototype =
      JS_InitClass(ctx, global, JS::NullPtr(), jsb_ibstream_class, nullptr, 0, properties, funcs,
                   NULL, // no static properties
                   st_funcs);

  // add the proto and JSClass to the type->js info hash table
  JS::RootedObject proto(ctx, jsb_ibstream_prototype);
  // JS_SetProperty(ctx, proto, "_className", className);
  JS_SetProperty(ctx, proto, "__nativeObj", JS::TrueHandleValue);
  JS_SetProperty(ctx, proto, "__is_ref", JS::FalseHandleValue);

  jsb_register_class<yasio::ibstream>(ctx, jsb_ibstream_class, proto, JS::NullPtr());
}

///////////////////////// obstream /////////////////////////////////
JSClass* jsb_obstream_class;
JSObject* jsb_obstream_prototype;

static bool jsb_yasio_obstream__ctor(JSContext* ctx, uint32_t argc, jsval* vp)
{
  JS::CallArgs args     = JS::CallArgsFromVp(argc, vp);
  bool ok               = true;
  yasio::obstream* cobj = nullptr;
  if (argc == 0)
  {
    cobj = new (std::nothrow) yasio::obstream();
  }
  else
  {
    auto arg0 = args.get(0);
    if (arg0.isInt32())
      cobj = new (std::nothrow) yasio::obstream(arg0.toInt32());
    else
      cobj = new (std::nothrow) yasio::obstream();
  }
  js_type_class_t* typeClass = js_get_type_from_native<yasio::obstream>(cobj);

  // link the native object with the javascript object
  JS::RootedObject jsobj(
      ctx, jsb_create_weak_jsobject(ctx, cobj, typeClass, TypeTest<yasio::obstream>::s_name()));
  args.rval().set(OBJECT_TO_JSVAL(jsobj));
  if (JS_HasProperty(ctx, jsobj, "_ctor", &ok) && ok)
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(jsobj), "_ctor", args);
  return true;
}

bool js_yasio_obstream_push32(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_push32 : Invalid Native Object");

  cobj->push32();

  args.rval().setUndefined();

  return true;
}
bool js_yasio_obstream_pop32(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_pop32 : Invalid Native Object");

  if (argc == 0)
  {
    cobj->pop32();
  }
  else if (argc == 1)
  {
    cobj->pop32(args.get(0).toInt32());
  }

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_push16(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_push16 : Invalid Native Object");

  cobj->push16();

  args.rval().setUndefined();

  return true;
}
bool js_yasio_obstream_pop16(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_pop16 : Invalid Native Object");

  if (argc == 0)
  {
    cobj->pop16();
  }
  else if (argc == 1)
  {
    cobj->pop16(args.get(0).toInt32());
  }

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_push8(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_push8 : Invalid Native Object");

  cobj->push8();

  args.rval().setUndefined();

  return true;
}
bool js_yasio_obstream_pop8(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_pop8 : Invalid Native Object");

  if (argc == 0)
  {
    cobj->pop8();
  }
  else if (argc == 1)
  {
    cobj->pop8(args.get(0).toInt32());
  }

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_bool(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_bool : Invalid Native Object");

  cobj->write<bool>(args.get(0).toBoolean());

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_ix(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_ix : Invalid Native Object");

  cobj->write_ix(args.get(0).toInt32());

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_i8(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_i8 : Invalid Native Object");

  cobj->write<uint8_t>(args.get(0).toInt32());

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_i16(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_i16 : Invalid Native Object");

  cobj->write<uint16_t>(args.get(0).toInt32());

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_i32(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_i32 : Invalid Native Object");

  cobj->write<uint32_t>(args.get(0).toInt32());

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_i64(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok               = true;
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_i64 : Invalid Native Object");

  double argval = 0;
  JS::ToNumber(ctx, args.get(0), &argval);
  cobj->write<int64_t>((int64_t)argval);

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_f(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_f : Invalid Native Object");

  double argval = 0;
  JS::ToNumber(ctx, args.get(0), &argval);
  cobj->write((float)argval);

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_lf(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_lf : Invalid Native Object");

  double argval = 0;
  JS::ToNumber(ctx, args.get(0), &argval);
  cobj->write(argval);

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_v(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_v : Invalid Native Object");

  auto arg0 = args.get(0);

  yasio_jsb::string_view_adapter sva;
  bool unrecognized_object = false;
  sva.set(arg0, ctx, &unrecognized_object);

  int length_field_bits = -1; // default: use variant length of length field
  if (argc >= 2)
    length_field_bits = args[1].toInt32();
  switch (length_field_bits)
  {
    case -1: // variant bits
      cobj->write_v(sva);
      break;
    case 32: // 32bits
      cobj->write_v32(sva);
      break;
    case 16: // 16bits
      cobj->write_v16(sva);
      break;
    default: // 8bits
      cobj->write_v8(sva);
  }

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_write_bytes(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_write_bytes : Invalid Native Object");

  auto arg0 = args.get(0);

  yasio_jsb::string_view_adapter sva;
  bool unrecognized_object = false;
  sva.set(arg0, ctx, &unrecognized_object);

  if (!sva.empty())
    cobj->write_bytes(sva);

  args.rval().setUndefined();

  return true;
}

bool js_yasio_obstream_length(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_length : Invalid Native Object");

  auto length = cobj->length();
  args.rval().set(uint32_to_jsval(ctx, length));

  return true;
}

bool js_yasio_obstream_sub(JSContext* ctx, uint32_t argc, jsval* vp)
{
  yasio::obstream* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (yasio::obstream*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_obstream_sub : Invalid Native Object");

  do
  {
    size_t offset = 0;
    size_t count  = -1;
    CC_BREAK_IF(argc == 0);

    if (argc >= 1)
    {
      offset = args.get(0).toInt32();
    }
    if (argc >= 2)
    {
      count = args.get(1).toInt32();
    }

    auto subobj                = new yasio::obstream(cobj->sub(offset, count));
    js_type_class_t* typeClass = js_get_type_from_native<yasio::obstream>(subobj);

    // link the native object with the javascript object
    JS::RootedObject jsobj(
        ctx, jsb_create_weak_jsobject(ctx, subobj, typeClass, TypeTest<yasio::obstream>::s_name()));
    args.rval().set(OBJECT_TO_JSVAL(jsobj));

    return true;
  } while (false);

  JS_ReportError(ctx, "js_yasio_obstream_sub : wrong number of arguments");
  return false;
}

void js_register_yasio_obstream(JSContext* ctx, JS::HandleObject global)
{
  jsb_obstream_class              = (JSClass*)calloc(1, sizeof(JSClass));
  jsb_obstream_class->name        = "obstream";
  jsb_obstream_class->addProperty = JS_PropertyStub;
  jsb_obstream_class->delProperty = JS_DeletePropertyStub;
  jsb_obstream_class->getProperty = JS_PropertyStub;
  jsb_obstream_class->setProperty = JS_StrictPropertyStub;
  jsb_obstream_class->enumerate   = JS_EnumerateStub;
  jsb_obstream_class->resolve     = JS_ResolveStub;
  jsb_obstream_class->convert     = JS_ConvertStub;
  jsb_obstream_class->finalize    = jsb_yasio__dtor<yasio::obstream>;
  jsb_obstream_class->flags       = JSCLASS_HAS_RESERVED_SLOTS(2);

  static JSPropertySpec properties[] = {JS_PS_END};

  static JSFunctionSpec funcs[] = {
      JS_FN("push32", js_yasio_obstream_push32, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("push16", js_yasio_obstream_push16, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("push8", js_yasio_obstream_push8, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("pop32", js_yasio_obstream_pop32, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("pop16", js_yasio_obstream_pop16, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("pop8", js_yasio_obstream_pop8, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_bool", js_yasio_obstream_write_bool, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_ix", js_yasio_obstream_write_ix, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_i8", js_yasio_obstream_write_i8, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_i16", js_yasio_obstream_write_i16, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_i32", js_yasio_obstream_write_i32, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_i64", js_yasio_obstream_write_i64, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_f", js_yasio_obstream_write_f, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_lf", js_yasio_obstream_write_lf, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_v", js_yasio_obstream_write_v, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_bytes", js_yasio_obstream_write_bytes, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("length", js_yasio_obstream_length, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("sub", js_yasio_obstream_sub, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FS_END};

  static JSFunctionSpec st_funcs[] = {JS_FS_END};

  // JS::RootedObject parentProto(ctx, jsb_cocos2d_Ref_prototype);
  jsb_obstream_prototype = JS_InitClass(ctx, global, JS::NullPtr(), jsb_obstream_class,
                                        jsb_yasio_obstream__ctor, 0, properties, funcs,
                                        NULL, // no static properties
                                        st_funcs);

  // add the proto and JSClass to the type->js info hash table
  JS::RootedObject proto(ctx, jsb_obstream_prototype);
  // JS_SetProperty(ctx, proto, "_className", className);
  JS_SetProperty(ctx, proto, "__nativeObj", JS::TrueHandleValue);
  JS_SetProperty(ctx, proto, "__is_ref", JS::FalseHandleValue);

  jsb_register_class<yasio::obstream>(ctx, jsb_obstream_class, proto, JS::NullPtr());
}

///////////////////////// transport ///////////////////////////////
JSClass* jsb_transport_class;
JSObject* jsb_transport_prototype;

void js_register_yasio_transport(JSContext* ctx, JS::HandleObject global)
{
  jsb_transport_class              = (JSClass*)calloc(1, sizeof(JSClass));
  jsb_transport_class->name        = "transport";
  jsb_transport_class->addProperty = JS_PropertyStub;
  jsb_transport_class->delProperty = JS_DeletePropertyStub;
  jsb_transport_class->getProperty = JS_PropertyStub;
  jsb_transport_class->setProperty = JS_StrictPropertyStub;
  jsb_transport_class->enumerate   = JS_EnumerateStub;
  jsb_transport_class->resolve     = JS_ResolveStub;
  jsb_transport_class->convert     = JS_ConvertStub;
  jsb_transport_class->flags       = JSCLASS_HAS_RESERVED_SLOTS(2);

  static JSPropertySpec properties[] = {JS_PS_END};

  static JSFunctionSpec funcs[] = {JS_FS_END};

  static JSFunctionSpec st_funcs[] = {JS_FS_END};

  jsb_transport_prototype =
      JS_InitClass(ctx, global, JS::NullPtr(), jsb_transport_class, nullptr, 0, properties, funcs,
                   NULL, // no static properties
                   st_funcs);

  // add the proto and JSClass to the type->js info hash table
  JS::RootedObject proto(ctx, jsb_transport_prototype);
  // JS_SetProperty(ctx, proto, "_className", className);
  JS_SetProperty(ctx, proto, "__nativeObj", JS::TrueHandleValue);
  JS_SetProperty(ctx, proto, "__is_ref", JS::FalseHandleValue);

  jsb_register_class<io_transport>(ctx, jsb_transport_class, proto, JS::NullPtr());
}

///////////////////////// io_event /////////////////////////////////
JSClass* jsb_io_event_class;
JSObject* jsb_io_event_prototype;

bool js_yasio_io_event_kind(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_kind : Invalid Native Object");

  args.rval().set(int32_to_jsval(ctx, cobj->kind()));

  return true;
}

bool js_yasio_io_event_status(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_status : Invalid Native Object");

  args.rval().set(int32_to_jsval(ctx, cobj->status()));

  return true;
}

bool js_yasio_io_event_packet(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_packet : Invalid Native Object");

  auto& packet = cobj->packet();

  if (!packet.empty())
  {
    bool raw  = false;
    bool copy = false;
    if (argc >= 1)
      raw = args[0].toBoolean();
    if (argc >= 1)
      copy = args[1].toBoolean();
    if (!raw)
    {
      std::unique_ptr<yasio::ibstream> ibs(!copy ? new yasio::ibstream(std::move(packet))
                                                 : new yasio::ibstream(packet));
      args.rval().set(jsb_yasio_to_jsval<yasio::ibstream>(ctx, std::move(ibs)));
    }
    else
    {
      args.rval().set(yasio_jsb::createJSArrayBuffer(ctx, packet.data(), packet.size()));
      if (!copy)
      {
        packet.clear();
        packet.shrink_to_fit();
      }
    }
  }
  else
  {
    args.rval().setNull();
  }

  return true;
}

bool js_yasio_io_event_cindex(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_cindex : Invalid Native Object");

  args.rval().set(int32_to_jsval(ctx, cobj->cindex()));

  return true;
}

bool js_yasio_io_event_transport(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_transport : Invalid Native Object");

  args.rval().set(jsb_yasio_to_jsval(ctx, cobj->transport()));

  return true;
}

#if !defined(YASIO_MINIFY_EVENT)
bool js_yasio_io_event_timestamp(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok        = true;
  io_event* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_event*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_event_timestamp : Invalid Native Object");

  args.rval().set(long_long_to_jsval(ctx, cobj->timestamp()));

  return true;
}
#endif

void js_register_yasio_io_event(JSContext* ctx, JS::HandleObject global)
{
  jsb_io_event_class              = (JSClass*)calloc(1, sizeof(JSClass));
  jsb_io_event_class->name        = "io_event";
  jsb_io_event_class->addProperty = JS_PropertyStub;
  jsb_io_event_class->delProperty = JS_DeletePropertyStub;
  jsb_io_event_class->getProperty = JS_PropertyStub;
  jsb_io_event_class->setProperty = JS_StrictPropertyStub;
  jsb_io_event_class->enumerate   = JS_EnumerateStub;
  jsb_io_event_class->resolve     = JS_ResolveStub;
  jsb_io_event_class->convert     = JS_ConvertStub;
  jsb_io_event_class->finalize    = jsb_yasio__dtor<io_event>;
  jsb_io_event_class->flags       = JSCLASS_HAS_RESERVED_SLOTS(2);

  static JSPropertySpec properties[] = {JS_PS_END};

  static JSFunctionSpec funcs[] = {
    JS_FN("kind", js_yasio_io_event_kind, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_FN("status", js_yasio_io_event_status, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_FN("packet", js_yasio_io_event_packet, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_FN("cindex", js_yasio_io_event_cindex, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_FN("transport", js_yasio_io_event_transport, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
#if !defined(YASIO_MINIFY_EVENT)
    JS_FN("timestamp", js_yasio_io_event_timestamp, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
#endif
    JS_FS_END
  };

  static JSFunctionSpec st_funcs[] = {JS_FS_END};

  jsb_io_event_prototype =
      JS_InitClass(ctx, global, JS::NullPtr(), jsb_io_event_class, nullptr, 0, properties, funcs,
                   NULL, // no static properties
                   st_funcs);

  // add the proto and JSClass to the type->js info hash table
  JS::RootedObject proto(ctx, jsb_io_event_prototype);
  // JS_SetProperty(ctx, proto, "_className", className);
  JS_SetProperty(ctx, proto, "__nativeObj", JS::TrueHandleValue);
  JS_SetProperty(ctx, proto, "__is_ref", JS::FalseHandleValue);

  jsb_register_class<io_event>(ctx, jsb_io_event_class, proto, JS::NullPtr());
}

/////////////// io_service //////////////////////

JSClass* jsb_io_service_class;
JSObject* jsb_io_service_prototype;

static bool jsb_yasio_io_service__ctor(JSContext* ctx, uint32_t argc, jsval* vp)
{
  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  bool ok           = true;

  io_service* cobj = nullptr;
  if (argc == 1)
  {
    auto arg0 = args.get(0);
    if (JS_IsArrayObject(ctx, arg0))
    {
      std::vector<inet::io_hostent> hostents;
      jsval_to_std_vector_hostent(ctx, arg0, &hostents);
      cobj = new (std::nothrow) io_service(!hostents.empty() ? &hostents.front() : nullptr,
                                           std::max(1, (int)hostents.size()));
    }
    else if (arg0.isObject())
    {
      inet::io_hostent ioh;
      jsval_to_hostent(ctx, arg0, &ioh);
      cobj = new (std::nothrow) io_service(&ioh, 1);
    }
    else if (arg0.isNumber())
      cobj = new (std::nothrow) io_service(args.get(0).toInt32());
  }
  else
  {
    cobj = new (std::nothrow) io_service();
  }

  if (cobj != nullptr)
  {
    js_type_class_t* typeClass = js_get_type_from_native<io_service>(cobj);

    // link the native object with the javascript object
    JS::RootedObject jsobj(
        ctx, jsb_create_weak_jsobject(ctx, cobj, typeClass, TypeTest<io_service>::s_name()));
    args.rval().set(OBJECT_TO_JSVAL(jsobj));
    if (JS_HasProperty(ctx, jsobj, "_ctor", &ok) && ok)
      ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(jsobj), "_ctor", args);
  }
  else
  {
    args.rval().setNull();
  }
  return false;
}

bool js_yasio_io_service_start(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_start : Invalid Native Object");

  do
  {
    if (argc == 1)
    {
      auto arg0 = args.get(0); // event cb
      CC_BREAK_IF((JS_TypeOfValue(ctx, arg0) != JSTYPE_FUNCTION));

      JS::RootedObject jstarget(ctx, args.thisv().toObjectOrNull());
      auto func            = std::make_shared<JSFunctionWrapper>(ctx, jstarget, arg0, args.thisv());
      io_event_cb_t fnwrap = [=](inet::event_ptr event) {
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        jsval jevent = jsb_yasio_to_jsval(ctx, std::move(event));
        JS::RootedValue rval(ctx);
        bool succeed = func->invoke(1, &jevent, &rval);
        if (!succeed && JS_IsExceptionPending(ctx))
        {
          JS_ReportPendingException(ctx);
        }
      };

      cobj->start(std::move(fnwrap));

      args.rval().setUndefined();
      return true;
    }
  } while (0);

  JS_ReportError(ctx, "js_yasio_io_service_start : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_stop(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_stop : Invalid Native Object");

  cobj->stop();
  return true;
}

bool js_yasio_io_service_open(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_open : Invalid Native Object");

  do
  {
    if (argc == 2)
    {
      cobj->open(args.get(0).toInt32(), args.get(1).toInt32());
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_open : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_is_open(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_is_open : Invalid Native Object");

  do
  {
    if (argc == 1)
    {
      bool opened = false;
      auto arg0   = args.get(0);
      if (arg0.isInt32())
      {
        opened = cobj->is_open(arg0.toInt32());
      }
      else if (arg0.isObject())
      {
        auto transport = jsb_yasio_jsval_to_io_transport(ctx, arg0);
        opened         = cobj->is_open(transport);
      }
      args.rval().set(BOOLEAN_TO_JSVAL(opened));
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_is_open : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_close(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_close : Invalid Native Object");

  do
  {
    if (argc == 1)
    {
      auto arg0 = args.get(0);
      if (arg0.isInt32())
      {
        cobj->close(arg0.toInt32());
      }
      else if (arg0.isObject())
      {
        auto transport = jsb_yasio_jsval_to_io_transport(ctx, arg0);
        cobj->close(transport);
      }
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_close : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_dispatch(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_dispatch : Invalid Native Object");

  do
  {
    if (argc == 1)
    {
      cobj->dispatch(args.get(0).toInt32());
      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_dispatch : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_set_option(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok             = true;
  io_service* service = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  service           = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(service, ctx, false, "js_yasio_io_service_set_option : Invalid Native Object");

  do
  {
    if (argc >= 2)
    {
      auto arg0 = args[0];
      auto opt  = arg0.toInt32();
      switch (opt)
      {
        case YOPT_C_LOCAL_HOST:
        case YOPT_C_REMOTE_HOST:
          if (args[2].isString())
          {
            JSStringWrapper str(args[2].toString());
            service->set_option(opt, args[1].toInt32(), str.get());
          }
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
          if (args[2].isString())
          {
            JSStringWrapper str(args[2].toString());
            service->set_option(opt, args[1].toInt32(), str.get(), args[3].toInt32());
          }
          break;
        case YOPT_C_MOD_FLAGS:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(), args[3].toInt32());
          break;
        case YOPT_S_TCP_KEEPALIVE:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(), args[3].toInt32());
          break;
        case YOPT_C_LFBFD_PARAMS:
          service->set_option(opt, args[1].toInt32(), args[2].toInt32(), args[3].toInt32(),
                              args[4].toInt32(), args[5].toInt32());
          break;
        case YOPT_S_EVENT_CB: {
          JS::RootedObject jstarget(ctx, args.thisv().toObjectOrNull());
          auto func = std::make_shared<JSFunctionWrapper>(ctx, jstarget, args[1], args.thisv());
          io_event_cb_t callback = [=](inet::event_ptr event) {
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            jsval jevent = jsb_yasio_to_jsval(ctx, std::move(event));
            JS::RootedValue rval(ctx);
            bool succeed = func->invoke(1, &jevent, &rval);
            if (!succeed && JS_IsExceptionPending(ctx))
            {
              JS_ReportPendingException(ctx);
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

  JS_ReportError(ctx, "js_yasio_io_service_set_option : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_write(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_write : Invalid Native Object");

  do
  {
    if (argc == 2)
    {
      auto arg0 = args.get(0);
      auto arg1 = args.get(1);

      auto transport = jsb_yasio_jsval_to_io_transport(ctx, arg0);

      yasio_jsb::string_view_adapter sva;
      bool unrecognized_object = false;
      sva.set(arg1, ctx, &unrecognized_object);
      if (!sva.empty())
      {
        cobj->write(transport, std::vector<char>(sva.data(), sva.data() + sva.size()));
      }
      else if (unrecognized_object)
      {
        auto obs = jsb_yasio_jsval_to_obstram(ctx, arg1);
        if (obs != nullptr)
        {
          auto& buffer = obs->buffer();
          if (!buffer.empty())
            cobj->write(transport, obs->buffer());
        }
      }

      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_write : wrong number of arguments");
  return false;
}

bool js_yasio_io_service_write_to(JSContext* ctx, uint32_t argc, jsval* vp)
{
  bool ok          = true;
  io_service* cobj = nullptr;

  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject obj(ctx);
  obj.set(args.thisv().toObjectOrNull());
  js_proxy_t* proxy = jsb_get_js_proxy(obj);
  cobj              = (io_service*)(proxy ? proxy->ptr : nullptr);
  JSB_PRECONDITION2(cobj, ctx, false, "js_yasio_io_service_write_to : Invalid Native Object");

  do
  {
    if (argc == 4)
    {
      auto arg0 = args.get(0);
      auto arg1 = args.get(1);
      yasio_jsb::string_view_adapter ip;
      ip.set(args.get(2), ctx);
      if (ip.empty())
      {
        JS_ReportError(ctx, "js_yasio_io_service_write_to : the ip can't be empty!");
        return false;
      }
      u_short port = static_cast<u_short>(args.get(3).toInt32());

      auto transport = jsb_yasio_jsval_to_io_transport(ctx, arg0);

      yasio_jsb::string_view_adapter sva;
      bool unrecognized_object = false;
      sva.set(arg1, ctx, &unrecognized_object);
      if (!sva.empty())
      {
        cobj->write_to(transport, std::vector<char>(sva.data(), sva.data() + sva.size()),
                       ip::endpoint{ip.data(), port});
      }
      else if (unrecognized_object)
      {
        auto obs = jsb_yasio_jsval_to_obstram(ctx, arg1);
        if (obs != nullptr)
        {
          auto& buffer = obs->buffer();
          if (!buffer.empty())
            cobj->write_to(transport, obs->buffer(), ip::endpoint{ip.data(), port});
        }
      }

      return true;
    }
  } while (false);

  JS_ReportError(ctx, "js_yasio_io_service_write_to : wrong number of arguments");
  return false;
}

void js_register_yasio_io_service(JSContext* ctx, JS::HandleObject global)
{
  jsb_io_service_class              = (JSClass*)calloc(1, sizeof(JSClass));
  jsb_io_service_class->name        = "io_service";
  jsb_io_service_class->addProperty = JS_PropertyStub;
  jsb_io_service_class->delProperty = JS_DeletePropertyStub;
  jsb_io_service_class->getProperty = JS_PropertyStub;
  jsb_io_service_class->setProperty = JS_StrictPropertyStub;
  jsb_io_service_class->enumerate   = JS_EnumerateStub;
  jsb_io_service_class->resolve     = JS_ResolveStub;
  jsb_io_service_class->convert     = JS_ConvertStub;
  jsb_io_service_class->finalize    = jsb_yasio__dtor<io_service>;
  jsb_io_service_class->flags       = JSCLASS_HAS_RESERVED_SLOTS(2);

  static JSPropertySpec properties[] = {JS_PS_END};

  static JSFunctionSpec funcs[] = {
      JS_FN("start", js_yasio_io_service_start, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("stop", js_yasio_io_service_stop, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("open", js_yasio_io_service_open, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("close", js_yasio_io_service_close, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("is_open", js_yasio_io_service_is_open, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("dispatch", js_yasio_io_service_dispatch, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("set_option", js_yasio_io_service_set_option, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write", js_yasio_io_service_write, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FN("write_to", js_yasio_io_service_write_to, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
      JS_FS_END};

  static JSFunctionSpec st_funcs[] = {JS_FS_END};

  jsb_io_service_prototype = JS_InitClass(ctx, global, JS::NullPtr(), jsb_io_service_class,
                                          jsb_yasio_io_service__ctor, 0, properties, funcs,
                                          NULL, // no static properties
                                          st_funcs);

  // add the proto and JSClass to the type->js info hash table
  JS::RootedObject proto(ctx, jsb_io_service_prototype);
  // JS_SetProperty(ctx, proto, "_className", className);
  JS_SetProperty(ctx, proto, "__nativeObj", JS::TrueHandleValue);
  JS_SetProperty(ctx, proto, "__is_ref", JS::FalseHandleValue);

  jsb_register_class<io_service>(ctx, jsb_io_service_class, proto, JS::NullPtr());
}

void jsb_register_yasio(JSContext* ctx, JS::HandleObject global)
{
  JS::RootedObject yasio(ctx);
  get_or_create_js_obj(ctx, global, "yasio", &yasio);

  JS_DefineFunction(ctx, yasio, "setTimeout", jsb_yasio_setTimeout, 2, 0);
  JS_DefineFunction(ctx, yasio, "setInterval", jsb_yasio_setInterval, 2, 0);
  JS_DefineFunction(ctx, yasio, "clearTimeout", jsb_yasio_killTimer, 1, 0);
  JS_DefineFunction(ctx, yasio, "clearInterval", jsb_yasio_killTimer, 1, 0);
  JS_DefineFunction(ctx, yasio, "highp_clock", jsb_yasio_highp_clock, 0, 0);
  JS_DefineFunction(ctx, yasio, "highp_time", jsb_yasio_highp_time, 0, 0);

  js_register_yasio_ibstream(ctx, yasio);
  js_register_yasio_obstream(ctx, yasio);
  js_register_yasio_transport(ctx, yasio);
  js_register_yasio_io_event(ctx, yasio);
  js_register_yasio_io_service(ctx, yasio);

  // ##-- yasio enums
  JS::RootedValue __jsvalIntVal(ctx);

#define YASIO_SET_INT_PROP(name, value)                                                            \
  __jsvalIntVal = INT_TO_JSVAL(value);                                                             \
  JS_SetProperty(ctx, yasio, name, __jsvalIntVal)
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

  YASIO_EXPORT_ENUM(SEEK_CUR);
  YASIO_EXPORT_ENUM(SEEK_SET);
  YASIO_EXPORT_ENUM(SEEK_END);
}
