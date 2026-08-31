#pragma once

// Axmol Lua binding runtime.  The generator only emits declarations against
// this small API; object identity and Lua-version differences stay here.

#include "lua-bindings/runtime/axlua_sol_config.h"

// Historical Axmol Lua bindings accepted any Lua number for an integral C++ parameter and applied
// the native conversion.  Keep that long-standing Lua API behavior: sol2's
// optional precision guard rejects values such as sqrt(1 / 19) * 100 before
// the generated binding can call the C++ int overload.
#ifndef SOL_NO_CHECK_NUMBER_PRECISION
#    define SOL_NO_CHECK_NUMBER_PRECISION 1
#endif
#if !SOL_IS_OFF(SOL_NUMBER_PRECISION_CHECKS)
#    error "Axmol Lua bindings require legacy numeric conversions"
#endif
#include "axmol/base/Event.h"
#include "axmol/base/Object.h"
#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_conversions.h"

#include <string_view>
#include <string>
#include <atomic>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace axlua
{

// Owning Lua callback used by adapter delegates.  The callback keeps the Lua
// function alive through LuaCallbackState and invalidates safely when its VM
// shuts down; it does not rely on an object/type/handler global lookup.
template <class Signature>
class Callback;

template <class Return, class... Arguments>
class Callback<Return(Arguments...)>
{
public:
    Callback() = default;
    Callback(lua_State* state, int index) : _function(make_lua_callback<Return(Arguments...)>(state, index)) {}
    explicit Callback(std::function<Return(Arguments...)> function) : _function(std::move(function)) {}

    explicit operator bool() const { return static_cast<bool>(_function); }
    Return operator()(Arguments... arguments) const
    {
        if constexpr (std::is_void_v<Return>)
        {
            if (_function)
                _function(std::forward<Arguments>(arguments)...);
        }
        else
        {
            return _function ? _function(std::forward<Arguments>(arguments)...) : Return{};
        }
    }
    void reset() { _function = {}; }
    const std::function<Return(Arguments...)>& function() const { return _function; }

private:
    std::function<Return(Arguments...)> _function;
};

void drain_pending_callback_unrefs(lua_State* state);

namespace detail
{

struct LuaCallbackRegistry;

class BorrowedObjectScope
{
public:
    explicit BorrowedObjectScope(lua_State* state) : _state(state) {}
    ~BorrowedObjectScope() noexcept { invalidate(); }

    void track(int index);
    void invalidate() noexcept;

private:
    lua_State* _state = nullptr;
    std::vector<int> _references;
};

void invalidate_borrowed_userdata(lua_State* state, int index);

template <class T>
using callback_argument_pointee_t = std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

template <class T>
inline constexpr bool is_borrowed_callback_argument_v =
    std::is_pointer_v<std::remove_reference_t<T>> && std::is_base_of_v<ax::Event, callback_argument_pointee_t<T>>;

template <class T>
int push_borrowed_object(lua_State* state, T* object, BorrowedObjectScope& scope)
{
    if (object == nullptr)
    {
        lua_pushnil(state);
        return 1;
    }

    // Use sol2's registered type metadata, but deliberately bypass Axmol's
    // persistent-object identity/WeakPtr path. Event instances are commonly
    // automatic-storage values owned by the input dispatcher.
    sol::stack::unqualified_pusher<sol::detail::as_pointer_tag<T>>::push(state, object);
    scope.track(-1);
    return 1;
}

template <class T>
void push_callback_argument(lua_State* state, T&& value, BorrowedObjectScope& scope)
{
    if constexpr (is_borrowed_callback_argument_v<T>)
        push_borrowed_object(state, value, scope);
    else
        sol::stack::push(state, std::forward<T>(value));
}

class LuaCallbackState
{
public:
    static std::shared_ptr<LuaCallbackState> create(lua_State* state, int index);

    ~LuaCallbackState();

    void invalidate() noexcept;

    template <class Return, class... Arguments>
    Return invoke(Arguments&&... arguments)
    {
        if (!prepare_invoke())
            return default_callback_result<Return>();

        const int top = lua_gettop(_state);
        lua_rawgeti(_state, LUA_REGISTRYINDEX, _ref);
        if (!lua_isfunction(_state, -1))
        {
            lua_settop(_state, top);
            return default_callback_result<Return>();
        }

        BorrowedObjectScope borrowedObjects(_state);
        (push_callback_argument(_state, std::forward<Arguments>(arguments), borrowedObjects), ...);
        constexpr int resultCount = std::is_void_v<Return> ? 0 : 1;
        if (lua_pcall(_state, sizeof...(Arguments), resultCount, 0) != LUA_OK)
        {
            const char* message = lua_tostring(_state, -1);
            report_callback_error(message != nullptr ? message : "unknown Lua error");
            lua_settop(_state, top);
            return default_callback_result<Return>();
        }

        if constexpr (std::is_void_v<Return>)
        {
            lua_settop(_state, top);
            return;
        }
        else
        {
            // Legacy Lua handlers commonly omit an explicit return value for
            // callbacks whose native result is used only as a capture/accept
            // flag.  Lua's nil has always mapped to the native default (false
            // for bool, zero for arithmetic results); preserve that contract
            // while still rejecting non-nil values of an incompatible type.
            if (lua_isnil(_state, -1))
            {
                lua_settop(_state, top);
                return default_callback_result<Return>();
            }
            if (!sol::stack::check<Return>(_state, -1))
            {
                report_callback_error("Lua callback returned a value with an incompatible type");
                lua_settop(_state, top);
                return default_callback_result<Return>();
            }
            Return result = sol::stack::get<Return>(_state, -1);
            lua_settop(_state, top);
            return result;
        }
    }

private:
    LuaCallbackState(lua_State* state, std::shared_ptr<LuaCallbackRegistry> registry)
        : _state(state), _registry(std::move(registry)), _ownerThread(std::this_thread::get_id())
    {}

    bool prepare_invoke();

    template <class Return>
    static Return default_callback_result()
    {
        if constexpr (std::is_void_v<Return>)
            return;
        else if constexpr (std::is_pointer_v<Return>)
            return nullptr;
        else
            return Return{};
    }

    void report_callback_error(const char* message);

    lua_State* _state = nullptr;
    // The shared registry outlives lua_close when native callbacks survive
    // their VM. A new VM at the same address gets a different registry.
    std::shared_ptr<LuaCallbackRegistry> _registry;
    int _ref          = LUA_NOREF;
    std::atomic_bool _active{false};
    std::thread::id _ownerThread;

    friend std::shared_ptr<LuaCallbackState> create_callback_state(lua_State*, int);
};
}  // namespace detail

void shutdown_callbacks(lua_State* state);

namespace detail
{
template <class Signature>
struct LuaCallbackFactory;

template <class Return, class... Arguments>
struct LuaCallbackFactory<Return(Arguments...)>
{
    static std::function<Return(Arguments...)> create(lua_State* state, int index)
    {
        auto callback = LuaCallbackState::create(state, index);
        // sol2 accepts nil for std::function parameters.  Keep that meaning
        // intact: an empty callback clears the native hook.  Returning a
        // capturing lambda with a null state would defer the failure until
        // the next native event and dereference a null pointer.
        if (!callback)
            return {};
        return [callback = std::move(callback)](Arguments... arguments) -> Return {
            // Keep the callback state alive for the complete native call. This
            // matters when the Lua callback removes its own listener or the
            // owning object is released reentrantly.
            auto activeCallback = callback;
            return activeCallback->template invoke<Return>(std::forward<Arguments>(arguments)...);
        };
    }
};
}  // namespace detail

template <class Signature>
std::function<Signature> make_lua_callback(lua_State* state, int index)
{
    return detail::LuaCallbackFactory<Signature>::create(state, index);
}

bool is_invalid_userdata(lua_State* state, int index);
void remember_object(lua_State* state, ax::Object* object);
void remember_userdata_object(lua_State* state, int index, ax::Object* object);
bool object_userdata_current(lua_State* state, int index, ax::Object* object);
inline int absolute_index(lua_State* state, int index)
{
    return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(state) + index + 1;
}

inline void make_weak_value_table(lua_State* state, int tableIndex)
{
    tableIndex = absolute_index(state, tableIndex);
    if (!lua_getmetatable(state, tableIndex))
        lua_newtable(state);
    const int metatableIndex = absolute_index(state, -1);
    axlua::adapter::push_literal(state, "v");
    lua_setfield(state, metatableIndex, "__mode");
    lua_setmetatable(state, tableIndex);
}

template <class T>
int push_object(lua_State* state, T* object);

using AdapterObjectPusher = int (*)(lua_State* state, void* object);

// Legacy manual functions only retain a runtime type name.  Generated classes
// register a typed bridge here so their return values are still constructed by
// sol2, including all inheritance/type metadata expected by generated methods.
void register_object_pusher(lua_State* state,
                            std::string_view typeName,
                            const std::type_info& nativeType,
                            AdapterObjectPusher pusher);
bool push_registered_object(lua_State* state, void* object, std::string_view typeName);
bool push_registered_dynamic_object(lua_State* state, ax::Object* object, const std::type_info& staticType);

template <class T>
int push_object(lua_State* state, T* object)
{
    if (object == nullptr)
    {
        lua_pushnil(state);
        return 1;
    }

    using NativeT = std::remove_cv_t<T>;
    if constexpr (std::is_base_of_v<ax::Object, NativeT>)
    {
        auto* nativeObject = static_cast<ax::Object*>(const_cast<NativeT*>(object));
        remember_object(state, nativeObject);
        if (push_registered_dynamic_object(state, nativeObject, typeid(NativeT)))
            return 1;
    }

    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        make_weak_value_table(state, -1);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
    }
    const int identityTable = absolute_index(state, -1);
    lua_pushlightuserdata(state, const_cast<void*>(static_cast<const void*>(object)));
    lua_rawget(state, identityTable);
    if (lua_isuserdata(state, -1))
    {
        if constexpr (std::is_base_of_v<ax::Object, NativeT>)
        {
            auto* nativeObject = static_cast<ax::Object*>(const_cast<NativeT*>(object));
            if (!object_userdata_current(state, -1, nativeObject))
            {
                lua_pop(state, 1);
                lua_pushlightuserdata(state, const_cast<void*>(static_cast<const void*>(object)));
                lua_pushnil(state);
                lua_rawset(state, identityTable);
            }
            else
            {
                lua_remove(state, identityTable);
                return 1;
            }
        }
        else
        {
            lua_remove(state, identityTable);
            return 1;
        }
    }
    else
        lua_pop(state, 1);

    sol::stack::unqualified_pusher<sol::detail::as_pointer_tag<T>>::push(state, object);
    const int userdataIndex = absolute_index(state, -1);
    lua_pushlightuserdata(state, const_cast<void*>(static_cast<const void*>(object)));
    lua_pushvalue(state, userdataIndex);
    lua_rawset(state, identityTable);
    if constexpr (std::is_base_of_v<ax::Object, NativeT>)
        remember_userdata_object(state, userdataIndex, static_cast<ax::Object*>(const_cast<NativeT*>(object)));
    lua_remove(state, identityTable);
    return 1;
}

