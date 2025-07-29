#include "base/Config.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#pragma once
#include "tolua++.h"
int register_all_ax_controller(lua_State* tolua_S);
#endif // __ax_controller_h__
