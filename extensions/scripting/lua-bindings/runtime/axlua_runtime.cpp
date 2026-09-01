#include "lua-bindings/runtime/axlua_runtime.h"

template <>
std::function<void()> axlua::make_lua_callback<void()>(lua_State* state, int index)
{
    return axlua::detail::LuaCallbackFactory<void()>::create(state, index);
}
#include "axmol/base/Object.h"
#include "axmol/base/WeakPtr.h"
#include "axmol/base/Logging.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"

#include <utility>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <mutex>
#include <thread>

namespace axlua::detail
{
// Lua references belong to a VM lifetime, not to a reusable lua_State address.
// Workers only mark existing entries; every Lua API call stays on the owner.
struct LuaCallbackRegistry
{
    explicit LuaCallbackRegistry(lua_State* value) : state(value), owner(std::this_thread::get_id()) {}

    void release(int reference) noexcept
    {
        std::lock_guard lock(mutex);
        if (state == nullptr)
            return;
        auto found = references.find(reference);
        if (found == references.end())
            return;
        if (std::this_thread::get_id() == owner)
        {
            luaL_unref(state, LUA_REGISTRYINDEX, reference);
            references.erase(found);
        }
        else
        {
            found->second = true;
            hasPending    = true;
        }
    }

    bool drain()
    {
        if (std::this_thread::get_id() != owner)
            return false;
        std::lock_guard lock(mutex);
        if (state == nullptr)
            return false;
        if (!hasPending)
            return true;
        for (auto iterator = references.begin(); iterator != references.end();)
        {
            if (iterator->second)
            {
                luaL_unref(state, LUA_REGISTRYINDEX, iterator->first);
                iterator = references.erase(iterator);
            }
            else
                ++iterator;
        }
        hasPending = false;
        return true;
    }

    void close()
    {
        std::lock_guard lock(mutex);
        auto* closing = std::exchange(state, nullptr);
        if (closing != nullptr && std::this_thread::get_id() == owner)
            for (const auto& [reference, pending] : references)
                luaL_unref(closing, LUA_REGISTRYINDEX, reference);
        // Closing on the owner is the normal path. Director disposal must
        // never touch a foreign VM; lua_close reclaims its references later.
        references.clear();
    }

    lua_State* state;
    const std::thread::id owner;
    std::mutex mutex;
    std::unordered_map<int, bool> references;
    bool hasPending = false;
};
}  // namespace axlua::detail

namespace
{
struct DynamicObjectPushers
{
    // Fast path: RTTI descriptors have process lifetime and normally have one
    // canonical address.  The name map is the cross-DSO fallback; a successful
    // fallback is cached into this pointer map.
    std::unordered_map<const std::type_info*, axlua::AdapterObjectPusher> byAddress;
    std::unordered_map<std::string, axlua::AdapterObjectPusher> byName;
};

struct NativeObjectRecord
{
    ax::WeakPtr<ax::Object> object;
    // One native object may be seen by several independent Lua VMs. Lua APIs
    // remain thread-confined, so keep the VM owner thread alongside each
    // exposure and only invalidate handles that are safe to access here.
    std::unordered_map<lua_State*, std::thread::id> vmThreads;
};

// Lua's registry is shared by the main thread and every coroutine.  Binding
// registrations and shutdown bookkeeping must use the VM identity rather
// than a transient coroutine pointer.  `install()` records the main thread
// before any generated binding can create callbacks or userdata.
constexpr char kAxluaMainThreadRegistryKey[]     = "axlua.runtime.main_thread";
constexpr char kAxluaObjectLifetimeRegistryKey[] = "axlua.object.lifetimes";

lua_State* vm_state(lua_State* state)
{
    if (state == nullptr)
        return nullptr;

    lua_getfield(state, LUA_REGISTRYINDEX, kAxluaMainThreadRegistryKey);
    auto* mainState = lua_tothread(state, -1);
    lua_pop(state, 1);
    return mainState != nullptr ? mainState : state;
}

void remember_vm_state(lua_State* state)
{
    if (state == nullptr)
        return;

    lua_getfield(state, LUA_REGISTRYINDEX, kAxluaMainThreadRegistryKey);
    const bool alreadyRegistered = lua_isthread(state, -1);
    lua_pop(state, 1);
    if (alreadyRegistered)
        return;

    lua_pushthread(state);
    lua_setfield(state, LUA_REGISTRYINDEX, kAxluaMainThreadRegistryKey);
}

class AxluaRuntimeState
{
public:
    static AxluaRuntimeState& instance()
    {
        if (_instance == nullptr)
            _instance = new AxluaRuntimeState();
        return *_instance;
    }

    static AxluaRuntimeState* existing() { return _instance; }

    static void dispose()
    {
        auto* state = _instance;
        if (state == nullptr)
            return;
        _instance = nullptr;
        delete state;
    }

    std::unordered_map<ax::Object*, NativeObjectRecord> nativeObjects;
    std::mutex nativeObjectsMutex;
    std::unordered_map<lua_State*, std::shared_ptr<axlua::detail::LuaCallbackRegistry>> luaCallbacks;
    std::mutex luaCallbacksMutex;
    std::unordered_map<lua_State*, std::unordered_map<std::string, axlua::AdapterObjectPusher>> objectPushers;
    std::unordered_map<lua_State*, DynamicObjectPushers> dynamicObjectPushers;
    std::unordered_map<const std::type_info*, std::string> nativeTypeNamesByAddress;
    std::unordered_map<std::string, std::string> nativeTypeNamesByName;

