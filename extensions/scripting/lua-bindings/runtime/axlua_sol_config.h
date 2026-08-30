#pragma once

// Every translation unit that includes sol2 for the Axmol Lua binding must
// see the same std::function getter. Keeping this in one wrapper avoids an
// ODR split between generated bindings and the yasio bridge.
#include "lua.hpp"

#include <functional>

namespace axlua
{
template <class Signature>
std::function<Signature> make_lua_callback(lua_State* state, int index);

// yasio's Lua bridge is compiled as a separate translation unit and only
// sees this sol2 configuration header.  Keep the callback signature it uses
// as an out-of-line specialization so the bridge does not leave an unresolved
// function-template reference at link time.
template <>
std::function<void()> make_lua_callback<void()>(lua_State* state, int index);
}

#define SOL_AXMOL_STD_FUNCTION_GETTER(Signature, State, Index) axlua::make_lua_callback<Signature>((State), (Index))

#include "sol/sol.hpp"

#undef SOL_AXMOL_STD_FUNCTION_GETTER
