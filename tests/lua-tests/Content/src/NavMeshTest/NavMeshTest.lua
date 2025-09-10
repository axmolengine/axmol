local AgentUserData =
{
    time = 0
}

local function jump(v1, v2, height, t)
    local out = {}
    out.x = v1.x + t * (v2.x - v1.x)
    out.y = v1.y + t * (v2.y - v1.y)
    out.z = v1.z + t * (v2.z - v1.z)
    out.y = out.y + height * math.sin(math.pi * t)
    return out
end

local actionManager = ax.Director:getInstance():getActionManager()

----------------------------------------
----NavMeshBaseTestDemo
----------------------------------------
local NavMeshBaseTestDemo = class("NavMeshBaseTestDemo", function ()
    -- body
    local scene = ax.Scene:createWithPhysics()
    return scene
end)

function NavMeshBaseTestDemo:ctor()

    TestCastScene.initWithLayer(self)
    TestCastScene.titleLabel:setString(self:title())
    TestCastScene.subtitleLabel:setString(self:subtitle())

    self:init()

    local function onNodeEvent(event)
        if "enter" == event then
            self:onEnter()
        elseif "exit" == event then
            self:onExit()
        end
    end

    self:registerScriptHandler(onNodeEvent)
end

function NavMeshBaseTestDemo:title()
    return "Physics3D Test"
end

function NavMeshBaseTestDemo:subtitle()
    return ""
end

function NavMeshBaseTestDemo:init()
    self._angle = 0.0
    self._agents = {}
    local size = ax.Director:getInstance():getLogicalSize()
    self._camera = ax.Camera:createPerspective(30.0, size.width / size.height, 1.0, 1000.0)
    self._camera:setPosition3D(ax.vec3(0.0, 50.0, 100.0))
    self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))
    self._camera:setCameraFlag(ax.CameraFlag.USER1)
    self:addChild(self._camera)

    self:registerTouchEvent()

    self:initScene()

    self:scheduleUpdateWithPriorityLua(function(dt)
        if #self._agents == 0 then
            return
        end

        if not self._resumeFlag and nil ~= self._agentNode then
            self._resumeFlag = true
            actionManager:resumeTarget(self._agentNode)
        end

        local currentVelocity = nil
        local speed = 0
        for i = 1, #self._agents do
            currentVelocity = self._agents[i][1]:getCurrentVelocity()
            speed = math.sqrt(currentVelocity.x * currentVelocity.x + currentVelocity.y * currentVelocity.y + currentVelocity.z * currentVelocity.z) * 0.2
            if speed < 0 then
                speed = 0.0
            end
            self._agents[i][2]:setSpeed(speed)
        end
    end, 0)

    self:extend()
end

function NavMeshBaseTestDemo:onEnter()
    local hitResult = {}
    local ret = false
    local physicsWorld = self:getPhysics3DWorld()
    ret, hitResult = physicsWorld:rayCast(ax.vec3(0.0, 50.0, 0.0), ax.vec3(0.0, -50.0, 0.0), hitResult)
    self:createAgent(hitResult.hitPosition)
end

function NavMeshBaseTestDemo:onExit()
    self:unscheduleUpdate()
end

function NavMeshBaseTestDemo:registerTouchEvent()

end

function NavMeshBaseTestDemo:extend()

end

