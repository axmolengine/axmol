#include "lua-bindings/auto/axlua_sceneext_auto.hpp"
#include "extensions/sceneext/src/sceneext/sceneext.h"
#include "extensions/sceneext/src/sceneext/ComExtensionData.h"
#include "lua-bindings/manual/sceneext/lua-sceneext-conversions.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_sceneext_ActionFrame_setFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_setFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ActionFrame:setFrameIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_setFrameIndex'", nullptr);
            return 0;
        }
        obj->setFrameIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:setFrameIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_setFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_getFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_getFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_getFrameIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrameIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:getFrameIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_getFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_setFrameTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_setFrameTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionFrame:setFrameTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_setFrameTime'", nullptr);
            return 0;
        }
        obj->setFrameTime(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:setFrameTime",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_setFrameTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_getFrameTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_getFrameTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_getFrameTime'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrameTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:getFrameTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_getFrameTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_setFrameType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_setFrameType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ActionFrame:setFrameType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_setFrameType'", nullptr);
            return 0;
        }
        obj->setFrameType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:setFrameType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_setFrameType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_getFrameType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_getFrameType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_getFrameType'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrameType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:getFrameType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_getFrameType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_setEasingType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_setEasingType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ActionFrame:setEasingType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_setEasingType'", nullptr);
            return 0;
        }
        obj->setEasingType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:setEasingType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_setEasingType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_getEasingType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_getEasingType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_getEasingType'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEasingType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:getEasingType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_getEasingType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_getAction'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionFrame:getAction");

            if (!ok) { break; }
            ax::ext::ActionFrame* arg1;
            ok &= luaval_to_object<ax::ext::ActionFrame>(tolua_S, 3, "axext.ActionFrame",&arg1, "axext.ActionFrame:getAction");

            if (!ok) { break; }
            ax::ActionInterval* ret = obj->getAction(arg0, arg1);
            object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionFrame:getAction");

            if (!ok) { break; }
            ax::ActionInterval* ret = obj->getAction(arg0);
            object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ActionFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_setEasingParameter(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFrame_setEasingParameter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::vector<float> arg0;

        ok &= luaval_to_std_vector_float(tolua_S, 2, &arg0, "axext.ActionFrame:setEasingParameter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_setEasingParameter'", nullptr);
            return 0;
        }
        obj->setEasingParameter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:setEasingParameter",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_setEasingParameter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFrame:ActionFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionFrame");
    tolua_cclass(tolua_S,"ActionFrame","axext.ActionFrame","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ActionFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionFrame_constructor);
        tolua_function(tolua_S,"setFrameIndex",lua_ax_sceneext_ActionFrame_setFrameIndex);
        tolua_function(tolua_S,"getFrameIndex",lua_ax_sceneext_ActionFrame_getFrameIndex);
        tolua_function(tolua_S,"setFrameTime",lua_ax_sceneext_ActionFrame_setFrameTime);
        tolua_function(tolua_S,"getFrameTime",lua_ax_sceneext_ActionFrame_getFrameTime);
        tolua_function(tolua_S,"setFrameType",lua_ax_sceneext_ActionFrame_setFrameType);
        tolua_function(tolua_S,"getFrameType",lua_ax_sceneext_ActionFrame_getFrameType);
        tolua_function(tolua_S,"setEasingType",lua_ax_sceneext_ActionFrame_setEasingType);
        tolua_function(tolua_S,"getEasingType",lua_ax_sceneext_ActionFrame_getEasingType);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionFrame_getAction);
        tolua_function(tolua_S,"setEasingParameter",lua_ax_sceneext_ActionFrame_setEasingParameter);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionFrame";
    g_typeCast[typeName] = "axext.ActionFrame";
    return 1;
}

int lua_ax_sceneext_ActionMoveFrame_setPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionMoveFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionMoveFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionMoveFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionMoveFrame_setPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "axext.ActionMoveFrame:setPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionMoveFrame_setPosition'", nullptr);
            return 0;
        }
        obj->setPosition(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionMoveFrame:setPosition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionMoveFrame_setPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionMoveFrame_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionMoveFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionMoveFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionMoveFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionMoveFrame_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionMoveFrame_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionMoveFrame:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionMoveFrame_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionMoveFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionMoveFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionMoveFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionMoveFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionMoveFrame_getAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionMoveFrame:getAction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionMoveFrame_getAction'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAction(arg0);
        object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionMoveFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionMoveFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionMoveFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionMoveFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionMoveFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionMoveFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionMoveFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionMoveFrame:ActionMoveFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionMoveFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionMoveFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionMoveFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionMoveFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionMoveFrame");
    tolua_cclass(tolua_S,"ActionMoveFrame","axext.ActionMoveFrame","axext.ActionFrame",nullptr);

    tolua_beginmodule(tolua_S,"ActionMoveFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionMoveFrame_constructor);
        tolua_function(tolua_S,"setPosition",lua_ax_sceneext_ActionMoveFrame_setPosition);
        tolua_function(tolua_S,"getPosition",lua_ax_sceneext_ActionMoveFrame_getPosition);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionMoveFrame_getAction);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionMoveFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionMoveFrame";
    g_typeCast[typeName] = "axext.ActionMoveFrame";
    return 1;
}

int lua_ax_sceneext_ActionScaleFrame_setScaleX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionScaleFrame_setScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionScaleFrame:setScaleX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_setScaleX'", nullptr);
            return 0;
        }
        obj->setScaleX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:setScaleX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_setScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionScaleFrame_getScaleX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionScaleFrame_getScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_getScaleX'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScaleX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:getScaleX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_getScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionScaleFrame_setScaleY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionScaleFrame_setScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionScaleFrame:setScaleY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_setScaleY'", nullptr);
            return 0;
        }
        obj->setScaleY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:setScaleY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_setScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionScaleFrame_getScaleY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionScaleFrame_getScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_getScaleY'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScaleY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:getScaleY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_getScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionScaleFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionScaleFrame_getAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionScaleFrame:getAction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_getAction'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAction(arg0);
        object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionScaleFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionScaleFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionScaleFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionScaleFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionScaleFrame:ActionScaleFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionScaleFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionScaleFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionScaleFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionScaleFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionScaleFrame");
    tolua_cclass(tolua_S,"ActionScaleFrame","axext.ActionScaleFrame","axext.ActionFrame",nullptr);

    tolua_beginmodule(tolua_S,"ActionScaleFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionScaleFrame_constructor);
        tolua_function(tolua_S,"setScaleX",lua_ax_sceneext_ActionScaleFrame_setScaleX);
        tolua_function(tolua_S,"getScaleX",lua_ax_sceneext_ActionScaleFrame_getScaleX);
        tolua_function(tolua_S,"setScaleY",lua_ax_sceneext_ActionScaleFrame_setScaleY);
        tolua_function(tolua_S,"getScaleY",lua_ax_sceneext_ActionScaleFrame_getScaleY);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionScaleFrame_getAction);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionScaleFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionScaleFrame";
    g_typeCast[typeName] = "axext.ActionScaleFrame";
    return 1;
}

int lua_ax_sceneext_ActionRotationFrame_setRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionRotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionRotationFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionRotationFrame_setRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionRotationFrame:setRotation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionRotationFrame_setRotation'", nullptr);
            return 0;
        }
        obj->setRotation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionRotationFrame:setRotation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionRotationFrame_setRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionRotationFrame_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionRotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionRotationFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionRotationFrame_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionRotationFrame_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRotation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionRotationFrame:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionRotationFrame_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionRotationFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionRotationFrame* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ActionRotationFrame*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionRotationFrame_getAction'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionRotationFrame:getAction");

            if (!ok) { break; }
            ax::ext::ActionFrame* arg1;
            ok &= luaval_to_object<ax::ext::ActionFrame>(tolua_S, 3, "axext.ActionFrame",&arg1, "axext.ActionRotationFrame:getAction");

            if (!ok) { break; }
            ax::ActionInterval* ret = obj->getAction(arg0, arg1);
            object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionRotationFrame:getAction");

            if (!ok) { break; }
            ax::ActionInterval* ret = obj->getAction(arg0);
            object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ActionRotationFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionRotationFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionRotationFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionRotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionRotationFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionRotationFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionRotationFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionRotationFrame:ActionRotationFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionRotationFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionRotationFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionRotationFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionRotationFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionRotationFrame");
    tolua_cclass(tolua_S,"ActionRotationFrame","axext.ActionRotationFrame","axext.ActionFrame",nullptr);

    tolua_beginmodule(tolua_S,"ActionRotationFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionRotationFrame_constructor);
        tolua_function(tolua_S,"setRotation",lua_ax_sceneext_ActionRotationFrame_setRotation);
        tolua_function(tolua_S,"getRotation",lua_ax_sceneext_ActionRotationFrame_getRotation);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionRotationFrame_getAction);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionRotationFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionRotationFrame";
    g_typeCast[typeName] = "axext.ActionRotationFrame";
    return 1;
}

int lua_ax_sceneext_ActionFadeFrame_setOpacity(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFadeFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFadeFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFadeFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFadeFrame_setOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ActionFadeFrame:setOpacity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFadeFrame_setOpacity'", nullptr);
            return 0;
        }
        obj->setOpacity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFadeFrame:setOpacity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFadeFrame_setOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFadeFrame_getOpacity(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFadeFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFadeFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFadeFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFadeFrame_getOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFadeFrame_getOpacity'", nullptr);
            return 0;
        }
        auto&& ret = obj->getOpacity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFadeFrame:getOpacity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFadeFrame_getOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFadeFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFadeFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionFadeFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionFadeFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionFadeFrame_getAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionFadeFrame:getAction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFadeFrame_getAction'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAction(arg0);
        object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFadeFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFadeFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionFadeFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionFadeFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionFadeFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionFadeFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionFadeFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionFadeFrame:ActionFadeFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionFadeFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionFadeFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionFadeFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionFadeFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionFadeFrame");
    tolua_cclass(tolua_S,"ActionFadeFrame","axext.ActionFadeFrame","axext.ActionFrame",nullptr);

    tolua_beginmodule(tolua_S,"ActionFadeFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionFadeFrame_constructor);
        tolua_function(tolua_S,"setOpacity",lua_ax_sceneext_ActionFadeFrame_setOpacity);
        tolua_function(tolua_S,"getOpacity",lua_ax_sceneext_ActionFadeFrame_getOpacity);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionFadeFrame_getAction);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionFadeFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionFadeFrame";
    g_typeCast[typeName] = "axext.ActionFadeFrame";
    return 1;
}

int lua_ax_sceneext_ActionTintFrame_setColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionTintFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionTintFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionTintFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTintFrame_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Color32 arg0;

        ok &=luaval_to_color32(tolua_S, 2, &arg0, "axext.ActionTintFrame:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTintFrame_setColor'", nullptr);
            return 0;
        }
        obj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionTintFrame:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTintFrame_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTintFrame_getColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionTintFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionTintFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionTintFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTintFrame_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTintFrame_getColor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColor();
        color32_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionTintFrame:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTintFrame_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTintFrame_getAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionTintFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionTintFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionTintFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTintFrame_getAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionTintFrame:getAction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTintFrame_getAction'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAction(arg0);
        object_to_luaval<ax::ActionInterval>(tolua_S, "ax.ActionInterval",(ax::ActionInterval*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionTintFrame:getAction",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTintFrame_getAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTintFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionTintFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTintFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionTintFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionTintFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionTintFrame:ActionTintFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTintFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionTintFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionTintFrame)");
    return 0;
}

int lua_register_ax_sceneext_ActionTintFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionTintFrame");
    tolua_cclass(tolua_S,"ActionTintFrame","axext.ActionTintFrame","axext.ActionFrame",nullptr);

    tolua_beginmodule(tolua_S,"ActionTintFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionTintFrame_constructor);
        tolua_function(tolua_S,"setColor",lua_ax_sceneext_ActionTintFrame_setColor);
        tolua_function(tolua_S,"getColor",lua_ax_sceneext_ActionTintFrame_getColor);
        tolua_function(tolua_S,"getAction",lua_ax_sceneext_ActionTintFrame_getAction);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionTintFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionTintFrame";
    g_typeCast[typeName] = "axext.ActionTintFrame";
    return 1;
}

int lua_ax_sceneext_ActionObject_setName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_setName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ActionObject:setName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_setName'", nullptr);
            return 0;
        }
        obj->setName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:setName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_setName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_getName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_getName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_getName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:getName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_getName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_setLoop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_setLoop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.ActionObject:setLoop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_setLoop'", nullptr);
            return 0;
        }
        obj->setLoop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:setLoop",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_setLoop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_getLoop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_getLoop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_getLoop'", nullptr);
            return 0;
        }
        auto&& ret = obj->getLoop();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:getLoop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_getLoop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_setUnitTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_setUnitTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionObject:setUnitTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_setUnitTime'", nullptr);
            return 0;
        }
        obj->setUnitTime(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:setUnitTime",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_setUnitTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_getUnitTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_getUnitTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_getUnitTime'", nullptr);
            return 0;
        }
        auto&& ret = obj->getUnitTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:getUnitTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_getUnitTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_setCurrentTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_setCurrentTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionObject:setCurrentTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_setCurrentTime'", nullptr);
            return 0;
        }
        obj->setCurrentTime(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:setCurrentTime",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_setCurrentTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_getCurrentTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_getCurrentTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_getCurrentTime'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCurrentTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:getCurrentTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_getCurrentTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_getTotalTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_getTotalTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_getTotalTime'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTotalTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:getTotalTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_getTotalTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = obj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_play(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_play'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            ax::CallFunc* arg0;
            ok &= luaval_to_object<ax::CallFunc>(tolua_S, 2, "ax.CallFunc",&arg0, "axext.ActionObject:play");

            if (!ok) { break; }
            obj->play(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 0) {
            obj->play();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ActionObject:play",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_play'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_pause(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_pause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_pause'", nullptr);
            return 0;
        }
        obj->pause();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:pause",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_pause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_stop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_stop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_stop'", nullptr);
            return 0;
        }
        obj->stop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:stop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_stop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_addActionNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_addActionNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ActionNode* arg0;

        ok &= luaval_to_object<ax::ext::ActionNode>(tolua_S, 2, "axext.ActionNode",&arg0, "axext.ActionObject:addActionNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_addActionNode'", nullptr);
            return 0;
        }
        obj->addActionNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:addActionNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_addActionNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_removeActionNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_removeActionNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ActionNode* arg0;

        ok &= luaval_to_object<ax::ext::ActionNode>(tolua_S, 2, "axext.ActionNode",&arg0, "axext.ActionObject:removeActionNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_removeActionNode'", nullptr);
            return 0;
        }
        obj->removeActionNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:removeActionNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_removeActionNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_updateToFrameByTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_updateToFrameByTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionObject:updateToFrameByTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_updateToFrameByTime'", nullptr);
            return 0;
        }
        obj->updateToFrameByTime(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:updateToFrameByTime",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_updateToFrameByTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_simulationActionUpdate(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionObject",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionObject_simulationActionUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ActionObject:simulationActionUpdate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_simulationActionUpdate'", nullptr);
            return 0;
        }
        obj->simulationActionUpdate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:simulationActionUpdate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_simulationActionUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionObject_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionObject* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionObject_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ActionObject();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ActionObject");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionObject:ActionObject",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionObject_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionObject_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionObject)");
    return 0;
}

int lua_register_ax_sceneext_ActionObject(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionObject");
    tolua_cclass(tolua_S,"ActionObject","axext.ActionObject","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ActionObject");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionObject_constructor);
        tolua_function(tolua_S,"setName",lua_ax_sceneext_ActionObject_setName);
        tolua_function(tolua_S,"getName",lua_ax_sceneext_ActionObject_getName);
        tolua_function(tolua_S,"setLoop",lua_ax_sceneext_ActionObject_setLoop);
        tolua_function(tolua_S,"getLoop",lua_ax_sceneext_ActionObject_getLoop);
        tolua_function(tolua_S,"setUnitTime",lua_ax_sceneext_ActionObject_setUnitTime);
        tolua_function(tolua_S,"getUnitTime",lua_ax_sceneext_ActionObject_getUnitTime);
        tolua_function(tolua_S,"setCurrentTime",lua_ax_sceneext_ActionObject_setCurrentTime);
        tolua_function(tolua_S,"getCurrentTime",lua_ax_sceneext_ActionObject_getCurrentTime);
        tolua_function(tolua_S,"getTotalTime",lua_ax_sceneext_ActionObject_getTotalTime);
        tolua_function(tolua_S,"isPlaying",lua_ax_sceneext_ActionObject_isPlaying);
        tolua_function(tolua_S,"play",lua_ax_sceneext_ActionObject_play);
        tolua_function(tolua_S,"pause",lua_ax_sceneext_ActionObject_pause);
        tolua_function(tolua_S,"stop",lua_ax_sceneext_ActionObject_stop);
        tolua_function(tolua_S,"addActionNode",lua_ax_sceneext_ActionObject_addActionNode);
        tolua_function(tolua_S,"removeActionNode",lua_ax_sceneext_ActionObject_removeActionNode);
        tolua_function(tolua_S,"updateToFrameByTime",lua_ax_sceneext_ActionObject_updateToFrameByTime);
        tolua_function(tolua_S,"simulationActionUpdate",lua_ax_sceneext_ActionObject_simulationActionUpdate);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionObject).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionObject";
    g_typeCast[typeName] = "axext.ActionObject";
    return 1;
}

int lua_ax_sceneext_ActionManagerEx_getActionByName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionManagerEx* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionManagerEx_getActionByName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ActionManagerEx:getActionByName");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ActionManagerEx:getActionByName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_getActionByName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionByName(arg0, arg1);
        object_to_luaval<ax::ext::ActionObject>(tolua_S, "axext.ActionObject",(ax::ext::ActionObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionManagerEx:getActionByName",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_getActionByName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionManagerEx_playActionByName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionManagerEx* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ActionManagerEx*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionManagerEx_playActionByName'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 3) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ActionManagerEx:playActionByName");

            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ActionManagerEx:playActionByName");

            if (!ok) { break; }
            ax::CallFunc* arg2;
            ok &= luaval_to_object<ax::CallFunc>(tolua_S, 4, "ax.CallFunc",&arg2, "axext.ActionManagerEx:playActionByName");

            if (!ok) { break; }
            ax::ext::ActionObject* ret = obj->playActionByName(arg0, arg1, arg2);
            object_to_luaval<ax::ext::ActionObject>(tolua_S, "axext.ActionObject",(ax::ext::ActionObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ActionManagerEx:playActionByName");

            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ActionManagerEx:playActionByName");

            if (!ok) { break; }
            ax::ext::ActionObject* ret = obj->playActionByName(arg0, arg1);
            object_to_luaval<ax::ext::ActionObject>(tolua_S, "axext.ActionObject",(ax::ext::ActionObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ActionManagerEx:playActionByName",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_playActionByName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionManagerEx_stopActionByName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionManagerEx* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionManagerEx_stopActionByName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ActionManagerEx:stopActionByName");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ActionManagerEx:stopActionByName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_stopActionByName'", nullptr);
            return 0;
        }
        auto&& ret = obj->stopActionByName(arg0, arg1);
        object_to_luaval<ax::ext::ActionObject>(tolua_S, "axext.ActionObject",(ax::ext::ActionObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionManagerEx:stopActionByName",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_stopActionByName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionManagerEx_releaseActions(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionManagerEx* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionManagerEx_releaseActions'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_releaseActions'", nullptr);
            return 0;
        }
        obj->releaseActions();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionManagerEx:releaseActions",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_releaseActions'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionManagerEx_getStudioVersionNumber(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ActionManagerEx* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ActionManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionManagerEx_getStudioVersionNumber'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_getStudioVersionNumber'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStudioVersionNumber();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ActionManagerEx:getStudioVersionNumber",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_getStudioVersionNumber'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionManagerEx_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ActionManagerEx::getInstance();
        object_to_luaval<ax::ext::ActionManagerEx>(tolua_S, "axext.ActionManagerEx",(ax::ext::ActionManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ActionManagerEx:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ActionManagerEx_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ActionManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionManagerEx_destroyInstance'", nullptr);
            return 0;
        }
        ax::ext::ActionManagerEx::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ActionManagerEx:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionManagerEx_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_ActionManagerEx_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionManagerEx)");
    return 0;
}

int lua_register_ax_sceneext_ActionManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ActionManagerEx");
    tolua_cclass(tolua_S,"ActionManagerEx","axext.ActionManagerEx","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ActionManagerEx");
        tolua_function(tolua_S,"getActionByName",lua_ax_sceneext_ActionManagerEx_getActionByName);
        tolua_function(tolua_S,"playActionByName",lua_ax_sceneext_ActionManagerEx_playActionByName);
        tolua_function(tolua_S,"stopActionByName",lua_ax_sceneext_ActionManagerEx_stopActionByName);
        tolua_function(tolua_S,"releaseActions",lua_ax_sceneext_ActionManagerEx_releaseActions);
        tolua_function(tolua_S,"getStudioVersionNumber",lua_ax_sceneext_ActionManagerEx_getStudioVersionNumber);
        tolua_function(tolua_S,"getInstance", lua_ax_sceneext_ActionManagerEx_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_sceneext_ActionManagerEx_destroyInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ActionManagerEx).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ActionManagerEx";
    g_typeCast[typeName] = "axext.ActionManagerEx";
    return 1;
}

int lua_ax_sceneext_BaseData_setColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BaseData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BaseData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BaseData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BaseData_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Color32 arg0;

        ok &=luaval_to_color32(tolua_S, 2, &arg0, "axext.BaseData:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BaseData_setColor'", nullptr);
            return 0;
        }
        obj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BaseData:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BaseData_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BaseData_getColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BaseData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BaseData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BaseData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BaseData_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BaseData_getColor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColor();
        color32_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BaseData:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BaseData_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BaseData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.BaseData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BaseData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::BaseData::create();
        object_to_luaval<ax::ext::BaseData>(tolua_S, "axext.BaseData",(ax::ext::BaseData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.BaseData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BaseData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_BaseData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BaseData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BaseData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::BaseData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.BaseData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BaseData:BaseData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BaseData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_BaseData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BaseData)");
    return 0;
}

int lua_register_ax_sceneext_BaseData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.BaseData");
    tolua_cclass(tolua_S,"BaseData","axext.BaseData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"BaseData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_BaseData_constructor);
        tolua_function(tolua_S,"setColor",lua_ax_sceneext_BaseData_setColor);
        tolua_function(tolua_S,"getColor",lua_ax_sceneext_BaseData_getColor);
        tolua_function(tolua_S,"create", lua_ax_sceneext_BaseData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::BaseData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.BaseData";
    g_typeCast[typeName] = "axext.BaseData";
    return 1;
}

