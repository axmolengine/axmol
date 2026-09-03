#pragma once

struct lua_State;

// Execute the headless Lua binding regression suite against an initialized
// Lua VM. The lua-tests application invokes this only in smoke mode.
int runLuaBindingSmoke(lua_State* state);

// Destroy the primary engine/VM and verify both lua_close and late callbacks.
int finishLuaBindingSmoke(lua_State* state);
