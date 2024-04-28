#include "base/Config.h"
#if defined(AX_ENABLE_NAVMESH)
#ifndef __ax_navmesh_h__
#define __ax_navmesh_h__
#include "tolua++.h"
int register_all_ax_navmesh(lua_State* tolua_S);
#endif // __ax_navmesh_h__
#endif //#if defined(AX_ENABLE_NAVMESH)
