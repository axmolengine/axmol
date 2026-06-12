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

#include "tolua++.h"

#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"
#include "axmol/base/AccelerationEventListener.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/KeyboardEvent.h"
#include "axmol/scene/Node.h"

namespace ax
{
class LuaCustomEventListener
{
public:
    static CustomEventListener* create(std::string_view eventName);
};

class LuaAccelerationEventListener
{
public:
    static AccelerationEventListener* create();
};
}

using namespace ax;

TOLUA_API int register_all_ax_manual(lua_State* tolua_S);

TOLUA_API int register_all_ax_module_manual(lua_State* tolua_S);

TOLUA_API int register_all_ax_math_manual(lua_State* tolua_S);

TOLUA_API int register_all_ax_shaders_manual(lua_State *tolua_S);

TOLUA_API int register_all_ax_bytearray_manual(lua_State *tulua_S);

struct LuaAccelerationEventData
{
    AccelerationEvent* event;

    explicit LuaAccelerationEventData(AccelerationEvent* inEvent) : event(inEvent) {}
};

struct LuaEventKeyboarData
{
    KeyboardEvent* event;

    explicit LuaEventKeyboarData(KeyboardEvent* inEvent) : event(inEvent) {}
};

struct LuaPointerEventData
{
    PointerEvent* event;

    explicit LuaPointerEventData(PointerEvent* inEvent) : event(inEvent) {}
};

class LuaNode : public ax::Node
{
public:
    virtual ~LuaNode() {}

    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
};

TOLUA_API int tolua_luanode_open(lua_State* tolua_S);
TOLUA_API int register_luanode_manual(lua_State* tolua_S);
