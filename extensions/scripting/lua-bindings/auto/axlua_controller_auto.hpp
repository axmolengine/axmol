#include "base/ccConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#ifndef __axys_controller_h__
#define __axys_controller_h__

#include "tolua++.h"

int register_all_axys_controller(lua_State* tolua_S);






















#endif // __axys_controller_h__
#endif //#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
