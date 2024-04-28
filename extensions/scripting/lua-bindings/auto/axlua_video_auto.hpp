#include "base/Config.h"
#if defined(AX_ENABLE_MEDIA)
#ifndef __ax_video_h__
#define __ax_video_h__
#include "tolua++.h"
int register_all_ax_video(lua_State* tolua_S);
#endif // __ax_video_h__
#endif //#if defined(AX_ENABLE_MEDIA)