    void drainCallbacks()
    {
        std::lock_guard lock(luaCallbacksMutex);
        for (const auto& [state, registry] : luaCallbacks)
            registry->drain();
    }

private:
    AxluaRuntimeState()
    {
        _dispatcher        = ax::Director::getInstance()->getEventDispatcher();
        _disposingListener = _dispatcher->addCustomEventListener(
            ax::Director::EVENT_DISPOSING, [](ax::CustomEvent*) { AxluaRuntimeState::dispose(); }, 2);
        _updateListener = _dispatcher->addCustomEventListener(ax::Director::EVENT_BEFORE_UPDATE,
                                                              [this](ax::CustomEvent*) { drainCallbacks(); });
    }

    ~AxluaRuntimeState()
    {
        if (_dispatcher != nullptr && _disposingListener != nullptr)
            _dispatcher->removeEventListener(_disposingListener);
        if (_dispatcher != nullptr && _updateListener != nullptr)
            _dispatcher->removeEventListener(_updateListener);
        _disposingListener = nullptr;
        decltype(luaCallbacks) callbacks;
        {
            std::lock_guard lock(luaCallbacksMutex);
            callbacks.swap(luaCallbacks);
        }
        for (const auto& [state, registry] : callbacks)
            registry->close();
        objectPushers.clear();
        dynamicObjectPushers.clear();
        nativeTypeNamesByAddress.clear();
        nativeTypeNamesByName.clear();
        nativeObjects.clear();
    }

    static AxluaRuntimeState* _instance;
    ax::EventDispatcher* _dispatcher            = nullptr;
    ax::CustomEventListener* _disposingListener = nullptr;
    ax::CustomEventListener* _updateListener    = nullptr;
};

AxluaRuntimeState* AxluaRuntimeState::_instance = nullptr;

AxluaRuntimeState& runtime_state()
{
    return AxluaRuntimeState::instance();
}

void drain_pending_unrefs(lua_State* state)
{
    auto* runtime = AxluaRuntimeState::existing();
    if (state == nullptr || runtime == nullptr)
        return;
    // Accept the owning main state without querying its registry first.
    // The token checks the thread and closed state before any Lua operation.
    std::lock_guard lock(runtime->luaCallbacksMutex);
    auto found = runtime->luaCallbacks.find(state);
    if (found != runtime->luaCallbacks.end())
        found->second->drain();
}

int absolute_index(lua_State* state, int index)
{
    return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(state) + index + 1;
}

void push_user_environment(lua_State* state, int index)
{
    index = absolute_index(state, index);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_getfenv(state, index);
    // Lua 5.1/LuaJIT may return the shared global environment for userdata.
    // Never use that table as Axmol's peer storage.
    bool sharedEnvironment = false;
    if (lua_istable(state, -1))
    {
        lua_pushvalue(state, LUA_GLOBALSINDEX);
        sharedEnvironment = lua_rawequal(state, -1, -2) != 0;
        lua_pop(state, 1);
    }
#elif LUA_VERSION_NUM >= 504
    lua_getiuservalue(state, index, 1);
#else
    lua_getuservalue(state, index);
#endif
    if (!lua_istable(state, -1)
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        || sharedEnvironment
#endif
    )
    {
        lua_pop(state, 1);
        lua_newtable(state);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        lua_pushvalue(state, -1);
        lua_setfenv(state, index);
#elif LUA_VERSION_NUM >= 504
        lua_pushvalue(state, -1);
        lua_setiuservalue(state, index, 1);
#else
        lua_pushvalue(state, -1);
        lua_setuservalue(state, index);
#endif
    }
}

// Read an existing peer/uservalue without materializing one. Ordinary native
// objects have no peer table until Lua code explicitly attaches peer state;
// keeping this lookup non-allocating is the no-peer instance-index fast path.
bool push_existing_user_environment(lua_State* state, int index)
{
    index = absolute_index(state, index);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_getfenv(state, index);
    bool sharedEnvironment = false;
    if (lua_istable(state, -1))
    {
        lua_pushvalue(state, LUA_GLOBALSINDEX);
        sharedEnvironment = lua_rawequal(state, -1, -2) != 0;
        lua_pop(state, 1);
    }
    if (!lua_istable(state, -1) || sharedEnvironment)
    {
        lua_pop(state, 1);
        return false;
    }
#elif LUA_VERSION_NUM >= 504
    lua_getiuservalue(state, index, 1);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
#else
    lua_getuservalue(state, index);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
#endif
    return true;
}

void mark_userdata_invalid(lua_State* state, int index)
{
    index = absolute_index(state, index);
    if (!lua_isuserdata(state, index))
        return;

    // Install a private marker table. In Lua 5.1/LuaJIT the existing fenv may
    // be the shared global environment, which would otherwise invalidate every
    // userdata sharing that table.
    lua_newtable(state);
    lua_pushboolean(state, 1);
    lua_setfield(state, -2, "__axlua_invalid");
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_setfenv(state, index);
#elif LUA_VERSION_NUM >= 504
    lua_setiuservalue(state, index, 1);
#else
    lua_setuservalue(state, index);
#endif

    // Both sol2 pointer userdata and the remaining compatibility userdata keep
    // the non-owning native pointer in their first machine word.
    if (auto** storage = static_cast<void**>(lua_touserdata(state, index)))
        *storage = nullptr;
}

void invalidate_pointer_registry_entry(lua_State* state, const char* registryName, void* object)
{
    lua_getfield(state, LUA_REGISTRYINDEX, registryName);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return;
    }

