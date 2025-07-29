#include "base/Config.h"
#if defined(AX_ENABLE_NAVMESH)
#pragma once
#include "tolua++.h"
int register_all_ax_navmesh(lua_State* tolua_S);
#endif // __ax_navmesh_h__
