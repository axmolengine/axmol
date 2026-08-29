/****************************************************************************
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "lua-bindings/runtime/LuaEngine.h"
#include "lua-bindings/runtime/axlua_adapter.h"

#include "extensions/GUI/src/GUI/ControlExtension/Control.h"
#include "lua-bindings/adapters/base/axlua_base_adapter.h"
#include "lua-bindings/adapters/extension/axlua_extension_adapter.h"
#include "lua-bindings/adapters/sceneext/axlua_sceneext_adapter.hpp"
#include "lua-bindings/adapters/ui/axlua_ui_adapter.hpp"
#include "axmol/2d/MenuItem.h"
#include "axmol/base/Director.h"
#include "axmol/base/CustomEvent.h"

namespace ax
{

namespace
{
const char* inputPhaseToLuaTouchName(InputPhase phase)
{
    switch (phase)
    {
    case InputPhase::PointerDown:
        return "began";
    case InputPhase::PointerMove:
        return "moved";
    case InputPhase::PointerUp:
        return "ended";
    case InputPhase::PointerCancel:
        return "cancelled";
    default:
        return nullptr;
    }
}
}  // namespace

LuaEngine* LuaEngine::_defaultEngine = nullptr;

LuaEngine* LuaEngine::getInstance(void)
{
    if (!_defaultEngine)
    {
        _defaultEngine = new LuaEngine();
        _defaultEngine->init();
    }
    return _defaultEngine;
}

LuaEngine::~LuaEngine(void)
{
    // Detach the stack before releasing it.  LuaStack is an Object and its
    // base destructor can run while ScriptEngineManager still tears down the
    // engine; callbacks during that phase must not re-enter Lua.
    LuaStack* stack = _stack;
    _stack          = nullptr;
    AxluaCallbackRegistry::destroyInstance();
    AX_SAFE_RELEASE(stack);
    _defaultEngine = nullptr;
}

bool LuaEngine::init(void)
{
    _stack = LuaStack::create();
    _stack->retain();
    return true;
}

void LuaEngine::addSearchPath(const char* path)
{
    _stack->addSearchPath(path);
}

void LuaEngine::addLuaLoader(lua_CFunction func)
{
    _stack->addLuaLoader(func);
}

void LuaEngine::removeScriptObjectByObject(Object* pObj)
{
    if (_stack == nullptr || _stack->getLuaState() == nullptr)
        return;

    _stack->removeScriptObjectByObject(pObj);
    auto scriptHandlerMgr = AxluaCallbackRegistry::getInstanceIfExists();
    if (scriptHandlerMgr)
        scriptHandlerMgr->removeObjectAllHandlers(pObj);
}

void LuaEngine::removeScriptHandler(int nHandler)
{
    _stack->removeScriptHandler(nHandler);
}

int LuaEngine::executeString(const char* codes)
{
    int ret = _stack->executeString(codes);
    _stack->clean();
    return ret;
}

int LuaEngine::executeScriptFile(const char* filename)
{
    int ret = _stack->executeScriptFile(filename);
    _stack->clean();
    return ret;
}

int LuaEngine::executeGlobalFunction(const char* functionName)
{
    int ret = _stack->executeGlobalFunction(functionName);
    _stack->clean();
    return ret;
}

int LuaEngine::executeEvent(int nHandler,
                            const char* pEventName,
                            Object* pEventSource /* = nullptr*/,
                            const char* pEventSourceClassName /* = nullptr*/)
{
    _stack->pushString(pEventName);
    if (pEventSource)
    {
        _stack->pushObject(pEventSource, pEventSourceClassName ? pEventSourceClassName : "ax.Object");
    }
    int ret = _stack->executeFunctionByHandler(nHandler, pEventSource ? 2 : 1);
    _stack->clean();
    return ret;
}

bool LuaEngine::handleAssert(const char* msg)
{
    bool ret = _stack->handleAssert(msg);
    _stack->clean();
    return ret;
}

