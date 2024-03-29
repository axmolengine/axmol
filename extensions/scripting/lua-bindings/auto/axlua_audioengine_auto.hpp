#include "base/Config.h"
#if defined(AX_ENABLE_AUDIO) && (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
#ifndef __ax_audioengine_h__
#define __ax_audioengine_h__
#include "tolua++.h"
int register_all_ax_audioengine(lua_State* tolua_S);
#endif // __ax_audioengine_h__
#endif //#if defined(AX_ENABLE_AUDIO) && (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
