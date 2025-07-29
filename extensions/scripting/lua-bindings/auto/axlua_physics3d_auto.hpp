#pragma once
#if defined(AX_ENABLE_3D_PHYSICS)
#include "base/Config.h"
#include "tolua++.h"
int register_all_ax_physics3d(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_3D_PHYSICS)