int LuaEngine::reallocateScriptHandler(int nHandler)
{
    int nRet = _stack->reallocateScriptHandler(nHandler);
    _stack->clean();
    return nRet;
}

bool LuaEngine::parseConfig(ConfigType type, std::string_view str)
{
    lua_getglobal(_stack->getLuaState(), "__onParseConfig");
    if (!lua_isfunction(_stack->getLuaState(), -1))
    {
        AXLOGW("[LUA WARN] name '{}' does not represent a Lua function", "__onParseConfig");
        lua_pop(_stack->getLuaState(), 1);
        return false;
    }

    _stack->pushInt((int)type);
    _stack->pushString(str.data(), str.length());

    return _stack->executeFunction(2);
}

int LuaEngine::sendEvent(const ScriptEvent& evt)
{
    switch (evt.type)
    {
    case kNodeEvent:
        return handleNodeEvent(evt.data);
    case kMenuClickedEvent:
        return handleMenuClickedEvent(evt.data);
    case kCallFuncEvent:
        return handleCallFuncActionEvent(evt.data);
    case kScheduleEvent:
        return handleScheduler(evt.data);
    case kTouchEvent:
        return handleTouchEvent(evt.data);
    case kTouchesEvent:
        return handleTouchesEvent(evt.data);
    case kKeypadEvent:
        return handleKeypadEvent(evt.data);
    case kAccelerometerEvent:
        return handleAccelerometerEvent(evt.data);
    case kCommonEvent:
        return handleCommonEvent(evt.data);
    case kControlEvent:
        return handlerControlEvent(evt.data);
    default:;
    }

    return 0;
}

int LuaEngine::handleNodeEvent(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = (BasicScriptData*)data;
    if (nullptr == basicScriptData->nativeObject || nullptr == basicScriptData->value)
        return 0;

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(basicScriptData->nativeObject,
                                                                         AxluaCallbackRegistry::HandlerType::NODE);

    if (0 == handler)
        return 0;

    int action = *((int*)(basicScriptData->value));
    switch (action)
    {
    case kNodeOnEnter:
        _stack->pushString("enter");
        break;

    case kNodeOnExit:
        _stack->pushString("exit");
        break;

    case kNodeOnEnterTransitionDidFinish:
        _stack->pushString("enterTransitionFinish");
        break;

    case kNodeOnExitTransitionDidStart:
        _stack->pushString("exitTransitionStart");
        break;

    case kNodeOnCleanup:
        _stack->pushString("cleanup");
        break;

    default:
        return 0;
    }
    int ret = _stack->executeFunctionByHandler(handler, 1);
    _stack->clean();
    return ret;
}

int LuaEngine::handleMenuClickedEvent(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = (BasicScriptData*)data;
    if (nullptr == basicScriptData->nativeObject)
        return 0;

    MenuItem* menuItem = static_cast<MenuItem*>(basicScriptData->nativeObject);

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(
        menuItem, AxluaCallbackRegistry::HandlerType::MENU_CLICKED);
    if (0 == handler)
        return 0;

    _stack->pushInt(menuItem->getTag());
    _stack->pushObject(menuItem, "ax.MenuItem");
    int ret = _stack->executeFunctionByHandler(handler, 2);
    _stack->clean();
    return ret;
}

int LuaEngine::handleCallFuncActionEvent(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = static_cast<BasicScriptData*>(data);
    if (nullptr == basicScriptData->nativeObject)
        return 0;

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(basicScriptData->nativeObject,
                                                                         AxluaCallbackRegistry::HandlerType::CALLFUNC);

    if (0 == handler)
        return 0;

    Object* target = static_cast<Object*>(basicScriptData->value);
    if (nullptr != target)
    {
        _stack->pushObject(target, "ax.Node");
    }
    int ret = _stack->executeFunctionByHandler(handler, target ? 1 : 0);
    _stack->clean();
    return ret;
}

