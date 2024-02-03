#ifndef __COCOS_SCRIPTING_LUA_BINDING_MANUAL_FAIRYGUI_AXLUA_FAIRYGUI_CONVERSIONS_H__
#define __COCOS_SCRIPTING_LUA_BINDING_MANUAL_FAIRYGUI_AXLUA_FAIRYGUI_CONVERSIONS_H__

#include "lua-bindings/manual/tolua_fix.h"
#include "fairygui/FairyGUI.h"

extern void margin_to_luaval(lua_State* L, const fairygui::Margin& _margin);
extern bool luaval_to_margin(lua_State* L, int lo, fairygui::Margin* outValue, const char* funcName = "");

#endif //__COCOS_SCRIPTING_LUA_BINDING_MANUAL_FAIRYGUI_AXLUA_FAIRYGUI_CONVERSIONS_H__