class Module;
int class_index(lua_State* state);
int class_new_index(lua_State* state);
int method_dispatch(lua_State* state);
int static_method_dispatch(lua_State* state);
int overload_method_dispatch(lua_State* state);
int overload_static_method_dispatch(lua_State* state);
void install_class_static_index(lua_State* state, int classTableIndex);

// sol::overload resolves candidates through its own usertype graph.  Axmol
// keeps inheritance and userdata identity in this runtime, so select a native
// callable from the Lua argument signature first, then invoke that callable
// through sol2's normal checked conversion path.
template <class Function, class... Arguments>
struct OverloadCandidate
{
    Function function;
};

template <class... Arguments, class Function>
auto overload_candidate(Function&& function)
{
    return OverloadCandidate<std::decay_t<Function>, Arguments...>{std::forward<Function>(function)};
}

template <class... Candidates>
struct OverloadSet
{
    std::tuple<Candidates...> candidates;
};

template <class... Candidates>
auto overload(Candidates&&... candidates)
{
    return OverloadSet<std::decay_t<Candidates>...>{
        std::tuple<std::decay_t<Candidates>...>(std::forward<Candidates>(candidates)...)};
}

template <class... Arguments>
int overload_signature_matches(lua_State* state)
{
    const auto count   = static_cast<int>(sizeof...(Arguments));
    const bool matches = lua_gettop(state) == count && sol::stack::multi_check<Arguments...>(state, 1, &sol::no_panic);
    lua_pushboolean(state, matches ? 1 : 0);
    return 1;
}

