#include "base/Config.h"
#if defined(AX_ENABLE_PHYSICS)
#pragma once
#include "tolua++.h"
int register_all_ax_physics(lua_State* tolua_S);
#endif // __ax_physics_h__