int lua_ax_sceneext_DisplayData_copy(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayData_copy'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::DisplayData* arg0;

        ok &= luaval_to_object<ax::ext::DisplayData>(tolua_S, 2, "axext.DisplayData",&arg0, "axext.DisplayData:copy");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayData_copy'", nullptr);
            return 0;
        }
        obj->copy(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayData:copy",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayData_copy'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.DisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::DisplayData::create();
        object_to_luaval<ax::ext::DisplayData>(tolua_S, "axext.DisplayData",(ax::ext::DisplayData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.DisplayData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_DisplayData_changeDisplayToTexture(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.DisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.DisplayData:changeDisplayToTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayData_changeDisplayToTexture'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::DisplayData::changeDisplayToTexture(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.DisplayData:changeDisplayToTexture",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayData_changeDisplayToTexture'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_DisplayData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::DisplayData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.DisplayData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayData:DisplayData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_DisplayData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (DisplayData)");
    return 0;
}

int lua_register_ax_sceneext_DisplayData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.DisplayData");
    tolua_cclass(tolua_S,"DisplayData","axext.DisplayData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"DisplayData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_DisplayData_constructor);
        tolua_function(tolua_S,"copy",lua_ax_sceneext_DisplayData_copy);
        tolua_function(tolua_S,"create", lua_ax_sceneext_DisplayData_create);
        tolua_function(tolua_S,"changeDisplayToTexture", lua_ax_sceneext_DisplayData_changeDisplayToTexture);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::DisplayData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.DisplayData";
    g_typeCast[typeName] = "axext.DisplayData";
    return 1;
}

int lua_ax_sceneext_SpriteDisplayData_copy(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SpriteDisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.SpriteDisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::SpriteDisplayData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SpriteDisplayData_copy'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::DisplayData* arg0;

        ok &= luaval_to_object<ax::ext::DisplayData>(tolua_S, 2, "axext.DisplayData",&arg0, "axext.SpriteDisplayData:copy");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SpriteDisplayData_copy'", nullptr);
            return 0;
        }
        obj->copy(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SpriteDisplayData:copy",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SpriteDisplayData_copy'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SpriteDisplayData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.SpriteDisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SpriteDisplayData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::SpriteDisplayData::create();
        object_to_luaval<ax::ext::SpriteDisplayData>(tolua_S, "axext.SpriteDisplayData",(ax::ext::SpriteDisplayData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.SpriteDisplayData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SpriteDisplayData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_SpriteDisplayData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SpriteDisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SpriteDisplayData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::SpriteDisplayData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.SpriteDisplayData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SpriteDisplayData:SpriteDisplayData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SpriteDisplayData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_SpriteDisplayData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SpriteDisplayData)");
    return 0;
}

int lua_register_ax_sceneext_SpriteDisplayData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.SpriteDisplayData");
    tolua_cclass(tolua_S,"SpriteDisplayData","axext.SpriteDisplayData","axext.DisplayData",nullptr);

    tolua_beginmodule(tolua_S,"SpriteDisplayData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_SpriteDisplayData_constructor);
        tolua_function(tolua_S,"copy",lua_ax_sceneext_SpriteDisplayData_copy);
        tolua_function(tolua_S,"create", lua_ax_sceneext_SpriteDisplayData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::SpriteDisplayData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.SpriteDisplayData";
    g_typeCast[typeName] = "axext.SpriteDisplayData";
    return 1;
}

int lua_ax_sceneext_ArmatureDisplayData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ArmatureDisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDisplayData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ArmatureDisplayData::create();
        object_to_luaval<ax::ext::ArmatureDisplayData>(tolua_S, "axext.ArmatureDisplayData",(ax::ext::ArmatureDisplayData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ArmatureDisplayData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDisplayData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ArmatureDisplayData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDisplayData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ArmatureDisplayData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ArmatureDisplayData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDisplayData:ArmatureDisplayData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDisplayData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ArmatureDisplayData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ArmatureDisplayData)");
    return 0;
}

int lua_register_ax_sceneext_ArmatureDisplayData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ArmatureDisplayData");
    tolua_cclass(tolua_S,"ArmatureDisplayData","axext.ArmatureDisplayData","axext.DisplayData",nullptr);

    tolua_beginmodule(tolua_S,"ArmatureDisplayData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ArmatureDisplayData_constructor);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ArmatureDisplayData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ArmatureDisplayData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ArmatureDisplayData";
    g_typeCast[typeName] = "axext.ArmatureDisplayData";
    return 1;
}

int lua_ax_sceneext_ParticleDisplayData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ParticleDisplayData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ParticleDisplayData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ParticleDisplayData::create();
        object_to_luaval<ax::ext::ParticleDisplayData>(tolua_S, "axext.ParticleDisplayData",(ax::ext::ParticleDisplayData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ParticleDisplayData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ParticleDisplayData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ParticleDisplayData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ParticleDisplayData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ParticleDisplayData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ParticleDisplayData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ParticleDisplayData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ParticleDisplayData:ParticleDisplayData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ParticleDisplayData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ParticleDisplayData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ParticleDisplayData)");
    return 0;
}

int lua_register_ax_sceneext_ParticleDisplayData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ParticleDisplayData");
    tolua_cclass(tolua_S,"ParticleDisplayData","axext.ParticleDisplayData","axext.DisplayData",nullptr);

    tolua_beginmodule(tolua_S,"ParticleDisplayData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ParticleDisplayData_constructor);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ParticleDisplayData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ParticleDisplayData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ParticleDisplayData";
    g_typeCast[typeName] = "axext.ParticleDisplayData";
    return 1;
}

int lua_ax_sceneext_BoneData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneData:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneData_addDisplayData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneData_addDisplayData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::DisplayData* arg0;

        ok &= luaval_to_object<ax::ext::DisplayData>(tolua_S, 2, "axext.DisplayData",&arg0, "axext.BoneData:addDisplayData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneData_addDisplayData'", nullptr);
            return 0;
        }
        obj->addDisplayData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneData:addDisplayData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneData_addDisplayData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneData_getDisplayData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneData_getDisplayData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.BoneData:getDisplayData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneData_getDisplayData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDisplayData(arg0);
        object_to_luaval<ax::ext::DisplayData>(tolua_S, "axext.DisplayData",(ax::ext::DisplayData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneData:getDisplayData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneData_getDisplayData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.BoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::BoneData::create();
        object_to_luaval<ax::ext::BoneData>(tolua_S, "axext.BoneData",(ax::ext::BoneData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.BoneData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_BoneData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::BoneData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.BoneData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneData:BoneData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_BoneData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BoneData)");
    return 0;
}

int lua_register_ax_sceneext_BoneData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.BoneData");
    tolua_cclass(tolua_S,"BoneData","axext.BoneData","axext.BaseData",nullptr);

    tolua_beginmodule(tolua_S,"BoneData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_BoneData_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_BoneData_init);
        tolua_function(tolua_S,"addDisplayData",lua_ax_sceneext_BoneData_addDisplayData);
        tolua_function(tolua_S,"getDisplayData",lua_ax_sceneext_BoneData_getDisplayData);
        tolua_function(tolua_S,"create", lua_ax_sceneext_BoneData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::BoneData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.BoneData";
    g_typeCast[typeName] = "axext.BoneData";
    return 1;
}

int lua_ax_sceneext_ArmatureData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureData:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureData_addBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureData_addBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::BoneData* arg0;

        ok &= luaval_to_object<ax::ext::BoneData>(tolua_S, 2, "axext.BoneData",&arg0, "axext.ArmatureData:addBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureData_addBoneData'", nullptr);
            return 0;
        }
        obj->addBoneData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureData:addBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureData_addBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureData_getBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureData_getBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureData:getBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureData_getBoneData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoneData(arg0);
        object_to_luaval<ax::ext::BoneData>(tolua_S, "axext.BoneData",(ax::ext::BoneData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureData:getBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureData_getBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ArmatureData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ArmatureData::create();
        object_to_luaval<ax::ext::ArmatureData>(tolua_S, "axext.ArmatureData",(ax::ext::ArmatureData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ArmatureData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ArmatureData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ArmatureData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ArmatureData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureData:ArmatureData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ArmatureData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ArmatureData)");
    return 0;
}

int lua_register_ax_sceneext_ArmatureData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ArmatureData");
    tolua_cclass(tolua_S,"ArmatureData","axext.ArmatureData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ArmatureData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ArmatureData_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ArmatureData_init);
        tolua_function(tolua_S,"addBoneData",lua_ax_sceneext_ArmatureData_addBoneData);
        tolua_function(tolua_S,"getBoneData",lua_ax_sceneext_ArmatureData_getBoneData);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ArmatureData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ArmatureData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ArmatureData";
    g_typeCast[typeName] = "axext.ArmatureData";
    return 1;
}

int lua_ax_sceneext_FrameData_copy(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::FrameData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.FrameData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::FrameData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_FrameData_copy'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const ax::ext::BaseData* arg0;

        ok &= luaval_to_object<const ax::ext::BaseData>(tolua_S, 2, "axext.BaseData",&arg0, "axext.FrameData:copy");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_FrameData_copy'", nullptr);
            return 0;
        }
        obj->copy(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.FrameData:copy",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_FrameData_copy'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_FrameData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.FrameData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_FrameData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::FrameData::create();
        object_to_luaval<ax::ext::FrameData>(tolua_S, "axext.FrameData",(ax::ext::FrameData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.FrameData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_FrameData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_FrameData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::FrameData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_FrameData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::FrameData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.FrameData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.FrameData:FrameData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_FrameData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_FrameData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (FrameData)");
    return 0;
}

int lua_register_ax_sceneext_FrameData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.FrameData");
    tolua_cclass(tolua_S,"FrameData","axext.FrameData","axext.BaseData",nullptr);

    tolua_beginmodule(tolua_S,"FrameData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_FrameData_constructor);
        tolua_function(tolua_S,"copy",lua_ax_sceneext_FrameData_copy);
        tolua_function(tolua_S,"create", lua_ax_sceneext_FrameData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::FrameData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.FrameData";
    g_typeCast[typeName] = "axext.FrameData";
    return 1;
}

int lua_ax_sceneext_MovementBoneData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementBoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.MovementBoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::MovementBoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_MovementBoneData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementBoneData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementBoneData:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementBoneData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_MovementBoneData_addFrameData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementBoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.MovementBoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::MovementBoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_MovementBoneData_addFrameData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::FrameData* arg0;

        ok &= luaval_to_object<ax::ext::FrameData>(tolua_S, 2, "axext.FrameData",&arg0, "axext.MovementBoneData:addFrameData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementBoneData_addFrameData'", nullptr);
            return 0;
        }
        obj->addFrameData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementBoneData:addFrameData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementBoneData_addFrameData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_MovementBoneData_getFrameData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementBoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.MovementBoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::MovementBoneData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_MovementBoneData_getFrameData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.MovementBoneData:getFrameData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementBoneData_getFrameData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrameData(arg0);
        object_to_luaval<ax::ext::FrameData>(tolua_S, "axext.FrameData",(ax::ext::FrameData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementBoneData:getFrameData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementBoneData_getFrameData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_MovementBoneData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.MovementBoneData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementBoneData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::MovementBoneData::create();
        object_to_luaval<ax::ext::MovementBoneData>(tolua_S, "axext.MovementBoneData",(ax::ext::MovementBoneData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.MovementBoneData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementBoneData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_MovementBoneData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementBoneData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementBoneData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::MovementBoneData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.MovementBoneData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementBoneData:MovementBoneData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementBoneData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_MovementBoneData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (MovementBoneData)");
    return 0;
}

int lua_register_ax_sceneext_MovementBoneData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.MovementBoneData");
    tolua_cclass(tolua_S,"MovementBoneData","axext.MovementBoneData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"MovementBoneData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_MovementBoneData_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_MovementBoneData_init);
        tolua_function(tolua_S,"addFrameData",lua_ax_sceneext_MovementBoneData_addFrameData);
        tolua_function(tolua_S,"getFrameData",lua_ax_sceneext_MovementBoneData_getFrameData);
        tolua_function(tolua_S,"create", lua_ax_sceneext_MovementBoneData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::MovementBoneData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.MovementBoneData";
    g_typeCast[typeName] = "axext.MovementBoneData";
    return 1;
}

int lua_ax_sceneext_MovementData_addMovementBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.MovementData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::MovementData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_MovementData_addMovementBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::MovementBoneData* arg0;

        ok &= luaval_to_object<ax::ext::MovementBoneData>(tolua_S, 2, "axext.MovementBoneData",&arg0, "axext.MovementData:addMovementBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementData_addMovementBoneData'", nullptr);
            return 0;
        }
        obj->addMovementBoneData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementData:addMovementBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementData_addMovementBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_MovementData_getMovementBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.MovementData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::MovementData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_MovementData_getMovementBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.MovementData:getMovementBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementData_getMovementBoneData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMovementBoneData(arg0);
        object_to_luaval<ax::ext::MovementBoneData>(tolua_S, "axext.MovementBoneData",(ax::ext::MovementBoneData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementData:getMovementBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementData_getMovementBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_MovementData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.MovementData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::MovementData::create();
        object_to_luaval<ax::ext::MovementData>(tolua_S, "axext.MovementData",(ax::ext::MovementData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.MovementData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_MovementData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::MovementData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_MovementData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::MovementData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.MovementData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.MovementData:MovementData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_MovementData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_MovementData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (MovementData)");
    return 0;
}

int lua_register_ax_sceneext_MovementData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.MovementData");
    tolua_cclass(tolua_S,"MovementData","axext.MovementData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"MovementData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_MovementData_constructor);
        tolua_function(tolua_S,"addMovementBoneData",lua_ax_sceneext_MovementData_addMovementBoneData);
        tolua_function(tolua_S,"getMovementBoneData",lua_ax_sceneext_MovementData_getMovementBoneData);
        tolua_function(tolua_S,"create", lua_ax_sceneext_MovementData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::MovementData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.MovementData";
    g_typeCast[typeName] = "axext.MovementData";
    return 1;
}

int lua_ax_sceneext_AnimationData_addMovement(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::AnimationData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.AnimationData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::AnimationData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AnimationData_addMovement'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::MovementData* arg0;

        ok &= luaval_to_object<ax::ext::MovementData>(tolua_S, 2, "axext.MovementData",&arg0, "axext.AnimationData:addMovement");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnimationData_addMovement'", nullptr);
            return 0;
        }
        obj->addMovement(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.AnimationData:addMovement",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnimationData_addMovement'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AnimationData_getMovement(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::AnimationData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.AnimationData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::AnimationData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AnimationData_getMovement'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.AnimationData:getMovement");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnimationData_getMovement'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMovement(arg0);
        object_to_luaval<ax::ext::MovementData>(tolua_S, "axext.MovementData",(ax::ext::MovementData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.AnimationData:getMovement",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnimationData_getMovement'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AnimationData_getMovementCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::AnimationData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.AnimationData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::AnimationData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AnimationData_getMovementCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnimationData_getMovementCount'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMovementCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.AnimationData:getMovementCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnimationData_getMovementCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AnimationData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.AnimationData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnimationData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::AnimationData::create();
        object_to_luaval<ax::ext::AnimationData>(tolua_S, "axext.AnimationData",(ax::ext::AnimationData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.AnimationData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnimationData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_AnimationData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::AnimationData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnimationData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::AnimationData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.AnimationData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.AnimationData:AnimationData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnimationData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_AnimationData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (AnimationData)");
    return 0;
}

int lua_register_ax_sceneext_AnimationData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.AnimationData");
    tolua_cclass(tolua_S,"AnimationData","axext.AnimationData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"AnimationData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_AnimationData_constructor);
        tolua_function(tolua_S,"addMovement",lua_ax_sceneext_AnimationData_addMovement);
        tolua_function(tolua_S,"getMovement",lua_ax_sceneext_AnimationData_getMovement);
        tolua_function(tolua_S,"getMovementCount",lua_ax_sceneext_AnimationData_getMovementCount);
        tolua_function(tolua_S,"create", lua_ax_sceneext_AnimationData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::AnimationData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.AnimationData";
    g_typeCast[typeName] = "axext.AnimationData";
    return 1;
}

int lua_ax_sceneext_ContourData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ContourData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ContourData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ContourData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ContourData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ContourData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ContourData:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ContourData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ContourData_addVertex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ContourData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ContourData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ContourData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ContourData_addVertex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "axext.ContourData:addVertex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ContourData_addVertex'", nullptr);
            return 0;
        }
        obj->addVertex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ContourData:addVertex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ContourData_addVertex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ContourData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ContourData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ContourData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ContourData::create();
        object_to_luaval<ax::ext::ContourData>(tolua_S, "axext.ContourData",(ax::ext::ContourData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ContourData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ContourData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ContourData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ContourData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ContourData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ContourData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ContourData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ContourData:ContourData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ContourData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ContourData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ContourData)");
    return 0;
}

int lua_register_ax_sceneext_ContourData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ContourData");
    tolua_cclass(tolua_S,"ContourData","axext.ContourData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ContourData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ContourData_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ContourData_init);
        tolua_function(tolua_S,"addVertex",lua_ax_sceneext_ContourData_addVertex);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ContourData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ContourData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ContourData";
    g_typeCast[typeName] = "axext.ContourData";
    return 1;
}

int lua_ax_sceneext_TextureData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::TextureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.TextureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::TextureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_TextureData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.TextureData:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_TextureData_addContourData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::TextureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.TextureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::TextureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_TextureData_addContourData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ContourData* arg0;

        ok &= luaval_to_object<ax::ext::ContourData>(tolua_S, 2, "axext.ContourData",&arg0, "axext.TextureData:addContourData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureData_addContourData'", nullptr);
            return 0;
        }
        obj->addContourData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.TextureData:addContourData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureData_addContourData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_TextureData_getContourData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::TextureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.TextureData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::TextureData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_TextureData_getContourData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.TextureData:getContourData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureData_getContourData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getContourData(arg0);
        object_to_luaval<ax::ext::ContourData>(tolua_S, "axext.ContourData",(ax::ext::ContourData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.TextureData:getContourData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureData_getContourData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_TextureData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.TextureData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::TextureData::create();
        object_to_luaval<ax::ext::TextureData>(tolua_S, "axext.TextureData",(ax::ext::TextureData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.TextureData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_TextureData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::TextureData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::TextureData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.TextureData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.TextureData:TextureData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_TextureData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (TextureData)");
    return 0;
}

int lua_register_ax_sceneext_TextureData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.TextureData");
    tolua_cclass(tolua_S,"TextureData","axext.TextureData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"TextureData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_TextureData_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_TextureData_init);
        tolua_function(tolua_S,"addContourData",lua_ax_sceneext_TextureData_addContourData);
        tolua_function(tolua_S,"getContourData",lua_ax_sceneext_TextureData_getContourData);
        tolua_function(tolua_S,"create", lua_ax_sceneext_TextureData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::TextureData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.TextureData";
    g_typeCast[typeName] = "axext.TextureData";
    return 1;
}

int lua_ax_sceneext_BoneTweenController_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.BoneTweenController:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_play(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_play'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5)
    {
        ax::ext::MovementBoneData* arg0;
        int arg1;
        int arg2;
        int arg3;
        int arg4;

        ok &= luaval_to_object<ax::ext::MovementBoneData>(tolua_S, 2, "axext.MovementBoneData",&arg0, "axext.BoneTweenController:play");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.BoneTweenController:play");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axext.BoneTweenController:play");

        ok &= luaval_to_int(tolua_S, 5, &arg3, "axext.BoneTweenController:play");

        ok &= luaval_to_int(tolua_S, 6, &arg4, "axext.BoneTweenController:play");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_play'", nullptr);
            return 0;
        }
        obj->play(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:play",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_play'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_setArmatureController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_setArmatureController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ArmatureController* arg0;

        ok &= luaval_to_object<ax::ext::ArmatureController>(tolua_S, 2, "axext.ArmatureController",&arg0, "axext.BoneTweenController:setArmatureController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_setArmatureController'", nullptr);
            return 0;
        }
        obj->setArmatureController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:setArmatureController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_setArmatureController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_getArmatureController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_getArmatureController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_getArmatureController'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmatureController();
        object_to_luaval<ax::ext::ArmatureController>(tolua_S, "axext.ArmatureController",(ax::ext::ArmatureController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:getArmatureController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_getArmatureController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_gotoAndPlay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_gotoAndPlay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.BoneTweenController:gotoAndPlay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_gotoAndPlay'", nullptr);
            return 0;
        }
        obj->gotoAndPlay(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:gotoAndPlay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_gotoAndPlay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_gotoAndPause(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::BoneTweenController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneTweenController_gotoAndPause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.BoneTweenController:gotoAndPause");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_gotoAndPause'", nullptr);
            return 0;
        }
        obj->gotoAndPause(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:gotoAndPause",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_gotoAndPause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneTweenController_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.BoneTweenController",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::ext::Bone* arg0;
        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.BoneTweenController:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::BoneTweenController::create(arg0);
        object_to_luaval<ax::ext::BoneTweenController>(tolua_S, "axext.BoneTweenController",(ax::ext::BoneTweenController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.BoneTweenController:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_BoneTweenController_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::BoneTweenController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneTweenController_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::BoneTweenController();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.BoneTweenController");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.BoneTweenController:BoneTweenController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneTweenController_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_BoneTweenController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BoneTweenController)");
    return 0;
}

int lua_register_ax_sceneext_BoneTweenController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.BoneTweenController");
    tolua_cclass(tolua_S,"BoneTweenController","axext.BoneTweenController","axext.AnimationController",nullptr);

    tolua_beginmodule(tolua_S,"BoneTweenController");
        tolua_function(tolua_S,"new",lua_ax_sceneext_BoneTweenController_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_BoneTweenController_init);
        tolua_function(tolua_S,"play",lua_ax_sceneext_BoneTweenController_play);
        tolua_function(tolua_S,"setArmatureController",lua_ax_sceneext_BoneTweenController_setArmatureController);
        tolua_function(tolua_S,"getArmatureController",lua_ax_sceneext_BoneTweenController_getArmatureController);
        tolua_function(tolua_S,"gotoAndPlay",lua_ax_sceneext_BoneTweenController_gotoAndPlay);
        tolua_function(tolua_S,"gotoAndPause",lua_ax_sceneext_BoneTweenController_gotoAndPause);
        tolua_function(tolua_S,"create", lua_ax_sceneext_BoneTweenController_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::BoneTweenController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.BoneTweenController";
    g_typeCast[typeName] = "axext.BoneTweenController";
    return 1;
}

int lua_ax_sceneext_DisplayManager_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.DisplayManager:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_addDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_addDisplay'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axext.DisplayManager:addDisplay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.DisplayManager:addDisplay");

            if (!ok) { break; }
            obj->addDisplay(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            ax::ext::DisplayData* arg0;
            ok &= luaval_to_object<ax::ext::DisplayData>(tolua_S, 2, "axext.DisplayData",&arg0, "axext.DisplayManager:addDisplay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.DisplayManager:addDisplay");

            if (!ok) { break; }
            obj->addDisplay(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.DisplayManager:addDisplay",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_addDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_removeDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_removeDisplay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.DisplayManager:removeDisplay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_removeDisplay'", nullptr);
            return 0;
        }
        obj->removeDisplay(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:removeDisplay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_removeDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getDecorativeDisplayList(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getDecorativeDisplayList'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getDecorativeDisplayList'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDecorativeDisplayList();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getDecorativeDisplayList",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getDecorativeDisplayList'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_changeDisplayWithIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.DisplayManager:changeDisplayWithIndex");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.DisplayManager:changeDisplayWithIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithIndex'", nullptr);
            return 0;
        }
        obj->changeDisplayWithIndex(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:changeDisplayWithIndex",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_changeDisplayWithName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        bool arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.DisplayManager:changeDisplayWithName");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.DisplayManager:changeDisplayWithName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithName'", nullptr);
            return 0;
        }
        obj->changeDisplayWithName(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:changeDisplayWithName",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_changeDisplayWithName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getDisplayRenderNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDisplayRenderNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getDisplayRenderNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getDisplayRenderNodeType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNodeType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNodeType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getDisplayRenderNodeType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getDisplayRenderNodeType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getDisplayRenderNodeType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getCurrentDisplayIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getCurrentDisplayIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getCurrentDisplayIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCurrentDisplayIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getCurrentDisplayIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getCurrentDisplayIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_setVisible(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_setVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.DisplayManager:setVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_setVisible'", nullptr);
            return 0;
        }
        obj->setVisible(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:setVisible",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_setVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_isVisible(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_isVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_isVisible'", nullptr);
            return 0;
        }
        auto&& ret = obj->isVisible();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:isVisible",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_isVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getContentSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getContentSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getContentSize'", nullptr);
            return 0;
        }
        auto&& ret = obj->getContentSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getContentSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getContentSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getBoundingBox(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getBoundingBox'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getBoundingBox'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoundingBox();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getBoundingBox",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getBoundingBox'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getAnchorPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getAnchorPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getAnchorPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnchorPoint();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getAnchorPoint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getAnchorPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_getAnchorPointInPoints(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_getAnchorPointInPoints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_getAnchorPointInPoints'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnchorPointInPoints();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:getAnchorPointInPoints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_getAnchorPointInPoints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_containPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_containPoint'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.DisplayManager:containPoint");

            if (!ok) { break; }
            double arg1;
            ok &= luaval_to_number(tolua_S, 3, &arg1, "axext.DisplayManager:containPoint");

            if (!ok) { break; }
            bool ret = obj->containPoint(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            ax::Vec2 arg0;
            ok &= luaval_to_vec2(tolua_S, 2, &arg0, "axext.DisplayManager:containPoint");

            if (!ok) { break; }
            bool ret = obj->containPoint(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.DisplayManager:containPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_containPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_setForceChangeDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_setForceChangeDisplay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.DisplayManager:setForceChangeDisplay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_setForceChangeDisplay'", nullptr);
            return 0;
        }
        obj->setForceChangeDisplay(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:setForceChangeDisplay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_setForceChangeDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_isForceChangeDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::DisplayManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_DisplayManager_isForceChangeDisplay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_isForceChangeDisplay'", nullptr);
            return 0;
        }
        auto&& ret = obj->isForceChangeDisplay();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:isForceChangeDisplay",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_isForceChangeDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_DisplayManager_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.DisplayManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::ext::Bone* arg0;
        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.DisplayManager:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::DisplayManager::create(arg0);
        object_to_luaval<ax::ext::DisplayManager>(tolua_S, "axext.DisplayManager",(ax::ext::DisplayManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.DisplayManager:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_DisplayManager_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::DisplayManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_DisplayManager_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::DisplayManager();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.DisplayManager");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.DisplayManager:DisplayManager",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_DisplayManager_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_DisplayManager_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (DisplayManager)");
    return 0;
}

int lua_register_ax_sceneext_DisplayManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.DisplayManager");
    tolua_cclass(tolua_S,"DisplayManager","axext.DisplayManager","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"DisplayManager");
        tolua_function(tolua_S,"new",lua_ax_sceneext_DisplayManager_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_DisplayManager_init);
        tolua_function(tolua_S,"addDisplay",lua_ax_sceneext_DisplayManager_addDisplay);
        tolua_function(tolua_S,"removeDisplay",lua_ax_sceneext_DisplayManager_removeDisplay);
        tolua_function(tolua_S,"getDecorativeDisplayList",lua_ax_sceneext_DisplayManager_getDecorativeDisplayList);
        tolua_function(tolua_S,"changeDisplayWithIndex",lua_ax_sceneext_DisplayManager_changeDisplayWithIndex);
        tolua_function(tolua_S,"changeDisplayWithName",lua_ax_sceneext_DisplayManager_changeDisplayWithName);
        tolua_function(tolua_S,"getDisplayRenderNode",lua_ax_sceneext_DisplayManager_getDisplayRenderNode);
        tolua_function(tolua_S,"getDisplayRenderNodeType",lua_ax_sceneext_DisplayManager_getDisplayRenderNodeType);
        tolua_function(tolua_S,"getCurrentDisplayIndex",lua_ax_sceneext_DisplayManager_getCurrentDisplayIndex);
        tolua_function(tolua_S,"setVisible",lua_ax_sceneext_DisplayManager_setVisible);
        tolua_function(tolua_S,"isVisible",lua_ax_sceneext_DisplayManager_isVisible);
        tolua_function(tolua_S,"getContentSize",lua_ax_sceneext_DisplayManager_getContentSize);
        tolua_function(tolua_S,"getBoundingBox",lua_ax_sceneext_DisplayManager_getBoundingBox);
        tolua_function(tolua_S,"getAnchorPoint",lua_ax_sceneext_DisplayManager_getAnchorPoint);
        tolua_function(tolua_S,"getAnchorPointInPoints",lua_ax_sceneext_DisplayManager_getAnchorPointInPoints);
        tolua_function(tolua_S,"containPoint",lua_ax_sceneext_DisplayManager_containPoint);
        tolua_function(tolua_S,"setForceChangeDisplay",lua_ax_sceneext_DisplayManager_setForceChangeDisplay);
        tolua_function(tolua_S,"isForceChangeDisplay",lua_ax_sceneext_DisplayManager_isForceChangeDisplay);
        tolua_function(tolua_S,"create", lua_ax_sceneext_DisplayManager_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::DisplayManager).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.DisplayManager";
    g_typeCast[typeName] = "axext.DisplayManager";
    return 1;
}

int lua_ax_sceneext_Bone_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Bone:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_addDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_addDisplay'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axext.Bone:addDisplay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.Bone:addDisplay");

            if (!ok) { break; }
            obj->addDisplay(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            ax::ext::DisplayData* arg0;
            ok &= luaval_to_object<ax::ext::DisplayData>(tolua_S, 2, "axext.DisplayData",&arg0, "axext.Bone:addDisplay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.Bone:addDisplay");

            if (!ok) { break; }
            obj->addDisplay(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.Bone:addDisplay",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_addDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_removeDisplay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_removeDisplay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.Bone:removeDisplay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_removeDisplay'", nullptr);
            return 0;
        }
        obj->removeDisplay(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:removeDisplay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_removeDisplay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_changeDisplayWithIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_changeDisplayWithIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.Bone:changeDisplayWithIndex");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.Bone:changeDisplayWithIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_changeDisplayWithIndex'", nullptr);
            return 0;
        }
        obj->changeDisplayWithIndex(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:changeDisplayWithIndex",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_changeDisplayWithIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_changeDisplayWithName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_changeDisplayWithName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        bool arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Bone:changeDisplayWithName");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.Bone:changeDisplayWithName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_changeDisplayWithName'", nullptr);
            return 0;
        }
        obj->changeDisplayWithName(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:changeDisplayWithName",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_changeDisplayWithName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_addChildBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_addChildBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Bone:addChildBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_addChildBone'", nullptr);
            return 0;
        }
        obj->addChildBone(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:addChildBone",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_addChildBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setParentBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setParentBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Bone:setParentBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setParentBone'", nullptr);
            return 0;
        }
        obj->setParentBone(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setParentBone",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setParentBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getParentBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getParentBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getParentBone'", nullptr);
            return 0;
        }
        auto&& ret = obj->getParentBone();
        object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getParentBone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getParentBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_removeFromParent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_removeFromParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.Bone:removeFromParent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_removeFromParent'", nullptr);
            return 0;
        }
        obj->removeFromParent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:removeFromParent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_removeFromParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_removeChildBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_removeChildBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ext::Bone* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Bone:removeChildBone");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.Bone:removeChildBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_removeChildBone'", nullptr);
            return 0;
        }
        obj->removeChildBone(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:removeChildBone",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_removeChildBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_updateColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_updateColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_updateColor'", nullptr);
            return 0;
        }
        obj->updateColor();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:updateColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_updateColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_updateZOrder(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_updateZOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_updateZOrder'", nullptr);
            return 0;
        }
        obj->updateZOrder();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:updateZOrder",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_updateZOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getTween(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getTween'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getTween'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTween();
        object_to_luaval<ax::ext::BoneTweenController>(tolua_S, "axext.BoneTweenController",(ax::ext::BoneTweenController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getTween",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getTween'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setTransformDirty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setTransformDirty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.Bone:setTransformDirty");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setTransformDirty'", nullptr);
            return 0;
        }
        obj->setTransformDirty(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setTransformDirty",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setTransformDirty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_isTransformDirty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_isTransformDirty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_isTransformDirty'", nullptr);
            return 0;
        }
        auto&& ret = obj->isTransformDirty();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:isTransformDirty",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_isTransformDirty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getNodeToArmatureTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getNodeToArmatureTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getNodeToArmatureTransform'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNodeToArmatureTransform();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getNodeToArmatureTransform",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getNodeToArmatureTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getDisplayRenderNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getDisplayRenderNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getDisplayRenderNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDisplayRenderNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getDisplayRenderNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getDisplayRenderNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getDisplayRenderNodeType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getDisplayRenderNodeType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getDisplayRenderNodeType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getDisplayRenderNodeType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getDisplayRenderNodeType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getDisplayRenderNodeType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getColliderDetector(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getColliderDetector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getColliderDetector'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColliderDetector();
        object_to_luaval<ax::ext::ColliderDetector>(tolua_S, "axext.ColliderDetector",(ax::ext::ColliderDetector*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getColliderDetector",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getColliderDetector'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::BoneData* arg0;

        ok &= luaval_to_object<ax::ext::BoneData>(tolua_S, 2, "axext.BoneData",&arg0, "axext.Bone:setBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setBoneData'", nullptr);
            return 0;
        }
        obj->setBoneData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getBoneData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoneData();
        object_to_luaval<ax::ext::BoneData>(tolua_S, "axext.BoneData",(ax::ext::BoneData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getBoneData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setArmature(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setArmature'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Armature* arg0;

        ok &= luaval_to_object<ax::ext::Armature>(tolua_S, 2, "axext.Armature",&arg0, "axext.Bone:setArmature");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setArmature'", nullptr);
            return 0;
        }
        obj->setArmature(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setArmature",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setArmature'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getArmature(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getArmature'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getArmature'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmature();
        object_to_luaval<ax::ext::Armature>(tolua_S, "axext.Armature",(ax::ext::Armature*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getArmature",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getArmature'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setChildArmature(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setChildArmature'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Armature* arg0;

        ok &= luaval_to_object<ax::ext::Armature>(tolua_S, 2, "axext.Armature",&arg0, "axext.Bone:setChildArmature");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setChildArmature'", nullptr);
            return 0;
        }
        obj->setChildArmature(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setChildArmature",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setChildArmature'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getChildArmature(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getChildArmature'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getChildArmature'", nullptr);
            return 0;
        }
        auto&& ret = obj->getChildArmature();
        object_to_luaval<ax::ext::Armature>(tolua_S, "axext.Armature",(ax::ext::Armature*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getChildArmature",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getChildArmature'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getDisplayManager(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getDisplayManager'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getDisplayManager'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDisplayManager();
        object_to_luaval<ax::ext::DisplayManager>(tolua_S, "axext.DisplayManager",(ax::ext::DisplayManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getDisplayManager",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getDisplayManager'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setIgnoreMovementBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setIgnoreMovementBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.Bone:setIgnoreMovementBoneData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setIgnoreMovementBoneData'", nullptr);
            return 0;
        }
        obj->setIgnoreMovementBoneData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setIgnoreMovementBoneData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setIgnoreMovementBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_isIgnoreMovementBoneData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_isIgnoreMovementBoneData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_isIgnoreMovementBoneData'", nullptr);
            return 0;
        }
        auto&& ret = obj->isIgnoreMovementBoneData();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:isIgnoreMovementBoneData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_isIgnoreMovementBoneData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "axext.Bone:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setBlendFunc'", nullptr);
            return 0;
        }
        obj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_setBlendDirty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_setBlendDirty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.Bone:setBlendDirty");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_setBlendDirty'", nullptr);
            return 0;
        }
        obj->setBlendDirty(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:setBlendDirty",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_setBlendDirty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_isBlendDirty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_isBlendDirty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_isBlendDirty'", nullptr);
            return 0;
        }
        auto&& ret = obj->isBlendDirty();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:isBlendDirty",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_isBlendDirty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getTweenData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getTweenData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getTweenData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTweenData();
        object_to_luaval<ax::ext::FrameData>(tolua_S, "axext.FrameData",(ax::ext::FrameData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getTweenData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getTweenData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_getWorldInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Bone*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Bone_getWorldInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_getWorldInfo'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWorldInfo();
        object_to_luaval<ax::ext::BaseData>(tolua_S, "axext.BaseData",(ax::ext::BaseData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:getWorldInfo",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_getWorldInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Bone_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.Bone",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Bone:create");
            if (!ok) { break; }
            ax::ext::Bone* ret = ax::ext::Bone::create(arg0);
            object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ext::Bone* ret = ax::ext::Bone::create();
            object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "axext.Bone:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_Bone_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Bone* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Bone_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::Bone();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.Bone");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Bone:Bone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Bone_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_Bone_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Bone)");
    return 0;
}

int lua_register_ax_sceneext_Bone(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.Bone");
    tolua_cclass(tolua_S,"Bone","axext.Bone","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"Bone");
        tolua_function(tolua_S,"new",lua_ax_sceneext_Bone_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_Bone_init);
        tolua_function(tolua_S,"addDisplay",lua_ax_sceneext_Bone_addDisplay);
        tolua_function(tolua_S,"removeDisplay",lua_ax_sceneext_Bone_removeDisplay);
        tolua_function(tolua_S,"changeDisplayWithIndex",lua_ax_sceneext_Bone_changeDisplayWithIndex);
        tolua_function(tolua_S,"changeDisplayWithName",lua_ax_sceneext_Bone_changeDisplayWithName);
        tolua_function(tolua_S,"addChildBone",lua_ax_sceneext_Bone_addChildBone);
        tolua_function(tolua_S,"setParentBone",lua_ax_sceneext_Bone_setParentBone);
        tolua_function(tolua_S,"getParentBone",lua_ax_sceneext_Bone_getParentBone);
        tolua_function(tolua_S,"removeFromParent",lua_ax_sceneext_Bone_removeFromParent);
        tolua_function(tolua_S,"removeChildBone",lua_ax_sceneext_Bone_removeChildBone);
        tolua_function(tolua_S,"updateColor",lua_ax_sceneext_Bone_updateColor);
        tolua_function(tolua_S,"updateZOrder",lua_ax_sceneext_Bone_updateZOrder);
        tolua_function(tolua_S,"getTween",lua_ax_sceneext_Bone_getTween);
        tolua_function(tolua_S,"setTransformDirty",lua_ax_sceneext_Bone_setTransformDirty);
        tolua_function(tolua_S,"isTransformDirty",lua_ax_sceneext_Bone_isTransformDirty);
        tolua_function(tolua_S,"getNodeToArmatureTransform",lua_ax_sceneext_Bone_getNodeToArmatureTransform);
        tolua_function(tolua_S,"getDisplayRenderNode",lua_ax_sceneext_Bone_getDisplayRenderNode);
        tolua_function(tolua_S,"getDisplayRenderNodeType",lua_ax_sceneext_Bone_getDisplayRenderNodeType);
        tolua_function(tolua_S,"getColliderDetector",lua_ax_sceneext_Bone_getColliderDetector);
        tolua_function(tolua_S,"setBoneData",lua_ax_sceneext_Bone_setBoneData);
        tolua_function(tolua_S,"getBoneData",lua_ax_sceneext_Bone_getBoneData);
        tolua_function(tolua_S,"setArmature",lua_ax_sceneext_Bone_setArmature);
        tolua_function(tolua_S,"getArmature",lua_ax_sceneext_Bone_getArmature);
        tolua_function(tolua_S,"setChildArmature",lua_ax_sceneext_Bone_setChildArmature);
        tolua_function(tolua_S,"getChildArmature",lua_ax_sceneext_Bone_getChildArmature);
        tolua_function(tolua_S,"getDisplayManager",lua_ax_sceneext_Bone_getDisplayManager);
        tolua_function(tolua_S,"setIgnoreMovementBoneData",lua_ax_sceneext_Bone_setIgnoreMovementBoneData);
        tolua_function(tolua_S,"isIgnoreMovementBoneData",lua_ax_sceneext_Bone_isIgnoreMovementBoneData);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_sceneext_Bone_setBlendFunc);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_sceneext_Bone_getBlendFunc);
        tolua_function(tolua_S,"setBlendDirty",lua_ax_sceneext_Bone_setBlendDirty);
        tolua_function(tolua_S,"isBlendDirty",lua_ax_sceneext_Bone_isBlendDirty);
        tolua_function(tolua_S,"getTweenData",lua_ax_sceneext_Bone_getTweenData);
        tolua_function(tolua_S,"getWorldInfo",lua_ax_sceneext_Bone_getWorldInfo);
        tolua_function(tolua_S,"create", lua_ax_sceneext_Bone_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::Bone).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.Bone";
    g_typeCast[typeName] = "axext.Bone";
    return 1;
}

int lua_ax_sceneext_BatchNode_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.BatchNode",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BatchNode_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::BatchNode::create();
        object_to_luaval<ax::ext::BatchNode>(tolua_S, "axext.BatchNode",(ax::ext::BatchNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.BatchNode:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BatchNode_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_BatchNode_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BatchNode)");
    return 0;
}

int lua_register_ax_sceneext_BatchNode(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.BatchNode");
    tolua_cclass(tolua_S,"BatchNode","axext.BatchNode","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"BatchNode");
        tolua_function(tolua_S,"create", lua_ax_sceneext_BatchNode_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::BatchNode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.BatchNode";
    g_typeCast[typeName] = "axext.BatchNode";
    return 1;
}

int lua_ax_sceneext_ArmatureDataManager_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addArmatureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        ax::ext::ArmatureData* arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addArmatureData");

        ok &= luaval_to_object<ax::ext::ArmatureData>(tolua_S, 3, "axext.ArmatureData",&arg1, "axext.ArmatureDataManager:addArmatureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureData'", nullptr);
            return 0;
        }
        obj->addArmatureData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        ax::ext::ArmatureData* arg1;
        std::string_view arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addArmatureData");

        ok &= luaval_to_object<ax::ext::ArmatureData>(tolua_S, 3, "axext.ArmatureData",&arg1, "axext.ArmatureDataManager:addArmatureData");

        ok &= luaval_to_std_string_view(tolua_S, 4,&arg2, "axext.ArmatureDataManager:addArmatureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureData'", nullptr);
            return 0;
        }
        obj->addArmatureData(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:addArmatureData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getArmatureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:getArmatureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmatureData(arg0);
        object_to_luaval<ax::ext::ArmatureData>(tolua_S, "axext.ArmatureData",(ax::ext::ArmatureData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getArmatureData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_removeArmatureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:removeArmatureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureData'", nullptr);
            return 0;
        }
        obj->removeArmatureData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:removeArmatureData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addAnimationData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addAnimationData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        ax::ext::AnimationData* arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addAnimationData");

        ok &= luaval_to_object<ax::ext::AnimationData>(tolua_S, 3, "axext.AnimationData",&arg1, "axext.ArmatureDataManager:addAnimationData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addAnimationData'", nullptr);
            return 0;
        }
        obj->addAnimationData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        ax::ext::AnimationData* arg1;
        std::string_view arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addAnimationData");

        ok &= luaval_to_object<ax::ext::AnimationData>(tolua_S, 3, "axext.AnimationData",&arg1, "axext.ArmatureDataManager:addAnimationData");

        ok &= luaval_to_std_string_view(tolua_S, 4,&arg2, "axext.ArmatureDataManager:addAnimationData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addAnimationData'", nullptr);
            return 0;
        }
        obj->addAnimationData(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:addAnimationData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addAnimationData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getAnimationData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:getAnimationData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationData(arg0);
        object_to_luaval<ax::ext::AnimationData>(tolua_S, "axext.AnimationData",(ax::ext::AnimationData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getAnimationData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_removeAnimationData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_removeAnimationData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:removeAnimationData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_removeAnimationData'", nullptr);
            return 0;
        }
        obj->removeAnimationData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:removeAnimationData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_removeAnimationData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addTextureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addTextureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        ax::ext::TextureData* arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addTextureData");

        ok &= luaval_to_object<ax::ext::TextureData>(tolua_S, 3, "axext.TextureData",&arg1, "axext.ArmatureDataManager:addTextureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addTextureData'", nullptr);
            return 0;
        }
        obj->addTextureData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        ax::ext::TextureData* arg1;
        std::string_view arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addTextureData");

        ok &= luaval_to_object<ax::ext::TextureData>(tolua_S, 3, "axext.TextureData",&arg1, "axext.ArmatureDataManager:addTextureData");

        ok &= luaval_to_std_string_view(tolua_S, 4,&arg2, "axext.ArmatureDataManager:addTextureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addTextureData'", nullptr);
            return 0;
        }
        obj->addTextureData(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:addTextureData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addTextureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getTextureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getTextureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:getTextureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getTextureData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTextureData(arg0);
        object_to_luaval<ax::ext::TextureData>(tolua_S, "axext.TextureData",(ax::ext::TextureData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getTextureData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getTextureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_removeTextureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_removeTextureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:removeTextureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_removeTextureData'", nullptr);
            return 0;
        }
        obj->removeTextureData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:removeTextureData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_removeTextureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addArmatureFileInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureFileInfo'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 3) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addArmatureFileInfo");

            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ArmatureDataManager:addArmatureFileInfo");

            if (!ok) { break; }
            std::string_view arg2;
            ok &= luaval_to_std_string_view(tolua_S, 4,&arg2, "axext.ArmatureDataManager:addArmatureFileInfo");

            if (!ok) { break; }
            obj->addArmatureFileInfo(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addArmatureFileInfo");

            if (!ok) { break; }
            obj->addArmatureFileInfo(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ArmatureDataManager:addArmatureFileInfo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addArmatureFileInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addSpriteFrameFromFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ArmatureDataManager:addSpriteFrameFromFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile'", nullptr);
            return 0;
        }
        obj->addSpriteFrameFromFile(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        std::string_view arg1;
        std::string_view arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addSpriteFrameFromFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ArmatureDataManager:addSpriteFrameFromFile");

        ok &= luaval_to_std_string_view(tolua_S, 4,&arg2, "axext.ArmatureDataManager:addSpriteFrameFromFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile'", nullptr);
            return 0;
        }
        obj->addSpriteFrameFromFile(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:addSpriteFrameFromFile",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_removeArmatureFileInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureFileInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:removeArmatureFileInfo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureFileInfo'", nullptr);
            return 0;
        }
        obj->removeArmatureFileInfo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:removeArmatureFileInfo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_removeArmatureFileInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_isAutoLoadSpriteFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_isAutoLoadSpriteFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_isAutoLoadSpriteFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAutoLoadSpriteFile();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:isAutoLoadSpriteFile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_isAutoLoadSpriteFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getArmatureDatas(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureDatas'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureDatas'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmatureDatas();
        axmap_string_key_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getArmatureDatas",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getArmatureDatas'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getAnimationDatas(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationDatas'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationDatas'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationDatas();
        axmap_string_key_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getAnimationDatas",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getAnimationDatas'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getTextureDatas(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getTextureDatas'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getTextureDatas'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTextureDatas();
        axmap_string_key_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getTextureDatas",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getTextureDatas'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_addRelativeData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_addRelativeData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:addRelativeData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_addRelativeData'", nullptr);
            return 0;
        }
        obj->addRelativeData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:addRelativeData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_addRelativeData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getRelativeData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureDataManager* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureDataManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureDataManager_getRelativeData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureDataManager:getRelativeData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getRelativeData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRelativeData(arg0);
        #pragma warning NO CONVERSION FROM NATIVE FOR RelativeData*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureDataManager:getRelativeData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getRelativeData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ArmatureDataManager::getInstance();
        object_to_luaval<ax::ext::ArmatureDataManager>(tolua_S, "axext.ArmatureDataManager",(ax::ext::ArmatureDataManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ArmatureDataManager:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ArmatureDataManager_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ArmatureDataManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureDataManager_destroyInstance'", nullptr);
            return 0;
        }
        ax::ext::ArmatureDataManager::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ArmatureDataManager:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureDataManager_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_ArmatureDataManager_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ArmatureDataManager)");
    return 0;
}

int lua_register_ax_sceneext_ArmatureDataManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ArmatureDataManager");
    tolua_cclass(tolua_S,"ArmatureDataManager","axext.ArmatureDataManager","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ArmatureDataManager");
        tolua_function(tolua_S,"init",lua_ax_sceneext_ArmatureDataManager_init);
        tolua_function(tolua_S,"addArmatureData",lua_ax_sceneext_ArmatureDataManager_addArmatureData);
        tolua_function(tolua_S,"getArmatureData",lua_ax_sceneext_ArmatureDataManager_getArmatureData);
        tolua_function(tolua_S,"removeArmatureData",lua_ax_sceneext_ArmatureDataManager_removeArmatureData);
        tolua_function(tolua_S,"addAnimationData",lua_ax_sceneext_ArmatureDataManager_addAnimationData);
        tolua_function(tolua_S,"getAnimationData",lua_ax_sceneext_ArmatureDataManager_getAnimationData);
        tolua_function(tolua_S,"removeAnimationData",lua_ax_sceneext_ArmatureDataManager_removeAnimationData);
        tolua_function(tolua_S,"addTextureData",lua_ax_sceneext_ArmatureDataManager_addTextureData);
        tolua_function(tolua_S,"getTextureData",lua_ax_sceneext_ArmatureDataManager_getTextureData);
        tolua_function(tolua_S,"removeTextureData",lua_ax_sceneext_ArmatureDataManager_removeTextureData);
        tolua_function(tolua_S,"addArmatureFileInfo",lua_ax_sceneext_ArmatureDataManager_addArmatureFileInfo);
        tolua_function(tolua_S,"addSpriteFrameFromFile",lua_ax_sceneext_ArmatureDataManager_addSpriteFrameFromFile);
        tolua_function(tolua_S,"removeArmatureFileInfo",lua_ax_sceneext_ArmatureDataManager_removeArmatureFileInfo);
        tolua_function(tolua_S,"isAutoLoadSpriteFile",lua_ax_sceneext_ArmatureDataManager_isAutoLoadSpriteFile);
        tolua_function(tolua_S,"getArmatureDatas",lua_ax_sceneext_ArmatureDataManager_getArmatureDatas);
        tolua_function(tolua_S,"getAnimationDatas",lua_ax_sceneext_ArmatureDataManager_getAnimationDatas);
        tolua_function(tolua_S,"getTextureDatas",lua_ax_sceneext_ArmatureDataManager_getTextureDatas);
        tolua_function(tolua_S,"addRelativeData",lua_ax_sceneext_ArmatureDataManager_addRelativeData);
        tolua_function(tolua_S,"getRelativeData",lua_ax_sceneext_ArmatureDataManager_getRelativeData);
        tolua_function(tolua_S,"getInstance", lua_ax_sceneext_ArmatureDataManager_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_sceneext_ArmatureDataManager_destroyInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ArmatureDataManager).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ArmatureDataManager";
    g_typeCast[typeName] = "axext.ArmatureDataManager";
    return 1;
}

int lua_ax_sceneext_Armature_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_init'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Armature:init");

            if (!ok) { break; }
            ax::ext::Bone* arg1;
            ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 3, "axext.Bone",&arg1, "axext.Armature:init");

            if (!ok) { break; }
            bool ret = obj->init(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Armature:init");

            if (!ok) { break; }
            bool ret = obj->init(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.Armature:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_addBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_addBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ext::Bone* arg0;
        std::string_view arg1;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Armature:addBone");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.Armature:addBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_addBone'", nullptr);
            return 0;
        }
        obj->addBone(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:addBone",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_addBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Armature:getBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getBone'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBone(arg0);
        object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getBone",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_changeBoneParent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_changeBoneParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ext::Bone* arg0;
        std::string_view arg1;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Armature:changeBoneParent");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.Armature:changeBoneParent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_changeBoneParent'", nullptr);
            return 0;
        }
        obj->changeBoneParent(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:changeBoneParent",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_changeBoneParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_removeBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_removeBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ext::Bone* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Armature:removeBone");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.Armature:removeBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_removeBone'", nullptr);
            return 0;
        }
        obj->removeBone(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:removeBone",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_removeBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getBoneDic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getBoneDic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getBoneDic'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoneDic();
        axmap_string_key_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getBoneDic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getBoneDic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getBoneAtPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getBoneAtPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.Armature:getBoneAtPoint");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "axext.Armature:getBoneAtPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getBoneAtPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoneAtPoint(arg0, arg1);
        object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getBoneAtPoint",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getBoneAtPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "axext.Armature:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setBlendFunc'", nullptr);
            return 0;
        }
        obj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_updateOffsetPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_updateOffsetPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_updateOffsetPoint'", nullptr);
            return 0;
        }
        obj->updateOffsetPoint();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:updateOffsetPoint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_updateOffsetPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getOffsetPoints(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getOffsetPoints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getOffsetPoints'", nullptr);
            return 0;
        }
        auto&& ret = obj->getOffsetPoints();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getOffsetPoints",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getOffsetPoints'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setAnimationController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setAnimationController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ArmatureController* arg0;

        ok &= luaval_to_object<ax::ext::ArmatureController>(tolua_S, 2, "axext.ArmatureController",&arg0, "axext.Armature:setAnimationController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setAnimationController'", nullptr);
            return 0;
        }
        obj->setAnimationController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setAnimationController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setAnimationController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getAnimationController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getAnimationController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getAnimationController'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationController();
        object_to_luaval<ax::ext::ArmatureController>(tolua_S, "axext.ArmatureController",(ax::ext::ArmatureController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getAnimationController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getAnimationController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getArmatureTransformDirty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getArmatureTransformDirty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getArmatureTransformDirty'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmatureTransformDirty();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getArmatureTransformDirty",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getArmatureTransformDirty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setArmatureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setArmatureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::ArmatureData* arg0;

        ok &= luaval_to_object<ax::ext::ArmatureData>(tolua_S, 2, "axext.ArmatureData",&arg0, "axext.Armature:setArmatureData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setArmatureData'", nullptr);
            return 0;
        }
        obj->setArmatureData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setArmatureData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setArmatureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getArmatureData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getArmatureData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getArmatureData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArmatureData();
        object_to_luaval<ax::ext::ArmatureData>(tolua_S, "axext.ArmatureData",(ax::ext::ArmatureData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getArmatureData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getArmatureData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setParentBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setParentBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Armature:setParentBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setParentBone'", nullptr);
            return 0;
        }
        obj->setParentBone(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setParentBone",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setParentBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getParentBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getParentBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getParentBone'", nullptr);
            return 0;
        }
        auto&& ret = obj->getParentBone();
        object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getParentBone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getParentBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.Armature:setVersion");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setVersion'", nullptr);
            return 0;
        }
        obj->setVersion(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setVersion",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getVersion'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVersion();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_setBatchNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_setBatchNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::BatchNode* arg0;

        ok &= luaval_to_object<ax::ext::BatchNode>(tolua_S, 2, "axext.BatchNode",&arg0, "axext.Armature:setBatchNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_setBatchNode'", nullptr);
            return 0;
        }
        obj->setBatchNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:setBatchNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_setBatchNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_getBatchNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Armature*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Armature_getBatchNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_getBatchNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBatchNode();
        object_to_luaval<ax::ext::BatchNode>(tolua_S, "axext.BatchNode",(ax::ext::BatchNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:getBatchNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_getBatchNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Armature_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.Armature",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Armature:create");
            if (!ok) { break; }
            ax::ext::Armature* ret = ax::ext::Armature::create(arg0);
            object_to_luaval<ax::ext::Armature>(tolua_S, "axext.Armature",(ax::ext::Armature*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ext::Armature* ret = ax::ext::Armature::create();
            object_to_luaval<ax::ext::Armature>(tolua_S, "axext.Armature",(ax::ext::Armature*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Armature:create");
            if (!ok) { break; }
            ax::ext::Bone* arg1;
            ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 3, "axext.Bone",&arg1, "axext.Armature:create");
            if (!ok) { break; }
            ax::ext::Armature* ret = ax::ext::Armature::create(arg0, arg1);
            object_to_luaval<ax::ext::Armature>(tolua_S, "axext.Armature",(ax::ext::Armature*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "axext.Armature:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_Armature_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Armature* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Armature_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::Armature();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.Armature");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Armature:Armature",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Armature_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_Armature_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Armature)");
    return 0;
}

int lua_register_ax_sceneext_Armature(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.Armature");
    tolua_cclass(tolua_S,"Armature","axext.Armature","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"Armature");
        tolua_function(tolua_S,"new",lua_ax_sceneext_Armature_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_Armature_init);
        tolua_function(tolua_S,"addBone",lua_ax_sceneext_Armature_addBone);
        tolua_function(tolua_S,"getBone",lua_ax_sceneext_Armature_getBone);
        tolua_function(tolua_S,"changeBoneParent",lua_ax_sceneext_Armature_changeBoneParent);
        tolua_function(tolua_S,"removeBone",lua_ax_sceneext_Armature_removeBone);
        tolua_function(tolua_S,"getBoneDic",lua_ax_sceneext_Armature_getBoneDic);
        tolua_function(tolua_S,"getBoneAtPoint",lua_ax_sceneext_Armature_getBoneAtPoint);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_sceneext_Armature_setBlendFunc);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_sceneext_Armature_getBlendFunc);
        tolua_function(tolua_S,"updateOffsetPoint",lua_ax_sceneext_Armature_updateOffsetPoint);
        tolua_function(tolua_S,"getOffsetPoints",lua_ax_sceneext_Armature_getOffsetPoints);
        tolua_function(tolua_S,"setAnimationController",lua_ax_sceneext_Armature_setAnimationController);
        tolua_function(tolua_S,"getAnimationController",lua_ax_sceneext_Armature_getAnimationController);
        tolua_function(tolua_S,"getArmatureTransformDirty",lua_ax_sceneext_Armature_getArmatureTransformDirty);
        tolua_function(tolua_S,"setArmatureData",lua_ax_sceneext_Armature_setArmatureData);
        tolua_function(tolua_S,"getArmatureData",lua_ax_sceneext_Armature_getArmatureData);
        tolua_function(tolua_S,"setParentBone",lua_ax_sceneext_Armature_setParentBone);
        tolua_function(tolua_S,"getParentBone",lua_ax_sceneext_Armature_getParentBone);
        tolua_function(tolua_S,"setVersion",lua_ax_sceneext_Armature_setVersion);
        tolua_function(tolua_S,"getVersion",lua_ax_sceneext_Armature_getVersion);
        tolua_function(tolua_S,"setBatchNode",lua_ax_sceneext_Armature_setBatchNode);
        tolua_function(tolua_S,"getBatchNode",lua_ax_sceneext_Armature_getBatchNode);
        tolua_function(tolua_S,"create", lua_ax_sceneext_Armature_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::Armature).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.Armature";
    g_typeCast[typeName] = "axext.Armature";
    return 1;
}

int lua_ax_sceneext_ArmatureController_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Armature* arg0;

        ok &= luaval_to_object<ax::ext::Armature>(tolua_S, 2, "axext.Armature",&arg0, "axext.ArmatureController:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_setSpeedScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_setSpeedScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ArmatureController:setSpeedScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_setSpeedScale'", nullptr);
            return 0;
        }
        obj->setSpeedScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:setSpeedScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_setSpeedScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_getSpeedScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_getSpeedScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_getSpeedScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSpeedScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:getSpeedScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_getSpeedScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_play(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_play'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureController:play");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_play'", nullptr);
            return 0;
        }
        obj->play(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        int arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureController:play");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:play");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_play'", nullptr);
            return 0;
        }
        obj->play(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        int arg1;
        int arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ArmatureController:play");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:play");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axext.ArmatureController:play");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_play'", nullptr);
            return 0;
        }
        obj->play(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:play",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_play'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_playWithIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_playWithIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndex'", nullptr);
            return 0;
        }
        obj->playWithIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        int arg0;
        int arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndex");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndex'", nullptr);
            return 0;
        }
        obj->playWithIndex(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        int arg0;
        int arg1;
        int arg2;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndex");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithIndex");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axext.ArmatureController:playWithIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndex'", nullptr);
            return 0;
        }
        obj->playWithIndex(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:playWithIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_playWithIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_playWithNames(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_playWithNames'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::vector<std::string> arg0;

        ok &= luaval_to_std_vector_string(tolua_S, 2, &arg0, "axext.ArmatureController:playWithNames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithNames'", nullptr);
            return 0;
        }
        obj->playWithNames(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<std::string> arg0;
        int arg1;

        ok &= luaval_to_std_vector_string(tolua_S, 2, &arg0, "axext.ArmatureController:playWithNames");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithNames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithNames'", nullptr);
            return 0;
        }
        obj->playWithNames(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::vector<std::string> arg0;
        int arg1;
        bool arg2;

        ok &= luaval_to_std_vector_string(tolua_S, 2, &arg0, "axext.ArmatureController:playWithNames");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithNames");

        ok &= luaval_to_boolean(tolua_S, 4, &arg2, "axext.ArmatureController:playWithNames");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithNames'", nullptr);
            return 0;
        }
        obj->playWithNames(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:playWithNames",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_playWithNames'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_playWithIndexes(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_playWithIndexes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::vector<int> arg0;

        ok &= luaval_to_std_vector_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndexes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndexes'", nullptr);
            return 0;
        }
        obj->playWithIndexes(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        std::vector<int> arg0;
        int arg1;

        ok &= luaval_to_std_vector_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndexes");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithIndexes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndexes'", nullptr);
            return 0;
        }
        obj->playWithIndexes(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::vector<int> arg0;
        int arg1;
        bool arg2;

        ok &= luaval_to_std_vector_int(tolua_S, 2, &arg0, "axext.ArmatureController:playWithIndexes");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ArmatureController:playWithIndexes");

        ok &= luaval_to_boolean(tolua_S, 4, &arg2, "axext.ArmatureController:playWithIndexes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_playWithIndexes'", nullptr);
            return 0;
        }
        obj->playWithIndexes(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:playWithIndexes",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_playWithIndexes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_gotoAndPlay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_gotoAndPlay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ArmatureController:gotoAndPlay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_gotoAndPlay'", nullptr);
            return 0;
        }
        obj->gotoAndPlay(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:gotoAndPlay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_gotoAndPlay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_gotoAndPause(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_gotoAndPause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ArmatureController:gotoAndPause");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_gotoAndPause'", nullptr);
            return 0;
        }
        obj->gotoAndPause(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:gotoAndPause",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_gotoAndPause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_getMovementCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_getMovementCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_getMovementCount'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMovementCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:getMovementCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_getMovementCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_getCurrentMovementID(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_getCurrentMovementID'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_getCurrentMovementID'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCurrentMovementID();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:getCurrentMovementID",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_getCurrentMovementID'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_setAnimationData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_setAnimationData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::AnimationData* arg0;

        ok &= luaval_to_object<ax::ext::AnimationData>(tolua_S, 2, "axext.AnimationData",&arg0, "axext.ArmatureController:setAnimationData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_setAnimationData'", nullptr);
            return 0;
        }
        obj->setAnimationData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:setAnimationData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_setAnimationData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_getAnimationData(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ArmatureController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ArmatureController_getAnimationData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_getAnimationData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationData();
        object_to_luaval<ax::ext::AnimationData>(tolua_S, "axext.AnimationData",(ax::ext::AnimationData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:getAnimationData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_getAnimationData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ArmatureController_obtain(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ArmatureController",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::ext::Armature* arg0;
        ok &= luaval_to_object<ax::ext::Armature>(tolua_S, 2, "axext.Armature",&arg0, "axext.ArmatureController:obtain");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_obtain'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ArmatureController::obtain(arg0);
        object_to_luaval<ax::ext::ArmatureController>(tolua_S, "axext.ArmatureController",(ax::ext::ArmatureController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ArmatureController:obtain",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_obtain'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ArmatureController_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ArmatureController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ArmatureController_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ArmatureController();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ArmatureController");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ArmatureController:ArmatureController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ArmatureController_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ArmatureController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ArmatureController)");
    return 0;
}

int lua_register_ax_sceneext_ArmatureController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ArmatureController");
    tolua_cclass(tolua_S,"ArmatureController","axext.ArmatureController","axext.AnimationController",nullptr);

    tolua_beginmodule(tolua_S,"ArmatureController");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ArmatureController_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ArmatureController_init);
        tolua_function(tolua_S,"setSpeedScale",lua_ax_sceneext_ArmatureController_setSpeedScale);
        tolua_function(tolua_S,"getSpeedScale",lua_ax_sceneext_ArmatureController_getSpeedScale);
        tolua_function(tolua_S,"play",lua_ax_sceneext_ArmatureController_play);
        tolua_function(tolua_S,"playWithIndex",lua_ax_sceneext_ArmatureController_playWithIndex);
        tolua_function(tolua_S,"playWithNames",lua_ax_sceneext_ArmatureController_playWithNames);
        tolua_function(tolua_S,"playWithIndexes",lua_ax_sceneext_ArmatureController_playWithIndexes);
        tolua_function(tolua_S,"gotoAndPlay",lua_ax_sceneext_ArmatureController_gotoAndPlay);
        tolua_function(tolua_S,"gotoAndPause",lua_ax_sceneext_ArmatureController_gotoAndPause);
        tolua_function(tolua_S,"getMovementCount",lua_ax_sceneext_ArmatureController_getMovementCount);
        tolua_function(tolua_S,"getCurrentMovementID",lua_ax_sceneext_ArmatureController_getCurrentMovementID);
        tolua_function(tolua_S,"setAnimationData",lua_ax_sceneext_ArmatureController_setAnimationData);
        tolua_function(tolua_S,"getAnimationData",lua_ax_sceneext_ArmatureController_getAnimationData);
        tolua_function(tolua_S,"obtain", lua_ax_sceneext_ArmatureController_obtain);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ArmatureController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ArmatureController";
    g_typeCast[typeName] = "axext.ArmatureController";
    return 1;
}

int lua_ax_sceneext_Skin_updateArmatureTransform(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Skin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Skin_updateArmatureTransform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_updateArmatureTransform'", nullptr);
            return 0;
        }
        obj->updateArmatureTransform();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:updateArmatureTransform",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_updateArmatureTransform'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Skin_getNodeToWorldTransformAR(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Skin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Skin_getNodeToWorldTransformAR'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_getNodeToWorldTransformAR'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNodeToWorldTransformAR();
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:getNodeToWorldTransformAR",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_getNodeToWorldTransformAR'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Skin_setBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Skin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Skin_setBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::Bone* arg0;

        ok &= luaval_to_object<ax::ext::Bone>(tolua_S, 2, "axext.Bone",&arg0, "axext.Skin:setBone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_setBone'", nullptr);
            return 0;
        }
        obj->setBone(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:setBone",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_setBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Skin_getBone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Skin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Skin_getBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_getBone'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBone();
        object_to_luaval<ax::ext::Bone>(tolua_S, "axext.Bone",(ax::ext::Bone*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:getBone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_getBone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Skin_getDisplayName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::Skin*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Skin_getDisplayName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_getDisplayName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDisplayName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:getDisplayName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_getDisplayName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Skin_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Skin:create");
            if (!ok) { break; }
            ax::ext::Skin* ret = ax::ext::Skin::create(arg0);
            object_to_luaval<ax::ext::Skin>(tolua_S, "axext.Skin",(ax::ext::Skin*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ext::Skin* ret = ax::ext::Skin::create();
            object_to_luaval<ax::ext::Skin>(tolua_S, "axext.Skin",(ax::ext::Skin*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "axext.Skin:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_Skin_createWithSpriteFrameName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.Skin",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.Skin:createWithSpriteFrameName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_createWithSpriteFrameName'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::Skin::createWithSpriteFrameName(arg0);
        object_to_luaval<ax::ext::Skin>(tolua_S, "axext.Skin",(ax::ext::Skin*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.Skin:createWithSpriteFrameName",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_createWithSpriteFrameName'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_Skin_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::Skin* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Skin_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::Skin();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.Skin");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.Skin:Skin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Skin_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_Skin_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Skin)");
    return 0;
}

int lua_register_ax_sceneext_Skin(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.Skin");
    tolua_cclass(tolua_S,"Skin","axext.Skin","ax.Sprite",nullptr);

    tolua_beginmodule(tolua_S,"Skin");
        tolua_function(tolua_S,"new",lua_ax_sceneext_Skin_constructor);
        tolua_function(tolua_S,"updateArmatureTransform",lua_ax_sceneext_Skin_updateArmatureTransform);
        tolua_function(tolua_S,"getNodeToWorldTransformAR",lua_ax_sceneext_Skin_getNodeToWorldTransformAR);
        tolua_function(tolua_S,"setBone",lua_ax_sceneext_Skin_setBone);
        tolua_function(tolua_S,"getBone",lua_ax_sceneext_Skin_getBone);
        tolua_function(tolua_S,"getDisplayName",lua_ax_sceneext_Skin_getDisplayName);
        tolua_function(tolua_S,"create", lua_ax_sceneext_Skin_create);
        tolua_function(tolua_S,"createWithSpriteFrameName", lua_ax_sceneext_Skin_createWithSpriteFrameName);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::Skin).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.Skin";
    g_typeCast[typeName] = "axext.Skin";
    return 1;
}

int lua_ax_sceneext_ComAttribute_setInt(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_setInt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        int arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:setInt");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ComAttribute:setInt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_setInt'", nullptr);
            return 0;
        }
        obj->setInt(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:setInt",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_setInt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_setFloat(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_setFloat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        double arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:setFloat");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "axext.ComAttribute:setFloat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_setFloat'", nullptr);
            return 0;
        }
        obj->setFloat(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:setFloat",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_setFloat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_setBool(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_setBool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        bool arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:setBool");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.ComAttribute:setBool");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_setBool'", nullptr);
            return 0;
        }
        obj->setBool(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:setBool",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_setBool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_setString(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_setString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:setString");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ComAttribute:setString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_setString'", nullptr);
            return 0;
        }
        obj->setString(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:setString",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_setString'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_getInt(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_getInt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getInt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getInt'", nullptr);
            return 0;
        }
        auto&& ret = obj->getInt(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        int arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getInt");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.ComAttribute:getInt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getInt'", nullptr);
            return 0;
        }
        auto&& ret = obj->getInt(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:getInt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_getInt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_getFloat(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_getFloat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getFloat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getFloat'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFloat(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        double arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getFloat");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "axext.ComAttribute:getFloat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getFloat'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFloat(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:getFloat",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_getFloat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_getBool(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_getBool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getBool");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getBool'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBool(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        bool arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getBool");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.ComAttribute:getBool");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getBool'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBool(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:getBool",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_getBool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_getString(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_getString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getString'", nullptr);
            return 0;
        }
        auto&& ret = obj->getString(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:getString");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axext.ComAttribute:getString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_getString'", nullptr);
            return 0;
        }
        auto&& ret = obj->getString(arg0, arg1);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:getString",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_getString'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_parse(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAttribute* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAttribute*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAttribute_parse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComAttribute:parse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_parse'", nullptr);
            return 0;
        }
        auto&& ret = obj->parse(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAttribute:parse",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_parse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAttribute_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_createInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComAttribute::createInstance();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComAttribute:createInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComAttribute_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComAttribute",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAttribute_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComAttribute::create();
        object_to_luaval<ax::ext::ComAttribute>(tolua_S, "axext.ComAttribute",(ax::ext::ComAttribute*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComAttribute:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAttribute_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_ComAttribute_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ComAttribute)");
    return 0;
}

int lua_register_ax_sceneext_ComAttribute(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ComAttribute");
    tolua_cclass(tolua_S,"ComAttribute","axext.ComAttribute","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"ComAttribute");
        tolua_function(tolua_S,"setInt",lua_ax_sceneext_ComAttribute_setInt);
        tolua_function(tolua_S,"setFloat",lua_ax_sceneext_ComAttribute_setFloat);
        tolua_function(tolua_S,"setBool",lua_ax_sceneext_ComAttribute_setBool);
        tolua_function(tolua_S,"setString",lua_ax_sceneext_ComAttribute_setString);
        tolua_function(tolua_S,"getInt",lua_ax_sceneext_ComAttribute_getInt);
        tolua_function(tolua_S,"getFloat",lua_ax_sceneext_ComAttribute_getFloat);
        tolua_function(tolua_S,"getBool",lua_ax_sceneext_ComAttribute_getBool);
        tolua_function(tolua_S,"getString",lua_ax_sceneext_ComAttribute_getString);
        tolua_function(tolua_S,"parse",lua_ax_sceneext_ComAttribute_parse);
        tolua_function(tolua_S,"createInstance", lua_ax_sceneext_ComAttribute_createInstance);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ComAttribute_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ComAttribute).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ComAttribute";
    g_typeCast[typeName] = "axext.ComAttribute";
    return 1;
}

int lua_ax_sceneext_ComAudio_end(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_end'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_end'", nullptr);
            return 0;
        }
        obj->end();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:end",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_end'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_preloadBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_preloadBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:preloadBackgroundMusic"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_preloadBackgroundMusic'", nullptr);
            return 0;
        }
        obj->preloadBackgroundMusic(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:preloadBackgroundMusic",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_preloadBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_playBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_playBackgroundMusic'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            const char* arg0;
            std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:playBackgroundMusic"); arg0 = arg0_tmp.c_str();

            if (!ok) { break; }
            obj->playBackgroundMusic(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            const char* arg0;
            std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:playBackgroundMusic"); arg0 = arg0_tmp.c_str();

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.ComAudio:playBackgroundMusic");

            if (!ok) { break; }
            obj->playBackgroundMusic(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 0) {
            obj->playBackgroundMusic();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ComAudio:playBackgroundMusic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_playBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_stopBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_stopBackgroundMusic'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 0) {
            obj->stopBackgroundMusic();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            bool arg0;
            ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.ComAudio:stopBackgroundMusic");

            if (!ok) { break; }
            obj->stopBackgroundMusic(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ComAudio:stopBackgroundMusic",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_stopBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_pauseBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_pauseBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_pauseBackgroundMusic'", nullptr);
            return 0;
        }
        obj->pauseBackgroundMusic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:pauseBackgroundMusic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_pauseBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_resumeBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_resumeBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_resumeBackgroundMusic'", nullptr);
            return 0;
        }
        obj->resumeBackgroundMusic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:resumeBackgroundMusic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_resumeBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_rewindBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_rewindBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_rewindBackgroundMusic'", nullptr);
            return 0;
        }
        obj->rewindBackgroundMusic();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:rewindBackgroundMusic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_rewindBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_willPlayBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_willPlayBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_willPlayBackgroundMusic'", nullptr);
            return 0;
        }
        auto&& ret = obj->willPlayBackgroundMusic();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:willPlayBackgroundMusic",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_willPlayBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_isBackgroundMusicPlaying(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_isBackgroundMusicPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_isBackgroundMusicPlaying'", nullptr);
            return 0;
        }
        auto&& ret = obj->isBackgroundMusicPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:isBackgroundMusicPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_isBackgroundMusicPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_getBackgroundMusicVolume(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_getBackgroundMusicVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_getBackgroundMusicVolume'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBackgroundMusicVolume();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:getBackgroundMusicVolume",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_getBackgroundMusicVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_setBackgroundMusicVolume(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_setBackgroundMusicVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ComAudio:setBackgroundMusicVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_setBackgroundMusicVolume'", nullptr);
            return 0;
        }
        obj->setBackgroundMusicVolume(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:setBackgroundMusicVolume",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_setBackgroundMusicVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_getEffectsVolume(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_getEffectsVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_getEffectsVolume'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEffectsVolume();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:getEffectsVolume",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_getEffectsVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_setEffectsVolume(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_setEffectsVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axext.ComAudio:setEffectsVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_setEffectsVolume'", nullptr);
            return 0;
        }
        obj->setEffectsVolume(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:setEffectsVolume",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_setEffectsVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_playEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_playEffect'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            const char* arg0;
            std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:playEffect"); arg0 = arg0_tmp.c_str();

            if (!ok) { break; }
            unsigned int ret = obj->playEffect(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            const char* arg0;
            std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:playEffect"); arg0 = arg0_tmp.c_str();

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axext.ComAudio:playEffect");

            if (!ok) { break; }
            unsigned int ret = obj->playEffect(arg0, arg1);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 0) {
            unsigned int ret = obj->playEffect();
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axext.ComAudio:playEffect",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_playEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_pauseEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_pauseEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ComAudio:pauseEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_pauseEffect'", nullptr);
            return 0;
        }
        obj->pauseEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:pauseEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_pauseEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_pauseAllEffects(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_pauseAllEffects'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_pauseAllEffects'", nullptr);
            return 0;
        }
        obj->pauseAllEffects();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:pauseAllEffects",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_pauseAllEffects'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_resumeEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_resumeEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ComAudio:resumeEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_resumeEffect'", nullptr);
            return 0;
        }
        obj->resumeEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:resumeEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_resumeEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_resumeAllEffects(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_resumeAllEffects'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_resumeAllEffects'", nullptr);
            return 0;
        }
        obj->resumeAllEffects();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:resumeAllEffects",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_resumeAllEffects'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_stopEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_stopEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ComAudio:stopEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_stopEffect'", nullptr);
            return 0;
        }
        obj->stopEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:stopEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_stopEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_stopAllEffects(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_stopAllEffects'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_stopAllEffects'", nullptr);
            return 0;
        }
        obj->stopAllEffects();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:stopAllEffects",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_stopAllEffects'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_preloadEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_preloadEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:preloadEffect"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_preloadEffect'", nullptr);
            return 0;
        }
        obj->preloadEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:preloadEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_preloadEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_unloadEffect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_unloadEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:unloadEffect"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_unloadEffect'", nullptr);
            return 0;
        }
        obj->unloadEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:unloadEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_unloadEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_setFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_setFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.ComAudio:setFile"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_setFile'", nullptr);
            return 0;
        }
        obj->setFile(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:setFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_setFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_getFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_getFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_getFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFile();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:getFile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_getFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_setLoop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_setLoop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axext.ComAudio:setLoop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_setLoop'", nullptr);
            return 0;
        }
        obj->setLoop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:setLoop",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_setLoop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_isLoop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_isLoop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_isLoop'", nullptr);
            return 0;
        }
        auto&& ret = obj->isLoop();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:isLoop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_isLoop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_start(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_start'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_start'", nullptr);
            return 0;
        }
        obj->start();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:start",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_start'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_stop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComAudio* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComAudio*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComAudio_stop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_stop'", nullptr);
            return 0;
        }
        obj->stop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComAudio:stop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_stop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComAudio_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_createInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComAudio::createInstance();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComAudio:createInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComAudio_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComAudio",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComAudio_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComAudio::create();
        object_to_luaval<ax::ext::ComAudio>(tolua_S, "axext.ComAudio",(ax::ext::ComAudio*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComAudio:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComAudio_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_ComAudio_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ComAudio)");
    return 0;
}

