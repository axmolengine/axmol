#include "base/ccConfig.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#ifndef __cocos2dx_controller_h__
#define __cocos2dx_controller_h__

#include "tolua++.h"

int register_all_cocos2dx_controller(lua_State* tolua_S);






















#endif // __cocos2dx_controller_h__
#endif //#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
