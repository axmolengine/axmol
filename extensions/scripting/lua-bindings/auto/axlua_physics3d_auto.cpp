#include "lua-bindings/auto/axlua_physics3d_auto.hpp"
#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
#include "physics3d/Physics3D.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_physics3d_Physics3DShape_getShapeType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_getShapeType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_getShapeType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getShapeType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:getShapeType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_getShapeType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_getbtShape(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_getbtShape'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_getbtShape'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getbtShape();
        object_to_luaval<btCollisionShape>(tolua_S, "btCollisionShape",(btCollisionShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:getbtShape",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_getbtShape'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_initBox(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_initBox'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DShape:initBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_initBox'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initBox(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:initBox",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_initBox'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_initSphere(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_initSphere'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:initSphere");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_initSphere'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initSphere(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:initSphere",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_initSphere'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_initCylinder(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_initCylinder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:initCylinder");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DShape:initCylinder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_initCylinder'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initCylinder(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:initCylinder",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_initCylinder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_initCapsule(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DShape*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DShape_initCapsule'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:initCapsule");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DShape:initCapsule");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_initCapsule'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initCapsule(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:initCapsule",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_initCapsule'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DShape_createBox(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Vec3 arg0;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DShape:createBox");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_createBox'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DShape::createBox(arg0);
        object_to_luaval<ax::Physics3DShape>(tolua_S, "ax.Physics3DShape",(ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createBox",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_createBox'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DShape_createSphere(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:createSphere");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_createSphere'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DShape::createSphere(arg0);
        object_to_luaval<ax::Physics3DShape>(tolua_S, "ax.Physics3DShape",(ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createSphere",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_createSphere'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DShape_createCylinder(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        double arg1;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:createCylinder");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DShape:createCylinder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_createCylinder'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DShape::createCylinder(arg0, arg1);
        object_to_luaval<ax::Physics3DShape>(tolua_S, "ax.Physics3DShape",(ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createCylinder",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_createCylinder'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DShape_createCapsule(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double arg0;
        double arg1;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DShape:createCapsule");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DShape:createCapsule");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_createCapsule'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DShape::createCapsule(arg0, arg1);
        object_to_luaval<ax::Physics3DShape>(tolua_S, "ax.Physics3DShape",(ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createCapsule",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_createCapsule'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DShape_createConvexHull(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DShape",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        const ax::Vec3* arg0;
        int arg1;
        ok &= luaval_to_object<const ax::Vec3>(tolua_S, 2, "ax.Vec3",&arg0, "ax.Physics3DShape:createConvexHull");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ax.Physics3DShape:createConvexHull");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_createConvexHull'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DShape::createConvexHull(arg0, arg1);
        object_to_luaval<ax::Physics3DShape>(tolua_S, "ax.Physics3DShape",(ax::Physics3DShape*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DShape:createConvexHull",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_createConvexHull'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DShape_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DShape* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DShape_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DShape();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DShape");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DShape:Physics3DShape",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DShape_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DShape_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DShape)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DShape(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DShape");
    tolua_cclass(tolua_S,"Physics3DShape","ax.Physics3DShape","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DShape");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DShape_constructor);
        tolua_function(tolua_S,"getShapeType",lua_ax_physics3d_Physics3DShape_getShapeType);
        tolua_function(tolua_S,"getbtShape",lua_ax_physics3d_Physics3DShape_getbtShape);
        tolua_function(tolua_S,"initBox",lua_ax_physics3d_Physics3DShape_initBox);
        tolua_function(tolua_S,"initSphere",lua_ax_physics3d_Physics3DShape_initSphere);
        tolua_function(tolua_S,"initCylinder",lua_ax_physics3d_Physics3DShape_initCylinder);
        tolua_function(tolua_S,"initCapsule",lua_ax_physics3d_Physics3DShape_initCapsule);
        tolua_function(tolua_S,"createBox", lua_ax_physics3d_Physics3DShape_createBox);
        tolua_function(tolua_S,"createSphere", lua_ax_physics3d_Physics3DShape_createSphere);
        tolua_function(tolua_S,"createCylinder", lua_ax_physics3d_Physics3DShape_createCylinder);
        tolua_function(tolua_S,"createCapsule", lua_ax_physics3d_Physics3DShape_createCapsule);
        tolua_function(tolua_S,"createConvexHull", lua_ax_physics3d_Physics3DShape_createConvexHull);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DShape).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DShape";
    g_typeCast[typeName] = "ax.Physics3DShape";
    return 1;
}

int lua_ax_physics3d_Physics3DObject_getObjType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getObjType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getObjType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getObjType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getObjType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getObjType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_setUserData'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_setUserData'", nullptr);
            return 0;
        }
        cobj->setUserData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_setUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_getUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getUserData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUserData();
        #pragma warning NO CONVERSION FROM NATIVE FOR void*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getUserData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_setPhysicsWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_setPhysicsWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DWorld* arg0;

        ok &= luaval_to_object<ax::Physics3DWorld>(tolua_S, 2, "ax.Physics3DWorld",&arg0, "ax.Physics3DObject:setPhysicsWorld");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_setPhysicsWorld'", nullptr);
            return 0;
        }
        cobj->setPhysicsWorld(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:setPhysicsWorld",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_setPhysicsWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_getPhysicsWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getPhysicsWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getPhysicsWorld'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPhysicsWorld();
        object_to_luaval<ax::Physics3DWorld>(tolua_S, "ax.Physics3DWorld",(ax::Physics3DWorld*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getPhysicsWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getPhysicsWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_getWorldTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getWorldTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getWorldTransform'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getWorldTransform();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getWorldTransform",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getWorldTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_getCollisionCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getCollisionCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getCollisionCallback'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCollisionCallback();
        #pragma warning NO CONVERSION FROM NATIVE FOR std::function;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getCollisionCallback",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getCollisionCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_needCollisionCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_needCollisionCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_needCollisionCallback'", nullptr);
            return 0;
        }
        auto&& ret = cobj->needCollisionCallback();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:needCollisionCallback",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_needCollisionCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_setMask(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_setMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.Physics3DObject:setMask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_setMask'", nullptr);
            return 0;
        }
        cobj->setMask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:setMask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_setMask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DObject_getMask(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DObject_getMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DObject_getMask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DObject:getMask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DObject_getMask'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_Physics3DObject_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DObject)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DObject(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DObject");
    tolua_cclass(tolua_S,"Physics3DObject","ax.Physics3DObject","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DObject");
        tolua_function(tolua_S,"getObjType",lua_ax_physics3d_Physics3DObject_getObjType);
        tolua_function(tolua_S,"setUserData",lua_ax_physics3d_Physics3DObject_setUserData);
        tolua_function(tolua_S,"getUserData",lua_ax_physics3d_Physics3DObject_getUserData);
        tolua_function(tolua_S,"setPhysicsWorld",lua_ax_physics3d_Physics3DObject_setPhysicsWorld);
        tolua_function(tolua_S,"getPhysicsWorld",lua_ax_physics3d_Physics3DObject_getPhysicsWorld);
        tolua_function(tolua_S,"getWorldTransform",lua_ax_physics3d_Physics3DObject_getWorldTransform);
        tolua_function(tolua_S,"getCollisionCallback",lua_ax_physics3d_Physics3DObject_getCollisionCallback);
        tolua_function(tolua_S,"needCollisionCallback",lua_ax_physics3d_Physics3DObject_needCollisionCallback);
        tolua_function(tolua_S,"setMask",lua_ax_physics3d_Physics3DObject_setMask);
        tolua_function(tolua_S,"getMask",lua_ax_physics3d_Physics3DObject_getMask);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DObject).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DObject";
    g_typeCast[typeName] = "ax.Physics3DObject";
    return 1;
}

int lua_ax_physics3d_Physics3DRigidBody_getRigidBody(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getRigidBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getRigidBody'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRigidBody();
        object_to_luaval<btRigidBody>(tolua_S, "btRigidBody",(btRigidBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getRigidBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getRigidBody'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyForce");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DRigidBody:applyForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyForce'", nullptr);
            return 0;
        }
        cobj->applyForce(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyForce",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyCentralForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyCentralForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralForce'", nullptr);
            return 0;
        }
        cobj->applyCentralForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyCentralForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyCentralImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyCentralImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralImpulse'", nullptr);
            return 0;
        }
        cobj->applyCentralImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyCentralImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyCentralImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyTorque");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorque'", nullptr);
            return 0;
        }
        cobj->applyTorque(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyTorque",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyTorqueImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorqueImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyTorqueImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorqueImpulse'", nullptr);
            return 0;
        }
        cobj->applyTorqueImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyTorqueImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyTorqueImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:applyImpulse");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DRigidBody:applyImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyImpulse'", nullptr);
            return 0;
        }
        cobj->applyImpulse(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyImpulse",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_applyDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_applyDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:applyDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_applyDamping'", nullptr);
            return 0;
        }
        cobj->applyDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:applyDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_applyDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setLinearVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearVelocity'", nullptr);
            return 0;
        }
        cobj->setLinearVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setLinearVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getLinearVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearVelocity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getLinearVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setLinearFactor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearFactor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setLinearFactor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearFactor'", nullptr);
            return 0;
        }
        cobj->setLinearFactor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setLinearFactor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setLinearFactor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getLinearFactor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearFactor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearFactor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearFactor();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getLinearFactor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearFactor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setAngularFactor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setAngularFactor'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setAngularFactor");

            if (!ok) { break; }
            cobj->setAngularFactor(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::Vec3 arg0;
            ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setAngularFactor");

            if (!ok) { break; }
            cobj->setAngularFactor(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.Physics3DRigidBody:setAngularFactor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setAngularFactor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getAngularFactor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularFactor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularFactor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularFactor();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getAngularFactor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularFactor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setAngularVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setAngularVelocity'", nullptr);
            return 0;
        }
        cobj->setAngularVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setAngularVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getAngularVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularVelocity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getAngularVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setCenterOfMassTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setCenterOfMassTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Mat4 arg0;

        ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setCenterOfMassTransform");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setCenterOfMassTransform'", nullptr);
            return 0;
        }
        cobj->setCenterOfMassTransform(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setCenterOfMassTransform",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setCenterOfMassTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getCenterOfMassTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getCenterOfMassTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getCenterOfMassTransform'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCenterOfMassTransform();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getCenterOfMassTransform",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getCenterOfMassTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setDamping");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DRigidBody:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setDamping'", nullptr);
            return 0;
        }
        cobj->setDamping(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setDamping",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getLinearDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getLinearDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getLinearDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getAngularDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularDamping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularDamping();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getAngularDamping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getAngularDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setGravity'", nullptr);
            return 0;
        }
        cobj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setGravity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getGravity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGravity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getGravity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setInvInertiaDiagLocal(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setInvInertiaDiagLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DRigidBody:setInvInertiaDiagLocal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setInvInertiaDiagLocal'", nullptr);
            return 0;
        }
        cobj->setInvInertiaDiagLocal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setInvInertiaDiagLocal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setInvInertiaDiagLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getInvInertiaDiagLocal(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getInvInertiaDiagLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getInvInertiaDiagLocal'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getInvInertiaDiagLocal();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getInvInertiaDiagLocal",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getInvInertiaDiagLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setMassProps(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setMassProps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setMassProps");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DRigidBody:setMassProps");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setMassProps'", nullptr);
            return 0;
        }
        cobj->setMassProps(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setMassProps",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setMassProps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getInvMass(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getInvMass'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getInvMass'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getInvMass();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getInvMass",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getInvMass'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getTotalForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalForce'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTotalForce();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getTotalForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getTotalTorque(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalTorque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalTorque'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTotalTorque();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getTotalTorque",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getTotalTorque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setRestitution");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setRestitution'", nullptr);
            return 0;
        }
        cobj->setRestitution(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setRestitution",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getRestitution(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getRestitution'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getRestitution'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitution();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getRestitution",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getRestitution'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setFriction'", nullptr);
            return 0;
        }
        cobj->setFriction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setFriction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getFriction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setRollingFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setRollingFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setRollingFriction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setRollingFriction'", nullptr);
            return 0;
        }
        cobj->setRollingFriction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setRollingFriction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setRollingFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getRollingFriction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getRollingFriction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getRollingFriction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRollingFriction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getRollingFriction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getRollingFriction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setHitFraction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setHitFraction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setHitFraction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setHitFraction'", nullptr);
            return 0;
        }
        cobj->setHitFraction(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setHitFraction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setHitFraction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getHitFraction(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getHitFraction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getHitFraction'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getHitFraction();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getHitFraction",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getHitFraction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setCcdMotionThreshold(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdMotionThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setCcdMotionThreshold");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdMotionThreshold'", nullptr);
            return 0;
        }
        cobj->setCcdMotionThreshold(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setCcdMotionThreshold",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdMotionThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getCcdMotionThreshold(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdMotionThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdMotionThreshold'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCcdMotionThreshold();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getCcdMotionThreshold",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdMotionThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setCcdSweptSphereRadius(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdSweptSphereRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setCcdSweptSphereRadius");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdSweptSphereRadius'", nullptr);
            return 0;
        }
        cobj->setCcdSweptSphereRadius(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setCcdSweptSphereRadius",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setCcdSweptSphereRadius'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getCcdSweptSphereRadius(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdSweptSphereRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdSweptSphereRadius'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCcdSweptSphereRadius();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getCcdSweptSphereRadius",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getCcdSweptSphereRadius'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setKinematic");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setKinematic'", nullptr);
            return 0;
        }
        cobj->setKinematic(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setKinematic",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_isKinematic(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_isKinematic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_isKinematic'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isKinematic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:isKinematic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_isKinematic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getConstraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:getConstraint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getConstraint(arg0);
        object_to_luaval<ax::Physics3DConstraint>(tolua_S, "ax.Physics3DConstraint",(ax::Physics3DConstraint*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getConstraint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_getConstraintCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraintCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraintCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getConstraintCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:getConstraintCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_getConstraintCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_setActive(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_setActive'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:setActive");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_setActive'", nullptr);
            return 0;
        }
        cobj->setActive(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:setActive",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_setActive'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DRigidBodyDes* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR Physics3DRigidBodyDes*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_init'", nullptr);
            return 0;
        }
        auto&& ret = cobj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_addConstraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_addConstraint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DConstraint* arg0;

        ok &= luaval_to_object<ax::Physics3DConstraint>(tolua_S, 2, "ax.Physics3DConstraint",&arg0, "ax.Physics3DRigidBody:addConstraint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_addConstraint'", nullptr);
            return 0;
        }
        cobj->addConstraint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:addConstraint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_addConstraint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_removeConstraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DRigidBody",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::Physics3DRigidBody*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DRigidBody_removeConstraint'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            unsigned int arg0;
            ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.Physics3DRigidBody:removeConstraint");

            if (!ok) { break; }
            cobj->removeConstraint(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::Physics3DConstraint* arg0;
            ok &= luaval_to_object<ax::Physics3DConstraint>(tolua_S, 2, "ax.Physics3DConstraint",&arg0, "ax.Physics3DRigidBody:removeConstraint");

            if (!ok) { break; }
            cobj->removeConstraint(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.Physics3DRigidBody:removeConstraint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_removeConstraint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DRigidBody_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DRigidBody* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DRigidBody_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DRigidBody();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DRigidBody");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DRigidBody:Physics3DRigidBody",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DRigidBody_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DRigidBody_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DRigidBody)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DRigidBody(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DRigidBody");
    tolua_cclass(tolua_S,"Physics3DRigidBody","ax.Physics3DRigidBody","ax.Physics3DObject",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DRigidBody");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DRigidBody_constructor);
        tolua_function(tolua_S,"getRigidBody",lua_ax_physics3d_Physics3DRigidBody_getRigidBody);
        tolua_function(tolua_S,"applyForce",lua_ax_physics3d_Physics3DRigidBody_applyForce);
        tolua_function(tolua_S,"applyCentralForce",lua_ax_physics3d_Physics3DRigidBody_applyCentralForce);
        tolua_function(tolua_S,"applyCentralImpulse",lua_ax_physics3d_Physics3DRigidBody_applyCentralImpulse);
        tolua_function(tolua_S,"applyTorque",lua_ax_physics3d_Physics3DRigidBody_applyTorque);
        tolua_function(tolua_S,"applyTorqueImpulse",lua_ax_physics3d_Physics3DRigidBody_applyTorqueImpulse);
        tolua_function(tolua_S,"applyImpulse",lua_ax_physics3d_Physics3DRigidBody_applyImpulse);
        tolua_function(tolua_S,"applyDamping",lua_ax_physics3d_Physics3DRigidBody_applyDamping);
        tolua_function(tolua_S,"setLinearVelocity",lua_ax_physics3d_Physics3DRigidBody_setLinearVelocity);
        tolua_function(tolua_S,"getLinearVelocity",lua_ax_physics3d_Physics3DRigidBody_getLinearVelocity);
        tolua_function(tolua_S,"setLinearFactor",lua_ax_physics3d_Physics3DRigidBody_setLinearFactor);
        tolua_function(tolua_S,"getLinearFactor",lua_ax_physics3d_Physics3DRigidBody_getLinearFactor);
        tolua_function(tolua_S,"setAngularFactor",lua_ax_physics3d_Physics3DRigidBody_setAngularFactor);
        tolua_function(tolua_S,"getAngularFactor",lua_ax_physics3d_Physics3DRigidBody_getAngularFactor);
        tolua_function(tolua_S,"setAngularVelocity",lua_ax_physics3d_Physics3DRigidBody_setAngularVelocity);
        tolua_function(tolua_S,"getAngularVelocity",lua_ax_physics3d_Physics3DRigidBody_getAngularVelocity);
        tolua_function(tolua_S,"setCenterOfMassTransform",lua_ax_physics3d_Physics3DRigidBody_setCenterOfMassTransform);
        tolua_function(tolua_S,"getCenterOfMassTransform",lua_ax_physics3d_Physics3DRigidBody_getCenterOfMassTransform);
        tolua_function(tolua_S,"setDamping",lua_ax_physics3d_Physics3DRigidBody_setDamping);
        tolua_function(tolua_S,"getLinearDamping",lua_ax_physics3d_Physics3DRigidBody_getLinearDamping);
        tolua_function(tolua_S,"getAngularDamping",lua_ax_physics3d_Physics3DRigidBody_getAngularDamping);
        tolua_function(tolua_S,"setGravity",lua_ax_physics3d_Physics3DRigidBody_setGravity);
        tolua_function(tolua_S,"getGravity",lua_ax_physics3d_Physics3DRigidBody_getGravity);
        tolua_function(tolua_S,"setInvInertiaDiagLocal",lua_ax_physics3d_Physics3DRigidBody_setInvInertiaDiagLocal);
        tolua_function(tolua_S,"getInvInertiaDiagLocal",lua_ax_physics3d_Physics3DRigidBody_getInvInertiaDiagLocal);
        tolua_function(tolua_S,"setMassProps",lua_ax_physics3d_Physics3DRigidBody_setMassProps);
        tolua_function(tolua_S,"getInvMass",lua_ax_physics3d_Physics3DRigidBody_getInvMass);
        tolua_function(tolua_S,"getTotalForce",lua_ax_physics3d_Physics3DRigidBody_getTotalForce);
        tolua_function(tolua_S,"getTotalTorque",lua_ax_physics3d_Physics3DRigidBody_getTotalTorque);
        tolua_function(tolua_S,"setRestitution",lua_ax_physics3d_Physics3DRigidBody_setRestitution);
        tolua_function(tolua_S,"getRestitution",lua_ax_physics3d_Physics3DRigidBody_getRestitution);
        tolua_function(tolua_S,"setFriction",lua_ax_physics3d_Physics3DRigidBody_setFriction);
        tolua_function(tolua_S,"getFriction",lua_ax_physics3d_Physics3DRigidBody_getFriction);
        tolua_function(tolua_S,"setRollingFriction",lua_ax_physics3d_Physics3DRigidBody_setRollingFriction);
        tolua_function(tolua_S,"getRollingFriction",lua_ax_physics3d_Physics3DRigidBody_getRollingFriction);
        tolua_function(tolua_S,"setHitFraction",lua_ax_physics3d_Physics3DRigidBody_setHitFraction);
        tolua_function(tolua_S,"getHitFraction",lua_ax_physics3d_Physics3DRigidBody_getHitFraction);
        tolua_function(tolua_S,"setCcdMotionThreshold",lua_ax_physics3d_Physics3DRigidBody_setCcdMotionThreshold);
        tolua_function(tolua_S,"getCcdMotionThreshold",lua_ax_physics3d_Physics3DRigidBody_getCcdMotionThreshold);
        tolua_function(tolua_S,"setCcdSweptSphereRadius",lua_ax_physics3d_Physics3DRigidBody_setCcdSweptSphereRadius);
        tolua_function(tolua_S,"getCcdSweptSphereRadius",lua_ax_physics3d_Physics3DRigidBody_getCcdSweptSphereRadius);
        tolua_function(tolua_S,"setKinematic",lua_ax_physics3d_Physics3DRigidBody_setKinematic);
        tolua_function(tolua_S,"isKinematic",lua_ax_physics3d_Physics3DRigidBody_isKinematic);
        tolua_function(tolua_S,"getConstraint",lua_ax_physics3d_Physics3DRigidBody_getConstraint);
        tolua_function(tolua_S,"getConstraintCount",lua_ax_physics3d_Physics3DRigidBody_getConstraintCount);
        tolua_function(tolua_S,"setActive",lua_ax_physics3d_Physics3DRigidBody_setActive);
        tolua_function(tolua_S,"init",lua_ax_physics3d_Physics3DRigidBody_init);
        tolua_function(tolua_S,"addConstraint",lua_ax_physics3d_Physics3DRigidBody_addConstraint);
        tolua_function(tolua_S,"removeConstraint",lua_ax_physics3d_Physics3DRigidBody_removeConstraint);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DRigidBody).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DRigidBody";
    g_typeCast[typeName] = "ax.Physics3DRigidBody";
    return 1;
}

int lua_ax_physics3d_Physics3DComponent_setPhysics3DObject(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_setPhysics3DObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DObject* arg0;

        ok &= luaval_to_object<ax::Physics3DObject>(tolua_S, 2, "ax.Physics3DObject",&arg0, "ax.Physics3DComponent:setPhysics3DObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_setPhysics3DObject'", nullptr);
            return 0;
        }
        cobj->setPhysics3DObject(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:setPhysics3DObject",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_setPhysics3DObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_getPhysics3DObject(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_getPhysics3DObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_getPhysics3DObject'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPhysics3DObject();
        object_to_luaval<ax::Physics3DObject>(tolua_S, "ax.Physics3DObject",(ax::Physics3DObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:getPhysics3DObject",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_getPhysics3DObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_addToPhysicsWorld(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_addToPhysicsWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DWorld* arg0;

        ok &= luaval_to_object<ax::Physics3DWorld>(tolua_S, 2, "ax.Physics3DWorld",&arg0, "ax.Physics3DComponent:addToPhysicsWorld");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_addToPhysicsWorld'", nullptr);
            return 0;
        }
        cobj->addToPhysicsWorld(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:addToPhysicsWorld",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_addToPhysicsWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_setSyncFlag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_setSyncFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DComponent::PhysicsSyncFlag arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Physics3DComponent:setSyncFlag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_setSyncFlag'", nullptr);
            return 0;
        }
        cobj->setSyncFlag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:setSyncFlag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_setSyncFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_syncNodeToPhysics(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_syncNodeToPhysics'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_syncNodeToPhysics'", nullptr);
            return 0;
        }
        cobj->syncNodeToPhysics();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:syncNodeToPhysics",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_syncNodeToPhysics'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_syncPhysicsToNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DComponent_syncPhysicsToNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_syncPhysicsToNode'", nullptr);
            return 0;
        }
        cobj->syncPhysicsToNode();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:syncPhysicsToNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_syncPhysicsToNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DComponent_getPhysics3DComponentName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_getPhysics3DComponentName'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DComponent::getPhysics3DComponentName();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DComponent:getPhysics3DComponentName",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_getPhysics3DComponentName'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DComponent_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DComponent_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DComponent();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DComponent");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DComponent:Physics3DComponent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DComponent_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DComponent_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DComponent)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DComponent(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DComponent");
    tolua_cclass(tolua_S,"Physics3DComponent","ax.Physics3DComponent","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DComponent");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DComponent_constructor);
        tolua_function(tolua_S,"setPhysics3DObject",lua_ax_physics3d_Physics3DComponent_setPhysics3DObject);
        tolua_function(tolua_S,"getPhysics3DObject",lua_ax_physics3d_Physics3DComponent_getPhysics3DObject);
        tolua_function(tolua_S,"addToPhysicsWorld",lua_ax_physics3d_Physics3DComponent_addToPhysicsWorld);
        tolua_function(tolua_S,"setSyncFlag",lua_ax_physics3d_Physics3DComponent_setSyncFlag);
        tolua_function(tolua_S,"syncNodeToPhysics",lua_ax_physics3d_Physics3DComponent_syncNodeToPhysics);
        tolua_function(tolua_S,"syncPhysicsToNode",lua_ax_physics3d_Physics3DComponent_syncPhysicsToNode);
        tolua_function(tolua_S,"getPhysics3DComponentName", lua_ax_physics3d_Physics3DComponent_getPhysics3DComponentName);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DComponent).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DComponent";
    g_typeCast[typeName] = "ax.Physics3DComponent";
    return 1;
}

