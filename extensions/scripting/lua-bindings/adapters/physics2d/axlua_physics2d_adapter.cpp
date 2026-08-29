/****************************************************************************
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

#include "lua-bindings/adapters/base/axlua_base_adapter.h"

#if defined(AX_ENABLE_PHYSICS_2D)
#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "lua-bindings/runtime/axlua_conversions.h"
#    include "lua-bindings/runtime/LuaEngine.h"
#    include "axmol/scene/Scene.h"

#    ifndef AX_SAFE_DELETE_ARRAY
#        define AX_SAFE_DELETE_ARRAY(p) \
            do                     \
            {                      \
                delete[] (p);      \
                (p) = nullptr;     \
            }                      \
            while (0)
#    endif

int axlua_physics2d_PhysicsWorld2D_getScene(lua_State* luaState)
{
    int argc                = 0;
    ax::PhysicsWorld2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PhysicsWorld2D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::PhysicsWorld2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_physics2d_PhysicsWorld2D_getScene'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        ax::Scene* ret = obj->getScene();
        do
        {
            auto className = getLuaTypeName<Object>(ret, "ax.Scene");

            axlua::adapter::push_object(luaState, (void*)(&ret), className);

        } while (0);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "getScene", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_PhysicsWorld2D_getScene'.", &conversionError);
#    endif

    return 0;
}

int axlua_physics2d_PhysicsWorld2D_rayCast(lua_State* luaState)
{
    int argc                = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok                 = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PhysicsWorld2D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::PhysicsWorld2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_physics2d_PhysicsWorld2D_rayCast'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        std::function<bool(ax::PhysicsWorld2D&, const ax::RayCastHit2D&, void*)> arg0;
        ax::Vec2 arg1;
        ax::Vec2 arg2;
        LUA_FUNCTION handler = axlua::adapter::ref_function(luaState, 2, 0);
        auto stack           = LuaEngine::getInstance()->getLuaStack();
        do
        {
            arg0 = [handler, stack](ax::PhysicsWorld2D& world, const ax::RayCastHit2D& info, void* data) -> bool {
                auto Ls = stack->getLuaState();
                axlua::adapter::push_usertype(Ls, (void*)(&world), getLuaTypeName(&world, "ax.PhysicsWorld2D"));
                physics_raycastinfo_to_luaval(Ls, info);
                return stack->executeFunctionByHandler(handler, 2);
            };
        } while (0);

        ok &= luaval_to_vec2(luaState, 3, &arg1, "ax.PhysicsWorld2D:rayCast");
        ok &= luaval_to_vec2(luaState, 4, &arg2, "ax.PhysicsWorld2D:rayCast");
        if (!ok)
        {
            stack->removeScriptHandler(handler);
            return 0;
        }
        obj->rayCast(arg0, Ray2D::fromPoints(arg1, arg2), nullptr);
        stack->removeScriptHandler(handler);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "rayCast", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_PhysicsWorld2D_rayCast'.", &conversionError);
#    endif

    return 0;
}

int axlua_physics2d_Collider2D_recenterPoints(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Collider2D", 0, &conversionError))
        goto argumentError;
#    endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(luaState, 2, &arg0, &arg1, "ax.Collider2D:recenterPoints");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::Collider2D::recenterPoints(arg0, arg1);
        vec2_array_to_luaval(luaState, arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);

        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::Vec2 arg2;
        do
        {
            ok = luaval_to_array_of_vec2(luaState, 2, &arg0, &arg1, "ax.Collider2D:recenterPoints");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_vec2(luaState, 3, &arg2, "ax.Collider2D:recenterPoints");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::Collider2D::recenterPoints(arg0, arg1, arg2);
        vec2_array_to_luaval(luaState, arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ", "recenterPoints", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_Collider2D_recenterPoints'.", &conversionError);
#    endif
    return 0;
}

int axlua_physics2d_Collider2D_getPolygonCenter(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Collider2D", 0, &conversionError))
        goto argumentError;
#    endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(luaState, 2, &arg0, &arg1, "ax.Collider2D:getPolygonCenter");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::Vec2 ret = ax::Collider2D::getPolygonCenter(std::span(arg0, arg1));
        AX_SAFE_DELETE_ARRAY(arg0);
        vec2_to_luaval(luaState, ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ", "getPolygonCenter", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_Collider2D_getPolygonCenter'.", &conversionError);
#    endif
    return 0;
}

int axlua_physics2d_PolygonCollider2D_getPoints(lua_State* luaState)
{
    int argc                   = 0;
    ax::PolygonCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PolygonCollider2D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::PolygonCollider2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_physics2d_PolygonCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(luaState, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_PolygonCollider2D_getPoints'.", &conversionError);
#    endif

    return 0;
}

int axlua_physics2d_EdgePolygonCollider2D_getPoints(lua_State* luaState)
{
    int argc                       = 0;
    ax::EdgePolygonCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.EdgePolygonCollider2D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::EdgePolygonCollider2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_physics2d_EdgePolygonCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(luaState, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_EdgePolygonCollider2D_getPoints'.", &conversionError);
#    endif

    return 0;
}

int axlua_physics2d_EdgeChainCollider2D_getPoints(lua_State* luaState)
{
    int argc                     = 0;
    ax::EdgeChainCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.EdgeChainCollider2D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::EdgeChainCollider2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_physics2d_EdgeChainCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(luaState, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_physics2d_EdgeChainCollider2D_getPoints'.", &conversionError);
#    endif

    return 0;
}

int register_all_ax_physics2d_adapter(lua_State* luaState)
{
    lua_pushstring(luaState, "ax.Collider2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        lua_pushstring(luaState, "recenterPoints");
        lua_pushcfunction(luaState, axlua_physics2d_Collider2D_recenterPoints);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "getPolygonCenter");
        lua_pushcfunction(luaState, axlua_physics2d_Collider2D_getPolygonCenter);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "getPolyonCenter");
        lua_pushcfunction(luaState, axlua_physics2d_Collider2D_getPolygonCenter);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    lua_pushstring(luaState, "ax.PolygonCollider2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        lua_pushstring(luaState, "getPoints");
        lua_pushcfunction(luaState, axlua_physics2d_PolygonCollider2D_getPoints);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    lua_pushstring(luaState, "ax.EdgePolygonCollider2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        lua_pushstring(luaState, "getPoints");
        lua_pushcfunction(luaState, axlua_physics2d_EdgePolygonCollider2D_getPoints);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    lua_pushstring(luaState, "ax.EdgeChainCollider2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        lua_pushstring(luaState, "getPoints");
        lua_pushcfunction(luaState, axlua_physics2d_EdgeChainCollider2D_getPoints);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    lua_pushstring(luaState, "ax.PhysicsWorld2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        lua_pushstring(luaState, "getScene");
        lua_pushcfunction(luaState, axlua_physics2d_PhysicsWorld2D_getScene);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "rayCast");
        lua_pushcfunction(luaState, axlua_physics2d_PhysicsWorld2D_rayCast);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "DEBUGDRAW_NONE");
        lua_pushnumber(luaState, PhysicsWorld2D::DEBUGDRAW_NONE);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "DEBUGDRAW_SHAPE");
        lua_pushnumber(luaState, PhysicsWorld2D::DEBUGDRAW_SHAPE);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "DEBUGDRAW_JOINT");
        lua_pushnumber(luaState, PhysicsWorld2D::DEBUGDRAW_JOINT);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "DEBUGDRAW_CONTACT");
        lua_pushnumber(luaState, PhysicsWorld2D::DEBUGDRAW_CONTACT);
        lua_rawset(luaState, -3);
        lua_pushstring(luaState, "DEBUGDRAW_ALL");
        lua_pushnumber(luaState, PhysicsWorld2D::DEBUGDRAW_ALL);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    // The generated registrations no longer leave the `ax` module table on
    // the Lua stack.  Push the explicit compatibility target before using
    // the legacy helper, which intentionally writes to the table at -1.
    lua_getglobal(luaState, "ax");
    if (lua_istable(luaState, -1))
        axlua::adapter::set_constant(luaState, "PHYSICS_INFINITY_2D", phconsts::LinearHuge);
    lua_pop(luaState, 1);

    return 0;
}

#endif