int lua_register_ax_sceneext_ComAudio(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ComAudio");
    tolua_cclass(tolua_S,"ComAudio","axext.ComAudio","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"ComAudio");
        tolua_function(tolua_S,"endToLua",lua_ax_sceneext_ComAudio_end);
        tolua_function(tolua_S,"preloadBackgroundMusic",lua_ax_sceneext_ComAudio_preloadBackgroundMusic);
        tolua_function(tolua_S,"playBackgroundMusic",lua_ax_sceneext_ComAudio_playBackgroundMusic);
        tolua_function(tolua_S,"stopBackgroundMusic",lua_ax_sceneext_ComAudio_stopBackgroundMusic);
        tolua_function(tolua_S,"pauseBackgroundMusic",lua_ax_sceneext_ComAudio_pauseBackgroundMusic);
        tolua_function(tolua_S,"resumeBackgroundMusic",lua_ax_sceneext_ComAudio_resumeBackgroundMusic);
        tolua_function(tolua_S,"rewindBackgroundMusic",lua_ax_sceneext_ComAudio_rewindBackgroundMusic);
        tolua_function(tolua_S,"willPlayBackgroundMusic",lua_ax_sceneext_ComAudio_willPlayBackgroundMusic);
        tolua_function(tolua_S,"isBackgroundMusicPlaying",lua_ax_sceneext_ComAudio_isBackgroundMusicPlaying);
        tolua_function(tolua_S,"getBackgroundMusicVolume",lua_ax_sceneext_ComAudio_getBackgroundMusicVolume);
        tolua_function(tolua_S,"setBackgroundMusicVolume",lua_ax_sceneext_ComAudio_setBackgroundMusicVolume);
        tolua_function(tolua_S,"getEffectsVolume",lua_ax_sceneext_ComAudio_getEffectsVolume);
        tolua_function(tolua_S,"setEffectsVolume",lua_ax_sceneext_ComAudio_setEffectsVolume);
        tolua_function(tolua_S,"playEffect",lua_ax_sceneext_ComAudio_playEffect);
        tolua_function(tolua_S,"pauseEffect",lua_ax_sceneext_ComAudio_pauseEffect);
        tolua_function(tolua_S,"pauseAllEffects",lua_ax_sceneext_ComAudio_pauseAllEffects);
        tolua_function(tolua_S,"resumeEffect",lua_ax_sceneext_ComAudio_resumeEffect);
        tolua_function(tolua_S,"resumeAllEffects",lua_ax_sceneext_ComAudio_resumeAllEffects);
        tolua_function(tolua_S,"stopEffect",lua_ax_sceneext_ComAudio_stopEffect);
        tolua_function(tolua_S,"stopAllEffects",lua_ax_sceneext_ComAudio_stopAllEffects);
        tolua_function(tolua_S,"preloadEffect",lua_ax_sceneext_ComAudio_preloadEffect);
        tolua_function(tolua_S,"unloadEffect",lua_ax_sceneext_ComAudio_unloadEffect);
        tolua_function(tolua_S,"setFile",lua_ax_sceneext_ComAudio_setFile);
        tolua_function(tolua_S,"getFile",lua_ax_sceneext_ComAudio_getFile);
        tolua_function(tolua_S,"setLoop",lua_ax_sceneext_ComAudio_setLoop);
        tolua_function(tolua_S,"isLoop",lua_ax_sceneext_ComAudio_isLoop);
        tolua_function(tolua_S,"start",lua_ax_sceneext_ComAudio_start);
        tolua_function(tolua_S,"stop",lua_ax_sceneext_ComAudio_stop);
        tolua_function(tolua_S,"createInstance", lua_ax_sceneext_ComAudio_createInstance);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ComAudio_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ComAudio).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ComAudio";
    g_typeCast[typeName] = "axext.ComAudio";
    return 1;
}

