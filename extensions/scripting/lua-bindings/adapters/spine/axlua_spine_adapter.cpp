/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
#if defined(AX_ENABLE_EXT_SPINE)
#    include "lua-bindings/adapters/spine/axlua_spine_adapter.hpp"
#    include "lua-bindings/generated/axlua_spine_gen.h"

#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "lua-bindings/runtime/axlua_conversions.h"
#    include "lua-bindings/runtime/AxluaCallbackRegistry.h"
#    include "lua-bindings/runtime/LuaValue.h"
#    include "spine/spine-axmol.h"
#    include "lua-bindings/adapters/spine/LuaSkeletonAnimation.h"
#    include "lua-bindings/runtime/LuaEngine.h"

using namespace spine;

static int axlua_SkeletonAnimation_createWithFile(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "sp.SkeletonAnimation", 0, &conversionError))
        goto argumentError;
#    endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 3, 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        const char* skeletonDataFile = axlua::adapter::to_string(L, 2, "");
        const char* atlasFile        = axlua::adapter::to_string(L, 3, "");

        auto returnValue = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile);

        axlua::adapter::push_object(L, (void*)returnValue, "sp.SkeletonAnimation");
        return 1;
    }
    else if (3 == argc)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(L, 4, 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        const char* skeletonDataFile = axlua::adapter::to_string(L, 2, "");
        const char* atlasFile        = axlua::adapter::to_string(L, 3, "");
        LUA_NUMBER scale             = axlua::adapter::to_number(L, 4, 1);

        auto returnValue = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile, scale);

        axlua::adapter::push_object(L, (void*)returnValue, "sp.SkeletonAnimation");
        return 1;
    }

    luaL_error(L,
               "'createWithFile' function of SkeletonAnimation has wrong number of arguments: %d, was expecting %d\n",
               argc, 2);

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'createWithFile'.", &conversionError);
#    endif
    return 0;
}

int executeSpineEvent(LuaSkeletonAnimation* skeletonAnimation,
                      int handler,
                      spine::EventType eventType,
                      spine::TrackEntry* entry,
                      spine::Event* event = nullptr)
{
    if (nullptr == skeletonAnimation || 0 == handler)
        return 0;

    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    if (nullptr == stack)
        return 0;

    lua_State* L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    if (nullptr == L)
        return 0;

    int ret = 0;

    std::string animationName = entry ? entry->getAnimation().getName().buffer() : "";
    std::string eventTypeName = "";

    switch (eventType)
    {
    case spine::EventType::EventType_Start:
    {
        eventTypeName = "start";
    }
    break;
    case spine::EventType::EventType_Interrupt:
    {
        eventTypeName = "interrupt";
    }
    break;
    case spine::EventType::EventType_End:
    {
        eventTypeName = "end";
    }
    break;
    case spine::EventType::EventType_Dispose:
    {
        eventTypeName = "dispose";
    }
    break;
    case spine::EventType::EventType_Complete:
    {
        eventTypeName = "complete";
    }
    break;
    case spine::EventType::EventType_Event:
    {
        eventTypeName = "event";
    }
    break;

    default:
        break;
    }

    LuaValueDict spineEvent;
    spineEvent["type"]       = LuaValue::stringValue(eventTypeName);
    spineEvent["trackIndex"] = LuaValue::intValue(entry ? entry->getTrackIndex() : 0);
    spineEvent["animation"]  = LuaValue::stringValue(animationName);
    spineEvent["loopCount"]  = LuaValue::intValue(
        entry && entry->getAnimationEnd() > 0 ? std::floor(entry->getTrackTime() / entry->getAnimationEnd()) : 0);

    if (nullptr != event)
    {
        LuaValueDict eventData;
        eventData["name"]        = LuaValue::stringValue(event->getData().getName().buffer());
        eventData["intValue"]    = LuaValue::intValue(event->getInt());
        eventData["floatValue"]  = LuaValue::floatValue(event->getFloat());
        eventData["stringValue"] = LuaValue::stringValue(event->getString().buffer());
        spineEvent["eventData"]  = LuaValue::dictValue(eventData);
    }

    stack->pushLuaValueDict(spineEvent);
    ret = stack->executeFunctionByHandler(handler, 1);
    return ret;
}

