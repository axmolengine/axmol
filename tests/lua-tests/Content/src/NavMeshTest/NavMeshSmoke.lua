-- Graphical regression using the actual sample and its move/off-mesh callback.
-- WASM: lua-tests.html?navmesh-smoke; native: AXMOL_NAVMESH_SMOKE=1.
-- Requires navigation and Physics3D assets; the headless ABI smoke stays separate.

local function worldPosition(agent)
    local transform = agent:getOwner():getNodeToWorldTransform()
    return ax.vec3(transform[13], transform[14], transform[15])
end

local function distanceSquared(a, b)
    return (a.x-b.x)^2 + (a.z-b.z)^2
end

local function checkMovement(scene)
    local elapsed = 0
    local start, destination
    local key = "navmesh-movement-smoke"
    scene:schedule(function(dt)
        local ok, err = pcall(function()
            elapsed = elapsed + dt
            assert(#scene._agents > 0, "initial agent was not created")
            local agent = scene._agents[1][1]
            if not start then
                start = worldPosition(agent)
                local hit, result = scene:getPhysicsWorld3D():rayCast(ax.vec3(10, 50, 10), ax.vec3(10, -50, 10), {})
                assert(hit, "destination ray missed the ground")
                destination = result.hitPosition
                scene:moveAgents(destination)
            else
                local position = worldPosition(agent)
                if distanceSquared(position, destination) < 1 then
                    assert(distanceSquared(position, start) > 1, "agent did not move")
                    scene:unschedule(key)
                    print(string.format("navmesh-smoke: PASS (%.2f, %.2f, %.2f) -> (%.2f, %.2f, %.2f)",
                        start.x, start.y, start.z, position.x, position.y, position.z))
                else
                    assert(elapsed < 15, "agent did not reach the destination within 15 simulation seconds")
                end
            end
        end)
        if not ok then
            scene:unschedule(key)
            print("navmesh-smoke: FAIL: " .. tostring(err))
        end
    end, 0, key)
    ax.Director:getInstance():replaceScene(scene)
end

-- Yield to the browser between physics-world creation and its first step:
-- Emscripten may need to start additional workers beyond the initial pool.
local launcher = ax.Scene:create()
ax.Director:getInstance():replaceScene(launcher)
launcher:scheduleOnce(function()
    local scene = NavMeshTest()
    scene:retain()
    launcher:scheduleOnce(function()
        checkMovement(scene)
        scene:release()
    end, 0.2, "navmesh-smoke-enter")
end, 0.1, "navmesh-smoke-create")