int lua_ax_physics3d_PhysicsMeshRenderer_getPhysicsObj(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsMeshRenderer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsMeshRenderer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsMeshRenderer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_PhysicsMeshRenderer_getPhysicsObj'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsMeshRenderer_getPhysicsObj'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPhysicsObj();
        object_to_luaval<ax::Physics3DObject>(tolua_S, "ax.Physics3DObject",(ax::Physics3DObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsMeshRenderer:getPhysicsObj",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsMeshRenderer_getPhysicsObj'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsMeshRenderer_setSyncFlag(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsMeshRenderer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsMeshRenderer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsMeshRenderer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_PhysicsMeshRenderer_setSyncFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DComponent::PhysicsSyncFlag arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.PhysicsMeshRenderer:setSyncFlag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsMeshRenderer_setSyncFlag'", nullptr);
            return 0;
        }
        cobj->setSyncFlag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsMeshRenderer:setSyncFlag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsMeshRenderer_setSyncFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsMeshRenderer_syncNodeToPhysics(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsMeshRenderer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsMeshRenderer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsMeshRenderer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncNodeToPhysics'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncNodeToPhysics'", nullptr);
            return 0;
        }
        cobj->syncNodeToPhysics();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsMeshRenderer:syncNodeToPhysics",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncNodeToPhysics'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsMeshRenderer_syncPhysicsToNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsMeshRenderer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PhysicsMeshRenderer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PhysicsMeshRenderer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncPhysicsToNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncPhysicsToNode'", nullptr);
            return 0;
        }
        cobj->syncPhysicsToNode();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsMeshRenderer:syncPhysicsToNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsMeshRenderer_syncPhysicsToNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_PhysicsMeshRenderer_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PhysicsMeshRenderer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_PhysicsMeshRenderer_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::PhysicsMeshRenderer();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.PhysicsMeshRenderer");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PhysicsMeshRenderer:PhysicsMeshRenderer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_PhysicsMeshRenderer_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_PhysicsMeshRenderer_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PhysicsMeshRenderer)");
    return 0;
}

