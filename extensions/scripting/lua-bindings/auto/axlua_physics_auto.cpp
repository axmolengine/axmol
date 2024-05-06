#include "lua-bindings/auto/axlua_physics_auto.hpp"
#if defined(AX_ENABLE_PHYSICS)
#include "cocos2d.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_physics_PhysicsShape_getBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getBody'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBody();
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getType(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getArea(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getArea'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getArea();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getArea",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getMoment'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMoment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getMoment",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShape:setMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setMoment'", nullptr);
            return 0;
        }
        cobj->setMoment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setMoment",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShape:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setTag'", nullptr);
            return 0;
        }
        cobj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getTag'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getMass'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMass();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShape:setMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setMass'", nullptr);
            return 0;
        }
        cobj->setMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getDensity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getDensity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getDensity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDensity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getDensity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getDensity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setDensity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setDensity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShape:setDensity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setDensity'", nullptr);
            return 0;
        }
        cobj->setDensity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setDensity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setDensity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getRestitution'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitution();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getRestitution",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShape:setRestitution");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setRestitution'", nullptr);
            return 0;
        }
        cobj->setRestitution(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setRestitution",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getFriction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShape:setFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setFriction'", nullptr);
            return 0;
        }
        cobj->setFriction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setFriction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getMaterial'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaterial();
        physics_material_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getMaterial",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setMaterial(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setMaterial'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PhysicsMaterial arg0;

        ok &= luaval_to_physics_material(tolua_S, 2, &arg0, "ax.PhysicsShape:setMaterial");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setMaterial'", nullptr);
            return 0;
        }
        cobj->setMaterial(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setMaterial",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setMaterial'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_isSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_isSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_isSensor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isSensor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:isSensor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_isSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setSensor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setSensor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsShape:setSensor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setSensor'", nullptr);
            return 0;
        }
        cobj->setSensor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setSensor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setSensor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_calculateDefaultMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_calculateDefaultMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_calculateDefaultMoment'", nullptr);
            return 0;
        }
        auto&& ret = cobj->calculateDefaultMoment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:calculateDefaultMoment",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_calculateDefaultMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getCenter(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getCenter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getCenter'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCenter();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getCenter",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getCenter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_containsPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_containsPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShape:containsPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_containsPoint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->containsPoint(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:containsPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_containsPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setCategoryBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setCategoryBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShape:setCategoryBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setCategoryBitmask'", nullptr);
            return 0;
        }
        cobj->setCategoryBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setCategoryBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setCategoryBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getCategoryBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getCategoryBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getCategoryBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCategoryBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getCategoryBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getCategoryBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setContactTestBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setContactTestBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShape:setContactTestBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setContactTestBitmask'", nullptr);
            return 0;
        }
        cobj->setContactTestBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setContactTestBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setContactTestBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getContactTestBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getContactTestBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getContactTestBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContactTestBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getContactTestBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getContactTestBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setCollisionBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setCollisionBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShape:setCollisionBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setCollisionBitmask'", nullptr);
            return 0;
        }
        cobj->setCollisionBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setCollisionBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setCollisionBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getCollisionBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getCollisionBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getCollisionBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCollisionBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getCollisionBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getCollisionBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_setGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_setGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShape:setGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_setGroup'", nullptr);
            return 0;
        }
        cobj->setGroup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:setGroup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_setGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShape_getGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShape_getGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShape_getGroup'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGroup();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShape:getGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShape_getGroup'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsShape_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShape)");
    return 0;
}

int lua_register_ax_physics_PhysicsShape(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShape");
    tolua_cclass(tolua_S,"PhysicsShape","ax.PhysicsShape","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShape");
        tolua_function(tolua_S,"getBody",lua_ax_physics_PhysicsShape_getBody);
        tolua_function(tolua_S,"getType",lua_ax_physics_PhysicsShape_getType);
        tolua_function(tolua_S,"getArea",lua_ax_physics_PhysicsShape_getArea);
        tolua_function(tolua_S,"getMoment",lua_ax_physics_PhysicsShape_getMoment);
        tolua_function(tolua_S,"setMoment",lua_ax_physics_PhysicsShape_setMoment);
        tolua_function(tolua_S,"setTag",lua_ax_physics_PhysicsShape_setTag);
        tolua_function(tolua_S,"getTag",lua_ax_physics_PhysicsShape_getTag);
        tolua_function(tolua_S,"getMass",lua_ax_physics_PhysicsShape_getMass);
        tolua_function(tolua_S,"setMass",lua_ax_physics_PhysicsShape_setMass);
        tolua_function(tolua_S,"getDensity",lua_ax_physics_PhysicsShape_getDensity);
        tolua_function(tolua_S,"setDensity",lua_ax_physics_PhysicsShape_setDensity);
        tolua_function(tolua_S,"getRestitution",lua_ax_physics_PhysicsShape_getRestitution);
        tolua_function(tolua_S,"setRestitution",lua_ax_physics_PhysicsShape_setRestitution);
        tolua_function(tolua_S,"getFriction",lua_ax_physics_PhysicsShape_getFriction);
        tolua_function(tolua_S,"setFriction",lua_ax_physics_PhysicsShape_setFriction);
        tolua_function(tolua_S,"getMaterial",lua_ax_physics_PhysicsShape_getMaterial);
        tolua_function(tolua_S,"setMaterial",lua_ax_physics_PhysicsShape_setMaterial);
        tolua_function(tolua_S,"isSensor",lua_ax_physics_PhysicsShape_isSensor);
        tolua_function(tolua_S,"setSensor",lua_ax_physics_PhysicsShape_setSensor);
        tolua_function(tolua_S,"calculateDefaultMoment",lua_ax_physics_PhysicsShape_calculateDefaultMoment);
        tolua_function(tolua_S,"getOffset",lua_ax_physics_PhysicsShape_getOffset);
        tolua_function(tolua_S,"getCenter",lua_ax_physics_PhysicsShape_getCenter);
        tolua_function(tolua_S,"containsPoint",lua_ax_physics_PhysicsShape_containsPoint);
        tolua_function(tolua_S,"setCategoryBitmask",lua_ax_physics_PhysicsShape_setCategoryBitmask);
        tolua_function(tolua_S,"getCategoryBitmask",lua_ax_physics_PhysicsShape_getCategoryBitmask);
        tolua_function(tolua_S,"setContactTestBitmask",lua_ax_physics_PhysicsShape_setContactTestBitmask);
        tolua_function(tolua_S,"getContactTestBitmask",lua_ax_physics_PhysicsShape_getContactTestBitmask);
        tolua_function(tolua_S,"setCollisionBitmask",lua_ax_physics_PhysicsShape_setCollisionBitmask);
        tolua_function(tolua_S,"getCollisionBitmask",lua_ax_physics_PhysicsShape_getCollisionBitmask);
        tolua_function(tolua_S,"setGroup",lua_ax_physics_PhysicsShape_setGroup);
        tolua_function(tolua_S,"getGroup",lua_ax_physics_PhysicsShape_getGroup);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShape).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShape";
    g_typeCast[typeName] = "ax.PhysicsShape";
    return 1;
}