    const int table = absolute_index(state, -1);
    lua_pushlightuserdata(state, object);
    lua_rawget(state, table);
    mark_userdata_invalid(state, -1);
    lua_pop(state, 1);
    lua_pushlightuserdata(state, object);
    lua_pushnil(state);
    lua_rawset(state, table);
    lua_pop(state, 1);
}

void remove_pointer_registry_entry(lua_State* state, const char* registryName, void* object)
{
    lua_getfield(state, LUA_REGISTRYINDEX, registryName);
    if (lua_istable(state, -1))
    {
        lua_pushlightuserdata(state, object);
        lua_pushnil(state);
        lua_rawset(state, -3);
    }
    lua_pop(state, 1);
}

int lua_getpeer(lua_State* state)
{
    if (!lua_isuserdata(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }
    push_user_environment(state, 1);
    return 1;
}

int lua_setpeer(lua_State* state)
{
    if (!lua_isuserdata(state, 1))
        return luaL_error(state, "axlua.setpeer expects userdata");
    if (lua_isnoneornil(state, 2))
    {
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        lua_pushnil(state);
        lua_setfenv(state, 1);
#elif LUA_VERSION_NUM >= 504
        lua_pushnil(state);
        lua_setiuservalue(state, 1, 1);
#else
        lua_pushnil(state);
        lua_setuservalue(state, 1);
#endif
        return 0;
    }
    if (!lua_istable(state, 2))
        return luaL_error(state, "axlua.setpeer expects a table or nil");
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_pushvalue(state, 2);
    lua_setfenv(state, 1);
#elif LUA_VERSION_NUM >= 504
    lua_pushvalue(state, 2);
    lua_setiuservalue(state, 1, 1);
#else
    lua_pushvalue(state, 2);
    lua_setuservalue(state, 1);
#endif
    return 0;
}

bool push_class_table(lua_State* state, int userdataIndex)
{
    if (!lua_isuserdata(state, userdataIndex) || !lua_getmetatable(state, userdataIndex))
        return false;

    // Generated userdata metatables are stable for the lifetime of the VM.
    // Keep their class table directly on the metatable so the hot instance
    // lookup avoids a registry table and a lightuserdata key round trip.
    lua_getfield(state, -1, "__axlua_class_table");
    if (lua_istable(state, -1))
    {
        lua_remove(state, -2);  // userdata metatable
        return true;
    }
    lua_pop(state, 1);

    const void* metatableKey = lua_topointer(state, -1);
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 2);
        return false;
    }
    lua_pushlightuserdata(state, const_cast<void*>(metatableKey));
    lua_rawget(state, -2);
    lua_remove(state, -2);  // class table registry
    lua_remove(state, -2);  // userdata metatable
    return lua_istable(state, -1);
}

bool push_registered_base(lua_State* state, int classTableIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);

    // Generated class tables retain their direct base table for the lifetime
    // of the VM. Prefer this local link over the registry indirection used by
    // compatibility tables; the returned table remains live and still sees
    // all runtime Lua modifications.
    lua_pushliteral(state, "__axlua_base_class");
    lua_rawget(state, classTableIndex);
    if (lua_istable(state, -1))
        return true;
    lua_pop(state, 1);

    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, classTableIndex)));
    lua_rawget(state, -2);
    lua_remove(state, -2);
    return lua_istable(state, -1);
}

enum class ClassMemberKind
{
    none,
    value,
    getter,
};

bool lookup_class_accessor(lua_State* state, int classTableIndex, int keyIndex, const char* accessorTable)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    lua_getfield(state, classTableIndex, accessorTable);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushvalue(state, keyIndex);
    lua_rawget(state, -2);
    lua_remove(state, -2);
    if (lua_isfunction(state, -1))
        return true;
    lua_pop(state, 1);
    return false;
}

ClassMemberKind lookup_class_member(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        lua_pushvalue(state, keyIndex);
        lua_gettable(state, classTableIndex);
        if (!lua_isnil(state, -1))
            return ClassMemberKind::value;
        lua_pop(state, 1);

        if (lookup_class_accessor(state, classTableIndex, keyIndex, ".get"))
            return ClassMemberKind::getter;

        if (!push_registered_base(state, classTableIndex))
            return ClassMemberKind::none;
        lua_remove(state, classTableIndex);
        classTableIndex = absolute_index(state, -1);
    }
    return ClassMemberKind::none;
}

bool lookup_class_setter(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        if (lookup_class_accessor(state, classTableIndex, keyIndex, ".set"))
            return true;
        if (!push_registered_base(state, classTableIndex))
            return false;
        lua_remove(state, classTableIndex);
        classTableIndex = absolute_index(state, -1);
    }
    return false;
}

bool lookup_registered_table_member(lua_State* state, int tableIndex, int keyIndex)
{
    tableIndex = absolute_index(state, tableIndex);
    keyIndex   = absolute_index(state, keyIndex);
    lua_pushvalue(state, keyIndex);
    lua_gettable(state, tableIndex);
    return !lua_isnil(state, -1);
}