int lua_register_ax_physics3d_PhysicsMeshRenderer(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PhysicsMeshRenderer");
    tolua_cclass(tolua_S,"PhysicsMeshRenderer","ax.PhysicsMeshRenderer","ax.MeshRenderer",nullptr);

    tolua_beginmodule(tolua_S,"PhysicsMeshRenderer");
        tolua_function(tolua_S,"new",lua_ax_physics3d_PhysicsMeshRenderer_constructor);
        tolua_function(tolua_S,"getPhysicsObj",lua_ax_physics3d_PhysicsMeshRenderer_getPhysicsObj);
        tolua_function(tolua_S,"setSyncFlag",lua_ax_physics3d_PhysicsMeshRenderer_setSyncFlag);
        tolua_function(tolua_S,"syncNodeToPhysics",lua_ax_physics3d_PhysicsMeshRenderer_syncNodeToPhysics);
        tolua_function(tolua_S,"syncPhysicsToNode",lua_ax_physics3d_PhysicsMeshRenderer_syncPhysicsToNode);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PhysicsMeshRenderer).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PhysicsMeshRenderer";
    g_typeCast[typeName] = "ax.PhysicsMeshRenderer";
    return 1;
}

int lua_ax_physics3d_Physics3DWorld_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DWorld:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_setGravity'", nullptr);
            return 0;
        }
        cobj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:setGravity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_getGravity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGravity();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:getGravity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_addPhysics3DObject(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DObject* arg0;

        ok &= luaval_to_object<ax::Physics3DObject>(tolua_S, 2, "ax.Physics3DObject",&arg0, "ax.Physics3DWorld:addPhysics3DObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DObject'", nullptr);
            return 0;
        }
        cobj->addPhysics3DObject(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:addPhysics3DObject",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_removePhysics3DObject(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DObject* arg0;

        ok &= luaval_to_object<ax::Physics3DObject>(tolua_S, 2, "ax.Physics3DObject",&arg0, "ax.Physics3DWorld:removePhysics3DObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DObject'", nullptr);
            return 0;
        }
        cobj->removePhysics3DObject(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:removePhysics3DObject",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DObjects(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DObjects'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DObjects'", nullptr);
            return 0;
        }
        cobj->removeAllPhysics3DObjects();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:removeAllPhysics3DObjects",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DObjects'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DConstraint* arg0;

        ok &= luaval_to_object<ax::Physics3DConstraint>(tolua_S, 2, "ax.Physics3DConstraint",&arg0, "ax.Physics3DWorld:addPhysics3DConstraint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint'", nullptr);
            return 0;
        }
        cobj->addPhysics3DConstraint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Physics3DConstraint* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::Physics3DConstraint>(tolua_S, 2, "ax.Physics3DConstraint",&arg0, "ax.Physics3DWorld:addPhysics3DConstraint");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.Physics3DWorld:addPhysics3DConstraint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint'", nullptr);
            return 0;
        }
        cobj->addPhysics3DConstraint(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:addPhysics3DConstraint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_removePhysics3DConstraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DConstraint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Physics3DConstraint* arg0;

        ok &= luaval_to_object<ax::Physics3DConstraint>(tolua_S, 2, "ax.Physics3DConstraint",&arg0, "ax.Physics3DWorld:removePhysics3DConstraint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DConstraint'", nullptr);
            return 0;
        }
        cobj->removePhysics3DConstraint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:removePhysics3DConstraint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_removePhysics3DConstraint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DConstraints(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DConstraints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DConstraints'", nullptr);
            return 0;
        }
        cobj->removeAllPhysics3DConstraints();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:removeAllPhysics3DConstraints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DConstraints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_stepSimulate(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_stepSimulate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DWorld:stepSimulate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_stepSimulate'", nullptr);
            return 0;
        }
        cobj->stepSimulate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:stepSimulate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_stepSimulate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_setDebugDrawEnable(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_setDebugDrawEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DWorld:setDebugDrawEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_setDebugDrawEnable'", nullptr);
            return 0;
        }
        cobj->setDebugDrawEnable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:setDebugDrawEnable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_setDebugDrawEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_isDebugDrawEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_isDebugDrawEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_isDebugDrawEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDebugDrawEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:isDebugDrawEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_isDebugDrawEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_debugDraw(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_debugDraw'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Renderer* arg0;

        ok &= luaval_to_object<ax::Renderer>(tolua_S, 2, "ax.Renderer",&arg0, "ax.Physics3DWorld:debugDraw");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_debugDraw'", nullptr);
            return 0;
        }
        cobj->debugDraw(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:debugDraw",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_debugDraw'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_collisionChecking(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_collisionChecking'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_collisionChecking'", nullptr);
            return 0;
        }
        cobj->collisionChecking();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:collisionChecking",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_collisionChecking'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_needCollisionChecking(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_needCollisionChecking'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_needCollisionChecking'", nullptr);
            return 0;
        }
        auto&& ret = cobj->needCollisionChecking();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:needCollisionChecking",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_needCollisionChecking'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_setGhostPairCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DWorld",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DWorld*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DWorld_setGhostPairCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_setGhostPairCallback'", nullptr);
            return 0;
        }
        cobj->setGhostPairCallback();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:setGhostPairCallback",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_setGhostPairCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DWorld_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DWorld* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DWorld_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DWorld();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DWorld");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DWorld:Physics3DWorld",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DWorld_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DWorld_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DWorld)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DWorld(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DWorld");
    tolua_cclass(tolua_S,"Physics3DWorld","ax.Physics3DWorld","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DWorld");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DWorld_constructor);
        tolua_function(tolua_S,"setGravity",lua_ax_physics3d_Physics3DWorld_setGravity);
        tolua_function(tolua_S,"getGravity",lua_ax_physics3d_Physics3DWorld_getGravity);
        tolua_function(tolua_S,"addPhysics3DObject",lua_ax_physics3d_Physics3DWorld_addPhysics3DObject);
        tolua_function(tolua_S,"removePhysics3DObject",lua_ax_physics3d_Physics3DWorld_removePhysics3DObject);
        tolua_function(tolua_S,"removeAllPhysics3DObjects",lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DObjects);
        tolua_function(tolua_S,"addPhysics3DConstraint",lua_ax_physics3d_Physics3DWorld_addPhysics3DConstraint);
        tolua_function(tolua_S,"removePhysics3DConstraint",lua_ax_physics3d_Physics3DWorld_removePhysics3DConstraint);
        tolua_function(tolua_S,"removeAllPhysics3DConstraints",lua_ax_physics3d_Physics3DWorld_removeAllPhysics3DConstraints);
        tolua_function(tolua_S,"stepSimulate",lua_ax_physics3d_Physics3DWorld_stepSimulate);
        tolua_function(tolua_S,"setDebugDrawEnable",lua_ax_physics3d_Physics3DWorld_setDebugDrawEnable);
        tolua_function(tolua_S,"isDebugDrawEnabled",lua_ax_physics3d_Physics3DWorld_isDebugDrawEnabled);
        tolua_function(tolua_S,"debugDraw",lua_ax_physics3d_Physics3DWorld_debugDraw);
        tolua_function(tolua_S,"collisionChecking",lua_ax_physics3d_Physics3DWorld_collisionChecking);
        tolua_function(tolua_S,"needCollisionChecking",lua_ax_physics3d_Physics3DWorld_needCollisionChecking);
        tolua_function(tolua_S,"setGhostPairCallback",lua_ax_physics3d_Physics3DWorld_setGhostPairCallback);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DWorld).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DWorld";
    g_typeCast[typeName] = "ax.Physics3DWorld";
    return 1;
}