template <class Function, class... Arguments>
void append_overload_candidate(lua_State* state,
                               int candidatesIndex,
                               int candidateIndex,
                               const OverloadCandidate<Function, Arguments...>& candidate)
{
    candidatesIndex = absolute_index(state, candidatesIndex);
    lua_newtable(state);
    const int candidateTable = absolute_index(state, -1);
    sol::stack::push(state, candidate.function);
    lua_rawseti(state, candidateTable, 1);
    lua_pushcfunction(state, &overload_signature_matches<Arguments...>);
    lua_rawseti(state, candidateTable, 2);
    lua_rawseti(state, candidatesIndex, candidateIndex);
}

template <class T>
class Class
{
public:
    Class(sol::table module, std::string_view name, std::string_view typeNamespace)
        : _module(std::move(module)), _name(name), _namespace(typeNamespace), _type(get_or_create_type(_module, _name))
    {
        auto* state = _module.lua_state();
        _module.push();
        lua_getfield(state, -1, _name.c_str());
        if (lua_istable(state, -1))
        {
            _table = sol::table(state, -1);
            lua_pop(state, 2);
        }
        else
        {
            lua_pop(state, 2);
            _type.push();
            _table = sol::table(state, -1);
            _module.set(_name, _table);
            lua_pop(state, 1);
        }
        const std::string qualifiedName = _namespace + "." + _name;
        _table.set("__axlua_name", qualifiedName);
        _table.set(".classname", qualifiedName);
        // Configure sol2 completely before replacing its instance dispatch.
        // sol2 propagates index/new_index changes to every backing metatable;
        // installing Axmol's dispatcher first would therefore be overwritten
        // by the following type configuration.
        _type.set(sol::meta_function::index, &class_index);
        _type.set(sol::meta_function::new_index, &class_new_index);
        register_class_table(_module.lua_state(), _type, _table);
        register_legacy_class_table(_module.lua_state(), qualifiedName, _table);
        register_object_pusher(_module.lua_state(), qualifiedName, typeid(T), &push_legacy_object);
    }

