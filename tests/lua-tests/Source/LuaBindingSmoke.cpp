#include "LuaBindingSmoke.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "axmol/scene/Node.h"
#include "axmol/base/AutoreleasePool.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/Scheduler.h"
#include "axmol/base/WeakPtr.h"
#include "lua-bindings/runtime/LuaEngine.h"

#include <stdio.h>
#include <atomic>
#include <new>
#include <thread>

namespace
{
bool checkLua(lua_State* state, const char* script)
{
    if (luaL_dostring(state, script) == LUA_OK)
        return true;
    fprintf(stderr, "lua-binding-smoke failed: %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return false;
}

bool checkMethodDispatch(lua_State* state)
{
    return checkLua(state, R"lua(
        local node = ax.Node:create()
        local child = ax.Node.create()
        local setPosition = node.setPosition
        assert(select('#', setPosition(node, 12, 34)) == 0)
        local x, y = node:getPosition()
        assert(x == 12 and y == 34)
        node:setPosition({x = 56, y = 78})
        x, y = node:getPosition()
        assert(x == 56 and y == 78)
        assert(not pcall(setPosition, node))
        assert(not pcall(setPosition, node, 1, 2, 3))
        assert(not pcall(setPosition, node, false, {}))

        -- Equal-arity candidates still need type matching, including object
        -- arguments. Object returns must retain canonical userdata identity.
        node:addChild(child, 0, 'dispatch-child')
        assert(rawequal(node:getChildByName('dispatch-child'), child))
        child:removeFromParent()
        node:addChild(child, 0, 17)
        assert(rawequal(node:getChildByTag(17), child))
        assert(node:getReferenceCount() >= 1) -- inherited Object method

        axlua.setpeer(node, {dispatchValue = 9})
        assert(node.dispatchValue == 9)
        axlua.setpeer(node, nil)
        assert(node.dispatchValue == nil)
        assert(node.setPosition == setPosition)

        -- Peer fields, peer metatables, and later class edits must all remain
        -- visible; method lookup must not cache stale inherited functions.
        node.dispatchValue = false
        assert(node.dispatchValue == false)
        node.setPosition = function(self) self.dispatchValue = 1 end
        node:setPosition()
        assert(node.dispatchValue == 1)
        node.setPosition = nil
        setmetatable(axlua.getpeer(node), {
            __index = {setPosition = function(self) self.dispatchValue = 2 end}
        })
        node:setPosition()
        assert(node.dispatchValue == 2)
        setmetatable(axlua.getpeer(node), nil)
        assert(node.setPosition == setPosition)

        local original = ax.Object.getReferenceCount
        ax.Object.getReferenceCount = function() return 12345 end
        local ok, result = pcall(function() return node:getReferenceCount() end)
        ax.Object.getReferenceCount = original
        assert(ok and result == 12345)
        assert(node:getReferenceCount() ~= 12345)
        assert(node.__missing_dispatch_member == nil)
        local co = coroutine.create(function()
            node:setPosition(90, 12)
            local x, y = node:getPosition()
            assert(x == 90 and y == 12)
        end)
        assert(coroutine.resume(co))
        child:removeFromParent()
    )lua");
}

bool checkExpiredLookup(lua_State* state)
{
    auto* object = new ax::Node();
    axlua::push_object(state, object);
    lua_setglobal(state, "__expired_lookup_node");
    // Expire on a worker so the WeakPtr, rather than a Lua invalidation marker,
    // must reject access through the lifetime table. Join before Lua.
    std::thread([object] { object->release(); }).join();
    const bool ok = checkLua(state, R"lua(
        local node = __expired_lookup_node
        assert(axlua.isnull(node))
        assert(not pcall(function() return node.setPosition end))
        assert(not pcall(function() node.value = 1 end))
        axlua.setpeer(node, {})
        assert(axlua.isnull(node))
        assert(not pcall(function() return node.getReferenceCount end))
        axlua.setpeer(node, nil)
        assert(not pcall(function() return node.setPosition end))
        __expired_lookup_node = nil
    )lua");
    return ok;
}

bool checkFastBindings(lua_State* state)
{
    bool ok = checkLua(state, R"lua(
        local node = ax.Node:create()

        -- Fast scalar setters preserve their public Lua API.
        node:setPosition(10, 20)
        assert(node:getPositionX() == 10 and node:getPositionY() == 20)
        node:setPosition({x = 30, y = 40})
        assert(node:getPositionX() == 30 and node:getPositionY() == 40)

        node:setScale(2.5)
        assert(node:getScaleX() == 2.5 and node:getScaleY() == 2.5)
        node:setScale(1, 2)
        assert(node:getScaleX() == 1 and node:getScaleY() == 2)

        node:setRotation(45)
        assert(node:getRotation() == 45)

        node:setOpacity(128)
        assert(node:getOpacity() == 128)

        node:setVisible(false)
        assert(node:isVisible() == false)
        node:setVisible(true)
        assert(node:isVisible() == true)

        -- A cached method resolves to the fast wrapper directly.
        local setPosition = ax.Node.setPosition
        setPosition(node, 50, 60)
        assert(node:getPositionX() == 50 and node:getPositionY() == 60)

        -- Wrong argument count and type still raise Lua errors.
        assert(not pcall(node.setPosition, node))
        assert(not pcall(node.setPosition, node, 1))
        assert(not pcall(node.setPosition, node, 1, 2, 3))
        assert(not pcall(node.setPosition, node, false, {}))
        assert(not pcall(node.setScale, node))
        assert(not pcall(node.setScale, node, 1, 2, 3))

        -- Fast and generic methods coexist on the same class.
        assert(select('#', node:getPosition()) == 2)

        -- Inherited fast method through a derived userdata.
        local scene = ax.Scene:create()
        scene:setPosition(7, 8)
        assert(scene:getPositionX() == 7 and scene:getPositionY() == 8)
    )lua");
    if (!ok)
        return false;

    // A non-Node Axmol object must be rejected by the fast wrapper's type
    // check rather than invoking Node code on an unrelated pointer.
    auto* scheduler = new ax::Scheduler();
    axlua::push_object(state, scheduler);
    lua_setglobal(state, "__fast_non_node");
    ok = checkLua(state, R"lua(
        local setOpacity = ax.Node.setOpacity
        assert(not pcall(setOpacity, __fast_non_node, 128))
        assert(not pcall(setOpacity, {}, 128))
        assert(not pcall(setOpacity, 42, 128))
    )lua");
    lua_pushnil(state);
    lua_setglobal(state, "__fast_non_node");
    scheduler->release();

    // The fast wrapper must keep stale-userdata protection intact.
    auto* expired = new ax::Node();
    axlua::push_object(state, expired);
    lua_setglobal(state, "__fast_expired");
    std::thread([expired] { expired->release(); }).join();
    ok = checkLua(state, R"lua(
        assert(axlua.isnull(__fast_expired))
        local setPosition = ax.Node.setPosition
        assert(not pcall(setPosition, __fast_expired, 1, 2))
        assert(not pcall(setPosition, __fast_expired, {x = 1, y = 2}))
    )lua") &&
         ok;
    lua_pushnil(state);
    lua_setglobal(state, "__fast_expired");
    return ok;
}

// Force address reuse while still destroying through Object::release(), as
// required by WeakPtr. Worker destruction is joined before any object access:
// Axmol's native reference counting is intentionally not thread-safe.
class ReusedObject final : public ax::Object
{
public:
    static void operator delete(void*) noexcept {}
};

bool checkObjectLifetimes(lua_State* state)
{
    alignas(ReusedObject) unsigned char storage[sizeof(ReusedObject)];
    auto* object = ::new (static_cast<void*>(storage)) ReusedObject();
    // Exercise adapter creation (including its -1 stack index), then require
    // the typed/generated pusher to return that same userdata.
    axlua::adapter::push_object(state, object, "ax.Object");
    lua_setglobal(state, "__old_object");
    axlua::push_object(state, static_cast<ax::Object*>(object));
    lua_setglobal(state, "__same_object");
    axlua::adapter::push_object(state, object, "ax.Object");
    lua_setglobal(state, "__adapter_object");
    lua_pushcfunction(state, [](lua_State* L) {
        auto* value = static_cast<ax::Object*>(axlua::adapter::to_usertype(L, 1, nullptr));
        lua_pushinteger(L, value->getReferenceCount());
        return 1;
    });
    lua_setglobal(state, "__adapter_get_count");
    bool ok = checkLua(state, R"lua(
        assert(rawequal(__old_object, __same_object))
        assert(rawequal(__old_object, __adapter_object))
        __old_object.peer_value = 17
        assert(__same_object.peer_value == 17)
        __cached_get_count = __old_object.getReferenceCount
        assert(__cached_get_count(__old_object) == 1)
        assert(__adapter_get_count(__old_object) == 1)
    )lua");

    auto* secondary = luaL_newstate();
    if (secondary == nullptr)
    {
        object->release();
        return false;
    }
    luaL_openlibs(secondary);
    axlua::install(secondary);
    {
        auto module = axlua::Module::from(secondary, "ax");
        module.class_<ax::Object>("Object").method("getReferenceCount", &ax::Object::getReferenceCount);
    }
    axlua::push_object(secondary, static_cast<ax::Object*>(object));
    lua_setglobal(secondary, "old");

    std::thread([object] { object->release(); }).join();
    // Repeated reads must stay invalid, including cached generated/adapter
    // methods that bypass __index. Neither reading nor address reuse revives it.
    const char* expiredChecks = R"lua(
        for i = 1, 3 do
            assert(axlua.isnull(__old_object))
            assert(not pcall(__cached_get_count, __old_object))
            assert(not pcall(__adapter_get_count, __old_object))
        end
    )lua";
    ok                        = checkLua(state, expiredChecks) && ok;
    for (int iteration = 0; iteration < 2; ++iteration)
    {
        object = ::new (static_cast<void*>(storage)) ReusedObject();
        axlua::push_object(state, static_cast<ax::Object*>(object));
        lua_setglobal(state, "__new_object");
        ok = checkLua(state, expiredChecks) && ok;
        ok = checkLua(state, R"lua(
            assert(not rawequal(__old_object, __new_object))
            assert(__new_object.peer_value == nil)
            assert(__new_object:getReferenceCount() == 1)
        )lua") &&
             ok;
        ok = checkLua(secondary,
                      "assert(axlua.isnull(old)); assert(not pcall(function() old:getReferenceCount() end))") &&
             ok;
        object->release();
    }
    axlua::shutdown(secondary);
    lua_close(secondary);

    // Non-Object adapter values must retain identity without being cast to
    // Object, while borrowed Event userdata remains usable only in its scope.
    int nativeValue = 0;
    axlua::adapter::push_usertype(state, &nativeValue, nullptr);
    axlua::adapter::push_usertype(state, &nativeValue, nullptr);
    ok = lua_rawequal(state, -1, -2) && ok;
    lua_pop(state, 2);
    if (!checkLua(state, R"lua(
        function __borrowed_event(event)
            assert(not event:isStopped())
            event:stopPropagation()
            __saved_event = event
        end
    )lua"))
        return false;
    lua_getglobal(state, "__borrowed_event");
    axlua::Callback<void(ax::Event*)> callback(state, -1);
    lua_pop(state, 1);
    ax::Event event(ax::Event::Type::CUSTOM);
    callback(&event);
    ok = event.isStopped() && ok;
    return checkLua(
               state,
               "assert(axlua.isnull(__saved_event)); assert(not pcall(function() __saved_event:isStopped() end))") &&
           ok;
}

bool checkCallbackLifetimes(lua_State* state)
{
    if (!checkLua(state, "__weak_callbacks = setmetatable({}, {__mode='v'})"))
        return false;
    std::vector<axlua::Callback<int()>> callbacks;
    for (int index = 1; index <= 256; ++index)
    {
        lua_pushinteger(state, index);
        lua_pushcclosure(state, [](lua_State* L) {
            lua_pushvalue(L, lua_upvalueindex(1));
            return 1;
        }, 1);
        callbacks.emplace_back(state, -1);
        lua_getglobal(state, "__weak_callbacks");
        lua_pushvalue(state, -2);
        lua_rawseti(state, -2, index);
        lua_pop(state, 2);
    }
    const int top     = lua_gettop(state);
    int foreignResult = -1;
    std::thread([&] { foreignResult = callbacks.front()(); }).join();
    bool ok = foreignResult == 0 && lua_gettop(state) == top && callbacks.front()() == 1;
    std::thread([owned = std::move(callbacks)]() mutable { owned.clear(); }).join();
    // No subsequent callback invocation: the normal frame boundary must
    // collect references dropped on a worker, without waiting for VM close.
    auto* dispatcher      = ax::Director::getInstance()->getEventDispatcher();
    const bool wasEnabled = dispatcher->isEnabled();
    // Smoke mode deliberately never creates a render view, which normally
    // enables the dispatcher. Enable it just for this simulated frame.
    dispatcher->setEnabled(true);
    dispatcher->dispatchCustomEvent(ax::Director::EVENT_BEFORE_UPDATE);
    dispatcher->setEnabled(wasEnabled);
    lua_gc(state, LUA_GCCOLLECT, 0);
    ok = checkLua(state, "assert(next(__weak_callbacks) == nil)") && ok;

    for (int iteration = 0; iteration < 16; ++iteration)
    {
        auto* vm = luaL_newstate();
        if (vm == nullptr)
            return false;
        axlua::install(vm);
        lua_pushcfunction(vm, [](lua_State* L) {
            lua_pushinteger(L, 42);
            return 1;
        });
        axlua::Callback<int()> surviving(vm, -1);
        std::vector<axlua::Callback<int()>> released;
        for (int index = 0; index < 32; ++index)
            released.emplace_back(vm, -1);
        lua_pop(vm, 1);
        ok = surviving() == 42 && ok;
        std::atomic_bool start{false};
        std::thread worker([owned = std::move(released), &start]() mutable {
            while (!start.load(std::memory_order_acquire))
                std::this_thread::yield();
            owned.clear();
        });
        start.store(true, std::memory_order_release);
        axlua::shutdown(vm);
        lua_close(vm);
        worker.join();
        ok = surviving() == 0 && ok;
        // Reopen while the old callback is retained. Pending releases from
        // the closed VM must not unref callbacks belonging to this VM.
        auto* reopened = luaL_newstate();
        if (reopened == nullptr)
            return false;
        axlua::install(reopened);
        lua_pushcfunction(reopened, [](lua_State* L) {
            lua_pushinteger(L, 7);
            return 1;
        });
        axlua::Callback<int()> fresh(reopened, -1);
        lua_pop(reopened, 1);
        std::thread([old = std::move(surviving)]() mutable { old.reset(); }).join();
        axlua::drain_pending_callback_unrefs(reopened);
        ok = fresh() == 7 && ok;
        axlua::shutdown(reopened);
        lua_close(reopened);
    }
    return ok;
}

bool checkScheduler(lua_State* state)
{
    auto* scheduler = new ax::Scheduler();
    axlua::push_object(state, scheduler);
    lua_setglobal(state, "__scheduler");
    bool ok = checkLua(state, R"lua(
        __scheduled_calls = 0
        __schedule_id = __scheduler:scheduleScriptFunc(function(dt)
            assert(dt > 0)
            __scheduled_calls = __scheduled_calls + 1
            if __scheduled_calls == 2 then __scheduler:unscheduleScriptEntry(__schedule_id) end
        end, 0, false)
        assert(type(__schedule_id) == 'number')
        __paused_id = __scheduler:scheduleScriptFunc(function() error('paused timer ran') end, 0, true)
    )lua");
    for (int index = 0; index < 6; ++index)
        scheduler->update(0.1f);
    ok = checkLua(state, "assert(__scheduled_calls == 2); __scheduler:unscheduleScriptEntry(__paused_id)") && ok;

    // Preserve the native integer-handler API as well as Lua's callback API.
    ok =
        checkLua(
            state,
            "__legacy_calls = 0; function __legacy_tick(dt) assert(dt > 0); __legacy_calls = __legacy_calls + 1 end") &&
        ok;
    lua_getglobal(state, "__legacy_tick");
    const auto handler = axlua::adapter::ref_function(state, -1, 0);
    lua_pop(state, 1);
    const auto entry = scheduler->scheduleScriptFunc(static_cast<unsigned int>(handler), 0, false);
    scheduler->update(0.1f);
    scheduler->update(0.1f);
    scheduler->update(0.1f);
    scheduler->unscheduleScriptEntry(entry);
    scheduler->update(0.1f);
    ok = checkLua(state, "assert(__legacy_calls == 2)") && ok;
    scheduler->release();
    return ok;
}
}  // namespace

int runLuaBindingSmoke(lua_State* state)
{
    if (state == nullptr)
        return 1;

    if (!checkMethodDispatch(state) || !checkExpiredLookup(state) || !checkFastBindings(state))
        return 1;
    fprintf(stderr, "lua-binding-smoke: dispatch, expired lookup, and fast bindings passed\n");

    if (!checkObjectLifetimes(state) || !checkCallbackLifetimes(state) || !checkScheduler(state))
    {
        fprintf(stderr, "lua-binding-smoke failed: lifecycle/scheduler regression\n");
        return 1;
    }

    // MeshCollider3D's vertex-array adapter passes the element at -1. A
    // relative-index regression silently collapsed all vertices to zero.
    const int stackTop = lua_gettop(state);
    if (luaL_dostring(state, "return {x=1.25, y=-2.5, z=3.75}") != LUA_OK)
    {
        lua_settop(state, stackTop);
        return 1;
    }
    ax::Vec3 vertex;
    const bool converted = luaval_to_vec3(state, -1, &vertex);
    const bool balanced  = lua_gettop(state) == stackTop + 1;
    lua_settop(state, stackTop);
    if (!converted || !balanced || vertex != ax::Vec3(1.25f, -2.5f, 3.75f))
    {
        fprintf(stderr, "lua-binding-smoke failed: negative-index Vec3 conversion\n");
        return 1;
    }
    auto* lifecycleNode = ax::Node::create();
    lifecycleNode->retain();
    axlua::push_object(state, lifecycleNode);
    lua_setglobal(state, "__axlua_runtime_lifecycle_node");

    // A second VM may be created by a host integration. Closing it must not
    // erase exposure bookkeeping for the engine's primary Lua VM.
    auto* secondaryState = luaL_newstate();
    if (secondaryState == nullptr)
    {
        lifecycleNode->release();
        return 1;
    }
    luaL_openlibs(secondaryState);
    axlua::install(secondaryState);
    axlua::remember_object(secondaryState, lifecycleNode);
    axlua::shutdown(secondaryState);
    lua_close(secondaryState);

    const char* script = R"lua(
        local material = { density = 1.0, restitution = 0.25, friction = 0.5 }
        local size = {x=1, y=1}
        local zero = {x=0, y=0}
        local calls = {
            function() return ax.Rigidbody2D.createCircle(1.0) end,
            function() return ax.Rigidbody2D.createCircle(1.0, material) end,
            function() return ax.Rigidbody2D.createCircle(1.0, material, zero) end,
            function() return ax.Rigidbody2D:createCircle(1.0) end,
            function() return ax.Rigidbody2D:createCircle(1.0, material) end,
            function() return ax.Rigidbody2D:createCircle(1.0, material, zero) end,
            function() return ax.Rigidbody2D.createBox(size) end,
            function() return ax.Rigidbody2D.createBox(size, material) end,
            function() return ax.Rigidbody2D.createBox(size, material, zero) end,
            function() return ax.Rigidbody2D:createBox(size) end,
            function() return ax.Rigidbody2D:createBox(size, material) end,
            function() return ax.Rigidbody2D:createBox(size, material, zero) end,
            function() return ax.Rigidbody2D.createEdgeBox(size) end,
            function() return ax.Rigidbody2D.createEdgeBox(size, material) end,
            function() return ax.Rigidbody2D.createEdgeBox(size, material, zero) end,
            function() return ax.Rigidbody2D:createEdgeBox(size) end,
            function() return ax.Rigidbody2D:createEdgeBox(size, material) end,
            function() return ax.Rigidbody2D:createEdgeBox(size, material, zero) end,
        }
        for _, call in ipairs(calls) do
            local ok, value = pcall(call)
            assert(ok, value)
            assert(value ~= nil)
        end

        -- The C++ harness enters this node after this script returns.  Node
        -- lifecycle methods are intentionally not public Lua API methods.
        local node = assert(__axlua_runtime_lifecycle_node)
        node:setContentSize(size)
        node:setAnchorPoint(zero)
        node:setAnchorPoint(0, 0)
        node:setRotationQuat({x = 0, y = 0, z = 0, w = 1})
        local positionX, positionY = node:getPosition()
        assert(positionX == 0 and positionY == 0)
        node:setOnEnterCallback(function() error("expected protected callback error") end)

        -- Ensure object pusher lookup uses the owning VM when this code runs
        -- in a coroutine rather than indexing registration by the coroutine.
        local co = coroutine.create(function()
            local child = ax.Node:create()
            assert(child ~= nil)
        end)
        assert(coroutine.resume(co))

        -- Value/ValueVector must keep the old scalar/table ABI instead of
        -- becoming opaque sol2 userdata.
        local values = ax.FileUtils:getInstance():getValueVectorFromFile("__axlua_missing_values__.plist")
        assert(type(values) == "table")

        -- Physics3D is lazy in normal applications.  This headless process
        -- deliberately does not initialize a Jolt world, so it verifies that
        -- the complete module and its table-factory adapters are registered
        -- without constructing native collision shapes.
        assert(type(ax.MeshCollider3D) == "table")
        assert(type(ax.MeshCollider3D.create) == "function")
        assert(type(ax.Rigidbody3D) == "table")
        assert(type(ax.Rigidbody3D.create) == "function")
    )lua";