int lua_ax_physics3d_Physics3DConstraint_getBreakingImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getBreakingImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getBreakingImpulse'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBreakingImpulse();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getBreakingImpulse",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getBreakingImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_setBreakingImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_setBreakingImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConstraint:setBreakingImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_setBreakingImpulse'", nullptr);
            return 0;
        }
        cobj->setBreakingImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:setBreakingImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_setBreakingImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_isEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_isEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_isEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:isEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_isEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_setEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_setEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DConstraint:setEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_setEnabled'", nullptr);
            return 0;
        }
        cobj->setEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:setEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_setEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getBodyA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getBodyA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getBodyA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBodyA();
        object_to_luaval<ax::Physics3DRigidBody>(tolua_S, "ax.Physics3DRigidBody",(ax::Physics3DRigidBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getBodyA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getBodyA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getBodyB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getBodyB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getBodyB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBodyB();
        object_to_luaval<ax::Physics3DRigidBody>(tolua_S, "ax.Physics3DRigidBody",(ax::Physics3DRigidBody*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getBodyB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getBodyB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getConstraintType(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getConstraintType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getConstraintType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getConstraintType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getConstraintType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getConstraintType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_setUserData'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_setUserData'", nullptr);
            return 0;
        }
        cobj->setUserData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_setUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getUserData(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getUserData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUserData();
        #pragma warning NO CONVERSION FROM NATIVE FOR void*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getUserData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getOverrideNumSolverIterations(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getOverrideNumSolverIterations'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getOverrideNumSolverIterations'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOverrideNumSolverIterations();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getOverrideNumSolverIterations",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getOverrideNumSolverIterations'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_setOverrideNumSolverIterations(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_setOverrideNumSolverIterations'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Physics3DConstraint:setOverrideNumSolverIterations");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_setOverrideNumSolverIterations'", nullptr);
            return 0;
        }
        cobj->setOverrideNumSolverIterations(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:setOverrideNumSolverIterations",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_setOverrideNumSolverIterations'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConstraint_getbtContraint(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConstraint_getbtContraint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConstraint_getbtContraint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getbtContraint();
        object_to_luaval<btTypedConstraint>(tolua_S, "btTypedConstraint",(btTypedConstraint*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConstraint:getbtContraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConstraint_getbtContraint'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_physics3d_Physics3DConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DConstraint");
    tolua_cclass(tolua_S,"Physics3DConstraint","ax.Physics3DConstraint","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DConstraint");
        tolua_function(tolua_S,"getBreakingImpulse",lua_ax_physics3d_Physics3DConstraint_getBreakingImpulse);
        tolua_function(tolua_S,"setBreakingImpulse",lua_ax_physics3d_Physics3DConstraint_setBreakingImpulse);
        tolua_function(tolua_S,"isEnabled",lua_ax_physics3d_Physics3DConstraint_isEnabled);
        tolua_function(tolua_S,"setEnabled",lua_ax_physics3d_Physics3DConstraint_setEnabled);
        tolua_function(tolua_S,"getBodyA",lua_ax_physics3d_Physics3DConstraint_getBodyA);
        tolua_function(tolua_S,"getBodyB",lua_ax_physics3d_Physics3DConstraint_getBodyB);
        tolua_function(tolua_S,"getConstraintType",lua_ax_physics3d_Physics3DConstraint_getConstraintType);
        tolua_function(tolua_S,"setUserData",lua_ax_physics3d_Physics3DConstraint_setUserData);
        tolua_function(tolua_S,"getUserData",lua_ax_physics3d_Physics3DConstraint_getUserData);
        tolua_function(tolua_S,"getOverrideNumSolverIterations",lua_ax_physics3d_Physics3DConstraint_getOverrideNumSolverIterations);
        tolua_function(tolua_S,"setOverrideNumSolverIterations",lua_ax_physics3d_Physics3DConstraint_setOverrideNumSolverIterations);
        tolua_function(tolua_S,"getbtContraint",lua_ax_physics3d_Physics3DConstraint_getbtContraint);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DConstraint";
    g_typeCast[typeName] = "ax.Physics3DConstraint";
    return 1;
}

int lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DPointToPointConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DPointToPointConstraint:setPivotPointInA");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInA'", nullptr);
            return 0;
        }
        cobj->setPivotPointInA(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DPointToPointConstraint:setPivotPointInA",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DPointToPointConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DPointToPointConstraint:setPivotPointInB");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInB'", nullptr);
            return 0;
        }
        cobj->setPivotPointInB(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DPointToPointConstraint:setPivotPointInB",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DPointToPointConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPivotPointInA();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DPointToPointConstraint:getPivotPointInA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DPointToPointConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPivotPointInB();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DPointToPointConstraint:getPivotPointInB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::Physics3DPointToPointConstraint*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_init'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 4) {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            ax::Vec3 arg3;
            ok &= luaval_to_vec3(tolua_S, 5, &arg3, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            bool ret = cobj->init(arg0, arg1, arg2, arg3);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DPointToPointConstraint:init");

            if (!ok) { break; }
            bool ret = cobj->init(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.Physics3DPointToPointConstraint:init",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DPointToPointConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 4)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg3;
            ok &= luaval_to_vec3(tolua_S, 5, &arg3, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Physics3DPointToPointConstraint* ret = ax::Physics3DPointToPointConstraint::create(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::Physics3DPointToPointConstraint>(tolua_S, "ax.Physics3DPointToPointConstraint",(ax::Physics3DPointToPointConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DPointToPointConstraint:create");
            if (!ok) { break; }
            ax::Physics3DPointToPointConstraint* ret = ax::Physics3DPointToPointConstraint::create(arg0, arg1);
            object_to_luaval<ax::Physics3DPointToPointConstraint>(tolua_S, "ax.Physics3DPointToPointConstraint",(ax::Physics3DPointToPointConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.Physics3DPointToPointConstraint:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DPointToPointConstraint_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DPointToPointConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DPointToPointConstraint();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DPointToPointConstraint");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DPointToPointConstraint:Physics3DPointToPointConstraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DPointToPointConstraint_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DPointToPointConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DPointToPointConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DPointToPointConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DPointToPointConstraint");
    tolua_cclass(tolua_S,"Physics3DPointToPointConstraint","ax.Physics3DPointToPointConstraint","ax.Physics3DConstraint",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DPointToPointConstraint");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DPointToPointConstraint_constructor);
        tolua_function(tolua_S,"setPivotPointInA",lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInA);
        tolua_function(tolua_S,"setPivotPointInB",lua_ax_physics3d_Physics3DPointToPointConstraint_setPivotPointInB);
        tolua_function(tolua_S,"getPivotPointInA",lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInA);
        tolua_function(tolua_S,"getPivotPointInB",lua_ax_physics3d_Physics3DPointToPointConstraint_getPivotPointInB);
        tolua_function(tolua_S,"init",lua_ax_physics3d_Physics3DPointToPointConstraint_init);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Physics3DPointToPointConstraint_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DPointToPointConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DPointToPointConstraint";
    g_typeCast[typeName] = "ax.Physics3DPointToPointConstraint";
    return 1;
}

int lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetA();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getFrameOffsetA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetB();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getFrameOffsetB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setFrames(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setFrames'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Mat4 arg0;
        ax::Mat4 arg1;

        ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Physics3DHingeConstraint:setFrames");

        ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:setFrames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setFrames'", nullptr);
            return 0;
        }
        cobj->setFrames(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setFrames",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setFrames'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setAngularOnly(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAngularOnly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setAngularOnly");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAngularOnly'", nullptr);
            return 0;
        }
        cobj->setAngularOnly(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setAngularOnly",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAngularOnly'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_enableAngularMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableAngularMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        bool arg0;
        double arg1;
        double arg2;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:enableAngularMotor");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DHingeConstraint:enableAngularMotor");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DHingeConstraint:enableAngularMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableAngularMotor'", nullptr);
            return 0;
        }
        cobj->enableAngularMotor(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:enableAngularMotor",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableAngularMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_enableMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:enableMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableMotor'", nullptr);
            return 0;
        }
        cobj->enableMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:enableMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_enableMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setMaxMotorImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setMaxMotorImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setMaxMotorImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setMaxMotorImpulse'", nullptr);
            return 0;
        }
        cobj->setMaxMotorImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setMaxMotorImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setMaxMotorImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DHingeConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        double arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DHingeConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4) 
    {
        double arg0;
        double arg1;
        double arg2;
        double arg3;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.Physics3DHingeConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 5) 
    {
        double arg0;
        double arg1;
        double arg2;
        double arg3;
        double arg4;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.Physics3DHingeConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.Physics3DHingeConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setLimit",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setAxis(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAxis'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3DHingeConstraint:setAxis");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAxis'", nullptr);
            return 0;
        }
        cobj->setAxis(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setAxis",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setAxis'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getLowerLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getLowerLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getLowerLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLowerLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getLowerLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getLowerLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getUpperLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUpperLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUpperLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUpperLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getUpperLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUpperLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getHingeAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getHingeAngle'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            ax::Mat4 arg0;
            ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Physics3DHingeConstraint:getHingeAngle");

            if (!ok) { break; }
            ax::Mat4 arg1;
            ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:getHingeAngle");

            if (!ok) { break; }
            double ret = cobj->getHingeAngle(arg0, arg1);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            double ret = cobj->getHingeAngle();
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.Physics3DHingeConstraint:getHingeAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getHingeAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getAFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAFrame();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getAFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getBFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getBFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getBFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBFrame();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getBFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getBFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getAngularOnly(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAngularOnly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAngularOnly'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularOnly();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getAngularOnly",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getAngularOnly'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getEnableAngularMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getEnableAngularMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getEnableAngularMotor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getEnableAngularMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getEnableAngularMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getEnableAngularMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getMotorTargetVelosity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMotorTargetVelosity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMotorTargetVelosity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMotorTargetVelosity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getMotorTargetVelosity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMotorTargetVelosity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getMaxMotorImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMaxMotorImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMaxMotorImpulse'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxMotorImpulse();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getMaxMotorImpulse",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getMaxMotorImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_getUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUseFrameOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUseFrameOffset();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:getUseFrameOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_getUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_setUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DHingeConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DHingeConstraint_setUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DHingeConstraint:setUseFrameOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_setUseFrameOffset'", nullptr);
            return 0;
        }
        cobj->setUseFrameOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:setUseFrameOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_setUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DHingeConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            bool arg3;
            ok &= luaval_to_boolean(tolua_S, 5,&arg3, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg1;
            ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg1;
            ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 6)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg3;
            ok &= luaval_to_vec3(tolua_S, 5, &arg3, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg4;
            ok &= luaval_to_vec3(tolua_S, 6, &arg4, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg5;
            ok &= luaval_to_vec3(tolua_S, 7, &arg5, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2, arg3, arg4, arg5);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 7)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg3;
            ok &= luaval_to_vec3(tolua_S, 5, &arg3, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg4;
            ok &= luaval_to_vec3(tolua_S, 6, &arg4, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Vec3 arg5;
            ok &= luaval_to_vec3(tolua_S, 7, &arg5, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            bool arg6;
            ok &= luaval_to_boolean(tolua_S, 8,&arg6, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg2;
            ok &= luaval_to_mat4(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg3;
            ok &= luaval_to_mat4(tolua_S, 5, &arg3, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 5)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg2;
            ok &= luaval_to_mat4(tolua_S, 4, &arg2, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg3;
            ok &= luaval_to_mat4(tolua_S, 5, &arg3, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            bool arg4;
            ok &= luaval_to_boolean(tolua_S, 6,&arg4, "ax.Physics3DHingeConstraint:create");
            if (!ok) { break; }
            ax::Physics3DHingeConstraint* ret = ax::Physics3DHingeConstraint::create(arg0, arg1, arg2, arg3, arg4);
            object_to_luaval<ax::Physics3DHingeConstraint>(tolua_S, "ax.Physics3DHingeConstraint",(ax::Physics3DHingeConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.Physics3DHingeConstraint:create",argc, 4);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DHingeConstraint_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DHingeConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DHingeConstraint_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DHingeConstraint();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DHingeConstraint");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DHingeConstraint:Physics3DHingeConstraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DHingeConstraint_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DHingeConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DHingeConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DHingeConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DHingeConstraint");
    tolua_cclass(tolua_S,"Physics3DHingeConstraint","ax.Physics3DHingeConstraint","ax.Physics3DConstraint",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DHingeConstraint");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DHingeConstraint_constructor);
        tolua_function(tolua_S,"getFrameOffsetA",lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetA);
        tolua_function(tolua_S,"getFrameOffsetB",lua_ax_physics3d_Physics3DHingeConstraint_getFrameOffsetB);
        tolua_function(tolua_S,"setFrames",lua_ax_physics3d_Physics3DHingeConstraint_setFrames);
        tolua_function(tolua_S,"setAngularOnly",lua_ax_physics3d_Physics3DHingeConstraint_setAngularOnly);
        tolua_function(tolua_S,"enableAngularMotor",lua_ax_physics3d_Physics3DHingeConstraint_enableAngularMotor);
        tolua_function(tolua_S,"enableMotor",lua_ax_physics3d_Physics3DHingeConstraint_enableMotor);
        tolua_function(tolua_S,"setMaxMotorImpulse",lua_ax_physics3d_Physics3DHingeConstraint_setMaxMotorImpulse);
        tolua_function(tolua_S,"setLimit",lua_ax_physics3d_Physics3DHingeConstraint_setLimit);
        tolua_function(tolua_S,"setAxis",lua_ax_physics3d_Physics3DHingeConstraint_setAxis);
        tolua_function(tolua_S,"getLowerLimit",lua_ax_physics3d_Physics3DHingeConstraint_getLowerLimit);
        tolua_function(tolua_S,"getUpperLimit",lua_ax_physics3d_Physics3DHingeConstraint_getUpperLimit);
        tolua_function(tolua_S,"getHingeAngle",lua_ax_physics3d_Physics3DHingeConstraint_getHingeAngle);
        tolua_function(tolua_S,"getAFrame",lua_ax_physics3d_Physics3DHingeConstraint_getAFrame);
        tolua_function(tolua_S,"getBFrame",lua_ax_physics3d_Physics3DHingeConstraint_getBFrame);
        tolua_function(tolua_S,"getAngularOnly",lua_ax_physics3d_Physics3DHingeConstraint_getAngularOnly);
        tolua_function(tolua_S,"getEnableAngularMotor",lua_ax_physics3d_Physics3DHingeConstraint_getEnableAngularMotor);
        tolua_function(tolua_S,"getMotorTargetVelosity",lua_ax_physics3d_Physics3DHingeConstraint_getMotorTargetVelosity);
        tolua_function(tolua_S,"getMaxMotorImpulse",lua_ax_physics3d_Physics3DHingeConstraint_getMaxMotorImpulse);
        tolua_function(tolua_S,"getUseFrameOffset",lua_ax_physics3d_Physics3DHingeConstraint_getUseFrameOffset);
        tolua_function(tolua_S,"setUseFrameOffset",lua_ax_physics3d_Physics3DHingeConstraint_setUseFrameOffset);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Physics3DHingeConstraint_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DHingeConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DHingeConstraint";
    g_typeCast[typeName] = "ax.Physics3DHingeConstraint";
    return 1;
}

int lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetA();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getFrameOffsetA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetB();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getFrameOffsetB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getLowerLinLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerLinLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerLinLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLowerLinLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getLowerLinLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerLinLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setLowerLinLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerLinLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setLowerLinLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerLinLimit'", nullptr);
            return 0;
        }
        cobj->setLowerLinLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setLowerLinLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerLinLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getUpperLinLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperLinLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperLinLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUpperLinLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getUpperLinLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperLinLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setUpperLinLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperLinLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setUpperLinLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperLinLimit'", nullptr);
            return 0;
        }
        cobj->setUpperLinLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setUpperLinLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperLinLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getLowerAngLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerAngLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerAngLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLowerAngLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getLowerAngLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLowerAngLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setLowerAngLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerAngLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setLowerAngLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerAngLimit'", nullptr);
            return 0;
        }
        cobj->setLowerAngLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setLowerAngLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setLowerAngLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getUpperAngLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperAngLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperAngLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUpperAngLimit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getUpperAngLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUpperAngLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setUpperAngLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperAngLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setUpperAngLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperAngLimit'", nullptr);
            return 0;
        }
        cobj->setUpperAngLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setUpperAngLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUpperAngLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getUseLinearReferenceFrameA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseLinearReferenceFrameA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseLinearReferenceFrameA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUseLinearReferenceFrameA();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getUseLinearReferenceFrameA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseLinearReferenceFrameA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessDirLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessDirLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionDirLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionDirLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingDirLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingDirLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessDirAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessDirAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionDirAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionDirAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingDirAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingDirAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessLimLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessLimLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionLimLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionLimLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingLimLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingLimLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessLimAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessLimAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionLimAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionLimAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingLimAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingLimAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessOrthoLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessOrthoLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionOrthoLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionOrthoLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoLin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingOrthoLin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingOrthoLin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoftnessOrthoAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getSoftnessOrthoAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRestitutionOrthoAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getRestitutionOrthoAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoAng'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDampingOrthoAng();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getDampingOrthoAng",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessDirLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirLin'", nullptr);
            return 0;
        }
        cobj->setSoftnessDirLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessDirLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionDirLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirLin'", nullptr);
            return 0;
        }
        cobj->setRestitutionDirLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionDirLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingDirLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirLin'", nullptr);
            return 0;
        }
        cobj->setDampingDirLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingDirLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessDirAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirAng'", nullptr);
            return 0;
        }
        cobj->setSoftnessDirAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessDirAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionDirAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirAng'", nullptr);
            return 0;
        }
        cobj->setRestitutionDirAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionDirAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingDirAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirAng'", nullptr);
            return 0;
        }
        cobj->setDampingDirAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingDirAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessLimLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimLin'", nullptr);
            return 0;
        }
        cobj->setSoftnessLimLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessLimLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionLimLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimLin'", nullptr);
            return 0;
        }
        cobj->setRestitutionLimLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionLimLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingLimLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimLin'", nullptr);
            return 0;
        }
        cobj->setDampingLimLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingLimLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessLimAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimAng'", nullptr);
            return 0;
        }
        cobj->setSoftnessLimAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessLimAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionLimAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimAng'", nullptr);
            return 0;
        }
        cobj->setRestitutionLimAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionLimAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingLimAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimAng'", nullptr);
            return 0;
        }
        cobj->setDampingLimAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingLimAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessOrthoLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoLin'", nullptr);
            return 0;
        }
        cobj->setSoftnessOrthoLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessOrthoLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionOrthoLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoLin'", nullptr);
            return 0;
        }
        cobj->setRestitutionOrthoLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionOrthoLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoLin(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoLin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingOrthoLin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoLin'", nullptr);
            return 0;
        }
        cobj->setDampingOrthoLin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingOrthoLin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoLin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setSoftnessOrthoAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoAng'", nullptr);
            return 0;
        }
        cobj->setSoftnessOrthoAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setSoftnessOrthoAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setRestitutionOrthoAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoAng'", nullptr);
            return 0;
        }
        cobj->setRestitutionOrthoAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setRestitutionOrthoAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoAng(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoAng'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setDampingOrthoAng");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoAng'", nullptr);
            return 0;
        }
        cobj->setDampingOrthoAng(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setDampingOrthoAng",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoAng'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setPoweredLinMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredLinMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setPoweredLinMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredLinMotor'", nullptr);
            return 0;
        }
        cobj->setPoweredLinMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setPoweredLinMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredLinMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getPoweredLinMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredLinMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredLinMotor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPoweredLinMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getPoweredLinMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredLinMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setTargetLinMotorVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetLinMotorVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setTargetLinMotorVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetLinMotorVelocity'", nullptr);
            return 0;
        }
        cobj->setTargetLinMotorVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setTargetLinMotorVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetLinMotorVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getTargetLinMotorVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetLinMotorVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetLinMotorVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTargetLinMotorVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getTargetLinMotorVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetLinMotorVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setMaxLinMotorForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxLinMotorForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setMaxLinMotorForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxLinMotorForce'", nullptr);
            return 0;
        }
        cobj->setMaxLinMotorForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setMaxLinMotorForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxLinMotorForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getMaxLinMotorForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxLinMotorForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxLinMotorForce'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxLinMotorForce();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getMaxLinMotorForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxLinMotorForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setPoweredAngMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredAngMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setPoweredAngMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredAngMotor'", nullptr);
            return 0;
        }
        cobj->setPoweredAngMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setPoweredAngMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setPoweredAngMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getPoweredAngMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredAngMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredAngMotor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPoweredAngMotor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getPoweredAngMotor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getPoweredAngMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setTargetAngMotorVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetAngMotorVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setTargetAngMotorVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetAngMotorVelocity'", nullptr);
            return 0;
        }
        cobj->setTargetAngMotorVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setTargetAngMotorVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setTargetAngMotorVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getTargetAngMotorVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetAngMotorVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetAngMotorVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTargetAngMotorVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getTargetAngMotorVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getTargetAngMotorVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setMaxAngMotorForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxAngMotorForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setMaxAngMotorForce");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxAngMotorForce'", nullptr);
            return 0;
        }
        cobj->setMaxAngMotorForce(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setMaxAngMotorForce",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setMaxAngMotorForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getMaxAngMotorForce(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxAngMotorForce'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxAngMotorForce'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxAngMotorForce();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getMaxAngMotorForce",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getMaxAngMotorForce'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getLinearPos(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLinearPos'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLinearPos'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearPos();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getLinearPos",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getLinearPos'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getAngularPos(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getAngularPos'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getAngularPos'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularPos();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getAngularPos",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getAngularPos'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_getUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseFrameOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUseFrameOffset();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:getUseFrameOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_getUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DSliderConstraint:setUseFrameOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUseFrameOffset'", nullptr);
            return 0;
        }
        cobj->setUseFrameOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setUseFrameOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_setFrames(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DSliderConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DSliderConstraint_setFrames'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Mat4 arg0;
        ax::Mat4 arg1;

        ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Physics3DSliderConstraint:setFrames");

        ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DSliderConstraint:setFrames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_setFrames'", nullptr);
            return 0;
        }
        cobj->setFrames(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:setFrames",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_setFrames'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DSliderConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 5)
    {
        ax::Physics3DRigidBody* arg0;
        ax::Physics3DRigidBody* arg1;
        ax::Mat4 arg2;
        ax::Mat4 arg3;
        bool arg4;
        ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DSliderConstraint:create");
        ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DSliderConstraint:create");
        ok &= luaval_to_mat4(tolua_S, 4, &arg2, "ax.Physics3DSliderConstraint:create");
        ok &= luaval_to_mat4(tolua_S, 5, &arg3, "ax.Physics3DSliderConstraint:create");
        ok &= luaval_to_boolean(tolua_S, 6,&arg4, "ax.Physics3DSliderConstraint:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::Physics3DSliderConstraint::create(arg0, arg1, arg2, arg3, arg4);
        object_to_luaval<ax::Physics3DSliderConstraint>(tolua_S, "ax.Physics3DSliderConstraint",(ax::Physics3DSliderConstraint*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Physics3DSliderConstraint:create",argc, 5);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DSliderConstraint_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DSliderConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DSliderConstraint_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DSliderConstraint();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DSliderConstraint");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DSliderConstraint:Physics3DSliderConstraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DSliderConstraint_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DSliderConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DSliderConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DSliderConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DSliderConstraint");
    tolua_cclass(tolua_S,"Physics3DSliderConstraint","ax.Physics3DSliderConstraint","ax.Physics3DConstraint",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DSliderConstraint");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DSliderConstraint_constructor);
        tolua_function(tolua_S,"getFrameOffsetA",lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetA);
        tolua_function(tolua_S,"getFrameOffsetB",lua_ax_physics3d_Physics3DSliderConstraint_getFrameOffsetB);
        tolua_function(tolua_S,"getLowerLinLimit",lua_ax_physics3d_Physics3DSliderConstraint_getLowerLinLimit);
        tolua_function(tolua_S,"setLowerLinLimit",lua_ax_physics3d_Physics3DSliderConstraint_setLowerLinLimit);
        tolua_function(tolua_S,"getUpperLinLimit",lua_ax_physics3d_Physics3DSliderConstraint_getUpperLinLimit);
        tolua_function(tolua_S,"setUpperLinLimit",lua_ax_physics3d_Physics3DSliderConstraint_setUpperLinLimit);
        tolua_function(tolua_S,"getLowerAngLimit",lua_ax_physics3d_Physics3DSliderConstraint_getLowerAngLimit);
        tolua_function(tolua_S,"setLowerAngLimit",lua_ax_physics3d_Physics3DSliderConstraint_setLowerAngLimit);
        tolua_function(tolua_S,"getUpperAngLimit",lua_ax_physics3d_Physics3DSliderConstraint_getUpperAngLimit);
        tolua_function(tolua_S,"setUpperAngLimit",lua_ax_physics3d_Physics3DSliderConstraint_setUpperAngLimit);
        tolua_function(tolua_S,"getUseLinearReferenceFrameA",lua_ax_physics3d_Physics3DSliderConstraint_getUseLinearReferenceFrameA);
        tolua_function(tolua_S,"getSoftnessDirLin",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirLin);
        tolua_function(tolua_S,"getRestitutionDirLin",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirLin);
        tolua_function(tolua_S,"getDampingDirLin",lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirLin);
        tolua_function(tolua_S,"getSoftnessDirAng",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessDirAng);
        tolua_function(tolua_S,"getRestitutionDirAng",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionDirAng);
        tolua_function(tolua_S,"getDampingDirAng",lua_ax_physics3d_Physics3DSliderConstraint_getDampingDirAng);
        tolua_function(tolua_S,"getSoftnessLimLin",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimLin);
        tolua_function(tolua_S,"getRestitutionLimLin",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimLin);
        tolua_function(tolua_S,"getDampingLimLin",lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimLin);
        tolua_function(tolua_S,"getSoftnessLimAng",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessLimAng);
        tolua_function(tolua_S,"getRestitutionLimAng",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionLimAng);
        tolua_function(tolua_S,"getDampingLimAng",lua_ax_physics3d_Physics3DSliderConstraint_getDampingLimAng);
        tolua_function(tolua_S,"getSoftnessOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoLin);
        tolua_function(tolua_S,"getRestitutionOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoLin);
        tolua_function(tolua_S,"getDampingOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoLin);
        tolua_function(tolua_S,"getSoftnessOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_getSoftnessOrthoAng);
        tolua_function(tolua_S,"getRestitutionOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_getRestitutionOrthoAng);
        tolua_function(tolua_S,"getDampingOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_getDampingOrthoAng);
        tolua_function(tolua_S,"setSoftnessDirLin",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirLin);
        tolua_function(tolua_S,"setRestitutionDirLin",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirLin);
        tolua_function(tolua_S,"setDampingDirLin",lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirLin);
        tolua_function(tolua_S,"setSoftnessDirAng",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessDirAng);
        tolua_function(tolua_S,"setRestitutionDirAng",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionDirAng);
        tolua_function(tolua_S,"setDampingDirAng",lua_ax_physics3d_Physics3DSliderConstraint_setDampingDirAng);
        tolua_function(tolua_S,"setSoftnessLimLin",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimLin);
        tolua_function(tolua_S,"setRestitutionLimLin",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimLin);
        tolua_function(tolua_S,"setDampingLimLin",lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimLin);
        tolua_function(tolua_S,"setSoftnessLimAng",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessLimAng);
        tolua_function(tolua_S,"setRestitutionLimAng",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionLimAng);
        tolua_function(tolua_S,"setDampingLimAng",lua_ax_physics3d_Physics3DSliderConstraint_setDampingLimAng);
        tolua_function(tolua_S,"setSoftnessOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoLin);
        tolua_function(tolua_S,"setRestitutionOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoLin);
        tolua_function(tolua_S,"setDampingOrthoLin",lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoLin);
        tolua_function(tolua_S,"setSoftnessOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_setSoftnessOrthoAng);
        tolua_function(tolua_S,"setRestitutionOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_setRestitutionOrthoAng);
        tolua_function(tolua_S,"setDampingOrthoAng",lua_ax_physics3d_Physics3DSliderConstraint_setDampingOrthoAng);
        tolua_function(tolua_S,"setPoweredLinMotor",lua_ax_physics3d_Physics3DSliderConstraint_setPoweredLinMotor);
        tolua_function(tolua_S,"getPoweredLinMotor",lua_ax_physics3d_Physics3DSliderConstraint_getPoweredLinMotor);
        tolua_function(tolua_S,"setTargetLinMotorVelocity",lua_ax_physics3d_Physics3DSliderConstraint_setTargetLinMotorVelocity);
        tolua_function(tolua_S,"getTargetLinMotorVelocity",lua_ax_physics3d_Physics3DSliderConstraint_getTargetLinMotorVelocity);
        tolua_function(tolua_S,"setMaxLinMotorForce",lua_ax_physics3d_Physics3DSliderConstraint_setMaxLinMotorForce);
        tolua_function(tolua_S,"getMaxLinMotorForce",lua_ax_physics3d_Physics3DSliderConstraint_getMaxLinMotorForce);
        tolua_function(tolua_S,"setPoweredAngMotor",lua_ax_physics3d_Physics3DSliderConstraint_setPoweredAngMotor);
        tolua_function(tolua_S,"getPoweredAngMotor",lua_ax_physics3d_Physics3DSliderConstraint_getPoweredAngMotor);
        tolua_function(tolua_S,"setTargetAngMotorVelocity",lua_ax_physics3d_Physics3DSliderConstraint_setTargetAngMotorVelocity);
        tolua_function(tolua_S,"getTargetAngMotorVelocity",lua_ax_physics3d_Physics3DSliderConstraint_getTargetAngMotorVelocity);
        tolua_function(tolua_S,"setMaxAngMotorForce",lua_ax_physics3d_Physics3DSliderConstraint_setMaxAngMotorForce);
        tolua_function(tolua_S,"getMaxAngMotorForce",lua_ax_physics3d_Physics3DSliderConstraint_getMaxAngMotorForce);
        tolua_function(tolua_S,"getLinearPos",lua_ax_physics3d_Physics3DSliderConstraint_getLinearPos);
        tolua_function(tolua_S,"getAngularPos",lua_ax_physics3d_Physics3DSliderConstraint_getAngularPos);
        tolua_function(tolua_S,"getUseFrameOffset",lua_ax_physics3d_Physics3DSliderConstraint_getUseFrameOffset);
        tolua_function(tolua_S,"setUseFrameOffset",lua_ax_physics3d_Physics3DSliderConstraint_setUseFrameOffset);
        tolua_function(tolua_S,"setFrames",lua_ax_physics3d_Physics3DSliderConstraint_setFrames);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Physics3DSliderConstraint_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DSliderConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DSliderConstraint";
    g_typeCast[typeName] = "ax.Physics3DSliderConstraint";
    return 1;
}

int lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        double arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DConeTwistConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4) 
    {
        double arg0;
        double arg1;
        double arg2;
        double arg3;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.Physics3DConeTwistConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 5) 
    {
        double arg0;
        double arg1;
        double arg2;
        double arg3;
        double arg4;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.Physics3DConeTwistConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 6) 
    {
        double arg0;
        double arg1;
        double arg2;
        double arg3;
        double arg4;
        double arg5;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.Physics3DConeTwistConstraint:setLimit");

        ok &= luaval_to_number(tolua_S, 7,&arg5, "ax.Physics3DConeTwistConstraint:setLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'", nullptr);
            return 0;
        }
        cobj->setLimit(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setLimit",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getAFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getAFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getAFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAFrame();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getAFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getAFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getBFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getBFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getBFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBFrame();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getBFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getBFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan1(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan1'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan1'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSwingSpan1();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getSwingSpan1",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan1'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan2(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan2'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan2'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSwingSpan2();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getSwingSpan2",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan2'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistSpan(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistSpan'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistSpan'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTwistSpan();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getTwistSpan",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistSpan'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistAngle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTwistAngle();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getTwistAngle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_setDamping(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setDamping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setDamping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setDamping'", nullptr);
            return 0;
        }
        cobj->setDamping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setDamping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setDamping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_enableMotor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_enableMotor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:enableMotor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_enableMotor'", nullptr);
            return 0;
        }
        cobj->enableMotor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:enableMotor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_enableMotor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulse(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setMaxMotorImpulse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulse'", nullptr);
            return 0;
        }
        cobj->setMaxMotorImpulse(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setMaxMotorImpulse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulseNormalized(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulseNormalized'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setMaxMotorImpulseNormalized");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulseNormalized'", nullptr);
            return 0;
        }
        cobj->setMaxMotorImpulseNormalized(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setMaxMotorImpulseNormalized",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulseNormalized'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getFixThresh(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFixThresh'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFixThresh'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFixThresh();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getFixThresh",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFixThresh'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_setFixThresh(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFixThresh'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:setFixThresh");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFixThresh'", nullptr);
            return 0;
        }
        cobj->setFixThresh(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setFixThresh",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFixThresh'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_GetPointForAngle(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_GetPointForAngle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.Physics3DConeTwistConstraint:GetPointForAngle");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.Physics3DConeTwistConstraint:GetPointForAngle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_GetPointForAngle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->GetPointForAngle(arg0, arg1);
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:GetPointForAngle",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_GetPointForAngle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_setFrames(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFrames'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Mat4 arg0;
        ax::Mat4 arg1;

        ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Physics3DConeTwistConstraint:setFrames");

        ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DConeTwistConstraint:setFrames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFrames'", nullptr);
            return 0;
        }
        cobj->setFrames(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:setFrames",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_setFrames'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetA(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetA'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetA'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetA();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getFrameOffsetA",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetA'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetB(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3DConeTwistConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetB'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrameOffsetB();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:getFrameOffsetB",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetB'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3DConeTwistConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 4)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg2;
            ok &= luaval_to_mat4(tolua_S, 4, &arg2, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg3;
            ok &= luaval_to_mat4(tolua_S, 5, &arg3, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Physics3DConeTwistConstraint* ret = ax::Physics3DConeTwistConstraint::create(arg0, arg1, arg2, arg3);
            object_to_luaval<ax::Physics3DConeTwistConstraint>(tolua_S, "ax.Physics3DConeTwistConstraint",(ax::Physics3DConeTwistConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg1;
            ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3DConeTwistConstraint:create");
            if (!ok) { break; }
            ax::Physics3DConeTwistConstraint* ret = ax::Physics3DConeTwistConstraint::create(arg0, arg1);
            object_to_luaval<ax::Physics3DConeTwistConstraint>(tolua_S, "ax.Physics3DConeTwistConstraint",(ax::Physics3DConeTwistConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.Physics3DConeTwistConstraint:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3DConeTwistConstraint_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3DConeTwistConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3DConeTwistConstraint();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3DConeTwistConstraint");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3DConeTwistConstraint:Physics3DConeTwistConstraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3DConeTwistConstraint_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3DConeTwistConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3DConeTwistConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3DConeTwistConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3DConeTwistConstraint");
    tolua_cclass(tolua_S,"Physics3DConeTwistConstraint","ax.Physics3DConeTwistConstraint","ax.Physics3DConstraint",nullptr);

    tolua_beginmodule(tolua_S,"Physics3DConeTwistConstraint");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3DConeTwistConstraint_constructor);
        tolua_function(tolua_S,"setLimit",lua_ax_physics3d_Physics3DConeTwistConstraint_setLimit);
        tolua_function(tolua_S,"getAFrame",lua_ax_physics3d_Physics3DConeTwistConstraint_getAFrame);
        tolua_function(tolua_S,"getBFrame",lua_ax_physics3d_Physics3DConeTwistConstraint_getBFrame);
        tolua_function(tolua_S,"getSwingSpan1",lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan1);
        tolua_function(tolua_S,"getSwingSpan2",lua_ax_physics3d_Physics3DConeTwistConstraint_getSwingSpan2);
        tolua_function(tolua_S,"getTwistSpan",lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistSpan);
        tolua_function(tolua_S,"getTwistAngle",lua_ax_physics3d_Physics3DConeTwistConstraint_getTwistAngle);
        tolua_function(tolua_S,"setDamping",lua_ax_physics3d_Physics3DConeTwistConstraint_setDamping);
        tolua_function(tolua_S,"enableMotor",lua_ax_physics3d_Physics3DConeTwistConstraint_enableMotor);
        tolua_function(tolua_S,"setMaxMotorImpulse",lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulse);
        tolua_function(tolua_S,"setMaxMotorImpulseNormalized",lua_ax_physics3d_Physics3DConeTwistConstraint_setMaxMotorImpulseNormalized);
        tolua_function(tolua_S,"getFixThresh",lua_ax_physics3d_Physics3DConeTwistConstraint_getFixThresh);
        tolua_function(tolua_S,"setFixThresh",lua_ax_physics3d_Physics3DConeTwistConstraint_setFixThresh);
        tolua_function(tolua_S,"GetPointForAngle",lua_ax_physics3d_Physics3DConeTwistConstraint_GetPointForAngle);
        tolua_function(tolua_S,"setFrames",lua_ax_physics3d_Physics3DConeTwistConstraint_setFrames);
        tolua_function(tolua_S,"getFrameOffsetA",lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetA);
        tolua_function(tolua_S,"getFrameOffsetB",lua_ax_physics3d_Physics3DConeTwistConstraint_getFrameOffsetB);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Physics3DConeTwistConstraint_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3DConeTwistConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3DConeTwistConstraint";
    g_typeCast[typeName] = "ax.Physics3DConeTwistConstraint";
    return 1;
}

int lua_ax_physics3d_Physics3D6DofConstraint_setLinearLowerLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearLowerLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3D6DofConstraint:setLinearLowerLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearLowerLimit'", nullptr);
            return 0;
        }
        cobj->setLinearLowerLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:setLinearLowerLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearLowerLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_getLinearLowerLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearLowerLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearLowerLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearLowerLimit();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:getLinearLowerLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearLowerLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_setLinearUpperLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearUpperLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3D6DofConstraint:setLinearUpperLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearUpperLimit'", nullptr);
            return 0;
        }
        cobj->setLinearUpperLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:setLinearUpperLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_setLinearUpperLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_getLinearUpperLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearUpperLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearUpperLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLinearUpperLimit();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:getLinearUpperLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_getLinearUpperLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_setAngularLowerLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularLowerLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3D6DofConstraint:setAngularLowerLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularLowerLimit'", nullptr);
            return 0;
        }
        cobj->setAngularLowerLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:setAngularLowerLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularLowerLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_getAngularLowerLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularLowerLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularLowerLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularLowerLimit();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:getAngularLowerLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularLowerLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_setAngularUpperLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularUpperLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Physics3D6DofConstraint:setAngularUpperLimit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularUpperLimit'", nullptr);
            return 0;
        }
        cobj->setAngularUpperLimit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:setAngularUpperLimit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_setAngularUpperLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_getAngularUpperLimit(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularUpperLimit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularUpperLimit'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAngularUpperLimit();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:getAngularUpperLimit",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_getAngularUpperLimit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_isLimited(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_isLimited'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Physics3D6DofConstraint:isLimited");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_isLimited'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isLimited(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:isLimited",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_isLimited'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_getUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_getUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_getUseFrameOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUseFrameOffset();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:getUseFrameOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_getUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_setUseFrameOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Physics3D6DofConstraint*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_physics3d_Physics3D6DofConstraint_setUseFrameOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.Physics3D6DofConstraint:setUseFrameOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_setUseFrameOffset'", nullptr);
            return 0;
        }
        cobj->setUseFrameOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:setUseFrameOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_setUseFrameOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Physics3D6DofConstraint",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg1;
            ok &= luaval_to_mat4(tolua_S, 3, &arg1, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Physics3D6DofConstraint* ret = ax::Physics3D6DofConstraint::create(arg0, arg1, arg2);
            object_to_luaval<ax::Physics3D6DofConstraint>(tolua_S, "ax.Physics3D6DofConstraint",(ax::Physics3D6DofConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 5)
        {
            ax::Physics3DRigidBody* arg0;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 2, "ax.Physics3DRigidBody",&arg0, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Physics3DRigidBody* arg1;
            ok &= luaval_to_object<ax::Physics3DRigidBody>(tolua_S, 3, "ax.Physics3DRigidBody",&arg1, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg2;
            ok &= luaval_to_mat4(tolua_S, 4, &arg2, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Mat4 arg3;
            ok &= luaval_to_mat4(tolua_S, 5, &arg3, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            bool arg4;
            ok &= luaval_to_boolean(tolua_S, 6,&arg4, "ax.Physics3D6DofConstraint:create");
            if (!ok) { break; }
            ax::Physics3D6DofConstraint* ret = ax::Physics3D6DofConstraint::create(arg0, arg1, arg2, arg3, arg4);
            object_to_luaval<ax::Physics3D6DofConstraint>(tolua_S, "ax.Physics3D6DofConstraint",(ax::Physics3D6DofConstraint*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.Physics3D6DofConstraint:create",argc, 5);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_physics3d_Physics3D6DofConstraint_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::Physics3D6DofConstraint* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_physics3d_Physics3D6DofConstraint_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::Physics3D6DofConstraint();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.Physics3D6DofConstraint");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Physics3D6DofConstraint:Physics3D6DofConstraint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_physics3d_Physics3D6DofConstraint_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_physics3d_Physics3D6DofConstraint_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Physics3D6DofConstraint)");
    return 0;
}

int lua_register_ax_physics3d_Physics3D6DofConstraint(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Physics3D6DofConstraint");
    tolua_cclass(tolua_S,"Physics3D6DofConstraint","ax.Physics3D6DofConstraint","ax.Physics3DConstraint",nullptr);

    tolua_beginmodule(tolua_S,"Physics3D6DofConstraint");
        tolua_function(tolua_S,"new",lua_ax_physics3d_Physics3D6DofConstraint_constructor);
        tolua_function(tolua_S,"setLinearLowerLimit",lua_ax_physics3d_Physics3D6DofConstraint_setLinearLowerLimit);
        tolua_function(tolua_S,"getLinearLowerLimit",lua_ax_physics3d_Physics3D6DofConstraint_getLinearLowerLimit);
        tolua_function(tolua_S,"setLinearUpperLimit",lua_ax_physics3d_Physics3D6DofConstraint_setLinearUpperLimit);
        tolua_function(tolua_S,"getLinearUpperLimit",lua_ax_physics3d_Physics3D6DofConstraint_getLinearUpperLimit);
        tolua_function(tolua_S,"setAngularLowerLimit",lua_ax_physics3d_Physics3D6DofConstraint_setAngularLowerLimit);
        tolua_function(tolua_S,"getAngularLowerLimit",lua_ax_physics3d_Physics3D6DofConstraint_getAngularLowerLimit);
        tolua_function(tolua_S,"setAngularUpperLimit",lua_ax_physics3d_Physics3D6DofConstraint_setAngularUpperLimit);
        tolua_function(tolua_S,"getAngularUpperLimit",lua_ax_physics3d_Physics3D6DofConstraint_getAngularUpperLimit);
        tolua_function(tolua_S,"isLimited",lua_ax_physics3d_Physics3D6DofConstraint_isLimited);
        tolua_function(tolua_S,"getUseFrameOffset",lua_ax_physics3d_Physics3D6DofConstraint_getUseFrameOffset);
        tolua_function(tolua_S,"setUseFrameOffset",lua_ax_physics3d_Physics3D6DofConstraint_setUseFrameOffset);
        tolua_function(tolua_S,"create", lua_ax_physics3d_Physics3D6DofConstraint_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Physics3D6DofConstraint).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Physics3D6DofConstraint";
    g_typeCast[typeName] = "ax.Physics3D6DofConstraint";
    return 1;
}
TOLUA_API int register_all_ax_physics3d(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_physics3d_Physics3DShape(tolua_S);
	lua_register_ax_physics3d_Physics3DObject(tolua_S);
	lua_register_ax_physics3d_Physics3DRigidBody(tolua_S);
	lua_register_ax_physics3d_Physics3DComponent(tolua_S);
	lua_register_ax_physics3d_PhysicsMeshRenderer(tolua_S);
	lua_register_ax_physics3d_Physics3DWorld(tolua_S);
	lua_register_ax_physics3d_Physics3DConstraint(tolua_S);
	lua_register_ax_physics3d_Physics3DPointToPointConstraint(tolua_S);
	lua_register_ax_physics3d_Physics3DHingeConstraint(tolua_S);
	lua_register_ax_physics3d_Physics3DSliderConstraint(tolua_S);
	lua_register_ax_physics3d_Physics3DConeTwistConstraint(tolua_S);
	lua_register_ax_physics3d_Physics3D6DofConstraint(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

#endif