int lua_ax_sceneext_ComController_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComController",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComController_createInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComController::createInstance();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComController:createInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComController_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComController_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComController",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComController_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComController::create();
        object_to_luaval<ax::ext::ComController>(tolua_S, "axext.ComController",(ax::ext::ComController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComController:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComController_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComController_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComController* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComController_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ComController();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ComController");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComController:ComController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComController_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ComController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ComController)");
    return 0;
}

int lua_register_ax_sceneext_ComController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ComController");
    tolua_cclass(tolua_S,"ComController","axext.ComController","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"ComController");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ComController_constructor);
        tolua_function(tolua_S,"createInstance", lua_ax_sceneext_ComController_createInstance);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ComController_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ComController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ComController";
    g_typeCast[typeName] = "axext.ComController";
    return 1;
}

int lua_ax_sceneext_ComRender_getNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComRender* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComRender",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComRender*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComRender_getNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComRender_getNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComRender:getNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComRender_getNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComRender_setNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComRender* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComRender",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComRender*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComRender_setNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axext.ComRender:setNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComRender_setNode'", nullptr);
            return 0;
        }
        obj->setNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComRender:setNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComRender_setNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComRender_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComRender",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComRender_createInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComRender::createInstance();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComRender:createInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComRender_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComRender_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComRender",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 2)
        {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axext.ComRender:create");
            if (!ok) { break; }
            const char* arg1;
            std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "axext.ComRender:create"); arg1 = arg1_tmp.c_str();
            if (!ok) { break; }
            ax::ext::ComRender* ret = ax::ext::ComRender::create(arg0, arg1);
            object_to_luaval<ax::ext::ComRender>(tolua_S, "axext.ComRender",(ax::ext::ComRender*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ext::ComRender* ret = ax::ext::ComRender::create();
            object_to_luaval<ax::ext::ComRender>(tolua_S, "axext.ComRender",(ax::ext::ComRender*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "axext.ComRender:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComRender_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_ComRender_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ComRender)");
    return 0;
}

int lua_register_ax_sceneext_ComRender(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ComRender");
    tolua_cclass(tolua_S,"ComRender","axext.ComRender","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"ComRender");
        tolua_function(tolua_S,"getNode",lua_ax_sceneext_ComRender_getNode);
        tolua_function(tolua_S,"setNode",lua_ax_sceneext_ComRender_setNode);
        tolua_function(tolua_S,"createInstance", lua_ax_sceneext_ComRender_createInstance);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ComRender_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ComRender).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ComRender";
    g_typeCast[typeName] = "axext.ComRender";
    return 1;
}

int lua_ax_sceneext_GUIReader_widgetFromJsonFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::GUIReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::GUIReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_GUIReader_widgetFromJsonFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.GUIReader:widgetFromJsonFile"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_widgetFromJsonFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->widgetFromJsonFile(arg0);
        object_to_luaval<ax::ui::Widget>(tolua_S, "axui.Widget",(ax::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.GUIReader:widgetFromJsonFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_widgetFromJsonFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_GUIReader_widgetFromBinaryFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::GUIReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::GUIReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_GUIReader_widgetFromBinaryFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.GUIReader:widgetFromBinaryFile"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_widgetFromBinaryFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->widgetFromBinaryFile(arg0);
        object_to_luaval<ax::ui::Widget>(tolua_S, "axui.Widget",(ax::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.GUIReader:widgetFromBinaryFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_widgetFromBinaryFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_GUIReader_getVersionInteger(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::GUIReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::GUIReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_GUIReader_getVersionInteger'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "axext.GUIReader:getVersionInteger"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_getVersionInteger'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVersionInteger(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.GUIReader:getVersionInteger",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_getVersionInteger'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_GUIReader_setFilePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::GUIReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::GUIReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_GUIReader_setFilePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.GUIReader:setFilePath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_setFilePath'", nullptr);
            return 0;
        }
        obj->setFilePath(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.GUIReader:setFilePath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_setFilePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_GUIReader_getFilePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::GUIReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::GUIReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_GUIReader_getFilePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_getFilePath'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFilePath();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.GUIReader:getFilePath",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_getFilePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_GUIReader_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::GUIReader::getInstance();
        object_to_luaval<ax::ext::GUIReader>(tolua_S, "axext.GUIReader",(ax::ext::GUIReader*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.GUIReader:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_GUIReader_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.GUIReader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_GUIReader_destroyInstance'", nullptr);
            return 0;
        }
        ax::ext::GUIReader::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.GUIReader:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_GUIReader_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_GUIReader_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GUIReader)");
    return 0;
}

int lua_register_ax_sceneext_GUIReader(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.GUIReader");
    tolua_cclass(tolua_S,"GUIReader","axext.GUIReader","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"GUIReader");
        tolua_function(tolua_S,"widgetFromJsonFile",lua_ax_sceneext_GUIReader_widgetFromJsonFile);
        tolua_function(tolua_S,"widgetFromBinaryFile",lua_ax_sceneext_GUIReader_widgetFromBinaryFile);
        tolua_function(tolua_S,"getVersionInteger",lua_ax_sceneext_GUIReader_getVersionInteger);
        tolua_function(tolua_S,"setFilePath",lua_ax_sceneext_GUIReader_setFilePath);
        tolua_function(tolua_S,"getFilePath",lua_ax_sceneext_GUIReader_getFilePath);
        tolua_function(tolua_S,"getInstance", lua_ax_sceneext_GUIReader_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_sceneext_GUIReader_destroyInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::GUIReader).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.GUIReader";
    g_typeCast[typeName] = "axext.GUIReader";
    return 1;
}

int lua_ax_sceneext_SceneReader_createNodeWithSceneFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SceneReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::SceneReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SceneReader_createNodeWithSceneFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.SceneReader:createNodeWithSceneFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_createNodeWithSceneFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->createNodeWithSceneFile(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        ax::ext::SceneReader::AttachComponentType arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.SceneReader:createNodeWithSceneFile");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axext.SceneReader:createNodeWithSceneFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_createNodeWithSceneFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->createNodeWithSceneFile(arg0, arg1);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SceneReader:createNodeWithSceneFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_createNodeWithSceneFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SceneReader_setTarget(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SceneReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::SceneReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SceneReader_setTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (ax::Object *, void *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_setTarget'", nullptr);
            return 0;
        }
        obj->setTarget(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SceneReader:setTarget",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_setTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SceneReader_getNodeByTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SceneReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::SceneReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SceneReader_getNodeByTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.SceneReader:getNodeByTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_getNodeByTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNodeByTag(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SceneReader:getNodeByTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_getNodeByTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SceneReader_getAttachComponentType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::SceneReader* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::SceneReader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SceneReader_getAttachComponentType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_getAttachComponentType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getAttachComponentType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.SceneReader:getAttachComponentType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_getAttachComponentType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SceneReader_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::SceneReader::getInstance();
        object_to_luaval<ax::ext::SceneReader>(tolua_S, "axext.SceneReader",(ax::ext::SceneReader*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.SceneReader:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_SceneReader_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_destroyInstance'", nullptr);
            return 0;
        }
        ax::ext::SceneReader::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.SceneReader:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_SceneReader_sceneReaderVersion(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.SceneReader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SceneReader_sceneReaderVersion'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::SceneReader::sceneReaderVersion();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.SceneReader:sceneReaderVersion",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SceneReader_sceneReaderVersion'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_sceneext_SceneReader_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SceneReader)");
    return 0;
}

int lua_register_ax_sceneext_SceneReader(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.SceneReader");
    tolua_cclass(tolua_S,"SceneReader","axext.SceneReader","",nullptr);

    tolua_beginmodule(tolua_S,"SceneReader");
        tolua_function(tolua_S,"createNodeWithSceneFile",lua_ax_sceneext_SceneReader_createNodeWithSceneFile);
        tolua_function(tolua_S,"setTarget",lua_ax_sceneext_SceneReader_setTarget);
        tolua_function(tolua_S,"getNodeByTag",lua_ax_sceneext_SceneReader_getNodeByTag);
        tolua_function(tolua_S,"getAttachComponentType",lua_ax_sceneext_SceneReader_getAttachComponentType);
        tolua_function(tolua_S,"getInstance", lua_ax_sceneext_SceneReader_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_sceneext_SceneReader_destroyInstance);
        tolua_function(tolua_S,"sceneReaderVersion", lua_ax_sceneext_SceneReader_sceneReaderVersion);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::SceneReader).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.SceneReader";
    g_typeCast[typeName] = "axext.SceneReader";
    return 1;
}

int lua_ax_sceneext_Frame_setFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        unsigned int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextFrame:setFrameIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setFrameIndex'", nullptr);
            return 0;
        }
        obj->setFrameIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setFrameIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_getFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_getFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_getFrameIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrameIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:getFrameIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_getFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_setTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::Timeline* arg0;

        ok &= luaval_to_object<ax::ext::timeline::Timeline>(tolua_S, 2, "axextTimeline",&arg0, "axextFrame:setTimeline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setTimeline'", nullptr);
            return 0;
        }
        obj->setTimeline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setTimeline",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_getTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_getTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_getTimeline'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTimeline();
        object_to_luaval<ax::ext::timeline::Timeline>(tolua_S, "axextTimeline",(ax::ext::timeline::Timeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:getTimeline",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_getTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_setNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextFrame:setNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setNode'", nullptr);
            return 0;
        }
        obj->setNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_getNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_getNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_getNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:getNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_getNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_setTween(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setTween'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axextFrame:setTween");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setTween'", nullptr);
            return 0;
        }
        obj->setTween(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setTween",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setTween'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_isTween(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_isTween'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_isTween'", nullptr);
            return 0;
        }
        auto&& ret = obj->isTween();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:isTween",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_isTween'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_setTweenType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setTweenType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::tweenfunc::TweenType arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextFrame:setTweenType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setTweenType'", nullptr);
            return 0;
        }
        obj->setTweenType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setTweenType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setTweenType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_getTweenType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_getTweenType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_getTweenType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getTweenType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:getTweenType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_getTweenType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_setEasingParams(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_setEasingParams'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::vector<float> arg0;

        ok &= luaval_to_std_vector_float(tolua_S, 2, &arg0, "axextFrame:setEasingParams");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_setEasingParams'", nullptr);
            return 0;
        }
        obj->setEasingParams(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:setEasingParams",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_setEasingParams'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_getEasingParams(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_getEasingParams'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_getEasingParams'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEasingParams();
        floatspan_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:getEasingParams",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_getEasingParams'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_isEnterWhenPassed(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_isEnterWhenPassed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_isEnterWhenPassed'", nullptr);
            return 0;
        }
        auto&& ret = obj->isEnterWhenPassed();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:isEnterWhenPassed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_isEnterWhenPassed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_apply(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_apply'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextFrame:apply");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_apply'", nullptr);
            return 0;
        }
        obj->apply(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:apply",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_apply'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Frame_clone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Frame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Frame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Frame_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Frame_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone();
        object_to_luaval<ax::ext::timeline::Frame>(tolua_S, "axextFrame",(ax::ext::timeline::Frame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextFrame:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Frame_clone'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_sceneext_Frame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Frame)");
    return 0;
}

int lua_register_ax_sceneext_Frame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextFrame");
    tolua_cclass(tolua_S,"Frame","axextFrame","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Frame");
        tolua_function(tolua_S,"setFrameIndex",lua_ax_sceneext_Frame_setFrameIndex);
        tolua_function(tolua_S,"getFrameIndex",lua_ax_sceneext_Frame_getFrameIndex);
        tolua_function(tolua_S,"setTimeline",lua_ax_sceneext_Frame_setTimeline);
        tolua_function(tolua_S,"getTimeline",lua_ax_sceneext_Frame_getTimeline);
        tolua_function(tolua_S,"setNode",lua_ax_sceneext_Frame_setNode);
        tolua_function(tolua_S,"getNode",lua_ax_sceneext_Frame_getNode);
        tolua_function(tolua_S,"setTween",lua_ax_sceneext_Frame_setTween);
        tolua_function(tolua_S,"isTween",lua_ax_sceneext_Frame_isTween);
        tolua_function(tolua_S,"setTweenType",lua_ax_sceneext_Frame_setTweenType);
        tolua_function(tolua_S,"getTweenType",lua_ax_sceneext_Frame_getTweenType);
        tolua_function(tolua_S,"setEasingParams",lua_ax_sceneext_Frame_setEasingParams);
        tolua_function(tolua_S,"getEasingParams",lua_ax_sceneext_Frame_getEasingParams);
        tolua_function(tolua_S,"isEnterWhenPassed",lua_ax_sceneext_Frame_isEnterWhenPassed);
        tolua_function(tolua_S,"apply",lua_ax_sceneext_Frame_apply);
        tolua_function(tolua_S,"clone",lua_ax_sceneext_Frame_clone);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::Frame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextFrame";
    g_typeCast[typeName] = "axextFrame";
    return 1;
}

int lua_ax_sceneext_VisibleFrame_setVisible(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::VisibleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextVisibleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::VisibleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_VisibleFrame_setVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axextVisibleFrame:setVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_VisibleFrame_setVisible'", nullptr);
            return 0;
        }
        obj->setVisible(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextVisibleFrame:setVisible",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_VisibleFrame_setVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_VisibleFrame_isVisible(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::VisibleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextVisibleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::VisibleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_VisibleFrame_isVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_VisibleFrame_isVisible'", nullptr);
            return 0;
        }
        auto&& ret = obj->isVisible();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextVisibleFrame:isVisible",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_VisibleFrame_isVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_VisibleFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextVisibleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_VisibleFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::VisibleFrame::create();
        object_to_luaval<ax::ext::timeline::VisibleFrame>(tolua_S, "axextVisibleFrame",(ax::ext::timeline::VisibleFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextVisibleFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_VisibleFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_VisibleFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::VisibleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_VisibleFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::VisibleFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextVisibleFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextVisibleFrame:VisibleFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_VisibleFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_VisibleFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (VisibleFrame)");
    return 0;
}

int lua_register_ax_sceneext_VisibleFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextVisibleFrame");
    tolua_cclass(tolua_S,"VisibleFrame","axextVisibleFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"VisibleFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_VisibleFrame_constructor);
        tolua_function(tolua_S,"setVisible",lua_ax_sceneext_VisibleFrame_setVisible);
        tolua_function(tolua_S,"isVisible",lua_ax_sceneext_VisibleFrame_isVisible);
        tolua_function(tolua_S,"create", lua_ax_sceneext_VisibleFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::VisibleFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextVisibleFrame";
    g_typeCast[typeName] = "axextVisibleFrame";
    return 1;
}

int lua_ax_sceneext_TextureFrame_setTextureName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::TextureFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTextureFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::TextureFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_TextureFrame_setTextureName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextTextureFrame:setTextureName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureFrame_setTextureName'", nullptr);
            return 0;
        }
        obj->setTextureName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTextureFrame:setTextureName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureFrame_setTextureName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_TextureFrame_getTextureName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::TextureFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTextureFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::TextureFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_TextureFrame_getTextureName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureFrame_getTextureName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTextureName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTextureFrame:getTextureName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureFrame_getTextureName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_TextureFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextTextureFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::TextureFrame::create();
        object_to_luaval<ax::ext::timeline::TextureFrame>(tolua_S, "axextTextureFrame",(ax::ext::timeline::TextureFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextTextureFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_TextureFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::TextureFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_TextureFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::TextureFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextTextureFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTextureFrame:TextureFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_TextureFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_TextureFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (TextureFrame)");
    return 0;
}

int lua_register_ax_sceneext_TextureFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextTextureFrame");
    tolua_cclass(tolua_S,"TextureFrame","axextTextureFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"TextureFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_TextureFrame_constructor);
        tolua_function(tolua_S,"setTextureName",lua_ax_sceneext_TextureFrame_setTextureName);
        tolua_function(tolua_S,"getTextureName",lua_ax_sceneext_TextureFrame_getTextureName);
        tolua_function(tolua_S,"create", lua_ax_sceneext_TextureFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::TextureFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextTextureFrame";
    g_typeCast[typeName] = "axextTextureFrame";
    return 1;
}

int lua_ax_sceneext_RotationFrame_setRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::RotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::RotationFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_RotationFrame_setRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextRotationFrame:setRotation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationFrame_setRotation'", nullptr);
            return 0;
        }
        obj->setRotation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextRotationFrame:setRotation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationFrame_setRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_RotationFrame_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::RotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::RotationFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_RotationFrame_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationFrame_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRotation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextRotationFrame:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationFrame_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_RotationFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextRotationFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::RotationFrame::create();
        object_to_luaval<ax::ext::timeline::RotationFrame>(tolua_S, "axextRotationFrame",(ax::ext::timeline::RotationFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextRotationFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_RotationFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::RotationFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::RotationFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextRotationFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextRotationFrame:RotationFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_RotationFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (RotationFrame)");
    return 0;
}

int lua_register_ax_sceneext_RotationFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextRotationFrame");
    tolua_cclass(tolua_S,"RotationFrame","axextRotationFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"RotationFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_RotationFrame_constructor);
        tolua_function(tolua_S,"setRotation",lua_ax_sceneext_RotationFrame_setRotation);
        tolua_function(tolua_S,"getRotation",lua_ax_sceneext_RotationFrame_getRotation);
        tolua_function(tolua_S,"create", lua_ax_sceneext_RotationFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::RotationFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextRotationFrame";
    g_typeCast[typeName] = "axextRotationFrame";
    return 1;
}

int lua_ax_sceneext_SkewFrame_setSkewX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkewFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkewFrame_setSkewX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextSkewFrame:setSkewX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_setSkewX'", nullptr);
            return 0;
        }
        obj->setSkewX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkewFrame:setSkewX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_setSkewX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkewFrame_getSkewX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkewFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkewFrame_getSkewX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_getSkewX'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSkewX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkewFrame:getSkewX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_getSkewX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkewFrame_setSkewY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkewFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkewFrame_setSkewY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextSkewFrame:setSkewY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_setSkewY'", nullptr);
            return 0;
        }
        obj->setSkewY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkewFrame:setSkewY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_setSkewY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkewFrame_getSkewY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkewFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkewFrame_getSkewY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_getSkewY'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSkewY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkewFrame:getSkewY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_getSkewY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkewFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::SkewFrame::create();
        object_to_luaval<ax::ext::timeline::SkewFrame>(tolua_S, "axextSkewFrame",(ax::ext::timeline::SkewFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextSkewFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_SkewFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkewFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::SkewFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextSkewFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkewFrame:SkewFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkewFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_SkewFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SkewFrame)");
    return 0;
}