int lua_ax_physics_PhysicsShapeCircle_getRadius(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeCircle* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeCircle",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeCircle*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeCircle_getRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_getRadius'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRadius();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeCircle:getRadius",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeCircle_getRadius'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShapeCircle_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeCircle",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::create(arg0);
        object_to_luaval<ax::PhysicsShapeCircle>(tolua_S, "ax.PhysicsShapeCircle",(ax::PhysicsShapeCircle*)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeCircle:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::create(arg0, arg1);
        object_to_luaval<ax::PhysicsShapeCircle>(tolua_S, "ax.PhysicsShapeCircle",(ax::PhysicsShapeCircle*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        ax::PhysicsMaterial arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeCircle:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsShapeCircle:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::create(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsShapeCircle>(tolua_S, "ax.PhysicsShapeCircle",(ax::PhysicsShapeCircle*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeCircle:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeCircle_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsShapeCircle_calculateArea(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeCircle",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:calculateArea");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_calculateArea'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::calculateArea(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeCircle:calculateArea",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeCircle_calculateArea'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsShapeCircle_calculateMoment(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeCircle",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        double arg1;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:calculateMoment");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.PhysicsShapeCircle:calculateMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_calculateMoment'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::calculateMoment(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        double arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsShapeCircle:calculateMoment");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.PhysicsShapeCircle:calculateMoment");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsShapeCircle:calculateMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeCircle_calculateMoment'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeCircle::calculateMoment(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeCircle:calculateMoment",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeCircle_calculateMoment'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsShapeCircle_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeCircle)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeCircle(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeCircle");
    tolua_cclass(tolua_S,"PhysicsShapeCircle","ax.PhysicsShapeCircle","ax.PhysicsShape",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeCircle");
        tolua_function(tolua_S,"getRadius",lua_ax_physics_PhysicsShapeCircle_getRadius);
        tolua_function(tolua_S,"create", lua_ax_physics_PhysicsShapeCircle_create);
        tolua_function(tolua_S,"calculateArea", lua_ax_physics_PhysicsShapeCircle_calculateArea);
        tolua_function(tolua_S,"calculateMoment", lua_ax_physics_PhysicsShapeCircle_calculateMoment);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeCircle).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeCircle";
    g_typeCast[typeName] = "ax.PhysicsShapeCircle";
    return 1;
}

int lua_ax_physics_PhysicsShapePolygon_getPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapePolygon* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapePolygon",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapePolygon*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapePolygon_getPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsShapePolygon:getPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapePolygon_getPoint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapePolygon:getPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapePolygon_getPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShapePolygon_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapePolygon* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapePolygon",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapePolygon*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapePolygon_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapePolygon_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapePolygon:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapePolygon_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsShapePolygon_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapePolygon)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapePolygon(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapePolygon");
    tolua_cclass(tolua_S,"PhysicsShapePolygon","ax.PhysicsShapePolygon","ax.PhysicsShape",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapePolygon");
        tolua_function(tolua_S,"getPoint",lua_ax_physics_PhysicsShapePolygon_getPoint);
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics_PhysicsShapePolygon_getPointsCount);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapePolygon).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapePolygon";
    g_typeCast[typeName] = "ax.PhysicsShapePolygon";
    return 1;
}

int lua_ax_physics_PhysicsShapeBox_getSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeBox_getSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeBox_getSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSize();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeBox:getSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeBox_getSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShapeBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeBox::create(arg0);
        object_to_luaval<ax::PhysicsShapeBox>(tolua_S, "ax.PhysicsShapeBox",(ax::PhysicsShapeBox*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeBox::create(arg0, arg1);
        object_to_luaval<ax::PhysicsShapeBox>(tolua_S, "ax.PhysicsShapeBox",(ax::PhysicsShapeBox*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsShapeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeBox::create(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsShapeBox>(tolua_S, "ax.PhysicsShapeBox",(ax::PhysicsShapeBox*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ax::Vec2 arg2;
        double arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsShapeBox:create");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsShapeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeBox::create(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsShapeBox>(tolua_S, "ax.PhysicsShapeBox",(ax::PhysicsShapeBox*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeBox:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeBox_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsShapeBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeBox)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeBox");
    tolua_cclass(tolua_S,"PhysicsShapeBox","ax.PhysicsShapeBox","ax.PhysicsShapePolygon",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeBox");
        tolua_function(tolua_S,"getSize",lua_ax_physics_PhysicsShapeBox_getSize);
        tolua_function(tolua_S,"create", lua_ax_physics_PhysicsShapeBox_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeBox).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeBox";
    g_typeCast[typeName] = "ax.PhysicsShapeBox";
    return 1;
}

int lua_ax_physics_PhysicsShapeEdgeSegment_getPointA(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeEdgeSegment* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeEdgeSegment",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeEdgeSegment*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPointA();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeEdgeSegment:getPointA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShapeEdgeSegment_getPointB(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeEdgeSegment* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeEdgeSegment",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeEdgeSegment*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPointB();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeEdgeSegment:getPointB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgeSegment_getPointB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsShapeEdgeSegment_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeEdgeSegment",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeSegment:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeSegment_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeSegment::create(arg0, arg1);
        object_to_luaval<ax::PhysicsShapeEdgeSegment>(tolua_S, "ax.PhysicsShapeEdgeSegment",(ax::PhysicsShapeEdgeSegment*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.PhysicsShapeEdgeSegment:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeSegment_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeSegment::create(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsShapeEdgeSegment>(tolua_S, "ax.PhysicsShapeEdgeSegment",(ax::PhysicsShapeEdgeSegment*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial arg2;
        double arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.PhysicsShapeEdgeSegment:create");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsShapeEdgeSegment:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeSegment_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeSegment::create(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsShapeEdgeSegment>(tolua_S, "ax.PhysicsShapeEdgeSegment",(ax::PhysicsShapeEdgeSegment*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeEdgeSegment:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgeSegment_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsShapeEdgeSegment_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeEdgeSegment)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeEdgeSegment(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeEdgeSegment");
    tolua_cclass(tolua_S,"PhysicsShapeEdgeSegment","ax.PhysicsShapeEdgeSegment","ax.PhysicsShape",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeEdgeSegment");
        tolua_function(tolua_S,"getPointA",lua_ax_physics_PhysicsShapeEdgeSegment_getPointA);
        tolua_function(tolua_S,"getPointB",lua_ax_physics_PhysicsShapeEdgeSegment_getPointB);
        tolua_function(tolua_S,"create", lua_ax_physics_PhysicsShapeEdgeSegment_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeEdgeSegment).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeEdgeSegment";
    g_typeCast[typeName] = "ax.PhysicsShapeEdgeSegment";
    return 1;
}

int lua_ax_physics_PhysicsShapeEdgePolygon_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeEdgePolygon* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeEdgePolygon",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeEdgePolygon*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeEdgePolygon_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgePolygon_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeEdgePolygon:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgePolygon_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsShapeEdgePolygon_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeEdgePolygon)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeEdgePolygon(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeEdgePolygon");
    tolua_cclass(tolua_S,"PhysicsShapeEdgePolygon","ax.PhysicsShapeEdgePolygon","ax.PhysicsShape",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeEdgePolygon");
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics_PhysicsShapeEdgePolygon_getPointsCount);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeEdgePolygon).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeEdgePolygon";
    g_typeCast[typeName] = "ax.PhysicsShapeEdgePolygon";
    return 1;
}

int lua_ax_physics_PhysicsShapeEdgeBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsShapeEdgeBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeBox::create(arg0);
        object_to_luaval<ax::PhysicsShapeEdgeBox>(tolua_S, "ax.PhysicsShapeEdgeBox",(ax::PhysicsShapeEdgeBox*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeBox::create(arg0, arg1);
        object_to_luaval<ax::PhysicsShapeEdgeBox>(tolua_S, "ax.PhysicsShapeEdgeBox",(ax::PhysicsShapeEdgeBox*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        double arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsShapeEdgeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeBox::create(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsShapeEdgeBox>(tolua_S, "ax.PhysicsShapeEdgeBox",(ax::PhysicsShapeEdgeBox*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        double arg2;
        ax::Vec2 arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsShapeEdgeBox:create");
        ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsShapeEdgeBox:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeBox_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsShapeEdgeBox::create(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsShapeEdgeBox>(tolua_S, "ax.PhysicsShapeEdgeBox",(ax::PhysicsShapeEdgeBox*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsShapeEdgeBox:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgeBox_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsShapeEdgeBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeEdgeBox)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeEdgeBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeEdgeBox");
    tolua_cclass(tolua_S,"PhysicsShapeEdgeBox","ax.PhysicsShapeEdgeBox","ax.PhysicsShapeEdgePolygon",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeEdgeBox");
        tolua_function(tolua_S,"create", lua_ax_physics_PhysicsShapeEdgeBox_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeEdgeBox).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeEdgeBox";
    g_typeCast[typeName] = "ax.PhysicsShapeEdgeBox";
    return 1;
}

int lua_ax_physics_PhysicsShapeEdgeChain_getPointsCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsShapeEdgeChain* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsShapeEdgeChain",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsShapeEdgeChain*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsShapeEdgeChain_getPointsCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsShapeEdgeChain_getPointsCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPointsCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsShapeEdgeChain:getPointsCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsShapeEdgeChain_getPointsCount'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsShapeEdgeChain_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsShapeEdgeChain)");
    return 0;
}

int lua_register_ax_physics_PhysicsShapeEdgeChain(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsShapeEdgeChain");
    tolua_cclass(tolua_S,"PhysicsShapeEdgeChain","ax.PhysicsShapeEdgeChain","ax.PhysicsShape",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsShapeEdgeChain");
        tolua_function(tolua_S,"getPointsCount",lua_ax_physics_PhysicsShapeEdgeChain_getPointsCount);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsShapeEdgeChain).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsShapeEdgeChain";
    g_typeCast[typeName] = "ax.PhysicsShapeEdgeChain";
    return 1;
}

int lua_ax_physics_PhysicsBody_addShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_addShape'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PhysicsShape* arg0;

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.PhysicsBody:addShape");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_addShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addShape(arg0);
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    if (argc == 2) 
    {
        ax::PhysicsShape* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.PhysicsBody:addShape");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.PhysicsBody:addShape");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_addShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addShape(arg0, arg1);
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:addShape",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_addShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_removeShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_removeShape'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            cobj->removeShape(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            cobj->removeShape(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::PhysicsShape* arg0;
            ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            cobj->removeShape(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            ax::PhysicsShape* arg0;
            ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.PhysicsBody:removeShape");

            if (!ok) { break; }
            cobj->removeShape(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.PhysicsBody:removeShape",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_removeShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_removeAllShapes(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_removeAllShapes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_removeAllShapes'", nullptr);
            return 0;
        }
        cobj->removeAllShapes();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:removeAllShapes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_removeAllShapes'", nullptr);
            return 0;
        }
        cobj->removeAllShapes(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:removeAllShapes",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_removeAllShapes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getShapes(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getShapes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getShapes'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShapes();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getShapes",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getShapes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getFirstShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getFirstShape'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getFirstShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFirstShape();
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getFirstShape",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getFirstShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getShape'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:getShape");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShape(arg0);
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getShape",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_applyForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_applyForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_applyForce'", nullptr);
            return 0;
        }
        cobj->applyForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:applyForce");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsBody:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_applyForce'", nullptr);
            return 0;
        }
        cobj->applyForce(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:applyForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_applyForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_resetForces(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_resetForces'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_resetForces'", nullptr);
            return 0;
        }
        cobj->resetForces();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:resetForces",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_resetForces'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_applyImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_applyImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_applyImpulse'", nullptr);
            return 0;
        }
        cobj->applyImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:applyImpulse");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsBody:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_applyImpulse'", nullptr);
            return 0;
        }
        cobj->applyImpulse(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:applyImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_applyImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_applyTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_applyTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:applyTorque");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_applyTorque'", nullptr);
            return 0;
        }
        cobj->applyTorque(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:applyTorque",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_applyTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:setVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setVelocity'", nullptr);
            return 0;
        }
        cobj->setVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVelocity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setAngularVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setAngularVelocity'", nullptr);
            return 0;
        }
        cobj->setAngularVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setAngularVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getVelocityAtLocalPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getVelocityAtLocalPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:getVelocityAtLocalPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getVelocityAtLocalPoint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVelocityAtLocalPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getVelocityAtLocalPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getVelocityAtLocalPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getVelocityAtWorldPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getVelocityAtWorldPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:getVelocityAtWorldPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getVelocityAtWorldPoint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVelocityAtWorldPoint(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getVelocityAtWorldPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getVelocityAtWorldPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getAngularVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getAngularVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setVelocityLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setVelocityLimit'", nullptr);
            return 0;
        }
        cobj->setVelocityLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setVelocityLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getVelocityLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVelocityLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getVelocityLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setAngularVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setAngularVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setAngularVelocityLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setAngularVelocityLimit'", nullptr);
            return 0;
        }
        cobj->setAngularVelocityLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setAngularVelocityLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setAngularVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getAngularVelocityLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getAngularVelocityLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getAngularVelocityLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularVelocityLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getAngularVelocityLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getAngularVelocityLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_removeFromWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_removeFromWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_removeFromWorld'", nullptr);
            return 0;
        }
        cobj->removeFromWorld();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:removeFromWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_removeFromWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getWorld();
        object_to_luaval<ax::PhysicsWorld>(tolua_S, "ax.PhysicsWorld",(ax::PhysicsWorld*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getNode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setCategoryBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setCategoryBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:setCategoryBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setCategoryBitmask'", nullptr);
            return 0;
        }
        cobj->setCategoryBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setCategoryBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setCategoryBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setContactTestBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setContactTestBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:setContactTestBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setContactTestBitmask'", nullptr);
            return 0;
        }
        cobj->setContactTestBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setContactTestBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setContactTestBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setCollisionBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setCollisionBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:setCollisionBitmask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setCollisionBitmask'", nullptr);
            return 0;
        }
        cobj->setCollisionBitmask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setCollisionBitmask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setCollisionBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getCategoryBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getCategoryBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getCategoryBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCategoryBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getCategoryBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getCategoryBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getContactTestBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getContactTestBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getContactTestBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContactTestBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getContactTestBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getContactTestBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getCollisionBitmask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getCollisionBitmask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getCollisionBitmask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCollisionBitmask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getCollisionBitmask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getCollisionBitmask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:setGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setGroup'", nullptr);
            return 0;
        }
        cobj->setGroup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setGroup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getGroup'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGroup();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRotation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setPositionOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setPositionOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:setPositionOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setPositionOffset'", nullptr);
            return 0;
        }
        cobj->setPositionOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setPositionOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setPositionOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getPositionOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getPositionOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getPositionOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPositionOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getPositionOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getPositionOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setRotationOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setRotationOffset'", nullptr);
            return 0;
        }
        cobj->setRotationOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setRotationOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getRotationOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRotationOffset();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getRotationOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_isDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_isDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_isDynamic'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDynamic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:isDynamic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_isDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setDynamic(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setDynamic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:setDynamic");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setDynamic'", nullptr);
            return 0;
        }
        cobj->setDynamic(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setDynamic",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setDynamic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setMass'", nullptr);
            return 0;
        }
        cobj->setMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getMass'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMass();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_addMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_addMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:addMass");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_addMass'", nullptr);
            return 0;
        }
        cobj->addMass(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:addMass",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_addMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setMoment'", nullptr);
            return 0;
        }
        cobj->setMoment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setMoment",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getMoment'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMoment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getMoment",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_addMoment(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_addMoment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:addMoment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_addMoment'", nullptr);
            return 0;
        }
        cobj->addMoment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:addMoment",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_addMoment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getLinearDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getLinearDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setLinearDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setLinearDamping'", nullptr);
            return 0;
        }
        cobj->setLinearDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setLinearDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getAngularDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getAngularDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:setAngularDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setAngularDamping'", nullptr);
            return 0;
        }
        cobj->setAngularDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setAngularDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_isResting(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_isResting'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_isResting'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isResting();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:isResting",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_isResting'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setResting(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setResting'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:setResting");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setResting'", nullptr);
            return 0;
        }
        cobj->setResting(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setResting",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setResting'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_isRotationEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_isRotationEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_isRotationEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isRotationEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:isRotationEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_isRotationEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setRotationEnable(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setRotationEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:setRotationEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setRotationEnable'", nullptr);
            return 0;
        }
        cobj->setRotationEnable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setRotationEnable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setRotationEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_isGravityEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_isGravityEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_isGravityEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isGravityEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:isGravityEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_isGravityEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setGravityEnable(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setGravityEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:setGravityEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setGravityEnable'", nullptr);
            return 0;
        }
        cobj->setGravityEnable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setGravityEnable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setGravityEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getTag'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsBody:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setTag'", nullptr);
            return 0;
        }
        cobj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_world2Local(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_world2Local'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:world2Local");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_world2Local'", nullptr);
            return 0;
        }
        auto&& ret = cobj->world2Local(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:world2Local",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_world2Local'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_local2World(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_local2World'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:local2World");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_local2World'", nullptr);
            return 0;
        }
        auto&& ret = cobj->local2World(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:local2World",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_local2World'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_getCPBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_getCPBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_getCPBody'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCPBody();
        #pragma warning NO CONVERSION FROM NATIVE FOR cpBody*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:getCPBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_getCPBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_setFixedUpdate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsBody_setFixedUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsBody:setFixedUpdate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_setFixedUpdate'", nullptr);
            return 0;
        }
        cobj->setFixedUpdate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsBody:setFixedUpdate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_setFixedUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsBody_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:create");
            if (!ok) { break; }
            ax::PhysicsBody* ret = ax::PhysicsBody::create(arg0);
            object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            ax::PhysicsBody* ret = ax::PhysicsBody::create();
            object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:create");
            if (!ok) { break; }
            double arg1;
            ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.PhysicsBody:create");
            if (!ok) { break; }
            ax::PhysicsBody* ret = ax::PhysicsBody::create(arg0, arg1);
            object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.PhysicsBody:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsBody_createCircle(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createCircle(arg0);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 2)
    {
        double arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:createCircle");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createCircle(arg0, arg1);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 3)
    {
        double arg0;
        ax::PhysicsMaterial arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsBody:createCircle");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createCircle");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsBody:createCircle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createCircle'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createCircle(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsBody:createCircle",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_createCircle'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsBody_createBox(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createBox(arg0);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createBox");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createBox(arg0, arg1);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createBox");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createBox");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsBody:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createBox(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsBody:createBox",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_createBox'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsBody_createEdgeSegment(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeSegment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeSegment'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeSegment(arg0, arg1);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.PhysicsBody:createEdgeSegment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeSegment'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeSegment(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        ax::PhysicsMaterial arg2;
        double arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_physics_material(tolua_S, 4, &arg2, "ax.PhysicsBody:createEdgeSegment");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsBody:createEdgeSegment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeSegment'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeSegment(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsBody:createEdgeSegment",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_createEdgeSegment'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics_PhysicsBody_createEdgeBox(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsBody",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec2 arg0;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeBox(arg0);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeBox(arg0, arg1);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        double arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsBody:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeBox(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    if (argc == 4)
    {
        ax::Vec2 arg0;
        ax::PhysicsMaterial arg1;
        double arg2;
        ax::Vec2 arg3;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_physics_material(tolua_S, 3, &arg1, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsBody:createEdgeBox");
        ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsBody:createEdgeBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsBody_createEdgeBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsBody::createEdgeBox(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsBody:createEdgeBox",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsBody_createEdgeBox'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsBody_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsBody)");
    return 0;
}

int lua_register_ax_physics_PhysicsBody(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsBody");
    tolua_cclass(tolua_S,"PhysicsBody","ax.PhysicsBody","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsBody");
        tolua_function(tolua_S,"addShape",lua_ax_physics_PhysicsBody_addShape);
        tolua_function(tolua_S,"removeShape",lua_ax_physics_PhysicsBody_removeShape);
        tolua_function(tolua_S,"removeAllShapes",lua_ax_physics_PhysicsBody_removeAllShapes);
        tolua_function(tolua_S,"getShapes",lua_ax_physics_PhysicsBody_getShapes);
        tolua_function(tolua_S,"getFirstShape",lua_ax_physics_PhysicsBody_getFirstShape);
        tolua_function(tolua_S,"getShape",lua_ax_physics_PhysicsBody_getShape);
        tolua_function(tolua_S,"applyForce",lua_ax_physics_PhysicsBody_applyForce);
        tolua_function(tolua_S,"resetForces",lua_ax_physics_PhysicsBody_resetForces);
        tolua_function(tolua_S,"applyImpulse",lua_ax_physics_PhysicsBody_applyImpulse);
        tolua_function(tolua_S,"applyTorque",lua_ax_physics_PhysicsBody_applyTorque);
        tolua_function(tolua_S,"setVelocity",lua_ax_physics_PhysicsBody_setVelocity);
        tolua_function(tolua_S,"getVelocity",lua_ax_physics_PhysicsBody_getVelocity);
        tolua_function(tolua_S,"setAngularVelocity",lua_ax_physics_PhysicsBody_setAngularVelocity);
        tolua_function(tolua_S,"getVelocityAtLocalPoint",lua_ax_physics_PhysicsBody_getVelocityAtLocalPoint);
        tolua_function(tolua_S,"getVelocityAtWorldPoint",lua_ax_physics_PhysicsBody_getVelocityAtWorldPoint);
        tolua_function(tolua_S,"getAngularVelocity",lua_ax_physics_PhysicsBody_getAngularVelocity);
        tolua_function(tolua_S,"setVelocityLimit",lua_ax_physics_PhysicsBody_setVelocityLimit);
        tolua_function(tolua_S,"getVelocityLimit",lua_ax_physics_PhysicsBody_getVelocityLimit);
        tolua_function(tolua_S,"setAngularVelocityLimit",lua_ax_physics_PhysicsBody_setAngularVelocityLimit);
        tolua_function(tolua_S,"getAngularVelocityLimit",lua_ax_physics_PhysicsBody_getAngularVelocityLimit);
        tolua_function(tolua_S,"removeFromWorld",lua_ax_physics_PhysicsBody_removeFromWorld);
        tolua_function(tolua_S,"getWorld",lua_ax_physics_PhysicsBody_getWorld);
        tolua_function(tolua_S,"getNode",lua_ax_physics_PhysicsBody_getNode);
        tolua_function(tolua_S,"setCategoryBitmask",lua_ax_physics_PhysicsBody_setCategoryBitmask);
        tolua_function(tolua_S,"setContactTestBitmask",lua_ax_physics_PhysicsBody_setContactTestBitmask);
        tolua_function(tolua_S,"setCollisionBitmask",lua_ax_physics_PhysicsBody_setCollisionBitmask);
        tolua_function(tolua_S,"getCategoryBitmask",lua_ax_physics_PhysicsBody_getCategoryBitmask);
        tolua_function(tolua_S,"getContactTestBitmask",lua_ax_physics_PhysicsBody_getContactTestBitmask);
        tolua_function(tolua_S,"getCollisionBitmask",lua_ax_physics_PhysicsBody_getCollisionBitmask);
        tolua_function(tolua_S,"setGroup",lua_ax_physics_PhysicsBody_setGroup);
        tolua_function(tolua_S,"getGroup",lua_ax_physics_PhysicsBody_getGroup);
        tolua_function(tolua_S,"getPosition",lua_ax_physics_PhysicsBody_getPosition);
        tolua_function(tolua_S,"getRotation",lua_ax_physics_PhysicsBody_getRotation);
        tolua_function(tolua_S,"setPositionOffset",lua_ax_physics_PhysicsBody_setPositionOffset);
        tolua_function(tolua_S,"getPositionOffset",lua_ax_physics_PhysicsBody_getPositionOffset);
        tolua_function(tolua_S,"setRotationOffset",lua_ax_physics_PhysicsBody_setRotationOffset);
        tolua_function(tolua_S,"getRotationOffset",lua_ax_physics_PhysicsBody_getRotationOffset);
        tolua_function(tolua_S,"isDynamic",lua_ax_physics_PhysicsBody_isDynamic);
        tolua_function(tolua_S,"setDynamic",lua_ax_physics_PhysicsBody_setDynamic);
        tolua_function(tolua_S,"setMass",lua_ax_physics_PhysicsBody_setMass);
        tolua_function(tolua_S,"getMass",lua_ax_physics_PhysicsBody_getMass);
        tolua_function(tolua_S,"addMass",lua_ax_physics_PhysicsBody_addMass);
        tolua_function(tolua_S,"setMoment",lua_ax_physics_PhysicsBody_setMoment);
        tolua_function(tolua_S,"getMoment",lua_ax_physics_PhysicsBody_getMoment);
        tolua_function(tolua_S,"addMoment",lua_ax_physics_PhysicsBody_addMoment);
        tolua_function(tolua_S,"getLinearDamping",lua_ax_physics_PhysicsBody_getLinearDamping);
        tolua_function(tolua_S,"setLinearDamping",lua_ax_physics_PhysicsBody_setLinearDamping);
        tolua_function(tolua_S,"getAngularDamping",lua_ax_physics_PhysicsBody_getAngularDamping);
        tolua_function(tolua_S,"setAngularDamping",lua_ax_physics_PhysicsBody_setAngularDamping);
        tolua_function(tolua_S,"isResting",lua_ax_physics_PhysicsBody_isResting);
        tolua_function(tolua_S,"setResting",lua_ax_physics_PhysicsBody_setResting);
        tolua_function(tolua_S,"isRotationEnabled",lua_ax_physics_PhysicsBody_isRotationEnabled);
        tolua_function(tolua_S,"setRotationEnable",lua_ax_physics_PhysicsBody_setRotationEnable);
        tolua_function(tolua_S,"isGravityEnabled",lua_ax_physics_PhysicsBody_isGravityEnabled);
        tolua_function(tolua_S,"setGravityEnable",lua_ax_physics_PhysicsBody_setGravityEnable);
        tolua_function(tolua_S,"getTag",lua_ax_physics_PhysicsBody_getTag);
        tolua_function(tolua_S,"setTag",lua_ax_physics_PhysicsBody_setTag);
        tolua_function(tolua_S,"world2Local",lua_ax_physics_PhysicsBody_world2Local);
        tolua_function(tolua_S,"local2World",lua_ax_physics_PhysicsBody_local2World);
        tolua_function(tolua_S,"getCPBody",lua_ax_physics_PhysicsBody_getCPBody);
        tolua_function(tolua_S,"setFixedUpdate",lua_ax_physics_PhysicsBody_setFixedUpdate);
        tolua_function(tolua_S,"create", lua_ax_physics_PhysicsBody_create);
        tolua_function(tolua_S,"createCircle", lua_ax_physics_PhysicsBody_createCircle);
        tolua_function(tolua_S,"createBox", lua_ax_physics_PhysicsBody_createBox);
        tolua_function(tolua_S,"createEdgeSegment", lua_ax_physics_PhysicsBody_createEdgeSegment);
        tolua_function(tolua_S,"createEdgeBox", lua_ax_physics_PhysicsBody_createEdgeBox);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsBody).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsBody";
    g_typeCast[typeName] = "ax.PhysicsBody";
    return 1;
}

int lua_ax_physics_PhysicsContact_getShapeA(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContact* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContact*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContact_getShapeA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContact_getShapeA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShapeA();
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContact:getShapeA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContact_getShapeA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContact_getShapeB(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContact* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContact*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContact_getShapeB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContact_getShapeB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShapeB();
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContact:getShapeB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContact_getShapeB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContact_getContactData(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContact* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContact*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContact_getContactData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContact_getContactData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContactData();
        physics_contactdata_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContact:getContactData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContact_getContactData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContact_getPreContactData(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContact* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContact*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContact_getPreContactData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContact_getPreContactData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPreContactData();
        physics_contactdata_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContact:getPreContactData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContact_getPreContactData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContact_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContact* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContact*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContact_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContact_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContact:getEventCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContact_getEventCode'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsContact_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsContact)");
    return 0;
}

int lua_register_ax_physics_PhysicsContact(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsContact");
    tolua_cclass(tolua_S,"PhysicsContact","ax.PhysicsContact","ax.EventCustom",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsContact");
        tolua_function(tolua_S,"getShapeA",lua_ax_physics_PhysicsContact_getShapeA);
        tolua_function(tolua_S,"getShapeB",lua_ax_physics_PhysicsContact_getShapeB);
        tolua_function(tolua_S,"getContactData",lua_ax_physics_PhysicsContact_getContactData);
        tolua_function(tolua_S,"getPreContactData",lua_ax_physics_PhysicsContact_getPreContactData);
        tolua_function(tolua_S,"getEventCode",lua_ax_physics_PhysicsContact_getEventCode);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsContact).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsContact";
    g_typeCast[typeName] = "ax.PhysicsContact";
    return 1;
}

int lua_ax_physics_PhysicsContactPreSolve_getRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_getRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_getRestitution'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitution();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:getRestitution",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_getRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_getFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_getFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_getFriction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:getFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_getFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_getSurfaceVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_getSurfaceVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_getSurfaceVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSurfaceVelocity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:getSurfaceVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_getSurfaceVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_setRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_setRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsContactPreSolve:setRestitution");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_setRestitution'", nullptr);
            return 0;
        }
        cobj->setRestitution(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:setRestitution",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_setRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_setFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_setFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsContactPreSolve:setFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_setFriction'", nullptr);
            return 0;
        }
        cobj->setFriction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:setFriction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_setFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_setSurfaceVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_setSurfaceVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsContactPreSolve:setSurfaceVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_setSurfaceVelocity'", nullptr);
            return 0;
        }
        cobj->setSurfaceVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:setSurfaceVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_setSurfaceVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPreSolve_ignore(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPreSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPreSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPreSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPreSolve_ignore'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPreSolve_ignore'", nullptr);
            return 0;
        }
        cobj->ignore();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPreSolve:ignore",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPreSolve_ignore'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsContactPreSolve_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsContactPreSolve)");
    return 0;
}

