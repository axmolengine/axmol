/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
#include "axmol/platform/PlatformConfig.h"
#include "axmol/base/Config.h"
#if defined(AX_ENABLE_PHYSICS_3D)
#    include "lua-bindings/adapters/physics3d/axlua_physics3d_adapter.h"
#    include "lua-bindings/generated/axlua_physics3d_gen.h"
#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "lua-bindings/runtime/axlua_conversions.h"
#    include "axmol/physics/physics-3d.h"

bool luaval_to_PhysicsWorld3D_HitResult(lua_State* L,
                                        int lo,
                                        ax::PhysicsWorld3D::HitResult* outValue,
                                        const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#    endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "hitPosition");
        lua_gettable(L, lo);
        if (!lua_istable(L, -1))
        {
            outValue->hitPosition = ax::Vec3();
        }
        else
        {
            luaval_to_vec3(L, lua_gettop(L), &(outValue->hitPosition));
        }
        lua_pop(L, 1);

        lua_pushstring(L, "hitNormal");
        lua_gettable(L, lo);
        if (!lua_istable(L, -1))
        {
            outValue->hitNormal = ax::Vec3();
        }
        else
        {
            luaval_to_vec3(L, lua_gettop(L), &(outValue->hitNormal));
        }
        lua_pop(L, 1);

        outValue->hitActor  = nullptr;
        outValue->rigidbody = nullptr;
        outValue->collider  = nullptr;
    }

    return true;
}

void PhysicsWorld3D_HitResult_to_luaval(lua_State* L, const ax::PhysicsWorld3D::HitResult& hitResult)
{
    if (nullptr == L)
        return;

    lua_newtable(L);
    lua_pushstring(L, "hitPosition");
    vec3_to_luaval(L, hitResult.hitPosition);
    lua_rawset(L, -3);
    lua_pushstring(L, "hitNormal");
    vec3_to_luaval(L, hitResult.hitNormal);
    lua_rawset(L, -3);
    lua_pushstring(L, "hitActor");
    if (nullptr == hitResult.hitActor)
    {
        lua_pushnil(L);
    }
    else
    {
        object_to_luaval<ax::Component>(L, "ax.Component", hitResult.hitActor);
    }
    lua_rawset(L, -3);
    lua_pushstring(L, "rigidBody");
    if (nullptr == hitResult.rigidbody)
    {
        lua_pushnil(L);
    }
    else
    {
        object_to_luaval<ax::Rigidbody3D>(L, "ax.Rigidbody3D", hitResult.rigidbody);
    }
    lua_rawset(L, -3);
    lua_pushstring(L, "collider");
    if (nullptr == hitResult.collider)
    {
        lua_pushnil(L);
    }
    else
    {
        object_to_luaval<ax::Collider3D>(L, "ax.Collider3D", hitResult.collider);
    }
    lua_rawset(L, -3);
}

int axlua_physics3d_Rigidbody3D_setTransformInPhysics(lua_State* L)
{
    int argc             = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok              = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.Rigidbody3D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::Rigidbody3D*)axlua::adapter::to_usertype(L, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_physics3d_Rigidbody3D_setTransformInPhysics'",
                                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(L) - 1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Quat arg1;
        ok &= luaval_to_vec3(L, 2, &arg0, "ax.Rigidbody3D:setTransformInPhysics");
        ok &= luaval_to_quaternion(L, 3, &arg1);
        if (!ok)
        {
            axlua::adapter::raise_error(
                L, "invalid arguments in function 'axlua_physics3d_Rigidbody3D_setTransformInPhysics'", nullptr);
            return 0;
        }
        obj->setTransformInPhysics(arg0, arg1);
        lua_settop(L, 1);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody3D:setTransformInPhysics",
               argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_physics3d_Rigidbody3D_setTransformInPhysics'.",
                                &conversionError);
#    endif
    return 0;
}

void extendRigidbody3D(lua_State* L)
{
    lua_pushstring(L, "ax.Rigidbody3D");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setTransformInPhysics", axlua_physics3d_Rigidbody3D_setTransformInPhysics);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_Collider3D_setTransformInPhysics(lua_State* L)
{
    int argc            = 0;
    ax::Collider3D* obj = nullptr;
    bool ok             = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.Collider3D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::Collider3D*)axlua::adapter::to_usertype(L, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_physics3d_Collider3D_setTransformInPhysics'",
                                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(L) - 1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Quat arg1;
        ok &= luaval_to_vec3(L, 2, &arg0, "ax.Collider3D:setTransformInPhysics");
        ok &= luaval_to_quaternion(L, 3, &arg1);
        if (!ok)
        {
            axlua::adapter::raise_error(
                L, "invalid arguments in function 'axlua_physics3d_Collider3D_setTransformInPhysics'", nullptr);
            return 0;
        }
        obj->setTransformInPhysics(arg0, arg1);
        lua_settop(L, 1);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Collider3D:setTransformInPhysics",
               argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_physics3d_Collider3D_setTransformInPhysics'.",
                                &conversionError);
#    endif
    return 0;
}

void extendCollider3D(lua_State* L)
{
    lua_pushstring(L, "ax.Collider3D");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setTransformInPhysics", axlua_physics3d_Collider3D_setTransformInPhysics);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_PhysicsWorld3D_rayCast(lua_State* L)
{
    int argc                = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok                 = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.PhysicsWorld3D", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::PhysicsWorld3D*)axlua::adapter::to_usertype(L, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_physics3d_PhysicsWorld3D_rayCast'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(L) - 1;
    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::PhysicsWorld3D::HitResult arg2;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.PhysicsWorld3D:rayCast");

        ok &= luaval_to_vec3(L, 3, &arg1, "ax.PhysicsWorld3D:rayCast");

        ok &= luaval_to_PhysicsWorld3D_HitResult(L, 4, &arg2, "ax.PhysicsWorld3D:rayCast");

        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'axlua_physics3d_PhysicsWorld3D_rayCast'",
                                        nullptr);
            return 0;
        }
        bool ret = obj->rayCast(arg0, arg1, &arg2);
        axlua::adapter::push_boolean(L, (bool)ret);
        PhysicsWorld3D_HitResult_to_luaval(L, arg2);
        return 2;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:rayCast", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_physics3d_PhysicsWorld3D_rayCast'.", &conversionError);
#    endif

    return 0;
}

void extendPhysicsWorld3D(lua_State* L)
{
    lua_pushstring(L, "ax.PhysicsWorld3D");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "rayCast", axlua_physics3d_PhysicsWorld3D_rayCast);
    }
    lua_pop(L, 1);
}

int register_all_physics3d_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendRigidbody3D(L);
    extendCollider3D(L);
    extendPhysicsWorld3D(L);

    return 1;
}

int register_physics3d_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_physics3d(L);
        register_all_physics3d_adapter(L);
    }
    lua_pop(L, 1);
    return 1;
}

#endif
