#include "base/ccConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#ifndef __axis_controller_h__
#define __axis_controller_h__

#include "tolua++.h"

int register_all_axis_controller(lua_State* tolua_S);






















#endif // __axis_controller_h__
#endif //#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