int lua_register_ax_sceneext_SkewFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextSkewFrame");
    tolua_cclass(tolua_S,"SkewFrame","axextSkewFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"SkewFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_SkewFrame_constructor);
        tolua_function(tolua_S,"setSkewX",lua_ax_sceneext_SkewFrame_setSkewX);
        tolua_function(tolua_S,"getSkewX",lua_ax_sceneext_SkewFrame_getSkewX);
        tolua_function(tolua_S,"setSkewY",lua_ax_sceneext_SkewFrame_setSkewY);
        tolua_function(tolua_S,"getSkewY",lua_ax_sceneext_SkewFrame_getSkewY);
        tolua_function(tolua_S,"create", lua_ax_sceneext_SkewFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::SkewFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextSkewFrame";
    g_typeCast[typeName] = "axextSkewFrame";
    return 1;
}

int lua_ax_sceneext_RotationSkewFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextRotationSkewFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationSkewFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::RotationSkewFrame::create();
        object_to_luaval<ax::ext::timeline::RotationSkewFrame>(tolua_S, "axextRotationSkewFrame",(ax::ext::timeline::RotationSkewFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextRotationSkewFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationSkewFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_RotationSkewFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::RotationSkewFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_RotationSkewFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::RotationSkewFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextRotationSkewFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextRotationSkewFrame:RotationSkewFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_RotationSkewFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_RotationSkewFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (RotationSkewFrame)");
    return 0;
}

int lua_register_ax_sceneext_RotationSkewFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextRotationSkewFrame");
    tolua_cclass(tolua_S,"RotationSkewFrame","axextRotationSkewFrame","axextSkewFrame",nullptr);

    tolua_beginmodule(tolua_S,"RotationSkewFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_RotationSkewFrame_constructor);
        tolua_function(tolua_S,"create", lua_ax_sceneext_RotationSkewFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::RotationSkewFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextRotationSkewFrame";
    g_typeCast[typeName] = "axextRotationSkewFrame";
    return 1;
}

int lua_ax_sceneext_PositionFrame_setPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_setPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Point arg0;

        ok &= luaval_to_point(tolua_S, 2, &arg0, "axextPositionFrame:setPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_setPosition'", nullptr);
            return 0;
        }
        obj->setPosition(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:setPosition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_setPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPosition();
        point_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_setX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_setX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextPositionFrame:setX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_setX'", nullptr);
            return 0;
        }
        obj->setX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:setX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_setX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_setY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_setY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextPositionFrame:setY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_setY'", nullptr);
            return 0;
        }
        obj->setY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:setY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_setY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_getX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_getX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_getX'", nullptr);
            return 0;
        }
        auto&& ret = obj->getX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:getX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_getX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_getY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PositionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PositionFrame_getY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_getY'", nullptr);
            return 0;
        }
        auto&& ret = obj->getY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:getY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_getY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PositionFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextPositionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::PositionFrame::create();
        object_to_luaval<ax::ext::timeline::PositionFrame>(tolua_S, "axextPositionFrame",(ax::ext::timeline::PositionFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextPositionFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_PositionFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PositionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PositionFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::PositionFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextPositionFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPositionFrame:PositionFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PositionFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_PositionFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PositionFrame)");
    return 0;
}

