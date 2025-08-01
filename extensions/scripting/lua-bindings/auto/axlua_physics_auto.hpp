#pragma once
#if defined(AX_ENABLE_PHYSICS)
#include "base/Config.h"
#include "tolua++.h"
int register_all_ax_physics(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_PHYSICS)