    template <class F>
    Class& method(std::string_view name, F&& function)
    {
        auto callable              = std::forward<F>(function);
        auto* state                = _module.lua_state();
        const auto methodName      = std::string(name);
        const auto qualifiedMethod = _namespace + "." + _name + ":" + methodName;
        _table.push();
        const int classTable = absolute_index(state, -1);
        sol::stack::push(state, callable);
        lua_pushlstring(state, qualifiedMethod.data(), qualifiedMethod.size());
        lua_pushcclosure(state, &method_dispatch, 2);
        lua_setfield(state, classTable, methodName.c_str());
        lua_pop(state, 1);
        return *this;
    }

    template <class... Candidates>
    Class& method(std::string_view name, OverloadSet<Candidates...> overloads)
    {
        return set_overload(name, std::move(overloads), false);
    }

    template <class F>
    Class& static_method(std::string_view name, F&& function)
    {
        auto callable = std::forward<F>(function);
        // Keep the Lua-facing class-table function stable and accept both
        // `ax.Type.method(...)` and the legacy-compatible
        // `ax.Type:method(...)` spelling.  The latter supplies the class
        // table as an implicit first argument, which sol2's native callable
        // adapter quite correctly does not discard by itself.
        auto* state                = _module.lua_state();
        const auto className       = std::string(name);
        const auto qualifiedMethod = _namespace + "." + _name + ":" + className;
        _table.push();
        const int classTable = absolute_index(state, -1);
        sol::stack::push(state, callable);
        lua_pushvalue(state, classTable);
        lua_pushlstring(state, qualifiedMethod.data(), qualifiedMethod.size());
        lua_pushcclosure(state, &static_method_dispatch, 3);
        lua_setfield(state, classTable, className.c_str());
        lua_pop(state, 1);
        return *this;
    }

    template <class... Candidates>
    Class& static_method(std::string_view name, OverloadSet<Candidates...> overloads)
    {
        return set_overload(name, std::move(overloads), true);
    }

    template <class... Constructors>
    Class& constructors()
    {
        _type.set(sol::meta_function::construct, sol::constructors<Constructors...>());
        return *this;
    }

    template <class... B>
    Class& bases()
    {
        _type.set(sol::base_classes, sol::bases<B...>());
        if constexpr (sizeof...(B) > 0)
            set_lua_base<B...>();
        // Adding bases changes sol2's instance dispatch tables. Restore the
        // Axmol dispatcher after that operation for both value and pointer
        // userdata.
        install_class_dispatchers(_module.lua_state(), _table);
        return *this;
    }

    template <class V>
    Class& field(std::string_view name, V T::* member)
    {
        const std::string key(name);
        auto getter = [member](T& self) { return self.*member; };
        auto setter = [member](T& self, V value) { self.*member = std::move(value); };
        set_accessor(".get", key, std::move(getter));
        set_accessor(".set", key, std::move(setter));
        return *this;
    }

    sol::table table() const { return _table; }

    sol::table get_or_create_table(std::string_view name)
    {
        const std::string key(name);
        sol::object existing = _table[key];
        if (existing.valid() && existing.is<sol::table>())
            return existing.as<sol::table>();

        sol::table created = sol::state_view(_module.lua_state()).create_table();
        _table.set(key, created);
        return created;
    }

private:
    template <class... Candidates>
    Class& set_overload(std::string_view name, OverloadSet<Candidates...> overloads, bool isStatic)
    {
        auto* state                = _module.lua_state();
        const auto methodName      = std::string(name);
        const auto qualifiedMethod = _namespace + "." + _name + ":" + methodName;
        _table.push();
        const int classTable = absolute_index(state, -1);
        lua_newtable(state);
        const int candidatesTable = absolute_index(state, -1);
        int candidateIndex        = 1;
        std::apply([&](const auto&... candidate) {
            (append_overload_candidate(state, candidatesTable, candidateIndex++, candidate), ...);
        }, overloads.candidates);
        if (isStatic)
            lua_pushvalue(state, classTable);
        lua_pushlstring(state, qualifiedMethod.data(), qualifiedMethod.size());
        lua_pushcclosure(state, isStatic ? &overload_static_method_dispatch : &overload_method_dispatch,
                         isStatic ? 3 : 2);
        lua_setfield(state, classTable, methodName.c_str());
        lua_pop(state, 1);
        return *this;
    }

