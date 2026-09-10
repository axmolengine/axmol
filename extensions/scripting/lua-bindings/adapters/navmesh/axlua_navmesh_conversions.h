/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#if defined(AX_ENABLE_NAVMESH)

#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "axmol/platform/PlatformMacros.h"

namespace ax
{
struct NavMeshAgentParam;
struct OffMeshLinkData;
}  // namespace ax

extern bool luaval_to_navmeshagentparam(lua_State* L,
                                        int lo,
                                        ax::NavMeshAgentParam* outValue,
                                        const char* funcName = "");
extern bool luaval_to_offmeshlinkdata(lua_State* L, int lo, ax::OffMeshLinkData* outValue, const char* funcName = "");

extern void navmeshagentparam_to_luaval(lua_State* L, const ax::NavMeshAgentParam& inValue);
extern void offmeshlinkdata_to_luaval(lua_State* L, const ax::OffMeshLinkData& inValue);

#endif  // #if defined(AX_ENABLE_NAVMESH)
