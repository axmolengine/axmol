#include "lua-bindings/auto/axlua_fairygui_auto.hpp"
#include "fairygui/FairyGUI.h"
#include "fairygui/FairyGUIMacros.h"
#include "fairygui/utils/html/HtmlObject.h"
#include "lua-bindings/manual/fairygui/axlua_fairygui_conversions.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_fairygui_InputEvent_getTarget(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTarget();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getTouch(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getTouch'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getTouch'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTouch();
        object_to_luaval<ax::Touch>(tolua_S, "ax.Touch",(ax::Touch*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getTouch",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getTouch'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getTouchId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getTouchId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getTouchId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTouchId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getTouchId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getTouchId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_isDoubleClick(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_isDoubleClick'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_isDoubleClick'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDoubleClick();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:isDoubleClick",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_isDoubleClick'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getButton(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getButton'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getButton'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getButton();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getButton",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getButton'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getKeyCode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getKeyCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getKeyCode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getKeyCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getKeyCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getKeyCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_isCtrlDown(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_isCtrlDown'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_isCtrlDown'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isCtrlDown();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:isCtrlDown",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_isCtrlDown'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_isAltDown(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_isAltDown'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_isAltDown'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isAltDown();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:isAltDown",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_isAltDown'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_isShiftDown(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_isShiftDown'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_isShiftDown'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isShiftDown();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:isShiftDown",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_isShiftDown'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getMouseWheelDelta(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getMouseWheelDelta'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getMouseWheelDelta'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMouseWheelDelta();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getMouseWheelDelta",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getMouseWheelDelta'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_getProcessor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.InputEvent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::InputEvent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_InputEvent_getProcessor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_getProcessor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getProcessor();
        object_to_luaval<fairygui::InputProcessor>(tolua_S, "fgui.InputProcessor",(fairygui::InputProcessor*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:getProcessor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_getProcessor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_InputEvent_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::InputEvent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_InputEvent_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::InputEvent();
        tolua_pushusertype(tolua_S,(void*)cobj,"fgui.InputEvent");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.InputEvent:InputEvent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_InputEvent_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_InputEvent_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (InputEvent)");
    return 0;
}

int lua_register_ax_fairygui_InputEvent(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.InputEvent");
    tolua_cclass(tolua_S,"InputEvent","fgui.InputEvent","",nullptr);

    tolua_beginmodule(tolua_S,"InputEvent");
        tolua_function(tolua_S,"new",lua_ax_fairygui_InputEvent_constructor);
        tolua_function(tolua_S,"getTarget",lua_ax_fairygui_InputEvent_getTarget);
        tolua_function(tolua_S,"getX",lua_ax_fairygui_InputEvent_getX);
        tolua_function(tolua_S,"getY",lua_ax_fairygui_InputEvent_getY);
        tolua_function(tolua_S,"getPosition",lua_ax_fairygui_InputEvent_getPosition);
        tolua_function(tolua_S,"getTouch",lua_ax_fairygui_InputEvent_getTouch);
        tolua_function(tolua_S,"getTouchId",lua_ax_fairygui_InputEvent_getTouchId);
        tolua_function(tolua_S,"isDoubleClick",lua_ax_fairygui_InputEvent_isDoubleClick);
        tolua_function(tolua_S,"getButton",lua_ax_fairygui_InputEvent_getButton);
        tolua_function(tolua_S,"getKeyCode",lua_ax_fairygui_InputEvent_getKeyCode);
        tolua_function(tolua_S,"isCtrlDown",lua_ax_fairygui_InputEvent_isCtrlDown);
        tolua_function(tolua_S,"isAltDown",lua_ax_fairygui_InputEvent_isAltDown);
        tolua_function(tolua_S,"isShiftDown",lua_ax_fairygui_InputEvent_isShiftDown);
        tolua_function(tolua_S,"getMouseWheelDelta",lua_ax_fairygui_InputEvent_getMouseWheelDelta);
        tolua_function(tolua_S,"getProcessor",lua_ax_fairygui_InputEvent_getProcessor);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::InputEvent).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.InputEvent";
    g_typeCast[typeName] = "fgui.InputEvent";
    return 1;
}

int lua_ax_fairygui_EventContext_getType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_getType'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_getType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_getSender(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_getSender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_getSender'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSender();
        object_to_luaval<ax::Object>(tolua_S, "ax.Object",(ax::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getSender",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_getSender'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_getInput(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_getInput'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_getInput'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getInput();
        object_to_luaval<fairygui::InputEvent>(tolua_S, "fgui.InputEvent",(fairygui::InputEvent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getInput",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_getInput'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_stopPropagation(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_stopPropagation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_stopPropagation'", nullptr);
            return 0;
        }
        cobj->stopPropagation();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:stopPropagation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_stopPropagation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_preventDefault(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_preventDefault'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_preventDefault'", nullptr);
            return 0;
        }
        cobj->preventDefault();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:preventDefault",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_preventDefault'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_isDefaultPrevented(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_isDefaultPrevented'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_isDefaultPrevented'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDefaultPrevented();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:isDefaultPrevented",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_isDefaultPrevented'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_captureTouch(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_captureTouch'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_captureTouch'", nullptr);
            return 0;
        }
        cobj->captureTouch();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:captureTouch",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_captureTouch'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_uncaptureTouch(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.EventContext",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_EventContext_uncaptureTouch'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_uncaptureTouch'", nullptr);
            return 0;
        }
        cobj->uncaptureTouch();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:uncaptureTouch",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_uncaptureTouch'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_EventContext_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_EventContext_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::EventContext();
        tolua_pushusertype(tolua_S,(void*)cobj,"fgui.EventContext");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:EventContext",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_EventContext_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_EventContext_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (EventContext)");
    return 0;
}

int lua_register_ax_fairygui_EventContext(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.EventContext");
    tolua_cclass(tolua_S,"EventContext","fgui.EventContext","",nullptr);

    tolua_beginmodule(tolua_S,"EventContext");
        tolua_function(tolua_S,"new",lua_ax_fairygui_EventContext_constructor);
        tolua_function(tolua_S,"getType",lua_ax_fairygui_EventContext_getType);
        tolua_function(tolua_S,"getSender",lua_ax_fairygui_EventContext_getSender);
        tolua_function(tolua_S,"getInput",lua_ax_fairygui_EventContext_getInput);
        tolua_function(tolua_S,"stopPropagation",lua_ax_fairygui_EventContext_stopPropagation);
        tolua_function(tolua_S,"preventDefault",lua_ax_fairygui_EventContext_preventDefault);
        tolua_function(tolua_S,"isDefaultPrevented",lua_ax_fairygui_EventContext_isDefaultPrevented);
        tolua_function(tolua_S,"captureTouch",lua_ax_fairygui_EventContext_captureTouch);
        tolua_function(tolua_S,"uncaptureTouch",lua_ax_fairygui_EventContext_uncaptureTouch);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::EventContext).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.EventContext";
    g_typeCast[typeName] = "fgui.EventContext";
    return 1;
}

int lua_ax_fairygui_UIEventDispatcher_removeEventListeners(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIEventDispatcher* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIEventDispatcher",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIEventDispatcher*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIEventDispatcher_removeEventListeners'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_removeEventListeners'", nullptr);
            return 0;
        }
        cobj->removeEventListeners();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIEventDispatcher:removeEventListeners",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIEventDispatcher_removeEventListeners'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIEventDispatcher_dispatchEvent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIEventDispatcher* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIEventDispatcher",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIEventDispatcher*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIEventDispatcher_dispatchEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:dispatchEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_dispatchEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->dispatchEvent(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        void* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:dispatchEvent");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_dispatchEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->dispatchEvent(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 3) 
    {
        int arg0;
        void* arg1;
        ax::Value arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:dispatchEvent");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;

        ok &= luaval_to_ccvalue(tolua_S, 4, &arg2, "fgui.UIEventDispatcher:dispatchEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_dispatchEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->dispatchEvent(arg0, arg1, arg2);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIEventDispatcher:dispatchEvent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIEventDispatcher_dispatchEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIEventDispatcher_bubbleEvent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIEventDispatcher* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIEventDispatcher",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIEventDispatcher*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIEventDispatcher_bubbleEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:bubbleEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_bubbleEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->bubbleEvent(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        void* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:bubbleEvent");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_bubbleEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->bubbleEvent(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 3) 
    {
        int arg0;
        void* arg1;
        ax::Value arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:bubbleEvent");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;

        ok &= luaval_to_ccvalue(tolua_S, 4, &arg2, "fgui.UIEventDispatcher:bubbleEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_bubbleEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->bubbleEvent(arg0, arg1, arg2);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIEventDispatcher:bubbleEvent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIEventDispatcher_bubbleEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIEventDispatcher_isDispatchingEvent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIEventDispatcher* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIEventDispatcher",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIEventDispatcher*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIEventDispatcher_isDispatchingEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIEventDispatcher:isDispatchingEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_isDispatchingEvent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDispatchingEvent(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIEventDispatcher:isDispatchingEvent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIEventDispatcher_isDispatchingEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIEventDispatcher_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIEventDispatcher* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIEventDispatcher_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::UIEventDispatcher();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.UIEventDispatcher");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIEventDispatcher:UIEventDispatcher",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIEventDispatcher_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_UIEventDispatcher_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (UIEventDispatcher)");
    return 0;
}

int lua_register_ax_fairygui_UIEventDispatcher(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.UIEventDispatcher");
    tolua_cclass(tolua_S,"UIEventDispatcher","fgui.UIEventDispatcher","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"UIEventDispatcher");
        tolua_function(tolua_S,"new",lua_ax_fairygui_UIEventDispatcher_constructor);
        tolua_function(tolua_S,"removeEventListeners",lua_ax_fairygui_UIEventDispatcher_removeEventListeners);
        tolua_function(tolua_S,"dispatchEvent",lua_ax_fairygui_UIEventDispatcher_dispatchEvent);
        tolua_function(tolua_S,"bubbleEvent",lua_ax_fairygui_UIEventDispatcher_bubbleEvent);
        tolua_function(tolua_S,"isDispatchingEvent",lua_ax_fairygui_UIEventDispatcher_isDispatchingEvent);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::UIEventDispatcher).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.UIEventDispatcher";
    g_typeCast[typeName] = "fgui.UIEventDispatcher";
    return 1;
}

int lua_ax_fairygui_GController_getParent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getParent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getParent();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getParent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_setParent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_setParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GComponent* arg0;

        ok &= luaval_to_object<fairygui::GComponent>(tolua_S, 2, "fgui.GComponent",&arg0, "fgui.GController:setParent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setParent'", nullptr);
            return 0;
        }
        cobj->setParent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:setParent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_setParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getSelectedIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getSelectedIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_setSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_setSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GController:setSelectedIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedIndex'", nullptr);
            return 0;
        }
        cobj->setSelectedIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GController:setSelectedIndex");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GController:setSelectedIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedIndex'", nullptr);
            return 0;
        }
        cobj->setSelectedIndex(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:setSelectedIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_setSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getSelectedPage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getSelectedPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getSelectedPage'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedPage();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getSelectedPage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getSelectedPage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_setSelectedPage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_setSelectedPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:setSelectedPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedPage'", nullptr);
            return 0;
        }
        cobj->setSelectedPage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:setSelectedPage");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GController:setSelectedPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedPage'", nullptr);
            return 0;
        }
        cobj->setSelectedPage(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:setSelectedPage",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_setSelectedPage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getSelectedPageId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getSelectedPageId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getSelectedPageId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedPageId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getSelectedPageId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getSelectedPageId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_setSelectedPageId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_setSelectedPageId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:setSelectedPageId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedPageId'", nullptr);
            return 0;
        }
        cobj->setSelectedPageId(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:setSelectedPageId");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GController:setSelectedPageId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setSelectedPageId'", nullptr);
            return 0;
        }
        cobj->setSelectedPageId(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:setSelectedPageId",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_setSelectedPageId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPrevisousIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPrevisousIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPrevisousIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPrevisousIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPrevisousIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPrevisousIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPreviousPage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPreviousPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPreviousPage'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPreviousPage();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPreviousPage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPreviousPage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPreviousPageId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPreviousPageId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPreviousPageId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPreviousPageId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPreviousPageId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPreviousPageId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPageCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPageCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPageCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPageCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPageCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_hasPage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_hasPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:hasPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_hasPage'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hasPage(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:hasPage",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_hasPage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPageIndexById(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPageIndexById'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:getPageIndexById");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPageIndexById'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageIndexById(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPageIndexById",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPageIndexById'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPageNameById(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPageNameById'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:getPageNameById");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPageNameById'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageNameById(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPageNameById",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPageNameById'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_getPageId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_getPageId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GController:getPageId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_getPageId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageId(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:getPageId",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_getPageId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_setOppositePageId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_setOppositePageId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GController:setOppositePageId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_setOppositePageId'", nullptr);
            return 0;
        }
        cobj->setOppositePageId(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:setOppositePageId",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_setOppositePageId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_runActions(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GController",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GController*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GController_runActions'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_runActions'", nullptr);
            return 0;
        }
        cobj->runActions();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:runActions",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_runActions'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GController_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GController* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GController_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GController();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GController");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GController:GController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GController_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GController_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GController)");
    return 0;
}

int lua_register_ax_fairygui_GController(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GController");
    tolua_cclass(tolua_S,"GController","fgui.GController","fgui.UIEventDispatcher",nullptr);

    tolua_beginmodule(tolua_S,"GController");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GController_constructor);
        tolua_function(tolua_S,"getParent",lua_ax_fairygui_GController_getParent);
        tolua_function(tolua_S,"setParent",lua_ax_fairygui_GController_setParent);
        tolua_function(tolua_S,"getSelectedIndex",lua_ax_fairygui_GController_getSelectedIndex);
        tolua_function(tolua_S,"setSelectedIndex",lua_ax_fairygui_GController_setSelectedIndex);
        tolua_function(tolua_S,"getSelectedPage",lua_ax_fairygui_GController_getSelectedPage);
        tolua_function(tolua_S,"setSelectedPage",lua_ax_fairygui_GController_setSelectedPage);
        tolua_function(tolua_S,"getSelectedPageId",lua_ax_fairygui_GController_getSelectedPageId);
        tolua_function(tolua_S,"setSelectedPageId",lua_ax_fairygui_GController_setSelectedPageId);
        tolua_function(tolua_S,"getPrevisousIndex",lua_ax_fairygui_GController_getPrevisousIndex);
        tolua_function(tolua_S,"getPreviousPage",lua_ax_fairygui_GController_getPreviousPage);
        tolua_function(tolua_S,"getPreviousPageId",lua_ax_fairygui_GController_getPreviousPageId);
        tolua_function(tolua_S,"getPageCount",lua_ax_fairygui_GController_getPageCount);
        tolua_function(tolua_S,"hasPage",lua_ax_fairygui_GController_hasPage);
        tolua_function(tolua_S,"getPageIndexById",lua_ax_fairygui_GController_getPageIndexById);
        tolua_function(tolua_S,"getPageNameById",lua_ax_fairygui_GController_getPageNameById);
        tolua_function(tolua_S,"getPageId",lua_ax_fairygui_GController_getPageId);
        tolua_function(tolua_S,"setOppositePageId",lua_ax_fairygui_GController_setOppositePageId);
        tolua_function(tolua_S,"runActions",lua_ax_fairygui_GController_runActions);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GController).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GController";
    g_typeCast[typeName] = "fgui.GController";
    return 1;
}