bool lookup_base_member(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        if (!push_registered_base(state, classTableIndex))
            return false;
        classTableIndex = absolute_index(state, -1);
        if (lookup_registered_table_member(state, classTableIndex, keyIndex))
            return true;
    }
    return false;
}

void canonicalize_registered_userdata(lua_State* state, int firstResult)
{
    firstResult = absolute_index(state, firstResult);
    for (int index = firstResult; index <= lua_gettop(state); ++index)
    {
        if (!lua_isuserdata(state, index) || !lua_getmetatable(state, index))
            continue;

        const void* metatableKey = lua_topointer(state, -1);
        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 2);  // pointer registry and userdata metatable
            continue;
        }
        lua_pushlightuserdata(state, const_cast<void*>(metatableKey));
        lua_rawget(state, -2);
        const bool registered = lua_istable(state, -1);
        lua_pop(state, 2);  // class table and registry metatable
        if (!registered)
        {
            lua_pop(state, 1);  // userdata metatable
            continue;
        }

        // Pointer metatables may be materialized lazily by sol2 after class
        // registration. Install the Axmol dispatcher on the concrete
        // metatable as soon as the first factory result is observed.
        lua_pushcfunction(state, &axlua::class_index);
        lua_setfield(state, -2, "__index");
        lua_pushcfunction(state, &axlua::class_new_index);
        lua_setfield(state, -2, "__newindex");

        void* nativeObject = *static_cast<void**>(lua_touserdata(state, index));
        lua_pop(state, 1);  // userdata metatable
        if (nativeObject == nullptr)
            continue;

        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            axlua::make_weak_value_table(state, -1);
            lua_pushvalue(state, -1);
            lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        }
        const int identityTable = absolute_index(state, -1);
        lua_pushlightuserdata(state, nativeObject);
        lua_rawget(state, identityTable);
        if (lua_isuserdata(state, -1) && !axlua::is_invalid_userdata(state, -1) && !lua_rawequal(state, -1, index))
        {
            lua_replace(state, index);
        }
        else
        {
            lua_pop(state, 1);
            lua_pushlightuserdata(state, nativeObject);
            lua_pushvalue(state, index);
            lua_rawset(state, identityTable);
        }
        lua_pop(state, 1);  // identity table
    }
}

int lua_isnull(lua_State* state)
{
    const bool invalid = axlua::is_invalid_userdata(state, 1);
    lua_pushboolean(state, lua_isnoneornil(state, 1) || invalid);
    return 1;
}

int lua_type_name(lua_State* state)
{
    if (!push_class_table(state, 1))
    {
        lua_pushstring(state, luaL_typename(state, 1));
        return 1;
    }
    lua_getfield(state, -1, "__axlua_name");
    lua_remove(state, -2);
    return 1;
}

int lua_iskindof(lua_State* state)
{
    const char* expected = luaL_checkstring(state, 2);
    if (!push_class_table(state, 1))
    {
        lua_pushboolean(state, 0);
        return 1;
    }

    for (int depth = 0; depth < 64 && lua_istable(state, -1); ++depth)
    {
        lua_getfield(state, -1, "__axlua_name");
        const bool matches = lua_isstring(state, -1) && std::strcmp(lua_tostring(state, -1), expected) == 0;
        lua_pop(state, 1);
        if (matches)
        {
            lua_settop(state, lua_gettop(state) - 1);
            lua_pushboolean(state, 1);
            return 1;
        }

        if (!push_registered_base(state, -1))
            break;
        lua_remove(state, -2);
    }
    lua_settop(state, lua_gettop(state) - 1);
    lua_pushboolean(state, 0);
    return 1;
}

