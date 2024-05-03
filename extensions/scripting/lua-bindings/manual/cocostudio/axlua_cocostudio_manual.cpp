/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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
#include "lua-bindings/manual/cocostudio/axlua_cocostudio_manual.hpp"
#include "lua-bindings/auto/axlua_studio_auto.hpp"
#include "lua-bindings/auto/axlua_csloader_auto.hpp"
#include "lua-bindings/manual/cocostudio/axlua_csloader_manual.hpp"

#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"
#include "lua-bindings/manual/LuaValue.h"
#include "cocostudio/CocoStudio.h"
#include "cocostudio/ActionTimeline/ActionTimelineCache.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/cocostudio/CustomGUIReader.h"

using namespace cocostudio;

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
    int handler =
        ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this, ScriptHandlerMgr::HandlerType::ARMATURE_EVENT);

    if (0 != handler)
    {
        LuaArmatureWrapperEventData wrapperData(LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FILE_ASYNC,
                                                (void*)&percent);

        BasicScriptData data(this, (void*)&wrapperData);

        LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ARMATURE_EVENT, (void*)&data);
    }
}

static int axlua_ArmatureAnimation_setMovementEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                = 0;
    ArmatureAnimation* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ccs.ArmatureAnimation", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ArmatureAnimation*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ArmatureAnimation_setMovementEventCallFunc'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        Vector<LuaArmatureWrapper*> vec;
        vec.pushBack(wrapper);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                          ScriptHandlerMgr::HandlerType::ARMATURE_EVENT);

        self->setMovementEventCallFunc(
            [=](Armature* armature, MovementEventType movementType, std::string_view movementID) {
                if (0 != handler)
                {
                    LuaArmatureMovementEventData movementData(armature, (int)movementType, movementID);

                    LuaArmatureWrapperEventData wrapperData(
                        LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::MOVEMENT_EVENT, (void*)&movementData);

                    BasicScriptData data((void*)vec.at(0), (void*)&wrapperData);

                    LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ARMATURE_EVENT, (void*)&data);
                }
            });
        return 0;
    }

    luaL_error(L,
               "'setMovementEventCallFunc' function of ArmatureAnimation has wrong number of arguments: %d, was "
               "expecting %d\n",
               argc, 1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'setMovementEventCallFunc'.", &tolua_err);
    return 0;
#endif
}

static int axlua_ArmatureAnimation_setFrameEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                = 0;
    ArmatureAnimation* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ccs.ArmatureAnimation", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ArmatureAnimation*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ArmatureAnimation_setFrameEventCallFunc'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        Vector<LuaArmatureWrapper*> vec;
        vec.pushBack(wrapper);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                          ScriptHandlerMgr::HandlerType::ARMATURE_EVENT);

        self->setFrameEventCallFunc(
            [=](cocostudio::Bone* bone, std::string_view frameEventName, int originFrameIndex, int currentFrameIndex) {
                if (0 != handler)
                {
                    std::string strFrameEventName(frameEventName);

                    LuaArmatureFrameEventData frameData(bone, frameEventName, originFrameIndex, currentFrameIndex);

                    LuaArmatureWrapperEventData wrapperData(
                        LuaArmatureWrapperEventData::LuaArmatureWrapperEventType::FRAME_EVENT, (void*)&frameData);

                    BasicScriptData data((void*)vec.at(0), (void*)&wrapperData);

                    LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ARMATURE_EVENT, (void*)&data);
                }
            });

        return 0;
    }

    luaL_error(
        L,
        "'setFrameEventCallFunc' function of ArmatureAnimation has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'setFrameEventCallFunc'.", &tolua_err);
#endif
    return 0;
}

static void extendArmatureAnimation(lua_State* L)
{
    lua_pushstring(L, "ccs.ArmatureAnimation");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setMovementEventCallFunc", axlua_ArmatureAnimation_setMovementEventCallFunc);
        tolua_function(L, "setFrameEventCallFunc", axlua_ArmatureAnimation_setFrameEventCallFunc);
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
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ccs.ArmatureDataManager", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ArmatureDataManager*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L,
                    "invalid 'self' in function 'axlua_ArmatureDataManager_addArmatureFileInfoAsyncCallFunc'\n",
                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err) || !toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* configFilePath = tolua_tostring(L, 2, "");
        LUA_FUNCTION handler       = (toluafix_ref_function(L, 3, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                          ScriptHandlerMgr::HandlerType::ARMATURE_EVENT);

        self->addArmatureFileInfoAsync(configFilePath, wrapper,
                                       AX_SCHEDULE_SELECTOR(LuaArmatureWrapper::addArmatureFileInfoAsyncCallback));

        return 0;
    }
    else if (4 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err) || !tolua_isstring(L, 3, 0, &tolua_err) ||
            !tolua_isstring(L, 4, 0, &tolua_err) || !toluafix_isfunction(L, 5, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* imagePath      = tolua_tostring(L, 2, "");
        const char* plistPath      = tolua_tostring(L, 3, "");
        const char* configFilePath = tolua_tostring(L, 4, "");

        LUA_FUNCTION handler = (toluafix_ref_function(L, 5, 0));

        LuaArmatureWrapper* wrapper = new LuaArmatureWrapper();
        wrapper->autorelease();

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)wrapper, handler,
                                                          ScriptHandlerMgr::HandlerType::ARMATURE_EVENT);

        self->addArmatureFileInfoAsync(imagePath, plistPath, configFilePath, wrapper,
                                       AX_SCHEDULE_SELECTOR(LuaArmatureWrapper::addArmatureFileInfoAsyncCallback));

        return 0;
    }

    luaL_error(L,
               "'addArmatureFileInfoAsync' function of ArmatureDataManager has wrong number of arguments: %d, was "
               "expecting %d\n",
               argc, 1);

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addArmatureFileInfoAsync'.", &tolua_err);
#endif
    return 0;
}