int axlua_SkeletonAnimation_registerSpineEventHandler00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "sp.SkeletonAnimation", 0, &conversionError) ||
        !axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError) ||
        !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 4, &conversionError))
        goto argumentError;
    else
    {
        LuaSkeletonAnimation* self = (LuaSkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);
        if (NULL != self)
        {
            int handler                = (axlua::adapter::ref_function(luaState, 2, 0));
            spine::EventType eventType = static_cast<spine::EventType>((int)axlua::adapter::to_number(luaState, 3, 0));

            switch (eventType)
            {
            case spine::EventType::EventType_Start:
            {
                self->setStartListener(
                    [=](spine::TrackEntry* entry) { executeSpineEvent(self, handler, eventType, entry); });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_START);
            }
            break;
            case spine::EventType::EventType_Interrupt:
            {
                self->setInterruptListener(
                    [=](spine::TrackEntry* entry) { executeSpineEvent(self, handler, eventType, entry); });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT);
            }
            break;
            case spine::EventType::EventType_End:
            {
                self->setEndListener(
                    [=](spine::TrackEntry* entry) { executeSpineEvent(self, handler, eventType, entry); });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_END);
            }
            break;
            case spine::EventType::EventType_Dispose:
            {
                self->setDisposeListener(
                    [=](spine::TrackEntry* entry) { executeSpineEvent(self, handler, eventType, entry); });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE);
            }
            break;
            case spine::EventType::EventType_Complete:
            {
                self->setCompleteListener(
                    [=](spine::TrackEntry* entry) { executeSpineEvent(self, handler, eventType, entry); });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE);
            }
            break;
            case spine::EventType::EventType_Event:
            {
                self->setEventListener([=](spine::TrackEntry* entry, spine::Event* event) {
                    executeSpineEvent(self, handler, eventType, entry, event);
                });
                AxluaCallbackRegistry::getInstance()->addObjectHandler(
                    (void*)self, handler, AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_EVENT);
            }
            break;
            default:
                break;
            }
        }
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'registerSpineEventHandler'.", &conversionError);
    return 0;
}

int axlua_SkeletonAnimation_unregisterSpineEventHandler00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "sp.SkeletonAnimation", 0, &conversionError) ||
        !axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 3, &conversionError))
        goto argumentError;
    else
    {
        LuaSkeletonAnimation* self = (LuaSkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);
        if (NULL != self)
        {
            spine::EventType eventType = static_cast<spine::EventType>((int)axlua::adapter::to_number(luaState, 2, 0));
            AxluaCallbackRegistry::HandlerType handlerType =
                AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_START;
            switch (eventType)
            {
            case spine::EventType::EventType_Start:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_START;
                self->setStartListener(nullptr);
                break;
            case spine::EventType::EventType_Interrupt:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT;
                break;
            case spine::EventType::EventType_End:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_END;
                self->setEndListener(nullptr);
                break;
            case spine::EventType::EventType_Dispose:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE;
                break;
            case spine::EventType::EventType_Complete:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE;
                self->setCompleteListener(nullptr);
                break;
            case spine::EventType::EventType_Event:
                handlerType = AxluaCallbackRegistry::HandlerType::EVENT_SPINE_ANIMATION_EVENT;
                self->setEventListener(nullptr);
                break;

            default:
                break;
            }
            AxluaCallbackRegistry::getInstance()->removeObjectHandler((void*)self, handlerType);
        }
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'unregisterScriptHandler'.", &conversionError);
    return 0;
}

static int axlua_spine_SkeletonAnimation_addAnimation(lua_State* luaState)
{
    int argc                      = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok                       = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "sp.SkeletonAnimation", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (spine::SkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_spine_SkeletonAnimation_addAnimation'",
                                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;

        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "sp.SkeletonAnimation:addAnimation");

        std::string arg1_tmp;
        ok &= luaval_to_std_string(luaState, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation");
        arg1 = arg1_tmp.c_str();

        ok &= luaval_to_boolean(luaState, 4, &arg2, "sp.SkeletonAnimation:addAnimation");
        if (!ok)
            return 0;
        obj->addAnimation(arg0, arg1, arg2);

        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 4)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        double arg3;

        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "sp.SkeletonAnimation:addAnimation");

        std::string arg1_tmp;
        ok &= luaval_to_std_string(luaState, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation");
        arg1 = arg1_tmp.c_str();

        ok &= luaval_to_boolean(luaState, 4, &arg2, "sp.SkeletonAnimation:addAnimation");

        ok &= luaval_to_number(luaState, 5, &arg3, "sp.SkeletonAnimation:addAnimation");
        if (!ok)
            return 0;

        obj->addAnimation(arg0, arg1, arg2, arg3);

        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "addAnimation", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_spine_SkeletonAnimation_addAnimation'.",
                                &conversionError);
#    endif

    return 0;
}

static int axlua_spine_SkeletonAnimation_setAnimation(lua_State* luaState)
{
    int argc                      = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok                       = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "sp.SkeletonAnimation", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (spine::SkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_spine_SkeletonAnimation_setAnimation'",
                                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;

        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "sp.SkeletonAnimation:setAnimation");

        std::string arg1_tmp;
        ok &= luaval_to_std_string(luaState, 3, &arg1_tmp, "sp.SkeletonAnimation:setAnimation");
        arg1 = arg1_tmp.c_str();

        ok &= luaval_to_boolean(luaState, 4, &arg2, "sp.SkeletonAnimation:setAnimation");
        if (!ok)
            return 0;

        obj->setAnimation(arg0, arg1, arg2);

        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "setAnimation", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_spine_SkeletonAnimation_setAnimation'.",
                                &conversionError);