int lua_cast(lua_State* state)
{
    if (lua_isnoneornil(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }
    luaL_checktype(state, 1, LUA_TUSERDATA);
    const char* expected = luaL_checkstring(state, 2);
    if (!push_class_table(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }

    bool matches = false;
    for (int depth = 0; depth < 64 && lua_istable(state, -1); ++depth)
    {
        lua_getfield(state, -1, "__axlua_name");
        matches = lua_isstring(state, -1) && std::strcmp(lua_tostring(state, -1), expected) == 0;
        lua_pop(state, 1);
        if (matches)
            break;
        if (!push_registered_base(state, -1))
            break;
        lua_remove(state, -2);
    }
    lua_settop(state, lua_gettop(state) - 1);
    if (!matches)
    {
        lua_pushnil(state);
        return 1;
    }
    lua_pushvalue(state, 1);
    return 1;
}

}  // namespace

namespace axlua
{

void drain_pending_callback_unrefs(lua_State* state)
{
    drain_pending_unrefs(state);
}

void register_object_pusher(lua_State* state,
                            std::string_view typeName,
                            const std::type_info& nativeType,
                            AdapterObjectPusher pusher)
{
    if (state == nullptr || typeName.empty() || pusher == nullptr)
        return;
    auto* vmState                                                 = vm_state(state);
    runtime_state().objectPushers[vmState][std::string(typeName)] = pusher;
    auto& dynamicPushers                                          = runtime_state().dynamicObjectPushers[vmState];
    dynamicPushers.byAddress[&nativeType]                         = pusher;
    dynamicPushers.byName[nativeType.name()]                      = pusher;
    register_native_type_name(nativeType, typeName);
}

bool push_registered_object(lua_State* state, void* object, std::string_view typeName)
{
    if (state == nullptr || object == nullptr || typeName.empty())
        return false;
    const auto stateEntry = runtime_state().objectPushers.find(vm_state(state));
    if (stateEntry == runtime_state().objectPushers.end())
        return false;
    const auto pusherEntry = stateEntry->second.find(std::string(typeName));
    if (pusherEntry == stateEntry->second.end())
        return false;
    pusherEntry->second(state, object);
    return true;
}

void register_native_type_name(const std::type_info& nativeType, std::string_view luaTypeName)
{
    if (luaTypeName.empty())
        return;
    const std::string name(luaTypeName);
    runtime_state().nativeTypeNamesByAddress[&nativeType]    = name;
    runtime_state().nativeTypeNamesByName[nativeType.name()] = name;
}

const char* find_native_type_name(const std::type_info& nativeType)
{
    if (const auto found = runtime_state().nativeTypeNamesByAddress.find(&nativeType);
        found != runtime_state().nativeTypeNamesByAddress.end())
        return found->second.c_str();

    const auto fallback = runtime_state().nativeTypeNamesByName.find(nativeType.name());
    if (fallback == runtime_state().nativeTypeNamesByName.end())
        return nullptr;
    return runtime_state().nativeTypeNamesByAddress.emplace(&nativeType, fallback->second).first->second.c_str();
}

bool push_registered_dynamic_object(lua_State* state, ax::Object* object, const std::type_info& staticType)
{
    if (state == nullptr || object == nullptr)
        return false;

    const std::type_info& dynamicType = typeid(*object);
    if (dynamicType == staticType)
        return false;

    const auto stateEntry = runtime_state().dynamicObjectPushers.find(vm_state(state));
    if (stateEntry == runtime_state().dynamicObjectPushers.end())
        return false;

    auto pusherEntry = stateEntry->second.byAddress.find(&dynamicType);
    if (pusherEntry == stateEntry->second.byAddress.end())
    {
        const auto fallback = stateEntry->second.byName.find(dynamicType.name());
        if (fallback == stateEntry->second.byName.end())
            return false;
        pusherEntry = stateEntry->second.byAddress.emplace(&dynamicType, fallback->second).first;
    }

    // dynamic_cast<void*> yields the complete object address even when the
    // statically returned pointer denotes a non-primary base subobject.
    pusherEntry->second(state, dynamic_cast<void*>(object));
    return true;
}

namespace detail
{
void BorrowedObjectScope::track(int index)
{
    if (_state == nullptr || !lua_isuserdata(_state, index))
        return;

    lua_pushvalue(_state, index);
    _references.push_back(luaL_ref(_state, LUA_REGISTRYINDEX));
}

void BorrowedObjectScope::invalidate() noexcept
{
    if (_state == nullptr)
        return;

    for (const int reference : _references)
    {
        lua_rawgeti(_state, LUA_REGISTRYINDEX, reference);
        invalidate_borrowed_userdata(_state, -1);
        lua_pop(_state, 1);
        luaL_unref(_state, LUA_REGISTRYINDEX, reference);
    }
    _references.clear();
}

void invalidate_borrowed_userdata(lua_State* state, int index)
{
    mark_userdata_invalid(state, index);
}

std::shared_ptr<LuaCallbackState> LuaCallbackState::create(lua_State* state, int index)
{
    if (state == nullptr || !lua_isfunction(state, index))
        return {};

    auto* vmState = vm_state(state);
    // The function reference is VM-wide. Invoke it through the main Lua
    // thread even when the function was supplied by a coroutine: native
    // callbacks are delivered on the engine thread, not on that coroutine's
    // transient stack.
    std::shared_ptr<LuaCallbackRegistry> registry;
    {
        auto& runtime = runtime_state();
        std::lock_guard lock(runtime.luaCallbacksMutex);
        auto& entry = runtime.luaCallbacks[vmState];
        if (!entry)
            entry = std::make_shared<LuaCallbackRegistry>(vmState);
        registry = entry;
    }
    auto callback = std::shared_ptr<LuaCallbackState>(new LuaCallbackState(vmState, registry));
    lua_pushvalue(state, index);
    callback->_ref = luaL_ref(state, LUA_REGISTRYINDEX);
    callback->_active.store(callback->_ref != LUA_NOREF && callback->_ref != LUA_REFNIL, std::memory_order_release);
    if (callback->_active.load(std::memory_order_acquire))
    {
        std::lock_guard lock(registry->mutex);
        registry->references.emplace(callback->_ref, false);
    }
    return callback;
}

LuaCallbackState::~LuaCallbackState()
{
    invalidate();
}

void LuaCallbackState::invalidate() noexcept
{
    if (!_active.exchange(false, std::memory_order_acq_rel))
        return;

    _registry->release(_ref);
}

bool LuaCallbackState::prepare_invoke()
{
    if (!_active.load(std::memory_order_acquire))
        return false;
    if (std::this_thread::get_id() != _ownerThread)
    {
        report_callback_error("attempted to invoke a Lua callback from a non-owner thread");
        return false;
    }
    return _registry->drain();
}

void LuaCallbackState::report_callback_error(const char* message)
{
    AXLOGE("[LUA CALLBACK ERROR] {}", message != nullptr ? message : "unknown Lua error");
}
}  // namespace detail

void shutdown_callbacks(lua_State* state)
{
    if (state == nullptr || AxluaRuntimeState::existing() == nullptr)
        return;

    std::shared_ptr<detail::LuaCallbackRegistry> callbacks;
    {
        std::lock_guard lock(runtime_state().luaCallbacksMutex);
        auto found = runtime_state().luaCallbacks.find(vm_state(state));
        if (found == runtime_state().luaCallbacks.end())
            return;
        callbacks = std::move(found->second);
        runtime_state().luaCallbacks.erase(found);
    }
    callbacks->close();
}

int class_static_index(lua_State* state)
{
    if (lua_isfunction(state, lua_upvalueindex(1)))
    {
        lua_pushvalue(state, lua_upvalueindex(1));
        lua_pushvalue(state, 1);
        lua_pushvalue(state, 2);
        if (lua_pcall(state, 2, 1, 0) != LUA_OK)
            return lua_error(state);
        if (!lua_isnil(state, -1))
            return 1;
        lua_pop(state, 1);
    }
    else if (lua_istable(state, lua_upvalueindex(1)))
    {
        lua_pushvalue(state, 2);
        lua_rawget(state, lua_upvalueindex(1));
        if (!lua_isnil(state, -1))
            return 1;
        lua_pop(state, 1);
    }

    if (lookup_base_member(state, 1, 2))
        return 1;
    lua_pushnil(state);
    return 1;
}

void install_class_static_index(lua_State* state, int classTableIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    if (!lua_getmetatable(state, classTableIndex))
        return;
    const int metatableIndex = absolute_index(state, -1);
    lua_getfield(state, metatableIndex, "__axlua_original_index");
    if (!lua_isnil(state, -1))
    {
        lua_pop(state, 2);
        return;
    }
    lua_pop(state, 1);
    lua_getfield(state, metatableIndex, "__index");
    lua_pushvalue(state, -1);
    lua_setfield(state, metatableIndex, "__axlua_original_index");
    lua_pushcclosure(state, &class_static_index, 1);
    lua_setfield(state, metatableIndex, "__index");
    lua_pop(state, 1);
}

bool is_invalid_userdata(lua_State* state, int index)
{
    if (lua_type(state, index) != LUA_TUSERDATA)
        return false;
    index = absolute_index(state, index);
    if (push_existing_user_environment(state, index))
    {
        lua_getfield(state, -1, "__axlua_invalid");
        const bool invalid = lua_toboolean(state, -1) != 0;
        lua_pop(state, 2);
        if (invalid)
            return true;
    }

    lua_getfield(state, LUA_REGISTRYINDEX, kAxluaObjectLifetimeRegistryKey);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushvalue(state, index);
    lua_rawget(state, -2);
    auto* lifetime     = static_cast<ax::WeakPtr<ax::Object>*>(lua_touserdata(state, -1));
    const bool expired = lifetime != nullptr && lifetime->expired();
    lua_pop(state, 2);
    return expired;
}

void remember_object(lua_State* state, ax::Object* object)
{
    if (state == nullptr || object == nullptr)
        return;

    object->markScriptBindingExposed();

    auto* runtime = AxluaRuntimeState::existing();
    if (runtime == nullptr)
        return;

    std::lock_guard lock(runtime->nativeObjectsMutex);

    const auto key         = vm_state(state);
    auto [found, inserted] = runtime->nativeObjects.try_emplace(object);
    if (inserted || found->second.object.expired())
    {
        found->second.object = ax::WeakPtr<ax::Object>(object);
        found->second.vmThreads.clear();
    }
    found->second.vmThreads[key] = std::this_thread::get_id();
}

void remember_userdata_object(lua_State* state, int index, ax::Object* object)
{
    auto* runtime = AxluaRuntimeState::existing();
    if (state == nullptr || object == nullptr || runtime == nullptr)
        return;
    index = absolute_index(state, index);
    ax::WeakPtr<ax::Object> lifetime;
    {
        std::lock_guard lock(runtime->nativeObjectsMutex);
        auto found = runtime->nativeObjects.find(object);
        if (found == runtime->nativeObjects.end())
            return;
        lifetime = found->second.object;
    }
    lua_getfield(state, LUA_REGISTRYINDEX, kAxluaObjectLifetimeRegistryKey);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_newtable(state);
        axlua::adapter::push_literal(state, "k");
        lua_setfield(state, -2, "__mode");
        lua_setmetatable(state, -2);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, kAxluaObjectLifetimeRegistryKey);
    }
    const int table = absolute_index(state, -1);
    lua_pushvalue(state, index);
    // Each userdata keeps its own WeakPtr generation. Replacing/removing the
    // address-indexed exposure record can never make an old handle live again.
    static_assert(std::is_trivially_destructible_v<ax::WeakPtr<ax::Object>>);
    new (lua_newuserdata(state, sizeof(lifetime))) ax::WeakPtr<ax::Object>(lifetime);
    lua_rawset(state, table);
    lua_pop(state, 1);
}

