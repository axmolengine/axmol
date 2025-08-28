#include "axlua_fairygui_conversions.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

void margin_to_luaval(lua_State* L, const fairygui::Margin& _margin)
{
    if (NULL == L)
        return;
    lua_newtable(L);
    lua_pushstring(L, "left");
    lua_pushnumber(L, (lua_Number)_margin.left);
    lua_rawset(L, -3);
    lua_pushstring(L, "top");
    lua_pushnumber(L, (lua_Number)_margin.top);
    lua_rawset(L, -3);
    lua_pushstring(L, "right");
    lua_pushnumber(L, (lua_Number)_margin.right);
    lua_rawset(L, -3);
    lua_pushstring(L, "bottom");
    lua_pushnumber(L, (lua_Number)_margin.bottom);
    lua_rawset(L, -3);
}

bool luaval_to_margin(lua_State* L, int lo, fairygui::Margin* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "left");
        lua_gettable(L, lo);
        outValue->left = lua_isnil(L, -1) ? 0 : (float)(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "top");
        lua_gettable(L, lo);
        outValue->top = lua_isnil(L, -1) ? 0 : (float)(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "right");
        lua_gettable(L, lo);
        outValue->right = lua_isnil(L, -1) ? 0 : (float)(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "bottom");
        lua_gettable(L, lo);
        outValue->bottom = lua_isnil(L, -1) ? 0 : (float)(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }

    return ok;
}
