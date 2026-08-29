/****************************************************************************
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
#include "lua-bindings/adapters/sceneext/axlua_sceneext_adapter.hpp"
#include "lua-bindings/generated/axlua_sceneext_gen.h"

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/AxluaCallbackRegistry.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "sceneext/sceneext.h"
#include "sceneio/ActionTimeline/ActionTimelineCache.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "lua-bindings/adapters/sceneext/CustomGUIReader.h"

using namespace ax::ext;

class LuaArmatureWrapper : public Object
{
public:
    LuaArmatureWrapper();
    virtual ~LuaArmatureWrapper();

    virtual void addArmatureFileInfoAsyncCallback(float percent);
};

LuaArmatureWrapper::LuaArmatureWrapper() {}

LuaArmatureWrapper::~LuaArmatureWrapper() {}

void LuaArmatureWrapper::addArmatureFileInfoAsyncCallback(float percent)
{
    int handler = AxluaCallbackRegistry::getInstance()->getObjectHandler(
        (void*)this, AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

    if (0 != handler)
    {
        LuaArmatureWrapperEventData wrapperData(LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FILE_ASYNC,
                                                (void*)&percent);

        BasicScriptData data(this, (void*)&wrapperData);

        LuaEngine::getInstance()->handleEvent(AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT, (void*)&data);
    }
}

static int axlua_ArmatureController_setMovementEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    ArmatureController* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axext.ArmatureController", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ArmatureController*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            L, "invalid 'self' in function 'axlua_ArmatureController_setMovementEventCallFunc'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        Vector<LuaArmatureWrapper*> vec;
        vec.pushBack(wrapper);
        AxluaCallbackRegistry::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                               AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

        self->setMovementEventCallFunc(
            [=](Armature* armature, MovementEventType movementType, std::string_view movementID) {
            if (0 != handler)
            {
                LuaArmatureMovementEventData movementData(armature, (int)movementType, movementID);

                LuaArmatureWrapperEventData wrapperData(
                    LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::MOVEMENT_EVENT, (void*)&movementData);

                BasicScriptData data((void*)vec.at(0), (void*)&wrapperData);

                LuaEngine::getInstance()->handleEvent(AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT, (void*)&data);
            }
        });
        return 0;
    }

    luaL_error(L,
               "'setMovementEventCallFunc' function of ArmatureController has wrong number of arguments: %d, was "
               "expecting %d\n",
               argc, 1);

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setMovementEventCallFunc'.", &conversionError);
    return 0;
#endif
}

static int axlua_ArmatureController_setFrameEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    ArmatureController* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axext.ArmatureController", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ArmatureController*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_ArmatureController_setFrameEventCallFunc'\n",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        Vector<LuaArmatureWrapper*> vec;
        vec.pushBack(wrapper);

        AxluaCallbackRegistry::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                               AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

        self->setFrameEventCallFunc(
            [=](ax::ext::Bone* bone, std::string_view frameEventName, int originFrameIndex, int currentFrameIndex) {
            if (0 != handler)
            {
                std::string strFrameEventName(frameEventName);

                LuaArmatureFrameEventData frameData(bone, frameEventName, originFrameIndex, currentFrameIndex);

                LuaArmatureWrapperEventData wrapperData(
                    LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FRAME_EVENT, (void*)&frameData);

                BasicScriptData data((void*)vec.at(0), (void*)&wrapperData);

                LuaEngine::getInstance()->handleEvent(AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT, (void*)&data);
            }
        });

        return 0;
    }

    luaL_error(
        L,
        "'setFrameEventCallFunc' function of ArmatureController has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setFrameEventCallFunc'.", &conversionError);
#endif
    return 0;
}

static void extendArmatureController(lua_State* L)
{
    lua_pushstring(L, "axext.ArmatureController");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setMovementEventCallFunc", axlua_ArmatureController_setMovementEventCallFunc);
        axlua::adapter::set_function(L, "setFrameEventCallFunc", axlua_ArmatureController_setFrameEventCallFunc);
    }
    lua_pop(L, 1);
}

static int axlua_ArmatureDataManager_addArmatureFileInfoAsyncCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                  = 0;
    ArmatureDataManager* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axext.ArmatureDataManager", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ArmatureDataManager*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            L, "invalid 'self' in function 'axlua_ArmatureDataManager_addArmatureFileInfoAsyncCallFunc'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_function(L, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        const char* configFilePath = axlua::adapter::to_string(L, 2, "");
        LUA_FUNCTION handler       = (axlua::adapter::ref_function(L, 3, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        AxluaCallbackRegistry::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                               AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

        self->addArmatureFileInfoAsync(configFilePath, wrapper,
                                       AX_SCHEDULE_SELECTOR(LuaArmatureWrapper::addArmatureFileInfoAsyncCallback));

        return 0;
    }
    else if (4 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 3, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 4, 0, &conversionError) ||
            !axlua::adapter::is_function(L, 5, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        const char* imagePath      = axlua::adapter::to_string(L, 2, "");
        const char* plistPath      = axlua::adapter::to_string(L, 3, "");
        const char* configFilePath = axlua::adapter::to_string(L, 4, "");

        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 5, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        AxluaCallbackRegistry::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                               AxluaCallbackRegistry::HandlerType::ARMATURE_EVENT);

        self->addArmatureFileInfoAsync(imagePath, plistPath, configFilePath, wrapper,
                                       AX_SCHEDULE_SELECTOR(LuaArmatureWrapper::addArmatureFileInfoAsyncCallback));

        return 0;
    }

    luaL_error(L,
               "'addArmatureFileInfoAsync' function of ArmatureDataManager has wrong number of arguments: %d, was "
               "expecting %d\n",
               argc, 1);

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'addArmatureFileInfoAsync'.", &conversionError);
#endif
    return 0;
}

static void extendArmatureDataManager(lua_State* L)
{
    lua_pushstring(L, "axext.ArmatureDataManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "addArmatureFileInfoAsync",
                                     axlua_ArmatureDataManager_addArmatureFileInfoAsyncCallFunc);
    }
    lua_pop(L, 1);
}

static int axlua_extension_Bone_setIgnoreMovementBoneData(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc            = 0;
    ax::ext::Bone* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axext.Bone", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::ext::Bone*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_extension_Bone_setIgnoreMovementBoneData'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_boolean(L, 2, 0, &conversionError))
            goto argumentError;
#endif
        bool ignore = (bool)axlua::adapter::to_boolean(L, 2, 0);
        self->setIgnoreMovementBoneData(ignore);
        return 0;
    }

    luaL_error(L, "'setIgnoreMovementBoneData' function of Bone  has wrong number of arguments: %d, was expecting %d\n",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setIgnoreMovementBoneData'.", &conversionError);
    return 0;
#endif
}

static void extendBone(lua_State* L)
{
    lua_pushstring(L, "axext.Bone");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setIgnoreMovementBoneData", axlua_extension_Bone_setIgnoreMovementBoneData);
    }
    lua_pop(L, 1);
}

int axlua_studio_ActionTimelineCache_getInstance(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(L, 1, "axext.ActionTimelineCache", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        if (!ok)
            return 0;
        ax::ext::timeline::ActionTimelineCache* ret = ax::ext::timeline::ActionTimelineCache::getInstance();
        axlua::adapter::push_usertype(L, (void*)ret, "axext.ActionTimelineCache");
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ActionTimelineCache:getInstance",
               argc, 0);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_studio_ActionTimelineCache_getInstance'.",
                                &conversionError);
#endif
    return 0;
}

static void extendActionTimelineCache(lua_State* L)
{
    lua_pushstring(L, "axext.ActionTimelineCache");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "getInstance", axlua_studio_ActionTimelineCache_getInstance);
    }
    lua_pop(L, 1);
}

static int axlua_ActionTimeline_setFrameEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                                = 0;
    ax::ext::timeline::ActionTimeline* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axext.ActionTimeline", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::ext::timeline::ActionTimeline*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_ActionTimeline_setFrameEventCallFunc'\n",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));
        self->setFrameEventCallFunc([=](ext::timeline::Frame* frame) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            axlua::adapter::push_object(stack->getLuaState(), (void*)frame, getLuaTypeName(frame, "axext.Frame"));
            stack->executeFunctionByHandler(handler, 1);
        });

        return 0;
    }

    luaL_error(
        L, "'setFrameEventCallFunc' function of ActionTimeline has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setFrameEventCallFunc'.", &conversionError);
#endif
    return 0;
}

static void extendActionTimeline(lua_State* L)
{
    lua_pushstring(L, "axext.ActionTimeline");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setFrameEventCallFunc", axlua_ActionTimeline_setFrameEventCallFunc);
    }
    lua_pop(L, 1);
}

int axlua_CustomGUIReader_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(L, 1, "axext.CustomGUIReader", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    do
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(L, 2, &arg0, "axext.CustomGUIReader:create");
            if (!ok)
            {
                break;
            }
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_function(L, 3, "LUA_FUNCTION", 0, &conversionError))
            {
                goto argumentError;
            }
#endif
            LUA_FUNCTION arg1 = axlua::adapter::ref_function(L, 3, 0);
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_function(L, 4, "LUA_FUNCTION", 0, &conversionError))
            {
                goto argumentError;
            }
#endif
            LUA_FUNCTION arg2 = axlua::adapter::ref_function(L, 4, 0);

            ax::ext::CustomGUIReader* ret = ax::ext::CustomGUIReader::create(arg0, arg1, arg2);
            object_to_luaval<ax::ext::CustomGUIReader>(L, "axext.CustomGUIReader", (ax::ext::CustomGUIReader*)ret);
            return 1;
        }
    } while (0);
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d", "axext.CustomGUIReader:create", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_CustomGUIReader_create'.", &conversionError);
#endif
    return 0;
}

int register_all_ax_sceneext_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;
    extendArmatureController(L);
    extendArmatureDataManager(L);
    extendBone(L);
    extendActionTimelineCache(L);
    extendActionTimeline(L);

    return 0;
}

int lua_register_ax_sceneext_CustomGUIReader(lua_State* L)
{
    axlua::adapter::module(L, "axext", 0);
    axlua::adapter::begin_module(L, "axext");

    axlua::adapter::register_usertype(L, "axext.CustomGUIReader");
    axlua::adapter::register_class(L, "CustomGUIReader", "axext.CustomGUIReader", "ax.Object", nullptr);

    axlua::adapter::begin_module(L, "CustomGUIReader");
    axlua::adapter::set_function(L, "create", axlua_CustomGUIReader_create);
    axlua::adapter::end_module(L);
    axlua::register_native_type_name(typeid(ax::ext::CustomGUIReader), "axext.CustomGUIReader");

    axlua::adapter::end_module(L);
    return 1;
}

int register_sceneext_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_sceneext(L);
        register_all_ax_sceneext_adapter(L);
    }
    lua_pop(L, 1);
    // The compatibility-only extension registration manages its own global
    // module stack and must not consume the caller's `_G` sentinel.
    lua_register_ax_sceneext_CustomGUIReader(L);
    return 1;
}