bool object_userdata_current(lua_State* state, int index, ax::Object* object)
{
    if (state == nullptr || object == nullptr || lua_type(state, index) != LUA_TUSERDATA)
        return false;
    index = absolute_index(state, index);
    lua_getfield(state, LUA_REGISTRYINDEX, kAxluaObjectLifetimeRegistryKey);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushvalue(state, index);
    lua_rawget(state, -2);
    auto* lifetime     = static_cast<ax::WeakPtr<ax::Object>*>(lua_touserdata(state, -1));
    const bool current = lifetime != nullptr && lifetime->get() == object;
    lua_pop(state, 2);
    return current;
}

int class_index(lua_State* state)
{
    if (lua_istable(state, 1))
    {
        if (lookup_base_member(state, 1, 2))
            return 1;
        lua_pushnil(state);
        return 1;
    }
    if (!lua_isuserdata(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }

    if (is_invalid_userdata(state, 1))
        return luaL_error(state, "attempt to access an expired Axmol object");

    // Legacy Lua classes attach their methods to the peer table's metatable
    // (`__index = cls`). Only inspect that compatibility layer when a peer
    // already exists; ordinary native objects skip these table operations.
    if (push_existing_user_environment(state, 1))
    {
        lua_pushvalue(state, 2);
        lua_gettable(state, -2);
        if (!lua_isnil(state, -1))
        {
            lua_remove(state, -2);
            return 1;
        }
        lua_pop(state, 2);
    }

    if (push_class_table(state, 1))
    {
        switch (lookup_class_member(state, -1, 2))
        {
        case ClassMemberKind::value:
            return 1;
        case ClassMemberKind::getter:
            lua_pushvalue(state, 1);
            if (lua_pcall(state, 1, 1, 0) != LUA_OK)
                return lua_error(state);
            return 1;
        case ClassMemberKind::none:
            lua_pop(state, 1);
            break;
        }
    }
    lua_pushnil(state);
    return 1;
}

int class_new_index(lua_State* state)
{
    if (lua_istable(state, 1))
    {
        lua_pushvalue(state, 2);
        lua_pushvalue(state, 3);
        lua_rawset(state, 1);
        return 0;
    }
    if (!lua_isuserdata(state, 1))
        return luaL_error(state, "axlua class expects userdata");
    if (is_invalid_userdata(state, 1))
        return luaL_error(state, "attempt to modify an expired Axmol object");

    if (push_class_table(state, 1))
    {
        if (lookup_class_setter(state, -1, 2))
        {
            lua_pushvalue(state, 1);
            lua_pushvalue(state, 3);
            if (lua_pcall(state, 2, 0, 0) != LUA_OK)
                return lua_error(state);
            return 0;
        }
        lua_pop(state, 1);
    }

    push_user_environment(state, 1);
    lua_pushvalue(state, 2);
    lua_pushvalue(state, 3);
    lua_rawset(state, -3);
    lua_pop(state, 1);
    return 0;
}

int dispatch_callable(lua_State* state, int argumentCount, int contextUpvalue)
{
    lua_pushvalue(state, lua_upvalueindex(1));
    lua_insert(state, 1);
    if (lua_pcall(state, argumentCount, LUA_MULTRET, 0) != LUA_OK)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        const char* message = lua_tostring(state, -1);
        return luaL_error(state, "%s: %s", context != nullptr ? context : "generated Lua binding",
                          message != nullptr ? message : "unknown error");
    }
    canonicalize_registered_userdata(state, 1);
    // lua_pcall leaves exactly the values returned by the native callable.
    // Preserve zero results for void methods instead of manufacturing nil.
    return lua_gettop(state);
}

