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
#pragma once

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_runtime.h"

#include "axmol/base/AccelerationEventListener.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/scene/Node.h"

namespace ax
{
class LuaCustomEventListener
{
public:
    static CustomEventListener* create(std::string_view eventName, axlua::Callback<void(CustomEvent*)> callback);
};

class LuaAccelerationEventListener
{
public:
    static AccelerationEventListener* create(axlua::Callback<void(AccelerationEvent*)> callback);
};
}  // namespace ax

using namespace ax;

int register_all_ax_adapter(lua_State* luaState);

int register_all_ax_module_adapter(lua_State* luaState);

int register_all_ax_math_adapter(lua_State* luaState);

int register_all_ax_shaders_adapter(lua_State* luaState);

int register_all_ax_bytearray_adapter(lua_State* tulua_S);

struct LuaAccelerationEventData
{
    AccelerationEvent* event;

    explicit LuaAccelerationEventData(AccelerationEvent* inEvent) : event(inEvent) {}
};
