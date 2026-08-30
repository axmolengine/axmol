#include "lua-bindings/runtime/LuaEngine.h"
#include "lua-bindings/runtime/LuaStack.h"

#include <cstdio>

int main()
{
    auto* stack        = ax::LuaEngine::getInstance()->getLuaStack();
    auto* state        = stack->getLuaState();
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

        -- nil must clear a generated std::function callback rather than
        -- leaving a deferred null callback that crashes on the next event.
        local node = ax.Node:create()
        node:setOnEnterCallback(function() error("expected protected callback error") end)
        node:onEnter()
        node:setOnEnterCallback(nil)
        node:onEnter()

        -- Value/ValueVector must keep the old scalar/table ABI instead of
        -- becoming opaque sol2 userdata.
        local values = ax.FileUtils:getInstance():getValueVectorFromFile("__axlua_missing_values__.plist")
        assert(type(values) == "table")

        -- std::span-based mesh colliders keep the legacy Lua table factory;
        -- verify the returned derived userdata is accepted by a Collider3D*
        -- parameter (both static call spellings).
        if ax.MeshCollider3D ~= nil then
            local triangles = {
                { x = 0, y = 0, z = 0 },
                { x = 1, y = 0, z = 0 },
                { x = 0, y = 1, z = 0 },
            }
            local collider = ax.MeshCollider3D:create(triangles)
            assert(collider ~= nil)
            local colliderDot = ax.MeshCollider3D.create(triangles)
            assert(colliderDot ~= nil)
            local body = ax.Rigidbody3D:create(collider, 0.0)
            assert(body ~= nil)
        end
    )lua";

    if (luaL_dostring(state, script) != LUA_OK)
    {
        std::fprintf(stderr, "lua-binding-runtime failed: %s\n", lua_tostring(state, -1));
        return 1;
    }
    return 0;
}