int lua_register_ax_physics_PhysicsContactPreSolve(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsContactPreSolve");
    tolua_cclass(tolua_S,"PhysicsContactPreSolve","ax.PhysicsContactPreSolve","",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsContactPreSolve");
        tolua_function(tolua_S,"getRestitution",lua_ax_physics_PhysicsContactPreSolve_getRestitution);
        tolua_function(tolua_S,"getFriction",lua_ax_physics_PhysicsContactPreSolve_getFriction);
        tolua_function(tolua_S,"getSurfaceVelocity",lua_ax_physics_PhysicsContactPreSolve_getSurfaceVelocity);
        tolua_function(tolua_S,"setRestitution",lua_ax_physics_PhysicsContactPreSolve_setRestitution);
        tolua_function(tolua_S,"setFriction",lua_ax_physics_PhysicsContactPreSolve_setFriction);
        tolua_function(tolua_S,"setSurfaceVelocity",lua_ax_physics_PhysicsContactPreSolve_setSurfaceVelocity);
        tolua_function(tolua_S,"ignore",lua_ax_physics_PhysicsContactPreSolve_ignore);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsContactPreSolve).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsContactPreSolve";
    g_typeCast[typeName] = "ax.PhysicsContactPreSolve";
    return 1;
}

int lua_ax_physics_PhysicsContactPostSolve_getRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPostSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPostSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPostSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPostSolve_getRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPostSolve_getRestitution'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitution();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPostSolve:getRestitution",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPostSolve_getRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPostSolve_getFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPostSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPostSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPostSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPostSolve_getFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPostSolve_getFriction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPostSolve:getFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPostSolve_getFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsContactPostSolve_getSurfaceVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsContactPostSolve* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsContactPostSolve",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsContactPostSolve*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsContactPostSolve_getSurfaceVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsContactPostSolve_getSurfaceVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSurfaceVelocity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsContactPostSolve:getSurfaceVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsContactPostSolve_getSurfaceVelocity'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsContactPostSolve_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsContactPostSolve)");
    return 0;
}