int lua_register_ax_sceneext_PositionFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextPositionFrame");
    tolua_cclass(tolua_S,"PositionFrame","axextPositionFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"PositionFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_PositionFrame_constructor);
        tolua_function(tolua_S,"setPosition",lua_ax_sceneext_PositionFrame_setPosition);
        tolua_function(tolua_S,"getPosition",lua_ax_sceneext_PositionFrame_getPosition);
        tolua_function(tolua_S,"setX",lua_ax_sceneext_PositionFrame_setX);
        tolua_function(tolua_S,"setY",lua_ax_sceneext_PositionFrame_setY);
        tolua_function(tolua_S,"getX",lua_ax_sceneext_PositionFrame_getX);
        tolua_function(tolua_S,"getY",lua_ax_sceneext_PositionFrame_getY);
        tolua_function(tolua_S,"create", lua_ax_sceneext_PositionFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::PositionFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextPositionFrame";
    g_typeCast[typeName] = "axextPositionFrame";
    return 1;
}

int lua_ax_sceneext_ScaleFrame_setScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ScaleFrame_setScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextScaleFrame:setScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_setScale'", nullptr);
            return 0;
        }
        obj->setScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:setScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_setScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ScaleFrame_setScaleX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ScaleFrame_setScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextScaleFrame:setScaleX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_setScaleX'", nullptr);
            return 0;
        }
        obj->setScaleX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:setScaleX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_setScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ScaleFrame_getScaleX(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ScaleFrame_getScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_getScaleX'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScaleX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:getScaleX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_getScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ScaleFrame_setScaleY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ScaleFrame_setScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextScaleFrame:setScaleY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_setScaleY'", nullptr);
            return 0;
        }
        obj->setScaleY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:setScaleY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_setScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ScaleFrame_getScaleY(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ScaleFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ScaleFrame_getScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_getScaleY'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScaleY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:getScaleY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_getScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ScaleFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextScaleFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ScaleFrame::create();
        object_to_luaval<ax::ext::timeline::ScaleFrame>(tolua_S, "axextScaleFrame",(ax::ext::timeline::ScaleFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextScaleFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ScaleFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ScaleFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ScaleFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ScaleFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextScaleFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextScaleFrame:ScaleFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ScaleFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ScaleFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ScaleFrame)");
    return 0;
}

int lua_register_ax_sceneext_ScaleFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextScaleFrame");
    tolua_cclass(tolua_S,"ScaleFrame","axextScaleFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"ScaleFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ScaleFrame_constructor);
        tolua_function(tolua_S,"setScale",lua_ax_sceneext_ScaleFrame_setScale);
        tolua_function(tolua_S,"setScaleX",lua_ax_sceneext_ScaleFrame_setScaleX);
        tolua_function(tolua_S,"getScaleX",lua_ax_sceneext_ScaleFrame_getScaleX);
        tolua_function(tolua_S,"setScaleY",lua_ax_sceneext_ScaleFrame_setScaleY);
        tolua_function(tolua_S,"getScaleY",lua_ax_sceneext_ScaleFrame_getScaleY);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ScaleFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ScaleFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextScaleFrame";
    g_typeCast[typeName] = "axextScaleFrame";
    return 1;
}

int lua_ax_sceneext_AnchorPointFrame_setAnchorPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AnchorPointFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextAnchorPointFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::AnchorPointFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AnchorPointFrame_setAnchorPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Point arg0;

        ok &= luaval_to_point(tolua_S, 2, &arg0, "axextAnchorPointFrame:setAnchorPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnchorPointFrame_setAnchorPoint'", nullptr);
            return 0;
        }
        obj->setAnchorPoint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAnchorPointFrame:setAnchorPoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnchorPointFrame_setAnchorPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AnchorPointFrame_getAnchorPoint(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AnchorPointFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextAnchorPointFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::AnchorPointFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AnchorPointFrame_getAnchorPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnchorPointFrame_getAnchorPoint'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnchorPoint();
        point_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAnchorPointFrame:getAnchorPoint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnchorPointFrame_getAnchorPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AnchorPointFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextAnchorPointFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnchorPointFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::AnchorPointFrame::create();
        object_to_luaval<ax::ext::timeline::AnchorPointFrame>(tolua_S, "axextAnchorPointFrame",(ax::ext::timeline::AnchorPointFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextAnchorPointFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnchorPointFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_AnchorPointFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AnchorPointFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AnchorPointFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::AnchorPointFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextAnchorPointFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAnchorPointFrame:AnchorPointFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AnchorPointFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_AnchorPointFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (AnchorPointFrame)");
    return 0;
}

int lua_register_ax_sceneext_AnchorPointFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextAnchorPointFrame");
    tolua_cclass(tolua_S,"AnchorPointFrame","axextAnchorPointFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"AnchorPointFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_AnchorPointFrame_constructor);
        tolua_function(tolua_S,"setAnchorPoint",lua_ax_sceneext_AnchorPointFrame_setAnchorPoint);
        tolua_function(tolua_S,"getAnchorPoint",lua_ax_sceneext_AnchorPointFrame_getAnchorPoint);
        tolua_function(tolua_S,"create", lua_ax_sceneext_AnchorPointFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::AnchorPointFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextAnchorPointFrame";
    g_typeCast[typeName] = "axextAnchorPointFrame";
    return 1;
}

int lua_ax_sceneext_InnerActionFrame_setInnerActionType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setInnerActionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::InnerActionType arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextInnerActionFrame:setInnerActionType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setInnerActionType'", nullptr);
            return 0;
        }
        obj->setInnerActionType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setInnerActionType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setInnerActionType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_getInnerActionType(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_getInnerActionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_getInnerActionType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getInnerActionType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:getInnerActionType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_getInnerActionType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_setEnterWithName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setEnterWithName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axextInnerActionFrame:setEnterWithName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setEnterWithName'", nullptr);
            return 0;
        }
        obj->setEnterWithName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setEnterWithName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setEnterWithName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_setStartFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setStartFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextInnerActionFrame:setStartFrameIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setStartFrameIndex'", nullptr);
            return 0;
        }
        obj->setStartFrameIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setStartFrameIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setStartFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_getStartFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_getStartFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_getStartFrameIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStartFrameIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:getStartFrameIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_getStartFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_setEndFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setEndFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextInnerActionFrame:setEndFrameIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setEndFrameIndex'", nullptr);
            return 0;
        }
        obj->setEndFrameIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setEndFrameIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setEndFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_getEndFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_getEndFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_getEndFrameIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEndFrameIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:getEndFrameIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_getEndFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_setAnimationName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setAnimationName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextInnerActionFrame:setAnimationName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setAnimationName'", nullptr);
            return 0;
        }
        obj->setAnimationName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setAnimationName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setAnimationName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_setSingleFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_setSingleFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextInnerActionFrame:setSingleFrameIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_setSingleFrameIndex'", nullptr);
            return 0;
        }
        obj->setSingleFrameIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:setSingleFrameIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_setSingleFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_getSingleFrameIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::InnerActionFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_InnerActionFrame_getSingleFrameIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_getSingleFrameIndex'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSingleFrameIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:getSingleFrameIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_getSingleFrameIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_InnerActionFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextInnerActionFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::InnerActionFrame::create();
        object_to_luaval<ax::ext::timeline::InnerActionFrame>(tolua_S, "axextInnerActionFrame",(ax::ext::timeline::InnerActionFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextInnerActionFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_InnerActionFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::InnerActionFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_InnerActionFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::InnerActionFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextInnerActionFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextInnerActionFrame:InnerActionFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_InnerActionFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_InnerActionFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (InnerActionFrame)");
    return 0;
}

int lua_register_ax_sceneext_InnerActionFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextInnerActionFrame");
    tolua_cclass(tolua_S,"InnerActionFrame","axextInnerActionFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"InnerActionFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_InnerActionFrame_constructor);
        tolua_function(tolua_S,"setInnerActionType",lua_ax_sceneext_InnerActionFrame_setInnerActionType);
        tolua_function(tolua_S,"getInnerActionType",lua_ax_sceneext_InnerActionFrame_getInnerActionType);
        tolua_function(tolua_S,"setEnterWithName",lua_ax_sceneext_InnerActionFrame_setEnterWithName);
        tolua_function(tolua_S,"setStartFrameIndex",lua_ax_sceneext_InnerActionFrame_setStartFrameIndex);
        tolua_function(tolua_S,"getStartFrameIndex",lua_ax_sceneext_InnerActionFrame_getStartFrameIndex);
        tolua_function(tolua_S,"setEndFrameIndex",lua_ax_sceneext_InnerActionFrame_setEndFrameIndex);
        tolua_function(tolua_S,"getEndFrameIndex",lua_ax_sceneext_InnerActionFrame_getEndFrameIndex);
        tolua_function(tolua_S,"setAnimationName",lua_ax_sceneext_InnerActionFrame_setAnimationName);
        tolua_function(tolua_S,"setSingleFrameIndex",lua_ax_sceneext_InnerActionFrame_setSingleFrameIndex);
        tolua_function(tolua_S,"getSingleFrameIndex",lua_ax_sceneext_InnerActionFrame_getSingleFrameIndex);
        tolua_function(tolua_S,"create", lua_ax_sceneext_InnerActionFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::InnerActionFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextInnerActionFrame";
    g_typeCast[typeName] = "axextInnerActionFrame";
    return 1;
}

int lua_ax_sceneext_ColorFrame_setAlpha(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ColorFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextColorFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ColorFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ColorFrame_setAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axextColorFrame:setAlpha");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ColorFrame_setAlpha'", nullptr);
            return 0;
        }
        obj->setAlpha(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextColorFrame:setAlpha",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ColorFrame_setAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ColorFrame_setColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ColorFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextColorFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ColorFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ColorFrame_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Color32 arg0;

        ok &=luaval_to_color32(tolua_S, 2, &arg0, "axextColorFrame:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ColorFrame_setColor'", nullptr);
            return 0;
        }
        obj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextColorFrame:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ColorFrame_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ColorFrame_getColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ColorFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextColorFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ColorFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ColorFrame_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ColorFrame_getColor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getColor();
        color32_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextColorFrame:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ColorFrame_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ColorFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextColorFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ColorFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ColorFrame::create();
        object_to_luaval<ax::ext::timeline::ColorFrame>(tolua_S, "axextColorFrame",(ax::ext::timeline::ColorFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextColorFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ColorFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ColorFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ColorFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ColorFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ColorFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextColorFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextColorFrame:ColorFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ColorFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ColorFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ColorFrame)");
    return 0;
}

int lua_register_ax_sceneext_ColorFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextColorFrame");
    tolua_cclass(tolua_S,"ColorFrame","axextColorFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"ColorFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ColorFrame_constructor);
        tolua_function(tolua_S,"setAlpha",lua_ax_sceneext_ColorFrame_setAlpha);
        tolua_function(tolua_S,"setColor",lua_ax_sceneext_ColorFrame_setColor);
        tolua_function(tolua_S,"getColor",lua_ax_sceneext_ColorFrame_getColor);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ColorFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ColorFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextColorFrame";
    g_typeCast[typeName] = "axextColorFrame";
    return 1;
}