int lua_ax_fairygui_GObject_getX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setX'", nullptr);
            return 0;
        }
        cobj->setX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setY'", nullptr);
            return 0;
        }
        cobj->setY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setPosition");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setPosition'", nullptr);
            return 0;
        }
        cobj->setPosition(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setPosition",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getXMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getXMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getXMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getXMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getXMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getXMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setXMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setXMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setXMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setXMin'", nullptr);
            return 0;
        }
        cobj->setXMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setXMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setXMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getYMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getYMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getYMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getYMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getYMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getYMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setYMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setYMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setYMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setYMin'", nullptr);
            return 0;
        }
        cobj->setYMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setYMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setYMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isPixelSnapping(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isPixelSnapping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isPixelSnapping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPixelSnapping();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isPixelSnapping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isPixelSnapping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setPixelSnapping(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setPixelSnapping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:setPixelSnapping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setPixelSnapping'", nullptr);
            return 0;
        }
        cobj->setPixelSnapping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setPixelSnapping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setPixelSnapping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getWidth(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getWidth'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setWidth(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setWidth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setWidth'", nullptr);
            return 0;
        }
        cobj->setWidth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setWidth",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getHeight(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getHeight'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setHeight(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setHeight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setHeight'", nullptr);
            return 0;
        }
        cobj->setHeight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setHeight",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setSize");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setSize'", nullptr);
            return 0;
        }
        cobj->setSize(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        bool arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setSize");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setSize");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.GObject:setSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setSize'", nullptr);
            return 0;
        }
        cobj->setSize(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setSize",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_center(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_center'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_center'", nullptr);
            return 0;
        }
        cobj->center();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:center");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_center'", nullptr);
            return 0;
        }
        cobj->center(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:center",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_center'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_makeFullScreen(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_makeFullScreen'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_makeFullScreen'", nullptr);
            return 0;
        }
        cobj->makeFullScreen();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:makeFullScreen",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_makeFullScreen'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getPivot(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getPivot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getPivot'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPivot();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getPivot",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getPivot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setPivot(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setPivot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setPivot");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setPivot");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setPivot'", nullptr);
            return 0;
        }
        cobj->setPivot(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        bool arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setPivot");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setPivot");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.GObject:setPivot");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setPivot'", nullptr);
            return 0;
        }
        cobj->setPivot(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setPivot",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setPivot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isPivotAsAnchor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isPivotAsAnchor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isPivotAsAnchor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPivotAsAnchor();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isPivotAsAnchor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isPivotAsAnchor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getScaleX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getScaleX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScaleX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getScaleX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setScaleX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setScaleX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setScaleX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setScaleX'", nullptr);
            return 0;
        }
        cobj->setScaleX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setScaleX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setScaleX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getScaleY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getScaleY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScaleY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getScaleY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setScaleY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setScaleY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setScaleY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setScaleY'", nullptr);
            return 0;
        }
        cobj->setScaleY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setScaleY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setScaleY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScale();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setScale");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GObject:setScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setScale'", nullptr);
            return 0;
        }
        cobj->setScale(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setScale",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getSkewX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getSkewX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getSkewX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSkewX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getSkewX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getSkewX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setSkewX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setSkewX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setSkewX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setSkewX'", nullptr);
            return 0;
        }
        cobj->setSkewX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setSkewX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setSkewX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getSkewY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getSkewY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getSkewY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSkewY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getSkewY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getSkewY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setSkewY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setSkewY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setSkewY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setSkewY'", nullptr);
            return 0;
        }
        cobj->setSkewY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setSkewY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setSkewY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getRotation(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getRotation'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRotation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getRotation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setRotation(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setRotation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setRotation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setRotation'", nullptr);
            return 0;
        }
        cobj->setRotation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setRotation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setRotation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getAlpha(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getAlpha'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAlpha();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getAlpha",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setAlpha(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GObject:setAlpha");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setAlpha'", nullptr);
            return 0;
        }
        cobj->setAlpha(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setAlpha",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isGrayed(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isGrayed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isGrayed'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isGrayed();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isGrayed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isGrayed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setGrayed(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setGrayed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:setGrayed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setGrayed'", nullptr);
            return 0;
        }
        cobj->setGrayed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setGrayed",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setGrayed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isVisible(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isVisible'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isVisible();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isVisible",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setVisible(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:setVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setVisible'", nullptr);
            return 0;
        }
        cobj->setVisible(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setVisible",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isTouchable(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isTouchable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isTouchable'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isTouchable();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isTouchable",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isTouchable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setTouchable(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setTouchable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:setTouchable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setTouchable'", nullptr);
            return 0;
        }
        cobj->setTouchable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setTouchable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setTouchable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getSortingOrder(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getSortingOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getSortingOrder'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSortingOrder();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getSortingOrder",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getSortingOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setSortingOrder(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setSortingOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:setSortingOrder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setSortingOrder'", nullptr);
            return 0;
        }
        cobj->setSortingOrder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setSortingOrder",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setSortingOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getGroup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getGroup'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGroup();
        object_to_luaval<fairygui::GGroup>(tolua_S, "fgui.GGroup",(fairygui::GGroup*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setGroup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GGroup* arg0;

        ok &= luaval_to_object<fairygui::GGroup>(tolua_S, 2, "fgui.GGroup",&arg0, "fgui.GObject:setGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setGroup'", nullptr);
            return 0;
        }
        cobj->setGroup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setGroup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getText(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getText'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getText();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getText",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getText'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setText(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GObject:setText");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setText'", nullptr);
            return 0;
        }
        cobj->setText(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setText",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setText'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getIcon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getIcon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getIcon'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getIcon();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getIcon",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getIcon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setIcon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setIcon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GObject:setIcon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setIcon'", nullptr);
            return 0;
        }
        cobj->setIcon(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setIcon",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setIcon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getTooltips(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getTooltips'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getTooltips'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTooltips();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getTooltips",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getTooltips'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setTooltips(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setTooltips'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GObject:setTooltips");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setTooltips'", nullptr);
            return 0;
        }
        cobj->setTooltips(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setTooltips",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setTooltips'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setData'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setData'", nullptr);
            return 0;
        }
        cobj->setData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getCustomData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getCustomData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getCustomData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCustomData();
        ccvalue_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getCustomData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getCustomData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setCustomData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setCustomData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Value arg0;

        ok &= luaval_to_ccvalue(tolua_S, 2, &arg0, "fgui.GObject:setCustomData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setCustomData'", nullptr);
            return 0;
        }
        cobj->setCustomData(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setCustomData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setCustomData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_isDraggable(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_isDraggable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_isDraggable'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDraggable();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:isDraggable",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_isDraggable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setDraggable(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setDraggable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GObject:setDraggable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setDraggable'", nullptr);
            return 0;
        }
        cobj->setDraggable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setDraggable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setDraggable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getDragBounds(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getDragBounds'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getDragBounds'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDragBounds();
        object_to_luaval<ax::Rect>(tolua_S, "ax.Rect",(ax::Rect*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getDragBounds",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getDragBounds'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setDragBounds(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setDragBounds'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.GObject:setDragBounds");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setDragBounds'", nullptr);
            return 0;
        }
        cobj->setDragBounds(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setDragBounds",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setDragBounds'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_startDrag(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_startDrag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_startDrag'", nullptr);
            return 0;
        }
        cobj->startDrag();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:startDrag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_startDrag'", nullptr);
            return 0;
        }
        cobj->startDrag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:startDrag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_startDrag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_stopDrag(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_stopDrag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_stopDrag'", nullptr);
            return 0;
        }
        cobj->stopDrag();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:stopDrag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_stopDrag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getResourceURL(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getResourceURL'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getResourceURL'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getResourceURL();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getResourceURL",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getResourceURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getPackageItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getPackageItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getPackageItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPackageItem();
        object_to_luaval<fairygui::PackageItem>(tolua_S, "fgui.PackageItem",(fairygui::PackageItem*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getPackageItem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getPackageItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_globalToLocal(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_globalToLocal'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.GObject:globalToLocal");

            if (!ok) { break; }
            ax::Rect ret = cobj->globalToLocal(arg0);
            rect_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::Vec2 arg0;
            ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GObject:globalToLocal");

            if (!ok) { break; }
            ax::Vec2 ret = cobj->globalToLocal(arg0);
            vec2_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GObject:globalToLocal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_globalToLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_localToGlobal(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_localToGlobal'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.GObject:localToGlobal");

            if (!ok) { break; }
            ax::Rect ret = cobj->localToGlobal(arg0);
            rect_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::Vec2 arg0;
            ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GObject:localToGlobal");

            if (!ok) { break; }
            ax::Vec2 ret = cobj->localToGlobal(arg0);
            vec2_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GObject:localToGlobal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_localToGlobal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_transformRect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_transformRect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Rect arg0;
        fairygui::GObject* arg1;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.GObject:transformRect");

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.GObject:transformRect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_transformRect'", nullptr);
            return 0;
        }
        auto&& ret = cobj->transformRect(arg0, arg1);
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:transformRect",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_transformRect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_relations(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_relations'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_relations'", nullptr);
            return 0;
        }
        auto&& ret = cobj->relations();
        object_to_luaval<fairygui::Relations>(tolua_S, "fgui.Relations",(fairygui::Relations*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:relations",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_relations'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_addRelation(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_addRelation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        fairygui::RelationType arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GObject:addRelation");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GObject:addRelation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_addRelation'", nullptr);
            return 0;
        }
        cobj->addRelation(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        fairygui::GObject* arg0;
        fairygui::RelationType arg1;
        bool arg2;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GObject:addRelation");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GObject:addRelation");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.GObject:addRelation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_addRelation'", nullptr);
            return 0;
        }
        cobj->addRelation(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:addRelation",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_addRelation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_removeRelation(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_removeRelation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        fairygui::RelationType arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GObject:removeRelation");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GObject:removeRelation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_removeRelation'", nullptr);
            return 0;
        }
        cobj->removeRelation(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:removeRelation",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_removeRelation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getGear(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getGear'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:getGear");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getGear'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getGear(arg0);
        object_to_luaval<fairygui::GearBase>(tolua_S, "fgui.GearBase",(fairygui::GearBase*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getGear",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getGear'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_checkGearController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_checkGearController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        int arg0;
        fairygui::GController* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:checkGearController");

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 3, "fgui.GController",&arg1, "fgui.GObject:checkGearController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_checkGearController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->checkGearController(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:checkGearController",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_checkGearController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_addDisplayLock(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_addDisplayLock'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_addDisplayLock'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addDisplayLock();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:addDisplayLock",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_addDisplayLock'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_releaseDisplayLock(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_releaseDisplayLock'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "fgui.GObject:releaseDisplayLock");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_releaseDisplayLock'", nullptr);
            return 0;
        }
        cobj->releaseDisplayLock(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:releaseDisplayLock",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_releaseDisplayLock'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getParent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getParent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getParent();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getParent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_findParent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_findParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_findParent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->findParent();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:findParent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_findParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_displayObject(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_displayObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_displayObject'", nullptr);
            return 0;
        }
        auto&& ret = cobj->displayObject();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:displayObject",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_displayObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getRoot(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getRoot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getRoot'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRoot();
        object_to_luaval<fairygui::GRoot>(tolua_S, "fgui.GRoot",(fairygui::GRoot*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getRoot",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getRoot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_onStage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_onStage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_onStage'", nullptr);
            return 0;
        }
        auto&& ret = cobj->onStage();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:onStage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_onStage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_removeFromParent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_removeFromParent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_removeFromParent'", nullptr);
            return 0;
        }
        cobj->removeFromParent();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:removeFromParent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_removeFromParent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getProp(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_getProp'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ObjectPropID arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:getProp");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getProp'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getProp(arg0);
        ccvalue_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getProp",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getProp'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_setProp(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_setProp'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::ObjectPropID arg0;
        ax::Value arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GObject:setProp");

        ok &= luaval_to_ccvalue(tolua_S, 3, &arg1, "fgui.GObject:setProp");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_setProp'", nullptr);
            return 0;
        }
        cobj->setProp(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:setProp",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_setProp'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_constructFromResource(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_constructFromResource'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_constructFromResource'", nullptr);
            return 0;
        }
        cobj->constructFromResource();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:constructFromResource",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_constructFromResource'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Vec2 arg0;
        const ax::Camera* arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GObject:hitTest");

        ok &= luaval_to_object<const ax::Camera>(tolua_S, 3, "ax.Camera",&arg1, "fgui.GObject:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_hitTest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hitTest(arg0, arg1);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:hitTest",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_treeNode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObject_treeNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_treeNode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->treeNode();
        object_to_luaval<fairygui::GTreeNode>(tolua_S, "fgui.GTreeNode",(fairygui::GTreeNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:treeNode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_treeNode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObject_getDraggingObject(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_getDraggingObject'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GObject::getDraggingObject();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GObject:getDraggingObject",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_getDraggingObject'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GObject_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GObject",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GObject::create();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GObject:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GObject_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObject_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GObject();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GObject");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:GObject",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObject_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GObject_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GObject)");
    return 0;
}

int lua_register_ax_fairygui_GObject(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GObject");
    tolua_cclass(tolua_S,"GObject","fgui.GObject","fgui.UIEventDispatcher",nullptr);

    tolua_beginmodule(tolua_S,"GObject");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GObject_constructor);
        tolua_function(tolua_S,"getX",lua_ax_fairygui_GObject_getX);
        tolua_function(tolua_S,"setX",lua_ax_fairygui_GObject_setX);
        tolua_function(tolua_S,"getY",lua_ax_fairygui_GObject_getY);
        tolua_function(tolua_S,"setY",lua_ax_fairygui_GObject_setY);
        tolua_function(tolua_S,"getPosition",lua_ax_fairygui_GObject_getPosition);
        tolua_function(tolua_S,"setPosition",lua_ax_fairygui_GObject_setPosition);
        tolua_function(tolua_S,"getXMin",lua_ax_fairygui_GObject_getXMin);
        tolua_function(tolua_S,"setXMin",lua_ax_fairygui_GObject_setXMin);
        tolua_function(tolua_S,"getYMin",lua_ax_fairygui_GObject_getYMin);
        tolua_function(tolua_S,"setYMin",lua_ax_fairygui_GObject_setYMin);
        tolua_function(tolua_S,"isPixelSnapping",lua_ax_fairygui_GObject_isPixelSnapping);
        tolua_function(tolua_S,"setPixelSnapping",lua_ax_fairygui_GObject_setPixelSnapping);
        tolua_function(tolua_S,"getWidth",lua_ax_fairygui_GObject_getWidth);
        tolua_function(tolua_S,"setWidth",lua_ax_fairygui_GObject_setWidth);
        tolua_function(tolua_S,"getHeight",lua_ax_fairygui_GObject_getHeight);
        tolua_function(tolua_S,"setHeight",lua_ax_fairygui_GObject_setHeight);
        tolua_function(tolua_S,"getSize",lua_ax_fairygui_GObject_getSize);
        tolua_function(tolua_S,"setSize",lua_ax_fairygui_GObject_setSize);
        tolua_function(tolua_S,"center",lua_ax_fairygui_GObject_center);
        tolua_function(tolua_S,"makeFullScreen",lua_ax_fairygui_GObject_makeFullScreen);
        tolua_function(tolua_S,"getPivot",lua_ax_fairygui_GObject_getPivot);
        tolua_function(tolua_S,"setPivot",lua_ax_fairygui_GObject_setPivot);
        tolua_function(tolua_S,"isPivotAsAnchor",lua_ax_fairygui_GObject_isPivotAsAnchor);
        tolua_function(tolua_S,"getScaleX",lua_ax_fairygui_GObject_getScaleX);
        tolua_function(tolua_S,"setScaleX",lua_ax_fairygui_GObject_setScaleX);
        tolua_function(tolua_S,"getScaleY",lua_ax_fairygui_GObject_getScaleY);
        tolua_function(tolua_S,"setScaleY",lua_ax_fairygui_GObject_setScaleY);
        tolua_function(tolua_S,"getScale",lua_ax_fairygui_GObject_getScale);
        tolua_function(tolua_S,"setScale",lua_ax_fairygui_GObject_setScale);
        tolua_function(tolua_S,"getSkewX",lua_ax_fairygui_GObject_getSkewX);
        tolua_function(tolua_S,"setSkewX",lua_ax_fairygui_GObject_setSkewX);
        tolua_function(tolua_S,"getSkewY",lua_ax_fairygui_GObject_getSkewY);
        tolua_function(tolua_S,"setSkewY",lua_ax_fairygui_GObject_setSkewY);
        tolua_function(tolua_S,"getRotation",lua_ax_fairygui_GObject_getRotation);
        tolua_function(tolua_S,"setRotation",lua_ax_fairygui_GObject_setRotation);
        tolua_function(tolua_S,"getAlpha",lua_ax_fairygui_GObject_getAlpha);
        tolua_function(tolua_S,"setAlpha",lua_ax_fairygui_GObject_setAlpha);
        tolua_function(tolua_S,"isGrayed",lua_ax_fairygui_GObject_isGrayed);
        tolua_function(tolua_S,"setGrayed",lua_ax_fairygui_GObject_setGrayed);
        tolua_function(tolua_S,"isVisible",lua_ax_fairygui_GObject_isVisible);
        tolua_function(tolua_S,"setVisible",lua_ax_fairygui_GObject_setVisible);
        tolua_function(tolua_S,"isTouchable",lua_ax_fairygui_GObject_isTouchable);
        tolua_function(tolua_S,"setTouchable",lua_ax_fairygui_GObject_setTouchable);
        tolua_function(tolua_S,"getSortingOrder",lua_ax_fairygui_GObject_getSortingOrder);
        tolua_function(tolua_S,"setSortingOrder",lua_ax_fairygui_GObject_setSortingOrder);
        tolua_function(tolua_S,"getGroup",lua_ax_fairygui_GObject_getGroup);
        tolua_function(tolua_S,"setGroup",lua_ax_fairygui_GObject_setGroup);
        tolua_function(tolua_S,"getText",lua_ax_fairygui_GObject_getText);
        tolua_function(tolua_S,"setText",lua_ax_fairygui_GObject_setText);
        tolua_function(tolua_S,"getIcon",lua_ax_fairygui_GObject_getIcon);
        tolua_function(tolua_S,"setIcon",lua_ax_fairygui_GObject_setIcon);
        tolua_function(tolua_S,"getTooltips",lua_ax_fairygui_GObject_getTooltips);
        tolua_function(tolua_S,"setTooltips",lua_ax_fairygui_GObject_setTooltips);
        tolua_function(tolua_S,"setData",lua_ax_fairygui_GObject_setData);
        tolua_function(tolua_S,"getCustomData",lua_ax_fairygui_GObject_getCustomData);
        tolua_function(tolua_S,"setCustomData",lua_ax_fairygui_GObject_setCustomData);
        tolua_function(tolua_S,"isDraggable",lua_ax_fairygui_GObject_isDraggable);
        tolua_function(tolua_S,"setDraggable",lua_ax_fairygui_GObject_setDraggable);
        tolua_function(tolua_S,"getDragBounds",lua_ax_fairygui_GObject_getDragBounds);
        tolua_function(tolua_S,"setDragBounds",lua_ax_fairygui_GObject_setDragBounds);
        tolua_function(tolua_S,"startDrag",lua_ax_fairygui_GObject_startDrag);
        tolua_function(tolua_S,"stopDrag",lua_ax_fairygui_GObject_stopDrag);
        tolua_function(tolua_S,"getResourceURL",lua_ax_fairygui_GObject_getResourceURL);
        tolua_function(tolua_S,"getPackageItem",lua_ax_fairygui_GObject_getPackageItem);
        tolua_function(tolua_S,"globalToLocal",lua_ax_fairygui_GObject_globalToLocal);
        tolua_function(tolua_S,"localToGlobal",lua_ax_fairygui_GObject_localToGlobal);
        tolua_function(tolua_S,"transformRect",lua_ax_fairygui_GObject_transformRect);
        tolua_function(tolua_S,"relations",lua_ax_fairygui_GObject_relations);
        tolua_function(tolua_S,"addRelation",lua_ax_fairygui_GObject_addRelation);
        tolua_function(tolua_S,"removeRelation",lua_ax_fairygui_GObject_removeRelation);
        tolua_function(tolua_S,"getGear",lua_ax_fairygui_GObject_getGear);
        tolua_function(tolua_S,"checkGearController",lua_ax_fairygui_GObject_checkGearController);
        tolua_function(tolua_S,"addDisplayLock",lua_ax_fairygui_GObject_addDisplayLock);
        tolua_function(tolua_S,"releaseDisplayLock",lua_ax_fairygui_GObject_releaseDisplayLock);
        tolua_function(tolua_S,"getParent",lua_ax_fairygui_GObject_getParent);
        tolua_function(tolua_S,"findParent",lua_ax_fairygui_GObject_findParent);
        tolua_function(tolua_S,"displayObject",lua_ax_fairygui_GObject_displayObject);
        tolua_function(tolua_S,"getRoot",lua_ax_fairygui_GObject_getRoot);
        tolua_function(tolua_S,"onStage",lua_ax_fairygui_GObject_onStage);
        tolua_function(tolua_S,"removeFromParent",lua_ax_fairygui_GObject_removeFromParent);
        tolua_function(tolua_S,"getProp",lua_ax_fairygui_GObject_getProp);
        tolua_function(tolua_S,"setProp",lua_ax_fairygui_GObject_setProp);
        tolua_function(tolua_S,"constructFromResource",lua_ax_fairygui_GObject_constructFromResource);
        tolua_function(tolua_S,"hitTest",lua_ax_fairygui_GObject_hitTest);
        tolua_function(tolua_S,"treeNode",lua_ax_fairygui_GObject_treeNode);
        tolua_function(tolua_S,"getDraggingObject", lua_ax_fairygui_GObject_getDraggingObject);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GObject_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GObject).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GObject";
    g_typeCast[typeName] = "fgui.GObject";
    return 1;
}

int lua_ax_fairygui_UIPackage_getId(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIPackage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIPackage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIPackage_getId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIPackage:getId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIPackage_getName(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIPackage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIPackage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIPackage_getName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getName();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIPackage:getName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIPackage_getItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIPackage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIPackage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIPackage_getItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItem(arg0);
        object_to_luaval<fairygui::PackageItem>(tolua_S, "fgui.PackageItem",(fairygui::PackageItem*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIPackage:getItem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIPackage_getItemByName(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIPackage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::UIPackage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_UIPackage_getItemByName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getItemByName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getItemByName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItemByName(arg0);
        object_to_luaval<fairygui::PackageItem>(tolua_S, "fgui.PackageItem",(fairygui::PackageItem*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIPackage:getItemByName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getItemByName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_UIPackage_getById(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getById");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getById'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getById(arg0);
        object_to_luaval<fairygui::UIPackage>(tolua_S, "fgui.UIPackage",(fairygui::UIPackage*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getById",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getById'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getByName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getByName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getByName'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getByName(arg0);
        object_to_luaval<fairygui::UIPackage>(tolua_S, "fgui.UIPackage",(fairygui::UIPackage*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getByName",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getByName'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_addPackage(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:addPackage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_addPackage'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::addPackage(arg0);
        object_to_luaval<fairygui::UIPackage>(tolua_S, "fgui.UIPackage",(fairygui::UIPackage*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:addPackage",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_addPackage'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_removePackage(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:removePackage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_removePackage'", nullptr);
            return 0;
        }
        fairygui::UIPackage::removePackage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:removePackage",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_removePackage'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_removeAllPackages(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_removeAllPackages'", nullptr);
            return 0;
        }
        fairygui::UIPackage::removeAllPackages();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:removeAllPackages",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_removeAllPackages'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_createObject(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string arg0;
        std::string arg1;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:createObject");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "fgui.UIPackage:createObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_createObject'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::createObject(arg0, arg1);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:createObject",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_createObject'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_createObjectFromURL(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:createObjectFromURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_createObjectFromURL'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::createObjectFromURL(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:createObjectFromURL",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_createObjectFromURL'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getItemURL(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string arg0;
        std::string arg1;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getItemURL");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "fgui.UIPackage:getItemURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getItemURL'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getItemURL(arg0, arg1);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getItemURL",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getItemURL'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getItemByURL(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "fgui.UIPackage:getItemByURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getItemByURL'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getItemByURL(arg0);
        object_to_luaval<fairygui::PackageItem>(tolua_S, "fgui.PackageItem",(fairygui::PackageItem*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getItemByURL",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getItemByURL'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_normalizeURL(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:normalizeURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_normalizeURL'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::normalizeURL(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:normalizeURL",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_normalizeURL'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getEmptyTexture(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getEmptyTexture'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getEmptyTexture();
        object_to_luaval<ax::Texture2D>(tolua_S, "ax.Texture2D",(ax::Texture2D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getEmptyTexture",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getEmptyTexture'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getBranch(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getBranch'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getBranch();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getBranch",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getBranch'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_setBranch(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:setBranch");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_setBranch'", nullptr);
            return 0;
        }
        fairygui::UIPackage::setBranch(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:setBranch",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_setBranch'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_getVar(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:getVar");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_getVar'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::UIPackage::getVar(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:getVar",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_getVar'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_setVar(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIPackage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string arg0;
        std::string arg1;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.UIPackage:setVar");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "fgui.UIPackage:setVar");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_setVar'", nullptr);
            return 0;
        }
        fairygui::UIPackage::setVar(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIPackage:setVar",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_setVar'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIPackage_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::UIPackage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIPackage_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::UIPackage();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.UIPackage");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.UIPackage:UIPackage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIPackage_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_UIPackage_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (UIPackage)");
    return 0;
}

int lua_register_ax_fairygui_UIPackage(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.UIPackage");
    tolua_cclass(tolua_S,"UIPackage","fgui.UIPackage","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"UIPackage");
        tolua_function(tolua_S,"new",lua_ax_fairygui_UIPackage_constructor);
        tolua_function(tolua_S,"getId",lua_ax_fairygui_UIPackage_getId);
        tolua_function(tolua_S,"getName",lua_ax_fairygui_UIPackage_getName);
        tolua_function(tolua_S,"getItem",lua_ax_fairygui_UIPackage_getItem);
        tolua_function(tolua_S,"getItemByName",lua_ax_fairygui_UIPackage_getItemByName);
        tolua_function(tolua_S,"getById", lua_ax_fairygui_UIPackage_getById);
        tolua_function(tolua_S,"getByName", lua_ax_fairygui_UIPackage_getByName);
        tolua_function(tolua_S,"addPackage", lua_ax_fairygui_UIPackage_addPackage);
        tolua_function(tolua_S,"removePackage", lua_ax_fairygui_UIPackage_removePackage);
        tolua_function(tolua_S,"removeAllPackages", lua_ax_fairygui_UIPackage_removeAllPackages);
        tolua_function(tolua_S,"createObject", lua_ax_fairygui_UIPackage_createObject);
        tolua_function(tolua_S,"createObjectFromURL", lua_ax_fairygui_UIPackage_createObjectFromURL);
        tolua_function(tolua_S,"getItemURL", lua_ax_fairygui_UIPackage_getItemURL);
        tolua_function(tolua_S,"getItemByURL", lua_ax_fairygui_UIPackage_getItemByURL);
        tolua_function(tolua_S,"normalizeURL", lua_ax_fairygui_UIPackage_normalizeURL);
        tolua_function(tolua_S,"getEmptyTexture", lua_ax_fairygui_UIPackage_getEmptyTexture);
        tolua_function(tolua_S,"getBranch", lua_ax_fairygui_UIPackage_getBranch);
        tolua_function(tolua_S,"setBranch", lua_ax_fairygui_UIPackage_setBranch);
        tolua_function(tolua_S,"getVar", lua_ax_fairygui_UIPackage_getVar);
        tolua_function(tolua_S,"setVar", lua_ax_fairygui_UIPackage_setVar);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::UIPackage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.UIPackage";
    g_typeCast[typeName] = "fgui.UIPackage";
    return 1;
}

int lua_ax_fairygui_GImage_getFlip(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_getFlip'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_getFlip'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFlip();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:getFlip",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_getFlip'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setFlip(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setFlip'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FlipType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GImage:setFlip");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setFlip'", nullptr);
            return 0;
        }
        cobj->setFlip(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setFlip",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setFlip'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_getColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_getColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GImage:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setColor'", nullptr);
            return 0;
        }
        cobj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_getFillMethod(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_getFillMethod'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_getFillMethod'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFillMethod();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:getFillMethod",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_getFillMethod'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setFillMethod(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setFillMethod'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FillMethod arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GImage:setFillMethod");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setFillMethod'", nullptr);
            return 0;
        }
        cobj->setFillMethod(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setFillMethod",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setFillMethod'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_getFillOrigin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_getFillOrigin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_getFillOrigin'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFillOrigin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:getFillOrigin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_getFillOrigin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setFillOrigin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setFillOrigin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FillOrigin arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GImage:setFillOrigin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setFillOrigin'", nullptr);
            return 0;
        }
        cobj->setFillOrigin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setFillOrigin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setFillOrigin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_isFillClockwise(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_isFillClockwise'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_isFillClockwise'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isFillClockwise();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:isFillClockwise",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_isFillClockwise'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setFillClockwise(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setFillClockwise'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GImage:setFillClockwise");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setFillClockwise'", nullptr);
            return 0;
        }
        cobj->setFillClockwise(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setFillClockwise",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setFillClockwise'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_getFillAmount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_getFillAmount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_getFillAmount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFillAmount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:getFillAmount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_getFillAmount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_setFillAmount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GImage*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GImage_setFillAmount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GImage:setFillAmount");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_setFillAmount'", nullptr);
            return 0;
        }
        cobj->setFillAmount(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:setFillAmount",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_setFillAmount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GImage_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GImage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GImage::create();
        object_to_luaval<fairygui::GImage>(tolua_S, "fgui.GImage",(fairygui::GImage*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GImage:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GImage_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GImage* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GImage_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GImage();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GImage");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GImage:GImage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GImage_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GImage_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GImage)");
    return 0;
}

int lua_register_ax_fairygui_GImage(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GImage");
    tolua_cclass(tolua_S,"GImage","fgui.GImage","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GImage");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GImage_constructor);
        tolua_function(tolua_S,"getFlip",lua_ax_fairygui_GImage_getFlip);
        tolua_function(tolua_S,"setFlip",lua_ax_fairygui_GImage_setFlip);
        tolua_function(tolua_S,"getColor",lua_ax_fairygui_GImage_getColor);
        tolua_function(tolua_S,"setColor",lua_ax_fairygui_GImage_setColor);
        tolua_function(tolua_S,"getFillMethod",lua_ax_fairygui_GImage_getFillMethod);
        tolua_function(tolua_S,"setFillMethod",lua_ax_fairygui_GImage_setFillMethod);
        tolua_function(tolua_S,"getFillOrigin",lua_ax_fairygui_GImage_getFillOrigin);
        tolua_function(tolua_S,"setFillOrigin",lua_ax_fairygui_GImage_setFillOrigin);
        tolua_function(tolua_S,"isFillClockwise",lua_ax_fairygui_GImage_isFillClockwise);
        tolua_function(tolua_S,"setFillClockwise",lua_ax_fairygui_GImage_setFillClockwise);
        tolua_function(tolua_S,"getFillAmount",lua_ax_fairygui_GImage_getFillAmount);
        tolua_function(tolua_S,"setFillAmount",lua_ax_fairygui_GImage_setFillAmount);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GImage_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GImage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GImage";
    g_typeCast[typeName] = "fgui.GImage";
    return 1;
}

int lua_ax_fairygui_GMovieClip_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_setPlaying(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GMovieClip:setPlaying");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaying'", nullptr);
            return 0;
        }
        cobj->setPlaying(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setPlaying",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_getFrame(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_getFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_getFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrame();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:getFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_getFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_setFrame(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setFrame'", nullptr);
            return 0;
        }
        cobj->setFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_getTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_getTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_getTimeScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTimeScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:getTimeScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_getTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_setTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GMovieClip:setTimeScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setTimeScale'", nullptr);
            return 0;
        }
        cobj->setTimeScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setTimeScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_advance(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_advance'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GMovieClip:advance");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_advance'", nullptr);
            return 0;
        }
        cobj->advance(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:advance",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_advance'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_getFlip(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_getFlip'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_getFlip'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFlip();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:getFlip",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_getFlip'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_setFlip(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setFlip'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FlipType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setFlip");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setFlip'", nullptr);
            return 0;
        }
        cobj->setFlip(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setFlip",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setFlip'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_getColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_getColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_setColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GMovieClip:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setColor'", nullptr);
            return 0;
        }
        cobj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GMovieClip_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GMovieClip::create();
        object_to_luaval<fairygui::GMovieClip>(tolua_S, "fgui.GMovieClip",(fairygui::GMovieClip*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GMovieClip:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GMovieClip_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GMovieClip();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GMovieClip");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:GMovieClip",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GMovieClip_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GMovieClip)");
    return 0;
}

int lua_register_ax_fairygui_GMovieClip(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GMovieClip");
    tolua_cclass(tolua_S,"GMovieClip","fgui.GMovieClip","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GMovieClip");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GMovieClip_constructor);
        tolua_function(tolua_S,"isPlaying",lua_ax_fairygui_GMovieClip_isPlaying);
        tolua_function(tolua_S,"setPlaying",lua_ax_fairygui_GMovieClip_setPlaying);
        tolua_function(tolua_S,"getFrame",lua_ax_fairygui_GMovieClip_getFrame);
        tolua_function(tolua_S,"setFrame",lua_ax_fairygui_GMovieClip_setFrame);
        tolua_function(tolua_S,"getTimeScale",lua_ax_fairygui_GMovieClip_getTimeScale);
        tolua_function(tolua_S,"setTimeScale",lua_ax_fairygui_GMovieClip_setTimeScale);
        tolua_function(tolua_S,"advance",lua_ax_fairygui_GMovieClip_advance);
        tolua_function(tolua_S,"getFlip",lua_ax_fairygui_GMovieClip_getFlip);
        tolua_function(tolua_S,"setFlip",lua_ax_fairygui_GMovieClip_setFlip);
        tolua_function(tolua_S,"getColor",lua_ax_fairygui_GMovieClip_getColor);
        tolua_function(tolua_S,"setColor",lua_ax_fairygui_GMovieClip_setColor);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GMovieClip_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GMovieClip).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GMovieClip";
    g_typeCast[typeName] = "fgui.GMovieClip";
    return 1;
}

int lua_ax_fairygui_GTextField_isUBBEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_isUBBEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_isUBBEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isUBBEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:isUBBEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_isUBBEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setUBBEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setUBBEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTextField:setUBBEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setUBBEnabled'", nullptr);
            return 0;
        }
        cobj->setUBBEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setUBBEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setUBBEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getAutoSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getAutoSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getAutoSize'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getAutoSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getAutoSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getAutoSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setAutoSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setAutoSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::AutoSizeType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTextField:setAutoSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setAutoSize'", nullptr);
            return 0;
        }
        cobj->setAutoSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setAutoSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setAutoSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_isSingleLine(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_isSingleLine'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_isSingleLine'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isSingleLine();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:isSingleLine",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_isSingleLine'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setSingleLine(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setSingleLine'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTextField:setSingleLine");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setSingleLine'", nullptr);
            return 0;
        }
        cobj->setSingleLine(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setSingleLine",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setSingleLine'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getTextFormat(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getTextFormat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getTextFormat'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTextFormat();
        object_to_luaval<fairygui::TextFormat>(tolua_S, "fgui.TextFormat",(fairygui::TextFormat*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getTextFormat",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getTextFormat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_applyTextFormat(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_applyTextFormat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_applyTextFormat'", nullptr);
            return 0;
        }
        cobj->applyTextFormat();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:applyTextFormat",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_applyTextFormat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getTextSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getTextSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getTextSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTextSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getTextSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getTextSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GTextField:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setColor'", nullptr);
            return 0;
        }
        cobj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getFontSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getFontSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTextField:setFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setFontSize'", nullptr);
            return 0;
        }
        cobj->setFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setFontSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getOutlineColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getOutlineColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getOutlineColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOutlineColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getOutlineColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getOutlineColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setOutlineColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setOutlineColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GTextField:setOutlineColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setOutlineColor'", nullptr);
            return 0;
        }
        cobj->setOutlineColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setOutlineColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setOutlineColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_getTemplateVars(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_getTemplateVars'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_getTemplateVars'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTemplateVars();
        ccvaluemap_to_luaval(tolua_S, *ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:getTemplateVars",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_getTemplateVars'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setTemplateVars(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setTemplateVars'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::ValueMap* arg0;

        ok &= luaval_to_ccvaluemap(tolua_S, 2, arg0, "fgui.GTextField:setTemplateVars");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setTemplateVars'", nullptr);
            return 0;
        }
        cobj->setTemplateVars(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setTemplateVars",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setTemplateVars'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_setVar(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_setVar'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        ax::Value arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GTextField:setVar");

        ok &= luaval_to_ccvalue(tolua_S, 3, &arg1, "fgui.GTextField:setVar");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_setVar'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setVar(arg0, arg1);
        object_to_luaval<fairygui::GTextField>(tolua_S, "fgui.GTextField",(fairygui::GTextField*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:setVar",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_setVar'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextField_flushVars(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextField_flushVars'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextField_flushVars'", nullptr);
            return 0;
        }
        cobj->flushVars();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextField:flushVars",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextField_flushVars'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_fairygui_GTextField_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GTextField)");
    return 0;
}