int lua_register_ax_physics_PhysicsContactPostSolve(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsContactPostSolve");
    tolua_cclass(tolua_S,"PhysicsContactPostSolve","ax.PhysicsContactPostSolve","",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsContactPostSolve");
        tolua_function(tolua_S,"getRestitution",lua_ax_physics_PhysicsContactPostSolve_getRestitution);
        tolua_function(tolua_S,"getFriction",lua_ax_physics_PhysicsContactPostSolve_getFriction);
        tolua_function(tolua_S,"getSurfaceVelocity",lua_ax_physics_PhysicsContactPostSolve_getSurfaceVelocity);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsContactPostSolve).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsContactPostSolve";
    g_typeCast[typeName] = "ax.PhysicsContactPostSolve";
    return 1;
}

int lua_ax_physics_EventListenerPhysicsContact_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EventListenerPhysicsContact",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContact_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EventListenerPhysicsContact::create();
        object_to_luaval<ax::EventListenerPhysicsContact>(tolua_S, "ax.EventListenerPhysicsContact",(ax::EventListenerPhysicsContact*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EventListenerPhysicsContact:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContact_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_EventListenerPhysicsContact_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerPhysicsContact)");
    return 0;
}

int lua_register_ax_physics_EventListenerPhysicsContact(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerPhysicsContact");
    tolua_cclass(tolua_S,"EventListenerPhysicsContact","ax.EventListenerPhysicsContact","ax.EventListenerCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerPhysicsContact");
        tolua_function(tolua_S,"create", lua_ax_physics_EventListenerPhysicsContact_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventListenerPhysicsContact).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerPhysicsContact";
    g_typeCast[typeName] = "ax.EventListenerPhysicsContact";
    return 1;
}

int lua_ax_physics_EventListenerPhysicsContactWithBodies_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventListenerPhysicsContactWithBodies* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventListenerPhysicsContactWithBodies",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventListenerPhysicsContactWithBodies*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_EventListenerPhysicsContactWithBodies_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::PhysicsShape* arg0;
        ax::PhysicsShape* arg1;

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.EventListenerPhysicsContactWithBodies:hitTest");

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 3, "ax.PhysicsShape",&arg1, "ax.EventListenerPhysicsContactWithBodies:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithBodies_hitTest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hitTest(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventListenerPhysicsContactWithBodies:hitTest",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithBodies_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_EventListenerPhysicsContactWithBodies_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EventListenerPhysicsContactWithBodies",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.EventListenerPhysicsContactWithBodies:create");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.EventListenerPhysicsContactWithBodies:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithBodies_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EventListenerPhysicsContactWithBodies::create(arg0, arg1);
        object_to_luaval<ax::EventListenerPhysicsContactWithBodies>(tolua_S, "ax.EventListenerPhysicsContactWithBodies",(ax::EventListenerPhysicsContactWithBodies*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EventListenerPhysicsContactWithBodies:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithBodies_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_EventListenerPhysicsContactWithBodies_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerPhysicsContactWithBodies)");
    return 0;
}

