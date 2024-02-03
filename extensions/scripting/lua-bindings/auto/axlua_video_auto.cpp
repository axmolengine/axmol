#include "lua-bindings/auto/axlua_video_auto.hpp"
#include "ui/UIMediaPlayer.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
TOLUA_API int register_all_ax_video(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"axui",0);
	tolua_beginmodule(tolua_S,"axui");


	tolua_endmodule(tolua_S);
	return 1;
}

