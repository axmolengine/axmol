#include "lua-bindings/auto/axlua_physics2d_auto.hpp"
#if defined(AX_ENABLE_PHYSICS_2D)
#include "axmol/physics/physics-2d.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_physics2d_Collider2D_getAttachedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getAttachedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getAttachedBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAttachedBody();
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getAttachedBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getAttachedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Collider2D:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setTag'", nullptr);
            return 0;
        }
        obj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getDensity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getDensity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getDensity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDensity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getDensity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getDensity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setDensity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setDensity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setDensity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setDensity'", nullptr);
            return 0;
        }
        obj->setDensity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setDensity");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Collider2D:setDensity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setDensity'", nullptr);
            return 0;
        }
        obj->setDensity(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setDensity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setDensity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getRestitution'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRestitution();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getRestitution",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setRestitution");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setRestitution'", nullptr);
            return 0;
        }
        obj->setRestitution(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setRestitution");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Collider2D:setRestitution");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setRestitution'", nullptr);
            return 0;
        }
        obj->setRestitution(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setRestitution",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getFriction'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setFriction'", nullptr);
            return 0;
        }
        obj->setFriction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Collider2D:setFriction");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Collider2D:setFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setFriction'", nullptr);
            return 0;
        }
        obj->setFriction(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setFriction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getMaterial'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaterial();
        physics_material2d_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getMaterial",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::PhysicsMaterial2D arg0;

        ok &= luaval_to_physics_material2d(tolua_S, 2, &arg0, "ax.Collider2D:setMaterial");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setMaterial'", nullptr);
            return 0;
        }
        obj->setMaterial(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setMaterial",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_isSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_isSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_isSensor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isSensor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:isSensor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_isSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Collider2D:setSensor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setSensor'", nullptr);
            return 0;
        }
        obj->setSensor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setSensor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_isEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_isEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ContactEventBits arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Collider2D:isEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_isEventEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isEventEnabled(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:isEventEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_isEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ContactEventBits arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Collider2D:setEventEnabled");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Collider2D:setEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setEventEnabled'", nullptr);
            return 0;
        }
        obj->setEventEnabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setEventEnabled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getOffset'", nullptr);
            return 0;
        }
        auto&& ret = obj->getOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getCenter(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getCenter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getCenter'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCenter();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getCenter",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getCenter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_containsPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_containsPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Collider2D:containsPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_containsPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->containsPoint(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:containsPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_containsPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setCategoryBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setCategoryBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned long long arg0;

        ok &= luaval_to_long_long(tolua_S, 2, (long long*)&arg0, "ax.Collider2D:setCategoryBits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setCategoryBits'", nullptr);
            return 0;
        }
        obj->setCategoryBits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setCategoryBits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setCategoryBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getCategoryBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getCategoryBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getCategoryBits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCategoryBits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getCategoryBits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getCategoryBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setMaskBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setMaskBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned long long arg0;

        ok &= luaval_to_long_long(tolua_S, 2, (long long*)&arg0, "ax.Collider2D:setMaskBits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setMaskBits'", nullptr);
            return 0;
        }
        obj->setMaskBits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setMaskBits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setMaskBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getMaskBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getMaskBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getMaskBits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaskBits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getMaskBits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getMaskBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_setGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_setGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Collider2D:setGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_setGroup'", nullptr);
            return 0;
        }
        obj->setGroup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:setGroup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_setGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_getGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_getGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_getGroup'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGroup();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:getGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_getGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_deatchFromBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_deatchFromBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_deatchFromBody'", nullptr);
            return 0;
        }
        obj->deatchFromBody();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:deatchFromBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_deatchFromBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Collider2D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Collider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Collider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Collider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Collider2D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Collider2D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Collider2D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Collider2D_isAttached'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics2d_Collider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Collider2D)");
    return 0;
}

int lua_register_ax_physics2d_Collider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Collider2D");
    tolua_cclass(tolua_S,"Collider2D","ax.Collider2D","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Collider2D");
        tolua_function(tolua_S,"getAttachedBody",lua_ax_physics2d_Collider2D_getAttachedBody);
        tolua_function(tolua_S,"getType",lua_ax_physics2d_Collider2D_getType);
        tolua_function(tolua_S,"setTag",lua_ax_physics2d_Collider2D_setTag);
        tolua_function(tolua_S,"getTag",lua_ax_physics2d_Collider2D_getTag);
        tolua_function(tolua_S,"getDensity",lua_ax_physics2d_Collider2D_getDensity);
        tolua_function(tolua_S,"setDensity",lua_ax_physics2d_Collider2D_setDensity);
        tolua_function(tolua_S,"getRestitution",lua_ax_physics2d_Collider2D_getRestitution);
        tolua_function(tolua_S,"setRestitution",lua_ax_physics2d_Collider2D_setRestitution);
        tolua_function(tolua_S,"getFriction",lua_ax_physics2d_Collider2D_getFriction);
        tolua_function(tolua_S,"setFriction",lua_ax_physics2d_Collider2D_setFriction);
        tolua_function(tolua_S,"getMaterial",lua_ax_physics2d_Collider2D_getMaterial);
        tolua_function(tolua_S,"setMaterial",lua_ax_physics2d_Collider2D_setMaterial);
        tolua_function(tolua_S,"isSensor",lua_ax_physics2d_Collider2D_isSensor);
        tolua_function(tolua_S,"setSensor",lua_ax_physics2d_Collider2D_setSensor);
        tolua_function(tolua_S,"isEventEnabled",lua_ax_physics2d_Collider2D_isEventEnabled);
        tolua_function(tolua_S,"setEventEnabled",lua_ax_physics2d_Collider2D_setEventEnabled);
        tolua_function(tolua_S,"getOffset",lua_ax_physics2d_Collider2D_getOffset);
        tolua_function(tolua_S,"getCenter",lua_ax_physics2d_Collider2D_getCenter);
        tolua_function(tolua_S,"containsPoint",lua_ax_physics2d_Collider2D_containsPoint);
        tolua_function(tolua_S,"setCategoryBits",lua_ax_physics2d_Collider2D_setCategoryBits);
        tolua_function(tolua_S,"getCategoryBits",lua_ax_physics2d_Collider2D_getCategoryBits);
        tolua_function(tolua_S,"setMaskBits",lua_ax_physics2d_Collider2D_setMaskBits);
        tolua_function(tolua_S,"getMaskBits",lua_ax_physics2d_Collider2D_getMaskBits);
        tolua_function(tolua_S,"setGroup",lua_ax_physics2d_Collider2D_setGroup);
        tolua_function(tolua_S,"getGroup",lua_ax_physics2d_Collider2D_getGroup);
        tolua_function(tolua_S,"deatchFromBody",lua_ax_physics2d_Collider2D_deatchFromBody);
        tolua_function(tolua_S,"isAttached",lua_ax_physics2d_Collider2D_isAttached);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Collider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Collider2D";
    g_typeCast[typeName] = "ax.Collider2D";
    return 1;
}

