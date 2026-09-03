#pragma once

#include "lua.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace axlua::adapter
{
inline constexpr auto kFunctionRegistry    = "axlua.function.refs";
inline constexpr auto kValueRootRegistry   = "axlua.value.root";
inline constexpr auto kObjectBoxRegistry   = "axlua.object.box";
inline constexpr auto kOwnedObjectRegistry = "axlua.object.owned";

struct Error
{
    int index;
    int array;
    const char* type;
};

void open(lua_State* state);
void register_usertype(lua_State* state, const char* type);
void module(lua_State* state, const char* name, int hasVariables);
void begin_module(lua_State* state, const char* name);
void end_module(lua_State* state);
void register_class(lua_State* state,
                    const char* luaName,
                    const char* nativeName,
                    const char* base,
                    lua_CFunction collector);
void set_function(lua_State* state, const char* name, lua_CFunction function);
void set_constant(lua_State* state, const char* name, lua_Number value);
void set_variable(lua_State* state, const char* name, lua_CFunction getter, lua_CFunction setter);
int register_gc(lua_State* state, int index);

const char* type_name(lua_State* state, int index);
void raise_error(lua_State* state, const char* message, Error* error);
int is_no_object(lua_State* state, int index, Error* error);
int is_boolean(lua_State* state, int index, int hasDefault, Error* error);
int is_number(lua_State* state, int index, int hasDefault, Error* error);
int is_string(lua_State* state, int index, int hasDefault, Error* error);
int is_table(lua_State* state, int index, int hasDefault, Error* error);
int is_usertable(lua_State* state, int index, const char* type, int hasDefault, Error* error);
int is_usertype(lua_State* state, int index, const char* type, int hasDefault, Error* error);
int is_function(lua_State* state, int index, const char* type, int hasDefault, Error* error);

lua_Number to_number(lua_State* state, int index, lua_Number defaultValue);
lua_Integer to_integer(lua_State* state, int index, lua_Integer defaultValue);
const char* to_string(lua_State* state, int index, const char* defaultValue);
void* to_usertype(lua_State* state, int index, void* defaultValue);
int to_boolean(lua_State* state, int index, int defaultValue);
lua_Number to_field_number(lua_State* state, int tableIndex, int field, lua_Number defaultValue);

void push_usertype(lua_State* state, void* value, const char* type);
void push_usertype_rooted(lua_State* state, void* value, const char* type);

void open_bindings(lua_State* state);
int push_object(lua_State* state, void* pointer, const char* type);
int ref_function(lua_State* state, int index, int defaultValue);
void push_function(lua_State* state, int reference);
void remove_function(lua_State* state, int reference);

inline void push_string_view(lua_State* state, std::string_view value)
{
    lua_pushlstring(state, value.data(), value.size());
}

// Unlike Lua's lua_pushliteral macro, this helper passes the literal length
// directly to lua_pushlstring and never performs a runtime strlen.
template <std::size_t N>
inline void push_literal(lua_State* state, const char (&value)[N])
{
    static_assert(N > 0, "a literal must include its terminating nul");
    lua_pushlstring(state, value, N - 1);
}
}  // namespace axlua::adapter