int LuaEngine::handleScheduler(void* data)
{
    if (nullptr == data)
        return 0;

    SchedulerScriptData* schedulerInfo = static_cast<SchedulerScriptData*>(data);

    _stack->pushFloat(schedulerInfo->elapse);
    int ret = _stack->executeFunctionByHandler(schedulerInfo->handler, 1);
    _stack->clean();

    return ret;
}

int LuaEngine::handleKeypadEvent(void* data)
{
    if (nullptr == data)
        return 0;

    KeypadScriptData* keypadScriptData = static_cast<KeypadScriptData*>(data);
    if (nullptr == keypadScriptData->nativeObject)
        return 0;

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(keypadScriptData->nativeObject,
                                                                         AxluaCallbackRegistry::HandlerType::KEYPAD);

    if (0 == handler)
        return 0;

    KeyboardEvent::KeyCode action = keypadScriptData->actionType;

    switch (action)
    {
    case KeyboardEvent::KeyCode::KEY_ESCAPE:
        _stack->pushString("backClicked");
        break;
    case KeyboardEvent::KeyCode::KEY_MENU:
        _stack->pushString("menuClicked");
        break;
    default:
        break;
    }

    int ret = _stack->executeFunctionByHandler(handler, 1);
    _stack->clean();
    return ret;
}

int LuaEngine::handleAccelerometerEvent(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = static_cast<BasicScriptData*>(data);
    if (nullptr == basicScriptData->nativeObject || nullptr == basicScriptData->value)
        return 0;

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(
        basicScriptData->nativeObject, AxluaCallbackRegistry::HandlerType::ACCELEROMETER);
    if (0 == handler)
        return 0;

    Acceleration* accelerationValue = static_cast<Acceleration*>(basicScriptData->value);
    _stack->pushFloat(float(accelerationValue->x));
    _stack->pushFloat(float(accelerationValue->y));
    _stack->pushFloat(float(accelerationValue->z));
    _stack->pushFloat(float(accelerationValue->timestamp));
    int ret = _stack->executeFunctionByHandler(handler, 4);
    _stack->clean();
    return ret;
}

int LuaEngine::handleCommonEvent(void* data)
{
    if (nullptr == data)
        return 0;

    CommonScriptData* commonInfo = static_cast<CommonScriptData*>(data);
    if (0 == commonInfo->handler)
        return 0;

    _stack->pushString(commonInfo->eventName);
    if (nullptr != commonInfo->eventSource)
    {
        if (commonInfo->eventSourceClassName[0])
        {
            _stack->pushObject(commonInfo->eventSource, commonInfo->eventSourceClassName);
        }
        else
        {
            _stack->pushObject(commonInfo->eventSource, "ax.Object");
        }
    }
    int ret = _stack->executeFunctionByHandler(commonInfo->handler, commonInfo->eventSource ? 2 : 1);
    _stack->clean();
    return ret;
}

int LuaEngine::handleTouchEvent(void* data)
{
    if (nullptr == data)
        return 0;

    TouchScriptData* touchScriptData = static_cast<TouchScriptData*>(data);
    if (nullptr == touchScriptData->nativeObject || nullptr == touchScriptData->event)
        return 0;

    auto* pointerEvent = static_cast<PointerEvent*>(touchScriptData->event);
    int handler        = AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)touchScriptData->nativeObject,
                                                                                AxluaCallbackRegistry::HandlerType::TOUCHES);

    if (0 == handler)
        return 0;

    auto phaseName = inputPhaseToLuaTouchName(touchScriptData->actionType);
    if (nullptr == phaseName)
        return 0;

    _stack->pushString(phaseName);
    const ax::Vec2 pt = pointerEvent->getWorldPoint();
    _stack->pushFloat(pt.x);
    _stack->pushFloat(pt.y);
    int ret = _stack->executeFunctionByHandler(handler, 3);
    _stack->clean();
    return ret;
}

