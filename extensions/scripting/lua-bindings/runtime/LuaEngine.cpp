/****************************************************************************
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 https://axmol.dev/
 ****************************************************************************/
#include "lua-bindings/runtime/LuaEngine.h"

namespace ax
{
LuaEngine* LuaEngine::_defaultEngine = nullptr;

LuaEngine* LuaEngine::getInstance(void)
{
    if (!_defaultEngine)
    {
        _defaultEngine = new LuaEngine();
        _defaultEngine->init();
    }
    return _defaultEngine;
}

LuaEngine::~LuaEngine(void)
{
    LuaStack* stack = _stack;
    _stack = nullptr;
    AX_SAFE_RELEASE(stack);
    _defaultEngine = nullptr;
}

bool LuaEngine::init(void)
{
    _stack = LuaStack::create();
    _stack->retain();
    return true;
}

void LuaEngine::addSearchPath(const char* path) { _stack->addSearchPath(path); }
void LuaEngine::addLuaLoader(lua_CFunction func) { _stack->addLuaLoader(func); }

void LuaEngine::removeScriptObjectByObject(Object* object)
{
    if (_stack != nullptr && _stack->getLuaState() != nullptr)
        _stack->removeScriptObjectByObject(object);
}

void LuaEngine::removeScriptHandler(int handler) { _stack->removeScriptHandler(handler); }

int LuaEngine::executeString(const char* code)
{
    const int result = _stack->executeString(code);
    _stack->clean();
    return result;
}

int LuaEngine::executeScriptFile(const char* filename)
{
    const int result = _stack->executeScriptFile(filename);
    _stack->clean();
    return result;
}

int LuaEngine::executeGlobalFunction(const char* functionName)
{
    const int result = _stack->executeGlobalFunction(functionName);
    _stack->clean();
    return result;
}

bool LuaEngine::handleAssert(const char* message)
{
    const bool result = _stack->handleAssert(message);
    _stack->clean();
    return result;
}

bool LuaEngine::parseConfig(ConfigType type, std::string_view value)
{
    lua_State* state = _stack->getLuaState();
    lua_getglobal(state, "__onParseConfig");
    if (!lua_isfunction(state, -1))
    {
        AXLOGW("[LUA WARN] name '{}' does not represent a Lua function", "__onParseConfig");
        lua_pop(state, 1);
        return false;
    }
    _stack->pushInt(static_cast<int>(type));
    _stack->pushString(value.data(), value.length());
    const int result = _stack->executeFunction(2);
    _stack->clean();
    return result != 0;
}

int LuaEngine::sendEvent(const ScriptEvent& event)
{
    (void)event;
    return 0;
}

int LuaEngine::reload(const char* moduleFileName) { return _stack->reload(moduleFileName); }
}  // namespace ax