    template <class F>
    void set_accessor(std::string_view accessorName, const std::string& key, F&& callable)
    {
        auto* state          = _module.lua_state();
        sol::object existing = _table[std::string(accessorName)];
        sol::table accessors = existing.valid() && existing.is<sol::table>() ? existing.as<sol::table>()
                                                                             : sol::state_view(state).create_table();
        // `table.set` stores an arbitrary lambda as a userdata in this sol2
        // fork.  Accessors are invoked by Lua through class_index/new_index,
        // so they must be materialized as Lua-callable functions explicitly.
        accessors.set_function(key, std::forward<F>(callable));
        _table.set(std::string(accessorName), accessors);
    }

    static void register_legacy_class_table(lua_State* state, const std::string& name, const sol::table& table)
    {
        // The remaining adapter bindings use the legacy registry contract:
        // LUA_REGISTRYINDEX["ax.Type"] is the class
        // table they extend.  Keep that contract backed by the generated
        // class table so manual methods are immediately visible to both
        // instances and static class calls.
        table.push();
        const int classTable = absolute_index(state, -1);

        axlua::adapter::push_literal(state, "__axlua_name");
        lua_pushlstring(state, name.data(), name.size());
        lua_rawset(state, classTable);
        axlua::adapter::push_literal(state, ".classname");
        lua_pushlstring(state, name.data(), name.size());
        lua_rawset(state, classTable);
        axlua::adapter::push_literal(state, "__index");
        lua_pushcfunction(state, &class_index);
        lua_rawset(state, classTable);
        axlua::adapter::push_literal(state, "__newindex");
        lua_pushcfunction(state, &class_new_index);
        lua_rawset(state, classTable);

        // Compatibility code can create a non-owning userdata before sol2
        // has pushed the same native pointer.  Such userdata uses this class
        // table as its metatable, so include it in the same lookup map as the
        // native sol2 value and pointer metatables.
        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            lua_pushvalue(state, -1);
            lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        }
        lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, classTable)));
        lua_pushvalue(state, classTable);
        lua_rawset(state, -3);
        lua_pop(state, 1);

        lua_pushvalue(state, classTable);
        lua_setfield(state, LUA_REGISTRYINDEX, name.c_str());

        lua_pop(state, 1);
    }

    static int push_legacy_object(lua_State* state, void* object)
    {
        return axlua::push_object(state, static_cast<T*>(object));
    }

    static sol::usertype<T> get_or_create_type(sol::table& module, std::string_view name)
    {
        auto* state = module.lua_state();
        module.push();
        const int moduleIndex = absolute_index(state, -1);
        lua_getfield(state, moduleIndex, std::string(name).c_str());
        if (lua_istable(state, -1))
        {
            lua_getfield(state, -1, sol::to_string(sol::meta_function::storage).c_str());
            const bool isSolUsertype = lua_islightuserdata(state, -1);
            lua_pop(state, 1);
            if (isSolUsertype)
            {
                sol::usertype<T> existing(state, -1);
                lua_pop(state, 2);
                return existing;
            }
        }
        lua_pop(state, 2);
        return module.template new_usertype<T>(std::string(name), no_automagic());
    }

    static void register_class_table(lua_State* state, sol::usertype<T>& type, const sol::table& table)
    {
        type.push();
        const int namedType = absolute_index(state, -1);
        // sol2 materializes the pointer metatable lazily on the first T*
        // push.  Axmol needs to register its class-table mapping before that
        // first factory call, otherwise the lazy metatable keeps sol2's native
        // __index/__newindex and bypasses peer fields and generated accessors.
        ensure_pointer_metatable(state);
        register_class_metatable(state, sol::usertype_traits<T>::metatable(), table, false);
        register_class_metatable(state, sol::usertype_traits<T*>::metatable(), table, true);
        lua_remove(state, namedType);
    }

    static void ensure_pointer_metatable(lua_State* state)
    {
        const auto& name  = sol::usertype_traits<T*>::metatable();
        const int created = luaL_newmetatable(state, name.c_str());
        if (created == 1)
        {
            sol::stack::stack_detail::set_undefined_methods_on<T*>(sol::stack_reference(state, -1));
        }
        lua_pop(state, 1);
    }

    static void install_class_dispatchers(lua_State* state, const sol::table& table)
    {
        register_class_metatable(state, sol::usertype_traits<T>::metatable(), table, false);
        register_class_metatable(state, sol::usertype_traits<T*>::metatable(), table, true);
    }

    static void register_class_metatable(lua_State* state,
                                         const std::string& name,
                                         const sol::table& table,
                                         bool pointerMetatable)
    {
        lua_getfield(state, LUA_REGISTRYINDEX, name.c_str());
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            return;
        }
        const int metatable = absolute_index(state, -1);
        const void* key     = lua_topointer(state, metatable);

        // sol2 uses distinct metatables for value and pointer userdata. Most
        // Axmol factories return pointers, so configuring only the named
        // usertype leaves native members ahead of Lua peer methods there.
        // Route both forms through the Axmol dispatcher so a Lua subclass can
        // override native names such as init(), onEnter(), and update().
        lua_pushcfunction(state, &class_index);
        lua_setfield(state, metatable, "__index");
        lua_pushcfunction(state, &class_new_index);
        lua_setfield(state, metatable, "__newindex");

        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            lua_pushvalue(state, -1);
            lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        }
        const int classes = absolute_index(state, -1);
        lua_pushlightuserdata(state, const_cast<void*>(key));
        table.push();
        lua_rawset(state, classes);
        lua_pop(state, 2);

        if (pointerMetatable)
        {
            lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
            if (!lua_istable(state, -1))
            {
                lua_pop(state, 1);
                lua_newtable(state);
                lua_pushvalue(state, -1);
                lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
            }
            const int pointerClasses = absolute_index(state, -1);
            lua_pushlightuserdata(state, const_cast<void*>(key));
            table.push();
            lua_rawset(state, pointerClasses);
            lua_pop(state, 1);
        }
    }

    static sol::constant_automagic_enrollments<sol::automagic_flags::none> no_automagic()
    {
        sol::constant_automagic_enrollments<sol::automagic_flags::none> result;
        result.default_constructor            = false;
        result.destructor                     = false;
        result.pairs_operator                 = false;
        result.to_string_operator             = false;
        result.call_operator                  = false;
        result.less_than_operator             = false;
        result.less_than_or_equal_to_operator = false;
        result.length_operator                = false;
        result.equal_to_operator              = false;
        return result;
    }

    template <class B>
    void set_lua_base()
    {
        auto* state = _module.lua_state();
        if (!push_registered_class_table<B>(state))
            return;
        const int baseTable = absolute_index(state, -1);
        _table.push();
        const int classTable = absolute_index(state, -1);
        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            lua_pushvalue(state, -1);
            lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
        }
        const int bases = absolute_index(state, -1);
        lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, classTable)));
        lua_pushvalue(state, baseTable);
        lua_rawset(state, bases);
        install_class_static_index(state, classTable);
        lua_pop(state, 3);
    }

    template <class B, class B2, class... Rest>
    void set_lua_base()
    {
        // Axmol's native hierarchy is single-inheritance in the generated
        // surface.  sol2 still receives the complete base pack above; the
        // first base is used for Lua table lookup.
        set_lua_base<B>();
    }

    template <class U>
    static bool push_registered_class_table(lua_State* state)
    {
        lua_getfield(state, LUA_REGISTRYINDEX, sol::usertype_traits<U*>::metatable().c_str());
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            return false;
        }
        const void* metatableKey = lua_topointer(state, -1);
        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 2);
            return false;
        }
        lua_pushlightuserdata(state, const_cast<void*>(metatableKey));
        lua_rawget(state, -2);
        lua_remove(state, -2);
        lua_remove(state, -2);
        return lua_istable(state, -1);
    }

    sol::table _module;
    sol::table _table;
    std::string _name;
    std::string _namespace;
    sol::usertype<T> _type;
};

