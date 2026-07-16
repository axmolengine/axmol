#include "lua-bindings/auto/axlua_physics3d_auto.hpp"
#if defined(AX_ENABLE_PHYSICS_3D)
#include "axmol/physics/physics-3d.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_physics3d_PhysicsActor_syncNodeToPhysics(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_syncNodeToPhysics'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_syncNodeToPhysics'", nullptr);
            return 0;
        }
        obj->syncNodeToPhysics();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:syncNodeToPhysics",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_syncNodeToPhysics'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_syncPhysicsToNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_syncPhysicsToNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_syncPhysicsToNode'", nullptr);
            return 0;
        }
        obj->syncPhysicsToNode();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:syncPhysicsToNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_syncPhysicsToNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_getWorldTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_getWorldTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_getWorldTransform'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorldTransform();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:getWorldTransform",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_getWorldTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_preSimulate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_preSimulate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_preSimulate'", nullptr);
            return 0;
        }
        obj->preSimulate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:preSimulate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_preSimulate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_postSimulate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_postSimulate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_postSimulate'", nullptr);
            return 0;
        }
        obj->postSimulate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:postSimulate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_postSimulate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_getActorType(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_getActorType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_getActorType'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActorType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:getActorType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_getActorType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_setSyncFlag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_setSyncFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsActor::SyncFlag arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsActor:setSyncFlag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_setSyncFlag'", nullptr);
            return 0;
        }
        obj->setSyncFlag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:setSyncFlag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_setSyncFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_getSyncFlag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_getSyncFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_getSyncFlag'", nullptr);
            return 0;
        }
        int ret = (int)obj->getSyncFlag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:getSyncFlag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_getSyncFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsActor:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_setTag'", nullptr);
            return 0;
        }
        obj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_getTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_isEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_isEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ContactEventBits arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsActor:isEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_isEventEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isEventEnabled(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:isEventEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_isEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsActor_setEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsActor* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsActor",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsActor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsActor_setEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ContactEventBits arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsActor:setEventEnabled");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.PhysicsActor:setEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsActor_setEventEnabled'", nullptr);
            return 0;
        }
        obj->setEventEnabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsActor:setEventEnabled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsActor_setEventEnabled'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_PhysicsActor_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PhysicsActor)");
    return 0;
}

int lua_register_ax_physics3d_PhysicsActor(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsActor");
    tolua_cclass(tolua_S,"PhysicsActor","ax.PhysicsActor","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsActor");
        tolua_function(tolua_S,"syncNodeToPhysics",lua_ax_physics3d_PhysicsActor_syncNodeToPhysics);
        tolua_function(tolua_S,"syncPhysicsToNode",lua_ax_physics3d_PhysicsActor_syncPhysicsToNode);
        tolua_function(tolua_S,"getWorldTransform",lua_ax_physics3d_PhysicsActor_getWorldTransform);
        tolua_function(tolua_S,"preSimulate",lua_ax_physics3d_PhysicsActor_preSimulate);
        tolua_function(tolua_S,"postSimulate",lua_ax_physics3d_PhysicsActor_postSimulate);
        tolua_function(tolua_S,"getActorType",lua_ax_physics3d_PhysicsActor_getActorType);
        tolua_function(tolua_S,"setSyncFlag",lua_ax_physics3d_PhysicsActor_setSyncFlag);
        tolua_function(tolua_S,"getSyncFlag",lua_ax_physics3d_PhysicsActor_getSyncFlag);
        tolua_function(tolua_S,"setTag",lua_ax_physics3d_PhysicsActor_setTag);
        tolua_function(tolua_S,"getTag",lua_ax_physics3d_PhysicsActor_getTag);
        tolua_function(tolua_S,"isEventEnabled",lua_ax_physics3d_PhysicsActor_isEventEnabled);
        tolua_function(tolua_S,"setEventEnabled",lua_ax_physics3d_PhysicsActor_setEventEnabled);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsActor).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsActor";
    g_typeCast[typeName] = "ax.PhysicsActor";
    return 1;
}

int lua_ax_physics3d_Collider3D_isValid(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_isValid'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_isValid'", nullptr);
            return 0;
        }
        auto&& ret = obj->isValid();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:isValid",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_isValid'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_hasOwnBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_hasOwnBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_hasOwnBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasOwnBody();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:hasOwnBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_hasOwnBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_isAttached'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_isAttachedToWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_isAttachedToWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_isAttachedToWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttachedToWorld();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:isAttachedToWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_isAttachedToWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorld();
        object_to_luaval<ax::PhysicsWorld3D>(tolua_S, "ax.PhysicsWorld3D",(ax::PhysicsWorld3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_getType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_getType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:getType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_getType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_setSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_setSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Collider3D:setSensor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_setSensor'", nullptr);
            return 0;
        }
        obj->setSensor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:setSensor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_setSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_isSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_isSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_isSensor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isSensor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:isSensor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_isSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_getMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_getMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_getMaterial'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaterial();
        physics_material_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:getMaterial",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_getMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_setMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_setMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsMaterial arg0;

        ok &= luaval_to_physics_material(tolua_S, 2, &arg0, "ax.Collider3D:setMaterial");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_setMaterial'", nullptr);
            return 0;
        }
        obj->setMaterial(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:setMaterial",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_setMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Collider3D_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Collider3D_setUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        void* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Collider3D_setUserData'", nullptr);
            return 0;
        }
        obj->setUserData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider3D:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Collider3D_setUserData'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_Collider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Collider3D)");
    return 0;
}

int lua_register_ax_physics3d_Collider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Collider3D");
    tolua_cclass(tolua_S,"Collider3D","ax.Collider3D","ax.PhysicsActor",nullptr);

    tolua_beginmodule(tolua_S,"Collider3D");
        tolua_function(tolua_S,"isValid",lua_ax_physics3d_Collider3D_isValid);
        tolua_function(tolua_S,"hasOwnBody",lua_ax_physics3d_Collider3D_hasOwnBody);
        tolua_function(tolua_S,"isAttached",lua_ax_physics3d_Collider3D_isAttached);
        tolua_function(tolua_S,"isAttachedToWorld",lua_ax_physics3d_Collider3D_isAttachedToWorld);
        tolua_function(tolua_S,"getWorld",lua_ax_physics3d_Collider3D_getWorld);
        tolua_function(tolua_S,"getType",lua_ax_physics3d_Collider3D_getType);
        tolua_function(tolua_S,"setSensor",lua_ax_physics3d_Collider3D_setSensor);
        tolua_function(tolua_S,"isSensor",lua_ax_physics3d_Collider3D_isSensor);
        tolua_function(tolua_S,"getMaterial",lua_ax_physics3d_Collider3D_getMaterial);
        tolua_function(tolua_S,"setMaterial",lua_ax_physics3d_Collider3D_setMaterial);
        tolua_function(tolua_S,"setUserData",lua_ax_physics3d_Collider3D_setUserData);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Collider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Collider3D";
    g_typeCast[typeName] = "ax.Collider3D";
    return 1;
}