int lua_ax_sceneext_AlphaFrame_setAlpha(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AlphaFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextAlphaFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::AlphaFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AlphaFrame_setAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        uint16_t arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextAlphaFrame:setAlpha");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AlphaFrame_setAlpha'", nullptr);
            return 0;
        }
        obj->setAlpha(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAlphaFrame:setAlpha",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AlphaFrame_setAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AlphaFrame_getAlpha(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AlphaFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextAlphaFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::AlphaFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_AlphaFrame_getAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AlphaFrame_getAlpha'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAlpha();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAlphaFrame:getAlpha",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AlphaFrame_getAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_AlphaFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextAlphaFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AlphaFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::AlphaFrame::create();
        object_to_luaval<ax::ext::timeline::AlphaFrame>(tolua_S, "axextAlphaFrame",(ax::ext::timeline::AlphaFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextAlphaFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AlphaFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_AlphaFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::AlphaFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_AlphaFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::AlphaFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextAlphaFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextAlphaFrame:AlphaFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_AlphaFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_AlphaFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (AlphaFrame)");
    return 0;
}

int lua_register_ax_sceneext_AlphaFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextAlphaFrame");
    tolua_cclass(tolua_S,"AlphaFrame","axextAlphaFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"AlphaFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_AlphaFrame_constructor);
        tolua_function(tolua_S,"setAlpha",lua_ax_sceneext_AlphaFrame_setAlpha);
        tolua_function(tolua_S,"getAlpha",lua_ax_sceneext_AlphaFrame_getAlpha);
        tolua_function(tolua_S,"create", lua_ax_sceneext_AlphaFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::AlphaFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextAlphaFrame";
    g_typeCast[typeName] = "axextAlphaFrame";
    return 1;
}

int lua_ax_sceneext_EventFrame_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::EventFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextEventFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::EventFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_EventFrame_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_EventFrame_init'", nullptr);
            return 0;
        }
        obj->init();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextEventFrame:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_EventFrame_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_EventFrame_setEvent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::EventFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextEventFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::EventFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_EventFrame_setEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextEventFrame:setEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_EventFrame_setEvent'", nullptr);
            return 0;
        }
        obj->setEvent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextEventFrame:setEvent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_EventFrame_setEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_EventFrame_getEvent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::EventFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextEventFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::EventFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_EventFrame_getEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_EventFrame_getEvent'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEvent();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextEventFrame:getEvent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_EventFrame_getEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_EventFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextEventFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_EventFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::EventFrame::create();
        object_to_luaval<ax::ext::timeline::EventFrame>(tolua_S, "axextEventFrame",(ax::ext::timeline::EventFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextEventFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_EventFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_EventFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::EventFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_EventFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::EventFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextEventFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextEventFrame:EventFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_EventFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_EventFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EventFrame)");
    return 0;
}

int lua_register_ax_sceneext_EventFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextEventFrame");
    tolua_cclass(tolua_S,"EventFrame","axextEventFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"EventFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_EventFrame_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_EventFrame_init);
        tolua_function(tolua_S,"setEvent",lua_ax_sceneext_EventFrame_setEvent);
        tolua_function(tolua_S,"getEvent",lua_ax_sceneext_EventFrame_getEvent);
        tolua_function(tolua_S,"create", lua_ax_sceneext_EventFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::EventFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextEventFrame";
    g_typeCast[typeName] = "axextEventFrame";
    return 1;
}

int lua_ax_sceneext_ZOrderFrame_setZOrder(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ZOrderFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextZOrderFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ZOrderFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ZOrderFrame_setZOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextZOrderFrame:setZOrder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ZOrderFrame_setZOrder'", nullptr);
            return 0;
        }
        obj->setZOrder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextZOrderFrame:setZOrder",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ZOrderFrame_setZOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ZOrderFrame_getZOrder(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ZOrderFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextZOrderFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ZOrderFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ZOrderFrame_getZOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ZOrderFrame_getZOrder'", nullptr);
            return 0;
        }
        auto&& ret = obj->getZOrder();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextZOrderFrame:getZOrder",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ZOrderFrame_getZOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ZOrderFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextZOrderFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ZOrderFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ZOrderFrame::create();
        object_to_luaval<ax::ext::timeline::ZOrderFrame>(tolua_S, "axextZOrderFrame",(ax::ext::timeline::ZOrderFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextZOrderFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ZOrderFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ZOrderFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ZOrderFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ZOrderFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ZOrderFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextZOrderFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextZOrderFrame:ZOrderFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ZOrderFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ZOrderFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ZOrderFrame)");
    return 0;
}

int lua_register_ax_sceneext_ZOrderFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextZOrderFrame");
    tolua_cclass(tolua_S,"ZOrderFrame","axextZOrderFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"ZOrderFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ZOrderFrame_constructor);
        tolua_function(tolua_S,"setZOrder",lua_ax_sceneext_ZOrderFrame_setZOrder);
        tolua_function(tolua_S,"getZOrder",lua_ax_sceneext_ZOrderFrame_getZOrder);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ZOrderFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ZOrderFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextZOrderFrame";
    g_typeCast[typeName] = "axextZOrderFrame";
    return 1;
}

int lua_ax_sceneext_BlendFuncFrame_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BlendFuncFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBlendFuncFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BlendFuncFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BlendFuncFrame_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BlendFuncFrame_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBlendFuncFrame:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BlendFuncFrame_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BlendFuncFrame_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BlendFuncFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBlendFuncFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BlendFuncFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BlendFuncFrame_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "axextBlendFuncFrame:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BlendFuncFrame_setBlendFunc'", nullptr);
            return 0;
        }
        obj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBlendFuncFrame:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BlendFuncFrame_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BlendFuncFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextBlendFuncFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BlendFuncFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::BlendFuncFrame::create();
        object_to_luaval<ax::ext::timeline::BlendFuncFrame>(tolua_S, "axextBlendFuncFrame",(ax::ext::timeline::BlendFuncFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextBlendFuncFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BlendFuncFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_BlendFuncFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BlendFuncFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BlendFuncFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::BlendFuncFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextBlendFuncFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBlendFuncFrame:BlendFuncFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BlendFuncFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_BlendFuncFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BlendFuncFrame)");
    return 0;
}

int lua_register_ax_sceneext_BlendFuncFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextBlendFuncFrame");
    tolua_cclass(tolua_S,"BlendFuncFrame","axextBlendFuncFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"BlendFuncFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_BlendFuncFrame_constructor);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_sceneext_BlendFuncFrame_getBlendFunc);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_sceneext_BlendFuncFrame_setBlendFunc);
        tolua_function(tolua_S,"create", lua_ax_sceneext_BlendFuncFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::BlendFuncFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextBlendFuncFrame";
    g_typeCast[typeName] = "axextBlendFuncFrame";
    return 1;
}

int lua_ax_sceneext_PlayableFrame_getPlayableAct(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PlayableFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPlayableFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PlayableFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PlayableFrame_getPlayableAct'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PlayableFrame_getPlayableAct'", nullptr);
            return 0;
        }
        auto&& ret = obj->getPlayableAct();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPlayableFrame:getPlayableAct",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PlayableFrame_getPlayableAct'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PlayableFrame_setPlayableAct(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PlayableFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextPlayableFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::PlayableFrame*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_PlayableFrame_setPlayableAct'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "axextPlayableFrame:setPlayableAct");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PlayableFrame_setPlayableAct'", nullptr);
            return 0;
        }
        obj->setPlayableAct(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPlayableFrame:setPlayableAct",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PlayableFrame_setPlayableAct'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_PlayableFrame_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextPlayableFrame",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PlayableFrame_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::PlayableFrame::create();
        object_to_luaval<ax::ext::timeline::PlayableFrame>(tolua_S, "axextPlayableFrame",(ax::ext::timeline::PlayableFrame*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextPlayableFrame:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PlayableFrame_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_PlayableFrame_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::PlayableFrame* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_PlayableFrame_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::PlayableFrame();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextPlayableFrame");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextPlayableFrame:PlayableFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_PlayableFrame_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_PlayableFrame_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PlayableFrame)");
    return 0;
}

int lua_register_ax_sceneext_PlayableFrame(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextPlayableFrame");
    tolua_cclass(tolua_S,"PlayableFrame","axextPlayableFrame","axextFrame",nullptr);

    tolua_beginmodule(tolua_S,"PlayableFrame");
        tolua_function(tolua_S,"new",lua_ax_sceneext_PlayableFrame_constructor);
        tolua_function(tolua_S,"getPlayableAct",lua_ax_sceneext_PlayableFrame_getPlayableAct);
        tolua_function(tolua_S,"setPlayableAct",lua_ax_sceneext_PlayableFrame_setPlayableAct);
        tolua_function(tolua_S,"create", lua_ax_sceneext_PlayableFrame_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::PlayableFrame).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextPlayableFrame";
    g_typeCast[typeName] = "axextPlayableFrame";
    return 1;
}

int lua_ax_sceneext_Timeline_gotoFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_gotoFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextTimeline:gotoFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_gotoFrame'", nullptr);
            return 0;
        }
        obj->gotoFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:gotoFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_gotoFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_stepToFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_stepToFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextTimeline:stepToFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_stepToFrame'", nullptr);
            return 0;
        }
        obj->stepToFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:stepToFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_stepToFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_getFrames(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_getFrames'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_getFrames'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFrames();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:getFrames",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_getFrames'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_addFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_addFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::Frame* arg0;

        ok &= luaval_to_object<ax::ext::timeline::Frame>(tolua_S, 2, "axextFrame",&arg0, "axextTimeline:addFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_addFrame'", nullptr);
            return 0;
        }
        obj->addFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:addFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_addFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_insertFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_insertFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::ext::timeline::Frame* arg0;
        int arg1;

        ok &= luaval_to_object<ax::ext::timeline::Frame>(tolua_S, 2, "axextFrame",&arg0, "axextTimeline:insertFrame");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axextTimeline:insertFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_insertFrame'", nullptr);
            return 0;
        }
        obj->insertFrame(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:insertFrame",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_insertFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_removeFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_removeFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::Frame* arg0;

        ok &= luaval_to_object<ax::ext::timeline::Frame>(tolua_S, 2, "axextFrame",&arg0, "axextTimeline:removeFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_removeFrame'", nullptr);
            return 0;
        }
        obj->removeFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:removeFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_removeFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_setActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_setActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextTimeline:setActionTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_setActionTag'", nullptr);
            return 0;
        }
        obj->setActionTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:setActionTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_setActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_getActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_getActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_getActionTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:getActionTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_getActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_setNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_setNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextTimeline:setNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_setNode'", nullptr);
            return 0;
        }
        obj->setNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:setNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_setNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_getNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_getNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_getNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getNode();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:getNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_getNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_setActionTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_setActionTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::ActionTimeline* arg0;

        ok &= luaval_to_object<ax::ext::timeline::ActionTimeline>(tolua_S, 2, "axextActionTimeline",&arg0, "axextTimeline:setActionTimeline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_setActionTimeline'", nullptr);
            return 0;
        }
        obj->setActionTimeline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:setActionTimeline",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_setActionTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_getActionTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_getActionTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_getActionTimeline'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionTimeline();
        object_to_luaval<ax::ext::timeline::ActionTimeline>(tolua_S, "axextActionTimeline",(ax::ext::timeline::ActionTimeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:getActionTimeline",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_getActionTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_clone(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::Timeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_Timeline_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone();
        object_to_luaval<ax::ext::timeline::Timeline>(tolua_S, "axextTimeline",(ax::ext::timeline::Timeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_Timeline_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::Timeline::create();
        object_to_luaval<ax::ext::timeline::Timeline>(tolua_S, "axextTimeline",(ax::ext::timeline::Timeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextTimeline:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_Timeline_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::Timeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_Timeline_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::Timeline();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextTimeline");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextTimeline:Timeline",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_Timeline_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_Timeline_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Timeline)");
    return 0;
}

int lua_register_ax_sceneext_Timeline(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextTimeline");
    tolua_cclass(tolua_S,"Timeline","axextTimeline","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Timeline");
        tolua_function(tolua_S,"new",lua_ax_sceneext_Timeline_constructor);
        tolua_function(tolua_S,"gotoFrame",lua_ax_sceneext_Timeline_gotoFrame);
        tolua_function(tolua_S,"stepToFrame",lua_ax_sceneext_Timeline_stepToFrame);
        tolua_function(tolua_S,"getFrames",lua_ax_sceneext_Timeline_getFrames);
        tolua_function(tolua_S,"addFrame",lua_ax_sceneext_Timeline_addFrame);
        tolua_function(tolua_S,"insertFrame",lua_ax_sceneext_Timeline_insertFrame);
        tolua_function(tolua_S,"removeFrame",lua_ax_sceneext_Timeline_removeFrame);
        tolua_function(tolua_S,"setActionTag",lua_ax_sceneext_Timeline_setActionTag);
        tolua_function(tolua_S,"getActionTag",lua_ax_sceneext_Timeline_getActionTag);
        tolua_function(tolua_S,"setNode",lua_ax_sceneext_Timeline_setNode);
        tolua_function(tolua_S,"getNode",lua_ax_sceneext_Timeline_getNode);
        tolua_function(tolua_S,"setActionTimeline",lua_ax_sceneext_Timeline_setActionTimeline);
        tolua_function(tolua_S,"getActionTimeline",lua_ax_sceneext_Timeline_getActionTimeline);
        tolua_function(tolua_S,"clone",lua_ax_sceneext_Timeline_clone);
        tolua_function(tolua_S,"create", lua_ax_sceneext_Timeline_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::Timeline).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextTimeline";
    g_typeCast[typeName] = "axextTimeline";
    return 1;
}

int lua_ax_sceneext_ActionTimelineData_setActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineData_setActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimelineData:setActionTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineData_setActionTag'", nullptr);
            return 0;
        }
        obj->setActionTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineData:setActionTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineData_setActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineData_getActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineData_getActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineData_getActionTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineData:getActionTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineData_getActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineData_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineData_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimelineData:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineData_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineData:init",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineData_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextActionTimelineData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimelineData:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ActionTimelineData::create(arg0);
        object_to_luaval<ax::ext::timeline::ActionTimelineData>(tolua_S, "axextActionTimelineData",(ax::ext::timeline::ActionTimelineData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextActionTimelineData:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ActionTimelineData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ActionTimelineData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextActionTimelineData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineData:ActionTimelineData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionTimelineData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionTimelineData)");
    return 0;
}

int lua_register_ax_sceneext_ActionTimelineData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextActionTimelineData");
    tolua_cclass(tolua_S,"ActionTimelineData","axextActionTimelineData","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ActionTimelineData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionTimelineData_constructor);
        tolua_function(tolua_S,"setActionTag",lua_ax_sceneext_ActionTimelineData_setActionTag);
        tolua_function(tolua_S,"getActionTag",lua_ax_sceneext_ActionTimelineData_getActionTag);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ActionTimelineData_init);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ActionTimelineData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ActionTimelineData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextActionTimelineData";
    g_typeCast[typeName] = "axextActionTimelineData";
    return 1;
}

int lua_ax_sceneext_ActionTimeline_play(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_play'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        bool arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextActionTimeline:play");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextActionTimeline:play");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_play'", nullptr);
            return 0;
        }
        obj->play(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:play",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_play'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_gotoFrameAndPlay(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_gotoFrameAndPlay'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            obj->gotoFrameAndPlay(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            obj->gotoFrameAndPlay(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 3) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4, &arg2, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            obj->gotoFrameAndPlay(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 4) {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int(tolua_S, 4, &arg2, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            bool arg3;
            ok &= luaval_to_boolean(tolua_S, 5, &arg3, "axextActionTimeline:gotoFrameAndPlay");

            if (!ok) { break; }
            obj->gotoFrameAndPlay(arg0, arg1, arg2, arg3);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextActionTimeline:gotoFrameAndPlay",argc, 4);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_gotoFrameAndPlay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_gotoFrameAndPause(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_gotoFrameAndPause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:gotoFrameAndPause");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_gotoFrameAndPause'", nullptr);
            return 0;
        }
        obj->gotoFrameAndPause(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:gotoFrameAndPause",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_gotoFrameAndPause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_pause(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_pause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_pause'", nullptr);
            return 0;
        }
        obj->pause();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:pause",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_pause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_resume(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_resume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_resume'", nullptr);
            return 0;
        }
        obj->resume();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:resume",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_resume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = obj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_setTimeSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_setTimeSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextActionTimeline:setTimeSpeed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_setTimeSpeed'", nullptr);
            return 0;
        }
        obj->setTimeSpeed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:setTimeSpeed",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_setTimeSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getTimeSpeed(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getTimeSpeed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getTimeSpeed'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTimeSpeed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getTimeSpeed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getTimeSpeed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_setDuration(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_setDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:setDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_setDuration'", nullptr);
            return 0;
        }
        obj->setDuration(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:setDuration",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_setDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getDuration(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getDuration'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDuration();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getDuration",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getStartFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getStartFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getStartFrame'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStartFrame();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getStartFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getStartFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getEndFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getEndFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getEndFrame'", nullptr);
            return 0;
        }
        auto&& ret = obj->getEndFrame();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getEndFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getEndFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_setCurrentFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_setCurrentFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:setCurrentFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_setCurrentFrame'", nullptr);
            return 0;
        }
        obj->setCurrentFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:setCurrentFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_setCurrentFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getCurrentFrame(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getCurrentFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getCurrentFrame'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCurrentFrame();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getCurrentFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getCurrentFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_addTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_addTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::Timeline* arg0;

        ok &= luaval_to_object<ax::ext::timeline::Timeline>(tolua_S, 2, "axextTimeline",&arg0, "axextActionTimeline:addTimeline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_addTimeline'", nullptr);
            return 0;
        }
        obj->addTimeline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:addTimeline",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_addTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_removeTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_removeTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::Timeline* arg0;

        ok &= luaval_to_object<ax::ext::timeline::Timeline>(tolua_S, 2, "axextTimeline",&arg0, "axextActionTimeline:removeTimeline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_removeTimeline'", nullptr);
            return 0;
        }
        obj->removeTimeline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:removeTimeline",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_removeTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getTimelines(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getTimelines'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getTimelines'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTimelines();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getTimelines",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getTimelines'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_addAnimationInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_addAnimationInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::AnimationInfo arg0;

        ok &= luaval_to_animationInfo(tolua_S, 2, &arg0, "axextActionTimeline:addAnimationInfo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_addAnimationInfo'", nullptr);
            return 0;
        }
        obj->addAnimationInfo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:addAnimationInfo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_addAnimationInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_removeAnimationInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_removeAnimationInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextActionTimeline:removeAnimationInfo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_removeAnimationInfo'", nullptr);
            return 0;
        }
        obj->removeAnimationInfo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:removeAnimationInfo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_removeAnimationInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_IsAnimationInfoExists(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_IsAnimationInfoExists'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextActionTimeline:IsAnimationInfoExists");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_IsAnimationInfoExists'", nullptr);
            return 0;
        }
        auto&& ret = obj->IsAnimationInfoExists(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:IsAnimationInfoExists",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_IsAnimationInfoExists'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_getAnimationInfo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_getAnimationInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextActionTimeline:getAnimationInfo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_getAnimationInfo'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationInfo(arg0);
        animationInfo_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:getAnimationInfo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_getAnimationInfo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_setAnimationEndCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_setAnimationEndCallFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::function<void ()> arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextActionTimeline:setAnimationEndCallFunc");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_setAnimationEndCallFunc'", nullptr);
            return 0;
        }
        obj->setAnimationEndCallFunc(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:setAnimationEndCallFunc",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_setAnimationEndCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_clearFrameEventCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_clearFrameEventCallFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_clearFrameEventCallFunc'", nullptr);
            return 0;
        }
        obj->clearFrameEventCallFunc();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:clearFrameEventCallFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_clearFrameEventCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_setLastFrameCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_setLastFrameCallFunc'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_setLastFrameCallFunc'", nullptr);
            return 0;
        }
        obj->setLastFrameCallFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:setLastFrameCallFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_setLastFrameCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_clearLastFrameCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_clearLastFrameCallFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_clearLastFrameCallFunc'", nullptr);
            return 0;
        }
        obj->clearLastFrameCallFunc();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:clearLastFrameCallFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_clearLastFrameCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_addFrameEndCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_addFrameEndCallFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        int arg0;
        std::string_view arg1;
        std::function<void ()> arg2;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:addFrameEndCallFunc");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axextActionTimeline:addFrameEndCallFunc");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_addFrameEndCallFunc'", nullptr);
            return 0;
        }
        obj->addFrameEndCallFunc(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:addFrameEndCallFunc",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_addFrameEndCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_removeFrameEndCallFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        std::string_view arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:removeFrameEndCallFunc");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axextActionTimeline:removeFrameEndCallFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFunc'", nullptr);
            return 0;
        }
        obj->removeFrameEndCallFunc(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:removeFrameEndCallFunc",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_removeFrameEndCallFuncs(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFuncs'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axextActionTimeline:removeFrameEndCallFuncs");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFuncs'", nullptr);
            return 0;
        }
        obj->removeFrameEndCallFuncs(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:removeFrameEndCallFuncs",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_removeFrameEndCallFuncs'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_clearFrameEndCallFuncs(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_clearFrameEndCallFuncs'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_clearFrameEndCallFuncs'", nullptr);
            return 0;
        }
        obj->clearFrameEndCallFuncs();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:clearFrameEndCallFuncs",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_clearFrameEndCallFuncs'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_start(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimeline*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimeline_start'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_start'", nullptr);
            return 0;
        }
        obj->start();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:start",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_start'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimeline_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextActionTimeline",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ActionTimeline::create();
        object_to_luaval<ax::ext::timeline::ActionTimeline>(tolua_S, "axextActionTimeline",(ax::ext::timeline::ActionTimeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextActionTimeline:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ActionTimeline_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimeline* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimeline_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ActionTimeline();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextActionTimeline");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimeline:ActionTimeline",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimeline_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionTimeline_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionTimeline)");
    return 0;
}

int lua_register_ax_sceneext_ActionTimeline(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextActionTimeline");
    tolua_cclass(tolua_S,"ActionTimeline","axextActionTimeline","ax.Action",nullptr);

    tolua_beginmodule(tolua_S,"ActionTimeline");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionTimeline_constructor);
        tolua_function(tolua_S,"play",lua_ax_sceneext_ActionTimeline_play);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ActionTimeline_init);
        tolua_function(tolua_S,"gotoFrameAndPlay",lua_ax_sceneext_ActionTimeline_gotoFrameAndPlay);
        tolua_function(tolua_S,"gotoFrameAndPause",lua_ax_sceneext_ActionTimeline_gotoFrameAndPause);
        tolua_function(tolua_S,"pause",lua_ax_sceneext_ActionTimeline_pause);
        tolua_function(tolua_S,"resume",lua_ax_sceneext_ActionTimeline_resume);
        tolua_function(tolua_S,"isPlaying",lua_ax_sceneext_ActionTimeline_isPlaying);
        tolua_function(tolua_S,"setTimeSpeed",lua_ax_sceneext_ActionTimeline_setTimeSpeed);
        tolua_function(tolua_S,"getTimeSpeed",lua_ax_sceneext_ActionTimeline_getTimeSpeed);
        tolua_function(tolua_S,"setDuration",lua_ax_sceneext_ActionTimeline_setDuration);
        tolua_function(tolua_S,"getDuration",lua_ax_sceneext_ActionTimeline_getDuration);
        tolua_function(tolua_S,"getStartFrame",lua_ax_sceneext_ActionTimeline_getStartFrame);
        tolua_function(tolua_S,"getEndFrame",lua_ax_sceneext_ActionTimeline_getEndFrame);
        tolua_function(tolua_S,"setCurrentFrame",lua_ax_sceneext_ActionTimeline_setCurrentFrame);
        tolua_function(tolua_S,"getCurrentFrame",lua_ax_sceneext_ActionTimeline_getCurrentFrame);
        tolua_function(tolua_S,"addTimeline",lua_ax_sceneext_ActionTimeline_addTimeline);
        tolua_function(tolua_S,"removeTimeline",lua_ax_sceneext_ActionTimeline_removeTimeline);
        tolua_function(tolua_S,"getTimelines",lua_ax_sceneext_ActionTimeline_getTimelines);
        tolua_function(tolua_S,"addAnimationInfo",lua_ax_sceneext_ActionTimeline_addAnimationInfo);
        tolua_function(tolua_S,"removeAnimationInfo",lua_ax_sceneext_ActionTimeline_removeAnimationInfo);
        tolua_function(tolua_S,"IsAnimationInfoExists",lua_ax_sceneext_ActionTimeline_IsAnimationInfoExists);
        tolua_function(tolua_S,"getAnimationInfo",lua_ax_sceneext_ActionTimeline_getAnimationInfo);
        tolua_function(tolua_S,"setAnimationEndCallFunc",lua_ax_sceneext_ActionTimeline_setAnimationEndCallFunc);
        tolua_function(tolua_S,"clearFrameEventCallFunc",lua_ax_sceneext_ActionTimeline_clearFrameEventCallFunc);
        tolua_function(tolua_S,"setLastFrameCallFunc",lua_ax_sceneext_ActionTimeline_setLastFrameCallFunc);
        tolua_function(tolua_S,"clearLastFrameCallFunc",lua_ax_sceneext_ActionTimeline_clearLastFrameCallFunc);
        tolua_function(tolua_S,"addFrameEndCallFunc",lua_ax_sceneext_ActionTimeline_addFrameEndCallFunc);
        tolua_function(tolua_S,"removeFrameEndCallFunc",lua_ax_sceneext_ActionTimeline_removeFrameEndCallFunc);
        tolua_function(tolua_S,"removeFrameEndCallFuncs",lua_ax_sceneext_ActionTimeline_removeFrameEndCallFuncs);
        tolua_function(tolua_S,"clearFrameEndCallFuncs",lua_ax_sceneext_ActionTimeline_clearFrameEndCallFuncs);
        tolua_function(tolua_S,"start",lua_ax_sceneext_ActionTimeline_start);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ActionTimeline_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ActionTimeline).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextActionTimeline";
    g_typeCast[typeName] = "axextActionTimeline";
    return 1;
}