int lua_register_ax_physics_EventListenerPhysicsContactWithBodies(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerPhysicsContactWithBodies");
    tolua_cclass(tolua_S,"EventListenerPhysicsContactWithBodies","ax.EventListenerPhysicsContactWithBodies","ax.EventListenerPhysicsContact",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerPhysicsContactWithBodies");
        tolua_function(tolua_S,"hitTest",lua_ax_physics_EventListenerPhysicsContactWithBodies_hitTest);
        tolua_function(tolua_S,"create", lua_ax_physics_EventListenerPhysicsContactWithBodies_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventListenerPhysicsContactWithBodies).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerPhysicsContactWithBodies";
    g_typeCast[typeName] = "ax.EventListenerPhysicsContactWithBodies";
    return 1;
}

int lua_ax_physics_EventListenerPhysicsContactWithShapes_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventListenerPhysicsContactWithShapes* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventListenerPhysicsContactWithShapes",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventListenerPhysicsContactWithShapes*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_EventListenerPhysicsContactWithShapes_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::PhysicsShape* arg0;
        ax::PhysicsShape* arg1;

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.EventListenerPhysicsContactWithShapes:hitTest");

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 3, "ax.PhysicsShape",&arg1, "ax.EventListenerPhysicsContactWithShapes:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithShapes_hitTest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hitTest(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventListenerPhysicsContactWithShapes:hitTest",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithShapes_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_EventListenerPhysicsContactWithShapes_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EventListenerPhysicsContactWithShapes",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::PhysicsShape* arg0;
        ax::PhysicsShape* arg1;
        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.EventListenerPhysicsContactWithShapes:create");
        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 3, "ax.PhysicsShape",&arg1, "ax.EventListenerPhysicsContactWithShapes:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithShapes_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EventListenerPhysicsContactWithShapes::create(arg0, arg1);
        object_to_luaval<ax::EventListenerPhysicsContactWithShapes>(tolua_S, "ax.EventListenerPhysicsContactWithShapes",(ax::EventListenerPhysicsContactWithShapes*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EventListenerPhysicsContactWithShapes:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithShapes_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_EventListenerPhysicsContactWithShapes_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerPhysicsContactWithShapes)");
    return 0;
}

int lua_register_ax_physics_EventListenerPhysicsContactWithShapes(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerPhysicsContactWithShapes");
    tolua_cclass(tolua_S,"EventListenerPhysicsContactWithShapes","ax.EventListenerPhysicsContactWithShapes","ax.EventListenerPhysicsContact",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerPhysicsContactWithShapes");
        tolua_function(tolua_S,"hitTest",lua_ax_physics_EventListenerPhysicsContactWithShapes_hitTest);
        tolua_function(tolua_S,"create", lua_ax_physics_EventListenerPhysicsContactWithShapes_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventListenerPhysicsContactWithShapes).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerPhysicsContactWithShapes";
    g_typeCast[typeName] = "ax.EventListenerPhysicsContactWithShapes";
    return 1;
}

int lua_ax_physics_EventListenerPhysicsContactWithGroup_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventListenerPhysicsContactWithGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventListenerPhysicsContactWithGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventListenerPhysicsContactWithGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_EventListenerPhysicsContactWithGroup_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::PhysicsShape* arg0;
        ax::PhysicsShape* arg1;

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 2, "ax.PhysicsShape",&arg0, "ax.EventListenerPhysicsContactWithGroup:hitTest");

        ok &= luaval_to_object<ax::PhysicsShape>(tolua_S, 3, "ax.PhysicsShape",&arg1, "ax.EventListenerPhysicsContactWithGroup:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithGroup_hitTest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hitTest(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventListenerPhysicsContactWithGroup:hitTest",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithGroup_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_EventListenerPhysicsContactWithGroup_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EventListenerPhysicsContactWithGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.EventListenerPhysicsContactWithGroup:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_EventListenerPhysicsContactWithGroup_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EventListenerPhysicsContactWithGroup::create(arg0);
        object_to_luaval<ax::EventListenerPhysicsContactWithGroup>(tolua_S, "ax.EventListenerPhysicsContactWithGroup",(ax::EventListenerPhysicsContactWithGroup*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EventListenerPhysicsContactWithGroup:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_EventListenerPhysicsContactWithGroup_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_EventListenerPhysicsContactWithGroup_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerPhysicsContactWithGroup)");
    return 0;
}

int lua_register_ax_physics_EventListenerPhysicsContactWithGroup(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerPhysicsContactWithGroup");
    tolua_cclass(tolua_S,"EventListenerPhysicsContactWithGroup","ax.EventListenerPhysicsContactWithGroup","ax.EventListenerPhysicsContact",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerPhysicsContactWithGroup");
        tolua_function(tolua_S,"hitTest",lua_ax_physics_EventListenerPhysicsContactWithGroup_hitTest);
        tolua_function(tolua_S,"create", lua_ax_physics_EventListenerPhysicsContactWithGroup_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventListenerPhysicsContactWithGroup).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerPhysicsContactWithGroup";
    g_typeCast[typeName] = "ax.EventListenerPhysicsContactWithGroup";
    return 1;
}

int lua_ax_physics_PhysicsJoint_getBodyA(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_getBodyA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_getBodyA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBodyA();
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:getBodyA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_getBodyA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_getBodyB(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_getBodyB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_getBodyB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBodyB();
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:getBodyB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_getBodyB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_getWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_getWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_getWorld'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getWorld();
        object_to_luaval<ax::PhysicsWorld>(tolua_S, "ax.PhysicsWorld",(ax::PhysicsWorld*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:getWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_getWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_getTag'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsJoint:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_setTag'", nullptr);
            return 0;
        }
        cobj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_isEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_isEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_isEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:isEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_isEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_setEnable(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_setEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsJoint:setEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_setEnable'", nullptr);
            return 0;
        }
        cobj->setEnable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:setEnable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_setEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_isCollisionEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_isCollisionEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_isCollisionEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isCollisionEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:isCollisionEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_isCollisionEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_setCollisionEnable(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_setCollisionEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsJoint:setCollisionEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_setCollisionEnable'", nullptr);
            return 0;
        }
        cobj->setCollisionEnable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:setCollisionEnable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_setCollisionEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_removeFormWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_removeFormWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_removeFormWorld'", nullptr);
            return 0;
        }
        cobj->removeFormWorld();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:removeFormWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_removeFormWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_setMaxForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_setMaxForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJoint:setMaxForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_setMaxForce'", nullptr);
            return 0;
        }
        cobj->setMaxForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:setMaxForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_setMaxForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJoint_getMaxForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJoint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJoint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJoint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJoint_getMaxForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJoint_getMaxForce'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxForce();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJoint:getMaxForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJoint_getMaxForce'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsJoint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJoint)");
    return 0;
}

int lua_register_ax_physics_PhysicsJoint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJoint");
    tolua_cclass(tolua_S,"PhysicsJoint","ax.PhysicsJoint","",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJoint");
        tolua_function(tolua_S,"getBodyA",lua_ax_physics_PhysicsJoint_getBodyA);
        tolua_function(tolua_S,"getBodyB",lua_ax_physics_PhysicsJoint_getBodyB);
        tolua_function(tolua_S,"getWorld",lua_ax_physics_PhysicsJoint_getWorld);
        tolua_function(tolua_S,"getTag",lua_ax_physics_PhysicsJoint_getTag);
        tolua_function(tolua_S,"setTag",lua_ax_physics_PhysicsJoint_setTag);
        tolua_function(tolua_S,"isEnabled",lua_ax_physics_PhysicsJoint_isEnabled);
        tolua_function(tolua_S,"setEnable",lua_ax_physics_PhysicsJoint_setEnable);
        tolua_function(tolua_S,"isCollisionEnabled",lua_ax_physics_PhysicsJoint_isCollisionEnabled);
        tolua_function(tolua_S,"setCollisionEnable",lua_ax_physics_PhysicsJoint_setCollisionEnable);
        tolua_function(tolua_S,"removeFormWorld",lua_ax_physics_PhysicsJoint_removeFormWorld);
        tolua_function(tolua_S,"setMaxForce",lua_ax_physics_PhysicsJoint_setMaxForce);
        tolua_function(tolua_S,"getMaxForce",lua_ax_physics_PhysicsJoint_getMaxForce);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJoint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJoint";
    g_typeCast[typeName] = "ax.PhysicsJoint";
    return 1;
}

int lua_ax_physics_PhysicsJointFixed_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointFixed* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointFixed",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointFixed*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointFixed_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointFixed_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointFixed:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointFixed_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointFixed_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointFixed",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        ax::Vec2 arg2;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointFixed:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointFixed:construct");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointFixed:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointFixed_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointFixed::construct(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsJointFixed>(tolua_S, "ax.PhysicsJointFixed",(ax::PhysicsJointFixed*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointFixed:construct",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointFixed_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointFixed_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointFixed)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointFixed(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointFixed");
    tolua_cclass(tolua_S,"PhysicsJointFixed","ax.PhysicsJointFixed","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointFixed");
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointFixed_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointFixed_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointFixed).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointFixed";
    g_typeCast[typeName] = "ax.PhysicsJointFixed";
    return 1;
}