class Module
{
public:
    static Module from(lua_State* state, std::string_view namespaceName = "ax", std::string_view typeNamespace = {});

    template <class T>
    Class<T> class_(std::string_view name)
    {
        return Class<T>(_table, name, _typeNamespace);
    }

    sol::state_view state() const { return _state; }
    sol::table table() const { return _table; }

    sol::table get_or_create_table(std::string_view name)
    {
        const std::string key(name);
        sol::object existing = _table[key];
        if (existing.valid() && existing.is<sol::table>())
            return existing.as<sol::table>();

        sol::table created = _state.create_table();
        _table.set(key, created);
        return created;
    }

private:
    Module(sol::state_view state, sol::table table, std::string namespaceName, std::string typeNamespace)
        : _state(state)
        , _table(std::move(table))
        , _namespace(std::move(namespaceName))
        , _typeNamespace(std::move(typeNamespace))
    {}

    sol::state_view _state;
    sol::table _table;
    std::string _namespace;
    std::string _typeNamespace;
};

// Installs the compatibility surface used by existing Axmol Lua scripts.
// This function is deliberately independent of generated modules.
void install(lua_State* state);
void shutdown(lua_State* state);

// Stable userdata/peer operations used by LuaStack and generated/manual code.
void push_peer(lua_State* state, int userdataIndex);
void set_peer(lua_State* state, int userdataIndex, int peerIndex);
void invalidate_object(lua_State* state, void* object);

}  // namespace axlua