int lua_ax_physics2d_CircleCollider2D_getRadius(lua_State* tolua_S)
{
    int argc = 0;
    ax::CircleCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CircleCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::CircleCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_CircleCollider2D_getRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_CircleCollider2D_getRadius'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRadius();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CircleCollider2D:getRadius",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_CircleCollider2D_getRadius'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_CircleCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CircleCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CircleCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_CircleCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CircleCollider2D::create(arg0);
        object_to_luaval<ax::CircleCollider2D>(tolua_S, "ax.CircleCollider2D",(ax::CircleCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CircleCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.CircleCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_CircleCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CircleCollider2D::create(arg0, arg1);
        object_to_luaval<ax::CircleCollider2D>(tolua_S, "ax.CircleCollider2D",(ax::CircleCollider2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.CircleCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.CircleCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.CircleCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_CircleCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::CircleCollider2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::CircleCollider2D>(tolua_S, "ax.CircleCollider2D",(ax::CircleCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.CircleCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_CircleCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_CircleCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (CircleCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_CircleCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.CircleCollider2D");
    tolua_cclass(tolua_S,"CircleCollider2D","ax.CircleCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"CircleCollider2D");
        tolua_function(tolua_S,"getRadius",lua_ax_physics2d_CircleCollider2D_getRadius);
        tolua_function(tolua_S,"create", lua_ax_physics2d_CircleCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::CircleCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.CircleCollider2D";
    g_typeCast[typeName] = "ax.CircleCollider2D";
    return 1;
}

int lua_ax_physics2d_PolygonCollider2D_getPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PolygonCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PolygonCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PolygonCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PolygonCollider2D_getPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PolygonCollider2D:getPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_getPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PolygonCollider2D:getPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PolygonCollider2D_getPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PolygonCollider2D_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::PolygonCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PolygonCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PolygonCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PolygonCollider2D_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PolygonCollider2D:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PolygonCollider2D_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PolygonCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PolygonCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.PolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PolygonCollider2D::create(arg0);
        object_to_luaval<ax::PolygonCollider2D>(tolua_S, "ax.PolygonCollider2D",(ax::PolygonCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.PolygonCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.PolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PolygonCollider2D::create(arg0, arg1);
        object_to_luaval<ax::PolygonCollider2D>(tolua_S, "ax.PolygonCollider2D",(ax::PolygonCollider2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.PolygonCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.PolygonCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PolygonCollider2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::PolygonCollider2D>(tolua_S, "ax.PolygonCollider2D",(ax::PolygonCollider2D*)ret);
        return 1;
    }
    if (argc == 4)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        double arg3;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.PolygonCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.PolygonCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PolygonCollider2D:create");
        ok &= luaval_to_number(tolua_S, 5, &arg3, "ax.PolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PolygonCollider2D::create(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PolygonCollider2D>(tolua_S, "ax.PolygonCollider2D",(ax::PolygonCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PolygonCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PolygonCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_PolygonCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PolygonCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_PolygonCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PolygonCollider2D");
    tolua_cclass(tolua_S,"PolygonCollider2D","ax.PolygonCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"PolygonCollider2D");
        tolua_function(tolua_S,"getPoint",lua_ax_physics2d_PolygonCollider2D_getPoint);
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics2d_PolygonCollider2D_getPointsCount);
        tolua_function(tolua_S,"create", lua_ax_physics2d_PolygonCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PolygonCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PolygonCollider2D";
    g_typeCast[typeName] = "ax.PolygonCollider2D";
    return 1;
}

int lua_ax_physics2d_BoxCollider2D_getSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::BoxCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.BoxCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::BoxCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_BoxCollider2D_getSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_BoxCollider2D_getSize'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSize();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.BoxCollider2D:getSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_BoxCollider2D_getSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_BoxCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.BoxCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.BoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_BoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider2D::create(arg0);
        object_to_luaval<ax::BoxCollider2D>(tolua_S, "ax.BoxCollider2D",(ax::BoxCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.BoxCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.BoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_BoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider2D::create(arg0, arg1);
        object_to_luaval<ax::BoxCollider2D>(tolua_S, "ax.BoxCollider2D",(ax::BoxCollider2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.BoxCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.BoxCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.BoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_BoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::BoxCollider2D>(tolua_S, "ax.BoxCollider2D",(ax::BoxCollider2D*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        double arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.BoxCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.BoxCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.BoxCollider2D:create");
        ok &= luaval_to_number(tolua_S, 5, &arg3, "ax.BoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_BoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::BoxCollider2D::create(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::BoxCollider2D>(tolua_S, "ax.BoxCollider2D",(ax::BoxCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.BoxCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_BoxCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_BoxCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BoxCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_BoxCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.BoxCollider2D");
    tolua_cclass(tolua_S,"BoxCollider2D","ax.BoxCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"BoxCollider2D");
        tolua_function(tolua_S,"getSize",lua_ax_physics2d_BoxCollider2D_getSize);
        tolua_function(tolua_S,"create", lua_ax_physics2d_BoxCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::BoxCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.BoxCollider2D";
    g_typeCast[typeName] = "ax.BoxCollider2D";
    return 1;
}

int lua_ax_physics2d_EdgeSegmentCollider2D_getPointA(lua_State* tolua_S)
{
    int argc = 0;
    ax::EdgeSegmentCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EdgeSegmentCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::EdgeSegmentCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointA'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointA();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EdgeSegmentCollider2D:getPointA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_EdgeSegmentCollider2D_getPointB(lua_State* tolua_S)
{
    int argc = 0;
    ax::EdgeSegmentCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EdgeSegmentCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::EdgeSegmentCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointB'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointB();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EdgeSegmentCollider2D:getPointB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeSegmentCollider2D_getPointB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_EdgeSegmentCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EdgeSegmentCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.EdgeSegmentCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.EdgeSegmentCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeSegmentCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeSegmentCollider2D::create(arg0, arg1);
        object_to_luaval<ax::EdgeSegmentCollider2D>(tolua_S, "ax.EdgeSegmentCollider2D",(ax::EdgeSegmentCollider2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial2D arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.EdgeSegmentCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.EdgeSegmentCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 4, &arg2, "ax.EdgeSegmentCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeSegmentCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeSegmentCollider2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::EdgeSegmentCollider2D>(tolua_S, "ax.EdgeSegmentCollider2D",(ax::EdgeSegmentCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EdgeSegmentCollider2D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeSegmentCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_EdgeSegmentCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EdgeSegmentCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_EdgeSegmentCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EdgeSegmentCollider2D");
    tolua_cclass(tolua_S,"EdgeSegmentCollider2D","ax.EdgeSegmentCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"EdgeSegmentCollider2D");
        tolua_function(tolua_S,"getPointA",lua_ax_physics2d_EdgeSegmentCollider2D_getPointA);
        tolua_function(tolua_S,"getPointB",lua_ax_physics2d_EdgeSegmentCollider2D_getPointB);
        tolua_function(tolua_S,"create", lua_ax_physics2d_EdgeSegmentCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EdgeSegmentCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EdgeSegmentCollider2D";
    g_typeCast[typeName] = "ax.EdgeSegmentCollider2D";
    return 1;
}

int lua_ax_physics2d_EdgePolygonCollider2D_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::EdgePolygonCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EdgePolygonCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::EdgePolygonCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_EdgePolygonCollider2D_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgePolygonCollider2D_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EdgePolygonCollider2D:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgePolygonCollider2D_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_EdgePolygonCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EdgePolygonCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.EdgePolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgePolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgePolygonCollider2D::create(arg0);
        object_to_luaval<ax::EdgePolygonCollider2D>(tolua_S, "ax.EdgePolygonCollider2D",(ax::EdgePolygonCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.EdgePolygonCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.EdgePolygonCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgePolygonCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgePolygonCollider2D::create(arg0, arg1);
        object_to_luaval<ax::EdgePolygonCollider2D>(tolua_S, "ax.EdgePolygonCollider2D",(ax::EdgePolygonCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EdgePolygonCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgePolygonCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_EdgePolygonCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EdgePolygonCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_EdgePolygonCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EdgePolygonCollider2D");
    tolua_cclass(tolua_S,"EdgePolygonCollider2D","ax.EdgePolygonCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"EdgePolygonCollider2D");
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics2d_EdgePolygonCollider2D_getPointsCount);
        tolua_function(tolua_S,"create", lua_ax_physics2d_EdgePolygonCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EdgePolygonCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EdgePolygonCollider2D";
    g_typeCast[typeName] = "ax.EdgePolygonCollider2D";
    return 1;
}

int lua_ax_physics2d_EdgeBoxCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EdgeBoxCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.EdgeBoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeBoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeBoxCollider2D::create(arg0);
        object_to_luaval<ax::EdgeBoxCollider2D>(tolua_S, "ax.EdgeBoxCollider2D",(ax::EdgeBoxCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.EdgeBoxCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.EdgeBoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeBoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeBoxCollider2D::create(arg0, arg1);
        object_to_luaval<ax::EdgeBoxCollider2D>(tolua_S, "ax.EdgeBoxCollider2D",(ax::EdgeBoxCollider2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.EdgeBoxCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.EdgeBoxCollider2D:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.EdgeBoxCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeBoxCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeBoxCollider2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::EdgeBoxCollider2D>(tolua_S, "ax.EdgeBoxCollider2D",(ax::EdgeBoxCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EdgeBoxCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeBoxCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_EdgeBoxCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EdgeBoxCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_EdgeBoxCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EdgeBoxCollider2D");
    tolua_cclass(tolua_S,"EdgeBoxCollider2D","ax.EdgeBoxCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"EdgeBoxCollider2D");
        tolua_function(tolua_S,"create", lua_ax_physics2d_EdgeBoxCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EdgeBoxCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EdgeBoxCollider2D";
    g_typeCast[typeName] = "ax.EdgeBoxCollider2D";
    return 1;
}

int lua_ax_physics2d_EdgeChainCollider2D_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::EdgeChainCollider2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EdgeChainCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::EdgeChainCollider2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_EdgeChainCollider2D_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeChainCollider2D_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EdgeChainCollider2D:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeChainCollider2D_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_EdgeChainCollider2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EdgeChainCollider2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.EdgeChainCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeChainCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeChainCollider2D::create(arg0);
        object_to_luaval<ax::EdgeChainCollider2D>(tolua_S, "ax.EdgeChainCollider2D",(ax::EdgeChainCollider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.EdgeChainCollider2D:create");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.EdgeChainCollider2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_EdgeChainCollider2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EdgeChainCollider2D::create(arg0, arg1);
        object_to_luaval<ax::EdgeChainCollider2D>(tolua_S, "ax.EdgeChainCollider2D",(ax::EdgeChainCollider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EdgeChainCollider2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_EdgeChainCollider2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_EdgeChainCollider2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EdgeChainCollider2D)");
    return 0;
}

int lua_register_ax_physics2d_EdgeChainCollider2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EdgeChainCollider2D");
    tolua_cclass(tolua_S,"EdgeChainCollider2D","ax.EdgeChainCollider2D","ax.Collider2D",nullptr);

    tolua_beginmodule(tolua_S,"EdgeChainCollider2D");
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics2d_EdgeChainCollider2D_getPointsCount);
        tolua_function(tolua_S,"create", lua_ax_physics2d_EdgeChainCollider2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EdgeChainCollider2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EdgeChainCollider2D";
    g_typeCast[typeName] = "ax.EdgeChainCollider2D";
    return 1;
}

int lua_ax_physics2d_Rigidbody2D_addCollider(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_addCollider'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Collider2D* arg0;

        ok &= luaval_to_object<ax::Collider2D>(tolua_S, 2, "ax.Collider2D",&arg0, "ax.Rigidbody2D:addCollider");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_addCollider'", nullptr);
            return 0;
        }
        auto&& ret = obj->addCollider(arg0);
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Collider2D* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::Collider2D>(tolua_S, 2, "ax.Collider2D",&arg0, "ax.Rigidbody2D:addCollider");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody2D:addCollider");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_addCollider'", nullptr);
            return 0;
        }
        auto&& ret = obj->addCollider(arg0, arg1);
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:addCollider",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_addCollider'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_removeCollider(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_removeCollider'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            obj->removeCollider(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            obj->removeCollider(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            ax::Collider2D* arg0;
            ok &= luaval_to_object<ax::Collider2D>(tolua_S, 2, "ax.Collider2D",&arg0, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            obj->removeCollider(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            ax::Collider2D* arg0;
            ok &= luaval_to_object<ax::Collider2D>(tolua_S, 2, "ax.Collider2D",&arg0, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody2D:removeCollider");

            if (!ok) { break; }
            obj->removeCollider(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.Rigidbody2D:removeCollider",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_removeCollider'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_removeAllColliders(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_removeAllColliders'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_removeAllColliders'", nullptr);
            return 0;
        }
        obj->removeAllColliders();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody2D:removeAllColliders");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_removeAllColliders'", nullptr);
            return 0;
        }
        obj->removeAllColliders(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:removeAllColliders",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_removeAllColliders'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getColliders(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getColliders'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getColliders'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColliders();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getColliders",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getColliders'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getFirstCollider(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getFirstCollider'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getFirstCollider'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFirstCollider();
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getFirstCollider",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getFirstCollider'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getCollider(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getCollider'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:getCollider");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getCollider'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCollider(arg0);
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getCollider",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getCollider'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_applyForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_applyForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_applyForce'", nullptr);
            return 0;
        }
        obj->applyForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:applyForce");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.Rigidbody2D:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_applyForce'", nullptr);
            return 0;
        }
        obj->applyForce(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:applyForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_applyForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_resetForces(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_resetForces'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_resetForces'", nullptr);
            return 0;
        }
        obj->resetForces();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:resetForces",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_resetForces'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_applyImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_applyImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_applyImpulse'", nullptr);
            return 0;
        }
        obj->applyImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:applyImpulse");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.Rigidbody2D:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_applyImpulse'", nullptr);
            return 0;
        }
        obj->applyImpulse(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:applyImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_applyImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_applyTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_applyTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:applyTorque");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_applyTorque'", nullptr);
            return 0;
        }
        obj->applyTorque(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:applyTorque",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_applyTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:setVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setVelocity'", nullptr);
            return 0;
        }
        obj->setVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setAngularVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocity'", nullptr);
            return 0;
        }
        obj->setAngularVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setAngularVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getVelocityAtLocalPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtLocalPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:getVelocityAtLocalPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtLocalPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocityAtLocalPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getVelocityAtLocalPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtLocalPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getVelocityAtWorldPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtWorldPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:getVelocityAtWorldPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtWorldPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocityAtWorldPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getVelocityAtWorldPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getVelocityAtWorldPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getAngularVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setVelocityLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setVelocityLimit'", nullptr);
            return 0;
        }
        obj->setVelocityLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setVelocityLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getVelocityLimit'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVelocityLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getVelocityLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setAngularVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setAngularVelocityLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocityLimit'", nullptr);
            return 0;
        }
        obj->setAngularVelocityLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setAngularVelocityLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setAngularVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getAngularVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocityLimit'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularVelocityLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getAngularVelocityLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getAngularVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_removeFromWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_removeFromWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_removeFromWorld'", nullptr);
            return 0;
        }
        obj->removeFromWorld();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:removeFromWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_removeFromWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorld();
        object_to_luaval<ax::PhysicsWorld2D>(tolua_S, "ax.PhysicsWorld2D",(ax::PhysicsWorld2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ContactEventBits arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setEventEnabled");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.Rigidbody2D:setEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setEventEnabled'", nullptr);
            return 0;
        }
        obj->setEventEnabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setEventEnabled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getCategoryBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getCategoryBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getCategoryBits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCategoryBits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getCategoryBits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getCategoryBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setCategoryBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setCategoryBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned long long arg0;

        ok &= luaval_to_long_long(tolua_S, 2, (long long*)&arg0, "ax.Rigidbody2D:setCategoryBits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setCategoryBits'", nullptr);
            return 0;
        }
        obj->setCategoryBits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setCategoryBits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setCategoryBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getCollisionMaskBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getCollisionMaskBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getCollisionMaskBits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCollisionMaskBits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getCollisionMaskBits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getCollisionMaskBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setCollisionMaskBits(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setCollisionMaskBits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setCollisionMaskBits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setCollisionMaskBits'", nullptr);
            return 0;
        }
        obj->setCollisionMaskBits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setCollisionMaskBits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setCollisionMaskBits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setGroup'", nullptr);
            return 0;
        }
        obj->setGroup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setGroup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getGroup'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGroup();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRotation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setPositionOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setPositionOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:setPositionOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setPositionOffset'", nullptr);
            return 0;
        }
        obj->setPositionOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setPositionOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setPositionOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getPositionOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getPositionOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getPositionOffset'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPositionOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getPositionOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getPositionOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setRotationOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setRotationOffset'", nullptr);
            return 0;
        }
        obj->setRotationOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setRotationOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getRotationOffset'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRotationOffset();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getRotationOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setDynamic'", nullptr);
            return 0;
        }
        obj->setDynamic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setDynamic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isDynamic'", nullptr);
            return 0;
        }
        auto&& ret = obj->isDynamic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isDynamic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setKinematic'", nullptr);
            return 0;
        }
        obj->setKinematic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setKinematic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isKinematic'", nullptr);
            return 0;
        }
        auto&& ret = obj->isKinematic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isKinematic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getBodyType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getBodyType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getBodyType'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBodyType();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getBodyType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getBodyType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setBodyType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setBodyType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody2D::BodyType arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setBodyType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setBodyType'", nullptr);
            return 0;
        }
        obj->setBodyType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setBodyType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setBodyType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isAutoMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isAutoMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isAutoMass'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAutoMass();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isAutoMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isAutoMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setAutoMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setAutoMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody2D:setAutoMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setAutoMass'", nullptr);
            return 0;
        }
        obj->setAutoMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setAutoMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setAutoMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getMass'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMass();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setMass'", nullptr);
            return 0;
        }
        obj->setMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getMoment'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMoment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getMoment",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setMoment'", nullptr);
            return 0;
        }
        obj->setMoment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setMoment",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getLinearDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getLinearDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setLinearDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setLinearDamping'", nullptr);
            return 0;
        }
        obj->setLinearDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setLinearDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getAngularDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getAngularDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setAngularDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setAngularDamping'", nullptr);
            return 0;
        }
        obj->setAngularDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setAngularDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isAttached'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getCollisionDetectionMode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getCollisionDetectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getCollisionDetectionMode'", nullptr);
            return 0;
        }
        int ret = (int)obj->getCollisionDetectionMode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getCollisionDetectionMode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getCollisionDetectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setCollisionDetectionMode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setCollisionDetectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody2D::CollisionDetectionMode arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setCollisionDetectionMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setCollisionDetectionMode'", nullptr);
            return 0;
        }
        obj->setCollisionDetectionMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setCollisionDetectionMode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setCollisionDetectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setSleeping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setSleeping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody2D:setSleeping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setSleeping'", nullptr);
            return 0;
        }
        obj->setSleeping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setSleeping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setSleeping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isSleeping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isSleeping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isSleeping'", nullptr);
            return 0;
        }
        auto&& ret = obj->isSleeping();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isSleeping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isSleeping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isRotationEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isRotationEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isRotationEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isRotationEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isRotationEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isRotationEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setRotationEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setRotationEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody2D:setRotationEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setRotationEnabled'", nullptr);
            return 0;
        }
        obj->setRotationEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setRotationEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setRotationEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_isGravityEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_isGravityEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_isGravityEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isGravityEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:isGravityEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_isGravityEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setGravityEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setGravityEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Rigidbody2D:setGravityEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setGravityEnabled'", nullptr);
            return 0;
        }
        obj->setGravityEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setGravityEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setGravityEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setGravityScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setGravityScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:setGravityScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setGravityScale'", nullptr);
            return 0;
        }
        obj->setGravityScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setGravityScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setGravityScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getGravityScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getGravityScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getGravityScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGravityScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getGravityScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getGravityScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Rigidbody2D:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_setTag'", nullptr);
            return 0;
        }
        obj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_world2Local(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_world2Local'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:world2Local");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_world2Local'", nullptr);
            return 0;
        }
        auto&& ret = obj->world2Local(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:world2Local",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_world2Local'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_local2World(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_local2World'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:local2World");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_local2World'", nullptr);
            return 0;
        }
        auto&& ret = obj->local2World(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:local2World",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_local2World'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_getAngleRadians(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_getAngleRadians'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_getAngleRadians'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngleRadians();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:getAngleRadians",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_getAngleRadians'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_internalHandle(lua_State* tolua_S)
{
    int argc = 0;
    ax::Rigidbody2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Rigidbody2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Rigidbody2D_internalHandle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_internalHandle'", nullptr);
            return 0;
        }
        auto&& ret = obj->internalHandle();
        #pragma warning NO CONVERSION FROM NATIVE FOR b2BodyId;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Rigidbody2D:internalHandle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_internalHandle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Rigidbody2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::create();
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createCircle(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createCircle(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:createCircle");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createCircle(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Rigidbody2D:createCircle");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createCircle");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.Rigidbody2D:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createCircle(arg0, arg1, arg2);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createCircle",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createCircle'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createBox(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createBox(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createBox");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createBox(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createBox");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createBox");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.Rigidbody2D:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createBox(arg0, arg1, arg2);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createBox",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createBox'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createPolygon(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createPolygon'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createPolygon(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createPolygon");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createPolygon'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createPolygon(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createPolygon");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createPolygon");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.Rigidbody2D:createPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createPolygon'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createPolygon(arg0, arg1, arg2);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createPolygon",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createPolygon'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createEdgeSegment(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeSegment");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgeSegment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeSegment'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeSegment(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial2D arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeSegment");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgeSegment");
        ok &= luaval_to_physics_material2d(tolua_S, 4, &arg2, "ax.Rigidbody2D:createEdgeSegment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeSegment'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeSegment(arg0, arg1, arg2);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createEdgeSegment",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createEdgeSegment'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createEdgeBox(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeBox(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeBox");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeBox(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial2D arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeBox");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgeBox");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.Rigidbody2D:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeBox(arg0, arg1, arg2);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createEdgeBox",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createEdgeBox'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createEdgePolygon(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgePolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgePolygon'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgePolygon(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgePolygon");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgePolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgePolygon'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgePolygon(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createEdgePolygon",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createEdgePolygon'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics2d_Rigidbody2D_createEdgeChain(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Rigidbody2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::vector<ax::Vec2> arg0;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeChain");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeChain'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeChain(arg0);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<ax::Vec2> arg0;
        ax::PhysicsMaterial2D arg1;
        ok &= luaval_to_std_vector_float2(tolua_S, 2, &arg0, "ax.Rigidbody2D:createEdgeChain");
        ok &= luaval_to_physics_material2d(tolua_S, 3, &arg1, "ax.Rigidbody2D:createEdgeChain");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Rigidbody2D_createEdgeChain'", nullptr);
            return 0;
        }
        auto&& ret = ax::Rigidbody2D::createEdgeChain(arg0, arg1);
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Rigidbody2D:createEdgeChain",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Rigidbody2D_createEdgeChain'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_Rigidbody2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Rigidbody2D)");
    return 0;
}

int lua_register_ax_physics2d_Rigidbody2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Rigidbody2D");
    tolua_cclass(tolua_S,"Rigidbody2D","ax.Rigidbody2D","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"Rigidbody2D");
        tolua_function(tolua_S,"addCollider",lua_ax_physics2d_Rigidbody2D_addCollider);
        tolua_function(tolua_S,"removeCollider",lua_ax_physics2d_Rigidbody2D_removeCollider);
        tolua_function(tolua_S,"removeAllColliders",lua_ax_physics2d_Rigidbody2D_removeAllColliders);
        tolua_function(tolua_S,"getColliders",lua_ax_physics2d_Rigidbody2D_getColliders);
        tolua_function(tolua_S,"getFirstCollider",lua_ax_physics2d_Rigidbody2D_getFirstCollider);
        tolua_function(tolua_S,"getCollider",lua_ax_physics2d_Rigidbody2D_getCollider);
        tolua_function(tolua_S,"applyForce",lua_ax_physics2d_Rigidbody2D_applyForce);
        tolua_function(tolua_S,"resetForces",lua_ax_physics2d_Rigidbody2D_resetForces);
        tolua_function(tolua_S,"applyImpulse",lua_ax_physics2d_Rigidbody2D_applyImpulse);
        tolua_function(tolua_S,"applyTorque",lua_ax_physics2d_Rigidbody2D_applyTorque);
        tolua_function(tolua_S,"setVelocity",lua_ax_physics2d_Rigidbody2D_setVelocity);
        tolua_function(tolua_S,"getVelocity",lua_ax_physics2d_Rigidbody2D_getVelocity);
        tolua_function(tolua_S,"setAngularVelocity",lua_ax_physics2d_Rigidbody2D_setAngularVelocity);
        tolua_function(tolua_S,"getVelocityAtLocalPoint",lua_ax_physics2d_Rigidbody2D_getVelocityAtLocalPoint);
        tolua_function(tolua_S,"getVelocityAtWorldPoint",lua_ax_physics2d_Rigidbody2D_getVelocityAtWorldPoint);
        tolua_function(tolua_S,"getAngularVelocity",lua_ax_physics2d_Rigidbody2D_getAngularVelocity);
        tolua_function(tolua_S,"setVelocityLimit",lua_ax_physics2d_Rigidbody2D_setVelocityLimit);
        tolua_function(tolua_S,"getVelocityLimit",lua_ax_physics2d_Rigidbody2D_getVelocityLimit);
        tolua_function(tolua_S,"setAngularVelocityLimit",lua_ax_physics2d_Rigidbody2D_setAngularVelocityLimit);
        tolua_function(tolua_S,"getAngularVelocityLimit",lua_ax_physics2d_Rigidbody2D_getAngularVelocityLimit);
        tolua_function(tolua_S,"removeFromWorld",lua_ax_physics2d_Rigidbody2D_removeFromWorld);
        tolua_function(tolua_S,"getWorld",lua_ax_physics2d_Rigidbody2D_getWorld);
        tolua_function(tolua_S,"getNode",lua_ax_physics2d_Rigidbody2D_getNode);
        tolua_function(tolua_S,"setEventEnabled",lua_ax_physics2d_Rigidbody2D_setEventEnabled);
        tolua_function(tolua_S,"getCategoryBits",lua_ax_physics2d_Rigidbody2D_getCategoryBits);
        tolua_function(tolua_S,"setCategoryBits",lua_ax_physics2d_Rigidbody2D_setCategoryBits);
        tolua_function(tolua_S,"getCollisionMaskBits",lua_ax_physics2d_Rigidbody2D_getCollisionMaskBits);
        tolua_function(tolua_S,"setCollisionMaskBits",lua_ax_physics2d_Rigidbody2D_setCollisionMaskBits);
        tolua_function(tolua_S,"setGroup",lua_ax_physics2d_Rigidbody2D_setGroup);
        tolua_function(tolua_S,"getGroup",lua_ax_physics2d_Rigidbody2D_getGroup);
        tolua_function(tolua_S,"getPosition",lua_ax_physics2d_Rigidbody2D_getPosition);
        tolua_function(tolua_S,"getRotation",lua_ax_physics2d_Rigidbody2D_getRotation);
        tolua_function(tolua_S,"setPositionOffset",lua_ax_physics2d_Rigidbody2D_setPositionOffset);
        tolua_function(tolua_S,"getPositionOffset",lua_ax_physics2d_Rigidbody2D_getPositionOffset);
        tolua_function(tolua_S,"setRotationOffset",lua_ax_physics2d_Rigidbody2D_setRotationOffset);
        tolua_function(tolua_S,"getRotationOffset",lua_ax_physics2d_Rigidbody2D_getRotationOffset);
        tolua_function(tolua_S,"setDynamic",lua_ax_physics2d_Rigidbody2D_setDynamic);
        tolua_function(tolua_S,"isDynamic",lua_ax_physics2d_Rigidbody2D_isDynamic);
        tolua_function(tolua_S,"setKinematic",lua_ax_physics2d_Rigidbody2D_setKinematic);
        tolua_function(tolua_S,"isKinematic",lua_ax_physics2d_Rigidbody2D_isKinematic);
        tolua_function(tolua_S,"getBodyType",lua_ax_physics2d_Rigidbody2D_getBodyType);
        tolua_function(tolua_S,"setBodyType",lua_ax_physics2d_Rigidbody2D_setBodyType);
        tolua_function(tolua_S,"isAutoMass",lua_ax_physics2d_Rigidbody2D_isAutoMass);
        tolua_function(tolua_S,"setAutoMass",lua_ax_physics2d_Rigidbody2D_setAutoMass);
        tolua_function(tolua_S,"getMass",lua_ax_physics2d_Rigidbody2D_getMass);
        tolua_function(tolua_S,"setMass",lua_ax_physics2d_Rigidbody2D_setMass);
        tolua_function(tolua_S,"getMoment",lua_ax_physics2d_Rigidbody2D_getMoment);
        tolua_function(tolua_S,"setMoment",lua_ax_physics2d_Rigidbody2D_setMoment);
        tolua_function(tolua_S,"getLinearDamping",lua_ax_physics2d_Rigidbody2D_getLinearDamping);
        tolua_function(tolua_S,"setLinearDamping",lua_ax_physics2d_Rigidbody2D_setLinearDamping);
        tolua_function(tolua_S,"getAngularDamping",lua_ax_physics2d_Rigidbody2D_getAngularDamping);
        tolua_function(tolua_S,"setAngularDamping",lua_ax_physics2d_Rigidbody2D_setAngularDamping);
        tolua_function(tolua_S,"isAttached",lua_ax_physics2d_Rigidbody2D_isAttached);
        tolua_function(tolua_S,"getCollisionDetectionMode",lua_ax_physics2d_Rigidbody2D_getCollisionDetectionMode);
        tolua_function(tolua_S,"setCollisionDetectionMode",lua_ax_physics2d_Rigidbody2D_setCollisionDetectionMode);
        tolua_function(tolua_S,"setSleeping",lua_ax_physics2d_Rigidbody2D_setSleeping);
        tolua_function(tolua_S,"isSleeping",lua_ax_physics2d_Rigidbody2D_isSleeping);
        tolua_function(tolua_S,"isRotationEnabled",lua_ax_physics2d_Rigidbody2D_isRotationEnabled);
        tolua_function(tolua_S,"setRotationEnabled",lua_ax_physics2d_Rigidbody2D_setRotationEnabled);
        tolua_function(tolua_S,"isGravityEnabled",lua_ax_physics2d_Rigidbody2D_isGravityEnabled);
        tolua_function(tolua_S,"setGravityEnabled",lua_ax_physics2d_Rigidbody2D_setGravityEnabled);
        tolua_function(tolua_S,"setGravityScale",lua_ax_physics2d_Rigidbody2D_setGravityScale);
        tolua_function(tolua_S,"getGravityScale",lua_ax_physics2d_Rigidbody2D_getGravityScale);
        tolua_function(tolua_S,"setTag",lua_ax_physics2d_Rigidbody2D_setTag);
        tolua_function(tolua_S,"getTag",lua_ax_physics2d_Rigidbody2D_getTag);
        tolua_function(tolua_S,"world2Local",lua_ax_physics2d_Rigidbody2D_world2Local);
        tolua_function(tolua_S,"local2World",lua_ax_physics2d_Rigidbody2D_local2World);
        tolua_function(tolua_S,"getAngleRadians",lua_ax_physics2d_Rigidbody2D_getAngleRadians);
        tolua_function(tolua_S,"internalHandle",lua_ax_physics2d_Rigidbody2D_internalHandle);
        tolua_function(tolua_S,"create", lua_ax_physics2d_Rigidbody2D_create);
        tolua_function(tolua_S,"createCircle", lua_ax_physics2d_Rigidbody2D_createCircle);
        tolua_function(tolua_S,"createBox", lua_ax_physics2d_Rigidbody2D_createBox);
        tolua_function(tolua_S,"createPolygon", lua_ax_physics2d_Rigidbody2D_createPolygon);
        tolua_function(tolua_S,"createEdgeSegment", lua_ax_physics2d_Rigidbody2D_createEdgeSegment);
        tolua_function(tolua_S,"createEdgeBox", lua_ax_physics2d_Rigidbody2D_createEdgeBox);
        tolua_function(tolua_S,"createEdgePolygon", lua_ax_physics2d_Rigidbody2D_createEdgePolygon);
        tolua_function(tolua_S,"createEdgeChain", lua_ax_physics2d_Rigidbody2D_createEdgeChain);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Rigidbody2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Rigidbody2D";
    g_typeCast[typeName] = "ax.Rigidbody2D";
    return 1;
}

int lua_ax_physics2d_ContactEvent2D_getColliderA(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_ContactEvent2D_getColliderA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_ContactEvent2D_getColliderA'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColliderA();
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent2D:getColliderA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_ContactEvent2D_getColliderA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_ContactEvent2D_getColliderB(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_ContactEvent2D_getColliderB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_ContactEvent2D_getColliderB'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColliderB();
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent2D:getColliderB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_ContactEvent2D_getColliderB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_ContactEvent2D_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_ContactEvent2D_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_ContactEvent2D_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)obj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent2D:getEventCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_ContactEvent2D_getEventCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_ContactEvent2D_getContactInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ContactEvent2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ContactEvent2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ContactEvent2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_ContactEvent2D_getContactInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_ContactEvent2D_getContactInfo'", nullptr);
            return 0;
        }
        auto&& ret = obj->getContactInfo();
        physics_contactinfo2d_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ContactEvent2D:getContactInfo",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_ContactEvent2D_getContactInfo'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics2d_ContactEvent2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ContactEvent2D)");
    return 0;
}