int lua_ax_physics_PhysicsJointLimit_getAnchr1(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_getAnchr1'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_getAnchr1'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAnchr1();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:getAnchr1",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_getAnchr1'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_setAnchr1(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_setAnchr1'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointLimit:setAnchr1");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_setAnchr1'", nullptr);
            return 0;
        }
        cobj->setAnchr1(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:setAnchr1",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_setAnchr1'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_getAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_getAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_getAnchr2'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAnchr2();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:getAnchr2",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_getAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_setAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_setAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointLimit:setAnchr2");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_setAnchr2'", nullptr);
            return 0;
        }
        cobj->setAnchr2(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:setAnchr2",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_setAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_getMin(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_getMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_getMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:getMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_getMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_setMin(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_setMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointLimit:setMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_setMin'", nullptr);
            return 0;
        }
        cobj->setMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:setMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_setMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_getMax(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_getMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_getMax'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMax();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:getMax",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_getMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_setMax(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_setMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointLimit:setMax");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_setMax'", nullptr);
            return 0;
        }
        cobj->setMax(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:setMax",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_setMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointLimit_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointLimit_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointLimit:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointLimit_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 6)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::Vec2 arg2;
            ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::Vec2 arg3;
            ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            double arg4;
            ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            double arg5;
            ok &= luaval_to_number(tolua_S, 7,&arg5, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::PhysicsJointLimit* ret = ax::PhysicsJointLimit::construct(arg0, arg1, arg2, arg3, arg4, arg5);
            object_to_luaval<ax::PhysicsJointLimit>(tolua_S, "ax.PhysicsJointLimit",(ax::PhysicsJointLimit*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::Vec2 arg2;
            ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::Vec2 arg3;
            ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointLimit:construct");
            if (!ok) { break; }
            ax::PhysicsJointLimit* ret = ax::PhysicsJointLimit::construct(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::PhysicsJointLimit>(tolua_S, "ax.PhysicsJointLimit",(ax::PhysicsJointLimit*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.PhysicsJointLimit:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointLimit_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointLimit_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointLimit)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointLimit(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointLimit");
    tolua_cclass(tolua_S,"PhysicsJointLimit","ax.PhysicsJointLimit","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointLimit");
        tolua_function(tolua_S,"getAnchr1",lua_ax_physics_PhysicsJointLimit_getAnchr1);
        tolua_function(tolua_S,"setAnchr1",lua_ax_physics_PhysicsJointLimit_setAnchr1);
        tolua_function(tolua_S,"getAnchr2",lua_ax_physics_PhysicsJointLimit_getAnchr2);
        tolua_function(tolua_S,"setAnchr2",lua_ax_physics_PhysicsJointLimit_setAnchr2);
        tolua_function(tolua_S,"getMin",lua_ax_physics_PhysicsJointLimit_getMin);
        tolua_function(tolua_S,"setMin",lua_ax_physics_PhysicsJointLimit_setMin);
        tolua_function(tolua_S,"getMax",lua_ax_physics_PhysicsJointLimit_getMax);
        tolua_function(tolua_S,"setMax",lua_ax_physics_PhysicsJointLimit_setMax);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointLimit_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointLimit_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointLimit).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointLimit";
    g_typeCast[typeName] = "ax.PhysicsJointLimit";
    return 1;
}

int lua_ax_physics_PhysicsJointPin_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointPin* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointPin",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointPin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointPin_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointPin_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointPin:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointPin_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointPin_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointPin",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 4)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::Vec2 arg2;
            ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::Vec2 arg3;
            ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::PhysicsJointPin* ret = ax::PhysicsJointPin::construct(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::PhysicsJointPin>(tolua_S, "ax.PhysicsJointPin",(ax::PhysicsJointPin*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::Vec2 arg2;
            ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointPin:construct");
            if (!ok) { break; }
            ax::PhysicsJointPin* ret = ax::PhysicsJointPin::construct(arg0, arg1, arg2);
            object_to_luaval<ax::PhysicsJointPin>(tolua_S, "ax.PhysicsJointPin",(ax::PhysicsJointPin*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.PhysicsJointPin:construct",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointPin_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointPin_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointPin)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointPin(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointPin");
    tolua_cclass(tolua_S,"PhysicsJointPin","ax.PhysicsJointPin","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointPin");
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointPin_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointPin_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointPin).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointPin";
    g_typeCast[typeName] = "ax.PhysicsJointPin";
    return 1;
}

int lua_ax_physics_PhysicsJointDistance_getDistance(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointDistance* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointDistance",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointDistance*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointDistance_getDistance'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointDistance_getDistance'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDistance();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointDistance:getDistance",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointDistance_getDistance'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointDistance_setDistance(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointDistance* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointDistance",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointDistance*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointDistance_setDistance'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointDistance:setDistance");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointDistance_setDistance'", nullptr);
            return 0;
        }
        cobj->setDistance(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointDistance:setDistance",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointDistance_setDistance'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointDistance_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointDistance* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointDistance",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointDistance*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointDistance_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointDistance_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointDistance:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointDistance_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointDistance_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointDistance",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        ax::Vec2 arg2;
        ax::Vec2 arg3;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointDistance:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointDistance:construct");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointDistance:construct");
        ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointDistance:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointDistance_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointDistance::construct(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsJointDistance>(tolua_S, "ax.PhysicsJointDistance",(ax::PhysicsJointDistance*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointDistance:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointDistance_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointDistance_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointDistance)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointDistance(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointDistance");
    tolua_cclass(tolua_S,"PhysicsJointDistance","ax.PhysicsJointDistance","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointDistance");
        tolua_function(tolua_S,"getDistance",lua_ax_physics_PhysicsJointDistance_getDistance);
        tolua_function(tolua_S,"setDistance",lua_ax_physics_PhysicsJointDistance_setDistance);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointDistance_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointDistance_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointDistance).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointDistance";
    g_typeCast[typeName] = "ax.PhysicsJointDistance";
    return 1;
}

int lua_ax_physics_PhysicsJointSpring_getAnchr1(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_getAnchr1'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_getAnchr1'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAnchr1();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:getAnchr1",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_getAnchr1'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_setAnchr1(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_setAnchr1'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointSpring:setAnchr1");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_setAnchr1'", nullptr);
            return 0;
        }
        cobj->setAnchr1(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:setAnchr1",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_setAnchr1'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_getAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_getAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_getAnchr2'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAnchr2();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:getAnchr2",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_getAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_setAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_setAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointSpring:setAnchr2");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_setAnchr2'", nullptr);
            return 0;
        }
        cobj->setAnchr2(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:setAnchr2",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_setAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_getRestLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_getRestLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_getRestLength'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:getRestLength",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_getRestLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_setRestLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_setRestLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointSpring:setRestLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_setRestLength'", nullptr);
            return 0;
        }
        cobj->setRestLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:setRestLength",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_setRestLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_getStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_getStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_getStiffness'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getStiffness();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:getStiffness",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_getStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_setStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_setStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointSpring:setStiffness");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_setStiffness'", nullptr);
            return 0;
        }
        cobj->setStiffness(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:setStiffness",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_setStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointSpring:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_setDamping'", nullptr);
            return 0;
        }
        cobj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointSpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointSpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointSpring_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointSpring:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointSpring_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointSpring",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 6)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        ax::Vec2 arg2;
        ax::Vec2 arg3;
        double arg4;
        double arg5;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointSpring:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointSpring:construct");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointSpring:construct");
        ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointSpring:construct");
        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.PhysicsJointSpring:construct");
        ok &= luaval_to_number(tolua_S, 7,&arg5, "ax.PhysicsJointSpring:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointSpring_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointSpring::construct(arg0, arg1, arg2, arg3, arg4, arg5);
        object_to_luaval<ax::PhysicsJointSpring>(tolua_S, "ax.PhysicsJointSpring",(ax::PhysicsJointSpring*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointSpring:construct",argc, 6);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointSpring_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointSpring_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointSpring)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointSpring(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointSpring");
    tolua_cclass(tolua_S,"PhysicsJointSpring","ax.PhysicsJointSpring","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointSpring");
        tolua_function(tolua_S,"getAnchr1",lua_ax_physics_PhysicsJointSpring_getAnchr1);
        tolua_function(tolua_S,"setAnchr1",lua_ax_physics_PhysicsJointSpring_setAnchr1);
        tolua_function(tolua_S,"getAnchr2",lua_ax_physics_PhysicsJointSpring_getAnchr2);
        tolua_function(tolua_S,"setAnchr2",lua_ax_physics_PhysicsJointSpring_setAnchr2);
        tolua_function(tolua_S,"getRestLength",lua_ax_physics_PhysicsJointSpring_getRestLength);
        tolua_function(tolua_S,"setRestLength",lua_ax_physics_PhysicsJointSpring_setRestLength);
        tolua_function(tolua_S,"getStiffness",lua_ax_physics_PhysicsJointSpring_getStiffness);
        tolua_function(tolua_S,"setStiffness",lua_ax_physics_PhysicsJointSpring_setStiffness);
        tolua_function(tolua_S,"getDamping",lua_ax_physics_PhysicsJointSpring_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics_PhysicsJointSpring_setDamping);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointSpring_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointSpring_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointSpring).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointSpring";
    g_typeCast[typeName] = "ax.PhysicsJointSpring";
    return 1;
}

int lua_ax_physics_PhysicsJointGroove_getGrooveA(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_getGrooveA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_getGrooveA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGrooveA();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:getGrooveA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_getGrooveA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_setGrooveA(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_setGrooveA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointGroove:setGrooveA");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_setGrooveA'", nullptr);
            return 0;
        }
        cobj->setGrooveA(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:setGrooveA",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_setGrooveA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_getGrooveB(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_getGrooveB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_getGrooveB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGrooveB();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:getGrooveB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_getGrooveB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_setGrooveB(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_setGrooveB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointGroove:setGrooveB");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_setGrooveB'", nullptr);
            return 0;
        }
        cobj->setGrooveB(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:setGrooveB",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_setGrooveB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_getAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_getAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_getAnchr2'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAnchr2();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:getAnchr2",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_getAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_setAnchr2(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_setAnchr2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsJointGroove:setAnchr2");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_setAnchr2'", nullptr);
            return 0;
        }
        cobj->setAnchr2(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:setAnchr2",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_setAnchr2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGroove* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGroove*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGroove_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGroove:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGroove_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointGroove",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 5)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        ax::Vec2 arg2;
        ax::Vec2 arg3;
        ax::Vec2 arg4;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointGroove:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointGroove:construct");
        ok &= luaval_to_vec2(tolua_S, 4, &arg2, "ax.PhysicsJointGroove:construct");
        ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ax.PhysicsJointGroove:construct");
        ok &= luaval_to_vec2(tolua_S, 6, &arg4, "ax.PhysicsJointGroove:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGroove_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointGroove::construct(arg0, arg1, arg2, arg3, arg4);
        object_to_luaval<ax::PhysicsJointGroove>(tolua_S, "ax.PhysicsJointGroove",(ax::PhysicsJointGroove*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointGroove:construct",argc, 5);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGroove_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointGroove_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointGroove)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointGroove(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointGroove");
    tolua_cclass(tolua_S,"PhysicsJointGroove","ax.PhysicsJointGroove","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointGroove");
        tolua_function(tolua_S,"getGrooveA",lua_ax_physics_PhysicsJointGroove_getGrooveA);
        tolua_function(tolua_S,"setGrooveA",lua_ax_physics_PhysicsJointGroove_setGrooveA);
        tolua_function(tolua_S,"getGrooveB",lua_ax_physics_PhysicsJointGroove_getGrooveB);
        tolua_function(tolua_S,"setGrooveB",lua_ax_physics_PhysicsJointGroove_setGrooveB);
        tolua_function(tolua_S,"getAnchr2",lua_ax_physics_PhysicsJointGroove_getAnchr2);
        tolua_function(tolua_S,"setAnchr2",lua_ax_physics_PhysicsJointGroove_setAnchr2);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointGroove_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointGroove_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointGroove).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointGroove";
    g_typeCast[typeName] = "ax.PhysicsJointGroove";
    return 1;
}

