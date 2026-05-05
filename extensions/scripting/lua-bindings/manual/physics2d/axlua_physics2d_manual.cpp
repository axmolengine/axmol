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

#include "lua-bindings/manual/base/axlua_base_manual.hpp"

#if defined(AX_ENABLE_PHYSICS_2D)
#    include "lua-bindings/manual/tolua_fix.h"
#    include "lua-bindings/manual/LuaBasicConversions.h"
#    include "lua-bindings/manual/LuaValue.h"
#    include "lua-bindings/manual/LuaEngine.h"
#    include "axmol/scene/Scene.h"

#    ifndef AX_SAFE_DELETE_ARRAY
#        define do                 \
            {                      \
                if (p)             \
                {                  \
                    delete[] (p);  \
                    (p) = nullptr; \
                }                  \
            }                      \
            while (0)
#    endif

int axlua_physics2d_PhysicsWorld2D_getScene(lua_State* tolua_S)
{
    int argc                = 0;
    ax::PhysicsWorld2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S, "invalid 'obj' in function 'axlua_physics2d_PhysicsWorld2D_getScene'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        ax::Scene* ret = obj->getScene();
        do
        {
            auto className = getLuaTypeName<Object>(ret, "ax.Scene");

            int ID     = (int)(ret->_ID);
            int* luaID = &(ret->_luaID);
            toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)(&ret), className);

        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getScene", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_PhysicsWorld2D_getScene'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics2d_PhysicsWorld2D_rayCast(lua_State* tolua_S)
{
    int argc                = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok                 = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S, "invalid 'obj' in function 'axlua_physics2d_PhysicsWorld2D_rayCast'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        std::function<bool(ax::PhysicsWorld2D&, const ax::RayCastHit2D&, void*)> arg0;
        ax::Vec2 arg1;
        ax::Vec2 arg2;
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto stack           = LuaEngine::getInstance()->getLuaStack();
        do
        {
            arg0 = [handler, stack](ax::PhysicsWorld2D& world, const ax::RayCastHit2D& info, void* data) -> bool {
                auto Ls = stack->getLuaState();
                tolua_pushusertype(Ls, (void*)(&world), getLuaTypeName(&world, "ax.PhysicsWorld2D"));
                physics_raycastinfo_to_luaval(Ls, info);
                return stack->executeFunctionByHandler(handler, 2);
            };
        } while (0);

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsWorld2D:rayCast");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsWorld2D:rayCast");
        if (!ok)
        {
            stack->removeScriptHandler(handler);
            return 0;
        }
        obj->rayCast(arg0, Ray2D::fromPoints(arg1, arg2), nullptr);
        stack->removeScriptHandler(handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "rayCast", argc, 4);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_PhysicsWorld2D_rayCast'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics2d_Collider2D_recenterPoints(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Collider2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.Collider2D:recenterPoints");
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
        vec2_array_to_luaval(tolua_S, arg0, arg1);
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
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.Collider2D:recenterPoints");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_vec2(tolua_S, 3, &arg2, "ax.Collider2D:recenterPoints");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::Collider2D::recenterPoints(arg0, arg1, arg2);
        vec2_array_to_luaval(tolua_S, arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "recenterPoints", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_Collider2D_recenterPoints'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics2d_Collider2D_getPolygonCenter(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Collider2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.Collider2D:getPolygonCenter");
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
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "getPolygonCenter", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_Collider2D_getPolygonCenter'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics2d_PolygonCollider2D_getPoints(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::PolygonCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PolygonCollider2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    obj = (ax::PolygonCollider2D*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S, "invalid 'obj' in function 'axlua_physics2d_PolygonCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_PolygonCollider2D_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics2d_EdgePolygonCollider2D_getPoints(lua_State* tolua_S)
{
    int argc                       = 0;
    ax::EdgePolygonCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.EdgePolygonCollider2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    obj = (ax::EdgePolygonCollider2D*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S, "invalid 'obj' in function 'axlua_physics2d_EdgePolygonCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_EdgePolygonCollider2D_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics2d_EdgeChainCollider2D_getPoints(lua_State* tolua_S)
{
    int argc                     = 0;
    ax::EdgeChainCollider2D* obj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.EdgeChainCollider2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    obj = (ax::EdgeChainCollider2D*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S, "invalid 'obj' in function 'axlua_physics2d_EdgeChainCollider2D_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count      = obj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        obj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics2d_EdgeChainCollider2D_getPoints'.", &tolua_err);
#    endif

    return 0;
}

static int toaxlua_ContactEventListener2D_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                     = 0;
    ContactEventListener2D* self = nullptr;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.ContactEventListener2D", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    self = static_cast<ContactEventListener2D*>(tolua_tousertype(tolua_S, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_ContactEventListener2D_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#    if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#    endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_2D_CONTACT_BEGIN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactBegin = [handler](ContactEvent2D* event) -> bool {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(event, "ax.ContactEvent2D");
                bool ret = stack->executeFunctionByHandler(handler, 1);
                stack->clean();

                return ret;
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_2D_CONTACT_END:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactEnd = [handler](ContactEvent2D* event) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(event, "ax.ContactEvent2D");
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_2D_HIT:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onCollisionHit = [handler](ContactEvent2D* event) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(event, "ax.ContactEvent2D");
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_2D_SENSOR_BEGIN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onSensorBegin = [handler](ContactEvent2D* event) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(event, "ax.ContactEvent2D");
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_2D_SENSOR_END:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onSensorEnd = [handler](ContactEvent2D* event) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(event, "ax.ContactEvent2D");
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            };
        }
        break;
        default:
            break;
        }
        return 0;
    }

    luaL_error(tolua_S, "'registerScriptHandler' has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'registerScriptHandler'.", &tolua_err);
    return 0;
#    endif
}

int register_all_ax_physics2d_manual(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Collider2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "recenterPoints");
        lua_pushcfunction(tolua_S, axlua_physics2d_Collider2D_recenterPoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "getPolygonCenter");
        lua_pushcfunction(tolua_S, axlua_physics2d_Collider2D_getPolygonCenter);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "getPolyonCenter");
        lua_pushcfunction(tolua_S, axlua_physics2d_Collider2D_getPolygonCenter);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PolygonCollider2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics2d_PolygonCollider2D_getPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.EdgePolygonCollider2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics2d_EdgePolygonCollider2D_getPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.EdgeChainCollider2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics2d_EdgeChainCollider2D_getPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsWorld2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getScene");
        lua_pushcfunction(tolua_S, axlua_physics2d_PhysicsWorld2D_getScene);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "rayCast");
        lua_pushcfunction(tolua_S, axlua_physics2d_PhysicsWorld2D_rayCast);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_NONE");
        lua_pushnumber(tolua_S, PhysicsWorld2D::DEBUGDRAW_NONE);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_SHAPE");
        lua_pushnumber(tolua_S, PhysicsWorld2D::DEBUGDRAW_SHAPE);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_JOINT");
        lua_pushnumber(tolua_S, PhysicsWorld2D::DEBUGDRAW_JOINT);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_CONTACT");
        lua_pushnumber(tolua_S, PhysicsWorld2D::DEBUGDRAW_CONTACT);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_ALL");
        lua_pushnumber(tolua_S, PhysicsWorld2D::DEBUGDRAW_ALL);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.ContactEventListener2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "registerScriptHandler", toaxlua_ContactEventListener2D_registerScriptHandler);
    }
    lua_pop(tolua_S, 1);

    tolua_constant(tolua_S, "PHYSICS_INFINITY_2D", phconsts::LinearHuge);

    return 0;
}

#endif
