#pragma once
#if defined(AX_ENABLE_PHYSICS_3D)
#include "axmol/base/Config.h"
#include "tolua++.h"
int register_all_ax_physics3d(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_PHYSICS_3D)
