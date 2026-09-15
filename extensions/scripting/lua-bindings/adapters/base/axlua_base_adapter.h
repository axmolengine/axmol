/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