int lua_register_ax_fairygui_GTextField(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GTextField");
    tolua_cclass(tolua_S,"GTextField","fgui.GTextField","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GTextField");
        tolua_function(tolua_S,"isUBBEnabled",lua_ax_fairygui_GTextField_isUBBEnabled);
        tolua_function(tolua_S,"setUBBEnabled",lua_ax_fairygui_GTextField_setUBBEnabled);
        tolua_function(tolua_S,"getAutoSize",lua_ax_fairygui_GTextField_getAutoSize);
        tolua_function(tolua_S,"setAutoSize",lua_ax_fairygui_GTextField_setAutoSize);
        tolua_function(tolua_S,"isSingleLine",lua_ax_fairygui_GTextField_isSingleLine);
        tolua_function(tolua_S,"setSingleLine",lua_ax_fairygui_GTextField_setSingleLine);
        tolua_function(tolua_S,"getTextFormat",lua_ax_fairygui_GTextField_getTextFormat);
        tolua_function(tolua_S,"applyTextFormat",lua_ax_fairygui_GTextField_applyTextFormat);
        tolua_function(tolua_S,"getTextSize",lua_ax_fairygui_GTextField_getTextSize);
        tolua_function(tolua_S,"getColor",lua_ax_fairygui_GTextField_getColor);
        tolua_function(tolua_S,"setColor",lua_ax_fairygui_GTextField_setColor);
        tolua_function(tolua_S,"getFontSize",lua_ax_fairygui_GTextField_getFontSize);
        tolua_function(tolua_S,"setFontSize",lua_ax_fairygui_GTextField_setFontSize);
        tolua_function(tolua_S,"getOutlineColor",lua_ax_fairygui_GTextField_getOutlineColor);
        tolua_function(tolua_S,"setOutlineColor",lua_ax_fairygui_GTextField_setOutlineColor);
        tolua_function(tolua_S,"getTemplateVars",lua_ax_fairygui_GTextField_getTemplateVars);
        tolua_function(tolua_S,"setTemplateVars",lua_ax_fairygui_GTextField_setTemplateVars);
        tolua_function(tolua_S,"setVar",lua_ax_fairygui_GTextField_setVar);
        tolua_function(tolua_S,"flushVars",lua_ax_fairygui_GTextField_flushVars);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GTextField).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GTextField";
    g_typeCast[typeName] = "fgui.GTextField";
    return 1;
}

int lua_ax_fairygui_GBasicTextField_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GBasicTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GBasicTextField_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GBasicTextField::create();
        object_to_luaval<fairygui::GBasicTextField>(tolua_S, "fgui.GBasicTextField",(fairygui::GBasicTextField*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GBasicTextField:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GBasicTextField_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GBasicTextField_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GBasicTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GBasicTextField_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GBasicTextField();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GBasicTextField");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GBasicTextField:GBasicTextField",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GBasicTextField_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GBasicTextField_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GBasicTextField)");
    return 0;
}

int lua_register_ax_fairygui_GBasicTextField(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GBasicTextField");
    tolua_cclass(tolua_S,"GBasicTextField","fgui.GBasicTextField","fgui.GTextField",nullptr);

    tolua_beginmodule(tolua_S,"GBasicTextField");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GBasicTextField_constructor);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GBasicTextField_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GBasicTextField).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GBasicTextField";
    g_typeCast[typeName] = "fgui.GBasicTextField";
    return 1;
}

int lua_ax_fairygui_GRichTextField_getControl(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRichTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRichTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRichTextField*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRichTextField_getControl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GRichTextField:getControl");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRichTextField_getControl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getControl(arg0);
        object_to_luaval<fairygui::HtmlObject>(tolua_S, "fgui.HtmlObject",(fairygui::HtmlObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRichTextField:getControl",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRichTextField_getControl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRichTextField_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GRichTextField",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRichTextField_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GRichTextField::create();
        object_to_luaval<fairygui::GRichTextField>(tolua_S, "fgui.GRichTextField",(fairygui::GRichTextField*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GRichTextField:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRichTextField_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GRichTextField_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRichTextField* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRichTextField_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GRichTextField();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GRichTextField");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRichTextField:GRichTextField",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRichTextField_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GRichTextField_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GRichTextField)");
    return 0;
}

int lua_register_ax_fairygui_GRichTextField(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GRichTextField");
    tolua_cclass(tolua_S,"GRichTextField","fgui.GRichTextField","fgui.GTextField",nullptr);

    tolua_beginmodule(tolua_S,"GRichTextField");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GRichTextField_constructor);
        tolua_function(tolua_S,"getControl",lua_ax_fairygui_GRichTextField_getControl);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GRichTextField_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GRichTextField).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GRichTextField";
    g_typeCast[typeName] = "fgui.GRichTextField";
    return 1;
}

int lua_ax_fairygui_GTextInput_setPrompt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextInput*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextInput_setPrompt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GTextInput:setPrompt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_setPrompt'", nullptr);
            return 0;
        }
        cobj->setPrompt(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:setPrompt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_setPrompt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextInput_setPassword(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextInput*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextInput_setPassword'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTextInput:setPassword");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_setPassword'", nullptr);
            return 0;
        }
        cobj->setPassword(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:setPassword",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_setPassword'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextInput_setKeyboardType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextInput*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextInput_setKeyboardType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTextInput:setKeyboardType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_setKeyboardType'", nullptr);
            return 0;
        }
        cobj->setKeyboardType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:setKeyboardType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_setKeyboardType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextInput_setMaxLength(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextInput*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextInput_setMaxLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTextInput:setMaxLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_setMaxLength'", nullptr);
            return 0;
        }
        cobj->setMaxLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:setMaxLength",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_setMaxLength'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextInput_setRestrict(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTextInput*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTextInput_setRestrict'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GTextInput:setRestrict");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_setRestrict'", nullptr);
            return 0;
        }
        cobj->setRestrict(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:setRestrict",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_setRestrict'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTextInput_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTextInput",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTextInput::create();
        object_to_luaval<fairygui::GTextInput>(tolua_S, "fgui.GTextInput",(fairygui::GTextInput*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTextInput:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTextInput_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTextInput* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTextInput_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GTextInput();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GTextInput");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTextInput:GTextInput",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTextInput_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GTextInput_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GTextInput)");
    return 0;
}

int lua_register_ax_fairygui_GTextInput(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GTextInput");
    tolua_cclass(tolua_S,"GTextInput","fgui.GTextInput","fgui.GTextField",nullptr);

    tolua_beginmodule(tolua_S,"GTextInput");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GTextInput_constructor);
        tolua_function(tolua_S,"setPrompt",lua_ax_fairygui_GTextInput_setPrompt);
        tolua_function(tolua_S,"setPassword",lua_ax_fairygui_GTextInput_setPassword);
        tolua_function(tolua_S,"setKeyboardType",lua_ax_fairygui_GTextInput_setKeyboardType);
        tolua_function(tolua_S,"setMaxLength",lua_ax_fairygui_GTextInput_setMaxLength);
        tolua_function(tolua_S,"setRestrict",lua_ax_fairygui_GTextInput_setRestrict);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GTextInput_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GTextInput).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GTextInput";
    g_typeCast[typeName] = "fgui.GTextInput";
    return 1;
}

int lua_ax_fairygui_GGraph_drawRect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_drawRect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        double arg0;
        double arg1;
        int arg2;
        ax::Color4F arg3;
        ax::Color4F arg4;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GGraph:drawRect");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GGraph:drawRect");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GGraph:drawRect");

        ok &=luaval_to_color4f(tolua_S, 5, &arg3, "fgui.GGraph:drawRect");

        ok &=luaval_to_color4f(tolua_S, 6, &arg4, "fgui.GGraph:drawRect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawRect'", nullptr);
            return 0;
        }
        cobj->drawRect(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:drawRect",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_drawRect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_drawEllipse(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_drawEllipse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        double arg0;
        double arg1;
        int arg2;
        ax::Color4F arg3;
        ax::Color4F arg4;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GGraph:drawEllipse");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GGraph:drawEllipse");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GGraph:drawEllipse");

        ok &=luaval_to_color4f(tolua_S, 5, &arg3, "fgui.GGraph:drawEllipse");

        ok &=luaval_to_color4f(tolua_S, 6, &arg4, "fgui.GGraph:drawEllipse");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawEllipse'", nullptr);
            return 0;
        }
        cobj->drawEllipse(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:drawEllipse",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_drawEllipse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_drawPolygon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_drawPolygon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        int arg0;
        ax::Color4F arg1;
        ax::Color4F arg2;
        const ax::Vec2* arg3;
        int arg4;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGraph:drawPolygon");

        ok &=luaval_to_color4f(tolua_S, 3, &arg1, "fgui.GGraph:drawPolygon");

        ok &=luaval_to_color4f(tolua_S, 4, &arg2, "fgui.GGraph:drawPolygon");

        ok &= luaval_to_object<const ax::Vec2>(tolua_S, 5, "ax.Vec2",&arg3, "fgui.GGraph:drawPolygon");

        ok &= luaval_to_int32(tolua_S, 6,(int *)&arg4, "fgui.GGraph:drawPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawPolygon'", nullptr);
            return 0;
        }
        cobj->drawPolygon(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:drawPolygon",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_drawPolygon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_drawRegularPolygon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        int arg0;
        ax::Color4F arg1;
        ax::Color4F arg2;
        int arg3;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 3, &arg1, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 4, &arg2, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GGraph:drawRegularPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'", nullptr);
            return 0;
        }
        cobj->drawRegularPolygon(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 5) 
    {
        int arg0;
        ax::Color4F arg1;
        ax::Color4F arg2;
        int arg3;
        double arg4;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 3, &arg1, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 4, &arg2, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "fgui.GGraph:drawRegularPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'", nullptr);
            return 0;
        }
        cobj->drawRegularPolygon(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 6) 
    {
        int arg0;
        ax::Color4F arg1;
        ax::Color4F arg2;
        int arg3;
        double arg4;
        const float* arg5;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 3, &arg1, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 4, &arg2, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "fgui.GGraph:drawRegularPolygon");

        #pragma warning NO CONVERSION TO NATIVE FOR float*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'", nullptr);
            return 0;
        }
        cobj->drawRegularPolygon(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 7) 
    {
        int arg0;
        ax::Color4F arg1;
        ax::Color4F arg2;
        int arg3;
        double arg4;
        const float* arg5;
        int arg6;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 3, &arg1, "fgui.GGraph:drawRegularPolygon");

        ok &=luaval_to_color4f(tolua_S, 4, &arg2, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GGraph:drawRegularPolygon");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "fgui.GGraph:drawRegularPolygon");

        #pragma warning NO CONVERSION TO NATIVE FOR float*
		ok = false;

        ok &= luaval_to_int32(tolua_S, 8,(int *)&arg6, "fgui.GGraph:drawRegularPolygon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'", nullptr);
            return 0;
        }
        cobj->drawRegularPolygon(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:drawRegularPolygon",argc, 4);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_drawRegularPolygon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_isEmpty(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_isEmpty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_isEmpty'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isEmpty();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:isEmpty",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_isEmpty'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_getColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_getColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_setColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGraph*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGraph_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GGraph:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_setColor'", nullptr);
            return 0;
        }
        cobj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGraph_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GGraph",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GGraph::create();
        object_to_luaval<fairygui::GGraph>(tolua_S, "fgui.GGraph",(fairygui::GGraph*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GGraph:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GGraph_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGraph* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGraph_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GGraph();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GGraph");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGraph:GGraph",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGraph_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GGraph_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GGraph)");
    return 0;
}

int lua_register_ax_fairygui_GGraph(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GGraph");
    tolua_cclass(tolua_S,"GGraph","fgui.GGraph","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GGraph");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GGraph_constructor);
        tolua_function(tolua_S,"drawRect",lua_ax_fairygui_GGraph_drawRect);
        tolua_function(tolua_S,"drawEllipse",lua_ax_fairygui_GGraph_drawEllipse);
        tolua_function(tolua_S,"drawPolygon",lua_ax_fairygui_GGraph_drawPolygon);
        tolua_function(tolua_S,"drawRegularPolygon",lua_ax_fairygui_GGraph_drawRegularPolygon);
        tolua_function(tolua_S,"isEmpty",lua_ax_fairygui_GGraph_isEmpty);
        tolua_function(tolua_S,"getColor",lua_ax_fairygui_GGraph_getColor);
        tolua_function(tolua_S,"setColor",lua_ax_fairygui_GGraph_setColor);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GGraph_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GGraph).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GGraph";
    g_typeCast[typeName] = "fgui.GGraph";
    return 1;
}

int lua_ax_fairygui_GLoader_getURL(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getURL'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getURL'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getURL();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getURL",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setURL(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setURL'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GLoader:setURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setURL'", nullptr);
            return 0;
        }
        cobj->setURL(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setURL",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getAlign'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getAlign();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getAlign",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::TextHAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setAlign");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setAlign'", nullptr);
            return 0;
        }
        cobj->setAlign(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setAlign",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getVerticalAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getVerticalAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getVerticalAlign'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getVerticalAlign();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getVerticalAlign",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getVerticalAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setVerticalAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setVerticalAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::TextVAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setVerticalAlign");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setVerticalAlign'", nullptr);
            return 0;
        }
        cobj->setVerticalAlign(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setVerticalAlign",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setVerticalAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getAutoSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getAutoSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getAutoSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAutoSize();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getAutoSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getAutoSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setAutoSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setAutoSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GLoader:setAutoSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setAutoSize'", nullptr);
            return 0;
        }
        cobj->setAutoSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setAutoSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setAutoSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getFill(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getFill'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getFill'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFill();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getFill",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getFill'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFill(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFill'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::LoaderFillType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setFill");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFill'", nullptr);
            return 0;
        }
        cobj->setFill(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFill",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFill'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_isShrinkOnly(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_isShrinkOnly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_isShrinkOnly'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isShrinkOnly();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:isShrinkOnly",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_isShrinkOnly'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setShrinkOnly(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setShrinkOnly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GLoader:setShrinkOnly");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setShrinkOnly'", nullptr);
            return 0;
        }
        cobj->setShrinkOnly(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setShrinkOnly",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setShrinkOnly'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getContentSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getContentSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getContentSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getContentSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getContentSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GLoader:setColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setColor'", nullptr);
            return 0;
        }
        cobj->setColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setPlaying(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GLoader:setPlaying");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setPlaying'", nullptr);
            return 0;
        }
        cobj->setPlaying(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setPlaying",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getFrame(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrame();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFrame(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFrame'", nullptr);
            return 0;
        }
        cobj->setFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFrame",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getFillMethod(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getFillMethod'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getFillMethod'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFillMethod();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getFillMethod",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getFillMethod'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFillMethod(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFillMethod'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FillMethod arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setFillMethod");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFillMethod'", nullptr);
            return 0;
        }
        cobj->setFillMethod(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFillMethod",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFillMethod'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getFillOrigin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getFillOrigin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getFillOrigin'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getFillOrigin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getFillOrigin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getFillOrigin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFillOrigin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFillOrigin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::FillOrigin arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GLoader:setFillOrigin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFillOrigin'", nullptr);
            return 0;
        }
        cobj->setFillOrigin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFillOrigin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFillOrigin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_isFillClockwise(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_isFillClockwise'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_isFillClockwise'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isFillClockwise();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:isFillClockwise",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_isFillClockwise'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFillClockwise(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFillClockwise'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GLoader:setFillClockwise");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFillClockwise'", nullptr);
            return 0;
        }
        cobj->setFillClockwise(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFillClockwise",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFillClockwise'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getFillAmount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getFillAmount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getFillAmount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFillAmount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getFillAmount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getFillAmount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_setFillAmount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_setFillAmount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GLoader:setFillAmount");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_setFillAmount'", nullptr);
            return 0;
        }
        cobj->setFillAmount(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:setFillAmount",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_setFillAmount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_getComponent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GLoader_getComponent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_getComponent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getComponent();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:getComponent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_getComponent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GLoader_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GLoader::create();
        object_to_luaval<fairygui::GLoader>(tolua_S, "fgui.GLoader",(fairygui::GLoader*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GLoader:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GLoader_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GLoader_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GLoader();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GLoader");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GLoader:GLoader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GLoader_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GLoader_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GLoader)");
    return 0;
}

int lua_register_ax_fairygui_GLoader(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GLoader");
    tolua_cclass(tolua_S,"GLoader","fgui.GLoader","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GLoader");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GLoader_constructor);
        tolua_function(tolua_S,"getURL",lua_ax_fairygui_GLoader_getURL);
        tolua_function(tolua_S,"setURL",lua_ax_fairygui_GLoader_setURL);
        tolua_function(tolua_S,"getAlign",lua_ax_fairygui_GLoader_getAlign);
        tolua_function(tolua_S,"setAlign",lua_ax_fairygui_GLoader_setAlign);
        tolua_function(tolua_S,"getVerticalAlign",lua_ax_fairygui_GLoader_getVerticalAlign);
        tolua_function(tolua_S,"setVerticalAlign",lua_ax_fairygui_GLoader_setVerticalAlign);
        tolua_function(tolua_S,"getAutoSize",lua_ax_fairygui_GLoader_getAutoSize);
        tolua_function(tolua_S,"setAutoSize",lua_ax_fairygui_GLoader_setAutoSize);
        tolua_function(tolua_S,"getFill",lua_ax_fairygui_GLoader_getFill);
        tolua_function(tolua_S,"setFill",lua_ax_fairygui_GLoader_setFill);
        tolua_function(tolua_S,"isShrinkOnly",lua_ax_fairygui_GLoader_isShrinkOnly);
        tolua_function(tolua_S,"setShrinkOnly",lua_ax_fairygui_GLoader_setShrinkOnly);
        tolua_function(tolua_S,"getContentSize",lua_ax_fairygui_GLoader_getContentSize);
        tolua_function(tolua_S,"getColor",lua_ax_fairygui_GLoader_getColor);
        tolua_function(tolua_S,"setColor",lua_ax_fairygui_GLoader_setColor);
        tolua_function(tolua_S,"isPlaying",lua_ax_fairygui_GLoader_isPlaying);
        tolua_function(tolua_S,"setPlaying",lua_ax_fairygui_GLoader_setPlaying);
        tolua_function(tolua_S,"getFrame",lua_ax_fairygui_GLoader_getFrame);
        tolua_function(tolua_S,"setFrame",lua_ax_fairygui_GLoader_setFrame);
        tolua_function(tolua_S,"getFillMethod",lua_ax_fairygui_GLoader_getFillMethod);
        tolua_function(tolua_S,"setFillMethod",lua_ax_fairygui_GLoader_setFillMethod);
        tolua_function(tolua_S,"getFillOrigin",lua_ax_fairygui_GLoader_getFillOrigin);
        tolua_function(tolua_S,"setFillOrigin",lua_ax_fairygui_GLoader_setFillOrigin);
        tolua_function(tolua_S,"isFillClockwise",lua_ax_fairygui_GLoader_isFillClockwise);
        tolua_function(tolua_S,"setFillClockwise",lua_ax_fairygui_GLoader_setFillClockwise);
        tolua_function(tolua_S,"getFillAmount",lua_ax_fairygui_GLoader_getFillAmount);
        tolua_function(tolua_S,"setFillAmount",lua_ax_fairygui_GLoader_setFillAmount);
        tolua_function(tolua_S,"getComponent",lua_ax_fairygui_GLoader_getComponent);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GLoader_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GLoader).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GLoader";
    g_typeCast[typeName] = "fgui.GLoader";
    return 1;
}

