#pragma once
#if defined(AX_ENABLE_PHYSICS_2D)
#include "axmol/base/Config.h"
#include "tolua++.h"
int register_all_ax_physics2d(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_PHYSICS_2D)
