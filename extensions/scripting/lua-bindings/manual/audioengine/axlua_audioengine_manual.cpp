/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
#include "lua-bindings/manual/audioengine/axlua_audioengine_manual.h"
#include "lua-bindings/auto/axlua_audioengine_auto.hpp"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "audio/AudioEngine.h"

static int lua_get_AudioProfile_name(lua_State* L)
{
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_AudioProfile_name'\n", nullptr);
        return 0;
    }
#endif

    lua_pushstring(L, self->name.c_str());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_AudioProfile_name'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_AudioProfile_name(lua_State* L)
{
    int argc                    = 0;
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_AudioProfile_name'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        self->name = tolua_tostring(L, 2, 0);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_AudioProfile_name'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_AudioProfile_maxInstances(lua_State* L)
{
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_AudioProfile_maxInstances'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushnumber(L, (lua_Number)self->maxInstances);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_AudioProfile_maxInstances'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_AudioProfile_maxInstances(lua_State* L)
{
    int argc                    = 0;
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_AudioProfile_maxInstances'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        self->maxInstances = (unsigned)tolua_tonumber(L, 2, 0);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_AudioProfile_maxInstances'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_AudioProfile_minDelay(lua_State* L)
{
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_AudioProfile_minDelay'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushnumber(L, (lua_Number)self->minDelay);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_AudioProfile_minDelay'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_AudioProfile_minDelay(lua_State* L)
{
    int argc                    = 0;
    ax::AudioProfile* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AudioProfile", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AudioProfile*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_AudioProfile_minDelay'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        self->minDelay = tolua_tonumber(L, 2, 0);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_AudioProfile_minDelay'.", &tolua_err);
    return 0;
#endif
}

int axlua_audioengine_AudioEngine_setFinishCallback(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.AudioEngine", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int*)&arg0, "ax.AudioEngine:setFinishCallback");

#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 3, 0));

        ax::AudioEngine::setFinishCallback(arg0, [=](int audioID, std::string_view filePath) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();

            stack->pushInt(audioID);
            stack->pushString(filePath);

            stack->executeFunctionByHandler(handler, 2);

            stack->removeScriptHandler(handler);
        });

        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setFinishCallback",
               argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_audioengine_AudioEngine_setFinishCallback'.", &tolua_err);
#endif
    return 0;
}

int register_audioengine_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_audioengine(L);
        if (L)
        {
            lua_pushstring(L, "ax.AudioProfile");
            lua_rawget(L, LUA_REGISTRYINDEX);
            if (lua_istable(L, -1))
            {
                tolua_variable(L, "name", lua_get_AudioProfile_name, lua_set_AudioProfile_name);
                tolua_variable(L, "maxInstances", lua_get_AudioProfile_maxInstances, lua_set_AudioProfile_maxInstances);
                tolua_variable(L, "minDelay", lua_get_AudioProfile_minDelay, lua_set_AudioProfile_minDelay);
            }
            lua_pop(L, 1);

            lua_pushstring(L, "ax.AudioEngine");
            lua_rawget(L, LUA_REGISTRYINDEX);
            if (lua_istable(L, -1))
            {
                tolua_function(L, "setFinishCallback", axlua_audioengine_AudioEngine_setFinishCallback);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return 1;
}
