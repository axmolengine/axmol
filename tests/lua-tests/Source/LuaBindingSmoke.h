#pragma once

struct lua_State;

// Execute the headless Lua binding regression suite against an initialized
// Lua VM. The lua-tests application invokes this only in smoke mode.
int runLuaBindingSmoke(lua_State* state);