// sol2's ADL extension points are intentionally kept in ax's namespace.  A
// returned Axmol pointer therefore goes through the canonical identity table
// before sol2 allocates a new userdata.
namespace ax
{
#define AX_LUA_TABLE_VALUE_ADAPTER(Type, PushFunction, GetFunction)                                                    \
    inline int sol_lua_push(lua_State* state, const Type& value)                                                       \
    {                                                                                                                  \
        ::PushFunction(state, value);                                                                                  \
        return 1;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    inline Type sol_lua_get(sol::types<Type>, lua_State* state, int index, sol::stack::record& tracking)               \
    {                                                                                                                  \
        tracking.use(1);                                                                                               \
        Type value{};                                                                                                  \
        if (!::GetFunction(state, index, &value, "Axmol Lua binding"))                                                  \
            luaL_error(state, "invalid Axmol value table for argument #%d", index);                                  \
        return value;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    template <class Handler>                                                                                           \
    bool sol_lua_check(sol::types<Type>, lua_State* state, int index, Handler&& handler, sol::stack::record& tracking) \
    {                                                                                                                  \
        tracking.use(1);                                                                                               \
        if (lua_istable(state, index))                                                                                 \
            return true;                                                                                               \
        handler(state, index, sol::type::table, sol::type_of(state, index), "expected an Axmol value table");          \
        return false;                                                                                                  \
    }

AX_LUA_TABLE_VALUE_ADAPTER(Vec2, vec2_to_luaval, luaval_to_vec2)
AX_LUA_TABLE_VALUE_ADAPTER(Vec3, vec3_to_luaval, luaval_to_vec3)
AX_LUA_TABLE_VALUE_ADAPTER(Vec4, vec4_to_luaval, luaval_to_vec4)
AX_LUA_TABLE_VALUE_ADAPTER(Quat, quat_to_luaval, luaval_to_quat)
AX_LUA_TABLE_VALUE_ADAPTER(Rect, rect_to_luaval, luaval_to_rect)
AX_LUA_TABLE_VALUE_ADAPTER(Color32, color32_to_luaval, luaval_to_color32)
AX_LUA_TABLE_VALUE_ADAPTER(Color, color_to_luaval, luaval_to_color)
AX_LUA_TABLE_VALUE_ADAPTER(Mat4, mat4_to_luaval, luaval_to_mat4)
AX_LUA_TABLE_VALUE_ADAPTER(BlendFunc, blendfunc_to_luaval, luaval_to_blendfunc)
AX_LUA_TABLE_VALUE_ADAPTER(TTFConfig, ttfconfig_to_luaval, luaval_to_ttfconfig)
AX_LUA_TABLE_VALUE_ADAPTER(FontDefinition, fontdefinition_to_luaval, luaval_to_fontdefinition)
AX_LUA_TABLE_VALUE_ADAPTER(rhi::SamplerDesc, texParams_to_luaval, luaval_to_texparams)
AX_LUA_TABLE_VALUE_ADAPTER(MeshVertexAttrib, mesh_vertex_attrib_to_luaval, luaval_to_mesh_vertex_attrib)
AX_LUA_TABLE_VALUE_ADAPTER(ValueMap, valuemap_to_luaval, luaval_to_valuemap)
AX_LUA_TABLE_VALUE_ADAPTER(ValueMapIntKey, valuemapintkey_to_luaval, luaval_to_valuemapintkey)

// Value is a tagged scalar/table variant rather than a table-only struct.  It
// still uses the historical Lua scalar/table representation.
inline int sol_lua_push(lua_State* state, const Value& value)
{
    ::value_to_luaval(state, value);
    return 1;
}

inline Value sol_lua_get(sol::types<Value>, lua_State* state, int index, sol::stack::record& tracking)
{
    tracking.use(1);
    Value value;
    ::luaval_to_value(state, index, &value, "Axmol Lua binding");
    return value;
}

template <class Handler>
bool sol_lua_check(sol::types<Value>, lua_State* state, int index, Handler&& handler, sol::stack::record& tracking)
{
    tracking.use(1);
    const int type = lua_type(state, index);
    if (type == LUA_TNONE || type == LUA_TNIL || type == LUA_TBOOLEAN || type == LUA_TNUMBER || type == LUA_TSTRING ||
        type == LUA_TTABLE)
        return true;
    handler(state, index, sol::type::lua_nil, sol::type_of(state, index), "expected an Axmol Value");
    return false;
}

inline int sol_lua_push(lua_State* state, const ValueVector& value)
{
    ::valuespan_to_luaval(state, std::span<const Value>(value.data(), value.size()));
    return 1;
}

inline ValueVector sol_lua_get(sol::types<ValueVector>, lua_State* state, int index, sol::stack::record& tracking)
{
    tracking.use(1);
    ValueVector value;
    ::luaval_to_valuevector(state, index, &value, "Axmol Lua binding");
    return value;
}

template <class Handler>
bool sol_lua_check(sol::types<ValueVector>,
                   lua_State* state,
                   int index,
                   Handler&& handler,
                   sol::stack::record& tracking)
{
    tracking.use(1);
    if (lua_istable(state, index))
        return true;
    handler(state, index, sol::type::table, sol::type_of(state, index), "expected an Axmol ValueVector table");
    return false;
}
#if defined(AX_ENABLE_PHYSICS_2D)
AX_LUA_TABLE_VALUE_ADAPTER(PhysicsMaterial2D, physics_material2d_to_luaval, luaval_to_physics_material2d)
#endif
#if defined(AX_ENABLE_PHYSICS_3D)
AX_LUA_TABLE_VALUE_ADAPTER(PhysicsMaterial, physics_material_to_luaval, luaval_to_physics_material)
AX_LUA_TABLE_VALUE_ADAPTER(TwistLimits, twist_limits_to_luaval, luaval_to_twist_limits)
AX_LUA_TABLE_VALUE_ADAPTER(SixDofLimits, six_dof_limits_to_luaval, luaval_to_six_dof_limits)
AX_LUA_TABLE_VALUE_ADAPTER(JointSpring, joint_spring_to_luaval, luaval_to_joint_spring)
AX_LUA_TABLE_VALUE_ADAPTER(JointMotor, joint_motor_to_luaval, luaval_to_joint_motor)
AX_LUA_TABLE_VALUE_ADAPTER(JointLimits, joint_limits_to_luaval, luaval_to_joint_limits)
inline int sol_lua_push(lua_State* state, const ContactInfo3D& value)
{
    ::contact_info_3d_to_luaval(state, value);
    return 1;
}
#endif

#undef AX_LUA_TABLE_VALUE_ADAPTER

template <class T, std::enable_if_t<std::is_class_v<T>, int> = 0>
int sol_lua_push(lua_State* state, T* object)
{
    return axlua::push_object(state, object);
}

// Manual compatibility classes and generated sol2 classes share Axmol's Lua
// inheritance registry, but sol2's overload matcher only knows its own
// metatable graph. Consult the shared registry first so a manual subclass such
// as ax.DrawNode3D can be passed wherever a generated ax.Node* is expected.
template <class T, class Handler>
bool sol_lua_check(sol::types<T*>, lua_State* state, int index, Handler&& handler, sol::stack::record& tracking)
{
    if (lua_isnoneornil(state, index))
    {
        tracking.use(1);
        return true;
    }

    using NativeT = std::remove_cv_t<T>;
    if (const char* typeName = axlua::find_native_type_name(typeid(NativeT));
        typeName != nullptr && axlua::adapter::is_usertype(state, index, typeName, 0, nullptr))
    {
        tracking.use(1);
        return true;
    }

    return sol::stack::unqualified_checker<sol::detail::as_pointer_tag<T*>, sol::type::userdata>::check(
        state, index, std::forward<Handler>(handler), tracking);
}

template <class T, class Handler>
sol::optional<T*> sol_lua_check_get(sol::types<T*>,
                                    lua_State* state,
                                    int index,
                                    Handler&& handler,
                                    sol::stack::record& tracking)
{
    if (axlua::is_invalid_userdata(state, index))
    {
        handler(state, index, sol::type::userdata, sol::type::userdata, "native object has expired");
        return sol::nullopt;
    }

    // Persistent objects use per-userdata WeakPtr metadata; borrowed Events
    // use the invalid marker set when their callback scope ends. Both are
    // checked above without consulting a potentially reused native address.
    return sol::stack::unqualified_getter<sol::detail::as_pointer_tag<T>>::get(state, index, tracking);
}
}  // namespace ax

// The historical Axmol Lua API represents sequential C++ containers as Lua
// tables.  sol2 deliberately exposes containers as userdata by default, which
// is useful for reference semantics but breaks existing scripts (for example,
// FileUtils:getSearchPaths()).  Use sol2's own nested table conversion through
// its typed ADL hooks so both return values and const-reference arguments keep
// the table contract without generator-side wrappers.
namespace sol
{
template <class T, class Allocator>
int sol_lua_push(types<std::vector<T, Allocator>>, lua_State* state, const std::vector<T, Allocator>& values)
{
    return stack::push(state, as_nested_ref(values));
}

template <class T, class Allocator>
std::vector<T, Allocator> sol_lua_get(types<std::vector<T, Allocator>>,
                                      lua_State* state,
                                      int index,
                                      stack::record& tracking)
{
    using Vector = std::vector<T, Allocator>;
    if (lua_istable(state, index))
        return stack::unqualified_getter<nested<Vector>>::get(state, index, tracking);
    return stack::unqualified_getter<Vector>::get(state, index, tracking);
}

template <class T, class Allocator, class Handler>
bool sol_lua_check(types<std::vector<T, Allocator>>,
                   lua_State* state,
                   int index,
                   Handler&& handler,
                   stack::record& tracking)
{
    using Vector = std::vector<T, Allocator>;
    if (lua_istable(state, index))
    {
        stack::unqualified_checker<nested<Vector>, type::table> checker;
        return checker.check(state, index, std::forward<Handler>(handler), tracking);
    }
    stack::unqualified_checker<Vector, lua_type_of_v<Vector>> checker;
    return checker.check(state, index, std::forward<Handler>(handler), tracking);
}
}  // namespace sol