    if (luaL_dostring(state, script) != LUA_OK)
    {
        fprintf(stderr, "lua-binding-smoke failed: %s\n", lua_tostring(state, -1));
        lifecycleNode->release();
        return 1;
    }

    // Lua errors must stay inside the protected callback path.  This used to
    // cross sol2's std::function shim and panic the Lua VM.
    lifecycleNode->onEnter();
    if (luaL_dostring(state, R"lua(
        local node = assert(__axlua_runtime_lifecycle_node)
        local co = coroutine.create(function()
            node:setOnEnterCallback(function()
                _G.__axlua_coroutine_callback_ran = true
            end)
        end)
        assert(coroutine.resume(co))
    )lua") != LUA_OK)
    {
        fprintf(stderr, "lua-binding-smoke failed while assigning coroutine callback: %s\n", lua_tostring(state, -1));
        lifecycleNode->onExit();
        lifecycleNode->release();
        return 1;
    }
    lifecycleNode->onEnter();
    if (luaL_dostring(state, R"lua(
        assert(__axlua_coroutine_callback_ran)
        assert(__axlua_runtime_lifecycle_node:setOnEnterCallback(nil) == nil)
    )lua") != LUA_OK)
    {
        fprintf(stderr, "lua-binding-smoke failed while verifying coroutine callback: %s\n", lua_tostring(state, -1));
        lifecycleNode->onExit();
        lifecycleNode->release();
        return 1;
    }
    lifecycleNode->onExit();
    lifecycleNode->release();
    return 0;
}

int finishLuaBindingSmoke(lua_State* state)
{
    bool finalized = false;
    lua_newuserdata(state, 1);
    lua_newtable(state);
    lua_pushlightuserdata(state, &finalized);
    lua_pushcclosure(state, [](lua_State* L) {
        *static_cast<bool*>(lua_touserdata(L, lua_upvalueindex(1))) = true;
        return 0;
    }, 1);
    lua_setfield(state, -2, "__gc");
    lua_setmetatable(state, -2);
    lua_setfield(state, LUA_REGISTRYINDEX, "axlua.smoke.close_sentinel");
    lua_pushcfunction(state, [](lua_State* L) {
        lua_pushinteger(L, 42);
        return 1;
    });
    axlua::Callback<int()> surviving(state, -1);
    lua_pop(state, 1);
    const bool callableBeforeClose = surviving() == 42;
    ax::WeakPtr<ax::LuaStack> stack(ax::LuaEngine::getInstance()->getLuaStack());

    // Release the initial autorelease ownership before the engine's retain.
    // LuaStack's real destructor must perform shutdown and lua_close itself.
    ax::PoolManager::getInstance()->getCurrentPool()->clear();
    ax::ScriptEngineManager::getInstance()->removeScriptEngine();
    const bool closed = finalized && stack.expired();
    if (!closed || !callableBeforeClose || surviving() != 0)
    {
        fprintf(stderr, "lua-binding-smoke failed: main VM did not close safely\n");
        return 1;
    }
    fprintf(stdout, "lua-binding-smoke: main VM close passed\n");
    return 0;
}