static void extendArmatureDataManager(lua_State* L)
{
    lua_pushstring(L, "ccs.ArmatureDataManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addArmatureFileInfoAsync",
                       axlua_ArmatureDataManager_addArmatureFileInfoAsyncCallFunc);
    }
    lua_pop(L, 1);
}

static int axlua_extension_Bone_setIgnoreMovementBoneData(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc               = 0;
    cocostudio::Bone* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ccs.Bone", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<cocostudio::Bone*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_extension_Bone_setIgnoreMovementBoneData'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        bool ignore = (bool)tolua_toboolean(L, 2, 0);
        self->setIgnoreMovementBoneData(ignore);
        return 0;
    }

    luaL_error(L, "'setIgnoreMovementBoneData' function of Bone  has wrong number of arguments: %d, was expecting %d\n",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'setIgnoreMovementBoneData'.", &tolua_err);
    return 0;
#endif
}

static void extendBone(lua_State* L)
{
    lua_pushstring(L, "ccs.Bone");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setIgnoreMovementBoneData", axlua_extension_Bone_setIgnoreMovementBoneData);
    }
    lua_pop(L, 1);
}

int axlua_studio_ActionTimelineCache_getInstance(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ccs.ActionTimelineCache", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        if (!ok)
            return 0;
        cocostudio::timeline::ActionTimelineCache* ret = cocostudio::timeline::ActionTimelineCache::getInstance();
        tolua_pushusertype(L, (void*)ret, "ccs.ActionTimelineCache");
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccs.ActionTimelineCache:getInstance",
               argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_studio_ActionTimelineCache_getInstance'.", &tolua_err);
#endif
    return 0;
}

static void extendActionTimelineCache(lua_State* L)
{
    lua_pushstring(L, "ccs.ActionTimelineCache");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "getInstance", axlua_studio_ActionTimelineCache_getInstance);
    }
    lua_pop(L, 1);
}

static int axlua_ActionTimeline_setFrameEventCallFunc(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                                   = 0;
    cocostudio::timeline::ActionTimeline* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ccs.ActionTimeline", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<cocostudio::timeline::ActionTimeline*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ActionTimeline_setFrameEventCallFunc'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        self->setFrameEventCallFunc([=](cocostudio::timeline::Frame* frame) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            toluafix_pushusertype_ccobject(stack->getLuaState(), frame->_ID, &frame->_luaID, (void*)frame,
                                           getLuaTypeName(frame, "ccs.Frame"));
            stack->executeFunctionByHandler(handler, 1);
        });

        return 0;
    }

    luaL_error(
        L, "'setFrameEventCallFunc' function of ActionTimeline has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'setFrameEventCallFunc'.", &tolua_err);
#endif
    return 0;
}

static void extendActionTimeline(lua_State* L)
{
    lua_pushstring(L, "ccs.ActionTimeline");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setFrameEventCallFunc", axlua_ActionTimeline_setFrameEventCallFunc);
    }
    lua_pop(L, 1);
}

int axlua_CustomGUIReader_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ccs.CustomGUIReader", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    do
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(L, 2, &arg0, "ccs.CustomGUIReader:create");
            if (!ok)
            {
                break;
            }
#if _AX_DEBUG >= 1
            if (!toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            LUA_FUNCTION arg1 = toluafix_ref_function(L, 3, 0);
#if _AX_DEBUG >= 1
            if (!toluafix_isfunction(L, 4, "LUA_FUNCTION", 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            LUA_FUNCTION arg2 = toluafix_ref_function(L, 4, 0);

            cocostudio::CustomGUIReader* ret = cocostudio::CustomGUIReader::create(arg0, arg1, arg2);
            object_to_luaval<cocostudio::CustomGUIReader>(L, "ccs.CustomGUIReader", (cocostudio::CustomGUIReader*)ret);
            return 1;
        }
    } while (0);
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d", "ccs.CustomGUIReader:create", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_CustomGUIReader_create'.", &tolua_err);
#endif
    return 0;
}

int register_all_ax_coco_studio_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;
    extendArmatureAnimation(L);
    extendArmatureDataManager(L);
    extendBone(L);
    extendActionTimelineCache(L);
    extendActionTimeline(L);

    return 0;
}

int lua_register_axis_coco_studio_CustomGUIReader(lua_State* L)
{
    tolua_module(L, "ccs", 0);
    tolua_beginmodule(L, "ccs");

    tolua_usertype(L, "ccs.CustomGUIReader");
    tolua_cclass(L, "CustomGUIReader", "ccs.CustomGUIReader", "ax.Object", nullptr);

    tolua_beginmodule(L, "CustomGUIReader");
    tolua_function(L, "create", axlua_CustomGUIReader_create);
    tolua_endmodule(L);
    auto typeName                                    = typeid(cocostudio::CustomGUIReader).name();
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ccs.CustomGUIReader";
    g_typeCast[typeName]                             = "ccs.CustomGUIReader";

    tolua_endmodule(L);
    return 1;
}

int register_cocostudio_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_studio(L);
        register_all_ax_csloader(L);
        register_all_ax_coco_studio_manual(L);
        register_all_ax_csloader_manual(L);
        lua_register_axis_coco_studio_CustomGUIReader(L);
    }
    lua_pop(L, 1);
    return 1;
}
