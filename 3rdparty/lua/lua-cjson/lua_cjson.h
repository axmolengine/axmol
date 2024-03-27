
#ifndef __LUA_CJSON_H_
#define __LUA_CJSON_H_

#include "lua.h"

#define USE_INTERNAL_FPCONV

#if defined(__cplusplus)
extern "C" {
#endif

int luaopen_cjson(lua_State* l);
int luaopen_cjson_safe(lua_State* l);

#if defined(__cplusplus)
}
#endif

#endif // __LUA_CJSON_H_