int lua_ax_fairygui_GGroup_getLayout(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_getLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_getLayout'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getLayout();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:getLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_getLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setLayout(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GroupLayoutType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGroup:setLayout");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setLayout'", nullptr);
            return 0;
        }
        cobj->setLayout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setLayout",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_getColumnGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_getColumnGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_getColumnGap'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColumnGap();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:getColumnGap",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_getColumnGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setColumnGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setColumnGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGroup:setColumnGap");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setColumnGap'", nullptr);
            return 0;
        }
        cobj->setColumnGap(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setColumnGap",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setColumnGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_getLineGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_getLineGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_getLineGap'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLineGap();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:getLineGap",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_getLineGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setLineGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setLineGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGroup:setLineGap");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setLineGap'", nullptr);
            return 0;
        }
        cobj->setLineGap(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setLineGap",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setLineGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_isExcludeInvisibles(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_isExcludeInvisibles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_isExcludeInvisibles'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isExcludeInvisibles();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:isExcludeInvisibles",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_isExcludeInvisibles'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setExcludeInvisibles(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setExcludeInvisibles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GGroup:setExcludeInvisibles");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setExcludeInvisibles'", nullptr);
            return 0;
        }
        cobj->setExcludeInvisibles(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setExcludeInvisibles",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setExcludeInvisibles'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_isAutoSizeDisabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_isAutoSizeDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_isAutoSizeDisabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isAutoSizeDisabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:isAutoSizeDisabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_isAutoSizeDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setAutoSizeDisabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setAutoSizeDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GGroup:setAutoSizeDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setAutoSizeDisabled'", nullptr);
            return 0;
        }
        cobj->setAutoSizeDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setAutoSizeDisabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setAutoSizeDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_getMainGridIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_getMainGridIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_getMainGridIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMainGridIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:getMainGridIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_getMainGridIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setMainGridIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setMainGridIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGroup:setMainGridIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setMainGridIndex'", nullptr);
            return 0;
        }
        cobj->setMainGridIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setMainGridIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setMainGridIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_getMainGridMinSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_getMainGridMinSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_getMainGridMinSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMainGridMinSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:getMainGridMinSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_getMainGridMinSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setMainGridMinSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setMainGridMinSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GGroup:setMainGridMinSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setMainGridMinSize'", nullptr);
            return 0;
        }
        cobj->setMainGridMinSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setMainGridMinSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setMainGridMinSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_setBoundsChangedFlag(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_setBoundsChangedFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setBoundsChangedFlag'", nullptr);
            return 0;
        }
        cobj->setBoundsChangedFlag();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GGroup:setBoundsChangedFlag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_setBoundsChangedFlag'", nullptr);
            return 0;
        }
        cobj->setBoundsChangedFlag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:setBoundsChangedFlag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_setBoundsChangedFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_moveChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_moveChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GGroup:moveChildren");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GGroup:moveChildren");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_moveChildren'", nullptr);
            return 0;
        }
        cobj->moveChildren(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:moveChildren",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_moveChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_resizeChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GGroup*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GGroup_resizeChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GGroup:resizeChildren");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GGroup:resizeChildren");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_resizeChildren'", nullptr);
            return 0;
        }
        cobj->resizeChildren(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:resizeChildren",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_resizeChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GGroup_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GGroup",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GGroup::create();
        object_to_luaval<fairygui::GGroup>(tolua_S, "fgui.GGroup",(fairygui::GGroup*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GGroup:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GGroup_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GGroup* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GGroup_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GGroup();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GGroup");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GGroup:GGroup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GGroup_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GGroup_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GGroup)");
    return 0;
}

int lua_register_ax_fairygui_GGroup(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GGroup");
    tolua_cclass(tolua_S,"GGroup","fgui.GGroup","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GGroup");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GGroup_constructor);
        tolua_function(tolua_S,"getLayout",lua_ax_fairygui_GGroup_getLayout);
        tolua_function(tolua_S,"setLayout",lua_ax_fairygui_GGroup_setLayout);
        tolua_function(tolua_S,"getColumnGap",lua_ax_fairygui_GGroup_getColumnGap);
        tolua_function(tolua_S,"setColumnGap",lua_ax_fairygui_GGroup_setColumnGap);
        tolua_function(tolua_S,"getLineGap",lua_ax_fairygui_GGroup_getLineGap);
        tolua_function(tolua_S,"setLineGap",lua_ax_fairygui_GGroup_setLineGap);
        tolua_function(tolua_S,"isExcludeInvisibles",lua_ax_fairygui_GGroup_isExcludeInvisibles);
        tolua_function(tolua_S,"setExcludeInvisibles",lua_ax_fairygui_GGroup_setExcludeInvisibles);
        tolua_function(tolua_S,"isAutoSizeDisabled",lua_ax_fairygui_GGroup_isAutoSizeDisabled);
        tolua_function(tolua_S,"setAutoSizeDisabled",lua_ax_fairygui_GGroup_setAutoSizeDisabled);
        tolua_function(tolua_S,"getMainGridIndex",lua_ax_fairygui_GGroup_getMainGridIndex);
        tolua_function(tolua_S,"setMainGridIndex",lua_ax_fairygui_GGroup_setMainGridIndex);
        tolua_function(tolua_S,"getMainGridMinSize",lua_ax_fairygui_GGroup_getMainGridMinSize);
        tolua_function(tolua_S,"setMainGridMinSize",lua_ax_fairygui_GGroup_setMainGridMinSize);
        tolua_function(tolua_S,"setBoundsChangedFlag",lua_ax_fairygui_GGroup_setBoundsChangedFlag);
        tolua_function(tolua_S,"moveChildren",lua_ax_fairygui_GGroup_moveChildren);
        tolua_function(tolua_S,"resizeChildren",lua_ax_fairygui_GGroup_resizeChildren);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GGroup_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GGroup).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GGroup";
    g_typeCast[typeName] = "fgui.GGroup";
    return 1;
}

int lua_ax_fairygui_ScrollPane_setup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ByteBuffer* arg0;

        ok &= luaval_to_object<fairygui::ByteBuffer>(tolua_S, 2, "fgui.ByteBuffer",&arg0, "fgui.ScrollPane:setup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setup'", nullptr);
            return 0;
        }
        cobj->setup(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getOwner(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getOwner'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getOwner'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOwner();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getOwner",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getOwner'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getHeader(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getHeader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getHeader'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getHeader();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getHeader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getHeader'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getFooter(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getFooter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getFooter'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFooter();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getFooter",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getFooter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getVtScrollBar(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getVtScrollBar'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getVtScrollBar'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVtScrollBar();
        object_to_luaval<fairygui::GScrollBar>(tolua_S, "fgui.GScrollBar",(fairygui::GScrollBar*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getVtScrollBar",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getVtScrollBar'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getHzScrollBar(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getHzScrollBar'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getHzScrollBar'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getHzScrollBar();
        object_to_luaval<fairygui::GScrollBar>(tolua_S, "fgui.GScrollBar",(fairygui::GScrollBar*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getHzScrollBar",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getHzScrollBar'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isBouncebackEffect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isBouncebackEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isBouncebackEffect'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isBouncebackEffect();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isBouncebackEffect",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isBouncebackEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setBouncebackEffect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setBouncebackEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setBouncebackEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setBouncebackEffect'", nullptr);
            return 0;
        }
        cobj->setBouncebackEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setBouncebackEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setBouncebackEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isTouchEffect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isTouchEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isTouchEffect'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isTouchEffect();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isTouchEffect",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isTouchEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setTouchEffect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setTouchEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setTouchEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setTouchEffect'", nullptr);
            return 0;
        }
        cobj->setTouchEffect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setTouchEffect",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setTouchEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isInertiaDisabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isInertiaDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isInertiaDisabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isInertiaDisabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isInertiaDisabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isInertiaDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setInertiaDisabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setInertiaDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setInertiaDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setInertiaDisabled'", nullptr);
            return 0;
        }
        cobj->setInertiaDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setInertiaDisabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setInertiaDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getScrollStep(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getScrollStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getScrollStep'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScrollStep();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getScrollStep",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getScrollStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setScrollStep(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setScrollStep'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setScrollStep");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setScrollStep'", nullptr);
            return 0;
        }
        cobj->setScrollStep(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setScrollStep",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setScrollStep'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isSnapToItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isSnapToItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isSnapToItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isSnapToItem();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isSnapToItem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isSnapToItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setSnapToItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setSnapToItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setSnapToItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setSnapToItem'", nullptr);
            return 0;
        }
        cobj->setSnapToItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setSnapToItem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setSnapToItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isPageMode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isPageMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isPageMode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPageMode();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isPageMode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isPageMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPageMode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPageMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setPageMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageMode'", nullptr);
            return 0;
        }
        cobj->setPageMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPageMode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPageMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPageController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPageController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPageController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageController();
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPageController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPageController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPageController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPageController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.ScrollPane:setPageController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageController'", nullptr);
            return 0;
        }
        cobj->setPageController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPageController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPageController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isMouseWheelEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isMouseWheelEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isMouseWheelEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isMouseWheelEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isMouseWheelEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isMouseWheelEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setMouseWheelEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setMouseWheelEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:setMouseWheelEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setMouseWheelEnabled'", nullptr);
            return 0;
        }
        cobj->setMouseWheelEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setMouseWheelEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setMouseWheelEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getDecelerationRate(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getDecelerationRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getDecelerationRate'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDecelerationRate();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getDecelerationRate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getDecelerationRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setDecelerationRate(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setDecelerationRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setDecelerationRate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setDecelerationRate'", nullptr);
            return 0;
        }
        cobj->setDecelerationRate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setDecelerationRate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setDecelerationRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPosX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPosX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPosX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPosX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPosX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPosX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPosX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPosX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPosX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPosX'", nullptr);
            return 0;
        }
        cobj->setPosX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPosX");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPosX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPosX'", nullptr);
            return 0;
        }
        cobj->setPosX(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPosX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPosX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPosY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPosY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPosY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPosY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPosY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPosY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPosY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPosY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPosY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPosY'", nullptr);
            return 0;
        }
        cobj->setPosY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPosY");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPosY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPosY'", nullptr);
            return 0;
        }
        cobj->setPosY(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPosY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPosY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPercX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPercX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPercX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPercX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPercX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPercX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPercX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPercX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPercX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPercX'", nullptr);
            return 0;
        }
        cobj->setPercX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPercX");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPercX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPercX'", nullptr);
            return 0;
        }
        cobj->setPercX(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPercX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPercX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPercY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPercY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPercY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPercY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPercY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPercY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPercY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPercY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPercY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPercY'", nullptr);
            return 0;
        }
        cobj->setPercY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:setPercY");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPercY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPercY'", nullptr);
            return 0;
        }
        cobj->setPercY(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPercY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPercY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isBottomMost(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isBottomMost'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isBottomMost'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isBottomMost();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isBottomMost",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isBottomMost'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isRightMost(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isRightMost'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isRightMost'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isRightMost();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isRightMost",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isRightMost'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollLeft(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollLeft'", nullptr);
            return 0;
        }
        cobj->scrollLeft();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollLeft'", nullptr);
            return 0;
        }
        cobj->scrollLeft(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollLeft");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollLeft'", nullptr);
            return 0;
        }
        cobj->scrollLeft(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollLeft",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollRight(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollRight'", nullptr);
            return 0;
        }
        cobj->scrollRight();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollRight'", nullptr);
            return 0;
        }
        cobj->scrollRight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollRight");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollRight'", nullptr);
            return 0;
        }
        cobj->scrollRight(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollRight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollRight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollUp(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollUp'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollUp'", nullptr);
            return 0;
        }
        cobj->scrollUp();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollUp");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollUp'", nullptr);
            return 0;
        }
        cobj->scrollUp(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollUp");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollUp");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollUp'", nullptr);
            return 0;
        }
        cobj->scrollUp(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollUp",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollUp'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollDown(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollDown'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollDown'", nullptr);
            return 0;
        }
        cobj->scrollDown();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollDown");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollDown'", nullptr);
            return 0;
        }
        cobj->scrollDown(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollDown");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollDown");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollDown'", nullptr);
            return 0;
        }
        cobj->scrollDown(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollDown",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollDown'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollTop(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollTop'", nullptr);
            return 0;
        }
        cobj->scrollTop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollTop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollTop'", nullptr);
            return 0;
        }
        cobj->scrollTop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollTop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollTop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollBottom(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollBottom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollBottom'", nullptr);
            return 0;
        }
        cobj->scrollBottom();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.ScrollPane:scrollBottom");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_scrollBottom'", nullptr);
            return 0;
        }
        cobj->scrollBottom(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:scrollBottom",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollBottom'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_scrollToView(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_scrollToView'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            ax::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            ax::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.ScrollPane:scrollToView");

            if (!ok) { break; }
            cobj->scrollToView(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.ScrollPane:scrollToView",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_scrollToView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_isChildInView(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_isChildInView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.ScrollPane:isChildInView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_isChildInView'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isChildInView(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:isChildInView",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_isChildInView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPageX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPageX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPageX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPageX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPageX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPageX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPageX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:setPageX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageX'", nullptr);
            return 0;
        }
        cobj->setPageX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:setPageX");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPageX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageX'", nullptr);
            return 0;
        }
        cobj->setPageX(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPageX",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPageX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getPageY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getPageY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getPageY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPageY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getPageY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getPageY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_setPageY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_setPageY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:setPageY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageY'", nullptr);
            return 0;
        }
        cobj->setPageY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:setPageY");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.ScrollPane:setPageY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_setPageY'", nullptr);
            return 0;
        }
        cobj->setPageY(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:setPageY",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_setPageY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getScrollingPosX(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getScrollingPosX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getScrollingPosX'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScrollingPosX();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getScrollingPosX",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getScrollingPosX'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getScrollingPosY(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getScrollingPosY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getScrollingPosY'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScrollingPosY();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getScrollingPosY",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getScrollingPosY'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getContentSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getContentSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getContentSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getContentSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getContentSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getViewSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_getViewSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getViewSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:getViewSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getViewSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_lockHeader(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_lockHeader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:lockHeader");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_lockHeader'", nullptr);
            return 0;
        }
        cobj->lockHeader(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:lockHeader",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_lockHeader'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_lockFooter(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_lockFooter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.ScrollPane:lockFooter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_lockFooter'", nullptr);
            return 0;
        }
        cobj->lockFooter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:lockFooter",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_lockFooter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_cancelDragging(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::ScrollPane*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_ScrollPane_cancelDragging'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_cancelDragging'", nullptr);
            return 0;
        }
        cobj->cancelDragging();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:cancelDragging",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_cancelDragging'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_ScrollPane_getDraggingPane(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.ScrollPane",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_getDraggingPane'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::ScrollPane::getDraggingPane();
        object_to_luaval<fairygui::ScrollPane>(tolua_S, "fgui.ScrollPane",(fairygui::ScrollPane*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.ScrollPane:getDraggingPane",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_getDraggingPane'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_ScrollPane_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::ScrollPane* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GComponent* arg0;

        ok &= luaval_to_object<fairygui::GComponent>(tolua_S, 2, "fgui.GComponent",&arg0, "fgui.ScrollPane:ScrollPane");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_ScrollPane_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::ScrollPane(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.ScrollPane");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.ScrollPane:ScrollPane",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_ScrollPane_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_ScrollPane_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ScrollPane)");
    return 0;
}

int lua_register_ax_fairygui_ScrollPane(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.ScrollPane");
    tolua_cclass(tolua_S,"ScrollPane","fgui.ScrollPane","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ScrollPane");
        tolua_function(tolua_S,"new",lua_ax_fairygui_ScrollPane_constructor);
        tolua_function(tolua_S,"setup",lua_ax_fairygui_ScrollPane_setup);
        tolua_function(tolua_S,"getOwner",lua_ax_fairygui_ScrollPane_getOwner);
        tolua_function(tolua_S,"getHeader",lua_ax_fairygui_ScrollPane_getHeader);
        tolua_function(tolua_S,"getFooter",lua_ax_fairygui_ScrollPane_getFooter);
        tolua_function(tolua_S,"getVtScrollBar",lua_ax_fairygui_ScrollPane_getVtScrollBar);
        tolua_function(tolua_S,"getHzScrollBar",lua_ax_fairygui_ScrollPane_getHzScrollBar);
        tolua_function(tolua_S,"isBouncebackEffect",lua_ax_fairygui_ScrollPane_isBouncebackEffect);
        tolua_function(tolua_S,"setBouncebackEffect",lua_ax_fairygui_ScrollPane_setBouncebackEffect);
        tolua_function(tolua_S,"isTouchEffect",lua_ax_fairygui_ScrollPane_isTouchEffect);
        tolua_function(tolua_S,"setTouchEffect",lua_ax_fairygui_ScrollPane_setTouchEffect);
        tolua_function(tolua_S,"isInertiaDisabled",lua_ax_fairygui_ScrollPane_isInertiaDisabled);
        tolua_function(tolua_S,"setInertiaDisabled",lua_ax_fairygui_ScrollPane_setInertiaDisabled);
        tolua_function(tolua_S,"getScrollStep",lua_ax_fairygui_ScrollPane_getScrollStep);
        tolua_function(tolua_S,"setScrollStep",lua_ax_fairygui_ScrollPane_setScrollStep);
        tolua_function(tolua_S,"isSnapToItem",lua_ax_fairygui_ScrollPane_isSnapToItem);
        tolua_function(tolua_S,"setSnapToItem",lua_ax_fairygui_ScrollPane_setSnapToItem);
        tolua_function(tolua_S,"isPageMode",lua_ax_fairygui_ScrollPane_isPageMode);
        tolua_function(tolua_S,"setPageMode",lua_ax_fairygui_ScrollPane_setPageMode);
        tolua_function(tolua_S,"getPageController",lua_ax_fairygui_ScrollPane_getPageController);
        tolua_function(tolua_S,"setPageController",lua_ax_fairygui_ScrollPane_setPageController);
        tolua_function(tolua_S,"isMouseWheelEnabled",lua_ax_fairygui_ScrollPane_isMouseWheelEnabled);
        tolua_function(tolua_S,"setMouseWheelEnabled",lua_ax_fairygui_ScrollPane_setMouseWheelEnabled);
        tolua_function(tolua_S,"getDecelerationRate",lua_ax_fairygui_ScrollPane_getDecelerationRate);
        tolua_function(tolua_S,"setDecelerationRate",lua_ax_fairygui_ScrollPane_setDecelerationRate);
        tolua_function(tolua_S,"getPosX",lua_ax_fairygui_ScrollPane_getPosX);
        tolua_function(tolua_S,"setPosX",lua_ax_fairygui_ScrollPane_setPosX);
        tolua_function(tolua_S,"getPosY",lua_ax_fairygui_ScrollPane_getPosY);
        tolua_function(tolua_S,"setPosY",lua_ax_fairygui_ScrollPane_setPosY);
        tolua_function(tolua_S,"getPercX",lua_ax_fairygui_ScrollPane_getPercX);
        tolua_function(tolua_S,"setPercX",lua_ax_fairygui_ScrollPane_setPercX);
        tolua_function(tolua_S,"getPercY",lua_ax_fairygui_ScrollPane_getPercY);
        tolua_function(tolua_S,"setPercY",lua_ax_fairygui_ScrollPane_setPercY);
        tolua_function(tolua_S,"isBottomMost",lua_ax_fairygui_ScrollPane_isBottomMost);
        tolua_function(tolua_S,"isRightMost",lua_ax_fairygui_ScrollPane_isRightMost);
        tolua_function(tolua_S,"scrollLeft",lua_ax_fairygui_ScrollPane_scrollLeft);
        tolua_function(tolua_S,"scrollRight",lua_ax_fairygui_ScrollPane_scrollRight);
        tolua_function(tolua_S,"scrollUp",lua_ax_fairygui_ScrollPane_scrollUp);
        tolua_function(tolua_S,"scrollDown",lua_ax_fairygui_ScrollPane_scrollDown);
        tolua_function(tolua_S,"scrollTop",lua_ax_fairygui_ScrollPane_scrollTop);
        tolua_function(tolua_S,"scrollBottom",lua_ax_fairygui_ScrollPane_scrollBottom);
        tolua_function(tolua_S,"scrollToView",lua_ax_fairygui_ScrollPane_scrollToView);
        tolua_function(tolua_S,"isChildInView",lua_ax_fairygui_ScrollPane_isChildInView);
        tolua_function(tolua_S,"getPageX",lua_ax_fairygui_ScrollPane_getPageX);
        tolua_function(tolua_S,"setPageX",lua_ax_fairygui_ScrollPane_setPageX);
        tolua_function(tolua_S,"getPageY",lua_ax_fairygui_ScrollPane_getPageY);
        tolua_function(tolua_S,"setPageY",lua_ax_fairygui_ScrollPane_setPageY);
        tolua_function(tolua_S,"getScrollingPosX",lua_ax_fairygui_ScrollPane_getScrollingPosX);
        tolua_function(tolua_S,"getScrollingPosY",lua_ax_fairygui_ScrollPane_getScrollingPosY);
        tolua_function(tolua_S,"getContentSize",lua_ax_fairygui_ScrollPane_getContentSize);
        tolua_function(tolua_S,"getViewSize",lua_ax_fairygui_ScrollPane_getViewSize);
        tolua_function(tolua_S,"lockHeader",lua_ax_fairygui_ScrollPane_lockHeader);
        tolua_function(tolua_S,"lockFooter",lua_ax_fairygui_ScrollPane_lockFooter);
        tolua_function(tolua_S,"cancelDragging",lua_ax_fairygui_ScrollPane_cancelDragging);
        tolua_function(tolua_S,"getDraggingPane", lua_ax_fairygui_ScrollPane_getDraggingPane);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::ScrollPane).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.ScrollPane";
    g_typeCast[typeName] = "fgui.ScrollPane";
    return 1;
}

int lua_ax_fairygui_Transition_getOwner(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_getOwner'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_getOwner'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOwner();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:getOwner",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_getOwner'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_playReverse(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_playReverse'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.Transition:playReverse");

            if (!ok) { break; }
            double arg1;
            ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.Transition:playReverse");

            if (!ok) { break; }
            cobj->playReverse(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.Transition:playReverse");

            if (!ok) { break; }
            double arg1;
            ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.Transition:playReverse");

            if (!ok) { break; }
            std::function<void ()> arg2;
            do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;

            if (!ok) { break; }
            cobj->playReverse(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->playReverse();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::function<void ()> arg0;
            do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;

            if (!ok) { break; }
            cobj->playReverse(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.Transition:playReverse",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_playReverse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_changePlayTimes(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_changePlayTimes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.Transition:changePlayTimes");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_changePlayTimes'", nullptr);
            return 0;
        }
        cobj->changePlayTimes(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:changePlayTimes",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_changePlayTimes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_stop(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_stop'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            bool arg0;
            ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.Transition:stop");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.Transition:stop");

            if (!ok) { break; }
            cobj->stop(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->stop();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.Transition:stop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_stop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setAutoPlay(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setAutoPlay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        bool arg0;
        int arg1;
        double arg2;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.Transition:setAutoPlay");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.Transition:setAutoPlay");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.Transition:setAutoPlay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setAutoPlay'", nullptr);
            return 0;
        }
        cobj->setAutoPlay(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setAutoPlay",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setAutoPlay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setPaused(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setPaused'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.Transition:setPaused");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setPaused'", nullptr);
            return 0;
        }
        cobj->setPaused(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setPaused",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setPaused'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        ax::ValueVector arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:setValue");

        ok &= luaval_to_ccvaluevector(tolua_S, 3, &arg1, "fgui.Transition:setValue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setValue'", nullptr);
            return 0;
        }
        cobj->setValue(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setValue",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setHook(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setHook'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::function<void ()> arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:setHook");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setHook'", nullptr);
            return 0;
        }
        cobj->setHook(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setHook",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setHook'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_clearHooks(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_clearHooks'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_clearHooks'", nullptr);
            return 0;
        }
        cobj->clearHooks();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:clearHooks",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_clearHooks'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setTarget(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        fairygui::GObject* arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:setTarget");

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.Transition:setTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setTarget'", nullptr);
            return 0;
        }
        cobj->setTarget(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setTarget",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setDuration(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        double arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:setDuration");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.Transition:setDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setDuration'", nullptr);
            return 0;
        }
        cobj->setDuration(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setDuration",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_getLabelTime(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_getLabelTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:getLabelTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_getLabelTime'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLabelTime(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:getLabelTime",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_getLabelTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_getTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_getTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_getTimeScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTimeScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:getTimeScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_getTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_setTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_setTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.Transition:setTimeScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_setTimeScale'", nullptr);
            return 0;
        }
        cobj->setTimeScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:setTimeScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_setTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_updateFromRelations(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_updateFromRelations'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string arg0;
        double arg1;
        double arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.Transition:updateFromRelations");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.Transition:updateFromRelations");

        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.Transition:updateFromRelations");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_updateFromRelations'", nullptr);
            return 0;
        }
        cobj->updateFromRelations(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:updateFromRelations",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_updateFromRelations'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_onOwnerAddedToStage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_onOwnerAddedToStage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_onOwnerAddedToStage'", nullptr);
            return 0;
        }
        cobj->onOwnerAddedToStage();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:onOwnerAddedToStage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_onOwnerAddedToStage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_onOwnerRemovedFromStage(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Transition",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Transition*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Transition_onOwnerRemovedFromStage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_onOwnerRemovedFromStage'", nullptr);
            return 0;
        }
        cobj->onOwnerRemovedFromStage();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:onOwnerRemovedFromStage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_onOwnerRemovedFromStage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Transition_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Transition* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GComponent* arg0;

        ok &= luaval_to_object<fairygui::GComponent>(tolua_S, 2, "fgui.GComponent",&arg0, "fgui.Transition:Transition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Transition_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::Transition(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.Transition");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Transition:Transition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Transition_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_Transition_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Transition)");
    return 0;
}

