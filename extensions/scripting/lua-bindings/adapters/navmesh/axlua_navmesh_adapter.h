/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#if defined(AX_ENABLE_NAVMESH)

#    include "lua-bindings/runtime/axlua_adapter.h"

/**
 * @addtogroup lua
 * @{
 */

/**
 * Call this function can import the lua bindings for the navmesh module.
 * After registering, we could call the related navmesh code conveniently in the lua.
 * If you don't want to use the navmesh module in the lua, you only don't call this registering function.
 * If you don't register the navmesh module, the package size would become smaller .
 * The current mechanism,this function is called in the lua_module_register.h
 */

int register_navmesh_module(lua_State* L);

// end group
/// @}

#endif  // #if defined(AX_ENABLE_NAVMESH)
