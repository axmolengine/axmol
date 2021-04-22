//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YASIO__LUA_HPP
#define YASIO__LUA_HPP

#if !defined(YASIO_LUA_ENABLE_GLOBAL)
#  define YASIO_LUA_ENABLE_GLOBAL 0
#endif

#if defined(_WINDLL)
#  if defined(LUA_LIB)
#    define YASIO_LUA_API __declspec(dllexport)
#  else
#    define YASIO_LUA_API __declspec(dllimport)
#  endif
#else
#  define YASIO_LUA_API
#endif

#if defined(__cplusplus)
extern "C" {
#endif
#if !defined(NS_SLUA)
struct lua_State;
#endif
YASIO_LUA_API int luaopen_yasio(lua_State* L);
YASIO_LUA_API void luaregister_yasio(lua_State* L); // register yasio to package.preload
#if defined(__cplusplus)
}
#endif

#endif