int lua_register_ax_fairygui_Transition(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.Transition");
    tolua_cclass(tolua_S,"Transition","fgui.Transition","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Transition");
        tolua_function(tolua_S,"new",lua_ax_fairygui_Transition_constructor);
        tolua_function(tolua_S,"getOwner",lua_ax_fairygui_Transition_getOwner);
        tolua_function(tolua_S,"isPlaying",lua_ax_fairygui_Transition_isPlaying);
        tolua_function(tolua_S,"playReverse",lua_ax_fairygui_Transition_playReverse);
        tolua_function(tolua_S,"changePlayTimes",lua_ax_fairygui_Transition_changePlayTimes);
        tolua_function(tolua_S,"stop",lua_ax_fairygui_Transition_stop);
        tolua_function(tolua_S,"setAutoPlay",lua_ax_fairygui_Transition_setAutoPlay);
        tolua_function(tolua_S,"setPaused",lua_ax_fairygui_Transition_setPaused);
        tolua_function(tolua_S,"setValue",lua_ax_fairygui_Transition_setValue);
        tolua_function(tolua_S,"setHook",lua_ax_fairygui_Transition_setHook);
        tolua_function(tolua_S,"clearHooks",lua_ax_fairygui_Transition_clearHooks);
        tolua_function(tolua_S,"setTarget",lua_ax_fairygui_Transition_setTarget);
        tolua_function(tolua_S,"setDuration",lua_ax_fairygui_Transition_setDuration);
        tolua_function(tolua_S,"getLabelTime",lua_ax_fairygui_Transition_getLabelTime);
        tolua_function(tolua_S,"getTimeScale",lua_ax_fairygui_Transition_getTimeScale);
        tolua_function(tolua_S,"setTimeScale",lua_ax_fairygui_Transition_setTimeScale);
        tolua_function(tolua_S,"updateFromRelations",lua_ax_fairygui_Transition_updateFromRelations);
        tolua_function(tolua_S,"onOwnerAddedToStage",lua_ax_fairygui_Transition_onOwnerAddedToStage);
        tolua_function(tolua_S,"onOwnerRemovedFromStage",lua_ax_fairygui_Transition_onOwnerRemovedFromStage);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::Transition).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.Transition";
    g_typeCast[typeName] = "fgui.Transition";
    return 1;
}

int lua_ax_fairygui_GComponent_addChild(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_addChild'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:addChild");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_addChild'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addChild(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:addChild",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_addChild'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_addChildAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_addChildAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        int arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:addChildAt");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:addChildAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_addChildAt'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addChildAt(arg0, arg1);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:addChildAt",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_addChildAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_removeChild(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_removeChild'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:removeChild");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_removeChild'", nullptr);
            return 0;
        }
        cobj->removeChild(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:removeChild",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_removeChild'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_removeChildAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_removeChildAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:removeChildAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_removeChildAt'", nullptr);
            return 0;
        }
        cobj->removeChildAt(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:removeChildAt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_removeChildAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_removeChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_removeChildren'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:removeChildren");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:removeChildren");

            if (!ok) { break; }
            cobj->removeChildren(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->removeChildren();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GComponent:removeChildren",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_removeChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:getChildAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildAt'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildAt(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildAt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChild(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChild'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComponent:getChild");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChild'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChild(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChild",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChild'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildByPath(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildByPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComponent:getChildByPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildByPath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildByPath(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildByPath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildByPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildInGroup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildInGroup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const fairygui::GGroup* arg0;
        std::string arg1;

        ok &= luaval_to_object<const fairygui::GGroup>(tolua_S, 2, "fgui.GGroup",&arg0, "fgui.GComponent:getChildInGroup");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "fgui.GComponent:getChildInGroup");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildInGroup'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildInGroup(arg0, arg1);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildInGroup",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildInGroup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildById(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildById'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComponent:getChildById");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildById'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildById(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildById",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildById'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildren'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildren();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildren",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const fairygui::GObject* arg0;

        ok &= luaval_to_object<const fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:getChildIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getChildIndex(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setChildIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setChildIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        int arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:setChildIndex");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:setChildIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setChildIndex'", nullptr);
            return 0;
        }
        cobj->setChildIndex(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setChildIndex",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setChildIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setChildIndexBefore(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setChildIndexBefore'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        int arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:setChildIndexBefore");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:setChildIndexBefore");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setChildIndexBefore'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setChildIndexBefore(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setChildIndexBefore",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setChildIndexBefore'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_swapChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_swapChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        fairygui::GObject* arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:swapChildren");

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.GComponent:swapChildren");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_swapChildren'", nullptr);
            return 0;
        }
        cobj->swapChildren(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:swapChildren",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_swapChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_swapChildrenAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_swapChildrenAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        int arg0;
        int arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:swapChildrenAt");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:swapChildrenAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_swapChildrenAt'", nullptr);
            return 0;
        }
        cobj->swapChildrenAt(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:swapChildrenAt",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_swapChildrenAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_numChildren(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_numChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_numChildren'", nullptr);
            return 0;
        }
        auto&& ret = cobj->numChildren();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:numChildren",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_numChildren'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_isAncestorOf(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_isAncestorOf'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const fairygui::GObject* arg0;

        ok &= luaval_to_object<const fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:isAncestorOf");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_isAncestorOf'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isAncestorOf(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:isAncestorOf",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_isAncestorOf'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_isChildInView(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_isChildInView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:isChildInView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_isChildInView'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isChildInView(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:isChildInView",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_isChildInView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getFirstChildInView(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getFirstChildInView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getFirstChildInView'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFirstChildInView();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getFirstChildInView",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getFirstChildInView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_addController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_addController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GComponent:addController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_addController'", nullptr);
            return 0;
        }
        cobj->addController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:addController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_addController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getControllerAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getControllerAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:getControllerAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getControllerAt'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getControllerAt(arg0);
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getControllerAt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getControllerAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComponent:getController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getController(arg0);
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getControllers(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getControllers'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getControllers'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getControllers();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getControllers",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getControllers'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_removeController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_removeController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GComponent:removeController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_removeController'", nullptr);
            return 0;
        }
        cobj->removeController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:removeController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_removeController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_applyController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_applyController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GComponent:applyController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_applyController'", nullptr);
            return 0;
        }
        cobj->applyController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:applyController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_applyController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_applyAllControllers(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_applyAllControllers'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_applyAllControllers'", nullptr);
            return 0;
        }
        cobj->applyAllControllers();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:applyAllControllers",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_applyAllControllers'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getTransition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getTransition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComponent:getTransition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getTransition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTransition(arg0);
        object_to_luaval<fairygui::Transition>(tolua_S, "fgui.Transition",(fairygui::Transition*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getTransition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getTransition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getTransitionAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getTransitionAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:getTransitionAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getTransitionAt'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTransitionAt(arg0);
        object_to_luaval<fairygui::Transition>(tolua_S, "fgui.Transition",(fairygui::Transition*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getTransitionAt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getTransitionAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getTransitions(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getTransitions'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getTransitions'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTransitions();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getTransitions",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getTransitions'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getOpaque(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getOpaque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getOpaque'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOpaque();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getOpaque",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getOpaque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setOpaque(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setOpaque'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GComponent:setOpaque");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setOpaque'", nullptr);
            return 0;
        }
        cobj->setOpaque(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setOpaque",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setOpaque'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getMargin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getMargin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getMargin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMargin();
        margin_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getMargin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getMargin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setMargin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setMargin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::Margin arg0;

        ok &= luaval_to_margin(tolua_S, 2, &arg0, "fgui.GComponent:setMargin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setMargin'", nullptr);
            return 0;
        }
        cobj->setMargin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setMargin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setMargin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getChildrenRenderOrder(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getChildrenRenderOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getChildrenRenderOrder'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getChildrenRenderOrder();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getChildrenRenderOrder",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getChildrenRenderOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setChildrenRenderOrder(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setChildrenRenderOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ChildrenRenderOrder arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:setChildrenRenderOrder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setChildrenRenderOrder'", nullptr);
            return 0;
        }
        cobj->setChildrenRenderOrder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setChildrenRenderOrder",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setChildrenRenderOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getApexIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getApexIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getApexIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getApexIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getApexIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getApexIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setApexIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setApexIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComponent:setApexIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setApexIndex'", nullptr);
            return 0;
        }
        cobj->setApexIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setApexIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setApexIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getMask(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getMask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMask();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getMask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getMask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setMask(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setMask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "fgui.GComponent:setMask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setMask'", nullptr);
            return 0;
        }
        cobj->setMask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Node* arg0;
        bool arg1;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "fgui.GComponent:setMask");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GComponent:setMask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setMask'", nullptr);
            return 0;
        }
        cobj->setMask(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setMask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setMask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getHitArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getHitArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getHitArea'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getHitArea();
        object_to_luaval<fairygui::IHitTest>(tolua_S, "fgui.IHitTest",(fairygui::IHitTest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getHitArea",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getHitArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setHitArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setHitArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::IHitTest* arg0;

        ok &= luaval_to_object<fairygui::IHitTest>(tolua_S, 2, "fgui.IHitTest",&arg0, "fgui.GComponent:setHitArea");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setHitArea'", nullptr);
            return 0;
        }
        cobj->setHitArea(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setHitArea",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setHitArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getScrollPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getScrollPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getScrollPane'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getScrollPane();
        object_to_luaval<fairygui::ScrollPane>(tolua_S, "fgui.ScrollPane",(fairygui::ScrollPane*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getScrollPane",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getScrollPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getViewWidth(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getViewWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getViewWidth'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getViewWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getViewWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getViewWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setViewWidth(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setViewWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GComponent:setViewWidth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setViewWidth'", nullptr);
            return 0;
        }
        cobj->setViewWidth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setViewWidth",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setViewWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getViewHeight(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getViewHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getViewHeight'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getViewHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getViewHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getViewHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setViewHeight(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setViewHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GComponent:setViewHeight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setViewHeight'", nullptr);
            return 0;
        }
        cobj->setViewHeight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setViewHeight",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setViewHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_setBoundsChangedFlag(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_setBoundsChangedFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_setBoundsChangedFlag'", nullptr);
            return 0;
        }
        cobj->setBoundsChangedFlag();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:setBoundsChangedFlag",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_setBoundsChangedFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_ensureBoundsCorrect(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_ensureBoundsCorrect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_ensureBoundsCorrect'", nullptr);
            return 0;
        }
        cobj->ensureBoundsCorrect();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:ensureBoundsCorrect",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_ensureBoundsCorrect'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_getSnappingPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_getSnappingPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GComponent:getSnappingPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_getSnappingPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSnappingPosition(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:getSnappingPosition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_getSnappingPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_childSortingOrderChanged(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_childSortingOrderChanged'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        fairygui::GObject* arg0;
        int arg1;
        int arg2;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:childSortingOrderChanged");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GComponent:childSortingOrderChanged");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GComponent:childSortingOrderChanged");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_childSortingOrderChanged'", nullptr);
            return 0;
        }
        cobj->childSortingOrderChanged(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:childSortingOrderChanged",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_childSortingOrderChanged'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_childStateChanged(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_childStateChanged'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:childStateChanged");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_childStateChanged'", nullptr);
            return 0;
        }
        cobj->childStateChanged(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:childStateChanged",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_childStateChanged'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_adjustRadioGroupDepth(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComponent*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComponent_adjustRadioGroupDepth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::GObject* arg0;
        fairygui::GController* arg1;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GComponent:adjustRadioGroupDepth");

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 3, "fgui.GController",&arg1, "fgui.GComponent:adjustRadioGroupDepth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_adjustRadioGroupDepth'", nullptr);
            return 0;
        }
        cobj->adjustRadioGroupDepth(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:adjustRadioGroupDepth",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_adjustRadioGroupDepth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComponent_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GComponent",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GComponent::create();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GComponent:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GComponent_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComponent* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComponent_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GComponent();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GComponent");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComponent:GComponent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComponent_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GComponent_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GComponent)");
    return 0;
}

int lua_register_ax_fairygui_GComponent(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GComponent");
    tolua_cclass(tolua_S,"GComponent","fgui.GComponent","fgui.GObject",nullptr);

    tolua_beginmodule(tolua_S,"GComponent");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GComponent_constructor);
        tolua_function(tolua_S,"addChild",lua_ax_fairygui_GComponent_addChild);
        tolua_function(tolua_S,"addChildAt",lua_ax_fairygui_GComponent_addChildAt);
        tolua_function(tolua_S,"removeChild",lua_ax_fairygui_GComponent_removeChild);
        tolua_function(tolua_S,"removeChildAt",lua_ax_fairygui_GComponent_removeChildAt);
        tolua_function(tolua_S,"removeChildren",lua_ax_fairygui_GComponent_removeChildren);
        tolua_function(tolua_S,"getChildAt",lua_ax_fairygui_GComponent_getChildAt);
        tolua_function(tolua_S,"getChild",lua_ax_fairygui_GComponent_getChild);
        tolua_function(tolua_S,"getChildByPath",lua_ax_fairygui_GComponent_getChildByPath);
        tolua_function(tolua_S,"getChildInGroup",lua_ax_fairygui_GComponent_getChildInGroup);
        tolua_function(tolua_S,"getChildById",lua_ax_fairygui_GComponent_getChildById);
        tolua_function(tolua_S,"getChildren",lua_ax_fairygui_GComponent_getChildren);
        tolua_function(tolua_S,"getChildIndex",lua_ax_fairygui_GComponent_getChildIndex);
        tolua_function(tolua_S,"setChildIndex",lua_ax_fairygui_GComponent_setChildIndex);
        tolua_function(tolua_S,"setChildIndexBefore",lua_ax_fairygui_GComponent_setChildIndexBefore);
        tolua_function(tolua_S,"swapChildren",lua_ax_fairygui_GComponent_swapChildren);
        tolua_function(tolua_S,"swapChildrenAt",lua_ax_fairygui_GComponent_swapChildrenAt);
        tolua_function(tolua_S,"numChildren",lua_ax_fairygui_GComponent_numChildren);
        tolua_function(tolua_S,"isAncestorOf",lua_ax_fairygui_GComponent_isAncestorOf);
        tolua_function(tolua_S,"isChildInView",lua_ax_fairygui_GComponent_isChildInView);
        tolua_function(tolua_S,"getFirstChildInView",lua_ax_fairygui_GComponent_getFirstChildInView);
        tolua_function(tolua_S,"addController",lua_ax_fairygui_GComponent_addController);
        tolua_function(tolua_S,"getControllerAt",lua_ax_fairygui_GComponent_getControllerAt);
        tolua_function(tolua_S,"getController",lua_ax_fairygui_GComponent_getController);
        tolua_function(tolua_S,"getControllers",lua_ax_fairygui_GComponent_getControllers);
        tolua_function(tolua_S,"removeController",lua_ax_fairygui_GComponent_removeController);
        tolua_function(tolua_S,"applyController",lua_ax_fairygui_GComponent_applyController);
        tolua_function(tolua_S,"applyAllControllers",lua_ax_fairygui_GComponent_applyAllControllers);
        tolua_function(tolua_S,"getTransition",lua_ax_fairygui_GComponent_getTransition);
        tolua_function(tolua_S,"getTransitionAt",lua_ax_fairygui_GComponent_getTransitionAt);
        tolua_function(tolua_S,"getTransitions",lua_ax_fairygui_GComponent_getTransitions);
        tolua_function(tolua_S,"getOpaque",lua_ax_fairygui_GComponent_getOpaque);
        tolua_function(tolua_S,"setOpaque",lua_ax_fairygui_GComponent_setOpaque);
        tolua_function(tolua_S,"getMargin",lua_ax_fairygui_GComponent_getMargin);
        tolua_function(tolua_S,"setMargin",lua_ax_fairygui_GComponent_setMargin);
        tolua_function(tolua_S,"getChildrenRenderOrder",lua_ax_fairygui_GComponent_getChildrenRenderOrder);
        tolua_function(tolua_S,"setChildrenRenderOrder",lua_ax_fairygui_GComponent_setChildrenRenderOrder);
        tolua_function(tolua_S,"getApexIndex",lua_ax_fairygui_GComponent_getApexIndex);
        tolua_function(tolua_S,"setApexIndex",lua_ax_fairygui_GComponent_setApexIndex);
        tolua_function(tolua_S,"getMask",lua_ax_fairygui_GComponent_getMask);
        tolua_function(tolua_S,"setMask",lua_ax_fairygui_GComponent_setMask);
        tolua_function(tolua_S,"getHitArea",lua_ax_fairygui_GComponent_getHitArea);
        tolua_function(tolua_S,"setHitArea",lua_ax_fairygui_GComponent_setHitArea);
        tolua_function(tolua_S,"getScrollPane",lua_ax_fairygui_GComponent_getScrollPane);
        tolua_function(tolua_S,"getViewWidth",lua_ax_fairygui_GComponent_getViewWidth);
        tolua_function(tolua_S,"setViewWidth",lua_ax_fairygui_GComponent_setViewWidth);
        tolua_function(tolua_S,"getViewHeight",lua_ax_fairygui_GComponent_getViewHeight);
        tolua_function(tolua_S,"setViewHeight",lua_ax_fairygui_GComponent_setViewHeight);
        tolua_function(tolua_S,"setBoundsChangedFlag",lua_ax_fairygui_GComponent_setBoundsChangedFlag);
        tolua_function(tolua_S,"ensureBoundsCorrect",lua_ax_fairygui_GComponent_ensureBoundsCorrect);
        tolua_function(tolua_S,"getSnappingPosition",lua_ax_fairygui_GComponent_getSnappingPosition);
        tolua_function(tolua_S,"childSortingOrderChanged",lua_ax_fairygui_GComponent_childSortingOrderChanged);
        tolua_function(tolua_S,"childStateChanged",lua_ax_fairygui_GComponent_childStateChanged);
        tolua_function(tolua_S,"adjustRadioGroupDepth",lua_ax_fairygui_GComponent_adjustRadioGroupDepth);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GComponent_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GComponent).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GComponent";
    g_typeCast[typeName] = "fgui.GComponent";
    return 1;
}

int lua_ax_fairygui_GButton_getTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getTitle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitle();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getTitle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GButton:setTitle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setTitle'", nullptr);
            return 0;
        }
        cobj->setTitle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setTitle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getSelectedTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getSelectedTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getSelectedTitle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedTitle();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getSelectedTitle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getSelectedTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setSelectedTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setSelectedTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GButton:setSelectedTitle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setSelectedTitle'", nullptr);
            return 0;
        }
        cobj->setSelectedTitle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setSelectedTitle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setSelectedTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getSelectedIcon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getSelectedIcon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getSelectedIcon'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedIcon();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getSelectedIcon",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getSelectedIcon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setSelectedIcon(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setSelectedIcon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GButton:setSelectedIcon");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setSelectedIcon'", nullptr);
            return 0;
        }
        cobj->setSelectedIcon(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setSelectedIcon",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setSelectedIcon'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getTitleColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitleColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getTitleColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getTitleColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GButton:setTitleColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setTitleColor'", nullptr);
            return 0;
        }
        cobj->setTitleColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setTitleColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setTitleColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getTitleFontSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitleFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getTitleFontSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GButton:setTitleFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setTitleFontSize'", nullptr);
            return 0;
        }
        cobj->setTitleFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setTitleFontSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_isSelected(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_isSelected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_isSelected'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isSelected();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:isSelected",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_isSelected'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setSelected(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setSelected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GButton:setSelected");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setSelected'", nullptr);
            return 0;
        }
        cobj->setSelected(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setSelected",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setSelected'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getRelatedController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getRelatedController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getRelatedController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRelatedController();
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getRelatedController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getRelatedController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setRelatedController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setRelatedController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GButton:setRelatedController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setRelatedController'", nullptr);
            return 0;
        }
        cobj->setRelatedController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setRelatedController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setRelatedController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_isChangeStateOnClick(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_isChangeStateOnClick'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_isChangeStateOnClick'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isChangeStateOnClick();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:isChangeStateOnClick",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_isChangeStateOnClick'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_setChangeStateOnClick(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_setChangeStateOnClick'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GButton:setChangeStateOnClick");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_setChangeStateOnClick'", nullptr);
            return 0;
        }
        cobj->setChangeStateOnClick(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:setChangeStateOnClick",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_setChangeStateOnClick'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_getTextField(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GButton*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GButton_getTextField'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_getTextField'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTextField();
        object_to_luaval<fairygui::GTextField>(tolua_S, "fgui.GTextField",(fairygui::GTextField*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:getTextField",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_getTextField'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GButton_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GButton",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GButton::create();
        object_to_luaval<fairygui::GButton>(tolua_S, "fgui.GButton",(fairygui::GButton*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GButton:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GButton_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GButton* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GButton_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GButton();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GButton");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GButton:GButton",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GButton_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GButton_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GButton)");
    return 0;
}

int lua_register_ax_fairygui_GButton(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GButton");
    tolua_cclass(tolua_S,"GButton","fgui.GButton","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GButton");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GButton_constructor);
        tolua_function(tolua_S,"getTitle",lua_ax_fairygui_GButton_getTitle);
        tolua_function(tolua_S,"setTitle",lua_ax_fairygui_GButton_setTitle);
        tolua_function(tolua_S,"getSelectedTitle",lua_ax_fairygui_GButton_getSelectedTitle);
        tolua_function(tolua_S,"setSelectedTitle",lua_ax_fairygui_GButton_setSelectedTitle);
        tolua_function(tolua_S,"getSelectedIcon",lua_ax_fairygui_GButton_getSelectedIcon);
        tolua_function(tolua_S,"setSelectedIcon",lua_ax_fairygui_GButton_setSelectedIcon);
        tolua_function(tolua_S,"getTitleColor",lua_ax_fairygui_GButton_getTitleColor);
        tolua_function(tolua_S,"setTitleColor",lua_ax_fairygui_GButton_setTitleColor);
        tolua_function(tolua_S,"getTitleFontSize",lua_ax_fairygui_GButton_getTitleFontSize);
        tolua_function(tolua_S,"setTitleFontSize",lua_ax_fairygui_GButton_setTitleFontSize);
        tolua_function(tolua_S,"isSelected",lua_ax_fairygui_GButton_isSelected);
        tolua_function(tolua_S,"setSelected",lua_ax_fairygui_GButton_setSelected);
        tolua_function(tolua_S,"getRelatedController",lua_ax_fairygui_GButton_getRelatedController);
        tolua_function(tolua_S,"setRelatedController",lua_ax_fairygui_GButton_setRelatedController);
        tolua_function(tolua_S,"isChangeStateOnClick",lua_ax_fairygui_GButton_isChangeStateOnClick);
        tolua_function(tolua_S,"setChangeStateOnClick",lua_ax_fairygui_GButton_setChangeStateOnClick);
        tolua_function(tolua_S,"getTextField",lua_ax_fairygui_GButton_getTextField);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GButton_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GButton).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GButton";
    g_typeCast[typeName] = "fgui.GButton";
    return 1;
}

int lua_ax_fairygui_GObjectPool_getObject(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObjectPool* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObjectPool",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObjectPool*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObjectPool_getObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GObjectPool:getObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObjectPool_getObject'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getObject(arg0);
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObjectPool:getObject",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObjectPool_getObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObjectPool_returnObject(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObjectPool* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GObjectPool",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GObjectPool*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GObjectPool_returnObject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GObjectPool:returnObject");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObjectPool_returnObject'", nullptr);
            return 0;
        }
        cobj->returnObject(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObjectPool:returnObject",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObjectPool_returnObject'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GObjectPool_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObjectPool* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GObjectPool_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GObjectPool();
        tolua_pushusertype(tolua_S,(void*)cobj,"fgui.GObjectPool");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObjectPool:GObjectPool",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GObjectPool_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GObjectPool_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GObjectPool)");
    return 0;
}