#    endif

    return 0;
}

static int axlua_spine_SkeletonAnimation_getBoundingBox(lua_State* luaState)
{
    spine::SkeletonAnimation* obj = (spine::SkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'obj' in function 'axlua_spine_SkeletonAnimation_getBoundingBox'", nullptr);
        return 0;
    }
#    endif
    Rect rect = obj->getBoundingBox();
    // return a table
    lua_newtable(luaState);
    lua_pushstring(luaState, "x");
    lua_pushnumber(luaState, rect.origin.x);
    lua_rawset(luaState, -3);
    lua_pushstring(luaState, "y");
    lua_pushnumber(luaState, rect.origin.y);
    lua_rawset(luaState, -3);
    lua_pushstring(luaState, "width");
    lua_pushnumber(luaState, rect.size.width);
    lua_rawset(luaState, -3);
    lua_pushstring(luaState, "height");
    lua_pushnumber(luaState, rect.size.height);
    lua_rawset(luaState, -3);
    return 1;
}

static int axlua_spine_SkeletonAnimation_findBone(lua_State* luaState)
{
    int argc                      = 0;
    spine::SkeletonAnimation* obj = nullptr;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "sp.SkeletonAnimation", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (spine::SkeletonAnimation*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_spine_SkeletonAnimation_findBone'",
                                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        const char* arg0 = lua_tostring(luaState, 2);
        if (!arg0)
        {
            axlua::adapter::raise_error(luaState, "sp.SkeletonAnimation:findBone arg 1 must string", nullptr);
            return 0;
        }

        auto bone = obj->findBone(arg0);

        lua_newtable(luaState);

        if (NULL != bone)
        {
            auto& bonePose = bone->getAppliedPose();
            lua_pushstring(luaState, "x");
            lua_pushnumber(luaState, bonePose.getX());
            lua_rawset(luaState, -3); /* bone.x */

            lua_pushstring(luaState, "y");
            lua_pushnumber(luaState, bonePose.getY());
            lua_rawset(luaState, -3); /* bone.y */

            lua_pushstring(luaState, "rotation");
            lua_pushnumber(luaState, bonePose.getRotation());
            lua_rawset(luaState, -3); /* bone.rotation */

            lua_pushstring(luaState, "scaleX");
            lua_pushnumber(luaState, bonePose.getScaleX());
            lua_rawset(luaState, -3); /* bone.scaleX */
            lua_pushstring(luaState, "scaleY");
            lua_pushnumber(luaState, bonePose.getScaleY());
            lua_rawset(luaState, -3); /* bone.scaleY */

            lua_pushstring(luaState, "worldX");
            lua_pushnumber(luaState, bonePose.getWorldX());
            lua_rawset(luaState, -3); /* bone.worldX */
            lua_pushstring(luaState, "worldY");
            lua_pushnumber(luaState, bonePose.getWorldY());
            lua_rawset(luaState, -3); /* bone.worldY */
        }
        return 1;
    }
    luaL_error(luaState, "findBone has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_spine_SkeletonAnimation_findBone'.",
                                &conversionError);
#    endif

    return 0;
}

static void extendSkeletonAnimation(lua_State* L)
{
    lua_pushstring(L, "sp.SkeletonAnimation");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "create", axlua_SkeletonAnimation_createWithFile);
        axlua::adapter::set_function(L, "registerSpineEventHandler",
                                     axlua_SkeletonAnimation_registerSpineEventHandler00);
        axlua::adapter::set_function(L, "unregisterSpineEventHandler",
                                     axlua_SkeletonAnimation_unregisterSpineEventHandler00);
        axlua::adapter::set_function(L, "addAnimation", axlua_spine_SkeletonAnimation_addAnimation);
        axlua::adapter::set_function(L, "setAnimation", axlua_spine_SkeletonAnimation_setAnimation);
        axlua::adapter::set_function(L, "findBone", axlua_spine_SkeletonAnimation_findBone);
        axlua::adapter::set_function(L, "getBoundingBox", axlua_spine_SkeletonAnimation_getBoundingBox);
    }
    lua_pop(L, 1);

    axlua::register_native_type_name(typeid(LuaSkeletonAnimation), "sp.SkeletonAnimation");
}

int register_all_ax_spine_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendSkeletonAnimation(L);

    return 0;
}

int register_spine_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_spine(L);
        register_all_ax_spine_adapter(L);
    }
    lua_pop(L, 1);

    return 1;
}

#endif  // defined(AX_ENABLE_EXT_SPINE)