int lua_register_ax_physics2d_ContactEvent2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ContactEvent2D");
    tolua_cclass(tolua_S,"ContactEvent2D","ax.ContactEvent2D","ax.CustomEvent",nullptr);

    tolua_beginmodule(tolua_S,"ContactEvent2D");
        tolua_function(tolua_S,"getColliderA",lua_ax_physics2d_ContactEvent2D_getColliderA);
        tolua_function(tolua_S,"getColliderB",lua_ax_physics2d_ContactEvent2D_getColliderB);
        tolua_function(tolua_S,"getEventCode",lua_ax_physics2d_ContactEvent2D_getEventCode);
        tolua_function(tolua_S,"getContactInfo",lua_ax_physics2d_ContactEvent2D_getContactInfo);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ContactEvent2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ContactEvent2D";
    g_typeCast[typeName] = "ax.ContactEvent2D";
    return 1;
}

int lua_ax_physics2d_ContactEventListener2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ContactEventListener2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 2)
        {
            ax::Rigidbody2D* arg0;
            ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.ContactEventListener2D:create");
            if (!ok) { break; }
            ax::Rigidbody2D* arg1;
            ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 3, "ax.Rigidbody2D",&arg1, "ax.ContactEventListener2D:create");
            if (!ok) { break; }
            ax::ContactEventListener2D* ret = ax::ContactEventListener2D::create(arg0, arg1);
            object_to_luaval<ax::ContactEventListener2D>(tolua_S, "ax.ContactEventListener2D",(ax::ContactEventListener2D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ContactEventListener2D* ret = ax::ContactEventListener2D::create();
            object_to_luaval<ax::ContactEventListener2D>(tolua_S, "ax.ContactEventListener2D",(ax::ContactEventListener2D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.ContactEventListener2D:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_ContactEventListener2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_ContactEventListener2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ContactEventListener2D)");
    return 0;
}

int lua_register_ax_physics2d_ContactEventListener2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ContactEventListener2D");
    tolua_cclass(tolua_S,"ContactEventListener2D","ax.ContactEventListener2D","ax.CustomEventListener",nullptr);

    tolua_beginmodule(tolua_S,"ContactEventListener2D");
        tolua_function(tolua_S,"create", lua_ax_physics2d_ContactEventListener2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ContactEventListener2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ContactEventListener2D";
    g_typeCast[typeName] = "ax.ContactEventListener2D";
    return 1;
}

int lua_ax_physics2d_Joint2D_isAttached(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_isAttached'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_isAttached'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAttached();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:isAttached",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_isAttached'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorld();
        object_to_luaval<ax::PhysicsWorld2D>(tolua_S, "ax.PhysicsWorld2D",(ax::PhysicsWorld2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getAttachedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getAttachedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getAttachedBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAttachedBody();
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getAttachedBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getAttachedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setConnectedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setConnectedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;

        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.Joint2D:setConnectedBody");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setConnectedBody'", nullptr);
            return 0;
        }
        obj->setConnectedBody(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setConnectedBody",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setConnectedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getConnectedBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getConnectedBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getConnectedBody'", nullptr);
            return 0;
        }
        auto&& ret = obj->getConnectedBody();
        object_to_luaval<ax::Rigidbody2D>(tolua_S, "ax.Rigidbody2D",(ax::Rigidbody2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getConnectedBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getConnectedBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getAnchorSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getAnchorSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getAnchorSpace'", nullptr);
            return 0;
        }
        int ret = (int)obj->getAnchorSpace();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getAnchorSpace",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getAnchorSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setAnchorSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setAnchorSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Joint2D::AnchorSpace arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Joint2D:setAnchorSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setAnchorSpace'", nullptr);
            return 0;
        }
        obj->setAnchorSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setAnchorSpace",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setAnchorSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setAnchorsInWorldSpace(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setAnchorsInWorldSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Joint2D:setAnchorsInWorldSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setAnchorsInWorldSpace'", nullptr);
            return 0;
        }
        obj->setAnchorsInWorldSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setAnchorsInWorldSpace",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setAnchorsInWorldSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getAnchor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnchor();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setAnchors(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setAnchors'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Joint2D:setAnchors");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.Joint2D:setAnchors");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setAnchors'", nullptr);
            return 0;
        }
        obj->setAnchors(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setAnchors",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setAnchors'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Joint2D:setAnchor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setAnchor'", nullptr);
            return 0;
        }
        obj->setAnchor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setAnchor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getConnectedAnchor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getConnectedAnchor();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getConnectedAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.Joint2D:setConnectedAnchor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setConnectedAnchor'", nullptr);
            return 0;
        }
        obj->setConnectedAnchor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setConnectedAnchor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_isCollisionEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_isCollisionEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_isCollisionEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isCollisionEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:isCollisionEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_isCollisionEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setCollisionEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setCollisionEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Joint2D:setCollisionEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setCollisionEnabled'", nullptr);
            return 0;
        }
        obj->setCollisionEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setCollisionEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setCollisionEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setAutoConfigureConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setAutoConfigureConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.Joint2D:setAutoConfigureConnectedAnchor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setAutoConfigureConnectedAnchor'", nullptr);
            return 0;
        }
        obj->setAutoConfigureConnectedAnchor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setAutoConfigureConnectedAnchor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setAutoConfigureConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_isAutoConfigureConnectedAnchor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_isAutoConfigureConnectedAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_isAutoConfigureConnectedAnchor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAutoConfigureConnectedAnchor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:isAutoConfigureConnectedAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_isAutoConfigureConnectedAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setDrawScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setDrawScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.Joint2D:setDrawScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setDrawScale'", nullptr);
            return 0;
        }
        obj->setDrawScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setDrawScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setDrawScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_getTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_Joint2D_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Joint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Joint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::Joint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_Joint2D_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.Joint2D:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_Joint2D_setTag'", nullptr);
            return 0;
        }
        obj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Joint2D:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_Joint2D_setTag'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics2d_Joint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Joint2D)");
    return 0;
}

