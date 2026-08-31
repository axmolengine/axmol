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

#include "axmol/axmol.h"
#include "AppDelegate.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "lua_assetsmanager_test_sample.h"
#include "lua-bindings/runtime/lua_module_register.h"
#include "lua_test_bindings.h"
#include "LuaBindingSmoke.h"

#include <stdlib.h>

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include <emscripten/emscripten.h>
#endif

using namespace ax;

namespace
{
bool luaBindingSmokeRequested()
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
    // Emscripten does not import the host process environment into libc by
    // default. Read the flag directly from Node's process.env for the CTest
    // launcher while retaining getenv for native runners.
    return EM_ASM_INT(
        { return typeof process != = 'undefined' && process.env && process.env.AXMOL_LUA_BINDING_SMOKE == = '1'; });
#else
    const char* smoke = getenv("AXMOL_LUA_BINDING_SMOKE");
    return smoke != nullptr && smoke[0] == '1';
#endif
}
}  // namespace

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

void AppDelegate::applicationWillLaunch()
{
    ContextAttrs contextAttrs = {8, 8, 8, 8, 24, 8, 0};

    setContextAttrs(contextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    ax::setLogFmtFlag(ax::LogFmtFlag::Colored);

    // register lua engine
    LuaEngine* pEngine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(pEngine);

    LuaStack* stack = pEngine->getLuaStack();
    lua_State* L    = stack->getLuaState();

    lua_module_register(L);

    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || \
     AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
        register_assetsmanager_test_sample(L);
#endif
        register_test_binding(L);
    }
    if (lua_gettop(L) > 0)
        lua_pop(L, 1);

    // Keep the binding ABI regression in the same application target while
    // allowing CI to run it without entering the render loop or controller
    // scene. `std::exit` propagates the smoke result on every platform whose
    // Application::run() otherwise maps a failed launch to zero.
    if (luaBindingSmokeRequested())
    {
        const int smokeResult    = runLuaBindingSmoke(L);
        const int shutdownResult = finishLuaBindingSmoke(L);
        std::exit(smokeResult != 0 ? smokeResult : shutdownResult);
    }

    FileUtils::getInstance()->addSearchPath("src");
    FileUtils::getInstance()->addSearchPath("res");
    pEngine->executeScriptFile("controller.lua");

#if defined(AX_ENABLE_NAVMESH) && defined(AX_ENABLE_PHYSICS_3D)
#    if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
    const bool navMeshSmoke = EM_ASM_INT(
        { return typeof location != = 'undefined' && new URLSearchParams(location.search).has('navmesh-smoke'); });
#    else
    const char* navMeshSmokeFlag = getenv("AXMOL_NAVMESH_SMOKE");
    const bool navMeshSmoke      = navMeshSmokeFlag != nullptr && navMeshSmokeFlag[0] == '1';
#    endif
    if (navMeshSmoke)
        pEngine->executeScriptFile("NavMeshTest/NavMeshSmoke.lua");
#endif

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->deactivate();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->activate();
}
