#include "base/ccConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#ifndef __ax_audioengine_h__
#define __ax_audioengine_h__

#include "tolua++.h"

int register_all_ax_audioengine(lua_State* tolua_S);































#endif // __ax_audioengine_h__
#endif //#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