int lua_register_ax_physics2d_Joint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Joint2D");
    tolua_cclass(tolua_S,"Joint2D","ax.Joint2D","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"Joint2D");
        tolua_function(tolua_S,"isAttached",lua_ax_physics2d_Joint2D_isAttached);
        tolua_function(tolua_S,"getWorld",lua_ax_physics2d_Joint2D_getWorld);
        tolua_function(tolua_S,"getAttachedBody",lua_ax_physics2d_Joint2D_getAttachedBody);
        tolua_function(tolua_S,"setConnectedBody",lua_ax_physics2d_Joint2D_setConnectedBody);
        tolua_function(tolua_S,"getConnectedBody",lua_ax_physics2d_Joint2D_getConnectedBody);
        tolua_function(tolua_S,"getAnchorSpace",lua_ax_physics2d_Joint2D_getAnchorSpace);
        tolua_function(tolua_S,"setAnchorSpace",lua_ax_physics2d_Joint2D_setAnchorSpace);
        tolua_function(tolua_S,"setAnchorsInWorldSpace",lua_ax_physics2d_Joint2D_setAnchorsInWorldSpace);
        tolua_function(tolua_S,"getAnchor",lua_ax_physics2d_Joint2D_getAnchor);
        tolua_function(tolua_S,"setAnchors",lua_ax_physics2d_Joint2D_setAnchors);
        tolua_function(tolua_S,"setAnchor",lua_ax_physics2d_Joint2D_setAnchor);
        tolua_function(tolua_S,"getConnectedAnchor",lua_ax_physics2d_Joint2D_getConnectedAnchor);
        tolua_function(tolua_S,"setConnectedAnchor",lua_ax_physics2d_Joint2D_setConnectedAnchor);
        tolua_function(tolua_S,"isCollisionEnabled",lua_ax_physics2d_Joint2D_isCollisionEnabled);
        tolua_function(tolua_S,"setCollisionEnabled",lua_ax_physics2d_Joint2D_setCollisionEnabled);
        tolua_function(tolua_S,"setAutoConfigureConnectedAnchor",lua_ax_physics2d_Joint2D_setAutoConfigureConnectedAnchor);
        tolua_function(tolua_S,"isAutoConfigureConnectedAnchor",lua_ax_physics2d_Joint2D_isAutoConfigureConnectedAnchor);
        tolua_function(tolua_S,"setDrawScale",lua_ax_physics2d_Joint2D_setDrawScale);
        tolua_function(tolua_S,"getTag",lua_ax_physics2d_Joint2D_getTag);
        tolua_function(tolua_S,"setTag",lua_ax_physics2d_Joint2D_setTag);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Joint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Joint2D";
    g_typeCast[typeName] = "ax.Joint2D";
    return 1;
}