int method_dispatch(lua_State* state)
{
    return dispatch_callable(state, lua_gettop(state), 2);
}

int static_method_dispatch(lua_State* state)
{
    int argumentCount = lua_gettop(state);
    if (argumentCount > 0 && lua_rawequal(state, 1, lua_upvalueindex(2)))
    {
        lua_remove(state, 1);
        --argumentCount;
    }
    return dispatch_callable(state, argumentCount, 3);
}

int dispatch_overload_callable(lua_State* state, int argumentStart, int contextUpvalue)
{
    const int callArgumentCount      = lua_gettop(state);
    const int signatureArgumentCount = callArgumentCount - argumentStart + 1;
    int selected                     = 0;

    for (int candidate = 1;; ++candidate)
    {
        lua_rawgeti(state, lua_upvalueindex(1), candidate);
        if (lua_isnil(state, -1))
        {
            lua_pop(state, 1);
            break;
        }

        const int candidateTable = absolute_index(state, -1);
        lua_rawgeti(state, candidateTable, 2);
        const auto* signature = static_cast<const OverloadSignature*>(lua_touserdata(state, -1));
        const bool matches = signature->argumentCount == signatureArgumentCount &&
                             signature->matches(state, argumentStart);
        lua_pop(state, 2);
        if (matches)
        {
            selected = candidate;
            break;
        }
    }

    if (selected == 0)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        return luaL_error(state, "%s: no overload accepts the supplied Lua arguments",
                          context != nullptr ? context : "generated Lua binding");
    }

    lua_rawgeti(state, lua_upvalueindex(1), selected);
    lua_rawgeti(state, -1, 1);
    lua_remove(state, -2);
    lua_insert(state, 1);
    if (lua_pcall(state, callArgumentCount, LUA_MULTRET, 0) != LUA_OK)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        const char* message = lua_tostring(state, -1);
        return luaL_error(state, "%s: %s", context != nullptr ? context : "generated Lua binding",
                          message != nullptr ? message : "unknown error");
    }
    canonicalize_registered_userdata(state, 1);
    return lua_gettop(state);
}