int lua_ax_physics3d_BoxCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.BoxCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec3 arg0;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.BoxCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_BoxCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider3D::create(arg0);
        object_to_luaval<ax::BoxCollider3D>(tolua_S, "ax.BoxCollider3D",(ax::BoxCollider3D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.BoxCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.BoxCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_BoxCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider3D::create(arg0, arg1);
        object_to_luaval<ax::BoxCollider3D>(tolua_S, "ax.BoxCollider3D",(ax::BoxCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.BoxCollider3D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_BoxCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_BoxCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BoxCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_BoxCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.BoxCollider3D");
    tolua_cclass(tolua_S,"BoxCollider3D","ax.BoxCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"BoxCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_BoxCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::BoxCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.BoxCollider3D";
    g_typeCast[typeName] = "ax.BoxCollider3D";
    return 1;
}

int lua_ax_physics3d_SphereCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SphereCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SphereCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SphereCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SphereCollider3D::create(arg0);
        object_to_luaval<ax::SphereCollider3D>(tolua_S, "ax.SphereCollider3D",(ax::SphereCollider3D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SphereCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.SphereCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SphereCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SphereCollider3D::create(arg0, arg1);
        object_to_luaval<ax::SphereCollider3D>(tolua_S, "ax.SphereCollider3D",(ax::SphereCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SphereCollider3D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SphereCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_SphereCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SphereCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_SphereCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.SphereCollider3D");
    tolua_cclass(tolua_S,"SphereCollider3D","ax.SphereCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"SphereCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_SphereCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::SphereCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.SphereCollider3D";
    g_typeCast[typeName] = "ax.SphereCollider3D";
    return 1;
}

int lua_ax_physics3d_CylinderCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CylinderCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        double arg1;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CylinderCollider3D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.CylinderCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_CylinderCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CylinderCollider3D::create(arg0, arg1);
        object_to_luaval<ax::CylinderCollider3D>(tolua_S, "ax.CylinderCollider3D",(ax::CylinderCollider3D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        double arg1;
        ax::PhysicsMaterial arg2;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CylinderCollider3D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.CylinderCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.CylinderCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_CylinderCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CylinderCollider3D::create(arg0, arg1, arg2);
        object_to_luaval<ax::CylinderCollider3D>(tolua_S, "ax.CylinderCollider3D",(ax::CylinderCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.CylinderCollider3D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_CylinderCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_CylinderCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (CylinderCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_CylinderCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.CylinderCollider3D");
    tolua_cclass(tolua_S,"CylinderCollider3D","ax.CylinderCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"CylinderCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_CylinderCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::CylinderCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.CylinderCollider3D";
    g_typeCast[typeName] = "ax.CylinderCollider3D";
    return 1;
}

int lua_ax_physics3d_CapsuleCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CapsuleCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        double arg1;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CapsuleCollider3D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.CapsuleCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_CapsuleCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CapsuleCollider3D::create(arg0, arg1);
        object_to_luaval<ax::CapsuleCollider3D>(tolua_S, "ax.CapsuleCollider3D",(ax::CapsuleCollider3D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        double arg1;
        ax::PhysicsMaterial arg2;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CapsuleCollider3D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.CapsuleCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.CapsuleCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_CapsuleCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CapsuleCollider3D::create(arg0, arg1, arg2);
        object_to_luaval<ax::CapsuleCollider3D>(tolua_S, "ax.CapsuleCollider3D",(ax::CapsuleCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.CapsuleCollider3D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_CapsuleCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_CapsuleCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (CapsuleCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_CapsuleCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.CapsuleCollider3D");
    tolua_cclass(tolua_S,"CapsuleCollider3D","ax.CapsuleCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"CapsuleCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_CapsuleCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::CapsuleCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.CapsuleCollider3D";
    g_typeCast[typeName] = "ax.CapsuleCollider3D";
    return 1;
}

int lua_ax_physics3d_ConvexCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ConvexCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec3> arg0;
        ok &= luaval_to_std_vector_float3(tolua_S, 2, &arg0, "ax.ConvexCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConvexCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConvexCollider3D::create(arg0);
        object_to_luaval<ax::ConvexCollider3D>(tolua_S, "ax.ConvexCollider3D",(ax::ConvexCollider3D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec3> arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_std_vector_float3(tolua_S, 2, &arg0, "ax.ConvexCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.ConvexCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConvexCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConvexCollider3D::create(arg0, arg1);
        object_to_luaval<ax::ConvexCollider3D>(tolua_S, "ax.ConvexCollider3D",(ax::ConvexCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.ConvexCollider3D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConvexCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_ConvexCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ConvexCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_ConvexCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ConvexCollider3D");
    tolua_cclass(tolua_S,"ConvexCollider3D","ax.ConvexCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"ConvexCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_ConvexCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ConvexCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ConvexCollider3D";
    g_typeCast[typeName] = "ax.ConvexCollider3D";
    return 1;
}

int lua_ax_physics3d_MeshCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.MeshCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec3> arg0;
        ok &= luaval_to_std_vector_float3(tolua_S, 2, &arg0, "ax.MeshCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_MeshCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::MeshCollider3D::create(arg0);
        object_to_luaval<ax::MeshCollider3D>(tolua_S, "ax.MeshCollider3D",(ax::MeshCollider3D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec3> arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_std_vector_float3(tolua_S, 2, &arg0, "ax.MeshCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.MeshCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_MeshCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::MeshCollider3D::create(arg0, arg1);
        object_to_luaval<ax::MeshCollider3D>(tolua_S, "ax.MeshCollider3D",(ax::MeshCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.MeshCollider3D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_MeshCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_MeshCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (MeshCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_MeshCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.MeshCollider3D");
    tolua_cclass(tolua_S,"MeshCollider3D","ax.MeshCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"MeshCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_MeshCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::MeshCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.MeshCollider3D";
    g_typeCast[typeName] = "ax.MeshCollider3D";
    return 1;
}

int lua_ax_physics3d_HeightFieldCollider3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.HeightFieldCollider3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

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
        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_int(tolua_S, 3, &arg1, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_std_vector_float(tolua_S, 4, &arg2, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 5, &arg3, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 6, &arg4, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 7, &arg5, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_boolean(tolua_S, 8, &arg6, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_boolean(tolua_S, 9, &arg7, "ax.HeightFieldCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HeightFieldCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::HeightFieldCollider3D::create(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        object_to_luaval<ax::HeightFieldCollider3D>(tolua_S, "ax.HeightFieldCollider3D",(ax::HeightFieldCollider3D*)ret);
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
        ax::PhysicsMaterial arg8;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_int(tolua_S, 3, &arg1, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_std_vector_float(tolua_S, 4, &arg2, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 5, &arg3, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 6, &arg4, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_number(tolua_S, 7, &arg5, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_boolean(tolua_S, 8, &arg6, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_boolean(tolua_S, 9, &arg7, "ax.HeightFieldCollider3D:create");
        ok &= luaval_to_physics_material(tolua_S, 10, &arg8, "ax.HeightFieldCollider3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HeightFieldCollider3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::HeightFieldCollider3D::create(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        object_to_luaval<ax::HeightFieldCollider3D>(tolua_S, "ax.HeightFieldCollider3D",(ax::HeightFieldCollider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.HeightFieldCollider3D:create",argc, 8);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HeightFieldCollider3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_HeightFieldCollider3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (HeightFieldCollider3D)");
    return 0;
}

int lua_register_ax_physics3d_HeightFieldCollider3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.HeightFieldCollider3D");
    tolua_cclass(tolua_S,"HeightFieldCollider3D","ax.HeightFieldCollider3D","ax.Collider3D",nullptr);

    tolua_beginmodule(tolua_S,"HeightFieldCollider3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_HeightFieldCollider3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::HeightFieldCollider3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.HeightFieldCollider3D";
    g_typeCast[typeName] = "ax.HeightFieldCollider3D";
    return 1;
}

int lua_ax_physics3d_Rigidbody3D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_isAttached'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorld();
        object_to_luaval<ax::PhysicsWorld3D>(tolua_S, "ax.PhysicsWorld3D",(ax::PhysicsWorld3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody3D:setMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setMass'", nullptr);
            return 0;
        }
        obj->setMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getMass'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMass();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setInertia(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setInertia'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:setInertia");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setInertia'", nullptr);
            return 0;
        }
        obj->setInertia(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setInertia",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setInertia'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getInertia(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getInertia'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getInertia'", nullptr);
            return 0;
        }
        auto&& ret = obj->getInertia();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getInertia",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getInertia'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setMassData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setMassData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        double arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody3D:setMassData");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Rigidbody3D:setMassData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setMassData'", nullptr);
            return 0;
        }
        obj->setMassData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setMassData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setMassData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:setLinearVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setLinearVelocity'", nullptr);
            return 0;
        }
        obj->setLinearVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setLinearVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getLinearVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearVelocity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getLinearVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:setAngularVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setAngularVelocity'", nullptr);
            return 0;
        }
        obj->setAngularVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setAngularVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getAngularVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularVelocity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getAngularVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody3D:setLinearDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setLinearDamping'", nullptr);
            return 0;
        }
        obj->setLinearDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setLinearDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getLinearDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getLinearDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody3D:setAngularDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setAngularDamping'", nullptr);
            return 0;
        }
        obj->setAngularDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setAngularDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getAngularDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getAngularDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getTotalForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getTotalForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getTotalForce'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTotalForce();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getTotalForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getTotalForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getTotalTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getTotalTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getTotalTorque'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTotalTorque();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getTotalTorque",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getTotalTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getMaterial'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaterial();
        physics_material_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getMaterial",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsMaterial arg0;

        ok &= luaval_to_physics_material(tolua_S, 2, &arg0, "ax.Rigidbody3D:setMaterial");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setMaterial'", nullptr);
            return 0;
        }
        obj->setMaterial(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setMaterial",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getCollisionDetectionMode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getCollisionDetectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getCollisionDetectionMode'", nullptr);
            return 0;
        }
        int ret = (int)obj->getCollisionDetectionMode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getCollisionDetectionMode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getCollisionDetectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setCollisionDetectionMode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setCollisionDetectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody3D::CollisionDetectionMode arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody3D:setCollisionDetectionMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setCollisionDetectionMode'", nullptr);
            return 0;
        }
        obj->setCollisionDetectionMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setCollisionDetectionMode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setCollisionDetectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setDynamic'", nullptr);
            return 0;
        }
        obj->setDynamic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setDynamic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_isDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_isDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_isDynamic'", nullptr);
            return 0;
        }
        auto&& ret = obj->isDynamic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:isDynamic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_isDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setKinematic'", nullptr);
            return 0;
        }
        obj->setKinematic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setKinematic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_isKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_isKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_isKinematic'", nullptr);
            return 0;
        }
        auto&& ret = obj->isKinematic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:isKinematic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_isKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setMotionType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setMotionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody3D::MotionType arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody3D:setMotionType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setMotionType'", nullptr);
            return 0;
        }
        obj->setMotionType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setMotionType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setMotionType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getMotionType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getMotionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getMotionType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getMotionType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getMotionType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getMotionType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setAllowSleeping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setAllowSleeping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody3D:setAllowSleeping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setAllowSleeping'", nullptr);
            return 0;
        }
        obj->setAllowSleeping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setAllowSleeping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setAllowSleeping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_isAllowSleeping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_isAllowSleeping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_isAllowSleeping'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAllowSleeping();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:isAllowSleeping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_isAllowSleeping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setGravityScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setGravityScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody3D:setGravityScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setGravityScale'", nullptr);
            return 0;
        }
        obj->setGravityScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setGravityScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setGravityScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getGravityScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getGravityScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getGravityScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGravityScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getGravityScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getGravityScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setRotationEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setRotationEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::bvec3 arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR bvec3
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setRotationEnabled'", nullptr);
            return 0;
        }
        obj->setRotationEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setRotationEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setRotationEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_isRotationEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_isRotationEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody3D:isRotationEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_isRotationEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isRotationEnabled(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:isRotationEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_isRotationEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_applyForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_applyForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:applyForce");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Rigidbody3D:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_applyForce'", nullptr);
            return 0;
        }
        obj->applyForce(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:applyForce",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_applyForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_applyTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_applyTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:applyTorque");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_applyTorque'", nullptr);
            return 0;
        }
        obj->applyTorque(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:applyTorque",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_applyTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_applyImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_applyImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:applyImpulse");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Rigidbody3D:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_applyImpulse'", nullptr);
            return 0;
        }
        obj->applyImpulse(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:applyImpulse",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_applyImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_applyAngularImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_applyAngularImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:applyAngularImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_applyAngularImpulse'", nullptr);
            return 0;
        }
        obj->applyAngularImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:applyAngularImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_applyAngularImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setActive(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setActive'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody3D:setActive");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setActive'", nullptr);
            return 0;
        }
        obj->setActive(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setActive",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setActive'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getCollider(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getCollider'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getCollider'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCollider();
        object_to_luaval<ax::Collider3D>(tolua_S, "ax.Collider3D",(ax::Collider3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getCollider",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getCollider'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:setPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setPosition'", nullptr);
            return 0;
        }
        obj->setPosition(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec3 arg0;
        bool arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:setPosition");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody3D:setPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setPosition'", nullptr);
            return 0;
        }
        obj->setPosition(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setPosition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPosition();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Quat arg0;

        ok &= luaval_to_quat(tolua_S, 2, &arg0, "ax.Rigidbody3D:setRotation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setRotation'", nullptr);
            return 0;
        }
        obj->setRotation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Quat arg0;
        bool arg1;

        ok &= luaval_to_quat(tolua_S, 2, &arg0, "ax.Rigidbody3D:setRotation");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody3D:setRotation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setRotation'", nullptr);
            return 0;
        }
        obj->setRotation(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setRotation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRotation();
        quat_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_world2Local(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_world2Local'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:world2Local");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_world2Local'", nullptr);
            return 0;
        }
        auto&& ret = obj->world2Local(arg0);
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:world2Local",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_world2Local'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_local2World(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_local2World'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Rigidbody3D:local2World");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_local2World'", nullptr);
            return 0;
        }
        auto&& ret = obj->local2World(arg0);
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:local2World",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_local2World'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Rigidbody3D_setUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        void* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_setUserData'", nullptr);
            return 0;
        }
        obj->setUserData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody3D:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_setUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Rigidbody3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody3D::create();
        object_to_luaval<ax::Rigidbody3D>(tolua_S, "ax.Rigidbody3D",(ax::Rigidbody3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Collider3D* arg0;
        ok &= luaval_to_object<ax::Collider3D>(tolua_S, 2, "ax.Collider3D",&arg0, "ax.Rigidbody3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody3D::create(arg0);
        object_to_luaval<ax::Rigidbody3D>(tolua_S, "ax.Rigidbody3D",(ax::Rigidbody3D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Collider3D* arg0;
        double arg1;
        ok &= luaval_to_object<ax::Collider3D>(tolua_S, 2, "ax.Collider3D",&arg0, "ax.Rigidbody3D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.Rigidbody3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Rigidbody3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody3D::create(arg0, arg1);
        object_to_luaval<ax::Rigidbody3D>(tolua_S, "ax.Rigidbody3D",(ax::Rigidbody3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Rigidbody3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_Rigidbody3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Rigidbody3D)");
    return 0;
}

int lua_register_ax_physics3d_Rigidbody3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Rigidbody3D");
    tolua_cclass(tolua_S,"Rigidbody3D","ax.Rigidbody3D","ax.PhysicsActor",nullptr);

    tolua_beginmodule(tolua_S,"Rigidbody3D");
        tolua_function(tolua_S,"isAttached",lua_ax_physics3d_Rigidbody3D_isAttached);
        tolua_function(tolua_S,"getWorld",lua_ax_physics3d_Rigidbody3D_getWorld);
        tolua_function(tolua_S,"setMass",lua_ax_physics3d_Rigidbody3D_setMass);
        tolua_function(tolua_S,"getMass",lua_ax_physics3d_Rigidbody3D_getMass);
        tolua_function(tolua_S,"setInertia",lua_ax_physics3d_Rigidbody3D_setInertia);
        tolua_function(tolua_S,"getInertia",lua_ax_physics3d_Rigidbody3D_getInertia);
        tolua_function(tolua_S,"setMassData",lua_ax_physics3d_Rigidbody3D_setMassData);
        tolua_function(tolua_S,"setLinearVelocity",lua_ax_physics3d_Rigidbody3D_setLinearVelocity);
        tolua_function(tolua_S,"getLinearVelocity",lua_ax_physics3d_Rigidbody3D_getLinearVelocity);
        tolua_function(tolua_S,"setAngularVelocity",lua_ax_physics3d_Rigidbody3D_setAngularVelocity);
        tolua_function(tolua_S,"getAngularVelocity",lua_ax_physics3d_Rigidbody3D_getAngularVelocity);
        tolua_function(tolua_S,"setLinearDamping",lua_ax_physics3d_Rigidbody3D_setLinearDamping);
        tolua_function(tolua_S,"getLinearDamping",lua_ax_physics3d_Rigidbody3D_getLinearDamping);
        tolua_function(tolua_S,"setAngularDamping",lua_ax_physics3d_Rigidbody3D_setAngularDamping);
        tolua_function(tolua_S,"getAngularDamping",lua_ax_physics3d_Rigidbody3D_getAngularDamping);
        tolua_function(tolua_S,"getTotalForce",lua_ax_physics3d_Rigidbody3D_getTotalForce);
        tolua_function(tolua_S,"getTotalTorque",lua_ax_physics3d_Rigidbody3D_getTotalTorque);
        tolua_function(tolua_S,"getMaterial",lua_ax_physics3d_Rigidbody3D_getMaterial);
        tolua_function(tolua_S,"setMaterial",lua_ax_physics3d_Rigidbody3D_setMaterial);
        tolua_function(tolua_S,"getCollisionDetectionMode",lua_ax_physics3d_Rigidbody3D_getCollisionDetectionMode);
        tolua_function(tolua_S,"setCollisionDetectionMode",lua_ax_physics3d_Rigidbody3D_setCollisionDetectionMode);
        tolua_function(tolua_S,"setDynamic",lua_ax_physics3d_Rigidbody3D_setDynamic);
        tolua_function(tolua_S,"isDynamic",lua_ax_physics3d_Rigidbody3D_isDynamic);
        tolua_function(tolua_S,"setKinematic",lua_ax_physics3d_Rigidbody3D_setKinematic);
        tolua_function(tolua_S,"isKinematic",lua_ax_physics3d_Rigidbody3D_isKinematic);
        tolua_function(tolua_S,"setMotionType",lua_ax_physics3d_Rigidbody3D_setMotionType);
        tolua_function(tolua_S,"getMotionType",lua_ax_physics3d_Rigidbody3D_getMotionType);
        tolua_function(tolua_S,"setAllowSleeping",lua_ax_physics3d_Rigidbody3D_setAllowSleeping);
        tolua_function(tolua_S,"isAllowSleeping",lua_ax_physics3d_Rigidbody3D_isAllowSleeping);
        tolua_function(tolua_S,"setGravityScale",lua_ax_physics3d_Rigidbody3D_setGravityScale);
        tolua_function(tolua_S,"getGravityScale",lua_ax_physics3d_Rigidbody3D_getGravityScale);
        tolua_function(tolua_S,"setRotationEnabled",lua_ax_physics3d_Rigidbody3D_setRotationEnabled);
        tolua_function(tolua_S,"isRotationEnabled",lua_ax_physics3d_Rigidbody3D_isRotationEnabled);
        tolua_function(tolua_S,"applyForce",lua_ax_physics3d_Rigidbody3D_applyForce);
        tolua_function(tolua_S,"applyTorque",lua_ax_physics3d_Rigidbody3D_applyTorque);
        tolua_function(tolua_S,"applyImpulse",lua_ax_physics3d_Rigidbody3D_applyImpulse);
        tolua_function(tolua_S,"applyAngularImpulse",lua_ax_physics3d_Rigidbody3D_applyAngularImpulse);
        tolua_function(tolua_S,"setActive",lua_ax_physics3d_Rigidbody3D_setActive);
        tolua_function(tolua_S,"getCollider",lua_ax_physics3d_Rigidbody3D_getCollider);
        tolua_function(tolua_S,"setPosition",lua_ax_physics3d_Rigidbody3D_setPosition);
        tolua_function(tolua_S,"getPosition",lua_ax_physics3d_Rigidbody3D_getPosition);
        tolua_function(tolua_S,"setRotation",lua_ax_physics3d_Rigidbody3D_setRotation);
        tolua_function(tolua_S,"getRotation",lua_ax_physics3d_Rigidbody3D_getRotation);
        tolua_function(tolua_S,"world2Local",lua_ax_physics3d_Rigidbody3D_world2Local);
        tolua_function(tolua_S,"local2World",lua_ax_physics3d_Rigidbody3D_local2World);
        tolua_function(tolua_S,"setUserData",lua_ax_physics3d_Rigidbody3D_setUserData);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Rigidbody3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Rigidbody3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Rigidbody3D";
    g_typeCast[typeName] = "ax.Rigidbody3D";
    return 1;
}

int lua_ax_physics3d_Joint3D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_isAttached'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorld();
        object_to_luaval<ax::PhysicsWorld3D>(tolua_S, "ax.PhysicsWorld3D",(ax::PhysicsWorld3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getAttachedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getAttachedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getAttachedBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAttachedBody();
        object_to_luaval<ax::Rigidbody3D>(tolua_S, "ax.Rigidbody3D",(ax::Rigidbody3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getAttachedBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getAttachedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getConnectedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getConnectedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getConnectedBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->getConnectedBody();
        object_to_luaval<ax::Rigidbody3D>(tolua_S, "ax.Rigidbody3D",(ax::Rigidbody3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getConnectedBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getConnectedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setConnectedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setConnectedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;

        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.Joint3D:setConnectedBody");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setConnectedBody'", nullptr);
            return 0;
        }
        obj->setConnectedBody(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setConnectedBody",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setConnectedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getCoordinateSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getCoordinateSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getCoordinateSpace'", nullptr);
            return 0;
        }
        int ret = (int)obj->getCoordinateSpace();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getCoordinateSpace",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getCoordinateSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setCoordinateSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setCoordinateSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Joint3D::CoordinateSpace arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Joint3D:setCoordinateSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setCoordinateSpace'", nullptr);
            return 0;
        }
        obj->setCoordinateSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setCoordinateSpace",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setCoordinateSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAnchorsInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAnchorsInWorldSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAnchorsInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAnchorsInWorldSpace'", nullptr);
            return 0;
        }
        obj->setAnchorsInWorldSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAnchorsInWorldSpace",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAnchorsInWorldSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAnchors(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAnchors'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAnchors");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Joint3D:setAnchors");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAnchors'", nullptr);
            return 0;
        }
        obj->setAnchors(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAnchors",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAnchors'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getAnchor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnchor();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAnchor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAnchor'", nullptr);
            return 0;
        }
        obj->setAnchor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAnchor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getConnectedAnchor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getConnectedAnchor();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getConnectedAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setConnectedAnchor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setConnectedAnchor'", nullptr);
            return 0;
        }
        obj->setConnectedAnchor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setConnectedAnchor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAxesInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAxesInWorldSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAxesInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAxesInWorldSpace'", nullptr);
            return 0;
        }
        obj->setAxesInWorldSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAxesInWorldSpace",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAxesInWorldSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAxes(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAxes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAxes");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Joint3D:setAxes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAxes'", nullptr);
            return 0;
        }
        obj->setAxes(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAxes",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAxes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getAxis(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getAxis'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getAxis'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAxis();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getAxis",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getAxis'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setAxis(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setAxis'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setAxis");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setAxis'", nullptr);
            return 0;
        }
        obj->setAxis(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setAxis",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setAxis'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_getConnectedAxis(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_getConnectedAxis'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_getConnectedAxis'", nullptr);
            return 0;
        }
        auto&& ret = obj->getConnectedAxis();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:getConnectedAxis",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_getConnectedAxis'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setConnectedAxis(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setConnectedAxis'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Joint3D:setConnectedAxis");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setConnectedAxis'", nullptr);
            return 0;
        }
        obj->setConnectedAxis(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setConnectedAxis",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setConnectedAxis'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Joint3D_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_Joint3D_setUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        void* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Joint3D_setUserData'", nullptr);
            return 0;
        }
        obj->setUserData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint3D:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Joint3D_setUserData'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_Joint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Joint3D)");
    return 0;
}

int lua_register_ax_physics3d_Joint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Joint3D");
    tolua_cclass(tolua_S,"Joint3D","ax.Joint3D","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"Joint3D");
        tolua_function(tolua_S,"isAttached",lua_ax_physics3d_Joint3D_isAttached);
        tolua_function(tolua_S,"getWorld",lua_ax_physics3d_Joint3D_getWorld);
        tolua_function(tolua_S,"getAttachedBody",lua_ax_physics3d_Joint3D_getAttachedBody);
        tolua_function(tolua_S,"getConnectedBody",lua_ax_physics3d_Joint3D_getConnectedBody);
        tolua_function(tolua_S,"setConnectedBody",lua_ax_physics3d_Joint3D_setConnectedBody);
        tolua_function(tolua_S,"getCoordinateSpace",lua_ax_physics3d_Joint3D_getCoordinateSpace);
        tolua_function(tolua_S,"setCoordinateSpace",lua_ax_physics3d_Joint3D_setCoordinateSpace);
        tolua_function(tolua_S,"setAnchorsInWorldSpace",lua_ax_physics3d_Joint3D_setAnchorsInWorldSpace);
        tolua_function(tolua_S,"setAnchors",lua_ax_physics3d_Joint3D_setAnchors);
        tolua_function(tolua_S,"getAnchor",lua_ax_physics3d_Joint3D_getAnchor);
        tolua_function(tolua_S,"setAnchor",lua_ax_physics3d_Joint3D_setAnchor);
        tolua_function(tolua_S,"getConnectedAnchor",lua_ax_physics3d_Joint3D_getConnectedAnchor);
        tolua_function(tolua_S,"setConnectedAnchor",lua_ax_physics3d_Joint3D_setConnectedAnchor);
        tolua_function(tolua_S,"setAxesInWorldSpace",lua_ax_physics3d_Joint3D_setAxesInWorldSpace);
        tolua_function(tolua_S,"setAxes",lua_ax_physics3d_Joint3D_setAxes);
        tolua_function(tolua_S,"getAxis",lua_ax_physics3d_Joint3D_getAxis);
        tolua_function(tolua_S,"setAxis",lua_ax_physics3d_Joint3D_setAxis);
        tolua_function(tolua_S,"getConnectedAxis",lua_ax_physics3d_Joint3D_getConnectedAxis);
        tolua_function(tolua_S,"setConnectedAxis",lua_ax_physics3d_Joint3D_setConnectedAxis);
        tolua_function(tolua_S,"setUserData",lua_ax_physics3d_Joint3D_setUserData);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Joint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Joint3D";
    g_typeCast[typeName] = "ax.Joint3D";
    return 1;
}

int lua_ax_physics3d_PivotJoint3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PivotJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PivotJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PivotJoint3D::create();
        object_to_luaval<ax::PivotJoint3D>(tolua_S, "ax.PivotJoint3D",(ax::PivotJoint3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.PivotJoint3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PivotJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PivotJoint3D::create(arg0);
        object_to_luaval<ax::PivotJoint3D>(tolua_S, "ax.PivotJoint3D",(ax::PivotJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PivotJoint3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PivotJoint3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_PivotJoint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PivotJoint3D)");
    return 0;
}

int lua_register_ax_physics3d_PivotJoint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PivotJoint3D");
    tolua_cclass(tolua_S,"PivotJoint3D","ax.PivotJoint3D","ax.Joint3D",nullptr);

    tolua_beginmodule(tolua_S,"PivotJoint3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_PivotJoint3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PivotJoint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PivotJoint3D";
    g_typeCast[typeName] = "ax.PivotJoint3D";
    return 1;
}

int lua_ax_physics3d_HingeJoint3D_setUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_setUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.HingeJoint3D:setUseMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_setUseMotor'", nullptr);
            return 0;
        }
        obj->setUseMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:setUseMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_setUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_isUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_isUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_isUseMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:isUseMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_isUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointLimits arg0;

        ok &= luaval_to_joint_limits(tolua_S, 2, &arg0, "ax.HingeJoint3D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.HingeJoint3D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_clearLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_clearLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_clearLimits'", nullptr);
            return 0;
        }
        obj->clearLimits();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:clearLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_clearLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_hasLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_hasLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_hasLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:hasLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_hasLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        joint_limits_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_setMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_setMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointMotor arg0;

        ok &= luaval_to_joint_motor(tolua_S, 2, &arg0, "ax.HingeJoint3D:setMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_setMotor'", nullptr);
            return 0;
        }
        obj->setMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:setMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_setMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_getMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_getMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_getMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotor();
        joint_motor_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:getMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_getMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_setSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_setSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointSpring arg0;

        ok &= luaval_to_joint_spring(tolua_S, 2, &arg0, "ax.HingeJoint3D:setSpring");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_setSpring'", nullptr);
            return 0;
        }
        obj->setSpring(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:setSpring",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_setSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_getSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_getSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_getSpring'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpring();
        joint_spring_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:getSpring",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_getSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_getVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_getVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_getVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:getVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_getVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_getAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::HingeJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::HingeJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_HingeJoint3D_getAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_getAngle'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.HingeJoint3D:getAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_getAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_HingeJoint3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::HingeJoint3D::create();
        object_to_luaval<ax::HingeJoint3D>(tolua_S, "ax.HingeJoint3D",(ax::HingeJoint3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.HingeJoint3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::HingeJoint3D::create(arg0);
        object_to_luaval<ax::HingeJoint3D>(tolua_S, "ax.HingeJoint3D",(ax::HingeJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.HingeJoint3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_HingeJoint3D_createInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.HingeJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.HingeJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.HingeJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::HingeJoint3D::createInWorldSpace(arg0, arg1);
        object_to_luaval<ax::HingeJoint3D>(tolua_S, "ax.HingeJoint3D",(ax::HingeJoint3D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Rigidbody3D* arg2;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.HingeJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.HingeJoint3D:createInWorldSpace");
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 4, "ax.Rigidbody3D",&arg2, "ax.HingeJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_HingeJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::HingeJoint3D::createInWorldSpace(arg0, arg1, arg2);
        object_to_luaval<ax::HingeJoint3D>(tolua_S, "ax.HingeJoint3D",(ax::HingeJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.HingeJoint3D:createInWorldSpace",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_HingeJoint3D_createInWorldSpace'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_HingeJoint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (HingeJoint3D)");
    return 0;
}

int lua_register_ax_physics3d_HingeJoint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.HingeJoint3D");
    tolua_cclass(tolua_S,"HingeJoint3D","ax.HingeJoint3D","ax.Joint3D",nullptr);

    tolua_beginmodule(tolua_S,"HingeJoint3D");
        tolua_function(tolua_S,"setUseMotor",lua_ax_physics3d_HingeJoint3D_setUseMotor);
        tolua_function(tolua_S,"isUseMotor",lua_ax_physics3d_HingeJoint3D_isUseMotor);
        tolua_function(tolua_S,"setLimits",lua_ax_physics3d_HingeJoint3D_setLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics3d_HingeJoint3D_setUseLimits);
        tolua_function(tolua_S,"clearLimits",lua_ax_physics3d_HingeJoint3D_clearLimits);
        tolua_function(tolua_S,"hasLimits",lua_ax_physics3d_HingeJoint3D_hasLimits);
        tolua_function(tolua_S,"getLimits",lua_ax_physics3d_HingeJoint3D_getLimits);
        tolua_function(tolua_S,"setMotor",lua_ax_physics3d_HingeJoint3D_setMotor);
        tolua_function(tolua_S,"getMotor",lua_ax_physics3d_HingeJoint3D_getMotor);
        tolua_function(tolua_S,"setSpring",lua_ax_physics3d_HingeJoint3D_setSpring);
        tolua_function(tolua_S,"getSpring",lua_ax_physics3d_HingeJoint3D_getSpring);
        tolua_function(tolua_S,"getVelocity",lua_ax_physics3d_HingeJoint3D_getVelocity);
        tolua_function(tolua_S,"getAngle",lua_ax_physics3d_HingeJoint3D_getAngle);
        tolua_function(tolua_S,"create", lua_ax_physics3d_HingeJoint3D_create);
        tolua_function(tolua_S,"createInWorldSpace", lua_ax_physics3d_HingeJoint3D_createInWorldSpace);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::HingeJoint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.HingeJoint3D";
    g_typeCast[typeName] = "ax.HingeJoint3D";
    return 1;
}

int lua_ax_physics3d_SliderJoint3D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointLimits arg0;

        ok &= luaval_to_joint_limits(tolua_S, 2, &arg0, "ax.SliderJoint3D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_clearLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_clearLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_clearLimits'", nullptr);
            return 0;
        }
        obj->clearLimits();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:clearLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_clearLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_hasLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_hasLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_hasLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:hasLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_hasLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        joint_limits_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_setUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_setUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.SliderJoint3D:setUseMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_setUseMotor'", nullptr);
            return 0;
        }
        obj->setUseMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:setUseMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_setUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_isUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_isUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_isUseMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:isUseMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_isUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_setMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_setMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointMotor arg0;

        ok &= luaval_to_joint_motor(tolua_S, 2, &arg0, "ax.SliderJoint3D:setMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_setMotor'", nullptr);
            return 0;
        }
        obj->setMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:setMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_setMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_getMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_getMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_getMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotor();
        joint_motor_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:getMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_getMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_setSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_setSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointSpring arg0;

        ok &= luaval_to_joint_spring(tolua_S, 2, &arg0, "ax.SliderJoint3D:setSpring");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_setSpring'", nullptr);
            return 0;
        }
        obj->setSpring(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:setSpring",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_setSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_getSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_getSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_getSpring'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpring();
        joint_spring_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:getSpring",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_getSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_getVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SliderJoint3D_getVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_getVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint3D:getVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_getVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SliderJoint3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SliderJoint3D::create();
        object_to_luaval<ax::SliderJoint3D>(tolua_S, "ax.SliderJoint3D",(ax::SliderJoint3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.SliderJoint3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SliderJoint3D::create(arg0);
        object_to_luaval<ax::SliderJoint3D>(tolua_S, "ax.SliderJoint3D",(ax::SliderJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SliderJoint3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_SliderJoint3D_createInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SliderJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Vec3 arg2;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.SliderJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.SliderJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.SliderJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::SliderJoint3D::createInWorldSpace(arg0, arg1, arg2);
        object_to_luaval<ax::SliderJoint3D>(tolua_S, "ax.SliderJoint3D",(ax::SliderJoint3D*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Vec3 arg2;
        ax::Rigidbody3D* arg3;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.SliderJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.SliderJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.SliderJoint3D:createInWorldSpace");
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 5, "ax.Rigidbody3D",&arg3, "ax.SliderJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SliderJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::SliderJoint3D::createInWorldSpace(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::SliderJoint3D>(tolua_S, "ax.SliderJoint3D",(ax::SliderJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SliderJoint3D:createInWorldSpace",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SliderJoint3D_createInWorldSpace'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_SliderJoint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SliderJoint3D)");
    return 0;
}

int lua_register_ax_physics3d_SliderJoint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.SliderJoint3D");
    tolua_cclass(tolua_S,"SliderJoint3D","ax.SliderJoint3D","ax.Joint3D",nullptr);

    tolua_beginmodule(tolua_S,"SliderJoint3D");
        tolua_function(tolua_S,"setLimits",lua_ax_physics3d_SliderJoint3D_setLimits);
        tolua_function(tolua_S,"clearLimits",lua_ax_physics3d_SliderJoint3D_clearLimits);
        tolua_function(tolua_S,"hasLimits",lua_ax_physics3d_SliderJoint3D_hasLimits);
        tolua_function(tolua_S,"getLimits",lua_ax_physics3d_SliderJoint3D_getLimits);
        tolua_function(tolua_S,"setUseMotor",lua_ax_physics3d_SliderJoint3D_setUseMotor);
        tolua_function(tolua_S,"isUseMotor",lua_ax_physics3d_SliderJoint3D_isUseMotor);
        tolua_function(tolua_S,"setMotor",lua_ax_physics3d_SliderJoint3D_setMotor);
        tolua_function(tolua_S,"getMotor",lua_ax_physics3d_SliderJoint3D_getMotor);
        tolua_function(tolua_S,"setSpring",lua_ax_physics3d_SliderJoint3D_setSpring);
        tolua_function(tolua_S,"getSpring",lua_ax_physics3d_SliderJoint3D_getSpring);
        tolua_function(tolua_S,"getVelocity",lua_ax_physics3d_SliderJoint3D_getVelocity);
        tolua_function(tolua_S,"create", lua_ax_physics3d_SliderJoint3D_create);
        tolua_function(tolua_S,"createInWorldSpace", lua_ax_physics3d_SliderJoint3D_createInWorldSpace);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::SliderJoint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.SliderJoint3D";
    g_typeCast[typeName] = "ax.SliderJoint3D";
    return 1;
}

int lua_ax_physics3d_ConeTwistJoint3D_setUseTwistMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseTwistMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setUseTwistMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseTwistMotor'", nullptr);
            return 0;
        }
        obj->setUseTwistMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setUseTwistMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseTwistMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_isUseTwistMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseTwistMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseTwistMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseTwistMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:isUseTwistMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseTwistMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setUseSwingMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseSwingMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setUseSwingMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseSwingMotor'", nullptr);
            return 0;
        }
        obj->setUseSwingMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setUseSwingMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseSwingMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_isUseSwingMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseSwingMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseSwingMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseSwingMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:isUseSwingMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_isUseSwingMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::TwistLimits arg0;

        ok &= luaval_to_twist_limits(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_clearLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_clearLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_clearLimits'", nullptr);
            return 0;
        }
        obj->clearLimits();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:clearLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_clearLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_hasLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_hasLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_hasLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:hasLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_hasLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        twist_limits_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setTwistMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointMotor arg0;

        ok &= luaval_to_joint_motor(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setTwistMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistMotor'", nullptr);
            return 0;
        }
        obj->setTwistMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setTwistMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getTwistMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTwistMotor();
        joint_motor_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getTwistMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setSwingMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointMotor arg0;

        ok &= luaval_to_joint_motor(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setSwingMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingMotor'", nullptr);
            return 0;
        }
        obj->setSwingMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setSwingMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getSwingMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSwingMotor();
        joint_motor_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getSwingMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setTwistSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointSpring arg0;

        ok &= luaval_to_joint_spring(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setTwistSpring");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistSpring'", nullptr);
            return 0;
        }
        obj->setTwistSpring(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setTwistSpring",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setTwistSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getTwistSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistSpring'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTwistSpring();
        joint_spring_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getTwistSpring",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getTwistSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_setSwingSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::JointSpring arg0;

        ok &= luaval_to_joint_spring(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:setSwingSpring");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingSpring'", nullptr);
            return 0;
        }
        obj->setSwingSpring(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:setSwingSpring",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_setSwingSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getSwingSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingSpring'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSwingSpring();
        joint_spring_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getSwingSpring",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getSwingSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_getPointForAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::ConeTwistJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ConeTwistJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ConeTwistJoint3D_getPointForAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:getPointForAngle");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.ConeTwistJoint3D:getPointForAngle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_getPointForAngle'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointForAngle(arg0, arg1);
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ConeTwistJoint3D:getPointForAngle",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_getPointForAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConeTwistJoint3D::create();
        object_to_luaval<ax::ConeTwistJoint3D>(tolua_S, "ax.ConeTwistJoint3D",(ax::ConeTwistJoint3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.ConeTwistJoint3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConeTwistJoint3D::create(arg0);
        object_to_luaval<ax::ConeTwistJoint3D>(tolua_S, "ax.ConeTwistJoint3D",(ax::ConeTwistJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.ConeTwistJoint3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_ConeTwistJoint3D_createInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ConeTwistJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.ConeTwistJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConeTwistJoint3D::createInWorldSpace(arg0, arg1);
        object_to_luaval<ax::ConeTwistJoint3D>(tolua_S, "ax.ConeTwistJoint3D",(ax::ConeTwistJoint3D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Rigidbody3D* arg2;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.ConeTwistJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.ConeTwistJoint3D:createInWorldSpace");
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 4, "ax.Rigidbody3D",&arg2, "ax.ConeTwistJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ConeTwistJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::ConeTwistJoint3D::createInWorldSpace(arg0, arg1, arg2);
        object_to_luaval<ax::ConeTwistJoint3D>(tolua_S, "ax.ConeTwistJoint3D",(ax::ConeTwistJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.ConeTwistJoint3D:createInWorldSpace",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ConeTwistJoint3D_createInWorldSpace'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_ConeTwistJoint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ConeTwistJoint3D)");
    return 0;
}

int lua_register_ax_physics3d_ConeTwistJoint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ConeTwistJoint3D");
    tolua_cclass(tolua_S,"ConeTwistJoint3D","ax.ConeTwistJoint3D","ax.Joint3D",nullptr);

    tolua_beginmodule(tolua_S,"ConeTwistJoint3D");
        tolua_function(tolua_S,"setUseTwistMotor",lua_ax_physics3d_ConeTwistJoint3D_setUseTwistMotor);
        tolua_function(tolua_S,"isUseTwistMotor",lua_ax_physics3d_ConeTwistJoint3D_isUseTwistMotor);
        tolua_function(tolua_S,"setUseSwingMotor",lua_ax_physics3d_ConeTwistJoint3D_setUseSwingMotor);
        tolua_function(tolua_S,"isUseSwingMotor",lua_ax_physics3d_ConeTwistJoint3D_isUseSwingMotor);
        tolua_function(tolua_S,"setLimits",lua_ax_physics3d_ConeTwistJoint3D_setLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics3d_ConeTwistJoint3D_setUseLimits);
        tolua_function(tolua_S,"clearLimits",lua_ax_physics3d_ConeTwistJoint3D_clearLimits);
        tolua_function(tolua_S,"hasLimits",lua_ax_physics3d_ConeTwistJoint3D_hasLimits);
        tolua_function(tolua_S,"getLimits",lua_ax_physics3d_ConeTwistJoint3D_getLimits);
        tolua_function(tolua_S,"setTwistMotor",lua_ax_physics3d_ConeTwistJoint3D_setTwistMotor);
        tolua_function(tolua_S,"getTwistMotor",lua_ax_physics3d_ConeTwistJoint3D_getTwistMotor);
        tolua_function(tolua_S,"setSwingMotor",lua_ax_physics3d_ConeTwistJoint3D_setSwingMotor);
        tolua_function(tolua_S,"getSwingMotor",lua_ax_physics3d_ConeTwistJoint3D_getSwingMotor);
        tolua_function(tolua_S,"setTwistSpring",lua_ax_physics3d_ConeTwistJoint3D_setTwistSpring);
        tolua_function(tolua_S,"getTwistSpring",lua_ax_physics3d_ConeTwistJoint3D_getTwistSpring);
        tolua_function(tolua_S,"setSwingSpring",lua_ax_physics3d_ConeTwistJoint3D_setSwingSpring);
        tolua_function(tolua_S,"getSwingSpring",lua_ax_physics3d_ConeTwistJoint3D_getSwingSpring);
        tolua_function(tolua_S,"getPointForAngle",lua_ax_physics3d_ConeTwistJoint3D_getPointForAngle);
        tolua_function(tolua_S,"create", lua_ax_physics3d_ConeTwistJoint3D_create);
        tolua_function(tolua_S,"createInWorldSpace", lua_ax_physics3d_ConeTwistJoint3D_createInWorldSpace);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ConeTwistJoint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ConeTwistJoint3D";
    g_typeCast[typeName] = "ax.ConeTwistJoint3D";
    return 1;
}

int lua_ax_physics3d_SixDofJoint3D_setLinearLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SixDofJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SixDofJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SixDofJoint3D_setLinearLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::SixDofLimits arg0;

        ok &= luaval_to_six_dof_limits(tolua_S, 2, &arg0, "ax.SixDofJoint3D:setLinearLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_setLinearLimits'", nullptr);
            return 0;
        }
        obj->setLinearLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SixDofJoint3D:setLinearLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_setLinearLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_getLinearLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SixDofJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SixDofJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SixDofJoint3D_getLinearLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_getLinearLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearLimits();
        six_dof_limits_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SixDofJoint3D:getLinearLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_getLinearLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_setAngularLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SixDofJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SixDofJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SixDofJoint3D_setAngularLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::SixDofLimits arg0;

        ok &= luaval_to_six_dof_limits(tolua_S, 2, &arg0, "ax.SixDofJoint3D:setAngularLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_setAngularLimits'", nullptr);
            return 0;
        }
        obj->setAngularLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SixDofJoint3D:setAngularLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_setAngularLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_getAngularLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SixDofJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SixDofJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SixDofJoint3D_getAngularLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_getAngularLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularLimits();
        six_dof_limits_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SixDofJoint3D:getAngularLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_getAngularLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_isLimited(lua_State* tolua_S)
{
    int argc = 0;
    ax::SixDofJoint3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SixDofJoint3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_SixDofJoint3D_isLimited'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.SixDofJoint3D:isLimited");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_isLimited'", nullptr);
            return 0;
        }
        auto&& ret = obj->isLimited(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SixDofJoint3D:isLimited",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_isLimited'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SixDofJoint3D::create();
        object_to_luaval<ax::SixDofJoint3D>(tolua_S, "ax.SixDofJoint3D",(ax::SixDofJoint3D*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::Rigidbody3D* arg0;
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 2, "ax.Rigidbody3D",&arg0, "ax.SixDofJoint3D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SixDofJoint3D::create(arg0);
        object_to_luaval<ax::SixDofJoint3D>(tolua_S, "ax.SixDofJoint3D",(ax::SixDofJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SixDofJoint3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_SixDofJoint3D_createInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SixDofJoint3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.SixDofJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.SixDofJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::SixDofJoint3D::createInWorldSpace(arg0, arg1);
        object_to_luaval<ax::SixDofJoint3D>(tolua_S, "ax.SixDofJoint3D",(ax::SixDofJoint3D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Rigidbody3D* arg2;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.SixDofJoint3D:createInWorldSpace");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.SixDofJoint3D:createInWorldSpace");
        ok &= luaval_to_object<ax::Rigidbody3D>(tolua_S, 4, "ax.Rigidbody3D",&arg2, "ax.SixDofJoint3D:createInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_SixDofJoint3D_createInWorldSpace'", nullptr);
            return 0;
        }
        auto&& ret = ax::SixDofJoint3D::createInWorldSpace(arg0, arg1, arg2);
        object_to_luaval<ax::SixDofJoint3D>(tolua_S, "ax.SixDofJoint3D",(ax::SixDofJoint3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SixDofJoint3D:createInWorldSpace",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_SixDofJoint3D_createInWorldSpace'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_SixDofJoint3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SixDofJoint3D)");
    return 0;
}

int lua_register_ax_physics3d_SixDofJoint3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.SixDofJoint3D");
    tolua_cclass(tolua_S,"SixDofJoint3D","ax.SixDofJoint3D","ax.Joint3D",nullptr);

    tolua_beginmodule(tolua_S,"SixDofJoint3D");
        tolua_function(tolua_S,"setLinearLimits",lua_ax_physics3d_SixDofJoint3D_setLinearLimits);
        tolua_function(tolua_S,"getLinearLimits",lua_ax_physics3d_SixDofJoint3D_getLinearLimits);
        tolua_function(tolua_S,"setAngularLimits",lua_ax_physics3d_SixDofJoint3D_setAngularLimits);
        tolua_function(tolua_S,"getAngularLimits",lua_ax_physics3d_SixDofJoint3D_getAngularLimits);
        tolua_function(tolua_S,"isLimited",lua_ax_physics3d_SixDofJoint3D_isLimited);
        tolua_function(tolua_S,"create", lua_ax_physics3d_SixDofJoint3D_create);
        tolua_function(tolua_S,"createInWorldSpace", lua_ax_physics3d_SixDofJoint3D_createInWorldSpace);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::SixDofJoint3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.SixDofJoint3D";
    g_typeCast[typeName] = "ax.SixDofJoint3D";
    return 1;
}

int lua_ax_physics3d_ContactEvent3D_getActorA(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ContactEvent3D_getActorA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ContactEvent3D_getActorA'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActorA();
        object_to_luaval<ax::PhysicsActor>(tolua_S, "ax.PhysicsActor",(ax::PhysicsActor*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent3D:getActorA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ContactEvent3D_getActorA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ContactEvent3D_getActorB(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ContactEvent3D_getActorB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ContactEvent3D_getActorB'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActorB();
        object_to_luaval<ax::PhysicsActor>(tolua_S, "ax.PhysicsActor",(ax::PhysicsActor*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent3D:getActorB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ContactEvent3D_getActorB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ContactEvent3D_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ContactEvent3D_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ContactEvent3D_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)obj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent3D:getEventCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ContactEvent3D_getEventCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_ContactEvent3D_getContactInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_ContactEvent3D_getContactInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ContactEvent3D_getContactInfo'", nullptr);
            return 0;
        }
        auto&& ret = obj->getContactInfo();
        contact_info_3d_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent3D:getContactInfo",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ContactEvent3D_getContactInfo'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_ContactEvent3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ContactEvent3D)");
    return 0;
}

int lua_register_ax_physics3d_ContactEvent3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ContactEvent3D");
    tolua_cclass(tolua_S,"ContactEvent3D","ax.ContactEvent3D","ax.CustomEvent",nullptr);

    tolua_beginmodule(tolua_S,"ContactEvent3D");
        tolua_function(tolua_S,"getActorA",lua_ax_physics3d_ContactEvent3D_getActorA);
        tolua_function(tolua_S,"getActorB",lua_ax_physics3d_ContactEvent3D_getActorB);
        tolua_function(tolua_S,"getEventCode",lua_ax_physics3d_ContactEvent3D_getEventCode);
        tolua_function(tolua_S,"getContactInfo",lua_ax_physics3d_ContactEvent3D_getContactInfo);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ContactEvent3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ContactEvent3D";
    g_typeCast[typeName] = "ax.ContactEvent3D";
    return 1;
}

int lua_ax_physics3d_ContactEventListener3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ContactEventListener3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_ContactEventListener3D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ContactEventListener3D::create();
        object_to_luaval<ax::ContactEventListener3D>(tolua_S, "ax.ContactEventListener3D",(ax::ContactEventListener3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.ContactEventListener3D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_ContactEventListener3D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics3d_ContactEventListener3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ContactEventListener3D)");
    return 0;
}

int lua_register_ax_physics3d_ContactEventListener3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ContactEventListener3D");
    tolua_cclass(tolua_S,"ContactEventListener3D","ax.ContactEventListener3D","ax.CustomEventListener",nullptr);

    tolua_beginmodule(tolua_S,"ContactEventListener3D");
        tolua_function(tolua_S,"create", lua_ax_physics3d_ContactEventListener3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ContactEventListener3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ContactEventListener3D";
    g_typeCast[typeName] = "ax.ContactEventListener3D";
    return 1;
}

int lua_ax_physics3d_PhysicsWorld3D_getScene(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_getScene'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_getScene'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScene();
        object_to_luaval<ax::Scene>(tolua_S, "ax.Scene",(ax::Scene*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:getScene",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_getScene'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_setGravity'", nullptr);
            return 0;
        }
        obj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:setGravity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_getGravity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGravity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:getGravity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_setAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_setAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:setAutoStep");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_setAutoStep'", nullptr);
            return 0;
        }
        obj->setAutoStep(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:setAutoStep",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_setAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_isAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_isAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_isAutoStep'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAutoStep();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:isAutoStep",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_isAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_stepSimulation(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_stepSimulation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:stepSimulation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_stepSimulation'", nullptr);
            return 0;
        }
        obj->stepSimulation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:stepSimulation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_stepSimulation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_setDebugDrawEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_setDebugDrawEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:setDebugDrawEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_setDebugDrawEnabled'", nullptr);
            return 0;
        }
        obj->setDebugDrawEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:setDebugDrawEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_setDebugDrawEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_isDebugDrawEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_isDebugDrawEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_isDebugDrawEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isDebugDrawEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:isDebugDrawEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_isDebugDrawEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_setGlobalEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_setGlobalEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ContactEventBits arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:setGlobalEventEnabled");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.PhysicsWorld3D:setGlobalEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_setGlobalEventEnabled'", nullptr);
            return 0;
        }
        obj->setGlobalEventEnabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:setGlobalEventEnabled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_setGlobalEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_isGlobalEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_isGlobalEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ContactEventBits arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsWorld3D:isGlobalEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_isGlobalEventEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isGlobalEventEnabled(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:isGlobalEventEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_isGlobalEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_addPhysicsActor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_addPhysicsActor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsActor* arg0;

        ok &= luaval_to_object<ax::PhysicsActor>(tolua_S, 2, "ax.PhysicsActor",&arg0, "ax.PhysicsWorld3D:addPhysicsActor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_addPhysicsActor'", nullptr);
            return 0;
        }
        obj->addPhysicsActor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:addPhysicsActor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_addPhysicsActor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_removePhysicsActor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics3d_PhysicsWorld3D_removePhysicsActor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsActor* arg0;

        ok &= luaval_to_object<ax::PhysicsActor>(tolua_S, 2, "ax.PhysicsActor",&arg0, "ax.PhysicsWorld3D:removePhysicsActor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_removePhysicsActor'", nullptr);
            return 0;
        }
        obj->removePhysicsActor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:removePhysicsActor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_removePhysicsActor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_obtain(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Scene* arg0;
        ok &= luaval_to_object<ax::Scene>(tolua_S, 2, "ax.Scene",&arg0, "ax.PhysicsWorld3D:obtain");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_obtain'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsWorld3D::obtain(arg0);
        object_to_luaval<ax::PhysicsWorld3D>(tolua_S, "ax.PhysicsWorld3D",(ax::PhysicsWorld3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsWorld3D:obtain",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_obtain'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_release(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsWorld3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::PhysicsWorld3D* arg0;
        ok &= luaval_to_object<ax::PhysicsWorld3D>(tolua_S, 2, "ax.PhysicsWorld3D",&arg0, "ax.PhysicsWorld3D:release");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_release'", nullptr);
            return 0;
        }
        ax::PhysicsWorld3D::release(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsWorld3D:release",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_release'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_PhysicsWorld3D_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld3D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsWorld3D_constructor'", nullptr);
            return 0;
        }
        obj = new ax::PhysicsWorld3D();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"ax.PhysicsWorld3D");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld3D:PhysicsWorld3D",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsWorld3D_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_PhysicsWorld3D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PhysicsWorld3D)");
    return 0;
}

int lua_register_ax_physics3d_PhysicsWorld3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsWorld3D");
    tolua_cclass(tolua_S,"PhysicsWorld3D","ax.PhysicsWorld3D","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsWorld3D");
        tolua_function(tolua_S,"new",lua_ax_physics3d_PhysicsWorld3D_constructor);
        tolua_function(tolua_S,"getScene",lua_ax_physics3d_PhysicsWorld3D_getScene);
        tolua_function(tolua_S,"setGravity",lua_ax_physics3d_PhysicsWorld3D_setGravity);
        tolua_function(tolua_S,"getGravity",lua_ax_physics3d_PhysicsWorld3D_getGravity);
        tolua_function(tolua_S,"setAutoStep",lua_ax_physics3d_PhysicsWorld3D_setAutoStep);
        tolua_function(tolua_S,"isAutoStep",lua_ax_physics3d_PhysicsWorld3D_isAutoStep);
        tolua_function(tolua_S,"stepSimulation",lua_ax_physics3d_PhysicsWorld3D_stepSimulation);
        tolua_function(tolua_S,"setDebugDrawEnabled",lua_ax_physics3d_PhysicsWorld3D_setDebugDrawEnabled);
        tolua_function(tolua_S,"isDebugDrawEnabled",lua_ax_physics3d_PhysicsWorld3D_isDebugDrawEnabled);
        tolua_function(tolua_S,"setGlobalEventEnabled",lua_ax_physics3d_PhysicsWorld3D_setGlobalEventEnabled);
        tolua_function(tolua_S,"isGlobalEventEnabled",lua_ax_physics3d_PhysicsWorld3D_isGlobalEventEnabled);
        tolua_function(tolua_S,"addPhysicsActor",lua_ax_physics3d_PhysicsWorld3D_addPhysicsActor);
        tolua_function(tolua_S,"removePhysicsActor",lua_ax_physics3d_PhysicsWorld3D_removePhysicsActor);
        tolua_function(tolua_S,"obtain", lua_ax_physics3d_PhysicsWorld3D_obtain);
        tolua_function(tolua_S,"release", lua_ax_physics3d_PhysicsWorld3D_release);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsWorld3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsWorld3D";
    g_typeCast[typeName] = "ax.PhysicsWorld3D";
    return 1;
}
TOLUA_API int register_all_ax_physics3d(lua_State* tolua_S)
{
    tolua_open(tolua_S);

    tolua_module(tolua_S,"ax",0);
    tolua_beginmodule(tolua_S,"ax");

    lua_register_ax_physics3d_PhysicsActor(tolua_S);
    lua_register_ax_physics3d_Collider3D(tolua_S);
    lua_register_ax_physics3d_BoxCollider3D(tolua_S);
    lua_register_ax_physics3d_SphereCollider3D(tolua_S);
    lua_register_ax_physics3d_CylinderCollider3D(tolua_S);
    lua_register_ax_physics3d_CapsuleCollider3D(tolua_S);
    lua_register_ax_physics3d_ConvexCollider3D(tolua_S);
    lua_register_ax_physics3d_MeshCollider3D(tolua_S);
    lua_register_ax_physics3d_HeightFieldCollider3D(tolua_S);
    lua_register_ax_physics3d_Rigidbody3D(tolua_S);
    lua_register_ax_physics3d_Joint3D(tolua_S);
    lua_register_ax_physics3d_PivotJoint3D(tolua_S);
    lua_register_ax_physics3d_HingeJoint3D(tolua_S);
    lua_register_ax_physics3d_SliderJoint3D(tolua_S);
    lua_register_ax_physics3d_ConeTwistJoint3D(tolua_S);
    lua_register_ax_physics3d_SixDofJoint3D(tolua_S);
    lua_register_ax_physics3d_ContactEvent3D(tolua_S);
    lua_register_ax_physics3d_ContactEventListener3D(tolua_S);
    lua_register_ax_physics3d_PhysicsWorld3D(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

#endif
