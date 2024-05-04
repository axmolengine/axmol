/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "lua-bindings/manual/base/axlua_base_manual.hpp"

#if defined(AX_ENABLE_PHYSICS)
#    include "lua-bindings/manual/tolua_fix.h"
#    include "lua-bindings/manual/LuaBasicConversions.h"
#    include "lua-bindings/manual/LuaValue.h"
#    include "lua-bindings/manual/LuaEngine.h"
#    include "2d/Scene.h"

#    ifndef AX_SAFE_DELETE_ARRAY
#        define do                 \
            {                      \
                if (p)             \
                {                  \
                    delete[](p);   \
                    (p) = nullptr; \
                }                  \
            }                      \
            while (0)
#    endif

int axlua_physics_PhysicsBody_getJoints(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok                    = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsBody", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsBody_getJoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        do
        {

            if (!ok)
                return 0;
            auto& ret = cobj->getJoints();

            lua_newtable(tolua_S);

            if (ret.empty())
                return 1;

            auto iter      = ret.begin();
            int indexTable = 1;
            for (; iter != ret.end(); ++iter)
            {
                if (nullptr == *iter)
                    continue;

                lua_pushnumber(tolua_S, (lua_Number)indexTable);
                tolua_pushusertype(tolua_S, (void*)(*iter), getLuaTypeName(*iter, "ax.PhysicsJoint"));
                lua_rawset(tolua_S, -3);
                ++indexTable;
            }
        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getJoints", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsBody_getJoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsWorld_getScene(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::PhysicsWorld* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsWorld_getScene'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        ax::Scene& ret = cobj->getScene();
        do
        {
            auto className = getLuaTypeName<Object>(&ret, "ax.Scene");

            int ID     = (int)(ret._ID);
            int* luaID = &(ret._luaID);
            toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)(&ret), className);

        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getScene", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsWorld_getScene'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsWorld_rayCast(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok                     = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsWorld_rayCast'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        std::function<bool(ax::PhysicsWorld&, const ax::PhysicsRayCastInfo&, void*)> arg0;
        ax::Vec2 arg1;
        ax::Vec2 arg2;
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto stack           = LuaEngine::getInstance()->getLuaStack();
        do
        {
            arg0 = [handler, stack](ax::PhysicsWorld& world, const ax::PhysicsRayCastInfo& info,
                             void* data) -> bool {
                
                auto Ls    = stack->getLuaState();
                tolua_pushusertype(Ls, (void*)(&world), getLuaTypeName(&world, "ax.PhysicsWorld"));
                physics_raycastinfo_to_luaval(Ls, info);
                return stack->executeFunctionByHandler(handler, 2);
            };
        } while (0);

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsWorld:rayCast");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsWorld:rayCast");
        if (!ok)
        {
            stack->removeScriptHandler(handler);
            return 0;
        }
        cobj->rayCast(arg0, arg1, arg2, nullptr);
        stack->removeScriptHandler(handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "rayCast", argc, 4);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsWorld_rayCast'.", &tolua_err);
