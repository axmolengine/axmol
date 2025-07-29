#include "base/Config.h"
#if defined(AX_ENABLE_3D)
#pragma once
#include "tolua++.h"
int register_all_ax_3d(lua_State* tolua_S);
#endif // __ax_3d_h__
