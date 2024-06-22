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
#include "platform/PlatformConfig.h"
#include "base/Config.h"
#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
#    include "lua-bindings/manual/physics3d/axlua_physics3d_manual.h"
#    include "lua-bindings/auto/axlua_physics3d_auto.hpp"
#    include "lua-bindings/manual/tolua_fix.h"
#    include "lua-bindings/manual/LuaBasicConversions.h"
#    include "lua-bindings/manual/LuaEngine.h"
#    include "physics3d/Physics3D.h"

bool luaval_to_Physics3DRigidBodyDes(lua_State* L,
                                     int lo,
                                     ax::Physics3DRigidBodyDes* outValue,
                                     const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#    endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "mass");
        lua_gettable(L, lo);
        outValue->mass = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "localInertia");
        lua_gettable(L, lo);
        if (!lua_istable(L, -1))
        {
            outValue->localInertia = ax::Vec3(0.0, 0.0, 0.0);
        }
        else
        {
            luaval_to_vec3(L, lua_gettop(L), &outValue->localInertia);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "shape");
        lua_gettable(L, lo);
        if (!tolua_isusertype(L, -1, "ax.Physics3DShape", 0, &tolua_err))
        {
            outValue->shape = nullptr;
        }
        else
        {
            outValue->shape = static_cast<ax::Physics3DShape*>(tolua_tousertype(L, lua_gettop(L), nullptr));
        }
        lua_pop(L, 1);

        lua_pushstring(L, "originalTransform");
        lua_gettable(L, lo);
        if (!lua_istable(L, -1))
        {
            outValue->originalTransform = ax::Mat4();
        }
        else
        {
            luaval_to_mat4(L, lua_gettop(L), &outValue->originalTransform);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "disableSleep");
        lua_gettable(L, lo);
        outValue->disableSleep = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_Physics3DWorld_HitResult(lua_State* L,
                                        int lo,
                                        ax::Physics3DWorld::HitResult* outValue,
                                        const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
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

        lua_pushstring(L, "hitObj");
        lua_gettable(L, lo);
        if (!tolua_isusertype(L, -1, "ax.Physics3DObject", 0, &tolua_err))
        {
            outValue->hitObj = nullptr;
        }
        else
        {
            outValue->hitObj = static_cast<ax::Physics3DObject*>(tolua_tousertype(L, lua_gettop(L), nullptr));
        }
        lua_pop(L, 1);
    }
    return true;
}

void Physics3DWorld_HitResult_to_luaval(lua_State* L, const ax::Physics3DWorld::HitResult& hitResult)
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
    lua_pushstring(L, "hitObj");
    if (nullptr == hitResult.hitObj)
    {
        lua_pushnil(L);
    }
    else
    {
        object_to_luaval<ax::Physics3DObject>(L, "ax.Physics3DObject", hitResult.hitObj);
    }
    lua_rawset(L, -3);
}

