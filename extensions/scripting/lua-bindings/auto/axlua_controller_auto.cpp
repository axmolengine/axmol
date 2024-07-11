#include "lua-bindings/auto/axlua_controller_auto.hpp"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#include "base/GameController.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_controller_Controller_getDeviceName(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_getDeviceName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_getDeviceName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDeviceName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:getDeviceName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_getDeviceName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_getDeviceId(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_getDeviceId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_getDeviceId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDeviceId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:getDeviceId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_getDeviceId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_isConnected(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_isConnected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_isConnected'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isConnected();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:isConnected",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_isConnected'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_receiveExternalKeyEvent(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_receiveExternalKeyEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Controller:receiveExternalKeyEvent");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.Controller:receiveExternalKeyEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_receiveExternalKeyEvent'", nullptr);
            return 0;
        }
        cobj->receiveExternalKeyEvent(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:receiveExternalKeyEvent",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_receiveExternalKeyEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_setTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_setTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Controller:setTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_setTag'", nullptr);
            return 0;
        }
        cobj->setTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:setTag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_setTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_getTag(lua_State* tolua_S)
{
    int argc = 0;
    ax::Controller* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::Controller*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_Controller_getTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_getTag'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Controller:getTag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_getTag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_Controller_getControllerByTag(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Controller:getControllerByTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_getControllerByTag'", nullptr);
            return 0;
        }
        auto&& ret = ax::Controller::getControllerByTag(arg0);
        object_to_luaval<ax::Controller>(tolua_S, "ax.Controller",(ax::Controller*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Controller:getControllerByTag",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_getControllerByTag'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_controller_Controller_getControllerByDeviceId(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.Controller:getControllerByDeviceId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_getControllerByDeviceId'", nullptr);
            return 0;
        }
        auto&& ret = ax::Controller::getControllerByDeviceId(arg0);
        object_to_luaval<ax::Controller>(tolua_S, "ax.Controller",(ax::Controller*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Controller:getControllerByDeviceId",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_getControllerByDeviceId'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_controller_Controller_startDiscoveryController(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_startDiscoveryController'", nullptr);
            return 0;
        }
        ax::Controller::startDiscoveryController();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Controller:startDiscoveryController",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_startDiscoveryController'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_controller_Controller_stopDiscoveryController(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.Controller",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_Controller_stopDiscoveryController'", nullptr);
            return 0;
        }
        ax::Controller::stopDiscoveryController();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Controller:stopDiscoveryController",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_Controller_stopDiscoveryController'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_controller_Controller_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Controller)");
    return 0;
}

int lua_register_ax_controller_Controller(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Controller");
    tolua_cclass(tolua_S,"Controller","ax.Controller","",nullptr);

    tolua_beginmodule(tolua_S,"Controller");
        tolua_function(tolua_S,"getDeviceName",lua_ax_controller_Controller_getDeviceName);
        tolua_function(tolua_S,"getDeviceId",lua_ax_controller_Controller_getDeviceId);
        tolua_function(tolua_S,"isConnected",lua_ax_controller_Controller_isConnected);
        tolua_function(tolua_S,"receiveExternalKeyEvent",lua_ax_controller_Controller_receiveExternalKeyEvent);
        tolua_function(tolua_S,"setTag",lua_ax_controller_Controller_setTag);
        tolua_function(tolua_S,"getTag",lua_ax_controller_Controller_getTag);
        tolua_function(tolua_S,"getControllerByTag", lua_ax_controller_Controller_getControllerByTag);
        tolua_function(tolua_S,"getControllerByDeviceId", lua_ax_controller_Controller_getControllerByDeviceId);
        tolua_function(tolua_S,"startDiscoveryController", lua_ax_controller_Controller_startDiscoveryController);
        tolua_function(tolua_S,"stopDiscoveryController", lua_ax_controller_Controller_stopDiscoveryController);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::Controller).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Controller";
    g_typeCast[typeName] = "ax.Controller";
    return 1;
}

