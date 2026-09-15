/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#if defined(AX_ENABLE_PHYSICS_2D)

#    include "lua-bindings/runtime/axlua_adapter.h"

int register_all_ax_physics2d_adapter(lua_State* luaState);

#endif  // defined(AX_ENABLE_PHYSICS)
