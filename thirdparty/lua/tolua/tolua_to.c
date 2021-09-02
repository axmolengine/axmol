/* tolua: funcitons to convert to C types
** Support code for Lua bindings.
** Written by Waldemar Celes
** TeCGraf/PUC-Rio
** Apr 2003
** $Id: $
*/

/* This code is free software; you can redistribute it and/or modify it.
** The software provided hereunder is on an "as is" basis, and
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications.
*/

#include "tolua++.h"

#include <string.h>
#include <stdlib.h>

#if LUA_VERSION_NUM >= 504
static int libsize(const luaL_Reg* l) {
    int size = 0;
    for (; l && l->name; l++) size++;
    return size;
}

TOLUA_API const char* luaL_findtable(lua_State* L, int idx,
    const char* fname, int szhint) {
    const char* e;
    if (idx) lua_pushvalue(L, idx);
    do {
        e = strchr(fname, '.');
        if (e == NULL) e = fname + strlen(fname);
        lua_pushlstring(L, fname, e - fname);
        if (lua_rawget(L, -2) == LUA_TNIL) {  /* no such field? */
            lua_pop(L, 1);  /* remove this nil */
            lua_createtable(L, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
            lua_pushlstring(L, fname, e - fname);
            lua_pushvalue(L, -2);
            lua_settable(L, -4);  /* set new table into field */
        }
        else if (!lua_istable(L, -1)) {  /* field has a non-table value? */
            lua_pop(L, 2);  /* remove table and value */
            return fname;  /* return problematic part of the name */
        }
        lua_remove(L, -2);  /* remove previous table */
        fname = e + 1;
    } while (*e == '.');
    return NULL;
}

TOLUA_API void luaL_pushmodule(lua_State* L, const char* modname,
    int sizehint) {
    luaL_findtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE, 1);
    if (lua_getfield(L, -1, modname) != LUA_TTABLE) {  /* no LOADED[modname]? */
        lua_pop(L, 1);  /* remove previous result */
        /* try global variable (and create one if it does not exist) */
        lua_pushglobaltable(L);
        if (luaL_findtable(L, 0, modname, sizehint) != NULL)
            luaL_error(L, "name conflict for module '%s'", modname);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, modname);  /* LOADED[modname] = new table */
    }
    lua_remove(L, -2);  /* remove LOADED table */
}

TOLUA_API void luaL_openlib(lua_State* L, const char* libname,
    const luaL_Reg* l, int nup) {
    luaL_checkversion(L);
    if (libname) {
        luaL_pushmodule(L, libname, libsize(l));  /* get/create library table */
        lua_insert(L, -(nup + 1));  /* move library table to below upvalues */
    }
    if (l)
        luaL_setfuncs(L, l, nup);
    else
        lua_pop(L, nup);  /* remove upvalues */
}
#endif

TOLUA_API lua_Number tolua_tonumber (lua_State* L, int narg, lua_Number def)
{
    return lua_gettop(L)<abs(narg) ? def : lua_tonumber(L,narg);
}
TOLUA_API lua_Integer tolua_tointeger(lua_State* L, int narg, lua_Integer def) {
    return lua_gettop(L) < abs(narg) ? def : lua_tointeger(L, narg);
}

TOLUA_API const char* tolua_tostring (lua_State* L, int narg, const char* def)
{
    return lua_gettop(L)<abs(narg) ? def : lua_tostring(L,narg);
}

TOLUA_API void* tolua_touserdata (lua_State* L, int narg, void* def)
{

    /* return lua_gettop(L)<abs(narg) ? def : lua_touserdata(L,narg); */

    if (lua_gettop(L)<abs(narg)) {
        return def;
    };

    if (lua_islightuserdata(L, narg)) {

        return lua_touserdata(L,narg);
    };

    return tolua_tousertype(L, narg, def);
}

extern int push_table_instance(lua_State* L, int lo);

TOLUA_API void* tolua_tousertype (lua_State* L, int narg, void* def)
{
    if (lua_gettop(L)<abs(narg))
        return def;
    else
    {
        void* u;
        if (!lua_isuserdata(L, narg)) {
            if (!push_table_instance(L, narg)) return NULL;
        };
        u = lua_touserdata(L,narg);
        return (u==NULL) ? NULL : *((void**)u); /* nil represents NULL */
    }
}

TOLUA_API int tolua_tovalue (lua_State* L, int narg, int def)
{
    return lua_gettop(L)<abs(narg) ? def : narg;
}

TOLUA_API int tolua_toboolean (lua_State* L, int narg, int def)
{
    return lua_gettop(L)<abs(narg) ?  def : lua_toboolean(L,narg);
}

TOLUA_API lua_Number tolua_tofieldnumber (lua_State* L, int lo, int index, lua_Number def)
{
    double v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? def : lua_tonumber(L,-1);
    lua_pop(L,1);
    return v;
}

TOLUA_API const char* tolua_tofieldstring
(lua_State* L, int lo, int index, const char* def)
{
    const char* v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? def : lua_tostring(L,-1);
    lua_pop(L,1);
    return v;
}

TOLUA_API void* tolua_tofielduserdata (lua_State* L, int lo, int index, void* def)
{
    void* v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? def : lua_touserdata(L,-1);
    lua_pop(L,1);
    return v;
}

TOLUA_API void* tolua_tofieldusertype (lua_State* L, int lo, int index, void* def)
{
    void* v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? def : (*(void **)(lua_touserdata(L, -1))); /* lua_unboxpointer(L,-1); */
    lua_pop(L,1);
    return v;
}

TOLUA_API int tolua_tofieldvalue (lua_State* L, int lo, int index, int def)
{
    int v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? def : lo;
    lua_pop(L,1);
    return v;
}

TOLUA_API int tolua_getfieldboolean (lua_State* L, int lo, int index, int def)
{
    int v;
    lua_pushnumber(L,index);
    lua_gettable(L,lo);
    v = lua_isnil(L,-1) ? 0 : lua_toboolean(L,-1);
    lua_pop(L,1);
    return v;
}
