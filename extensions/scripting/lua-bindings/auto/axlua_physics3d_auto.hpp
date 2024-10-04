#include "base/Config.h"
#if defined(AX_ENABLE_3D_PHYSICS)
#ifndef __ax_physics3d_h__
#define __ax_physics3d_h__
#include "tolua++.h"
int register_all_ax_physics3d(lua_State* tolua_S);
#endif // __ax_physics3d_h__
#endif //#if defined(AX_ENABLE_3D_PHYSICS)
