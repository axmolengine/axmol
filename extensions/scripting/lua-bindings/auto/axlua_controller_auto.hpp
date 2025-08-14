#pragma once
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#include "axmol/base/Config.h"
#include "tolua++.h"
int register_all_ax_controller(lua_State* tolua_S);
#endif //#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
