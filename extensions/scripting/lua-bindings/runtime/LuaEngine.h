/****************************************************************************
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#pragma once

extern "C" {
#include "lua.h"
}

#include "axmol/base/ScriptSupport.h"
#include "lua-bindings/runtime/LuaStack.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "lua-bindings/runtime/Lua-BindingsExport.h"

/**
 * @addtogroup lua
 * @{
 */

namespace ax
{

/**
 * The Lua engine integrated into the axmol to process the interactive operation between lua and c++.
 *
 * @lua NA
 */
class AX_LUA_DLL LuaEngine : public ScriptEngineProtocol
{
public:
    /**
     * Get instance of LuaEngine.
     *
     * @return the instance of LuaEngine.
     */
    static LuaEngine* getInstance(void);

    /**
     * Destructor of LuaEngine.
     */
    virtual ~LuaEngine(void);

    /**
     * Get ccScriptType of LuaEngine used, it is always kScriptTypeLua.
     *
     * @return kScriptTypeLua.
     */
    ccScriptType getScriptType() override { return kScriptTypeLua; };

    /**
     * Get LuaStack of the LuaEngine.
     * All the interactive operation are all base on the LuaStack.
     *
     * @return LuaStack object.
     */
    LuaStack* getLuaStack(void) { return _stack; }

    /**
     * Add a path to find lua files in.
     *
     * @param path to be added to the Lua path.
     */
    virtual void addSearchPath(const char* path);

    /**
     * Add lua loader.
     *
     * @param func a function pointer point to the loader function.
     */
    virtual void addLuaLoader(lua_CFunction func);

    /**
     * Reload script code corresponding to moduleFileName.
     * If value of package["loaded"][moduleFileName] is existed, it would set the value nil.Then,it calls executeString
     * function.
     *
     * @param moduleFileName String object holding the filename of the script file that is to be executed.
     * @return 0 if the string is executed correctly or other if the string is executed wrongly.
     */
    virtual int reload(const char* moduleFileName);

    /**
     * Invalidate the native object's Lua identity, userdata ownership, root,
     * and registered handlers when the native object is destroyed.
     *
     * This function is called by Object destruction; application code should
     * not call it directly.
     *
     * @param object the key object to remove script object.
     */
    void removeScriptObjectByObject(Object* object) override;

    /**
     * Remove the Lua function reference identified by nHandler.
     *
     * @param nHandler the function reference index to find the corresponding Lua function pointer.
     */
    void removeScriptHandler(int nHandler) override;

    /**
     * Execute script code contained in the given string.
     *
     * @param codes holding the valid script code that should be executed.
     * @return 0 if the string is executed correctly, other if the string is executed wrongly.
     */
    int executeString(const char* codes) override;

    /**
     * Execute a script file.
     *
     * @param filename String object holding the filename of the script file that is to be executed.
     * @return the return values by calling executeFunction.
     */
    int executeScriptFile(const char* filename) override;

    /**
     * Execute a scripted global function.
     * The function should not take any parameters and should return an integer.
     *
     * @param functionName String object holding the name of the function, in the global script environment, that is to
     * be executed.
     * @return The integer value returned from the script function.
     */
    int executeGlobalFunction(const char* functionName) override;
    /**
     * Handle the assert message.
     *
     * @return return true if current _callFromLua of LuaStack is not equal to 0 otherwise return false.
     */
    bool handleAssert(const char* msg) override;

    /**
     * Parse the config information data.
     *
     * @param type in current mechanism,it always ConfigType::COCOSTUDIO.
     * @param str  the information data.
     * @return if __onParseConfig function exist in the Lua, it return the value that _stack->executeFunction returns
     * otherwise return false.
     */
    bool parseConfig(ConfigType type, std::string_view str) override;

    /** Forward engine events; modern adapters own their callbacks directly. */
    int sendEvent(const ScriptEvent& message) override;

private:
    LuaEngine(void) : _stack(nullptr) {}
    bool init(void);

private:
    static LuaEngine* _defaultEngine;
    LuaStack* _stack;
};

}  // namespace ax

// end group
/// @}
