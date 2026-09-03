/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

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

#include "lua-bindings/runtime/lua_module_register.h"

#include "lua-bindings/adapters/network/axlua_network_adapter.h"
#include "lua-bindings/adapters/sceneext/axlua_sceneext_adapter.hpp"
#include "lua-bindings/adapters/sceneio/axlua_sceneio_adapter.hpp"
#include "lua-bindings/adapters/extension/axlua_extension_adapter.h"
#include "lua-bindings/adapters/ui/axlua_ui_adapter.hpp"
#include "lua-bindings/adapters/spine/axlua_spine_adapter.hpp"
#include "lua-bindings/adapters/3d/axlua_3d_adapter.h"
#include "lua-bindings/adapters/physics3d/axlua_physics3d_adapter.h"
#include "lua-bindings/adapters/navmesh/axlua_navmesh_adapter.h"
#include "lua-bindings/adapters/fairygui/axlua_fairygui_adapter.hpp"
#include "lua-bindings/generated/axlua_audioengine_gen.h"
#include "lua_cjson.h"
#include "yasio/bindings/yasio_axlua.hpp"

static void lua_register_extensions(lua_State* L)
{

    static luaL_Reg lua_exts[] = {{"yasio", luaopen_yasio_axlua}, {"cjson", luaopen_cjson}, {NULL, NULL}};

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    auto lib = lua_exts;
    for (; lib->func; ++lib)
    {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);
}

int lua_module_register(lua_State* L)
{
    // Don't change the module register order unless you know what your are doing
    register_network_module(L);
    register_sceneext_module(L);
    register_sceneio_module(L);
    register_ui_module(L);
    register_extension_module(L);
    // TODO arnold
#if defined(AX_ENABLE_EXT_SPINE)
    register_spine_module(L);
#endif  // defined(AX_ENABLE_EXT_SPINE)
    register_ax_3d_module(L);
#if defined(AX_ENABLE_AUDIO) && (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || \
                                 AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) ||                           \
                                 AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
    register_all_ax_audioengine(L);
#endif
#if defined(AX_ENABLE_PHYSICS_3D)
    register_physics3d_module(L);
#endif
#if defined(AX_ENABLE_NAVMESH)
    register_navmesh_module(L);
#endif
#if defined(AX_ENABLE_EXT_FAIRYGUI)
    register_fairygui_module(L);
#endif  // defined(AX_ENABLE_EXT_FAIRYGUI)
    // register extensions: yaiso, lua-cjson
    lua_register_extensions(L);
    return 1;
}