int lua_ax_controller_EventController_getControllerEventType(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_getControllerEventType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_getControllerEventType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getControllerEventType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:getControllerEventType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_getControllerEventType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_getController(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_getController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_getController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getController();
        object_to_luaval<ax::Controller>(tolua_S, "ax.Controller",(ax::Controller*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:getController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_getController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_getKeyCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_getKeyCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_getKeyCode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getKeyCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:getKeyCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_getKeyCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_setKeyCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_setKeyCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.EventController:setKeyCode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_setKeyCode'", nullptr);
            return 0;
        }
        cobj->setKeyCode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:setKeyCode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_setKeyCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_setConnectStatus(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_setConnectStatus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.EventController:setConnectStatus");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_setConnectStatus'", nullptr);
            return 0;
        }
        cobj->setConnectStatus(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:setConnectStatus",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_setConnectStatus'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_isConnected(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::EventController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_controller_EventController_isConnected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventController_isConnected'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isConnected();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventController:isConnected",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_isConnected'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_controller_EventController_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::EventController* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 3) {
            ax::EventController::ControllerEventType arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.EventController:EventController");

            if (!ok) { break; }
            ax::Controller* arg1;
            ok &= luaval_to_object<ax::Controller>(tolua_S, 3, "ax.Controller",&arg1, "ax.EventController:EventController");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ax.EventController:EventController");

            if (!ok) { break; }
            cobj = new ax::EventController(arg0, arg1, arg2);
            cobj->autorelease();
            int ID =  (int)cobj->_ID ;
            int* luaID =  &cobj->_luaID ;
            toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.EventController");
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            ax::EventController::ControllerEventType arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.EventController:EventController");

            if (!ok) { break; }
            ax::Controller* arg1;
            ok &= luaval_to_object<ax::Controller>(tolua_S, 3, "ax.Controller",&arg1, "ax.EventController:EventController");

            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventController:EventController");

            if (!ok) { break; }
            cobj = new ax::EventController(arg0, arg1, arg2);
            cobj->autorelease();
            int ID =  (int)cobj->_ID ;
            int* luaID =  &cobj->_luaID ;
            toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"ax.EventController");
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.EventController:EventController",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventController_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_controller_EventController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EventController)");
    return 0;
}

int lua_register_ax_controller_EventController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventController");
    tolua_cclass(tolua_S,"EventController","ax.EventController","ax.Event",nullptr);

    tolua_beginmodule(tolua_S,"EventController");
        tolua_function(tolua_S,"new",lua_ax_controller_EventController_constructor);
        tolua_function(tolua_S,"getControllerEventType",lua_ax_controller_EventController_getControllerEventType);
        tolua_function(tolua_S,"getController",lua_ax_controller_EventController_getController);
        tolua_function(tolua_S,"getKeyCode",lua_ax_controller_EventController_getKeyCode);
        tolua_function(tolua_S,"setKeyCode",lua_ax_controller_EventController_setKeyCode);
        tolua_function(tolua_S,"setConnectStatus",lua_ax_controller_EventController_setConnectStatus);
        tolua_function(tolua_S,"isConnected",lua_ax_controller_EventController_isConnected);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventController";
    g_typeCast[typeName] = "ax.EventController";
    return 1;
}

int lua_ax_controller_EventListenerController_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.EventListenerController",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_controller_EventListenerController_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::EventListenerController::create();
        object_to_luaval<ax::EventListenerController>(tolua_S, "ax.EventListenerController",(ax::EventListenerController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.EventListenerController:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_controller_EventListenerController_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_controller_EventListenerController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EventListenerController)");
    return 0;
}

int lua_register_ax_controller_EventListenerController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerController");
    tolua_cclass(tolua_S,"EventListenerController","ax.EventListenerController","ax.EventListener",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerController");
        tolua_function(tolua_S,"create", lua_ax_controller_EventListenerController_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::EventListenerController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerController";
    g_typeCast[typeName] = "ax.EventListenerController";
    return 1;
}
TOLUA_API int register_all_ax_controller(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_controller_Controller(tolua_S);
	lua_register_ax_controller_EventController(tolua_S);
	lua_register_ax_controller_EventListenerController(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

#endif
