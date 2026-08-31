#include "LuaBindingSmoke.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "axmol/scene/Node.h"

#include <stdio.h>

int runLuaBindingSmoke(lua_State* state)
{
    if (state == nullptr)
        return 1;

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
        lifecycleNode->release();
        return 1;
    }
    lifecycleNode->release();
    return 0;
}