int lua_register_ax_fairygui_GObjectPool(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GObjectPool");
    tolua_cclass(tolua_S,"GObjectPool","fgui.GObjectPool","",nullptr);

    tolua_beginmodule(tolua_S,"GObjectPool");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GObjectPool_constructor);
        tolua_function(tolua_S,"getObject",lua_ax_fairygui_GObjectPool_getObject);
        tolua_function(tolua_S,"returnObject",lua_ax_fairygui_GObjectPool_returnObject);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GObjectPool).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GObjectPool";
    g_typeCast[typeName] = "fgui.GObjectPool";
    return 1;
}

int lua_ax_fairygui_GList_getDefaultItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getDefaultItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getDefaultItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDefaultItem();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getDefaultItem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getDefaultItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setDefaultItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setDefaultItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GList:setDefaultItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setDefaultItem'", nullptr);
            return 0;
        }
        cobj->setDefaultItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setDefaultItem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setDefaultItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getLayout(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getLayout'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getLayout();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setLayout(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ListLayoutType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setLayout");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setLayout'", nullptr);
            return 0;
        }
        cobj->setLayout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setLayout",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getLineCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getLineCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getLineCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLineCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getLineCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getLineCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setLineCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setLineCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setLineCount");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setLineCount'", nullptr);
            return 0;
        }
        cobj->setLineCount(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setLineCount",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setLineCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getColumnCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getColumnCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getColumnCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColumnCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getColumnCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getColumnCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setColumnCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setColumnCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setColumnCount");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setColumnCount'", nullptr);
            return 0;
        }
        cobj->setColumnCount(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setColumnCount",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setColumnCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getColumnGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getColumnGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getColumnGap'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getColumnGap();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getColumnGap",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getColumnGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setColumnGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setColumnGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setColumnGap");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setColumnGap'", nullptr);
            return 0;
        }
        cobj->setColumnGap(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setColumnGap",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setColumnGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getLineGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getLineGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getLineGap'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLineGap();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getLineGap",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getLineGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setLineGap(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setLineGap'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setLineGap");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setLineGap'", nullptr);
            return 0;
        }
        cobj->setLineGap(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setLineGap",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setLineGap'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getAlign'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getAlign();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getAlign",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::TextHAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setAlign");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setAlign'", nullptr);
            return 0;
        }
        cobj->setAlign(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setAlign",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getVerticalAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getVerticalAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getVerticalAlign'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getVerticalAlign();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getVerticalAlign",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getVerticalAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setVerticalAlign(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setVerticalAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::TextVAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setVerticalAlign");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setVerticalAlign'", nullptr);
            return 0;
        }
        cobj->setVerticalAlign(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setVerticalAlign",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setVerticalAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getAutoResizeItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getAutoResizeItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getAutoResizeItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAutoResizeItem();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getAutoResizeItem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getAutoResizeItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setAutoResizeItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setAutoResizeItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GList:setAutoResizeItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setAutoResizeItem'", nullptr);
            return 0;
        }
        cobj->setAutoResizeItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setAutoResizeItem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setAutoResizeItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getSelectionMode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getSelectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getSelectionMode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getSelectionMode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getSelectionMode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getSelectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setSelectionMode(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setSelectionMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ListSelectionMode arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setSelectionMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setSelectionMode'", nullptr);
            return 0;
        }
        cobj->setSelectionMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setSelectionMode",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setSelectionMode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getItemPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getItemPool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getItemPool'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItemPool();
        object_to_luaval<fairygui::GObjectPool>(tolua_S, "fgui.GObjectPool",(fairygui::GObjectPool*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getItemPool",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getItemPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getFromPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getFromPool'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GList:getFromPool");

            if (!ok) { break; }
            fairygui::GObject* ret = cobj->getFromPool(arg0);
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            fairygui::GObject* ret = cobj->getFromPool();
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GList:getFromPool",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getFromPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_returnToPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_returnToPool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GList:returnToPool");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_returnToPool'", nullptr);
            return 0;
        }
        cobj->returnToPool(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:returnToPool",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_returnToPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_addItemFromPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_addItemFromPool'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GList:addItemFromPool");

            if (!ok) { break; }
            fairygui::GObject* ret = cobj->addItemFromPool(arg0);
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            fairygui::GObject* ret = cobj->addItemFromPool();
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GList:addItemFromPool",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_addItemFromPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_removeChildToPoolAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_removeChildToPoolAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:removeChildToPoolAt");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_removeChildToPoolAt'", nullptr);
            return 0;
        }
        cobj->removeChildToPoolAt(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:removeChildToPoolAt",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_removeChildToPoolAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_removeChildToPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_removeChildToPool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GList:removeChildToPool");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_removeChildToPool'", nullptr);
            return 0;
        }
        cobj->removeChildToPool(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:removeChildToPool",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_removeChildToPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_removeChildrenToPool(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_removeChildrenToPool'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:removeChildrenToPool");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GList:removeChildrenToPool");

            if (!ok) { break; }
            cobj->removeChildrenToPool(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->removeChildrenToPool();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GList:removeChildrenToPool",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_removeChildrenToPool'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getSelectedIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getSelectedIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setSelectedIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setSelectedIndex'", nullptr);
            return 0;
        }
        cobj->setSelectedIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setSelectedIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getSelection(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getSelection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::vector<int> arg0;

        ok &= luaval_to_std_vector_int(tolua_S, 2, &arg0, "fgui.GList:getSelection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getSelection'", nullptr);
            return 0;
        }
        cobj->getSelection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getSelection",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getSelection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_addSelection(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_addSelection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:addSelection");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GList:addSelection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_addSelection'", nullptr);
            return 0;
        }
        cobj->addSelection(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:addSelection",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_addSelection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_removeSelection(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_removeSelection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:removeSelection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_removeSelection'", nullptr);
            return 0;
        }
        cobj->removeSelection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:removeSelection",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_removeSelection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_clearSelection(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_clearSelection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_clearSelection'", nullptr);
            return 0;
        }
        cobj->clearSelection();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:clearSelection",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_clearSelection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_selectAll(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_selectAll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_selectAll'", nullptr);
            return 0;
        }
        cobj->selectAll();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:selectAll",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_selectAll'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_selectReverse(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_selectReverse'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_selectReverse'", nullptr);
            return 0;
        }
        cobj->selectReverse();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:selectReverse",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_selectReverse'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_handleArrowKey(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_handleArrowKey'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:handleArrowKey");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_handleArrowKey'", nullptr);
            return 0;
        }
        cobj->handleArrowKey(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:handleArrowKey",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_handleArrowKey'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_resizeToFit(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_resizeToFit'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:resizeToFit");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GList:resizeToFit");

            if (!ok) { break; }
            cobj->resizeToFit(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:resizeToFit");

            if (!ok) { break; }
            cobj->resizeToFit(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GList:resizeToFit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_resizeToFit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_scrollToView(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_scrollToView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:scrollToView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_scrollToView'", nullptr);
            return 0;
        }
        cobj->scrollToView(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:scrollToView");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GList:scrollToView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_scrollToView'", nullptr);
            return 0;
        }
        cobj->scrollToView(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        int arg0;
        bool arg1;
        bool arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:scrollToView");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GList:scrollToView");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.GList:scrollToView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_scrollToView'", nullptr);
            return 0;
        }
        cobj->scrollToView(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:scrollToView",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_scrollToView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getSelectionController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getSelectionController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getSelectionController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectionController();
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getSelectionController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getSelectionController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setSelectionController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setSelectionController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GList:setSelectionController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setSelectionController'", nullptr);
            return 0;
        }
        cobj->setSelectionController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setSelectionController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setSelectionController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setVirtual(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setVirtual'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setVirtual'", nullptr);
            return 0;
        }
        cobj->setVirtual();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setVirtual",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setVirtual'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setVirtualAndLoop(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setVirtualAndLoop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setVirtualAndLoop'", nullptr);
            return 0;
        }
        cobj->setVirtualAndLoop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setVirtualAndLoop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setVirtualAndLoop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_isVirtual(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_isVirtual'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_isVirtual'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isVirtual();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:isVirtual",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_isVirtual'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_refreshVirtualList(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_refreshVirtualList'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_refreshVirtualList'", nullptr);
            return 0;
        }
        cobj->refreshVirtualList();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:refreshVirtualList",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_refreshVirtualList'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_getNumItems(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_getNumItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_getNumItems'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getNumItems();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:getNumItems",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_getNumItems'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_setNumItems(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_setNumItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:setNumItems");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_setNumItems'", nullptr);
            return 0;
        }
        cobj->setNumItems(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:setNumItems",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_setNumItems'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_childIndexToItemIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_childIndexToItemIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:childIndexToItemIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_childIndexToItemIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->childIndexToItemIndex(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:childIndexToItemIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_childIndexToItemIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_itemIndexToChildIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GList*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GList_itemIndexToChildIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GList:itemIndexToChildIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_itemIndexToChildIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->itemIndexToChildIndex(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:itemIndexToChildIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_itemIndexToChildIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GList_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GList",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GList::create();
        object_to_luaval<fairygui::GList>(tolua_S, "fgui.GList",(fairygui::GList*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GList:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GList_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GList* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GList_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GList();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GList");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GList:GList",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GList_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GList_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GList)");
    return 0;
}

int lua_register_ax_fairygui_GList(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GList");
    tolua_cclass(tolua_S,"GList","fgui.GList","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GList");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GList_constructor);
        tolua_function(tolua_S,"getDefaultItem",lua_ax_fairygui_GList_getDefaultItem);
        tolua_function(tolua_S,"setDefaultItem",lua_ax_fairygui_GList_setDefaultItem);
        tolua_function(tolua_S,"getLayout",lua_ax_fairygui_GList_getLayout);
        tolua_function(tolua_S,"setLayout",lua_ax_fairygui_GList_setLayout);
        tolua_function(tolua_S,"getLineCount",lua_ax_fairygui_GList_getLineCount);
        tolua_function(tolua_S,"setLineCount",lua_ax_fairygui_GList_setLineCount);
        tolua_function(tolua_S,"getColumnCount",lua_ax_fairygui_GList_getColumnCount);
        tolua_function(tolua_S,"setColumnCount",lua_ax_fairygui_GList_setColumnCount);
        tolua_function(tolua_S,"getColumnGap",lua_ax_fairygui_GList_getColumnGap);
        tolua_function(tolua_S,"setColumnGap",lua_ax_fairygui_GList_setColumnGap);
        tolua_function(tolua_S,"getLineGap",lua_ax_fairygui_GList_getLineGap);
        tolua_function(tolua_S,"setLineGap",lua_ax_fairygui_GList_setLineGap);
        tolua_function(tolua_S,"getAlign",lua_ax_fairygui_GList_getAlign);
        tolua_function(tolua_S,"setAlign",lua_ax_fairygui_GList_setAlign);
        tolua_function(tolua_S,"getVerticalAlign",lua_ax_fairygui_GList_getVerticalAlign);
        tolua_function(tolua_S,"setVerticalAlign",lua_ax_fairygui_GList_setVerticalAlign);
        tolua_function(tolua_S,"getAutoResizeItem",lua_ax_fairygui_GList_getAutoResizeItem);
        tolua_function(tolua_S,"setAutoResizeItem",lua_ax_fairygui_GList_setAutoResizeItem);
        tolua_function(tolua_S,"getSelectionMode",lua_ax_fairygui_GList_getSelectionMode);
        tolua_function(tolua_S,"setSelectionMode",lua_ax_fairygui_GList_setSelectionMode);
        tolua_function(tolua_S,"getItemPool",lua_ax_fairygui_GList_getItemPool);
        tolua_function(tolua_S,"getFromPool",lua_ax_fairygui_GList_getFromPool);
        tolua_function(tolua_S,"returnToPool",lua_ax_fairygui_GList_returnToPool);
        tolua_function(tolua_S,"addItemFromPool",lua_ax_fairygui_GList_addItemFromPool);
        tolua_function(tolua_S,"removeChildToPoolAt",lua_ax_fairygui_GList_removeChildToPoolAt);
        tolua_function(tolua_S,"removeChildToPool",lua_ax_fairygui_GList_removeChildToPool);
        tolua_function(tolua_S,"removeChildrenToPool",lua_ax_fairygui_GList_removeChildrenToPool);
        tolua_function(tolua_S,"getSelectedIndex",lua_ax_fairygui_GList_getSelectedIndex);
        tolua_function(tolua_S,"setSelectedIndex",lua_ax_fairygui_GList_setSelectedIndex);
        tolua_function(tolua_S,"getSelection",lua_ax_fairygui_GList_getSelection);
        tolua_function(tolua_S,"addSelection",lua_ax_fairygui_GList_addSelection);
        tolua_function(tolua_S,"removeSelection",lua_ax_fairygui_GList_removeSelection);
        tolua_function(tolua_S,"clearSelection",lua_ax_fairygui_GList_clearSelection);
        tolua_function(tolua_S,"selectAll",lua_ax_fairygui_GList_selectAll);
        tolua_function(tolua_S,"selectReverse",lua_ax_fairygui_GList_selectReverse);
        tolua_function(tolua_S,"handleArrowKey",lua_ax_fairygui_GList_handleArrowKey);
        tolua_function(tolua_S,"resizeToFit",lua_ax_fairygui_GList_resizeToFit);
        tolua_function(tolua_S,"scrollToView",lua_ax_fairygui_GList_scrollToView);
        tolua_function(tolua_S,"getSelectionController",lua_ax_fairygui_GList_getSelectionController);
        tolua_function(tolua_S,"setSelectionController",lua_ax_fairygui_GList_setSelectionController);
        tolua_function(tolua_S,"setVirtual",lua_ax_fairygui_GList_setVirtual);
        tolua_function(tolua_S,"setVirtualAndLoop",lua_ax_fairygui_GList_setVirtualAndLoop);
        tolua_function(tolua_S,"isVirtual",lua_ax_fairygui_GList_isVirtual);
        tolua_function(tolua_S,"refreshVirtualList",lua_ax_fairygui_GList_refreshVirtualList);
        tolua_function(tolua_S,"getNumItems",lua_ax_fairygui_GList_getNumItems);
        tolua_function(tolua_S,"setNumItems",lua_ax_fairygui_GList_setNumItems);
        tolua_function(tolua_S,"childIndexToItemIndex",lua_ax_fairygui_GList_childIndexToItemIndex);
        tolua_function(tolua_S,"itemIndexToChildIndex",lua_ax_fairygui_GList_itemIndexToChildIndex);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GList_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GList).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GList";
    g_typeCast[typeName] = "fgui.GList";
    return 1;
}

int lua_ax_fairygui_GComboBox_getTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getTitle'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitle();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getTitle",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setTitle(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setTitle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComboBox:setTitle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setTitle'", nullptr);
            return 0;
        }
        cobj->setTitle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setTitle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setTitle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getTitleColor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitleColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getTitleColor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getTitleColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "fgui.GComboBox:setTitleColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setTitleColor'", nullptr);
            return 0;
        }
        cobj->setTitleColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setTitleColor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setTitleColor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getTitleFontSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTitleFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getTitleFontSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComboBox:setTitleFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setTitleFontSize'", nullptr);
            return 0;
        }
        cobj->setTitleFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setTitleFontSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getValue'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getValue();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getValue",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GComboBox:setValue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setValue'", nullptr);
            return 0;
        }
        cobj->setValue(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setValue",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getSelectedIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectedIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getSelectedIndex",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GComboBox:setSelectedIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setSelectedIndex'", nullptr);
            return 0;
        }
        cobj->setSelectedIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setSelectedIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getSelectionController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getSelectionController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getSelectionController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSelectionController();
        object_to_luaval<fairygui::GController>(tolua_S, "fgui.GController",(fairygui::GController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getSelectionController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getSelectionController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_setSelectionController(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_setSelectionController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GController* arg0;

        ok &= luaval_to_object<fairygui::GController>(tolua_S, 2, "fgui.GController",&arg0, "fgui.GComboBox:setSelectionController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_setSelectionController'", nullptr);
            return 0;
        }
        cobj->setSelectionController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:setSelectionController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_setSelectionController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getItems(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getItems'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItems();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getItems",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getItems'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getIcons(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getIcons'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getIcons'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getIcons();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getIcons",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getIcons'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getValues(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getValues'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getValues'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getValues();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getValues",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getValues'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getDropdown(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getDropdown'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getDropdown'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDropdown();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getDropdown",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getDropdown'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_refresh(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_refresh'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_refresh'", nullptr);
            return 0;
        }
        cobj->refresh();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:refresh",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_refresh'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_getTextField(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GComboBox*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GComboBox_getTextField'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_getTextField'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTextField();
        object_to_luaval<fairygui::GTextField>(tolua_S, "fgui.GTextField",(fairygui::GTextField*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:getTextField",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_getTextField'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GComboBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GComboBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GComboBox::create();
        object_to_luaval<fairygui::GComboBox>(tolua_S, "fgui.GComboBox",(fairygui::GComboBox*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GComboBox:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GComboBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GComboBox* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GComboBox_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GComboBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GComboBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GComboBox:GComboBox",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GComboBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GComboBox_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GComboBox)");
    return 0;
}

int lua_register_ax_fairygui_GComboBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GComboBox");
    tolua_cclass(tolua_S,"GComboBox","fgui.GComboBox","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GComboBox");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GComboBox_constructor);
        tolua_function(tolua_S,"getTitle",lua_ax_fairygui_GComboBox_getTitle);
        tolua_function(tolua_S,"setTitle",lua_ax_fairygui_GComboBox_setTitle);
        tolua_function(tolua_S,"getTitleColor",lua_ax_fairygui_GComboBox_getTitleColor);
        tolua_function(tolua_S,"setTitleColor",lua_ax_fairygui_GComboBox_setTitleColor);
        tolua_function(tolua_S,"getTitleFontSize",lua_ax_fairygui_GComboBox_getTitleFontSize);
        tolua_function(tolua_S,"setTitleFontSize",lua_ax_fairygui_GComboBox_setTitleFontSize);
        tolua_function(tolua_S,"getValue",lua_ax_fairygui_GComboBox_getValue);
        tolua_function(tolua_S,"setValue",lua_ax_fairygui_GComboBox_setValue);
        tolua_function(tolua_S,"getSelectedIndex",lua_ax_fairygui_GComboBox_getSelectedIndex);
        tolua_function(tolua_S,"setSelectedIndex",lua_ax_fairygui_GComboBox_setSelectedIndex);
        tolua_function(tolua_S,"getSelectionController",lua_ax_fairygui_GComboBox_getSelectionController);
        tolua_function(tolua_S,"setSelectionController",lua_ax_fairygui_GComboBox_setSelectionController);
        tolua_function(tolua_S,"getItems",lua_ax_fairygui_GComboBox_getItems);
        tolua_function(tolua_S,"getIcons",lua_ax_fairygui_GComboBox_getIcons);
        tolua_function(tolua_S,"getValues",lua_ax_fairygui_GComboBox_getValues);
        tolua_function(tolua_S,"getDropdown",lua_ax_fairygui_GComboBox_getDropdown);
        tolua_function(tolua_S,"refresh",lua_ax_fairygui_GComboBox_refresh);
        tolua_function(tolua_S,"getTextField",lua_ax_fairygui_GComboBox_getTextField);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GComboBox_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GComboBox).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GComboBox";
    g_typeCast[typeName] = "fgui.GComboBox";
    return 1;
}

int lua_ax_fairygui_GProgressBar_getTitleType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_getTitleType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_getTitleType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTitleType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:getTitleType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_getTitleType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_setTitleType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_setTitleType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ProgressTitleType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GProgressBar:setTitleType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_setTitleType'", nullptr);
            return 0;
        }
        cobj->setTitleType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:setTitleType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_setTitleType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_getMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_getMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_getMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:getMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_getMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_setMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_setMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GProgressBar:setMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_setMin'", nullptr);
            return 0;
        }
        cobj->setMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:setMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_setMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_getMax(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_getMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_getMax'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMax();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:getMax",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_getMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_setMax(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_setMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GProgressBar:setMax");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_setMax'", nullptr);
            return 0;
        }
        cobj->setMax(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:setMax",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_setMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_getValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_getValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_getValue'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getValue();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:getValue",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_getValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_setValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_setValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GProgressBar:setValue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_setValue'", nullptr);
            return 0;
        }
        cobj->setValue(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:setValue",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_setValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_tweenValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_tweenValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GProgressBar:tweenValue");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GProgressBar:tweenValue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_tweenValue'", nullptr);
            return 0;
        }
        cobj->tweenValue(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:tweenValue",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_tweenValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_update(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GProgressBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GProgressBar_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GProgressBar:update");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_update'", nullptr);
            return 0;
        }
        cobj->update(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:update",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_update'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GProgressBar_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GProgressBar",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GProgressBar::create();
        object_to_luaval<fairygui::GProgressBar>(tolua_S, "fgui.GProgressBar",(fairygui::GProgressBar*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GProgressBar:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GProgressBar_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GProgressBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GProgressBar_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GProgressBar();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GProgressBar");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GProgressBar:GProgressBar",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GProgressBar_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GProgressBar_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GProgressBar)");
    return 0;
}

int lua_register_ax_fairygui_GProgressBar(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GProgressBar");
    tolua_cclass(tolua_S,"GProgressBar","fgui.GProgressBar","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GProgressBar");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GProgressBar_constructor);
        tolua_function(tolua_S,"getTitleType",lua_ax_fairygui_GProgressBar_getTitleType);
        tolua_function(tolua_S,"setTitleType",lua_ax_fairygui_GProgressBar_setTitleType);
        tolua_function(tolua_S,"getMin",lua_ax_fairygui_GProgressBar_getMin);
        tolua_function(tolua_S,"setMin",lua_ax_fairygui_GProgressBar_setMin);
        tolua_function(tolua_S,"getMax",lua_ax_fairygui_GProgressBar_getMax);
        tolua_function(tolua_S,"setMax",lua_ax_fairygui_GProgressBar_setMax);
        tolua_function(tolua_S,"getValue",lua_ax_fairygui_GProgressBar_getValue);
        tolua_function(tolua_S,"setValue",lua_ax_fairygui_GProgressBar_setValue);
        tolua_function(tolua_S,"tweenValue",lua_ax_fairygui_GProgressBar_tweenValue);
        tolua_function(tolua_S,"update",lua_ax_fairygui_GProgressBar_update);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GProgressBar_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GProgressBar).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GProgressBar";
    g_typeCast[typeName] = "fgui.GProgressBar";
    return 1;
}

int lua_ax_fairygui_GSlider_getTitleType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_getTitleType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_getTitleType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTitleType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:getTitleType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_getTitleType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_setTitleType(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_setTitleType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::ProgressTitleType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GSlider:setTitleType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_setTitleType'", nullptr);
            return 0;
        }
        cobj->setTitleType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:setTitleType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_setTitleType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_getMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_getMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_getMin'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:getMin",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_getMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_setMin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_setMin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GSlider:setMin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_setMin'", nullptr);
            return 0;
        }
        cobj->setMin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:setMin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_setMin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_getMax(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_getMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_getMax'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMax();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:getMax",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_getMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_setMax(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_setMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GSlider:setMax");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_setMax'", nullptr);
            return 0;
        }
        cobj->setMax(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:setMax",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_setMax'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_getValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_getValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_getValue'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getValue();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:getValue",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_getValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_setValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_setValue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GSlider:setValue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_setValue'", nullptr);
            return 0;
        }
        cobj->setValue(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:setValue",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_setValue'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_getWholeNumbers(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_getWholeNumbers'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_getWholeNumbers'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getWholeNumbers();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:getWholeNumbers",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_getWholeNumbers'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_setWholeNumbers(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GSlider*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GSlider_setWholeNumbers'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GSlider:setWholeNumbers");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_setWholeNumbers'", nullptr);
            return 0;
        }
        cobj->setWholeNumbers(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:setWholeNumbers",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_setWholeNumbers'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GSlider_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GSlider",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GSlider::create();
        object_to_luaval<fairygui::GSlider>(tolua_S, "fgui.GSlider",(fairygui::GSlider*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GSlider:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GSlider_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GSlider* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GSlider_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GSlider();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GSlider");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GSlider:GSlider",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GSlider_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GSlider_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GSlider)");
    return 0;
}

int lua_register_ax_fairygui_GSlider(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GSlider");
    tolua_cclass(tolua_S,"GSlider","fgui.GSlider","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GSlider");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GSlider_constructor);
        tolua_function(tolua_S,"getTitleType",lua_ax_fairygui_GSlider_getTitleType);
        tolua_function(tolua_S,"setTitleType",lua_ax_fairygui_GSlider_setTitleType);
        tolua_function(tolua_S,"getMin",lua_ax_fairygui_GSlider_getMin);
        tolua_function(tolua_S,"setMin",lua_ax_fairygui_GSlider_setMin);
        tolua_function(tolua_S,"getMax",lua_ax_fairygui_GSlider_getMax);
        tolua_function(tolua_S,"setMax",lua_ax_fairygui_GSlider_setMax);
        tolua_function(tolua_S,"getValue",lua_ax_fairygui_GSlider_getValue);
        tolua_function(tolua_S,"setValue",lua_ax_fairygui_GSlider_setValue);
        tolua_function(tolua_S,"getWholeNumbers",lua_ax_fairygui_GSlider_getWholeNumbers);
        tolua_function(tolua_S,"setWholeNumbers",lua_ax_fairygui_GSlider_setWholeNumbers);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GSlider_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GSlider).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GSlider";
    g_typeCast[typeName] = "fgui.GSlider";
    return 1;
}

