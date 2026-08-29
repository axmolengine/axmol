/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#include "lua-bindings/runtime/AxluaCallbackRegistry.h"
#include <map>
#include <string>

#include "lua-bindings/runtime/LuaStack.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/CustomEventListener.h"

namespace ax
{

LuaCallFunc* LuaCallFunc::create(const std::function<void(void*, Node*)>& func)
{
    auto ret = new LuaCallFunc();

    if (ret->initWithFunction(func))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

void LuaCallFunc::execute()
{
    if (_functionLua)
    {
        _functionLua((void*)this, _target);
    }
    else
    {
        CallFuncN::execute();
    }
}

bool LuaCallFunc::initWithFunction(const std::function<void(void*, Node*)>& func)
{
    _functionLua = func;
    return true;
}

LuaCallFunc* LuaCallFunc::clone() const
{
    int handler =
        AxluaCallbackRegistry::getInstance()->getObjectHandler((void*)this, AxluaCallbackRegistry::HandlerType::CALLFUNC);

    if (0 == handler)
        return nullptr;

    auto ret = new LuaCallFunc();

    if (_functionLua)
        ret->initWithFunction(_functionLua);

    ret->autorelease();

    int newscriptHandler = ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

    AxluaCallbackRegistry::getInstance()->addObjectHandler((void*)ret, newscriptHandler,
                                                      AxluaCallbackRegistry::HandlerType::CALLFUNC);

    return ret;
}

AxluaCallbackRegistry* AxluaCallbackRegistry::_instance = nullptr;

AxluaCallbackRegistry::AxluaCallbackRegistry() {}

AxluaCallbackRegistry::~AxluaCallbackRegistry()
{
    if (_dispatcher != nullptr && _disposingListener != nullptr)
        _dispatcher->removeEventListener(_disposingListener);
    _disposingListener = nullptr;
    _mapObjectHandlers.clear();
}

AxluaCallbackRegistry* AxluaCallbackRegistry::getInstance()
{
    if (nullptr == _instance)
    {
        _instance = new AxluaCallbackRegistry();
        _instance->init();
    }
    return _instance;
}

AxluaCallbackRegistry* AxluaCallbackRegistry::getInstanceIfExists()
{
    return _instance;
}

void AxluaCallbackRegistry::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}

void AxluaCallbackRegistry::init()
{
    _mapObjectHandlers.clear();
    _dispatcher = Director::getInstance()->getEventDispatcher();
    _disposingListener = static_cast<EventListener*>(_dispatcher->addCustomEventListener(
        Director::EVENT_DISPOSING, [](CustomEvent*) { AxluaCallbackRegistry::destroyInstance(); }, 1));
}

void AxluaCallbackRegistry::addObjectHandler(void* object, int handler, AxluaCallbackRegistry::HandlerType handlerType)
{
    if (nullptr == object)
        return;

    // may be not need
    removeObjectHandler(object, handlerType);

    auto iter = _mapObjectHandlers.find(object);
    VecHandlerPairs vecHandlers;
    vecHandlers.clear();
    if (_mapObjectHandlers.end() != iter)
        vecHandlers = iter->second;

    HandlerPair eventHanler = std::make_pair(handlerType, handler);
    vecHandlers.emplace_back(eventHanler);
    _mapObjectHandlers[object] = vecHandlers;
}
void AxluaCallbackRegistry::removeObjectHandler(void* object, AxluaCallbackRegistry::HandlerType handlerType)
{
    if (nullptr == object || _mapObjectHandlers.empty())
        return;

    auto iterMap = _mapObjectHandlers.find(object);
    if (_mapObjectHandlers.end() == iterMap)
        return;

    if (iterMap->second.empty())
        return;

    auto iterVec = iterMap->second.begin();
    for (; iterVec != iterMap->second.end(); ++iterVec)
    {
        if (iterVec->first == handlerType)
        {
            if (auto* engine = ScriptEngineManager::getScriptEngineIfExists())
                engine->removeScriptHandler(iterVec->second);
            iterMap->second.erase(iterVec);
            if (iterMap->second.empty())
            {
                _mapObjectHandlers.erase(iterMap);
            }
            break;
        }
    }
}

int AxluaCallbackRegistry::getObjectHandler(void* object, AxluaCallbackRegistry::HandlerType handlerType)
{
    if (nullptr == object || _mapObjectHandlers.empty())
        return 0;

    auto iter = _mapObjectHandlers.find(object);
    if (_mapObjectHandlers.end() != iter)
    {
        for (auto& handlerPair : iter->second)
            if (handlerPair.first == handlerType)
                return handlerPair.second;
    }

    return 0;
}

void AxluaCallbackRegistry::removeObjectAllHandlers(void* object)
{
    if (nullptr == object || _mapObjectHandlers.empty())
        return;

    auto iter = _mapObjectHandlers.find(object);
    if (_mapObjectHandlers.end() != iter)
    {
        if (!iter->second.empty())
        {
            for (auto& handlerPair : iter->second)
            {
                if (auto* engine = ScriptEngineManager::getScriptEngineIfExists())
                    engine->removeScriptHandler(handlerPair.second);
            }

            (iter->second).clear();
        }
        _mapObjectHandlers.erase(iter);
    }
}

AxluaCallbackRegistry::HandlerType AxluaCallbackRegistry::addCustomHandler(void* object, int handler)
{
    assert(nullptr != object);

    auto iter = _mapObjectHandlers.find(object);
    VecHandlerPairs vecHandlers;
    vecHandlers.clear();
    HandlerType handlerType = HandlerType::EVENT_CUSTOM_BEGAN;

    if (_mapObjectHandlers.end() != iter)
    {
        vecHandlers = iter->second;
        if (!vecHandlers.empty())
            handlerType = static_cast<HandlerType>((int)vecHandlers.back().first + 1);
    }
    assert(handlerType <= HandlerType::EVENT_CUSTOM_ENDED);

    HandlerPair eventHanler = std::make_pair(handlerType, handler);
    vecHandlers.emplace_back(eventHanler);
    _mapObjectHandlers[object] = vecHandlers;

    return handlerType;
}

}  // namespace ax