function NavMeshBaseTestDemo:initScene()
    self:getPhysics3DWorld():setDebugDrawEnable(false)

    local trianglesList = ax.Bundle3D:getTrianglesList("NavMesh/scene.obj")

    local rbDes = {}
    rbDes.mass = 0.0
    rbDes.shape = ax.Physics3DShape:createMesh(trianglesList, math.floor(#trianglesList / 3))
    local rigidBody = ax.Physics3DRigidBody:create(rbDes)
    local component = ax.Physics3DComponent:create(rigidBody)
    local sprite = ax.Sprite3D:create("NavMesh/scene.obj")
    sprite:addComponent(component)
    sprite:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(sprite)
    self:setPhysics3DDebugCamera(self._camera)

    local navMesh = ax.NavMesh:create("NavMesh/all_tiles_tilecache.bin", "NavMesh/geomset.txt")
    navMesh:setDebugDrawEnable(true)
    self:setNavMesh(navMesh)
    self:setNavMeshDebugCamera(self._camera)

    local ambientLight = ax.AmbientLight:create(ax.color32(64, 64, 64))
    ambientLight:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(ambientLight)

    local dirLight = ax.DirectionLight:create(ax.vec3(1.2, -1.1, 0.5), ax.color32(255, 255, 255))
    dirLight:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(dirLight)
end

function NavMeshBaseTestDemo:createAgent(pos)
    local filePath = "MeshRendererTest/girl.c3b"

    local navMeshAgentParam = {}
    navMeshAgentParam.radius = 2.0
    navMeshAgentParam.height = 8.0
    navMeshAgentParam.maxSpeed = 8.0

    local agent = ax.NavMeshAgent:create(navMeshAgentParam)
    local agentNode = ax.Sprite3D:create(filePath)
    agent:setOrientationRefAxes(ax.vec3(-1.0, 0.0, 1.0))
    agent.userdata = 0.0
    agentNode:setScale(0.05)
    agentNode:addComponent(agent)

    local node = ax.Node:create()
    node:addChild(agentNode)
    node:setPosition3D(pos)
    node:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(node)

    local animation = ax.Animation3D:create(filePath)
    local animate   = ax.Animate3D:create(animation)
    if nil ~= animate then
        agentNode:runAction(ax.RepeatForever:create(animate))
        animate:setSpeed(0.0)
    end

    self._agents[#self._agents + 1] =  {agent, animate}
end

function NavMeshBaseTestDemo:createObstacle(pos)
    local obstacle = ax.NavMeshObstacle:create(2.0, 8.0)
    local obstacleNode = ax.Sprite3D:create("MeshRendererTest/cylinder.c3b")
    obstacleNode:setPosition3D(ax.vec3(pos.x, pos.y -0.5, pos.z))
    obstacleNode:setRotation3D(ax.vec3(-90.0, 0.0, 0.0))
    obstacleNode:setScale(0.3)
    obstacleNode:addComponent(obstacle)
    obstacleNode:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(obstacleNode)
end

function NavMeshBaseTestDemo:moveAgents(des)
    if #self._agents == 0 then
        return
    end

    local agent = nil
    for i = 1, #self._agents do
        self._agents[i][1]:move(des, function (agent, totalTimeAfterMove)
            local data = agent.userdata
            if agent:isOnOffMeshLink() then
                agent:setAutoTraverseOffMeshLink(false)
                agent:setAutoOrientation(false)

                local linkdata = agent:getCurrentOffMeshLinkData()

                agent:getOwner():setPosition3D(jump(linkdata.startPosition, linkdata.endPosition, 10.0, data))
                local dir = ax.vec3sub(linkdata.endPosition, linkdata.startPosition)
                dir.y = 0.0
                dir = ax.vec3normalize(dir)
                local axes = ax.vec3(0.0, 0.0, 0.0)
                local refAxes = ax.vec3(-1.0, 0.0, 1.0)
                refAxes = ax.vec3normalize(refAxes)

                axes = vec3_cross(refAxes, dir, axes)

                local angle = ax.vec3dot(refAxes, dir)

                local quaternion = ax.quaternion_createFromAxisAngle(axes, math.acos(angle))

                agent:getOwner():setRotationQuat(quaternion)
                agent.userdata = agent.userdata + 0.01
                if 1.0 < agent.userdata then
                    agent:completeOffMeshLink()
                    agent:setAutoOrientation(true)
                    agent.userdata = 0.0
                end
            end
        end)
    end
end


----------------------------------------
----NavMeshBaseTestDemo
----------------------------------------

local NavMeshBasicTestDemo = class("NavMeshBasicTestDemo", NavMeshBaseTestDemo)

function NavMeshBasicTestDemo:title()
    return "Navigation Mesh Test"
end

function NavMeshBasicTestDemo:subtitle()
    return "Basic Test"
end

function NavMeshBasicTestDemo:registerTouchEvent()
    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(function(touches, event)
        self._needMoveAgents = true
    end,ax.Handler.EVENT_TOUCHES_BEGAN)

    listener:registerScriptHandler(function(touches, event)

        if #touches > 0 and self._camera ~= nil then
            local touch = touches[1]
            local delta = touch:getDelta()

            self._angle = self._angle - delta.x * math.pi / 180.0
            self._camera:setPosition3D(ax.vec3(100.0 * math.sin(self._angle), 50.0, 100.0 * math.cos(self._angle)))
            self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))

            if (delta.x * delta.x + delta.y * delta.y) > 16 then
                self._needMoveAgents = false
            end
        end
    end, ax.Handler.EVENT_TOUCHES_MOVED)

    listener:registerScriptHandler(function(touches, event)
        if not self._needMoveAgents then
            return
        end
        local physicsWorld = self:getPhysics3DWorld()
        if #touches > 0 then
            local touch = touches[1]
            local location = touch:getLocationInView()
            local nearP = ax.vec3(location.x, location.y, 0.0)
            local farP  = ax.vec3(location.x, location.y, 1.0)

            local size = ax.Director:getInstance():getLogicalSize()
            nearP = self._camera:unproject(size, nearP, nearP)
            farP  = self._camera:unproject(size, farP, farP)

            local hitResult = {}
            local ret = false
            ret, hitResult = physicsWorld:rayCast(nearP, farP, hitResult)
            self:moveAgents(hitResult.hitPosition)
        end
    end, ax.Handler.EVENT_TOUCHES_ENDED)

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function NavMeshBasicTestDemo:extend()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 15
    local debugLabel = ax.Label:createWithTTF(ttfConfig,"Debug Draw ON")
    local menuItem = ax.MenuItemLabel:create(debugLabel)
    menuItem:registerScriptTapHandler(function (tag, sender)
        local scene = ax.Director:getInstance():getRunningScene()
        local enabledDebug = not scene:getNavMesh():isDebugDrawEnabled()
        scene:getNavMesh():setDebugDrawEnable(enabledDebug)

        if enabledDebug then
            debugLabel:setString("DebugDraw ON")
        else
            debugLabel:setString("DebugDraw OFF")
        end
    end)

    menuItem:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem:setPosition(ax.p(VisibleRect:left().x, VisibleRect:top().y - 100))
    local menu = ax.Menu:create(menuItem)
    menu:setPosition(ax.p(0.0, 0.0))
    self:addChild(menu)
end
----------------------------------------
----NavMeshAdvanceTestDemo
----------------------------------------

local NavMeshAdvanceTestDemo = class("NavMeshAdvanceTestDemo", NavMeshBaseTestDemo)

function NavMeshAdvanceTestDemo:title()
    return "Navigation Mesh Test"
end

function NavMeshAdvanceTestDemo:subtitle()
    return "Advance Test"
end

function NavMeshAdvanceTestDemo:registerTouchEvent()
    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(function(touches, event)
        self._needMoveAgents = true
    end,ax.Handler.EVENT_TOUCHES_BEGAN)

    listener:registerScriptHandler(function(touches, event)

        if #touches > 0 and self._camera ~= nil then
            local touch = touches[1]
            local delta = touch:getDelta()

            self._angle = self._angle - delta.x * math.pi / 180.0
            self._camera:setPosition3D(ax.vec3(100.0 * math.sin(self._angle), 50.0, 100.0 * math.cos(self._angle)))
            self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))

            if (delta.x * delta.x + delta.y * delta.y) > 16 then
                self._needMoveAgents = false
            end
        end
    end, ax.Handler.EVENT_TOUCHES_MOVED)

    listener:registerScriptHandler(function(touches, event)
        if not self._needMoveAgents then
            return
        end
        local physicsWorld = self:getPhysics3DWorld()
        if #touches > 0 then
            local touch = touches[1]
            local location = touch:getLocationInView()
            local nearP = ax.vec3(location.x, location.y, 0.0)
            local farP  = ax.vec3(location.x, location.y, 1.0)

            local size = ax.Director:getInstance():getLogicalSize()
            nearP = self._camera:unproject(size, nearP, nearP)
            farP  = self._camera:unproject(size, farP, farP)

            local hitResult = {}
            local ret = false
            ret, hitResult = physicsWorld:rayCast(nearP, farP, hitResult)
            self:moveAgents(hitResult.hitPosition)
        end
    end, ax.Handler.EVENT_TOUCHES_ENDED)

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function NavMeshAdvanceTestDemo:extend()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 15
    local obstacleLabel = ax.Label:createWithTTF(ttfConfig,"Create Obstacle")
    local menuItem0 = ax.MenuItemLabel:create(obstacleLabel)
    menuItem0:registerScriptTapHandler(function (tag, sender)
        local scene = ax.Director:getInstance():getRunningScene()
        local x = math.random(-50, 50)
        local z = math.random(-50.0, 50.0)

        local hitResult = {}
        local ret = false
        ret, hitResult = scene:getPhysics3DWorld():rayCast(ax.vec3(x, 50.0, z), ax.vec3(x, -50.0, z), hitResult)
        self:createObstacle(hitResult.hitPosition)
    end)
    menuItem0:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem0:setPosition(ax.p(VisibleRect:left().x, VisibleRect:top().y - 50))

    local agentLabel    = ax.Label:createWithTTF(ttfConfig,"Create Agent")
    local menuItem1 = ax.MenuItemLabel:create(agentLabel)
    menuItem1:registerScriptTapHandler(function (tag, sender)
        local scene = ax.Director:getInstance():getRunningScene()
        local x = math.random(-50, 50)
        local z = math.random(-50.0, 50.0)

        local hitResult = {}
        local ret    = false
        ret, hitResult = scene:getPhysics3DWorld():rayCast(ax.vec3(x, 50.0, z), ax.vec3(x, -50.0, z), hitResult)
        self:createAgent(hitResult.hitPosition)
    end)
    menuItem1:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem1:setPosition(ax.p(VisibleRect:left().x, VisibleRect:top().y - 100))

    local debugLabel    = ax.Label:createWithTTF(ttfConfig,"Debug Draw ON")
    local menuItem2 = ax.MenuItemLabel:create(debugLabel)
    menuItem2:registerScriptTapHandler(function (tag, sender)
        local scene = ax.Director:getInstance():getRunningScene()
        local enabledDebug = not scene:getNavMesh():isDebugDrawEnabled()
        scene:getNavMesh():setDebugDrawEnable(enabledDebug)

        if enabledDebug then
            debugLabel:setString("DebugDraw ON")
        else
            debugLabel:setString("DebugDraw OFF")
        end
    end)
    menuItem2:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem2:setPosition(ax.p(VisibleRect:left().x, VisibleRect:top().y - 150))

    local menu = ax.Menu:create(menuItem0, menuItem1, menuItem2)
    menu:setPosition(ax.p(0.0, 0.0))
    self:addChild(menu)
end

----------------------------------------
----NavMeshTest
----------------------------------------
function NavMeshTest()
    Helper.usePhysics = true

    TestCastScene.createFunctionTable =
    {
        NavMeshBasicTestDemo.create,
        NavMeshAdvanceTestDemo.create,
    }

    local scene = NavMeshBasicTestDemo.create()
    scene:addChild(CreateBackMenuItem())

    return scene
end
