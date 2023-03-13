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
#include "scripting/lua-bindings/manual/extension/axlua_extension_manual.h"
#include "scripting/lua-bindings/auto/axlua_extension_auto.hpp"

#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/CCLuaValue.h"
#include "axmol-ext.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "scripting/lua-bindings/manual/base/LuaScriptHandlerMgr.h"

USING_NS_AX;
USING_NS_AX_EXT;

class LuaAssetsManagerDelegateProtocol : public Ref, public AssetsManagerDelegateProtocol
{
public:
    virtual ~LuaAssetsManagerDelegateProtocol() {}

    virtual void onProgress(int percent) override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData(percent);
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS, (void*)&data);
        }
    }

    virtual void onSuccess() override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_SUCCESS);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData;
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_SUCCESS, (void*)&data);
        }
    }

    virtual void onError(AssetsManager::ErrorCode errorCode) override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_ERROR);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData((int)errorCode);
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_ERROR, (void*)&data);
        }
    }
};

static int axlua_AssetsManager_setDelegate(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc            = 0;
    AssetsManager* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AssetsManager", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (AssetsManager*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_AssetsManager_setDelegate'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err) || !tolua_isnumber(L, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LuaAssetsManagerDelegateProtocol* delegate =
            dynamic_cast<LuaAssetsManagerDelegateProtocol*>(self->getDelegate());
        if (nullptr == delegate)
        {
            delegate = new LuaAssetsManagerDelegateProtocol();
            self->setUserObject(delegate);
            self->setDelegate(delegate);
            delegate->release();
        }

        LUA_FUNCTION handler                      = toluafix_ref_function(L, 2, 0);
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType)(
            (int)tolua_tonumber(L, 3, 0) + (int)ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)delegate, handler, handlerType);
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.AssetsManager:setDelegate", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_AssetsManager_setDelegate'.", &tolua_err);
    return 0;
#endif
}

static void extendAssetsManager(lua_State* L)
{
    lua_pushstring(L, "ax.AssetsManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setDelegate", axlua_AssetsManager_setDelegate);
    }
    lua_pop(L, 1);
}

static void extendManifest(lua_State* L)
{
    lua_pushstring(L, "ax.Manifest");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);
}

static int axlua_Extension_EventListenerAssetsManagerEx_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L, 1, "ax.EventListenerAssetsManagerEx", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 2)
    {

#if _AX_DEBUG >= 1
        if (!tolua_isusertype(L, 2, "ax.AssetsManagerEx", 0, &tolua_err) ||
            !toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif
        ax::extension::AssetsManagerEx* assetManager =
            static_cast<ax::extension::AssetsManagerEx*>(tolua_tousertype(L, 2, nullptr));

        LUA_FUNCTION handler = toluafix_ref_function(L, 3, 0);

        ax::extension::EventListenerAssetsManagerEx* ret =
            ax::extension::EventListenerAssetsManagerEx::create(assetManager, [=](EventAssetsManagerEx* event) {
                auto stack = LuaEngine::getInstance()->getLuaStack();
                int id     = event ? (int)event->_ID : -1;
                int* luaID = event ? &event->_luaID : nullptr;
                toluafix_pushusertype_ccobject(stack->getLuaState(), id, luaID, (void*)event,
                                               "ax.EventAssetsManagerEx");
                stack->executeFunctionByHandler(handler, 1);
            });

        int id     = (ret) ? (int)ret->_ID : -1;
        int* luaID = (ret) ? &ret->_luaID : nullptr;
        toluafix_pushusertype_ccobject(L, id, luaID, (void*)ret, "ax.EventListenerAssetsManagerEx");
        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Extension_EventListenerAssetsManagerEx_create'.", &tolua_err);
    return 0;
#endif
}

static void extendEventListenerAssetsManagerEx(lua_State* L)
{
    lua_pushstring(L, "ax.EventListenerAssetsManagerEx");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_Extension_EventListenerAssetsManagerEx_create);
    }
    lua_pop(L, 1);
}

int axlua_extension_ParticleSystem3D_getParticlePool(lua_State* tolua_S)
{
    int argc                        = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok                         = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.ParticleSystem3D", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_extension_ParticleSystem3D_getParticlePool'",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S,
                        "invalid arguments in function 'axlua_extension_ParticleSystem3D_getParticlePool'",
                        nullptr);
            return 0;
        }
        const ax::ParticlePool& ret = cobj->getParticlePool();
        tolua_pushusertype(tolua_S, (void*)&ret, "ax.ParticlePool");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticleSystem3D:getParticlePool", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_extension_ParticleSystem3D_getParticlePool'.", &tolua_err);
#endif

    return 0;
}

static void extendParticleSystem3D(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ParticleSystem3D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getParticlePool", axlua_extension_ParticleSystem3D_getParticlePool);
    }
    lua_pop(tolua_S, 1);
}

int axlua_extension_ParticlePool_getActiveDataList(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::ParticlePool* cobj = nullptr;
    bool ok                     = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.ParticlePool", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::ParticlePool*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_extension_ParticlePool_getActiveDataList'",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S,
                        "invalid arguments in function 'axlua_extension_ParticlePool_getActiveDataList'",
                        nullptr);
            return 0;
        }
        const ParticlePool::PoolList& ret = cobj->getActiveDataList();

        lua_newtable(tolua_S);
        if (ret.empty())
            return 1;

        int index = 1;
        for (const auto& obj : ret)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(tolua_S, (lua_Number)index);
            tolua_pushusertype(tolua_S, (void*)&ret, "ax.ParticlePool");
            lua_rawset(tolua_S, -3);
            ++index;
        }

        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticlePool:getActiveParticleList", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_extension_ParticlePool_getActiveParticleList'.",
                &tolua_err);
#endif

    return 0;
}

static void extendParticlePool(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ParticlePool");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getActiveDataList", axlua_extension_ParticlePool_getActiveDataList);
    }
    lua_pop(tolua_S, 1);
}

int register_all_ax_extension_manual(lua_State* tolua_S)
{
    extendAssetsManager(tolua_S);
    extendManifest(tolua_S);
    extendEventListenerAssetsManagerEx(tolua_S);
    extendParticleSystem3D(tolua_S);
    extendParticlePool(tolua_S);
    return 0;
}

int register_extension_module(lua_State* tolua_S)
{
    lua_getglobal(tolua_S, "_G");
    if (lua_istable(tolua_S, -1))  // stack:...,_G,
    {
        register_all_ax_extension(tolua_S);
        register_all_ax_extension_manual(tolua_S);
    }
    lua_pop(tolua_S, 1);
    return 1;
}