int lua_ax_physics_PhysicsJointRotarySpring_getRestAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_getRestAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_getRestAngle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:getRestAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_getRestAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_setRestAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_setRestAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRotarySpring:setRestAngle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_setRestAngle'", nullptr);
            return 0;
        }
        cobj->setRestAngle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:setRestAngle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_setRestAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_getStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_getStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_getStiffness'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getStiffness();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:getStiffness",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_getStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_setStiffness(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_setStiffness'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRotarySpring:setStiffness");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_setStiffness'", nullptr);
            return 0;
        }
        cobj->setStiffness(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:setStiffness",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_setStiffness'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_getDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_getDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_getDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:getDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_getDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRotarySpring:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_setDamping'", nullptr);
            return 0;
        }
        cobj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotarySpring* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotarySpring*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotarySpring_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotarySpring:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotarySpring_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointRotarySpring",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        double arg2;
        double arg3;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointRotarySpring:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointRotarySpring:construct");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsJointRotarySpring:construct");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsJointRotarySpring:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotarySpring_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointRotarySpring::construct(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsJointRotarySpring>(tolua_S, "ax.PhysicsJointRotarySpring",(ax::PhysicsJointRotarySpring*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointRotarySpring:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotarySpring_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointRotarySpring_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointRotarySpring)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointRotarySpring(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointRotarySpring");
    tolua_cclass(tolua_S,"PhysicsJointRotarySpring","ax.PhysicsJointRotarySpring","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointRotarySpring");
        tolua_function(tolua_S,"getRestAngle",lua_ax_physics_PhysicsJointRotarySpring_getRestAngle);
        tolua_function(tolua_S,"setRestAngle",lua_ax_physics_PhysicsJointRotarySpring_setRestAngle);
        tolua_function(tolua_S,"getStiffness",lua_ax_physics_PhysicsJointRotarySpring_getStiffness);
        tolua_function(tolua_S,"setStiffness",lua_ax_physics_PhysicsJointRotarySpring_setStiffness);
        tolua_function(tolua_S,"getDamping",lua_ax_physics_PhysicsJointRotarySpring_getDamping);
        tolua_function(tolua_S,"setDamping",lua_ax_physics_PhysicsJointRotarySpring_setDamping);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointRotarySpring_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointRotarySpring_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointRotarySpring).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointRotarySpring";
    g_typeCast[typeName] = "ax.PhysicsJointRotarySpring";
    return 1;
}

int lua_ax_physics_PhysicsJointRotaryLimit_getMin(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotaryLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotaryLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotaryLimit:getMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotaryLimit_setMin(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotaryLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotaryLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRotaryLimit:setMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMin'", nullptr);
            return 0;
        }
        cobj->setMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotaryLimit:setMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotaryLimit_getMax(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotaryLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotaryLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMax'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMax();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotaryLimit:getMax",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_getMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotaryLimit_setMax(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotaryLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotaryLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRotaryLimit:setMax");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMax'", nullptr);
            return 0;
        }
        cobj->setMax(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotaryLimit:setMax",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_setMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotaryLimit_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRotaryLimit* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRotaryLimit*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRotaryLimit_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRotaryLimit_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRotaryLimit:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRotaryLimit_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointRotaryLimit",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            ax::PhysicsJointRotaryLimit* ret = ax::PhysicsJointRotaryLimit::construct(arg0, arg1);
            object_to_luaval<ax::PhysicsJointRotaryLimit>(tolua_S, "ax.PhysicsJointRotaryLimit",(ax::PhysicsJointRotaryLimit*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            ax::PhysicsBody* arg1;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            double arg2;
            ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            double arg3;
            ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsJointRotaryLimit:construct");
            if (!ok) { break; }
            ax::PhysicsJointRotaryLimit* ret = ax::PhysicsJointRotaryLimit::construct(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::PhysicsJointRotaryLimit>(tolua_S, "ax.PhysicsJointRotaryLimit",(ax::PhysicsJointRotaryLimit*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.PhysicsJointRotaryLimit:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRotaryLimit_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointRotaryLimit_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointRotaryLimit)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointRotaryLimit(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointRotaryLimit");
    tolua_cclass(tolua_S,"PhysicsJointRotaryLimit","ax.PhysicsJointRotaryLimit","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointRotaryLimit");
        tolua_function(tolua_S,"getMin",lua_ax_physics_PhysicsJointRotaryLimit_getMin);
        tolua_function(tolua_S,"setMin",lua_ax_physics_PhysicsJointRotaryLimit_setMin);
        tolua_function(tolua_S,"getMax",lua_ax_physics_PhysicsJointRotaryLimit_getMax);
        tolua_function(tolua_S,"setMax",lua_ax_physics_PhysicsJointRotaryLimit_setMax);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointRotaryLimit_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointRotaryLimit_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointRotaryLimit).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointRotaryLimit";
    g_typeCast[typeName] = "ax.PhysicsJointRotaryLimit";
    return 1;
}

int lua_ax_physics_PhysicsJointRatchet_getAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_getAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_getAngle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:getAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_getAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_setAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_setAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRatchet:setAngle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_setAngle'", nullptr);
            return 0;
        }
        cobj->setAngle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:setAngle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_setAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_getPhase(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_getPhase'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_getPhase'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPhase();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:getPhase",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_getPhase'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_setPhase(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_setPhase'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRatchet:setPhase");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_setPhase'", nullptr);
            return 0;
        }
        cobj->setPhase(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:setPhase",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_setPhase'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_getRatchet(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_getRatchet'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_getRatchet'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRatchet();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:getRatchet",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_getRatchet'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_setRatchet(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_setRatchet'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointRatchet:setRatchet");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_setRatchet'", nullptr);
            return 0;
        }
        cobj->setRatchet(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:setRatchet",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_setRatchet'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointRatchet* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointRatchet*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointRatchet_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointRatchet:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointRatchet_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointRatchet",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        double arg2;
        double arg3;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointRatchet:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointRatchet:construct");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsJointRatchet:construct");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsJointRatchet:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointRatchet_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointRatchet::construct(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsJointRatchet>(tolua_S, "ax.PhysicsJointRatchet",(ax::PhysicsJointRatchet*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointRatchet:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointRatchet_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointRatchet_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointRatchet)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointRatchet(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointRatchet");
    tolua_cclass(tolua_S,"PhysicsJointRatchet","ax.PhysicsJointRatchet","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointRatchet");
        tolua_function(tolua_S,"getAngle",lua_ax_physics_PhysicsJointRatchet_getAngle);
        tolua_function(tolua_S,"setAngle",lua_ax_physics_PhysicsJointRatchet_setAngle);
        tolua_function(tolua_S,"getPhase",lua_ax_physics_PhysicsJointRatchet_getPhase);
        tolua_function(tolua_S,"setPhase",lua_ax_physics_PhysicsJointRatchet_setPhase);
        tolua_function(tolua_S,"getRatchet",lua_ax_physics_PhysicsJointRatchet_getRatchet);
        tolua_function(tolua_S,"setRatchet",lua_ax_physics_PhysicsJointRatchet_setRatchet);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointRatchet_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointRatchet_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointRatchet).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointRatchet";
    g_typeCast[typeName] = "ax.PhysicsJointRatchet";
    return 1;
}

int lua_ax_physics_PhysicsJointGear_getPhase(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGear* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGear*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGear_getPhase'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_getPhase'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPhase();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGear:getPhase",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_getPhase'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGear_setPhase(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGear* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGear*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGear_setPhase'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointGear:setPhase");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_setPhase'", nullptr);
            return 0;
        }
        cobj->setPhase(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGear:setPhase",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_setPhase'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGear_getRatio(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGear* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGear*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGear_getRatio'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_getRatio'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRatio();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGear:getRatio",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_getRatio'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGear_setRatio(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGear* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGear*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGear_setRatio'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointGear:setRatio");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_setRatio'", nullptr);
            return 0;
        }
        cobj->setRatio(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGear:setRatio",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_setRatio'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGear_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointGear* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointGear*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointGear_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointGear:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointGear_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointGear",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        double arg2;
        double arg3;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointGear:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointGear:construct");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsJointGear:construct");
        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.PhysicsJointGear:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointGear_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointGear::construct(arg0, arg1, arg2, arg3);
        object_to_luaval<ax::PhysicsJointGear>(tolua_S, "ax.PhysicsJointGear",(ax::PhysicsJointGear*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointGear:construct",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointGear_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointGear_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointGear)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointGear(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointGear");
    tolua_cclass(tolua_S,"PhysicsJointGear","ax.PhysicsJointGear","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointGear");
        tolua_function(tolua_S,"getPhase",lua_ax_physics_PhysicsJointGear_getPhase);
        tolua_function(tolua_S,"setPhase",lua_ax_physics_PhysicsJointGear_setPhase);
        tolua_function(tolua_S,"getRatio",lua_ax_physics_PhysicsJointGear_getRatio);
        tolua_function(tolua_S,"setRatio",lua_ax_physics_PhysicsJointGear_setRatio);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointGear_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointGear_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointGear).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointGear";
    g_typeCast[typeName] = "ax.PhysicsJointGear";
    return 1;
}

int lua_ax_physics_PhysicsJointMotor_getRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointMotor* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointMotor",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointMotor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointMotor_getRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointMotor_getRate'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRate();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointMotor:getRate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointMotor_getRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointMotor_setRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointMotor* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointMotor",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointMotor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointMotor_setRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsJointMotor:setRate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointMotor_setRate'", nullptr);
            return 0;
        }
        cobj->setRate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointMotor:setRate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointMotor_setRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointMotor_createConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsJointMotor* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsJointMotor",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsJointMotor*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsJointMotor_createConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointMotor_createConstraints'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createConstraints();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsJointMotor:createConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointMotor_createConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsJointMotor_construct(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PhysicsJointMotor",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::PhysicsBody* arg0;
        ax::PhysicsBody* arg1;
        double arg2;
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsJointMotor:construct");
        ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 3, "ax.PhysicsBody",&arg1, "ax.PhysicsJointMotor:construct");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.PhysicsJointMotor:construct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsJointMotor_construct'", nullptr);
            return 0;
        }
        auto&& ret = ax::PhysicsJointMotor::construct(arg0, arg1, arg2);
        object_to_luaval<ax::PhysicsJointMotor>(tolua_S, "ax.PhysicsJointMotor",(ax::PhysicsJointMotor*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.PhysicsJointMotor:construct",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsJointMotor_construct'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_physics_PhysicsJointMotor_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsJointMotor)");
    return 0;
}

int lua_register_ax_physics_PhysicsJointMotor(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsJointMotor");
    tolua_cclass(tolua_S,"PhysicsJointMotor","ax.PhysicsJointMotor","ax.PhysicsJoint",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsJointMotor");
        tolua_function(tolua_S,"getRate",lua_ax_physics_PhysicsJointMotor_getRate);
        tolua_function(tolua_S,"setRate",lua_ax_physics_PhysicsJointMotor_setRate);
        tolua_function(tolua_S,"createConstraints",lua_ax_physics_PhysicsJointMotor_createConstraints);
        tolua_function(tolua_S,"construct", lua_ax_physics_PhysicsJointMotor_construct);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsJointMotor).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsJointMotor";
    g_typeCast[typeName] = "ax.PhysicsJointMotor";
    return 1;
}