int lua_ax_physics2d_FixedJoint2D_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::FixedJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.FixedJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::FixedJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_FixedJoint2D_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FixedJoint2D_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FixedJoint2D:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FixedJoint2D_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_FixedJoint2D_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::FixedJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.FixedJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::FixedJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_FixedJoint2D_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.FixedJoint2D:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FixedJoint2D_setDamping'", nullptr);
            return 0;
        }
        obj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FixedJoint2D:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FixedJoint2D_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_FixedJoint2D_getFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::FixedJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.FixedJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::FixedJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_FixedJoint2D_getFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FixedJoint2D_getFrequency'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrequency();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FixedJoint2D:getFrequency",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FixedJoint2D_getFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_FixedJoint2D_setFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::FixedJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.FixedJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::FixedJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_FixedJoint2D_setFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.FixedJoint2D:setFrequency");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FixedJoint2D_setFrequency'", nullptr);
            return 0;
        }
        obj->setFrequency(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FixedJoint2D:setFrequency",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FixedJoint2D_setFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_FixedJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.FixedJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.FixedJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FixedJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::FixedJoint2D::create(arg0);
        object_to_luaval<ax::FixedJoint2D>(tolua_S, "ax.FixedJoint2D",(ax::FixedJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.FixedJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FixedJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_FixedJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (FixedJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_FixedJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.FixedJoint2D");
    tolua_cclass(tolua_S,"FixedJoint2D","ax.FixedJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"FixedJoint2D");
        tolua_function(tolua_S,"getDamping",lua_ax_physics2d_FixedJoint2D_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics2d_FixedJoint2D_setDamping);
        tolua_function(tolua_S,"getFrequency",lua_ax_physics2d_FixedJoint2D_getFrequency);
        tolua_function(tolua_S,"setFrequency",lua_ax_physics2d_FixedJoint2D_setFrequency);
        tolua_function(tolua_S,"create", lua_ax_physics2d_FixedJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::FixedJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.FixedJoint2D";
    g_typeCast[typeName] = "ax.FixedJoint2D";
    return 1;
}

int lua_ax_physics2d_DistanceJoint2D_getLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_getLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_getLength'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:getLength",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_getLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_setLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_setLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.DistanceJoint2D:setLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_setLength'", nullptr);
            return 0;
        }
        obj->setLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:setLength",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_setLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_isUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_isUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_isUseLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:isUseLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_isUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.DistanceJoint2D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.DistanceJoint2D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::DistanceJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::DistanceJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_DistanceJoint2D_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.DistanceJoint2D:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_setDamping'", nullptr);
            return 0;
        }
        obj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DistanceJoint2D:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_DistanceJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.DistanceJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.DistanceJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_DistanceJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::DistanceJoint2D::create(arg0);
        object_to_luaval<ax::DistanceJoint2D>(tolua_S, "ax.DistanceJoint2D",(ax::DistanceJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.DistanceJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_DistanceJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_DistanceJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (DistanceJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_DistanceJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.DistanceJoint2D");
    tolua_cclass(tolua_S,"DistanceJoint2D","ax.DistanceJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"DistanceJoint2D");
        tolua_function(tolua_S,"getLength",lua_ax_physics2d_DistanceJoint2D_getLength);
        tolua_function(tolua_S,"setLength",lua_ax_physics2d_DistanceJoint2D_setLength);
        tolua_function(tolua_S,"isUseLimits",lua_ax_physics2d_DistanceJoint2D_isUseLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics2d_DistanceJoint2D_setUseLimits);
        tolua_function(tolua_S,"getLimits",lua_ax_physics2d_DistanceJoint2D_getLimits);
        tolua_function(tolua_S,"setLimits",lua_ax_physics2d_DistanceJoint2D_setLimits);
        tolua_function(tolua_S,"getDamping",lua_ax_physics2d_DistanceJoint2D_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics2d_DistanceJoint2D_setDamping);
        tolua_function(tolua_S,"create", lua_ax_physics2d_DistanceJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::DistanceJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.DistanceJoint2D";
    g_typeCast[typeName] = "ax.DistanceJoint2D";
    return 1;
}

int lua_ax_physics2d_SpringJoint2D_getRestLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_getRestLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_getRestLength'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRestLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:getRestLength",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_getRestLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_setRestLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_setRestLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SpringJoint2D:setRestLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_setRestLength'", nullptr);
            return 0;
        }
        obj->setRestLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:setRestLength",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_setRestLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_getStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_getStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_getStiffness'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStiffness();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:getStiffness",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_getStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_setStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_setStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SpringJoint2D:setStiffness");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_setStiffness'", nullptr);
            return 0;
        }
        obj->setStiffness(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:setStiffness",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_setStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::SpringJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SpringJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SpringJoint2D_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SpringJoint2D:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_setDamping'", nullptr);
            return 0;
        }
        obj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SpringJoint2D:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SpringJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SpringJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        double arg0;
        double arg1;
        ax::Rigidbody2D* arg2;
        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SpringJoint2D:create");
        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.SpringJoint2D:create");
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 4, "ax.Rigidbody2D",&arg2, "ax.SpringJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SpringJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SpringJoint2D::create(arg0, arg1, arg2);
        object_to_luaval<ax::SpringJoint2D>(tolua_S, "ax.SpringJoint2D",(ax::SpringJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SpringJoint2D:create",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SpringJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_SpringJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SpringJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_SpringJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.SpringJoint2D");
    tolua_cclass(tolua_S,"SpringJoint2D","ax.SpringJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"SpringJoint2D");
        tolua_function(tolua_S,"getRestLength",lua_ax_physics2d_SpringJoint2D_getRestLength);
        tolua_function(tolua_S,"setRestLength",lua_ax_physics2d_SpringJoint2D_setRestLength);
        tolua_function(tolua_S,"getStiffness",lua_ax_physics2d_SpringJoint2D_getStiffness);
        tolua_function(tolua_S,"setStiffness",lua_ax_physics2d_SpringJoint2D_setStiffness);
        tolua_function(tolua_S,"getDamping",lua_ax_physics2d_SpringJoint2D_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics2d_SpringJoint2D_setDamping);
        tolua_function(tolua_S,"create", lua_ax_physics2d_SpringJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::SpringJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.SpringJoint2D";
    g_typeCast[typeName] = "ax.SpringJoint2D";
    return 1;
}

int lua_ax_physics2d_SliderJoint2D_getAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_getAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_getAngle'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:getAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_getAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SliderJoint2D:setAngle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setAngle'", nullptr);
            return 0;
        }
        obj->setAngle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setAngle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setAngleInRadians(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setAngleInRadians'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.SliderJoint2D:setAngleInRadians");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setAngleInRadians'", nullptr);
            return 0;
        }
        obj->setAngleInRadians(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setAngleInRadians",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setAngleInRadians'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_isUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_isUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_isUseMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:isUseMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_isUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.SliderJoint2D:setUseMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setUseMotor'", nullptr);
            return 0;
        }
        obj->setUseMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setUseMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_isUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_isUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_isUseLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:isUseLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_isUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.SliderJoint2D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_getMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_getMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_getMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotor();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:getMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_getMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.SliderJoint2D:setMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setMotor'", nullptr);
            return 0;
        }
        obj->setMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_GetLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_GetLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_GetLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->GetLimits();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:GetLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_GetLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.SliderJoint2D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_getJointTranslation(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_getJointTranslation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_getJointTranslation'", nullptr);
            return 0;
        }
        auto&& ret = obj->getJointTranslation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:getJointTranslation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_getJointTranslation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_getJointSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_getJointSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_getJointSpeed'", nullptr);
            return 0;
        }
        auto&& ret = obj->getJointSpeed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:getJointSpeed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_getJointSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_getMotorForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::SliderJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::SliderJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_SliderJoint2D_getMotorForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_getMotorForce'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotorForce();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.SliderJoint2D:getMotorForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_getMotorForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_SliderJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.SliderJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.SliderJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_SliderJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::SliderJoint2D::create(arg0);
        object_to_luaval<ax::SliderJoint2D>(tolua_S, "ax.SliderJoint2D",(ax::SliderJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.SliderJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_SliderJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_SliderJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SliderJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_SliderJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.SliderJoint2D");
    tolua_cclass(tolua_S,"SliderJoint2D","ax.SliderJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"SliderJoint2D");
        tolua_function(tolua_S,"getAngle",lua_ax_physics2d_SliderJoint2D_getAngle);
        tolua_function(tolua_S,"setAngle",lua_ax_physics2d_SliderJoint2D_setAngle);
        tolua_function(tolua_S,"setAngleInRadians",lua_ax_physics2d_SliderJoint2D_setAngleInRadians);
        tolua_function(tolua_S,"isUseMotor",lua_ax_physics2d_SliderJoint2D_isUseMotor);
        tolua_function(tolua_S,"setUseMotor",lua_ax_physics2d_SliderJoint2D_setUseMotor);
        tolua_function(tolua_S,"isUseLimits",lua_ax_physics2d_SliderJoint2D_isUseLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics2d_SliderJoint2D_setUseLimits);
        tolua_function(tolua_S,"getMotor",lua_ax_physics2d_SliderJoint2D_getMotor);
        tolua_function(tolua_S,"setMotor",lua_ax_physics2d_SliderJoint2D_setMotor);
        tolua_function(tolua_S,"GetLimits",lua_ax_physics2d_SliderJoint2D_GetLimits);
        tolua_function(tolua_S,"setLimits",lua_ax_physics2d_SliderJoint2D_setLimits);
        tolua_function(tolua_S,"getJointTranslation",lua_ax_physics2d_SliderJoint2D_getJointTranslation);
        tolua_function(tolua_S,"getJointSpeed",lua_ax_physics2d_SliderJoint2D_getJointSpeed);
        tolua_function(tolua_S,"getMotorForce",lua_ax_physics2d_SliderJoint2D_getMotorForce);
        tolua_function(tolua_S,"create", lua_ax_physics2d_SliderJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::SliderJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.SliderJoint2D";
    g_typeCast[typeName] = "ax.SliderJoint2D";
    return 1;
}

int lua_ax_physics2d_WheelJoint2D_isUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_isUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_isUseMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:isUseMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_isUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.WheelJoint2D:setUseMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setUseMotor'", nullptr);
            return 0;
        }
        obj->setUseMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setUseMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_getMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_getMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_getMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotor();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:getMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_getMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.WheelJoint2D:setMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setMotor'", nullptr);
            return 0;
        }
        obj->setMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_isUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_isUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_isUseLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:isUseLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_isUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.WheelJoint2D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.WheelJoint2D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.WheelJoint2D:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setDamping'", nullptr);
            return 0;
        }
        obj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_getStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_getStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_getStiffness'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStiffness();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:getStiffness",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_getStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_setStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_setStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.WheelJoint2D:setStiffness");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_setStiffness'", nullptr);
            return 0;
        }
        obj->setStiffness(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:setStiffness",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_setStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_getMotorTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::WheelJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::WheelJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_WheelJoint2D_getMotorTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_getMotorTorque'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotorTorque();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.WheelJoint2D:getMotorTorque",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_getMotorTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_WheelJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.WheelJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.WheelJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_WheelJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::WheelJoint2D::create(arg0);
        object_to_luaval<ax::WheelJoint2D>(tolua_S, "ax.WheelJoint2D",(ax::WheelJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.WheelJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_WheelJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_WheelJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (WheelJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_WheelJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.WheelJoint2D");
    tolua_cclass(tolua_S,"WheelJoint2D","ax.WheelJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"WheelJoint2D");
        tolua_function(tolua_S,"isUseMotor",lua_ax_physics2d_WheelJoint2D_isUseMotor);
        tolua_function(tolua_S,"setUseMotor",lua_ax_physics2d_WheelJoint2D_setUseMotor);
        tolua_function(tolua_S,"getMotor",lua_ax_physics2d_WheelJoint2D_getMotor);
        tolua_function(tolua_S,"setMotor",lua_ax_physics2d_WheelJoint2D_setMotor);
        tolua_function(tolua_S,"isUseLimits",lua_ax_physics2d_WheelJoint2D_isUseLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics2d_WheelJoint2D_setUseLimits);
        tolua_function(tolua_S,"getLimits",lua_ax_physics2d_WheelJoint2D_getLimits);
        tolua_function(tolua_S,"setLimits",lua_ax_physics2d_WheelJoint2D_setLimits);
        tolua_function(tolua_S,"getDamping",lua_ax_physics2d_WheelJoint2D_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics2d_WheelJoint2D_setDamping);
        tolua_function(tolua_S,"getStiffness",lua_ax_physics2d_WheelJoint2D_getStiffness);
        tolua_function(tolua_S,"setStiffness",lua_ax_physics2d_WheelJoint2D_setStiffness);
        tolua_function(tolua_S,"getMotorTorque",lua_ax_physics2d_WheelJoint2D_getMotorTorque);
        tolua_function(tolua_S,"create", lua_ax_physics2d_WheelJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::WheelJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.WheelJoint2D";
    g_typeCast[typeName] = "ax.WheelJoint2D";
    return 1;
}

int lua_ax_physics2d_PivotJoint2D_isUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_isUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_isUseMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:isUseMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_isUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setUseMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setUseMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PivotJoint2D:setUseMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setUseMotor'", nullptr);
            return 0;
        }
        obj->setUseMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setUseMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setUseMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_isUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_isUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_isUseLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseLimits();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:isUseLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_isUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setUseLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setUseLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PivotJoint2D:setUseLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setUseLimits'", nullptr);
            return 0;
        }
        obj->setUseLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setUseLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setUseLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getMotor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotor();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PivotJoint2D:setMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setMotor'", nullptr);
            return 0;
        }
        obj->setMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getMotorTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getMotorTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getMotorTorque'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMotorTorque();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getMotorTorque",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getMotorTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getLimits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLimits();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getLimits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setLimits(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setLimits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PivotJoint2D:setLimits");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setLimits'", nullptr);
            return 0;
        }
        obj->setLimits(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setLimits",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setLimits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_isUseSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_isUseSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_isUseSpring'", nullptr);
            return 0;
        }
        auto&& ret = obj->isUseSpring();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:isUseSpring",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_isUseSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setUseSpring(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setUseSpring'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PivotJoint2D:setUseSpring");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setUseSpring'", nullptr);
            return 0;
        }
        obj->setUseSpring(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setUseSpring",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setUseSpring'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getSpringDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getSpringDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getSpringDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpringDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getSpringDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getSpringDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setSpringDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setSpringDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PivotJoint2D:setSpringDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setSpringDamping'", nullptr);
            return 0;
        }
        obj->setSpringDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setSpringDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setSpringDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getSpringFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getSpringFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getSpringFrequency'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpringFrequency();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getSpringFrequency",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getSpringFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_setSpringFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_setSpringFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PivotJoint2D:setSpringFrequency");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_setSpringFrequency'", nullptr);
            return 0;
        }
        obj->setSpringFrequency(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:setSpringFrequency",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_setSpringFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_getReferenceAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PivotJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PivotJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PivotJoint2D_getReferenceAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_getReferenceAngle'", nullptr);
            return 0;
        }
        auto&& ret = obj->getReferenceAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PivotJoint2D:getReferenceAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_getReferenceAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PivotJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PivotJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.PivotJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PivotJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PivotJoint2D::create(arg0);
        object_to_luaval<ax::PivotJoint2D>(tolua_S, "ax.PivotJoint2D",(ax::PivotJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PivotJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PivotJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_PivotJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PivotJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_PivotJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PivotJoint2D");
    tolua_cclass(tolua_S,"PivotJoint2D","ax.PivotJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"PivotJoint2D");
        tolua_function(tolua_S,"isUseMotor",lua_ax_physics2d_PivotJoint2D_isUseMotor);
        tolua_function(tolua_S,"setUseMotor",lua_ax_physics2d_PivotJoint2D_setUseMotor);
        tolua_function(tolua_S,"isUseLimits",lua_ax_physics2d_PivotJoint2D_isUseLimits);
        tolua_function(tolua_S,"setUseLimits",lua_ax_physics2d_PivotJoint2D_setUseLimits);
        tolua_function(tolua_S,"getMotor",lua_ax_physics2d_PivotJoint2D_getMotor);
        tolua_function(tolua_S,"setMotor",lua_ax_physics2d_PivotJoint2D_setMotor);
        tolua_function(tolua_S,"getMotorTorque",lua_ax_physics2d_PivotJoint2D_getMotorTorque);
        tolua_function(tolua_S,"getLimits",lua_ax_physics2d_PivotJoint2D_getLimits);
        tolua_function(tolua_S,"setLimits",lua_ax_physics2d_PivotJoint2D_setLimits);
        tolua_function(tolua_S,"isUseSpring",lua_ax_physics2d_PivotJoint2D_isUseSpring);
        tolua_function(tolua_S,"setUseSpring",lua_ax_physics2d_PivotJoint2D_setUseSpring);
        tolua_function(tolua_S,"getSpringDamping",lua_ax_physics2d_PivotJoint2D_getSpringDamping);
        tolua_function(tolua_S,"setSpringDamping",lua_ax_physics2d_PivotJoint2D_setSpringDamping);
        tolua_function(tolua_S,"getSpringFrequency",lua_ax_physics2d_PivotJoint2D_getSpringFrequency);
        tolua_function(tolua_S,"setSpringFrequency",lua_ax_physics2d_PivotJoint2D_setSpringFrequency);
        tolua_function(tolua_S,"getReferenceAngle",lua_ax_physics2d_PivotJoint2D_getReferenceAngle);
        tolua_function(tolua_S,"create", lua_ax_physics2d_PivotJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PivotJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PivotJoint2D";
    g_typeCast[typeName] = "ax.PivotJoint2D";
    return 1;
}

int lua_ax_physics2d_PinJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PinJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Rigidbody2D* arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PinJoint2D:create");
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 3, "ax.Rigidbody2D",&arg1, "ax.PinJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PinJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PinJoint2D::create(arg0, arg1);
        object_to_luaval<ax::PinJoint2D>(tolua_S, "ax.PinJoint2D",(ax::PinJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PinJoint2D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PinJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_PinJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PinJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_PinJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PinJoint2D");
    tolua_cclass(tolua_S,"PinJoint2D","ax.PinJoint2D","ax.PivotJoint2D",nullptr);

    tolua_beginmodule(tolua_S,"PinJoint2D");
        tolua_function(tolua_S,"create", lua_ax_physics2d_PinJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PinJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PinJoint2D";
    g_typeCast[typeName] = "ax.PinJoint2D";
    return 1;
}

int lua_ax_physics2d_MotorJoint2D_setAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.MotorJoint2D:setAngularVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setAngularVelocity'", nullptr);
            return 0;
        }
        obj->setAngularVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setAngularVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_setAngularFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setAngularFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.MotorJoint2D:setAngularFrequency");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setAngularFrequency'", nullptr);
            return 0;
        }
        obj->setAngularFrequency(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setAngularFrequency",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setAngularFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_setAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.MotorJoint2D:setAngularDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setAngularDamping'", nullptr);
            return 0;
        }
        obj->setAngularDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setAngularDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_setLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.MotorJoint2D:setLinearVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setLinearVelocity'", nullptr);
            return 0;
        }
        obj->setLinearVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setLinearVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_setLinearFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setLinearFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.MotorJoint2D:setLinearFrequency");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setLinearFrequency'", nullptr);
            return 0;
        }
        obj->setLinearFrequency(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setLinearFrequency",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setLinearFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_setLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_setLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.MotorJoint2D:setLinearDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_setLinearDamping'", nullptr);
            return 0;
        }
        obj->setLinearDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:setLinearDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_setLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getAngularVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getAngularVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getAngularFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getAngularFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getAngularFrequency'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularFrequency();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getAngularFrequency",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getAngularFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getAngularDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAngularDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getAngularDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getLinearVelocity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearVelocity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getLinearVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getLinearFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getLinearFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getLinearFrequency'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearFrequency();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getLinearFrequency",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getLinearFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_getLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::MotorJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::MotorJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_MotorJoint2D_getLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_getLinearDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLinearDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MotorJoint2D:getLinearDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_getLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_MotorJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.MotorJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.MotorJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_MotorJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::MotorJoint2D::create(arg0);
        object_to_luaval<ax::MotorJoint2D>(tolua_S, "ax.MotorJoint2D",(ax::MotorJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.MotorJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_MotorJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_MotorJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (MotorJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_MotorJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.MotorJoint2D");
    tolua_cclass(tolua_S,"MotorJoint2D","ax.MotorJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"MotorJoint2D");
        tolua_function(tolua_S,"setAngularVelocity",lua_ax_physics2d_MotorJoint2D_setAngularVelocity);
        tolua_function(tolua_S,"setAngularFrequency",lua_ax_physics2d_MotorJoint2D_setAngularFrequency);
        tolua_function(tolua_S,"setAngularDamping",lua_ax_physics2d_MotorJoint2D_setAngularDamping);
        tolua_function(tolua_S,"setLinearVelocity",lua_ax_physics2d_MotorJoint2D_setLinearVelocity);
        tolua_function(tolua_S,"setLinearFrequency",lua_ax_physics2d_MotorJoint2D_setLinearFrequency);
        tolua_function(tolua_S,"setLinearDamping",lua_ax_physics2d_MotorJoint2D_setLinearDamping);
        tolua_function(tolua_S,"getAngularVelocity",lua_ax_physics2d_MotorJoint2D_getAngularVelocity);
        tolua_function(tolua_S,"getAngularFrequency",lua_ax_physics2d_MotorJoint2D_getAngularFrequency);
        tolua_function(tolua_S,"getAngularDamping",lua_ax_physics2d_MotorJoint2D_getAngularDamping);
        tolua_function(tolua_S,"getLinearVelocity",lua_ax_physics2d_MotorJoint2D_getLinearVelocity);
        tolua_function(tolua_S,"getLinearFrequency",lua_ax_physics2d_MotorJoint2D_getLinearFrequency);
        tolua_function(tolua_S,"getLinearDamping",lua_ax_physics2d_MotorJoint2D_getLinearDamping);
        tolua_function(tolua_S,"create", lua_ax_physics2d_MotorJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::MotorJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.MotorJoint2D";
    g_typeCast[typeName] = "ax.MotorJoint2D";
    return 1;
}

int lua_ax_physics2d_FilterJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.FilterJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Rigidbody2D* arg0;
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 2, "ax.Rigidbody2D",&arg0, "ax.FilterJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_FilterJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::FilterJoint2D::create(arg0);
        object_to_luaval<ax::FilterJoint2D>(tolua_S, "ax.FilterJoint2D",(ax::FilterJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.FilterJoint2D:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_FilterJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_FilterJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (FilterJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_FilterJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.FilterJoint2D");
    tolua_cclass(tolua_S,"FilterJoint2D","ax.FilterJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"FilterJoint2D");
        tolua_function(tolua_S,"create", lua_ax_physics2d_FilterJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::FilterJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.FilterJoint2D";
    g_typeCast[typeName] = "ax.FilterJoint2D";
    return 1;
}

int lua_ax_physics2d_TargetJoint2D_setMaxForceScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_setMaxForceScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.TargetJoint2D:setMaxForceScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_setMaxForceScale'", nullptr);
            return 0;
        }
        obj->setMaxForceScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:setMaxForceScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_setMaxForceScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_getMaxForceScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_getMaxForceScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_getMaxForceScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxForceScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:getMaxForceScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_getMaxForceScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_getMaxForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_getMaxForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_getMaxForce'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxForce();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:getMaxForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_getMaxForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.TargetJoint2D:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_setDamping'", nullptr);
            return 0;
        }
        obj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_setFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_setFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.TargetJoint2D:setFrequency");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_setFrequency'", nullptr);
            return 0;
        }
        obj->setFrequency(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:setFrequency",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_setFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_getFrequency(lua_State* tolua_S)
{
    int argc = 0;
    ax::TargetJoint2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::TargetJoint2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_TargetJoint2D_getFrequency'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_getFrequency'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrequency();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TargetJoint2D:getFrequency",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_getFrequency'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_TargetJoint2D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.TargetJoint2D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Rigidbody2D* arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.TargetJoint2D:create");
        ok &= luaval_to_object<ax::Rigidbody2D>(tolua_S, 3, "ax.Rigidbody2D",&arg1, "ax.TargetJoint2D:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_TargetJoint2D_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::TargetJoint2D::create(arg0, arg1);
        object_to_luaval<ax::TargetJoint2D>(tolua_S, "ax.TargetJoint2D",(ax::TargetJoint2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.TargetJoint2D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_TargetJoint2D_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics2d_TargetJoint2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (TargetJoint2D)");
    return 0;
}

int lua_register_ax_physics2d_TargetJoint2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.TargetJoint2D");
    tolua_cclass(tolua_S,"TargetJoint2D","ax.TargetJoint2D","ax.Joint2D",nullptr);

    tolua_beginmodule(tolua_S,"TargetJoint2D");
        tolua_function(tolua_S,"setMaxForceScale",lua_ax_physics2d_TargetJoint2D_setMaxForceScale);
        tolua_function(tolua_S,"getMaxForceScale",lua_ax_physics2d_TargetJoint2D_getMaxForceScale);
        tolua_function(tolua_S,"getMaxForce",lua_ax_physics2d_TargetJoint2D_getMaxForce);
        tolua_function(tolua_S,"setDamping",lua_ax_physics2d_TargetJoint2D_setDamping);
        tolua_function(tolua_S,"getDamping",lua_ax_physics2d_TargetJoint2D_getDamping);
        tolua_function(tolua_S,"setFrequency",lua_ax_physics2d_TargetJoint2D_setFrequency);
        tolua_function(tolua_S,"getFrequency",lua_ax_physics2d_TargetJoint2D_getFrequency);
        tolua_function(tolua_S,"create", lua_ax_physics2d_TargetJoint2D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::TargetJoint2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.TargetJoint2D";
    g_typeCast[typeName] = "ax.TargetJoint2D";
    return 1;
}

int lua_ax_physics2d_PhysicsWorld2D_internalHandle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_internalHandle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_internalHandle'", nullptr);
            return 0;
        }
        auto&& ret = obj->internalHandle();
        #pragma warning NO CONVERSION FROM NATIVE FOR b2WorldId;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:internalHandle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_internalHandle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_isGlobalEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_isGlobalEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ContactEventBits arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:isGlobalEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_isGlobalEventEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isGlobalEventEnabled(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:isGlobalEventEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_isGlobalEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setGlobalEventEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setGlobalEventEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ContactEventBits arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:setGlobalEventEnabled");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "ax.PhysicsWorld2D:setGlobalEventEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setGlobalEventEnabled'", nullptr);
            return 0;
        }
        obj->setGlobalEventEnabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setGlobalEventEnabled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setGlobalEventEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_overlapBox(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBox'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:overlapBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBox'", nullptr);
            return 0;
        }
        auto&& ret = obj->overlapBox(arg0);
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:overlapBox",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBox'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_overlapBoxAll(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBoxAll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:overlapBoxAll");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBoxAll'", nullptr);
            return 0;
        }
        auto&& ret = obj->overlapBoxAll(arg0);
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:overlapBoxAll",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_overlapBoxAll'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_overlapPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:overlapPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->overlapPoint(arg0);
        object_to_luaval<ax::Collider2D>(tolua_S, "ax.Collider2D",(ax::Collider2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:overlapPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_overlapPointAll(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPointAll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:overlapPointAll");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPointAll'", nullptr);
            return 0;
        }
        auto&& ret = obj->overlapPointAll(arg0);
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:overlapPointAll",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_overlapPointAll'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_getGravity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getGravity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:getGravity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setGravity'", nullptr);
            return 0;
        }
        obj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setGravity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:setSpeed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setSpeed'", nullptr);
            return 0;
        }
        obj->setSpeed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setSpeed",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_getSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_getSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_getSpeed'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpeed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:getSpeed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_getSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setSubsteps(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setSubsteps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:setSubsteps");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setSubsteps'", nullptr);
            return 0;
        }
        obj->setSubsteps(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setSubsteps",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setSubsteps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_getSubsteps(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_getSubsteps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_getSubsteps'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSubsteps();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:getSubsteps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_getSubsteps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setPreUpdateCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setPreUpdateCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void ()> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setPreUpdateCallback'", nullptr);
            return 0;
        }
        obj->setPreUpdateCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setPreUpdateCallback",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setPreUpdateCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setPostUpdateCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setPostUpdateCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void ()> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setPostUpdateCallback'", nullptr);
            return 0;
        }
        obj->setPostUpdateCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setPostUpdateCallback",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setPostUpdateCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_setAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_setAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:setAutoStep");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_setAutoStep'", nullptr);
            return 0;
        }
        obj->setAutoStep(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:setAutoStep",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_setAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_isAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_isAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_isAutoStep'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAutoStep();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:isAutoStep",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_isAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics2d_PhysicsWorld2D_stepSimulation(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld2D* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld2D",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::PhysicsWorld2D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_physics2d_PhysicsWorld2D_stepSimulation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "ax.PhysicsWorld2D:stepSimulation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics2d_PhysicsWorld2D_stepSimulation'", nullptr);
            return 0;
        }
        obj->stepSimulation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld2D:stepSimulation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics2d_PhysicsWorld2D_stepSimulation'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics2d_PhysicsWorld2D_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PhysicsWorld2D)");
    return 0;
}