int LuaEngine::handleTouchesEvent(void* data)
{
    if (nullptr == data)
        return 0;

    TouchScriptData* touchesScriptData = static_cast<TouchScriptData*>(data);
    if (nullptr == touchesScriptData->nativeObject || nullptr == touchesScriptData->event)
        return 0;

    auto* pointerEvent = static_cast<PointerEvent*>(touchesScriptData->event);

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)touchesScriptData->nativeObject,
                                                                         AxluaCallbackRegistry::HandlerType::TOUCHES);

    if (0 == handler)
        return 0;

    auto phaseName = inputPhaseToLuaTouchName(touchesScriptData->actionType);
    if (nullptr == phaseName)
        return 0;

    Director* pDirector = Director::getInstance();
    lua_State* L        = _stack->getLuaState();

    lua_newtable(L);
    ax::Vec2 pt = pointerEvent->getWorldPoint();
    lua_pushnumber(L, pt.x);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, pt.y);
    lua_rawseti(L, -2, 2);
    lua_pushinteger(L, pointerEvent->getPointerId());
    lua_rawseti(L, -2, 3);
    _stack->pushString(phaseName);
    int ret = _stack->executeFunctionByHandler(handler, 2);

    _stack->clean();
    return ret;
}

int LuaEngine::handlerControlEvent(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = static_cast<BasicScriptData*>(data);
    if (nullptr == basicScriptData->nativeObject)
        return 0;

    int controlEvents = *((int*)(basicScriptData->value));

    int ret = 0;

    for (int i = 0; i < kControlEventTotalNumber; i++)
    {
        if ((controlEvents & (1 << i)))
        {
            AxluaCallbackRegistry::HandlerType controlHandler =
                AxluaCallbackRegistry::HandlerType((int)AxluaCallbackRegistry::HandlerType::CONTROL_TOUCH_DOWN + i);
            const auto handler =
                AxluaCallbackRegistry::getInstance()->getObjectHandler(basicScriptData->nativeObject, controlHandler);

            if (0 != handler)
            {
                _stack->pushObject((Object*)basicScriptData->nativeObject, "ax.Object");
                _stack->pushInt(controlEvents);
                ret = _stack->executeFunctionByHandler(handler, 2);
                _stack->clean();
            }
        }
    }

    return ret;
}

int LuaEngine::handleEventAcc(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicScriptData = static_cast<BasicScriptData*>(data);
    if (nullptr == basicScriptData->nativeObject || nullptr == basicScriptData->value)
        return 0;

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(basicScriptData->nativeObject,
                                                                         AxluaCallbackRegistry::HandlerType::EVENT_ACC);
    if (0 == handler)
        return 0;

    lua_State* L = _stack->getLuaState();

    LuaAccelerationEventData* eventListennerAcc = static_cast<LuaAccelerationEventData*>(basicScriptData->value);
    axlua::adapter::push_object(L, (void*)(eventListennerAcc->event), "ax.Event");
    auto& accleration = static_cast<AccelerationEvent*>(eventListennerAcc->event)->getAcceleration();
    lua_pushnumber(L, accleration.x);
    lua_pushnumber(L, accleration.y);
    lua_pushnumber(L, accleration.z);
    lua_pushnumber(L, accleration.timestamp);
    int ret = _stack->executeFunctionByHandler(handler, 5);
    _stack->clean();
    return ret;
}

int LuaEngine::handleEvenCustom(void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* basicData = static_cast<BasicScriptData*>(data);
    if (nullptr == basicData->nativeObject || nullptr == basicData->value)
        return 0;

    CustomEvent* eventCustom = static_cast<CustomEvent*>(basicData->value);
    int handler              = AxluaCallbackRegistry::getInstance()->getObjectHandler(
        (void*)basicData->nativeObject, AxluaCallbackRegistry::HandlerType::EVENT_CUSTIOM);

    if (0 == handler)
        return 0;

    lua_State* L = _stack->getLuaState();
    axlua::adapter::push_object(L, (void*)(eventCustom), "ax.CustomEvent");
    int ret = _stack->executeFunctionByHandler(handler, 1);
    _stack->clean();

    return ret;
}

