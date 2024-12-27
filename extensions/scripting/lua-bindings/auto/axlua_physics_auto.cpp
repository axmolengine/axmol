#include "lua-bindings/auto/axlua_physics_auto.hpp"
#if defined(AX_ENABLE_PHYSICS)
#include "axmol.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
TOLUA_API int register_all_ax_physics(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");


	tolua_endmodule(tolua_S);
	return 1;
}

#endif