int lua_register_ax_physics2d_PhysicsWorld2D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsWorld2D");
    tolua_cclass(tolua_S,"PhysicsWorld2D","ax.PhysicsWorld2D","",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsWorld2D");
        tolua_function(tolua_S,"internalHandle",lua_ax_physics2d_PhysicsWorld2D_internalHandle);
        tolua_function(tolua_S,"isGlobalEventEnabled",lua_ax_physics2d_PhysicsWorld2D_isGlobalEventEnabled);
        tolua_function(tolua_S,"setGlobalEventEnabled",lua_ax_physics2d_PhysicsWorld2D_setGlobalEventEnabled);
        tolua_function(tolua_S,"overlapBox",lua_ax_physics2d_PhysicsWorld2D_overlapBox);
        tolua_function(tolua_S,"overlapBoxAll",lua_ax_physics2d_PhysicsWorld2D_overlapBoxAll);
        tolua_function(tolua_S,"overlapPoint",lua_ax_physics2d_PhysicsWorld2D_overlapPoint);
        tolua_function(tolua_S,"overlapPointAll",lua_ax_physics2d_PhysicsWorld2D_overlapPointAll);
        tolua_function(tolua_S,"getGravity",lua_ax_physics2d_PhysicsWorld2D_getGravity);
        tolua_function(tolua_S,"setGravity",lua_ax_physics2d_PhysicsWorld2D_setGravity);
        tolua_function(tolua_S,"setSpeed",lua_ax_physics2d_PhysicsWorld2D_setSpeed);
        tolua_function(tolua_S,"getSpeed",lua_ax_physics2d_PhysicsWorld2D_getSpeed);
        tolua_function(tolua_S,"setSubsteps",lua_ax_physics2d_PhysicsWorld2D_setSubsteps);
        tolua_function(tolua_S,"getSubsteps",lua_ax_physics2d_PhysicsWorld2D_getSubsteps);
        tolua_function(tolua_S,"setPreUpdateCallback",lua_ax_physics2d_PhysicsWorld2D_setPreUpdateCallback);
        tolua_function(tolua_S,"setPostUpdateCallback",lua_ax_physics2d_PhysicsWorld2D_setPostUpdateCallback);
        tolua_function(tolua_S,"setAutoStep",lua_ax_physics2d_PhysicsWorld2D_setAutoStep);
        tolua_function(tolua_S,"isAutoStep",lua_ax_physics2d_PhysicsWorld2D_isAutoStep);
        tolua_function(tolua_S,"stepSimulation",lua_ax_physics2d_PhysicsWorld2D_stepSimulation);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsWorld2D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsWorld2D";
    g_typeCast[typeName] = "ax.PhysicsWorld2D";
    return 1;
}
TOLUA_API int register_all_ax_physics2d(lua_State* tolua_S)
{
    tolua_open(tolua_S);

    tolua_module(tolua_S,"ax",0);
    tolua_beginmodule(tolua_S,"ax");

    lua_register_ax_physics2d_Collider2D(tolua_S);
    lua_register_ax_physics2d_CircleCollider2D(tolua_S);
    lua_register_ax_physics2d_PolygonCollider2D(tolua_S);
    lua_register_ax_physics2d_BoxCollider2D(tolua_S);
    lua_register_ax_physics2d_EdgeSegmentCollider2D(tolua_S);
    lua_register_ax_physics2d_EdgePolygonCollider2D(tolua_S);
    lua_register_ax_physics2d_EdgeBoxCollider2D(tolua_S);
    lua_register_ax_physics2d_EdgeChainCollider2D(tolua_S);
    lua_register_ax_physics2d_Rigidbody2D(tolua_S);
    lua_register_ax_physics2d_ContactEvent2D(tolua_S);
    lua_register_ax_physics2d_ContactEventListener2D(tolua_S);
    lua_register_ax_physics2d_Joint2D(tolua_S);
    lua_register_ax_physics2d_FixedJoint2D(tolua_S);
    lua_register_ax_physics2d_DistanceJoint2D(tolua_S);
    lua_register_ax_physics2d_SpringJoint2D(tolua_S);
    lua_register_ax_physics2d_SliderJoint2D(tolua_S);
    lua_register_ax_physics2d_WheelJoint2D(tolua_S);
    lua_register_ax_physics2d_PivotJoint2D(tolua_S);
    lua_register_ax_physics2d_PinJoint2D(tolua_S);
    lua_register_ax_physics2d_MotorJoint2D(tolua_S);
    lua_register_ax_physics2d_FilterJoint2D(tolua_S);
    lua_register_ax_physics2d_TargetJoint2D(tolua_S);
    lua_register_ax_physics2d_PhysicsWorld2D(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

#endif