int LuaEngine::handleEvent(AxluaCallbackRegistry::HandlerType type, void* data)
{
    switch (type)
    {
    case AxluaCallbackRegistry::HandlerType::SCROLLVIEW_SCROLL:
    case AxluaCallbackRegistry::HandlerType::SCROLLVIEW_ZOOM:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_TOUCHED:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_HIGHLIGHT:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_UNHIGHLIGHT:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_WILL_RECYCLE:
        return handleTableViewEvent(type, data);
    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_PROGRESS:
    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_ERROR:
    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_SUCCESS:
        return handleAssetsManagerEvent(type, data);
    case AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT:
        return handleArmatureWrapper(type, data);
    case AxluaCallbackRegistry::HandlerType::EVENT_ACC:
        return handleEventAcc(data);
    case AxluaCallbackRegistry::HandlerType::EVENT_CUSTIOM:
        return handleEvenCustom(data);
    default:;
    }

    return 0;
}

int LuaEngine::handleEvent(AxluaCallbackRegistry::HandlerType type,
                           void* data,
                           int numResults,
                           const std::function<void(lua_State*, int)>& func)
{
    switch (type)
    {
    case AxluaCallbackRegistry::HandlerType::TABLECELL_SIZE_FOR_INDEX:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_AT_INDEX:
    case AxluaCallbackRegistry::HandlerType::TABLEVIEW_NUMS_OF_CELLS:
        return handleTableViewEvent(type, data, numResults, func);
    default:;
    }

    return 0;
}

int LuaEngine::handleTableViewEvent(AxluaCallbackRegistry::HandlerType type, void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* eventData = static_cast<BasicScriptData*>(data);
    if (nullptr == eventData->nativeObject || nullptr == eventData->value)
        return 0;

    LuaTableViewEventData* tableViewData = static_cast<LuaTableViewEventData*>(eventData->value);
    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)eventData->nativeObject, type);

    if (0 == handler)
        return 0;

    Object* obj = static_cast<Object*>(eventData->nativeObject);
    if (nullptr == obj)
        return 0;

    int ret = 0;
    switch (type)
    {
    case AxluaCallbackRegistry::HandlerType::SCROLLVIEW_SCROLL:
    case AxluaCallbackRegistry::HandlerType::SCROLLVIEW_ZOOM:
    {
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(obj), "ax.TableView");
        ret = _stack->executeFunctionByHandler(handler, 1);
    }
    break;
    case AxluaCallbackRegistry::HandlerType::TABLECELL_TOUCHED:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_HIGHLIGHT:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_UNHIGHLIGHT:
    case AxluaCallbackRegistry::HandlerType::TABLECELL_WILL_RECYCLE:
    {
        Object* cellObject = static_cast<Object*>(tableViewData->value);
        if (nullptr == cellObject)
        {
            break;
        }
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(obj), "ax.TableView");
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(cellObject), "ax.TableViewCell");
        ret = _stack->executeFunctionByHandler(handler, 2);
    }
    break;
    default:
        break;
    }

    return ret;
}