int lua_ax_physics_PhysicsWorld_addJoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_addJoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PhysicsJoint* arg0;

        ok &= luaval_to_object<ax::PhysicsJoint>(tolua_S, 2, "ax.PhysicsJoint",&arg0, "ax.PhysicsWorld:addJoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_addJoint'", nullptr);
            return 0;
        }
        cobj->addJoint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:addJoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_addJoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_removeJoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_removeJoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PhysicsJoint* arg0;

        ok &= luaval_to_object<ax::PhysicsJoint>(tolua_S, 2, "ax.PhysicsJoint",&arg0, "ax.PhysicsWorld:removeJoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_removeJoint'", nullptr);
            return 0;
        }
        cobj->removeJoint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::PhysicsJoint* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::PhysicsJoint>(tolua_S, 2, "ax.PhysicsJoint",&arg0, "ax.PhysicsWorld:removeJoint");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.PhysicsWorld:removeJoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_removeJoint'", nullptr);
            return 0;
        }
        cobj->removeJoint(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:removeJoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_removeJoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_removeAllJoints(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_removeAllJoints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_removeAllJoints'", nullptr);
            return 0;
        }
        cobj->removeAllJoints();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsWorld:removeAllJoints");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_removeAllJoints'", nullptr);
            return 0;
        }
        cobj->removeAllJoints(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:removeAllJoints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_removeAllJoints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_removeBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_removeBody'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:removeBody");

            if (!ok) { break; }
            cobj->removeBody(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::PhysicsBody* arg0;
            ok &= luaval_to_object<ax::PhysicsBody>(tolua_S, 2, "ax.PhysicsBody",&arg0, "ax.PhysicsWorld:removeBody");

            if (!ok) { break; }
            cobj->removeBody(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.PhysicsWorld:removeBody",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_removeBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_removeAllBodies(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_removeAllBodies'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_removeAllBodies'", nullptr);
            return 0;
        }
        cobj->removeAllBodies();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:removeAllBodies",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_removeAllBodies'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getShapes(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getShapes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld:getShapes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getShapes'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShapes(arg0);
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getShapes",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getShapes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getShape'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld:getShape");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShape(arg0);
        object_to_luaval<ax::PhysicsShape>(tolua_S, "ax.PhysicsShape",(ax::PhysicsShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getShape",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getAllBodies(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getAllBodies'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getAllBodies'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAllBodies();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getAllBodies",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getAllBodies'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:getBody");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getBody'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBody(arg0);
        object_to_luaval<ax::PhysicsBody>(tolua_S, "ax.PhysicsBody",(ax::PhysicsBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getBody",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getGravity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGravity();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getGravity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.PhysicsWorld:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setGravity'", nullptr);
            return 0;
        }
        cobj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setGravity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setSlopBias(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setSlopBias'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsWorld:setSlopBias");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.PhysicsWorld:setSlopBias");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setSlopBias'", nullptr);
            return 0;
        }
        cobj->setSlopBias(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setSlopBias",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setSlopBias'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsWorld:setSpeed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setSpeed'", nullptr);
            return 0;
        }
        cobj->setSpeed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setSpeed",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getSpeed'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSpeed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getSpeed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setUpdateRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setUpdateRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:setUpdateRate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setUpdateRate'", nullptr);
            return 0;
        }
        cobj->setUpdateRate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setUpdateRate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setUpdateRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getUpdateRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getUpdateRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getUpdateRate'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUpdateRate();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getUpdateRate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getUpdateRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setSubsteps(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setSubsteps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:setSubsteps");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setSubsteps'", nullptr);
            return 0;
        }
        cobj->setSubsteps(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setSubsteps",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setSubsteps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getSubsteps(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getSubsteps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getSubsteps'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSubsteps();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getSubsteps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getSubsteps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setFixedUpdateRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setFixedUpdateRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:setFixedUpdateRate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setFixedUpdateRate'", nullptr);
            return 0;
        }
        cobj->setFixedUpdateRate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setFixedUpdateRate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setFixedUpdateRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getFixedUpdateRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getFixedUpdateRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getFixedUpdateRate'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFixedUpdateRate();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getFixedUpdateRate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getFixedUpdateRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setDebugDrawMask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setDebugDrawMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsWorld:setDebugDrawMask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setDebugDrawMask'", nullptr);
            return 0;
        }
        cobj->setDebugDrawMask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setDebugDrawMask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setDebugDrawMask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setPreUpdateCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setPreUpdateCallback'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setPreUpdateCallback'", nullptr);
            return 0;
        }
        cobj->setPreUpdateCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setPreUpdateCallback",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setPreUpdateCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setPostUpdateCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setPostUpdateCallback'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setPostUpdateCallback'", nullptr);
            return 0;
        }
        cobj->setPostUpdateCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setPostUpdateCallback",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setPostUpdateCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_getDebugDrawMask(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_getDebugDrawMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_getDebugDrawMask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDebugDrawMask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:getDebugDrawMask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_getDebugDrawMask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_setAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_setAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PhysicsWorld:setAutoStep");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_setAutoStep'", nullptr);
            return 0;
        }
        cobj->setAutoStep(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:setAutoStep",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_setAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_isAutoStep(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_isAutoStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_isAutoStep'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isAutoStep();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:isAutoStep",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_isAutoStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics_PhysicsWorld_step(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics_PhysicsWorld_step'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PhysicsWorld:step");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics_PhysicsWorld_step'", nullptr);
            return 0;
        }
        cobj->step(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsWorld:step",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics_PhysicsWorld_step'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics_PhysicsWorld_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsWorld)");
    return 0;
}

int lua_register_ax_physics_PhysicsWorld(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsWorld");
    tolua_cclass(tolua_S,"PhysicsWorld","ax.PhysicsWorld","",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsWorld");
        tolua_function(tolua_S,"addJoint",lua_ax_physics_PhysicsWorld_addJoint);
        tolua_function(tolua_S,"removeJoint",lua_ax_physics_PhysicsWorld_removeJoint);
        tolua_function(tolua_S,"removeAllJoints",lua_ax_physics_PhysicsWorld_removeAllJoints);
        tolua_function(tolua_S,"removeBody",lua_ax_physics_PhysicsWorld_removeBody);
        tolua_function(tolua_S,"removeAllBodies",lua_ax_physics_PhysicsWorld_removeAllBodies);
        tolua_function(tolua_S,"getShapes",lua_ax_physics_PhysicsWorld_getShapes);
        tolua_function(tolua_S,"getShape",lua_ax_physics_PhysicsWorld_getShape);
        tolua_function(tolua_S,"getAllBodies",lua_ax_physics_PhysicsWorld_getAllBodies);
        tolua_function(tolua_S,"getBody",lua_ax_physics_PhysicsWorld_getBody);
        tolua_function(tolua_S,"getGravity",lua_ax_physics_PhysicsWorld_getGravity);
        tolua_function(tolua_S,"setGravity",lua_ax_physics_PhysicsWorld_setGravity);
        tolua_function(tolua_S,"setSlopBias",lua_ax_physics_PhysicsWorld_setSlopBias);
        tolua_function(tolua_S,"setSpeed",lua_ax_physics_PhysicsWorld_setSpeed);
        tolua_function(tolua_S,"getSpeed",lua_ax_physics_PhysicsWorld_getSpeed);
        tolua_function(tolua_S,"setUpdateRate",lua_ax_physics_PhysicsWorld_setUpdateRate);
        tolua_function(tolua_S,"getUpdateRate",lua_ax_physics_PhysicsWorld_getUpdateRate);
        tolua_function(tolua_S,"setSubsteps",lua_ax_physics_PhysicsWorld_setSubsteps);
        tolua_function(tolua_S,"getSubsteps",lua_ax_physics_PhysicsWorld_getSubsteps);
        tolua_function(tolua_S,"setFixedUpdateRate",lua_ax_physics_PhysicsWorld_setFixedUpdateRate);
        tolua_function(tolua_S,"getFixedUpdateRate",lua_ax_physics_PhysicsWorld_getFixedUpdateRate);
        tolua_function(tolua_S,"setDebugDrawMask",lua_ax_physics_PhysicsWorld_setDebugDrawMask);
        tolua_function(tolua_S,"setPreUpdateCallback",lua_ax_physics_PhysicsWorld_setPreUpdateCallback);
        tolua_function(tolua_S,"setPostUpdateCallback",lua_ax_physics_PhysicsWorld_setPostUpdateCallback);
        tolua_function(tolua_S,"getDebugDrawMask",lua_ax_physics_PhysicsWorld_getDebugDrawMask);
        tolua_function(tolua_S,"setAutoStep",lua_ax_physics_PhysicsWorld_setAutoStep);
        tolua_function(tolua_S,"isAutoStep",lua_ax_physics_PhysicsWorld_isAutoStep);
        tolua_function(tolua_S,"step",lua_ax_physics_PhysicsWorld_step);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsWorld).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsWorld";
    g_typeCast[typeName] = "ax.PhysicsWorld";
    return 1;
}
TOLUA_API int register_all_ax_physics(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_physics_PhysicsShape(tolua_S);
	lua_register_ax_physics_PhysicsShapeCircle(tolua_S);
	lua_register_ax_physics_PhysicsShapePolygon(tolua_S);
	lua_register_ax_physics_PhysicsShapeBox(tolua_S);
	lua_register_ax_physics_PhysicsShapeEdgeSegment(tolua_S);
	lua_register_ax_physics_PhysicsShapeEdgePolygon(tolua_S);
	lua_register_ax_physics_PhysicsShapeEdgeBox(tolua_S);
	lua_register_ax_physics_PhysicsShapeEdgeChain(tolua_S);
	lua_register_ax_physics_PhysicsBody(tolua_S);
	lua_register_ax_physics_PhysicsContact(tolua_S);
	lua_register_ax_physics_PhysicsContactPreSolve(tolua_S);
	lua_register_ax_physics_PhysicsContactPostSolve(tolua_S);
	lua_register_ax_physics_EventListenerPhysicsContact(tolua_S);
	lua_register_ax_physics_EventListenerPhysicsContactWithBodies(tolua_S);
	lua_register_ax_physics_EventListenerPhysicsContactWithShapes(tolua_S);
	lua_register_ax_physics_EventListenerPhysicsContactWithGroup(tolua_S);
	lua_register_ax_physics_PhysicsJoint(tolua_S);
	lua_register_ax_physics_PhysicsJointFixed(tolua_S);
	lua_register_ax_physics_PhysicsJointLimit(tolua_S);
	lua_register_ax_physics_PhysicsJointPin(tolua_S);
	lua_register_ax_physics_PhysicsJointDistance(tolua_S);
	lua_register_ax_physics_PhysicsJointSpring(tolua_S);
	lua_register_ax_physics_PhysicsJointGroove(tolua_S);
	lua_register_ax_physics_PhysicsJointRotarySpring(tolua_S);
	lua_register_ax_physics_PhysicsJointRotaryLimit(tolua_S);
	lua_register_ax_physics_PhysicsJointRatchet(tolua_S);
	lua_register_ax_physics_PhysicsJointGear(tolua_S);
	lua_register_ax_physics_PhysicsJointMotor(tolua_S);
	lua_register_ax_physics_PhysicsWorld(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

#endif
