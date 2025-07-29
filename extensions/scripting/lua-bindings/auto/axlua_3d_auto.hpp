#pragma once
#if defined(AX_ENABLE_3D)
#include "base/Config.h"
#include "tolua++.h"
int register_all_ax_3d(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_3D)