int lua_ax_sceneext_ActionTimelineNode_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineNode_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Node* arg0;
        ax::ext::timeline::ActionTimeline* arg1;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextActionTimelineNode:init");

        ok &= luaval_to_object<ax::ext::timeline::ActionTimeline>(tolua_S, 3, "axextActionTimeline",&arg1, "axextActionTimelineNode:init");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_init'", nullptr);
            return 0;
        }
        auto&& ret = obj->init(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:init",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_setRoot(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineNode_setRoot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextActionTimelineNode:setRoot");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_setRoot'", nullptr);
            return 0;
        }
        obj->setRoot(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:setRoot",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_setRoot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_getRoot(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineNode_getRoot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_getRoot'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRoot();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:getRoot",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_getRoot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_setActionTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineNode_setActionTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::ext::timeline::ActionTimeline* arg0;

        ok &= luaval_to_object<ax::ext::timeline::ActionTimeline>(tolua_S, 2, "axextActionTimeline",&arg0, "axextActionTimelineNode:setActionTimeline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_setActionTimeline'", nullptr);
            return 0;
        }
        obj->setActionTimeline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:setActionTimeline",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_setActionTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_getActionTimeline(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::ActionTimelineNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ActionTimelineNode_getActionTimeline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_getActionTimeline'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionTimeline();
        object_to_luaval<ax::ext::timeline::ActionTimeline>(tolua_S, "axextActionTimeline",(ax::ext::timeline::ActionTimeline*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:getActionTimeline",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_getActionTimeline'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextActionTimelineNode",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::Node* arg0;
        ax::ext::timeline::ActionTimeline* arg1;
        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextActionTimelineNode:create");
        ok &= luaval_to_object<ax::ext::timeline::ActionTimeline>(tolua_S, 3, "axextActionTimeline",&arg1, "axextActionTimelineNode:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::ActionTimelineNode::create(arg0, arg1);
        object_to_luaval<ax::ext::timeline::ActionTimelineNode>(tolua_S, "axextActionTimelineNode",(ax::ext::timeline::ActionTimelineNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextActionTimelineNode:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ActionTimelineNode_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::ActionTimelineNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ActionTimelineNode_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::ActionTimelineNode();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextActionTimelineNode");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextActionTimelineNode:ActionTimelineNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ActionTimelineNode_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ActionTimelineNode_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ActionTimelineNode)");
    return 0;
}

int lua_register_ax_sceneext_ActionTimelineNode(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextActionTimelineNode");
    tolua_cclass(tolua_S,"ActionTimelineNode","axextActionTimelineNode","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"ActionTimelineNode");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ActionTimelineNode_constructor);
        tolua_function(tolua_S,"init",lua_ax_sceneext_ActionTimelineNode_init);
        tolua_function(tolua_S,"setRoot",lua_ax_sceneext_ActionTimelineNode_setRoot);
        tolua_function(tolua_S,"getRoot",lua_ax_sceneext_ActionTimelineNode_getRoot);
        tolua_function(tolua_S,"setActionTimeline",lua_ax_sceneext_ActionTimelineNode_setActionTimeline);
        tolua_function(tolua_S,"getActionTimeline",lua_ax_sceneext_ActionTimelineNode_getActionTimeline);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ActionTimelineNode_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::ActionTimelineNode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextActionTimelineNode";
    g_typeCast[typeName] = "axextActionTimelineNode";
    return 1;
}

int lua_ax_sceneext_BoneNode_getChildBones(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getChildBones'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 0) {
            ax::Vector<ax::ext::timeline::BoneNode *>& ret = obj->getChildBones();
            axvector_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 0) {
            const ax::Vector<ax::ext::timeline::BoneNode *>& ret = obj->getChildBones();
            axvector_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextBoneNode:getChildBones",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getChildBones'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getRootSkeletonNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getRootSkeletonNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getRootSkeletonNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRootSkeletonNode();
        object_to_luaval<ax::ext::timeline::SkeletonNode>(tolua_S, "axextSkeletonNode",(ax::ext::timeline::SkeletonNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getRootSkeletonNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getRootSkeletonNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getAllSubBones(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getAllSubBones'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getAllSubBones'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAllSubBones();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getAllSubBones",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getAllSubBones'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_addSkin(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_addSkin'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 3) {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextBoneNode:addSkin");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextBoneNode:addSkin");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4, &arg2, "axextBoneNode:addSkin");

            if (!ok) { break; }
            obj->addSkin(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextBoneNode:addSkin");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextBoneNode:addSkin");

            if (!ok) { break; }
            obj->addSkin(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextBoneNode:addSkin",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_addSkin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_displaySkin(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_displaySkin'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 2) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextBoneNode:displaySkin");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextBoneNode:displaySkin");

            if (!ok) { break; }
            obj->displaySkin(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 2) {
            ax::Node* arg0;
            ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "axextBoneNode:displaySkin");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3, &arg1, "axextBoneNode:displaySkin");

            if (!ok) { break; }
            obj->displaySkin(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextBoneNode:displaySkin",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_displaySkin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getVisibleSkins(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getVisibleSkins'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getVisibleSkins'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVisibleSkins();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getVisibleSkins",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getVisibleSkins'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getSkins(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getSkins'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 0) {
            ax::Vector<ax::Node *>& ret = obj->getSkins();
            axvector_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 0) {
            const ax::Vector<ax::Node *>& ret = obj->getSkins();
            axvector_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextBoneNode:getSkins",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getSkins'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getAllSubSkins(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getAllSubSkins'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getAllSubSkins'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAllSubSkins();
        axvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getAllSubSkins",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getAllSubSkins'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "axextBoneNode:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_setBlendFunc'", nullptr);
            return 0;
        }
        obj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_setDebugDrawEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_setDebugDrawEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "axextBoneNode:setDebugDrawEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_setDebugDrawEnabled'", nullptr);
            return 0;
        }
        obj->setDebugDrawEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:setDebugDrawEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_setDebugDrawEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_isDebugDrawEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_isDebugDrawEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_isDebugDrawEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->isDebugDrawEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:isDebugDrawEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_isDebugDrawEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_setDebugDrawLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_setDebugDrawLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextBoneNode:setDebugDrawLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_setDebugDrawLength'", nullptr);
            return 0;
        }
        obj->setDebugDrawLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:setDebugDrawLength",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_setDebugDrawLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getDebugDrawLength(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getDebugDrawLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getDebugDrawLength'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugDrawLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getDebugDrawLength",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getDebugDrawLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_setDebugDrawWidth(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_setDebugDrawWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "axextBoneNode:setDebugDrawWidth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_setDebugDrawWidth'", nullptr);
            return 0;
        }
        obj->setDebugDrawWidth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:setDebugDrawWidth",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_setDebugDrawWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getDebugDrawWidth(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getDebugDrawWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getDebugDrawWidth'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugDrawWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getDebugDrawWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getDebugDrawWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_setDebugDrawColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_setDebugDrawColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Color arg0;

        ok &=luaval_to_color(tolua_S, 2, &arg0, "axextBoneNode:setDebugDrawColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_setDebugDrawColor'", nullptr);
            return 0;
        }
        obj->setDebugDrawColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:setDebugDrawColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_setDebugDrawColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getDebugDrawColor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getDebugDrawColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getDebugDrawColor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugDrawColor();
        color_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getDebugDrawColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getDebugDrawColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_getVisibleSkinsRect(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::BoneNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_BoneNode_getVisibleSkinsRect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_getVisibleSkinsRect'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVisibleSkinsRect();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:getVisibleSkinsRect",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_getVisibleSkinsRect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_BoneNode_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextBoneNode",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 1)
        {
            int arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axextBoneNode:create");
            if (!ok) { break; }
            ax::ext::timeline::BoneNode* ret = ax::ext::timeline::BoneNode::create(arg0);
            object_to_luaval<ax::ext::timeline::BoneNode>(tolua_S, "axextBoneNode",(ax::ext::timeline::BoneNode*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            ax::ext::timeline::BoneNode* ret = ax::ext::timeline::BoneNode::create();
            object_to_luaval<ax::ext::timeline::BoneNode>(tolua_S, "axextBoneNode",(ax::ext::timeline::BoneNode*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "axextBoneNode:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_BoneNode_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::BoneNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_BoneNode_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::BoneNode();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextBoneNode");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextBoneNode:BoneNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_BoneNode_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_BoneNode_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (BoneNode)");
    return 0;
}

int lua_register_ax_sceneext_BoneNode(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextBoneNode");
    tolua_cclass(tolua_S,"BoneNode","axextBoneNode","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"BoneNode");
        tolua_function(tolua_S,"new",lua_ax_sceneext_BoneNode_constructor);
        tolua_function(tolua_S,"getChildBones",lua_ax_sceneext_BoneNode_getChildBones);
        tolua_function(tolua_S,"getRootSkeletonNode",lua_ax_sceneext_BoneNode_getRootSkeletonNode);
        tolua_function(tolua_S,"getAllSubBones",lua_ax_sceneext_BoneNode_getAllSubBones);
        tolua_function(tolua_S,"addSkin",lua_ax_sceneext_BoneNode_addSkin);
        tolua_function(tolua_S,"displaySkin",lua_ax_sceneext_BoneNode_displaySkin);
        tolua_function(tolua_S,"getVisibleSkins",lua_ax_sceneext_BoneNode_getVisibleSkins);
        tolua_function(tolua_S,"getSkins",lua_ax_sceneext_BoneNode_getSkins);
        tolua_function(tolua_S,"getAllSubSkins",lua_ax_sceneext_BoneNode_getAllSubSkins);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_sceneext_BoneNode_setBlendFunc);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_sceneext_BoneNode_getBlendFunc);
        tolua_function(tolua_S,"setDebugDrawEnabled",lua_ax_sceneext_BoneNode_setDebugDrawEnabled);
        tolua_function(tolua_S,"isDebugDrawEnabled",lua_ax_sceneext_BoneNode_isDebugDrawEnabled);
        tolua_function(tolua_S,"setDebugDrawLength",lua_ax_sceneext_BoneNode_setDebugDrawLength);
        tolua_function(tolua_S,"getDebugDrawLength",lua_ax_sceneext_BoneNode_getDebugDrawLength);
        tolua_function(tolua_S,"setDebugDrawWidth",lua_ax_sceneext_BoneNode_setDebugDrawWidth);
        tolua_function(tolua_S,"getDebugDrawWidth",lua_ax_sceneext_BoneNode_getDebugDrawWidth);
        tolua_function(tolua_S,"setDebugDrawColor",lua_ax_sceneext_BoneNode_setDebugDrawColor);
        tolua_function(tolua_S,"getDebugDrawColor",lua_ax_sceneext_BoneNode_getDebugDrawColor);
        tolua_function(tolua_S,"getVisibleSkinsRect",lua_ax_sceneext_BoneNode_getVisibleSkinsRect);
        tolua_function(tolua_S,"create", lua_ax_sceneext_BoneNode_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::BoneNode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextBoneNode";
    g_typeCast[typeName] = "axextBoneNode";
    return 1;
}

int lua_ax_sceneext_SkeletonNode_getBoneNode(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkeletonNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkeletonNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkeletonNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkeletonNode_getBoneNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextSkeletonNode:getBoneNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkeletonNode_getBoneNode'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBoneNode(arg0);
        object_to_luaval<ax::ext::timeline::BoneNode>(tolua_S, "axextBoneNode",(ax::ext::timeline::BoneNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkeletonNode:getBoneNode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_getBoneNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkeletonNode_getAllSubBonesMap(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkeletonNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkeletonNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkeletonNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkeletonNode_getAllSubBonesMap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkeletonNode_getAllSubBonesMap'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAllSubBonesMap();
        axmap_string_key_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkeletonNode:getAllSubBonesMap",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_getAllSubBonesMap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkeletonNode_changeSkins(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkeletonNode* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkeletonNode",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::ext::timeline::SkeletonNode*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkeletonNode_changeSkins'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextSkeletonNode:changeSkins");

            if (!ok) { break; }
            obj->changeSkins(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            tsl::robin_map<std::string, std::string, tlx::string_hash, tlx::equal_to> arg0;
            ok &= luaval_to_std_map_string_string(tolua_S, 2, &arg0, "axextSkeletonNode:changeSkins");

            if (!ok) { break; }
            obj->changeSkins(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axextSkeletonNode:changeSkins",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_changeSkins'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkeletonNode_addSkinGroup(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkeletonNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axextSkeletonNode",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::timeline::SkeletonNode*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_SkeletonNode_addSkinGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        tsl::robin_map<std::string, std::string, tlx::string_hash, tlx::equal_to> arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axextSkeletonNode:addSkinGroup");

        ok &= luaval_to_std_map_string_string(tolua_S, 3, &arg1, "axextSkeletonNode:addSkinGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkeletonNode_addSkinGroup'", nullptr);
            return 0;
        }
        obj->addSkinGroup(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkeletonNode:addSkinGroup",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_addSkinGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_SkeletonNode_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axextSkeletonNode",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkeletonNode_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::timeline::SkeletonNode::create();
        object_to_luaval<ax::ext::timeline::SkeletonNode>(tolua_S, "axextSkeletonNode",(ax::ext::timeline::SkeletonNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axextSkeletonNode:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_SkeletonNode_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::timeline::SkeletonNode* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_SkeletonNode_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::timeline::SkeletonNode();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axextSkeletonNode");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axextSkeletonNode:SkeletonNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_SkeletonNode_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_SkeletonNode_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SkeletonNode)");
    return 0;
}

int lua_register_ax_sceneext_SkeletonNode(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axextSkeletonNode");
    tolua_cclass(tolua_S,"SkeletonNode","axextSkeletonNode","axextBoneNode",nullptr);

    tolua_beginmodule(tolua_S,"SkeletonNode");
        tolua_function(tolua_S,"new",lua_ax_sceneext_SkeletonNode_constructor);
        tolua_function(tolua_S,"getBoneNode",lua_ax_sceneext_SkeletonNode_getBoneNode);
        tolua_function(tolua_S,"getAllSubBonesMap",lua_ax_sceneext_SkeletonNode_getAllSubBonesMap);
        tolua_function(tolua_S,"changeSkins",lua_ax_sceneext_SkeletonNode_changeSkins);
        tolua_function(tolua_S,"addSkinGroup",lua_ax_sceneext_SkeletonNode_addSkinGroup);
        tolua_function(tolua_S,"create", lua_ax_sceneext_SkeletonNode_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::timeline::SkeletonNode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axextSkeletonNode";
    g_typeCast[typeName] = "axextSkeletonNode";
    return 1;
}

int lua_ax_sceneext_ComExtensionData_setCustomProperty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComExtensionData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComExtensionData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComExtensionData_setCustomProperty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axext.ComExtensionData:setCustomProperty");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_setCustomProperty'", nullptr);
            return 0;
        }
        obj->setCustomProperty(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComExtensionData:setCustomProperty",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_setCustomProperty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComExtensionData_getCustomProperty(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComExtensionData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComExtensionData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComExtensionData_getCustomProperty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_getCustomProperty'", nullptr);
            return 0;
        }
        auto&& ret = obj->getCustomProperty();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComExtensionData:getCustomProperty",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_getCustomProperty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComExtensionData_setActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComExtensionData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComExtensionData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComExtensionData_setActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axext.ComExtensionData:setActionTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_setActionTag'", nullptr);
            return 0;
        }
        obj->setActionTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComExtensionData:setActionTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_setActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComExtensionData_getActionTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComExtensionData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::ext::ComExtensionData*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_sceneext_ComExtensionData_getActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_getActionTag'", nullptr);
            return 0;
        }
        auto&& ret = obj->getActionTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComExtensionData:getActionTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_getActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_sceneext_ComExtensionData_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_createInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComExtensionData::createInstance();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComExtensionData:createInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComExtensionData_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axext.ComExtensionData",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ext::ComExtensionData::create();
        object_to_luaval<ax::ext::ComExtensionData>(tolua_S, "axext.ComExtensionData",(ax::ext::ComExtensionData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.ComExtensionData:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_sceneext_ComExtensionData_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ext::ComExtensionData* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_sceneext_ComExtensionData_constructor'", nullptr);
            return 0;
        }
        obj = new ax::ext::ComExtensionData();
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axext.ComExtensionData");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axext.ComExtensionData:ComExtensionData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_sceneext_ComExtensionData_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_sceneext_ComExtensionData_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ComExtensionData)");
    return 0;
}

int lua_register_ax_sceneext_ComExtensionData(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axext.ComExtensionData");
    tolua_cclass(tolua_S,"ComExtensionData","axext.ComExtensionData","ax.Component",nullptr);

    tolua_beginmodule(tolua_S,"ComExtensionData");
        tolua_function(tolua_S,"new",lua_ax_sceneext_ComExtensionData_constructor);
        tolua_function(tolua_S,"setCustomProperty",lua_ax_sceneext_ComExtensionData_setCustomProperty);
        tolua_function(tolua_S,"getCustomProperty",lua_ax_sceneext_ComExtensionData_getCustomProperty);
        tolua_function(tolua_S,"setActionTag",lua_ax_sceneext_ComExtensionData_setActionTag);
        tolua_function(tolua_S,"getActionTag",lua_ax_sceneext_ComExtensionData_getActionTag);
        tolua_function(tolua_S,"createInstance", lua_ax_sceneext_ComExtensionData_createInstance);
        tolua_function(tolua_S,"create", lua_ax_sceneext_ComExtensionData_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ext::ComExtensionData).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axext.ComExtensionData";
    g_typeCast[typeName] = "axext.ComExtensionData";
    return 1;
}
TOLUA_API int register_all_ax_sceneext(lua_State* tolua_S)
{
    tolua_open(tolua_S);

    tolua_module(tolua_S,"ax",0);
    tolua_beginmodule(tolua_S,"ax");

    lua_register_ax_sceneext_ActionFrame(tolua_S);
    lua_register_ax_sceneext_ActionMoveFrame(tolua_S);
    lua_register_ax_sceneext_ActionScaleFrame(tolua_S);
    lua_register_ax_sceneext_ActionRotationFrame(tolua_S);
    lua_register_ax_sceneext_ActionFadeFrame(tolua_S);
    lua_register_ax_sceneext_ActionTintFrame(tolua_S);
    lua_register_ax_sceneext_ActionObject(tolua_S);
    lua_register_ax_sceneext_ActionManagerEx(tolua_S);
    lua_register_ax_sceneext_BaseData(tolua_S);
    lua_register_ax_sceneext_DisplayData(tolua_S);
    lua_register_ax_sceneext_SpriteDisplayData(tolua_S);
    lua_register_ax_sceneext_ArmatureDisplayData(tolua_S);
    lua_register_ax_sceneext_ParticleDisplayData(tolua_S);
    lua_register_ax_sceneext_BoneData(tolua_S);
    lua_register_ax_sceneext_ArmatureData(tolua_S);
    lua_register_ax_sceneext_FrameData(tolua_S);
    lua_register_ax_sceneext_MovementBoneData(tolua_S);
    lua_register_ax_sceneext_MovementData(tolua_S);
    lua_register_ax_sceneext_AnimationData(tolua_S);
    lua_register_ax_sceneext_ContourData(tolua_S);
    lua_register_ax_sceneext_TextureData(tolua_S);
    lua_register_ax_sceneext_BoneTweenController(tolua_S);
    lua_register_ax_sceneext_DisplayManager(tolua_S);
    lua_register_ax_sceneext_Bone(tolua_S);
    lua_register_ax_sceneext_BatchNode(tolua_S);
    lua_register_ax_sceneext_ArmatureDataManager(tolua_S);
    lua_register_ax_sceneext_Armature(tolua_S);
    lua_register_ax_sceneext_ArmatureController(tolua_S);
    lua_register_ax_sceneext_Skin(tolua_S);
    lua_register_ax_sceneext_ComAttribute(tolua_S);
    lua_register_ax_sceneext_ComAudio(tolua_S);
    lua_register_ax_sceneext_ComController(tolua_S);
    lua_register_ax_sceneext_ComRender(tolua_S);
    lua_register_ax_sceneext_GUIReader(tolua_S);
    lua_register_ax_sceneext_SceneReader(tolua_S);
    lua_register_ax_sceneext_Frame(tolua_S);
    lua_register_ax_sceneext_VisibleFrame(tolua_S);
    lua_register_ax_sceneext_TextureFrame(tolua_S);
    lua_register_ax_sceneext_RotationFrame(tolua_S);
    lua_register_ax_sceneext_SkewFrame(tolua_S);
    lua_register_ax_sceneext_RotationSkewFrame(tolua_S);
    lua_register_ax_sceneext_PositionFrame(tolua_S);
    lua_register_ax_sceneext_ScaleFrame(tolua_S);
    lua_register_ax_sceneext_AnchorPointFrame(tolua_S);
    lua_register_ax_sceneext_InnerActionFrame(tolua_S);
    lua_register_ax_sceneext_ColorFrame(tolua_S);
    lua_register_ax_sceneext_AlphaFrame(tolua_S);
    lua_register_ax_sceneext_EventFrame(tolua_S);
    lua_register_ax_sceneext_ZOrderFrame(tolua_S);
    lua_register_ax_sceneext_BlendFuncFrame(tolua_S);
    lua_register_ax_sceneext_PlayableFrame(tolua_S);
    lua_register_ax_sceneext_Timeline(tolua_S);
    lua_register_ax_sceneext_ActionTimelineData(tolua_S);
    lua_register_ax_sceneext_ActionTimeline(tolua_S);
    lua_register_ax_sceneext_ActionTimelineNode(tolua_S);
    lua_register_ax_sceneext_BoneNode(tolua_S);
    lua_register_ax_sceneext_SkeletonNode(tolua_S);
    lua_register_ax_sceneext_ComExtensionData(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

