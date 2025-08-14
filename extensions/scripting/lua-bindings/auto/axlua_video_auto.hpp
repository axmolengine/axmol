#pragma once
#if defined(AX_ENABLE_MEDIA)
#include "axmol/base/Config.h"
#include "tolua++.h"
int register_all_ax_video(lua_State* tolua_S);
#endif //#if defined(AX_ENABLE_MEDIA)