#    endif

    return 0;
}
int axlua_physics_PhysicsWorld_queryRect(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok                     = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsWorld_queryRect'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
        std::function<bool(ax::PhysicsWorld&, ax::PhysicsShape&, void*)> arg0;
        ax::Rect arg1;
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto stack           = LuaEngine::getInstance()->getLuaStack();
        do
        {
            arg0 = [handler,stack](ax::PhysicsWorld& world, ax::PhysicsShape& shape, void* data) -> bool {
               
                auto Ls    = stack->getLuaState();
                tolua_pushusertype(Ls, (void*)(&world), getLuaTypeName(&world, "ax.PhysicsWorld"));
                toluafix_pushusertype_ccobject(Ls, shape._ID, &shape._luaID, (void*)(&shape), "ax.PhysicsShape");
                return stack->executeFunctionByHandler(handler, 2);
            };
        } while (0);

        ok &= luaval_to_rect(tolua_S, 3, &arg1, "ax.PhysicsWorld:queryRect");
        if (!ok)
        {
            stack->removeScriptHandler(handler);
            return 0;
        }
        cobj->queryRect(arg0, arg1, nullptr);
        stack->removeScriptHandler(handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "queryRect", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsWorld_queryRect'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsWorld_queryPoint(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok                     = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsWorld", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsWorld_queryPoint'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
        std::function<bool(ax::PhysicsWorld&, ax::PhysicsShape&, void*)> arg0;
        ax::Vec2 arg1;
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto stack           = LuaEngine::getInstance()->getLuaStack();
        do
        {
            arg0 = [handler,stack](ax::PhysicsWorld& world, ax::PhysicsShape& shape, void* data) -> bool {
                auto Ls    = stack->getLuaState();
                tolua_pushusertype(Ls, (void*)(&world), getLuaTypeName(&world, "ax.PhysicsWorld"));
                toluafix_pushusertype_ccobject(Ls, shape._ID, &shape._luaID, (void*)(&shape), "ax.PhysicsShape");
                return stack->executeFunctionByHandler(handler, 2);
            };
            assert(false);
        } while (0);
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsWorld:queryPoint");
        if (!ok)
        {
            stack->removeScriptHandler(handler);
            return 0;
        }
        cobj->queryPoint(arg0, arg1, nullptr);
        stack->removeScriptHandler(handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "queryPoint", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsWorld_queryPoint'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsBody_createPolygon(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsBody", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0 = nullptr;
        int arg1            = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createPolygon");
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
        ax::PhysicsBody* ret = ax::PhysicsBody::createPolygon(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createPolygon");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createPolygon");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createPolygon(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {

            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        ax::Vec2 arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createPolygon");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createPolygon");
        ok &= luaval_to_vec2(tolua_S, 4, &arg3, "ax.PhysicsBody:createPolygon");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createPolygon(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "createPolygon", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsBody_createPolygon'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsBody_createEdgePolygon(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsBody", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgePolygon");
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
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgePolygon(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgePolygon");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createEdgePolygon");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgePolygon(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1;
        ax::PhysicsMaterial arg2;
        double arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgePolygon");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createEdgePolygon");
        ok &= luaval_to_number(tolua_S, 4, &arg3, "ax.PhysicsBody:createEdgePolygon");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgePolygon(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "createEdgePolygon", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsBody_createEdgePolygon'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsBody_createEdgeChain(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsBody", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgeChain");
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
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgeChain(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgeChain");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createEdgeChain");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgeChain(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1;
        ax::PhysicsMaterial arg2;
        double arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsBody:createEdgeChain");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsBody:createEdgeChain");
        ok &= luaval_to_number(tolua_S, 4, &arg3, "ax.PhysicsBody:createEdgeChain");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsBody* ret = ax::PhysicsBody::createEdgeChain(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        do
        {
            if (nullptr != ret)
            {
                int ID     = ret->_ID;
                int* luaID = &ret->_luaID;
                toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)ret, "ax.PhysicsBody");
            }
            else
            {
                lua_pushnil(tolua_S);
            }
        } while (0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "createEdgeChain", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsBody_createEdgeChain'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsShape_recenterPoints(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShape", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShape:recenterPoints");
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
        ax::PhysicsShape::recenterPoints(arg0, arg1);
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
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShape:recenterPoints");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_vec2(tolua_S, 3, &arg2, "ax.PhysicsShape:recenterPoints");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShape::recenterPoints(arg0, arg1, arg2);
        vec2_array_to_luaval(tolua_S, arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "recenterPoints", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShape_recenterPoints'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsShape_getPolygonCenter(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShape", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShape:getPolygonCenter");
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
        ax::Vec2 ret = ax::PhysicsShape::getPolygonCenter(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "getPolygonCenter", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShape_getPolygonCenter'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsShapeBox_getPoints(lua_State* tolua_S)
{
    int argc                       = 0;
    ax::PhysicsShapeBox* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsShapeBox", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsShapeBox*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsShapeBox_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        ax::Vec2 arg0[4];
        cobj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, 4);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeBox_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsShapePolygon_getPoints(lua_State* tolua_S)
{
    int argc                           = 0;
    ax::PhysicsShapePolygon* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsShapePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsShapePolygon*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsShapePolygon_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count           = cobj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        cobj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapePolygon_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsShapePolygon_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShapePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapePolygon:create");
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
        ax::PhysicsShapePolygon* ret = ax::PhysicsShapePolygon::create(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapePolygon>(tolua_S, "ax.PhysicsShapePolygon",
                                                       (ax::PhysicsShapePolygon*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapePolygon:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapePolygon:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapePolygon* ret = ax::PhysicsShapePolygon::create(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapePolygon>(tolua_S, "ax.PhysicsShapePolygon",
                                                       (ax::PhysicsShapePolygon*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        ax::Vec2 arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapePolygon:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapePolygon:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg3, "ax.PhysicsShapePolygon:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapePolygon* ret = ax::PhysicsShapePolygon::create(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapePolygon>(tolua_S, "ax.PhysicsShapePolygon",
                                                       (ax::PhysicsShapePolygon*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "create", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapePolygon_create'.", &tolua_err);
#    endif
    return 0;
}
int axlua_physics_PhysicsShapePolygon_calculateArea(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShapePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapePolygon:calculateArea");
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
        double ret = ax::PhysicsShapePolygon::calculateArea(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "calculateArea", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapePolygon_calculateArea'.", &tolua_err);
#    endif
    return 0;
}
int axlua_physics_PhysicsShapePolygon_calculateMoment(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShapePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        ax::Vec2* arg1;
        int arg2 = 0;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PhysicsShapePolygon:calculateMoment");
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 3, &arg1, &arg2, "ax.PhysicsShapePolygon:calculateMoment");
            if (nullptr == arg1)
            {
                LUA_PRECONDITION(arg1, "Invalid Native Object");
            }
        } while (0);
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg1);
            return 0;
        }
        double ret = ax::PhysicsShapePolygon::calculateMoment(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg1);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::Vec2* arg1;
        int arg2 = 0;
        ax::Vec2 arg3;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PhysicsShapePolygon:calculateMoment");
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 3, &arg1, &arg2, "ax.PhysicsShapePolygon:calculateMoment");
            if (nullptr == arg1)
            {
                LUA_PRECONDITION(arg1, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_vec2(tolua_S, 4, &arg3, "ax.PhysicsShapePolygon:calculateMoment");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg1);
            return 0;
        }
        double ret = ax::PhysicsShapePolygon::calculateMoment(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg1);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "calculateMoment", argc, 3);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapePolygon_calculateMoment'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsShapeEdgeBox_getPoints(lua_State* tolua_S)
{
    int argc                           = 0;
    ax::PhysicsShapeEdgeBox* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsShapeEdgeBox", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsShapeEdgeBox*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsShapeEdgeBox_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count           = cobj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        cobj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeEdgeBox_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsShapeEdgePolygon_getPoints(lua_State* tolua_S)
{
    int argc                               = 0;
    ax::PhysicsShapeEdgePolygon* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsShapeEdgePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsShapeEdgePolygon*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsShapeEdgePolygon_getPoints'",
                    NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count           = cobj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        cobj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeEdgePolygon_getPoints'.", &tolua_err);
#    endif

    return 0;
}

int axlua_physics_PhysicsShapeEdgeChain_getPoints(lua_State* tolua_S)
{
    int argc                             = 0;
    ax::PhysicsShapeEdgeChain* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.PhysicsShapeEdgeChain", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::PhysicsShapeEdgeChain*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_physics_PhysicsShapeEdgeChain_getPoints'", NULL);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int count           = cobj->getPointsCount();
        ax::Vec2* arg0 = new ax::Vec2[count];
        cobj->getPoints(arg0);
        vec2_array_to_luaval(tolua_S, arg0, count);
        AX_SAFE_DELETE_ARRAY(arg0);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getPoints", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeEdgeChain_getPoints'.", &tolua_err);
#    endif

    return 0;
}

static int toaxlua_EventListenerPhysicsContact_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                          = 0;
    EventListenerPhysicsContact* self = nullptr;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerPhysicsContact", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    self = static_cast<EventListenerPhysicsContact*>(tolua_tousertype(tolua_S, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,
                    "invalid 'self' in function 'toaxlua_EventListenerPhysicsContact_registerScriptHandler'\n",
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
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_CONTACT_BEGIN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactBegin = [handler](PhysicsContact& contact) -> bool {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(&contact, "ax.PhysicsContact");
                bool ret = stack->executeFunctionByHandler(handler, 1);
                stack->clean();

                return ret;
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_CONTACT_PRESOLVE:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactPreSolve = [handler](PhysicsContact& contact, PhysicsContactPreSolve& solve) -> bool {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(&contact, "ax.PhysicsContact");
                tolua_pushusertype(stack->getLuaState(), &solve, "ax.PhysicsContactPreSolve");
                bool ret = stack->executeFunctionByHandler(handler, 2);
                stack->clean();

                return ret;
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_CONTACT_POSTSOLVE:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactPostSolve = [handler](PhysicsContact& contact, const PhysicsContactPostSolve& solve) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(&contact, "ax.PhysicsContact");
                tolua_pushusertype(stack->getLuaState(), const_cast<PhysicsContactPostSolve*>(&solve),
                                   "ax.PhysicsContactPostSolve");
                stack->executeFunctionByHandler(handler, 2);
                stack->clean();
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_PHYSICS_CONTACT_SEPARATE:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onContactSeparate = [handler](PhysicsContact& contact) {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                stack->pushObject(&contact, "ax.PhysicsContact");
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

int axlua_physics_PhysicsShapeEdgePolygon_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShapeEdgePolygon", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgePolygon:create");
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
        ax::PhysicsShapeEdgePolygon* ret = ax::PhysicsShapeEdgePolygon::create(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgePolygon>(tolua_S, "ax.PhysicsShapeEdgePolygon",
                                                           (ax::PhysicsShapeEdgePolygon*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgePolygon:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapeEdgePolygon:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapeEdgePolygon* ret = ax::PhysicsShapeEdgePolygon::create(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgePolygon>(tolua_S, "ax.PhysicsShapeEdgePolygon",
                                                           (ax::PhysicsShapeEdgePolygon*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        double arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgePolygon:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapeEdgePolygon:create");
        ok &= luaval_to_number(tolua_S, 4, &arg3, "ax.PhysicsShapeEdgePolygon:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapeEdgePolygon* ret = ax::PhysicsShapeEdgePolygon::create(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgePolygon>(tolua_S, "ax.PhysicsShapeEdgePolygon",
                                                           (ax::PhysicsShapeEdgePolygon*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "create", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeEdgePolygon_create'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics_PhysicsShapeEdgeChain_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.PhysicsShapeEdgeChain", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgeChain:create");
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
        ax::PhysicsShapeEdgeChain* ret = ax::PhysicsShapeEdgeChain::create(arg0, arg1);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgeChain>(tolua_S, "ax.PhysicsShapeEdgeChain",
                                                         (ax::PhysicsShapeEdgeChain*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgeChain:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapeEdgeChain:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapeEdgeChain* ret = ax::PhysicsShapeEdgeChain::create(arg0, arg1, arg2);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgeChain>(tolua_S, "ax.PhysicsShapeEdgeChain",
                                                         (ax::PhysicsShapeEdgeChain*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2* arg0;
        int arg1 = 0;
        ax::PhysicsMaterial arg2;
        double arg3;
        do
        {
            ok = luaval_to_array_of_vec2(tolua_S, 2, &arg0, &arg1, "ax.PhysicsShapeEdgeChain:create");
            if (nullptr == arg0)
            {
                LUA_PRECONDITION(arg0, "Invalid Native Object");
            }
        } while (0);
        ok &= luaval_to_physics_material(tolua_S, 3, &arg2, "ax.PhysicsShapeEdgeChain:create");
        ok &= luaval_to_number(tolua_S, 4, &arg3, "ax.PhysicsShapeEdgeChain:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arg0);
            return 0;
        }
        ax::PhysicsShapeEdgeChain* ret = ax::PhysicsShapeEdgeChain::create(arg0, arg1, arg2, arg3);
        AX_SAFE_DELETE_ARRAY(arg0);
        object_to_luaval<ax::PhysicsShapeEdgeChain>(tolua_S, "ax.PhysicsShapeEdgeChain",
                                                         (ax::PhysicsShapeEdgeChain*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "create", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_physics_PhysicsShapeEdgeChain_create'.", &tolua_err);
#    endif
    return 0;
}

int register_all_ax_physics_manual(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.PhysicsBody");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getJoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsBody_getJoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "createPolygon");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsBody_createPolygon);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "createEdgeChain");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsBody_createEdgeChain);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "createEdgePolygon");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsBody_createEdgePolygon);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShape");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "recenterPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShape_recenterPoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "getPolygonCenter");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShape_getPolygonCenter);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "getPolyonCenter");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShape_getPolygonCenter);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShapeBox");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeBox_getPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShapeEdgeBox");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeEdgeBox_getPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShapePolygon");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapePolygon_getPoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapePolygon_create);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "calculateArea");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapePolygon_calculateArea);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "calculateMoment");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapePolygon_calculateMoment);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShapeEdgePolygon");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeEdgePolygon_getPoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeEdgePolygon_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsShapeEdgeChain");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getPoints");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeEdgeChain_getPoints);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsShapeEdgeChain_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.PhysicsWorld");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getScene");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsWorld_getScene);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "queryPoint");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsWorld_queryPoint);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "queryRect");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsWorld_queryRect);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "rayCast");
        lua_pushcfunction(tolua_S, axlua_physics_PhysicsWorld_rayCast);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_NONE");
        lua_pushnumber(tolua_S, PhysicsWorld::DEBUGDRAW_NONE);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_SHAPE");
        lua_pushnumber(tolua_S, PhysicsWorld::DEBUGDRAW_SHAPE);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_JOINT");
        lua_pushnumber(tolua_S, PhysicsWorld::DEBUGDRAW_JOINT);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_CONTACT");
        lua_pushnumber(tolua_S, PhysicsWorld::DEBUGDRAW_CONTACT);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "DEBUGDRAW_ALL");
        lua_pushnumber(tolua_S, PhysicsWorld::DEBUGDRAW_ALL);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);

    lua_pushstring(tolua_S, "ax.EventListenerPhysicsContact");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "registerScriptHandler",
                       toaxlua_EventListenerPhysicsContact_registerScriptHandler);
    }
    lua_pop(tolua_S, 1);

    tolua_constant(tolua_S, "PHYSICS_INFINITY", PHYSICS_INFINITY);

    return 0;
}

#endif