int lua_ax_fairygui_GScrollBar_setScrollPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GScrollBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GScrollBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GScrollBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GScrollBar_setScrollPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        fairygui::ScrollPane* arg0;
        bool arg1;

        ok &= luaval_to_object<fairygui::ScrollPane>(tolua_S, 2, "fgui.ScrollPane",&arg0, "fgui.GScrollBar:setScrollPane");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GScrollBar:setScrollPane");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_setScrollPane'", nullptr);
            return 0;
        }
        cobj->setScrollPane(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GScrollBar:setScrollPane",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_setScrollPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GScrollBar_setDisplayPerc(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GScrollBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GScrollBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GScrollBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GScrollBar_setDisplayPerc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GScrollBar:setDisplayPerc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_setDisplayPerc'", nullptr);
            return 0;
        }
        cobj->setDisplayPerc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GScrollBar:setDisplayPerc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_setDisplayPerc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GScrollBar_setScrollPerc(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GScrollBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GScrollBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GScrollBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GScrollBar_setScrollPerc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GScrollBar:setScrollPerc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_setScrollPerc'", nullptr);
            return 0;
        }
        cobj->setScrollPerc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GScrollBar:setScrollPerc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_setScrollPerc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GScrollBar_getMinSize(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GScrollBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GScrollBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GScrollBar*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GScrollBar_getMinSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_getMinSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMinSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GScrollBar:getMinSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_getMinSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GScrollBar_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GScrollBar",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GScrollBar::create();
        object_to_luaval<fairygui::GScrollBar>(tolua_S, "fgui.GScrollBar",(fairygui::GScrollBar*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GScrollBar:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GScrollBar_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GScrollBar* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GScrollBar_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GScrollBar();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GScrollBar");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GScrollBar:GScrollBar",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GScrollBar_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GScrollBar_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GScrollBar)");
    return 0;
}

int lua_register_ax_fairygui_GScrollBar(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GScrollBar");
    tolua_cclass(tolua_S,"GScrollBar","fgui.GScrollBar","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GScrollBar");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GScrollBar_constructor);
        tolua_function(tolua_S,"setScrollPane",lua_ax_fairygui_GScrollBar_setScrollPane);
        tolua_function(tolua_S,"setDisplayPerc",lua_ax_fairygui_GScrollBar_setDisplayPerc);
        tolua_function(tolua_S,"setScrollPerc",lua_ax_fairygui_GScrollBar_setScrollPerc);
        tolua_function(tolua_S,"getMinSize",lua_ax_fairygui_GScrollBar_getMinSize);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GScrollBar_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GScrollBar).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GScrollBar";
    g_typeCast[typeName] = "fgui.GScrollBar";
    return 1;
}

