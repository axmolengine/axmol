#include "base/Config.h"
#if defined(AX_ENABLE_3D_PHYSICS)
#pragma once
#include "tolua++.h"
int register_all_ax_physics3d(lua_State* tolua_S);
#endif // __ax_physics3d_h__
