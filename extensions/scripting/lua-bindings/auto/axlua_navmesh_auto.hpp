#pragma once
#if defined(AX_ENABLE_NAVMESH)
#include "base/Config.h"
#include "tolua++.h"
int register_all_ax_navmesh(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_NAVMESH)
