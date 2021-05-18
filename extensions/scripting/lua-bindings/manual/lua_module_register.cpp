/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 c4games.com.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "lua.hpp"

#include "scripting/lua-bindings/manual/lua_module_register.h"

#include "scripting/lua-bindings/manual/network/lua_cocos2dx_network_manual.h"
#include "scripting/lua-bindings/manual/cocostudio/lua_cocos2dx_coco_studio_manual.hpp"
#include "scripting/lua-bindings/manual/extension/lua_cocos2dx_extension_manual.h"
#include "scripting/lua-bindings/manual/ui/lua_cocos2dx_ui_manual.hpp"
#include "scripting/lua-bindings/manual/spine/lua_cocos2dx_spine_manual.hpp"
#include "scripting/lua-bindings/manual/3d/lua_cocos2dx_3d_manual.h"
#include "scripting/lua-bindings/manual/audioengine/lua_cocos2dx_audioengine_manual.h"
#include "scripting/lua-bindings/manual/physics3d/lua_cocos2dx_physics3d_manual.h"
#include "scripting/lua-bindings/manual/navmesh/lua_cocos2dx_navmesh_manual.h"
#include "scripting/lua-bindings/lua-cjson/lua_cjson.h"
#include "yasio/bindings/yasio_cclua.hpp"

static void lua_register_extensions(lua_State* L) {

    static luaL_Reg lua_exts[] = {
        {"yasio", luaopen_yasio_cclua},
        {"cjson", luaopen_cjson},
        {NULL, NULL}
    };

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    auto lib = lua_exts;
    for (; lib->func; ++lib) {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);
}

int lua_module_register(lua_State* L)
{
    // Don't change the module register order unless you know what your are doing
    register_network_module(L);
    register_cocostudio_module(L);
    register_ui_module(L);
    register_extension_module(L);
//TODO arnold
    register_spine_module(L);
    register_cocos3d_module(L);
    register_audioengine_module(L);
#if CC_USE_3D_PHYSICS && CC_ENABLE_BULLET_INTEGRATION
    register_physics3d_module(L);
#endif
#if CC_USE_NAVMESH
    register_navmesh_module(L);
#endif

    // register extensions: yaiso, lua-cjson
    lua_register_extensions(L);
    return 1;
}

