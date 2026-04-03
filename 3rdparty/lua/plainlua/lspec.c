/*
** axmol spec lua APIs, DO NOT DELETE
*/
#define lspec_c
#define LUA_LIB
#include "lspec.h"
#include "lstring.h"
#include "lapi.h"

LUA_API char* (lua_pushistring) (lua_State *L, size_t l)
{
    TString* ts;

    lua_lock(L);
    ts = luaS_createlngstrobj(L, l);
    setsvalue2s(L, L->top.p, ts);
    api_incr_top(L);
    luaC_checkGC(L);
    lua_unlock(L);
    return getlngstr(ts);
}