int lua_ax_fairygui_Window_show(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_show'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_show'", nullptr);
            return 0;
        }
        cobj->show();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:show",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_show'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_hide(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_hide'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_hide'", nullptr);
            return 0;
        }
        cobj->hide();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:hide",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_hide'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_hideImmediately(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_hideImmediately'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_hideImmediately'", nullptr);
            return 0;
        }
        cobj->hideImmediately();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:hideImmediately",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_hideImmediately'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_toggleStatus(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_toggleStatus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_toggleStatus'", nullptr);
            return 0;
        }
        cobj->toggleStatus();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:toggleStatus",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_toggleStatus'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_bringToFront(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_bringToFront'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_bringToFront'", nullptr);
            return 0;
        }
        cobj->bringToFront();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:bringToFront",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_bringToFront'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_isShowing(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_isShowing'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_isShowing'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isShowing();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:isShowing",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_isShowing'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_isTop(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_isTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_isTop'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isTop();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:isTop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_isTop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_isModal(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_isModal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_isModal'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isModal();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:isModal",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_isModal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setModal(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setModal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.Window:setModal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setModal'", nullptr);
            return 0;
        }
        cobj->setModal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setModal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setModal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_showModalWait(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_showModalWait'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.Window:showModalWait");

            if (!ok) { break; }
            cobj->showModalWait(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->showModalWait();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.Window:showModalWait",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_showModalWait'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_closeModalWait(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_closeModalWait'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.Window:closeModalWait");

            if (!ok) { break; }
            bool ret = cobj->closeModalWait(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            bool ret = cobj->closeModalWait();
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.Window:closeModalWait",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_closeModalWait'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_initWindow(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_initWindow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_initWindow'", nullptr);
            return 0;
        }
        cobj->initWindow();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:initWindow",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_initWindow'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_addUISource(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_addUISource'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::IUISource* arg0;

        ok &= luaval_to_object<fairygui::IUISource>(tolua_S, 2, "fgui.IUISource",&arg0, "fgui.Window:addUISource");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_addUISource'", nullptr);
            return 0;
        }
        cobj->addUISource(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:addUISource",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_addUISource'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_isBringToFrontOnClick(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_isBringToFrontOnClick'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_isBringToFrontOnClick'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isBringToFrontOnClick();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:isBringToFrontOnClick",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_isBringToFrontOnClick'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setBringToFrontOnClick(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setBringToFrontOnClick'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.Window:setBringToFrontOnClick");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setBringToFrontOnClick'", nullptr);
            return 0;
        }
        cobj->setBringToFrontOnClick(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setBringToFrontOnClick",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setBringToFrontOnClick'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getContentPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getContentPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getContentPane'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentPane();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getContentPane",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getContentPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setContentPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setContentPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GComponent* arg0;

        ok &= luaval_to_object<fairygui::GComponent>(tolua_S, 2, "fgui.GComponent",&arg0, "fgui.Window:setContentPane");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setContentPane'", nullptr);
            return 0;
        }
        cobj->setContentPane(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setContentPane",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setContentPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getFrame(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getFrame'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFrame();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getFrame",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getCloseButton(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getCloseButton'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getCloseButton'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCloseButton();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getCloseButton",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getCloseButton'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setCloseButton(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setCloseButton'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.Window:setCloseButton");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setCloseButton'", nullptr);
            return 0;
        }
        cobj->setCloseButton(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setCloseButton",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setCloseButton'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getDragArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getDragArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getDragArea'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDragArea();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getDragArea",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getDragArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setDragArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setDragArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.Window:setDragArea");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setDragArea'", nullptr);
            return 0;
        }
        cobj->setDragArea(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setDragArea",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setDragArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getContentArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getContentArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getContentArea'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentArea();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getContentArea",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getContentArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_setContentArea(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_setContentArea'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.Window:setContentArea");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_setContentArea'", nullptr);
            return 0;
        }
        cobj->setContentArea(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:setContentArea",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_setContentArea'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_getModalWaitingPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::Window*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_Window_getModalWaitingPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_getModalWaitingPane'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getModalWaitingPane();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:getModalWaitingPane",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_getModalWaitingPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_Window_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.Window",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::Window::create();
        object_to_luaval<fairygui::Window>(tolua_S, "fgui.Window",(fairygui::Window*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.Window:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_Window_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::Window* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_Window_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::Window();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.Window");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.Window:Window",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_Window_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_Window_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Window)");
    return 0;
}

int lua_register_ax_fairygui_Window(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.Window");
    tolua_cclass(tolua_S,"Window","fgui.Window","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"Window");
        tolua_function(tolua_S,"new",lua_ax_fairygui_Window_constructor);
        tolua_function(tolua_S,"show",lua_ax_fairygui_Window_show);
        tolua_function(tolua_S,"hide",lua_ax_fairygui_Window_hide);
        tolua_function(tolua_S,"hideImmediately",lua_ax_fairygui_Window_hideImmediately);
        tolua_function(tolua_S,"toggleStatus",lua_ax_fairygui_Window_toggleStatus);
        tolua_function(tolua_S,"bringToFront",lua_ax_fairygui_Window_bringToFront);
        tolua_function(tolua_S,"isShowing",lua_ax_fairygui_Window_isShowing);
        tolua_function(tolua_S,"isTop",lua_ax_fairygui_Window_isTop);
        tolua_function(tolua_S,"isModal",lua_ax_fairygui_Window_isModal);
        tolua_function(tolua_S,"setModal",lua_ax_fairygui_Window_setModal);
        tolua_function(tolua_S,"showModalWait",lua_ax_fairygui_Window_showModalWait);
        tolua_function(tolua_S,"closeModalWait",lua_ax_fairygui_Window_closeModalWait);
        tolua_function(tolua_S,"initWindow",lua_ax_fairygui_Window_initWindow);
        tolua_function(tolua_S,"addUISource",lua_ax_fairygui_Window_addUISource);
        tolua_function(tolua_S,"isBringToFrontOnClick",lua_ax_fairygui_Window_isBringToFrontOnClick);
        tolua_function(tolua_S,"setBringToFrontOnClick",lua_ax_fairygui_Window_setBringToFrontOnClick);
        tolua_function(tolua_S,"getContentPane",lua_ax_fairygui_Window_getContentPane);
        tolua_function(tolua_S,"setContentPane",lua_ax_fairygui_Window_setContentPane);
        tolua_function(tolua_S,"getFrame",lua_ax_fairygui_Window_getFrame);
        tolua_function(tolua_S,"getCloseButton",lua_ax_fairygui_Window_getCloseButton);
        tolua_function(tolua_S,"setCloseButton",lua_ax_fairygui_Window_setCloseButton);
        tolua_function(tolua_S,"getDragArea",lua_ax_fairygui_Window_getDragArea);
        tolua_function(tolua_S,"setDragArea",lua_ax_fairygui_Window_setDragArea);
        tolua_function(tolua_S,"getContentArea",lua_ax_fairygui_Window_getContentArea);
        tolua_function(tolua_S,"setContentArea",lua_ax_fairygui_Window_setContentArea);
        tolua_function(tolua_S,"getModalWaitingPane",lua_ax_fairygui_Window_getModalWaitingPane);
        tolua_function(tolua_S,"create", lua_ax_fairygui_Window_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::Window).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.Window";
    g_typeCast[typeName] = "fgui.Window";
    return 1;
}

int lua_ax_fairygui_GRoot_showWindow(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_showWindow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::Window* arg0;

        ok &= luaval_to_object<fairygui::Window>(tolua_S, 2, "fgui.Window",&arg0, "fgui.GRoot:showWindow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_showWindow'", nullptr);
            return 0;
        }
        cobj->showWindow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:showWindow",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_showWindow'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hideWindow(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hideWindow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::Window* arg0;

        ok &= luaval_to_object<fairygui::Window>(tolua_S, 2, "fgui.Window",&arg0, "fgui.GRoot:hideWindow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_hideWindow'", nullptr);
            return 0;
        }
        cobj->hideWindow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:hideWindow",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hideWindow'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hideWindowImmediately(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hideWindowImmediately'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::Window* arg0;

        ok &= luaval_to_object<fairygui::Window>(tolua_S, 2, "fgui.Window",&arg0, "fgui.GRoot:hideWindowImmediately");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_hideWindowImmediately'", nullptr);
            return 0;
        }
        cobj->hideWindowImmediately(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:hideWindowImmediately",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hideWindowImmediately'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_bringToFront(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_bringToFront'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::Window* arg0;

        ok &= luaval_to_object<fairygui::Window>(tolua_S, 2, "fgui.Window",&arg0, "fgui.GRoot:bringToFront");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_bringToFront'", nullptr);
            return 0;
        }
        cobj->bringToFront(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:bringToFront",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_bringToFront'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_showModalWait(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_showModalWait'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_showModalWait'", nullptr);
            return 0;
        }
        cobj->showModalWait();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:showModalWait",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_showModalWait'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_closeModalWait(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_closeModalWait'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_closeModalWait'", nullptr);
            return 0;
        }
        cobj->closeModalWait();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:closeModalWait",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_closeModalWait'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_closeAllExceptModals(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_closeAllExceptModals'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_closeAllExceptModals'", nullptr);
            return 0;
        }
        cobj->closeAllExceptModals();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:closeAllExceptModals",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_closeAllExceptModals'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_closeAllWindows(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_closeAllWindows'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_closeAllWindows'", nullptr);
            return 0;
        }
        cobj->closeAllWindows();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:closeAllWindows",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_closeAllWindows'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getTopWindow(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getTopWindow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getTopWindow'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTopWindow();
        object_to_luaval<fairygui::Window>(tolua_S, "fgui.Window",(fairygui::Window*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getTopWindow",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getTopWindow'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getModalWaitingPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getModalWaitingPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getModalWaitingPane'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getModalWaitingPane();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getModalWaitingPane",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getModalWaitingPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getModalLayer(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getModalLayer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getModalLayer'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getModalLayer();
        object_to_luaval<fairygui::GGraph>(tolua_S, "fgui.GGraph",(fairygui::GGraph*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getModalLayer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getModalLayer'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hasModalWindow(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hasModalWindow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_hasModalWindow'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hasModalWindow();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:hasModalWindow",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hasModalWindow'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_isModalWaiting(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_isModalWaiting'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_isModalWaiting'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isModalWaiting();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:isModalWaiting",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_isModalWaiting'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getInputProcessor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getInputProcessor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getInputProcessor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getInputProcessor();
        object_to_luaval<fairygui::InputProcessor>(tolua_S, "fgui.InputProcessor",(fairygui::InputProcessor*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getInputProcessor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getInputProcessor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getTouchPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getTouchPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GRoot:getTouchPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getTouchPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTouchPosition(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getTouchPosition",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getTouchPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getTouchTarget(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getTouchTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getTouchTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTouchTarget();
        object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getTouchTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getTouchTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_worldToRoot(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_worldToRoot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GRoot:worldToRoot");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_worldToRoot'", nullptr);
            return 0;
        }
        auto&& ret = cobj->worldToRoot(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:worldToRoot",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_worldToRoot'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_rootToWorld(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_rootToWorld'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GRoot:rootToWorld");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_rootToWorld'", nullptr);
            return 0;
        }
        auto&& ret = cobj->rootToWorld(arg0);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:rootToWorld",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_rootToWorld'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_showPopup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_showPopup'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 3) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:showPopup");

            if (!ok) { break; }
            fairygui::GObject* arg1;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.GRoot:showPopup");

            if (!ok) { break; }
            fairygui::PopupDirection arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GRoot:showPopup");

            if (!ok) { break; }
            cobj->showPopup(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:showPopup");

            if (!ok) { break; }
            cobj->showPopup(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GRoot:showPopup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_showPopup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_togglePopup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_togglePopup'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 3) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:togglePopup");

            if (!ok) { break; }
            fairygui::GObject* arg1;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.GRoot:togglePopup");

            if (!ok) { break; }
            fairygui::PopupDirection arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GRoot:togglePopup");

            if (!ok) { break; }
            cobj->togglePopup(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:togglePopup");

            if (!ok) { break; }
            cobj->togglePopup(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GRoot:togglePopup",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_togglePopup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hidePopup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hidePopup'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:hidePopup");

            if (!ok) { break; }
            cobj->hidePopup(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->hidePopup();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.GRoot:hidePopup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hidePopup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hasAnyPopup(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hasAnyPopup'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_hasAnyPopup'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hasAnyPopup();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:hasAnyPopup",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hasAnyPopup'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getPoupPosition(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getPoupPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        fairygui::GObject* arg0;
        fairygui::GObject* arg1;
        fairygui::PopupDirection arg2;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:getPoupPosition");

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 3, "fgui.GObject",&arg1, "fgui.GRoot:getPoupPosition");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GRoot:getPoupPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getPoupPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPoupPosition(arg0, arg1, arg2);
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getPoupPosition",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getPoupPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_showTooltips(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_showTooltips'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GRoot:showTooltips");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_showTooltips'", nullptr);
            return 0;
        }
        cobj->showTooltips(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:showTooltips",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_showTooltips'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_showTooltipsWin(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_showTooltipsWin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GObject* arg0;

        ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.GRoot:showTooltipsWin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_showTooltipsWin'", nullptr);
            return 0;
        }
        cobj->showTooltipsWin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:showTooltipsWin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_showTooltipsWin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_hideTooltips(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_hideTooltips'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_hideTooltips'", nullptr);
            return 0;
        }
        cobj->hideTooltips();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:hideTooltips",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_hideTooltips'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_playSound(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_playSound'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GRoot:playSound");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_playSound'", nullptr);
            return 0;
        }
        cobj->playSound(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        double arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.GRoot:playSound");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GRoot:playSound");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_playSound'", nullptr);
            return 0;
        }
        cobj->playSound(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:playSound",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_playSound'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_isSoundEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_isSoundEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_isSoundEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isSoundEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:isSoundEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_isSoundEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_setSoundEnabled(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_setSoundEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GRoot:setSoundEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_setSoundEnabled'", nullptr);
            return 0;
        }
        cobj->setSoundEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:setSoundEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_setSoundEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_getSoundVolumeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_getSoundVolumeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getSoundVolumeScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSoundVolumeScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:getSoundVolumeScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getSoundVolumeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_setSoundVolumeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GRoot*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GRoot_setSoundVolumeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GRoot:setSoundVolumeScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_setSoundVolumeScale'", nullptr);
            return 0;
        }
        cobj->setSoundVolumeScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:setSoundVolumeScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_setSoundVolumeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GRoot_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Scene* arg0;
        ok &= luaval_to_object<ax::Scene>(tolua_S, 2, "ax.Scene",&arg0, "fgui.GRoot:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GRoot::create(arg0);
        object_to_luaval<fairygui::GRoot>(tolua_S, "fgui.GRoot",(fairygui::GRoot*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Scene* arg0;
        int arg1;
        ok &= luaval_to_object<ax::Scene>(tolua_S, 2, "ax.Scene",&arg0, "fgui.GRoot:create");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GRoot:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_create'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GRoot::create(arg0, arg1);
        object_to_luaval<fairygui::GRoot>(tolua_S, "fgui.GRoot",(fairygui::GRoot*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GRoot:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GRoot_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GRoot",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GRoot::getInstance();
        object_to_luaval<fairygui::GRoot>(tolua_S, "fgui.GRoot",(fairygui::GRoot*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GRoot:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GRoot_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GRoot* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GRoot_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GRoot();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GRoot");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GRoot:GRoot",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GRoot_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GRoot_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GRoot)");
    return 0;
}

int lua_register_ax_fairygui_GRoot(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GRoot");
    tolua_cclass(tolua_S,"GRoot","fgui.GRoot","fgui.GComponent",nullptr);

    tolua_beginmodule(tolua_S,"GRoot");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GRoot_constructor);
        tolua_function(tolua_S,"showWindow",lua_ax_fairygui_GRoot_showWindow);
        tolua_function(tolua_S,"hideWindow",lua_ax_fairygui_GRoot_hideWindow);
        tolua_function(tolua_S,"hideWindowImmediately",lua_ax_fairygui_GRoot_hideWindowImmediately);
        tolua_function(tolua_S,"bringToFront",lua_ax_fairygui_GRoot_bringToFront);
        tolua_function(tolua_S,"showModalWait",lua_ax_fairygui_GRoot_showModalWait);
        tolua_function(tolua_S,"closeModalWait",lua_ax_fairygui_GRoot_closeModalWait);
        tolua_function(tolua_S,"closeAllExceptModals",lua_ax_fairygui_GRoot_closeAllExceptModals);
        tolua_function(tolua_S,"closeAllWindows",lua_ax_fairygui_GRoot_closeAllWindows);
        tolua_function(tolua_S,"getTopWindow",lua_ax_fairygui_GRoot_getTopWindow);
        tolua_function(tolua_S,"getModalWaitingPane",lua_ax_fairygui_GRoot_getModalWaitingPane);
        tolua_function(tolua_S,"getModalLayer",lua_ax_fairygui_GRoot_getModalLayer);
        tolua_function(tolua_S,"hasModalWindow",lua_ax_fairygui_GRoot_hasModalWindow);
        tolua_function(tolua_S,"isModalWaiting",lua_ax_fairygui_GRoot_isModalWaiting);
        tolua_function(tolua_S,"getInputProcessor",lua_ax_fairygui_GRoot_getInputProcessor);
        tolua_function(tolua_S,"getTouchPosition",lua_ax_fairygui_GRoot_getTouchPosition);
        tolua_function(tolua_S,"getTouchTarget",lua_ax_fairygui_GRoot_getTouchTarget);
        tolua_function(tolua_S,"worldToRoot",lua_ax_fairygui_GRoot_worldToRoot);
        tolua_function(tolua_S,"rootToWorld",lua_ax_fairygui_GRoot_rootToWorld);
        tolua_function(tolua_S,"showPopup",lua_ax_fairygui_GRoot_showPopup);
        tolua_function(tolua_S,"togglePopup",lua_ax_fairygui_GRoot_togglePopup);
        tolua_function(tolua_S,"hidePopup",lua_ax_fairygui_GRoot_hidePopup);
        tolua_function(tolua_S,"hasAnyPopup",lua_ax_fairygui_GRoot_hasAnyPopup);
        tolua_function(tolua_S,"getPoupPosition",lua_ax_fairygui_GRoot_getPoupPosition);
        tolua_function(tolua_S,"showTooltips",lua_ax_fairygui_GRoot_showTooltips);
        tolua_function(tolua_S,"showTooltipsWin",lua_ax_fairygui_GRoot_showTooltipsWin);
        tolua_function(tolua_S,"hideTooltips",lua_ax_fairygui_GRoot_hideTooltips);
        tolua_function(tolua_S,"playSound",lua_ax_fairygui_GRoot_playSound);
        tolua_function(tolua_S,"isSoundEnabled",lua_ax_fairygui_GRoot_isSoundEnabled);
        tolua_function(tolua_S,"setSoundEnabled",lua_ax_fairygui_GRoot_setSoundEnabled);
        tolua_function(tolua_S,"getSoundVolumeScale",lua_ax_fairygui_GRoot_getSoundVolumeScale);
        tolua_function(tolua_S,"setSoundVolumeScale",lua_ax_fairygui_GRoot_setSoundVolumeScale);
        tolua_function(tolua_S,"create", lua_ax_fairygui_GRoot_create);
        tolua_function(tolua_S,"getInstance", lua_ax_fairygui_GRoot_getInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GRoot).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GRoot";
    g_typeCast[typeName] = "fgui.GRoot";
    return 1;
}

int lua_ax_fairygui_PopupMenu_addItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_addItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::function<void (fairygui::EventContext *)> arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:addItem");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_addItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addItem(arg0, arg1);
        object_to_luaval<fairygui::GButton>(tolua_S, "fgui.GButton",(fairygui::GButton*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:addItem",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_addItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_addItemAt(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_addItemAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string arg0;
        int arg1;
        std::function<void (fairygui::EventContext *)> arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:addItemAt");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.PopupMenu:addItemAt");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_addItemAt'", nullptr);
            return 0;
        }
        auto&& ret = cobj->addItemAt(arg0, arg1, arg2);
        object_to_luaval<fairygui::GButton>(tolua_S, "fgui.GButton",(fairygui::GButton*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:addItemAt",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_addItemAt'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_addSeperator(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_addSeperator'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_addSeperator'", nullptr);
            return 0;
        }
        cobj->addSeperator();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:addSeperator",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_addSeperator'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_getItemName(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_getItemName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.PopupMenu:getItemName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_getItemName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItemName(arg0);
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:getItemName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_getItemName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_setItemText(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_setItemText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:setItemText");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "fgui.PopupMenu:setItemText");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_setItemText'", nullptr);
            return 0;
        }
        cobj->setItemText(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:setItemText",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_setItemText'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_setItemVisible(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_setItemVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:setItemVisible");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.PopupMenu:setItemVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_setItemVisible'", nullptr);
            return 0;
        }
        cobj->setItemVisible(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:setItemVisible",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_setItemVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_setItemGrayed(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_setItemGrayed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:setItemGrayed");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.PopupMenu:setItemGrayed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_setItemGrayed'", nullptr);
            return 0;
        }
        cobj->setItemGrayed(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:setItemGrayed",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_setItemGrayed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_setItemCheckable(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_setItemCheckable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:setItemCheckable");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.PopupMenu:setItemCheckable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_setItemCheckable'", nullptr);
            return 0;
        }
        cobj->setItemCheckable(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:setItemCheckable",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_setItemCheckable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_setItemChecked(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_setItemChecked'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        bool arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:setItemChecked");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.PopupMenu:setItemChecked");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_setItemChecked'", nullptr);
            return 0;
        }
        cobj->setItemChecked(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:setItemChecked",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_setItemChecked'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_isItemChecked(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_isItemChecked'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:isItemChecked");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_isItemChecked'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isItemChecked(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:isItemChecked",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_isItemChecked'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_removeItem(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_removeItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:removeItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_removeItem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->removeItem(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:removeItem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_removeItem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_clearItems(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_clearItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_clearItems'", nullptr);
            return 0;
        }
        cobj->clearItems();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:clearItems",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_clearItems'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_getItemCount(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_getItemCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_getItemCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getItemCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:getItemCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_getItemCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_getContentPane(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_getContentPane'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_getContentPane'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentPane();
        object_to_luaval<fairygui::GComponent>(tolua_S, "fgui.GComponent",(fairygui::GComponent*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:getContentPane",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_getContentPane'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_getList(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_getList'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_getList'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getList();
        object_to_luaval<fairygui::GList>(tolua_S, "fgui.GList",(fairygui::GList*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:getList",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_getList'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_show(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::PopupMenu*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_PopupMenu_show'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            fairygui::GObject* arg0;
            ok &= luaval_to_object<fairygui::GObject>(tolua_S, 2, "fgui.GObject",&arg0, "fgui.PopupMenu:show");

            if (!ok) { break; }
            fairygui::PopupDirection arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.PopupMenu:show");

            if (!ok) { break; }
            cobj->show(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->show();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "fgui.PopupMenu:show",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_show'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_PopupMenu_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.PopupMenu",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 0)
        {
            fairygui::PopupMenu* ret = fairygui::PopupMenu::create();
            object_to_luaval<fairygui::PopupMenu>(tolua_S, "fgui.PopupMenu",(fairygui::PopupMenu*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.PopupMenu:create");
            if (!ok) { break; }
            fairygui::PopupMenu* ret = fairygui::PopupMenu::create(arg0);
            object_to_luaval<fairygui::PopupMenu>(tolua_S, "fgui.PopupMenu",(fairygui::PopupMenu*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "fgui.PopupMenu:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_PopupMenu_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::PopupMenu* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_PopupMenu_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::PopupMenu();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.PopupMenu");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.PopupMenu:PopupMenu",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_PopupMenu_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_PopupMenu_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (PopupMenu)");
    return 0;
}

int lua_register_ax_fairygui_PopupMenu(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.PopupMenu");
    tolua_cclass(tolua_S,"PopupMenu","fgui.PopupMenu","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"PopupMenu");
        tolua_function(tolua_S,"new",lua_ax_fairygui_PopupMenu_constructor);
        tolua_function(tolua_S,"addItem",lua_ax_fairygui_PopupMenu_addItem);
        tolua_function(tolua_S,"addItemAt",lua_ax_fairygui_PopupMenu_addItemAt);
        tolua_function(tolua_S,"addSeperator",lua_ax_fairygui_PopupMenu_addSeperator);
        tolua_function(tolua_S,"getItemName",lua_ax_fairygui_PopupMenu_getItemName);
        tolua_function(tolua_S,"setItemText",lua_ax_fairygui_PopupMenu_setItemText);
        tolua_function(tolua_S,"setItemVisible",lua_ax_fairygui_PopupMenu_setItemVisible);
        tolua_function(tolua_S,"setItemGrayed",lua_ax_fairygui_PopupMenu_setItemGrayed);
        tolua_function(tolua_S,"setItemCheckable",lua_ax_fairygui_PopupMenu_setItemCheckable);
        tolua_function(tolua_S,"setItemChecked",lua_ax_fairygui_PopupMenu_setItemChecked);
        tolua_function(tolua_S,"isItemChecked",lua_ax_fairygui_PopupMenu_isItemChecked);
        tolua_function(tolua_S,"removeItem",lua_ax_fairygui_PopupMenu_removeItem);
        tolua_function(tolua_S,"clearItems",lua_ax_fairygui_PopupMenu_clearItems);
        tolua_function(tolua_S,"getItemCount",lua_ax_fairygui_PopupMenu_getItemCount);
        tolua_function(tolua_S,"getContentPane",lua_ax_fairygui_PopupMenu_getContentPane);
        tolua_function(tolua_S,"getList",lua_ax_fairygui_PopupMenu_getList);
        tolua_function(tolua_S,"show",lua_ax_fairygui_PopupMenu_show);
        tolua_function(tolua_S,"create", lua_ax_fairygui_PopupMenu_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::PopupMenu).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.PopupMenu";
    g_typeCast[typeName] = "fgui.PopupMenu";
    return 1;
}

int lua_ax_fairygui_UIObjectFactory_newObject(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIObjectFactory",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            fairygui::ObjectType arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.UIObjectFactory:newObject");
            if (!ok) { break; }
            fairygui::GObject* ret = fairygui::UIObjectFactory::newObject(arg0);
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            fairygui::PackageItem* arg0;
            ok &= luaval_to_object<fairygui::PackageItem>(tolua_S, 2, "fgui.PackageItem",&arg0, "fgui.UIObjectFactory:newObject");
            if (!ok) { break; }
            fairygui::GObject* ret = fairygui::UIObjectFactory::newObject(arg0);
            object_to_luaval<fairygui::GObject>(tolua_S, "fgui.GObject",(fairygui::GObject*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "fgui.UIObjectFactory:newObject",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIObjectFactory_newObject'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_UIObjectFactory_setLoaderExtension(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.UIObjectFactory",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::function<fairygui::GLoader * ()> arg0;
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_UIObjectFactory_setLoaderExtension'", nullptr);
            return 0;
        }
        fairygui::UIObjectFactory::setLoaderExtension(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIObjectFactory:setLoaderExtension",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_UIObjectFactory_setLoaderExtension'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_fairygui_UIObjectFactory_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (UIObjectFactory)");
    return 0;
}

int lua_register_ax_fairygui_UIObjectFactory(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.UIObjectFactory");
    tolua_cclass(tolua_S,"UIObjectFactory","fgui.UIObjectFactory","",nullptr);

    tolua_beginmodule(tolua_S,"UIObjectFactory");
        tolua_function(tolua_S,"newObject", lua_ax_fairygui_UIObjectFactory_newObject);
        tolua_function(tolua_S,"setLoaderExtension", lua_ax_fairygui_UIObjectFactory_setLoaderExtension);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::UIObjectFactory).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.UIObjectFactory";
    g_typeCast[typeName] = "fgui.UIObjectFactory";
    return 1;
}

int lua_ax_fairygui_DragDropManager_getAgent(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::DragDropManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.DragDropManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::DragDropManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_DragDropManager_getAgent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_getAgent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAgent();
        object_to_luaval<fairygui::GLoader>(tolua_S, "fgui.GLoader",(fairygui::GLoader*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.DragDropManager:getAgent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_getAgent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_DragDropManager_isDragging(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::DragDropManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.DragDropManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::DragDropManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_DragDropManager_isDragging'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_isDragging'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDragging();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.DragDropManager:isDragging",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_isDragging'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_DragDropManager_startDrag(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::DragDropManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.DragDropManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::DragDropManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_DragDropManager_startDrag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.DragDropManager:startDrag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_startDrag'", nullptr);
            return 0;
        }
        cobj->startDrag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        ax::Value arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.DragDropManager:startDrag");

        ok &= luaval_to_ccvalue(tolua_S, 3, &arg1, "fgui.DragDropManager:startDrag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_startDrag'", nullptr);
            return 0;
        }
        cobj->startDrag(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        std::string arg0;
        ax::Value arg1;
        int arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "fgui.DragDropManager:startDrag");

        ok &= luaval_to_ccvalue(tolua_S, 3, &arg1, "fgui.DragDropManager:startDrag");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.DragDropManager:startDrag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_startDrag'", nullptr);
            return 0;
        }
        cobj->startDrag(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.DragDropManager:startDrag",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_startDrag'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_DragDropManager_cancel(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::DragDropManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.DragDropManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::DragDropManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_DragDropManager_cancel'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_cancel'", nullptr);
            return 0;
        }
        cobj->cancel();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.DragDropManager:cancel",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_cancel'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_DragDropManager_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.DragDropManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::DragDropManager::getInstance();
        object_to_luaval<fairygui::DragDropManager>(tolua_S, "fgui.DragDropManager",(fairygui::DragDropManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.DragDropManager:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_DragDropManager_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::DragDropManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_DragDropManager_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::DragDropManager();
        tolua_pushusertype(tolua_S,(void*)cobj,"fgui.DragDropManager");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.DragDropManager:DragDropManager",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_DragDropManager_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_DragDropManager_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (DragDropManager)");
    return 0;
}

int lua_register_ax_fairygui_DragDropManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.DragDropManager");
    tolua_cclass(tolua_S,"DragDropManager","fgui.DragDropManager","",nullptr);

    tolua_beginmodule(tolua_S,"DragDropManager");
        tolua_function(tolua_S,"new",lua_ax_fairygui_DragDropManager_constructor);
        tolua_function(tolua_S,"getAgent",lua_ax_fairygui_DragDropManager_getAgent);
        tolua_function(tolua_S,"isDragging",lua_ax_fairygui_DragDropManager_isDragging);
        tolua_function(tolua_S,"startDrag",lua_ax_fairygui_DragDropManager_startDrag);
        tolua_function(tolua_S,"cancel",lua_ax_fairygui_DragDropManager_cancel);
        tolua_function(tolua_S,"getInstance", lua_ax_fairygui_DragDropManager_getInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::DragDropManager).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.DragDropManager";
    g_typeCast[typeName] = "fgui.DragDropManager";
    return 1;
}

int lua_ax_fairygui_GTweener_setDelay(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setDelay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setDelay");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setDelay'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setDelay(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setDelay",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setDelay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getDelay(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getDelay'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getDelay'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDelay();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getDelay",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getDelay'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setDuration(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setDuration'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setDuration(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setDuration",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getDuration(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getDuration'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDuration();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getDuration",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setBreakpoint(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setBreakpoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setBreakpoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setBreakpoint'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setBreakpoint(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setBreakpoint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setBreakpoint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setEase(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setEase'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::EaseType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTweener:setEase");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setEase'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setEase(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setEase",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setEase'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setEasePeriod(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setEasePeriod'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setEasePeriod");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setEasePeriod'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setEasePeriod(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setEasePeriod",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setEasePeriod'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setEaseOvershootOrAmplitude(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setEaseOvershootOrAmplitude'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setEaseOvershootOrAmplitude");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setEaseOvershootOrAmplitude'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setEaseOvershootOrAmplitude(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setEaseOvershootOrAmplitude",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setEaseOvershootOrAmplitude'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setRepeat(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setRepeat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTweener:setRepeat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setRepeat'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setRepeat(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        bool arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GTweener:setRepeat");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "fgui.GTweener:setRepeat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setRepeat'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setRepeat(arg0, arg1);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setRepeat",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setRepeat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getRepeat(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getRepeat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getRepeat'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRepeat();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getRepeat",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getRepeat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:setTimeScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setTimeScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setTimeScale(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setTimeScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setSnapping(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setSnapping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTweener:setSnapping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setSnapping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setSnapping(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setSnapping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setSnapping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setTargetAny(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setTargetAny'", nullptr);
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
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setTargetAny'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setTargetAny(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setTargetAny",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setTargetAny'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setTarget(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Object* arg0;

        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTweener:setTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setTarget(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Object* arg0;
        fairygui::TweenPropType arg1;

        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTweener:setTarget");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GTweener:setTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setTarget(arg0, arg1);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setTarget",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getTarget(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTarget();
        #pragma warning NO CONVERSION FROM NATIVE FOR void*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setUserData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Value arg0;

        ok &= luaval_to_ccvalue(tolua_S, 2, &arg0, "fgui.GTweener:setUserData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setUserData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setUserData(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setUserData",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setPath(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        fairygui::GPath* arg0;

        ok &= luaval_to_object<fairygui::GPath>(tolua_S, 2, "fgui.GPath",&arg0, "fgui.GTweener:setPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setPath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setPath(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setPath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getUserData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getUserData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getUserData'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUserData();
        ccvalue_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getUserData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getUserData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_getNormalizedTime(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_getNormalizedTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_getNormalizedTime'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getNormalizedTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:getNormalizedTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_getNormalizedTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_isCompleted(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_isCompleted'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_isCompleted'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isCompleted();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:isCompleted",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_isCompleted'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_allCompleted(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_allCompleted'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_allCompleted'", nullptr);
            return 0;
        }
        auto&& ret = cobj->allCompleted();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:allCompleted",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_allCompleted'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_setPaused(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_setPaused'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTweener:setPaused");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_setPaused'", nullptr);
            return 0;
        }
        auto&& ret = cobj->setPaused(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:setPaused",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_setPaused'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_seek(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_seek'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTweener:seek");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_seek'", nullptr);
            return 0;
        }
        cobj->seek(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:seek",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_seek'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_kill(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_kill'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_kill'", nullptr);
            return 0;
        }
        cobj->kill();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "fgui.GTweener:kill");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_kill'", nullptr);
            return 0;
        }
        cobj->kill(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:kill",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_kill'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_fairygui_GTweener_constructor(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTweener_constructor'", nullptr);
            return 0;
        }
        cobj = new fairygui::GTweener();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"fgui.GTweener");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:GTweener",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_fairygui_GTweener_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GTweener)");
    return 0;
}

int lua_register_ax_fairygui_GTweener(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GTweener");
    tolua_cclass(tolua_S,"GTweener","fgui.GTweener","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"GTweener");
        tolua_function(tolua_S,"new",lua_ax_fairygui_GTweener_constructor);
        tolua_function(tolua_S,"setDelay",lua_ax_fairygui_GTweener_setDelay);
        tolua_function(tolua_S,"getDelay",lua_ax_fairygui_GTweener_getDelay);
        tolua_function(tolua_S,"setDuration",lua_ax_fairygui_GTweener_setDuration);
        tolua_function(tolua_S,"getDuration",lua_ax_fairygui_GTweener_getDuration);
        tolua_function(tolua_S,"setBreakpoint",lua_ax_fairygui_GTweener_setBreakpoint);
        tolua_function(tolua_S,"setEase",lua_ax_fairygui_GTweener_setEase);
        tolua_function(tolua_S,"setEasePeriod",lua_ax_fairygui_GTweener_setEasePeriod);
        tolua_function(tolua_S,"setEaseOvershootOrAmplitude",lua_ax_fairygui_GTweener_setEaseOvershootOrAmplitude);
        tolua_function(tolua_S,"setRepeat",lua_ax_fairygui_GTweener_setRepeat);
        tolua_function(tolua_S,"getRepeat",lua_ax_fairygui_GTweener_getRepeat);
        tolua_function(tolua_S,"setTimeScale",lua_ax_fairygui_GTweener_setTimeScale);
        tolua_function(tolua_S,"setSnapping",lua_ax_fairygui_GTweener_setSnapping);
        tolua_function(tolua_S,"setTargetAny",lua_ax_fairygui_GTweener_setTargetAny);
        tolua_function(tolua_S,"setTarget",lua_ax_fairygui_GTweener_setTarget);
        tolua_function(tolua_S,"getTarget",lua_ax_fairygui_GTweener_getTarget);
        tolua_function(tolua_S,"setUserData",lua_ax_fairygui_GTweener_setUserData);
        tolua_function(tolua_S,"setPath",lua_ax_fairygui_GTweener_setPath);
        tolua_function(tolua_S,"getUserData",lua_ax_fairygui_GTweener_getUserData);
        tolua_function(tolua_S,"getNormalizedTime",lua_ax_fairygui_GTweener_getNormalizedTime);
        tolua_function(tolua_S,"isCompleted",lua_ax_fairygui_GTweener_isCompleted);
        tolua_function(tolua_S,"allCompleted",lua_ax_fairygui_GTweener_allCompleted);
        tolua_function(tolua_S,"setPaused",lua_ax_fairygui_GTweener_setPaused);
        tolua_function(tolua_S,"seek",lua_ax_fairygui_GTweener_seek);
        tolua_function(tolua_S,"kill",lua_ax_fairygui_GTweener_kill);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GTweener).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GTweener";
    g_typeCast[typeName] = "fgui.GTweener";
    return 1;
}

int lua_ax_fairygui_GTween_toFloat(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        double arg0;
        double arg1;
        double arg2;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTween:toFloat");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GTween:toFloat");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toFloat");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toFloat'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toFloat(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toFloat",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toFloat'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_toVec2(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Vec2 arg0;
        ax::Vec2 arg1;
        double arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GTween:toVec2");
        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "fgui.GTween:toVec2");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toVec2");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toVec2'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toVec2(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toVec2",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toVec2'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_toVec3(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        double arg2;
        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "fgui.GTween:toVec3");
        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "fgui.GTween:toVec3");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toVec3");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toVec3'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toVec3(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toVec3",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toVec3'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_toVec4(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Vec4 arg0;
        ax::Vec4 arg1;
        double arg2;
        ok &= luaval_to_vec4(tolua_S, 2, &arg0, "fgui.GTween:toVec4");
        ok &= luaval_to_vec4(tolua_S, 3, &arg1, "fgui.GTween:toVec4");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toVec4");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toVec4'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toVec4(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toVec4",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toVec4'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_toColor4B(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Color4B arg0;
        ax::Color4B arg1;
        double arg2;
        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "fgui.GTween:toColor4B");
        ok &=luaval_to_color4b(tolua_S, 3, &arg1, "fgui.GTween:toColor4B");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toColor4B");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toColor4B'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toColor4B(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toColor4B",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toColor4B'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_toDouble(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        double arg0;
        double arg1;
        double arg2;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTween:toDouble");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GTween:toDouble");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:toDouble");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_toDouble'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::toDouble(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:toDouble",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_toDouble'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_delayedCall(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        double arg0;
        ok &= luaval_to_number(tolua_S, 2,&arg0, "fgui.GTween:delayedCall");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_delayedCall'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::delayedCall(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:delayedCall",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_delayedCall'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_shake(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        ax::Vec2 arg0;
        double arg1;
        double arg2;
        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "fgui.GTween:shake");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "fgui.GTween:shake");
        ok &= luaval_to_number(tolua_S, 4,&arg2, "fgui.GTween:shake");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_shake'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::shake(arg0, arg1, arg2);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:shake",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_shake'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_isTweening(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Object* arg0;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:isTweening");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_isTweening'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::isTweening(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Object* arg0;
        fairygui::TweenPropType arg1;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:isTweening");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GTween:isTweening");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_isTweening'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::isTweening(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:isTweening",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_isTweening'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_kill(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Object* arg0;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:kill");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_kill'", nullptr);
            return 0;
        }
        fairygui::GTween::kill(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Object* arg0;
        fairygui::TweenPropType arg1;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:kill");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GTween:kill");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_kill'", nullptr);
            return 0;
        }
        fairygui::GTween::kill(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        ax::Object* arg0;
        fairygui::TweenPropType arg1;
        bool arg2;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:kill");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GTween:kill");
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "fgui.GTween:kill");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_kill'", nullptr);
            return 0;
        }
        fairygui::GTween::kill(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:kill",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_kill'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_getTween(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::Object* arg0;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:getTween");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_getTween'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::getTween(arg0);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Object* arg0;
        fairygui::TweenPropType arg1;
        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "fgui.GTween:getTween");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GTween:getTween");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_getTween'", nullptr);
            return 0;
        }
        auto&& ret = fairygui::GTween::getTween(arg0, arg1);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:getTween",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_getTween'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_fairygui_GTween_clean(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"fgui.GTween",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GTween_clean'", nullptr);
            return 0;
        }
        fairygui::GTween::clean();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.GTween:clean",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTween_clean'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_fairygui_GTween_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GTween)");
    return 0;
}

int lua_register_ax_fairygui_GTween(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"fgui.GTween");
    tolua_cclass(tolua_S,"GTween","fgui.GTween","",nullptr);

    tolua_beginmodule(tolua_S,"GTween");
        tolua_function(tolua_S,"toFloat", lua_ax_fairygui_GTween_toFloat);
        tolua_function(tolua_S,"toVec2", lua_ax_fairygui_GTween_toVec2);
        tolua_function(tolua_S,"toVec3", lua_ax_fairygui_GTween_toVec3);
        tolua_function(tolua_S,"toVec4", lua_ax_fairygui_GTween_toVec4);
        tolua_function(tolua_S,"toColor4B", lua_ax_fairygui_GTween_toColor4B);
        tolua_function(tolua_S,"toDouble", lua_ax_fairygui_GTween_toDouble);
        tolua_function(tolua_S,"delayedCall", lua_ax_fairygui_GTween_delayedCall);
        tolua_function(tolua_S,"shake", lua_ax_fairygui_GTween_shake);
        tolua_function(tolua_S,"isTweening", lua_ax_fairygui_GTween_isTweening);
        tolua_function(tolua_S,"kill", lua_ax_fairygui_GTween_kill);
        tolua_function(tolua_S,"getTween", lua_ax_fairygui_GTween_getTween);
        tolua_function(tolua_S,"clean", lua_ax_fairygui_GTween_clean);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(fairygui::GTween).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "fgui.GTween";
    g_typeCast[typeName] = "fgui.GTween";
    return 1;
}
TOLUA_API int register_all_ax_fairygui(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"fgui",0);
	tolua_beginmodule(tolua_S,"fgui");

	lua_register_ax_fairygui_InputEvent(tolua_S);
	lua_register_ax_fairygui_EventContext(tolua_S);
	lua_register_ax_fairygui_UIEventDispatcher(tolua_S);
	lua_register_ax_fairygui_GController(tolua_S);
	lua_register_ax_fairygui_GObject(tolua_S);
	lua_register_ax_fairygui_UIPackage(tolua_S);
	lua_register_ax_fairygui_GImage(tolua_S);
	lua_register_ax_fairygui_GMovieClip(tolua_S);
	lua_register_ax_fairygui_GTextField(tolua_S);
	lua_register_ax_fairygui_GBasicTextField(tolua_S);
	lua_register_ax_fairygui_GRichTextField(tolua_S);
	lua_register_ax_fairygui_GTextInput(tolua_S);
	lua_register_ax_fairygui_GGraph(tolua_S);
	lua_register_ax_fairygui_GLoader(tolua_S);
	lua_register_ax_fairygui_GGroup(tolua_S);
	lua_register_ax_fairygui_ScrollPane(tolua_S);
	lua_register_ax_fairygui_Transition(tolua_S);
	lua_register_ax_fairygui_GComponent(tolua_S);
	lua_register_ax_fairygui_GButton(tolua_S);
	lua_register_ax_fairygui_GObjectPool(tolua_S);
	lua_register_ax_fairygui_GList(tolua_S);
	lua_register_ax_fairygui_GComboBox(tolua_S);
	lua_register_ax_fairygui_GProgressBar(tolua_S);
	lua_register_ax_fairygui_GSlider(tolua_S);
	lua_register_ax_fairygui_GScrollBar(tolua_S);
	lua_register_ax_fairygui_Window(tolua_S);
	lua_register_ax_fairygui_GRoot(tolua_S);
	lua_register_ax_fairygui_PopupMenu(tolua_S);
	lua_register_ax_fairygui_UIObjectFactory(tolua_S);
	lua_register_ax_fairygui_DragDropManager(tolua_S);
	lua_register_ax_fairygui_GTweener(tolua_S);
	lua_register_ax_fairygui_GTween(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

