/*
** axmol spec lua APIs, DO NOT DELETE
*/
#pragma once

#include "lua.h"

/*
** lua_pushistring
**
** Create and push a new Lua long string object of length `l` directly
** using luaS_createlngstrobj(), returning a writable buffer pointer.
**
** Unlike lua_pushstring/lua_pushlstring, this API is designed for
** in-place string construction: the caller can fill the returned buffer
** with native data (e.g. file contents, I/O streams) without incurring
** an extra memory copy.
**
** This makes it especially useful for scenarios such as:
**   - Reading large files or binary blobs directly into Lua strings
**   - Integrating with native libraries that produce data buffers
**   - Avoiding redundant allocations when bridging external data sources
**
** The string is managed by Lua's GC once pushed, and does not require
** manual freeing by the caller.
**
** Note: This API is only applicable to standard Lua (non-LuaJIT),
** as LuaJIT has its own internal string representation and memory model.
*/
#if defined(__cplusplus)
extern "C" {
#endif

LUA_API char*(lua_pushistring)(lua_State * L, size_t l);

#if defined(__cplusplus)
}
#endif