int overload_method_dispatch(lua_State* state)
{
    return dispatch_overload_callable(state, 2, 2);
}

int overload_static_method_dispatch(lua_State* state)
{
    if (lua_gettop(state) > 0 && lua_rawequal(state, 1, lua_upvalueindex(2)))
        lua_remove(state, 1);
    return dispatch_overload_callable(state, 1, 3);
}

Module Module::from(lua_State* state, std::string_view namespaceName, std::string_view typeNamespace)
{
    sol::state_view view(state);
    const std::string name(namespaceName);
    sol::object existing = view[name];
    sol::table module =
        existing.valid() && existing.is<sol::table>() ? existing.as<sol::table>() : view.create_named_table(name);
    const std::string effectiveTypeNamespace =
        typeNamespace.empty() ? std::string(namespaceName) : std::string(typeNamespace);
    return Module(view, module, std::string(namespaceName), effectiveTypeNamespace);
}

void install(lua_State* state)
{
    remember_vm_state(state);
    sol::state_view view(state);
    sol::object existing = view["axlua"];
    sol::table module =
        existing.valid() && existing.is<sol::table>() ? existing.as<sol::table>() : view.create_named_table("axlua");

    module.set_function("getpeer", &lua_getpeer);
    module.set_function("setpeer", &lua_setpeer);
    module.set_function("isnull", &lua_isnull);
    module.set_function("type", &lua_type_name);
    module.set_function("iskindof", &lua_iskindof);
    module.set_function("cast", &lua_cast);
}

void shutdown(lua_State* state)
{
    if (AxluaRuntimeState::existing() == nullptr)
        return;
    if (state != nullptr)
    {
        shutdown_callbacks(state);
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
        auto* vmState = vm_state(state);
        runtime_state().objectPushers.erase(vmState);
        runtime_state().dynamicObjectPushers.erase(vmState);
        {
            std::lock_guard lock(runtime_state().nativeObjectsMutex);
            for (auto iterator = runtime_state().nativeObjects.begin();
                 iterator != runtime_state().nativeObjects.end();)
            {
                iterator->second.vmThreads.erase(vmState);
                if (iterator->second.vmThreads.empty())
                    iterator = runtime_state().nativeObjects.erase(iterator);
                else
                    ++iterator;
            }
        }
    }
}

void push_peer(lua_State* state, int userdataIndex)
{
    if (!lua_isuserdata(state, userdataIndex))
    {
        lua_pushnil(state);
        return;
    }
    push_user_environment(state, userdataIndex);
}

void set_peer(lua_State* state, int userdataIndex, int peerIndex)
{
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_pushvalue(state, peerIndex);
    lua_setfenv(state, userdataIndex);
#elif LUA_VERSION_NUM >= 504
    lua_pushvalue(state, peerIndex);
    lua_setiuservalue(state, userdataIndex, 1);
#else
    lua_pushvalue(state, peerIndex);
    lua_setuservalue(state, userdataIndex);
#endif
}

void invalidate_object(lua_State*, void* object)
{
    auto* nativeObject = static_cast<ax::Object*>(object);
    if (nativeObject == nullptr)
        return;

    // Director dispatches EVENT_DISPOSING before destroying the engine.  The
    // runtime state is already disposed at that point; never recreate it from
    // an Object destructor running during Director teardown.
    auto* runtime = AxluaRuntimeState::existing();
    if (runtime == nullptr || !nativeObject->hasScriptBindingExposure())
        return;

    std::vector<lua_State*> statesToInvalidate;
    // Avoid touching Lua for objects that were never exposed to this binding,
    // including objects destroyed from worker threads.
    {
        std::lock_guard lock(runtime->nativeObjectsMutex);
        const auto found = runtime->nativeObjects.find(nativeObject);
        if (found == runtime->nativeObjects.end())
            return;
        for (const auto& [vmState, ownerThread] : found->second.vmThreads)
            if (ownerThread == std::this_thread::get_id())
                statesToInvalidate.push_back(vmState);
    }
    if (statesToInvalidate.empty())
    {
        // Lua is thread-confined. Keep the weak entry so subsequent Lua reads
        // reject the expired userdata without touching a foreign VM.
        return;
    }
    for (auto* vmState : statesToInvalidate)
    {
        invalidate_pointer_registry_entry(vmState, "axlua.object.identity", nativeObject);
        invalidate_pointer_registry_entry(vmState, axlua::adapter::kObjectBoxRegistry, nativeObject);
        invalidate_pointer_registry_entry(vmState, axlua::adapter::kValueRootRegistry, nativeObject);
        remove_pointer_registry_entry(vmState, axlua::adapter::kOwnedObjectRegistry, nativeObject);
    }
    {
        std::lock_guard lock(runtime->nativeObjectsMutex);
        auto found = runtime->nativeObjects.find(nativeObject);
        if (found != runtime->nativeObjects.end())
        {
            for (auto* vmState : statesToInvalidate)
                found->second.vmThreads.erase(vmState);
            if (found->second.vmThreads.empty())
                runtime->nativeObjects.erase(found);
        }
    }
}

}  // namespace axlua
