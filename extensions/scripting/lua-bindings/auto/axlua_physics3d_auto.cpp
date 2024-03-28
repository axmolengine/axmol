#include "lua-bindings/auto/axlua_physics3d_auto.hpp"
#if AX_USE_3D_PHYSICS && AX_ENABLE_BULLET_INTEGRATION
#include "physics3d/Physics3D.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
TOLUA_API int register_all_ax_physics3d(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");


	tolua_endmodule(tolua_S);
	return 1;
}

#endif