int axlua_physics3d_PhysicsMeshRenderer_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.PhysicsMeshRenderer", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    if (argc == 2)
    {
        std::string arg0;
        ax::Physics3DRigidBodyDes arg1;
        ok &= luaval_to_std_string(L, 2, &arg0, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_Physics3DRigidBodyDes(L, 3, &arg1, "ax.PhysicsMeshRenderer:create");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_PhysicsMeshRenderer_create'", nullptr);
            return 0;
        }
        ax::PhysicsMeshRenderer* ret = ax::PhysicsMeshRenderer::create(arg0, &arg1);
        object_to_luaval<ax::PhysicsMeshRenderer>(L, "ax.PhysicsMeshRenderer", (ax::PhysicsMeshRenderer*)ret);
        return 1;
    }
    if (argc == 3)
    {
        std::string arg0;
        ax::Physics3DRigidBodyDes arg1;
        ax::Vec3 arg2;
        ok &= luaval_to_std_string(L, 2, &arg0, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_Physics3DRigidBodyDes(L, 3, &arg1, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_vec3(L, 4, &arg2, "ax.PhysicsMeshRenderer:create");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_PhysicsMeshRenderer_create'", nullptr);
            return 0;
        }
        ax::PhysicsMeshRenderer* ret = ax::PhysicsMeshRenderer::create(arg0, &arg1, arg2);
        object_to_luaval<ax::PhysicsMeshRenderer>(L, "ax.PhysicsMeshRenderer", (ax::PhysicsMeshRenderer*)ret);
        return 1;
    }
    if (argc == 4)
    {
        std::string arg0;
        ax::Physics3DRigidBodyDes arg1;
        ax::Vec3 arg2;
        ax::Quaternion arg3;
        ok &= luaval_to_std_string(L, 2, &arg0, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_Physics3DRigidBodyDes(L, 3, &arg1, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_vec3(L, 4, &arg2, "ax.PhysicsMeshRenderer:create");
        ok &= luaval_to_quaternion(L, 5, &arg3);
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_PhysicsMeshRenderer_create'", nullptr);
            return 0;
        }
        ax::PhysicsMeshRenderer* ret = ax::PhysicsMeshRenderer::create(arg0, &arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsMeshRenderer>(L, "ax.PhysicsMeshRenderer", (ax::PhysicsMeshRenderer*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsMeshRenderer:create", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_PhysicsMeshRenderer_create'.", &tolua_err);
#    endif
    return 0;
}

void extendPhysicsMeshRenderer(lua_State* L)
{
    lua_pushstring(L, "ax.PhysicsMeshRenderer");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_physics3d_PhysicsMeshRenderer_create);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_Physics3DRigidBody_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Physics3DRigidBody", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
        ax::Physics3DRigidBodyDes arg0;
        ok &= luaval_to_Physics3DRigidBodyDes(L, 2, &arg0, "ax.Physics3DRigidBody:create");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_Physics3DRigidBody_create'", nullptr);
            return 0;
        }
        ax::Physics3DRigidBody* ret = ax::Physics3DRigidBody::create(&arg0);
        object_to_luaval<ax::Physics3DRigidBody>(L, "ax.Physics3DRigidBody", (ax::Physics3DRigidBody*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DRigidBody:create", argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DRigidBody_create'.", &tolua_err);
#    endif
    return 0;
}

void extendPhysics3DRigidBody(lua_State* L)
{
    lua_pushstring(L, "ax.Physics3DRigidBody");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_physics3d_Physics3DRigidBody_create);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_Physics3DComponent_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Physics3DComponent", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    do
    {
        if (argc == 1)
        {
            ax::Physics3DObject* arg0;
            ok &= luaval_to_object<ax::Physics3DObject>(L, 2, "ax.Physics3DObject", &arg0);
            if (!ok)
            {
                break;
            }
            ax::Physics3DComponent* ret = ax::Physics3DComponent::create(arg0);
            object_to_luaval<ax::Physics3DComponent>(L, "ax.Physics3DComponent",
                                                          (ax::Physics3DComponent*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            ax::Physics3DObject* arg0;
            ok &= luaval_to_object<ax::Physics3DObject>(L, 2, "ax.Physics3DObject", &arg0);
            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(L, 3, &arg1, "ax.Physics3DComponent:create");
            if (!ok)
            {
                break;
            }
            ax::Physics3DComponent* ret = ax::Physics3DComponent::create(arg0, arg1);
            object_to_luaval<ax::Physics3DComponent>(L, "ax.Physics3DComponent",
                                                          (ax::Physics3DComponent*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 3)
        {
            ax::Physics3DObject* arg0;
            ok &= luaval_to_object<ax::Physics3DObject>(L, 2, "ax.Physics3DObject", &arg0);
            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(L, 3, &arg1, "ax.Physics3DComponent:create");
            if (!ok)
            {
                break;
            }
            ax::Quaternion arg2;
            ok &= luaval_to_quaternion(L, 4, &arg2);
            if (!ok)
            {
                break;
            }
            ax::Physics3DComponent* ret = ax::Physics3DComponent::create(arg0, arg1, arg2);
            object_to_luaval<ax::Physics3DComponent>(L, "ax.Physics3DComponent",
                                                          (ax::Physics3DComponent*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            ax::Physics3DComponent* ret = ax::Physics3DComponent::create();
            object_to_luaval<ax::Physics3DComponent>(L, "ax.Physics3DComponent",
                                                          (ax::Physics3DComponent*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d", "ax.Physics3DComponent:create", argc, 0);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DComponent_create'.", &tolua_err);
#    endif
    return 0;
}

void extendPhysics3DComponent(lua_State* L)
{
    lua_pushstring(L, "ax.Physics3DComponent");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_physics3d_Physics3DComponent_create);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_Physics3DWorld_rayCast(lua_State* L)
{
    int argc                      = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok                       = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.Physics3DWorld", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(L, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_physics3d_Physics3DWorld_rayCast'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(L) - 1;
    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Physics3DWorld::HitResult arg2;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.Physics3DWorld:rayCast");

        ok &= luaval_to_vec3(L, 3, &arg1, "ax.Physics3DWorld:rayCast");

        ok &= luaval_to_Physics3DWorld_HitResult(L, 4, &arg2, "ax.Physics3DWorld:rayCast");

        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_Physics3DWorld_rayCast'", nullptr);
            return 0;
        }
        bool ret = cobj->rayCast(arg0, arg1, &arg2);
        tolua_pushboolean(L, (bool)ret);
        Physics3DWorld_HitResult_to_luaval(L, arg2);
        return 2;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:rayCast", argc, 3);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DWorld_rayCast'.", &tolua_err);
#    endif

    return 0;
}

void extendPhysics3DWorld(lua_State* L)
{
    lua_pushstring(L, "ax.Physics3DWorld");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "rayCast", axlua_physics3d_Physics3DWorld_rayCast);
    }
    lua_pop(L, 1);
}

int axlua_physics3d_Physics3DShape_createMesh(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Physics3DShape", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    if (argc == 2)
    {
        std::vector<Vec3> arg0;
        int arg1;
        ok &= luaval_to_std_vector_vec3(L, 2, &arg0, "ax.Physics3DShape:createMesh");
        ok &= luaval_to_int32(L, 3, (int*)&arg1, "ax.Physics3DShape:createMesh");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_Physics3DShape_createMesh'", nullptr);
            return 0;
        }
        ax::Physics3DShape* ret = ax::Physics3DShape::createMesh(&arg0[0], arg1);
        object_to_luaval<ax::Physics3DShape>(L, "ax.Physics3DShape", (ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createMesh", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DShape_createMesh'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics3d_Physics3DShape_createHeightfield(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Physics3DShape", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    if (argc == 8)
    {
        int arg0;
        int arg1;
        std::vector<float> arg2;
        double arg3;
        double arg4;
        double arg5;
        bool arg6;
        bool arg7;
        ok &= luaval_to_int32(L, 2, (int*)&arg0, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_int32(L, 3, (int*)&arg1, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_std_vector_float(L, 4, &arg2, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 5, &arg3, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 6, &arg4, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 7, &arg5, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_boolean(L, 8, &arg6, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_boolean(L, 9, &arg7, "ax.Physics3DShape:createHeightfield");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_Physics3DShape_createHeightfield'",
                        nullptr);
            return 0;
        }
        ax::Physics3DShape* ret = ax::Physics3DShape::createHeightfield(arg0, arg1, &arg2[0], (float)arg3,
                                                                                  (float)arg4, (float)arg5, arg6, arg7);
        object_to_luaval<ax::Physics3DShape>(L, "ax.Physics3DShape", (ax::Physics3DShape*)ret);
        return 1;
    }
    if (argc == 9)
    {
        int arg0;
        int arg1;
        std::vector<float> arg2;
        double arg3;
        double arg4;
        double arg5;
        bool arg6;
        bool arg7;
        bool arg8;
        ok &= luaval_to_int32(L, 2, (int*)&arg0, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_int32(L, 3, (int*)&arg1, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_std_vector_float(L, 4, &arg2, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 5, &arg3, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 6, &arg4, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_number(L, 7, &arg5, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_boolean(L, 8, &arg6, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_boolean(L, 9, &arg7, "ax.Physics3DShape:createHeightfield");
        ok &= luaval_to_boolean(L, 10, &arg8, "ax.Physics3DShape:createHeightfield");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_physics3d_Physics3DShape_createHeightfield'",
                        nullptr);
            return 0;
        }
        ax::Physics3DShape* ret = ax::Physics3DShape::createHeightfield(
            arg0, arg1, &arg2[0], (float)arg3, (float)arg4, (float)arg5, arg6, arg7, arg8);
        object_to_luaval<ax::Physics3DShape>(L, "ax.Physics3DShape", (ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createHeightfield",
               argc, 8);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DShape_createHeightfield'.", &tolua_err);
#    endif
    return 0;
}

int axlua_physics3d_Physics3DShape_createCompoundShape(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

    tolua_Error tolua_err;

#    if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Physics3DShape", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {

        std::vector<std::pair<ax::Physics3DShape*, ax::Mat4>> shapes;
        if (!tolua_istable(L, 2, 0, &tolua_err))
        {
#    if _AX_DEBUG >= 1
            luaval_to_native_err(L, "#ferror:", &tolua_err, "ax.Physics3DShape:createCompoundShape");
#    endif
            ok = false;
        }

        if (ok)
        {
            size_t len                     = lua_objlen(L, 2);
            ax::Physics3DShape* shape = nullptr;
            ax::Mat4 mat;
            for (size_t i = 0; i < len; i++)
            {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, 2);
                if (lua_istable(L, -1))
                {
                    lua_pushnumber(L, 1);
                    lua_gettable(L, -2);
                    luaval_to_object(L, lua_gettop(L), "ax.Physics3DShape", &shape);
                    lua_pop(L, 1);

                    lua_pushnumber(L, 2);
                    lua_gettable(L, -2);
                    luaval_to_mat4(L, lua_gettop(L), &mat);
                    lua_pop(L, 1);

                    shapes.emplace_back(std::make_pair(shape, mat));
                }
                lua_pop(L, 1);
            }
        }

        ax::Physics3DShape* ret = ax::Physics3DShape::createCompoundShape(shapes);
        object_to_luaval<ax::Physics3DShape>(L, "ax.Physics3DShape", (ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createCompoundShape",
               argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DShape_createCompoundShape'.", &tolua_err);
#    endif
    return 0;
}

void extendPhysics3DShape(lua_State* L)
{
    lua_pushstring(L, "ax.Physics3DShape");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "createMesh", axlua_physics3d_Physics3DShape_createMesh);
        tolua_function(L, "createHeightfield", axlua_physics3d_Physics3DShape_createHeightfield);
        tolua_function(L, "createCompoundShape", axlua_physics3d_Physics3DShape_createCompoundShape);
    }
    lua_pop(L, 1);
}

void CollisionPoint_to_luaval(lua_State* L, const ax::Physics3DCollisionInfo::CollisionPoint& collisionPoint)
{
    if (nullptr == L)
        return;

    lua_newtable(L);
    lua_pushstring(L, "localPositionOnA");
    vec3_to_luaval(L, collisionPoint.localPositionOnA);
    lua_rawset(L, -3);
    lua_pushstring(L, "worldPositionOnA");
    vec3_to_luaval(L, collisionPoint.worldPositionOnA);
    lua_rawset(L, -3);
    lua_pushstring(L, "localPositionOnB");
    vec3_to_luaval(L, collisionPoint.localPositionOnB);
    lua_rawset(L, -3);
    lua_pushstring(L, "worldPositionOnB");
    vec3_to_luaval(L, collisionPoint.worldPositionOnB);
    lua_rawset(L, -3);
    lua_pushstring(L, "worldNormalOnB");
    vec3_to_luaval(L, collisionPoint.worldNormalOnB);
    lua_rawset(L, -3);
}

int axlua_physics3d_Physics3DObject_setCollisionCallback(lua_State* L)
{
    int argc                       = 0;
    ax::Physics3DObject* cobj = nullptr;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.Physics3DObject", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(L, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_physics3d_Physics3DObject_setCollisionCallback'",
                    nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#    endif
        LUA_FUNCTION handler = toluafix_ref_function(L, 2, 0);
        cobj->setCollisionCallback([=](const ax::Physics3DCollisionInfo& ci) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            auto Ls    = stack->getLuaState();
            lua_newtable(Ls);
            lua_pushstring(Ls, "objA");
            if (nullptr == ci.objA)
            {
                lua_pushnil(Ls);
            }
            else
            {
                object_to_luaval(Ls, "ax.Physics3DObject", ci.objA);
            }
            lua_rawset(Ls, -3);
            lua_pushstring(Ls, "objB");
            if (nullptr == ci.objB)
            {
                lua_pushnil(Ls);
            }
            else
            {
                object_to_luaval(Ls, "ax.Physics3DObject", ci.objB);
            }
            lua_rawset(Ls, -3);
            lua_pushstring(Ls, "collisionPointList");
            if (ci.collisionPointList.empty())
            {
                lua_pushnil(Ls);
            }
            else
            {
                int vecIndex = 1;
                lua_newtable(Ls);
                for (const auto& value : ci.collisionPointList)
                {
                    lua_pushnumber(Ls, vecIndex);
                    CollisionPoint_to_luaval(Ls, value);
                    lua_rawset(Ls, -3);
                    ++vecIndex;
                }
            }
            lua_rawset(Ls, -3);
            stack->executeFunctionByHandler(handler, 1);
        });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.Physics3DObject:setCollisionCallback", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_physics3d_Physics3DObject_setCollisionCallback'.", &tolua_err);
#    endif

    return 0;
}

void extendPhysics3DObject(lua_State* L)
{
    lua_pushstring(L, "ax.Physics3DObject");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setCollisionCallback", axlua_physics3d_Physics3DObject_setCollisionCallback);
    }
    lua_pop(L, 1);
}

int register_all_physics3d_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendPhysicsMeshRenderer(L);
    extendPhysics3DRigidBody(L);
    extendPhysics3DComponent(L);
    extendPhysics3DWorld(L);
    extendPhysics3DShape(L);
    extendPhysics3DObject(L);

    return 1;
}

int register_physics3d_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_physics3d(L);
        register_all_physics3d_manual(L);
    }
    lua_pop(L, 1);
    return 1;
}

#endif