int LuaEngine::handleTableViewEvent(AxluaCallbackRegistry::HandlerType handlerType,
                                    void* data,
                                    int numResults,
                                    const std::function<void(lua_State*, int)>& func)
{
    if (nullptr == data || numResults <= 0)
        return 0;

    BasicScriptData* eventData = static_cast<BasicScriptData*>(data);
    if (nullptr == eventData->nativeObject || nullptr == eventData->value)
        return 0;

    LuaTableViewEventData* tableViewData = static_cast<LuaTableViewEventData*>(eventData->value);
    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)eventData->nativeObject, handlerType);

    if (0 == handler)
        return 0;

    Object* obj = static_cast<Object*>(eventData->nativeObject);
    if (nullptr == obj)
        return 0;

    int ret = 0;
    switch (handlerType)
    {
    case AxluaCallbackRegistry::HandlerType::TABLECELL_SIZE_FOR_INDEX:
    {
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(obj), "ax.TableView");
        _stack->pushLong(*((ssize_t*)tableViewData->value));
        ret = _stack->executeFunction(handler, 2, 2, func);
    }
    break;
    case AxluaCallbackRegistry::HandlerType::TABLECELL_AT_INDEX:
    {
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(obj), "ax.TableView");
        _stack->pushLong(*((ssize_t*)tableViewData->value));
        ret = _stack->executeFunction(handler, 2, 1, func);
    }
    break;
    case AxluaCallbackRegistry::HandlerType::TABLEVIEW_NUMS_OF_CELLS:
    {
        axlua::adapter::push_object(_stack->getLuaState(), (void*)(obj), "ax.TableView");
        ret = _stack->executeFunction(handler, 1, 1, func);
    }
    break;
    default:
        break;
    }

    return ret;
}

int LuaEngine::handleAssetsManagerEvent(AxluaCallbackRegistry::HandlerType type, void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* eventData = static_cast<BasicScriptData*>(data);
    if (nullptr == eventData->nativeObject || nullptr == eventData->value)
        return 0;

    LuaAssetsManagerEventData* assetsManagerData = static_cast<LuaAssetsManagerEventData*>(eventData->value);

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)eventData->nativeObject, type);

    if (0 == handler)
        return 0;

    int ret = 0;
    switch (type)
    {
    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_PROGRESS:
    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_ERROR:
    {
        _stack->pushInt(assetsManagerData->value);
        ret = _stack->executeFunctionByHandler(handler, 1);
    }
    break;

    case AxluaCallbackRegistry::HandlerType::ASSETSMANAGER_SUCCESS:
    {
        ret = _stack->executeFunctionByHandler(handler, 0);
    }
    break;

    default:
        break;
    }

    return ret;
}

int LuaEngine::handleArmatureWrapper(AxluaCallbackRegistry::HandlerType type, void* data)
{
    if (nullptr == data)
        return 0;

    BasicScriptData* eventData = static_cast<BasicScriptData*>(data);
    if (nullptr == eventData->nativeObject || nullptr == eventData->value)
        return 0;

    LuaArmatureWrapperEventData* wrapperData = static_cast<LuaArmatureWrapperEventData*>(eventData->value);

    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(
        (void*)eventData->nativeObject, AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

    if (0 == handler)
        return 0;

    switch (wrapperData->eventType)
    {
    case LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::MOVEMENT_EVENT:
    {
        LuaArmatureMovementEventData* movementData = static_cast<LuaArmatureMovementEventData*>(wrapperData->eventData);

        _stack->pushObject(movementData->objTarget, "axext.Armature");
        _stack->pushInt(movementData->movementType);
        _stack->pushString(movementData->movementID.c_str());
        _stack->executeFunctionByHandler(handler, 3);
    }
    break;
    case LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FRAME_EVENT:
    {
        LuaArmatureFrameEventData* frameData = static_cast<LuaArmatureFrameEventData*>(wrapperData->eventData);

        _stack->pushObject(frameData->objTarget, "axext.Bone");
        _stack->pushString(frameData->frameEventName.c_str());
        _stack->pushInt(frameData->originFrameIndex);
        _stack->pushInt(frameData->currentFrameIndex);
        _stack->executeFunctionByHandler(handler, 4);
    }
    break;
    case LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FILE_ASYNC:
    {
        _stack->pushFloat(*(float*)wrapperData->eventData);
        _stack->executeFunctionByHandler(handler, 1);
    }
    break;
    default:
        break;
    }

    _stack->clean();

    return 0;
}

int LuaEngine::reload(const char* moduleFileName)
{
    return _stack->reload(moduleFileName);
}

}  // namespace ax
