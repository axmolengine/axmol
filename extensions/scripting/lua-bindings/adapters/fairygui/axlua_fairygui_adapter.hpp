#pragma once
#if defined(AX_ENABLE_EXT_FAIRYGUI)

#    include "lua-bindings/runtime/axlua_adapter.h"

 int register_fairygui_module(lua_State* L);

#endif  // defined(AX_ENABLE_EXT_FAIRYGUI)
