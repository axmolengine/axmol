#include "base/Config.h"
#if defined(AX_ENABLE_MEDIA)
#pragma once
#include "tolua++.h"
int register_all_ax_video(lua_State* tolua_S);
#endif // __ax_video_h__
