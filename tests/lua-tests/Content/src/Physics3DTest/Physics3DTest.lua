local START_POS_X = -0.5
local START_POS_Y = -2.5
local START_POS_Z = -0.5

local ARRAY_SIZE_X = 4
local ARRAY_SIZE_Y = 3
local ARRAY_SIZE_Z = 4

local ContactEventBits =
{
    None = 0,
    PreSolve = 1,
    Hit = 2,
    Contact = 4,
    Sensor = 8,
    AllBits = 15,
}

local CollisionDetectionMode =
{
    Discrete = 0,
    Continuous = 1,
}

local boss = {"MeshRendererTest/boss.c3b", "MeshRendererTest/boss.obj"}

local function setScale3D(node, scale)
    node:setScaleX(scale.x)
    node:setScaleY(scale.y)
    node:setScaleZ(scale.z)
end

local function createMeshWithRigidbody(size, mass, pos, meshFile, texFile, name, disableGravity)
    local rigidbody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(size), mass)
    local mesh = ax.MeshRenderer:create(meshFile)
    if texFile ~= nil and texFile ~= "" then
        mesh:setTexture(texFile)
    end
    setScale3D(mesh, size)
    mesh:setPosition3D(pos)
    mesh:addComponent(rigidbody)
    mesh:setCameraMask(ax.CameraFlag.USER1)
    if name ~= nil then
        mesh:setName(name)
    end
    if disableGravity then
        rigidbody:setGravityScale(0)
    end
    return mesh, rigidbody
end

----------------------------------------
----Physics3DTestDemo
----------------------------------------
local Physics3DTestDemo = class("Physics3DTestDemo", function ()
    return ax.Layer:create()
end)

function Physics3DTestDemo:ctor()
    Helper.initWithLayer(self)
    Helper.titleLabel:setString(self:title())
    Helper.subtitleLabel:setString(self:subtitle())

    local function onNodeEvent(event)
        if "enter" == event then
            self:onEnter()
        elseif "exit" == event then
            self:onExit()
        end
    end

    self:registerScriptHandler(onNodeEvent)
end

function Physics3DTestDemo:title()
    return "Physics3D Test"
end

function Physics3DTestDemo:subtitle()
    return ""
end

function Physics3DTestDemo:shootBox(des)
    local cameraPosition = self._camera:getPosition3D()
    local linearVec = ax.vec3normalize(ax.vec3sub(des, cameraPosition))
    linearVec = ax.vec3mul(linearVec, 100)

    local mesh = ax.MeshRenderer:create("MeshRendererTest/box.c3t")
    mesh:setTexture("Images/Icon.png")
    mesh:setPosition3D(cameraPosition)
    mesh:setScale(0.5)
    mesh:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(mesh)

    local rigidBody = ax.Rigidbody3D:create(ax.SphereCollider3D:create(0.4), 1.0)
    rigidBody:setLinearVelocity(linearVec)
    rigidBody:setCollisionDetectionMode(CollisionDetectionMode.Continuous)
    rigidBody:setAllowSleeping(false)
    mesh:addComponent(rigidBody)
    rigidBody:syncNodeToPhysics()
    rigidBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.PHYSICS_TO_NODE)
end

function Physics3DTestDemo:onEnter()
    local scene = ax.Director:getInstance():getRunningScene()
    if nil == scene then
        return
    end

    self._physicsScene = scene
    local physics3DWorld = scene:getPhysicsWorld3D()
    physics3DWorld:setDebugDrawEnabled(false)

    local size = ax.Director:getInstance():getCanvasSize()
    self._camera = ax.Camera:createPerspective(30.0, size.width / size.height, 1.0, 1000.0)
    self._camera:setPosition3D(ax.vec3(0.0, 50.0, 100.0))
    self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))
    self._camera:setCameraFlag(ax.CameraFlag.USER1)
    self:addChild(self._camera)

    if Helper.index ~= 2 then
        local listener = ax.PointerEventListener:create()
        listener:registerScriptHandler(function(event)
            self._needShootBox = true
        end, ax.Handler.EVENT_POINTER_DOWN)

        listener:registerScriptHandler(function(event)
            if event ~= nil and self._camera ~= nil then
                local delta = event:getDelta()

                self._angle = self._angle - delta.x * math.pi / 180.0
                self._camera:setPosition3D(ax.vec3(100.0 * math.sin(self._angle), 50.0, 100.0 * math.cos(self._angle)))
                self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))

                if (delta.x * delta.x + delta.y * delta.y) > 16 then
                    self._needShootBox = false
                end
            end
        end, ax.Handler.EVENT_POINTER_MOVE)

        listener:registerScriptHandler(function(event)
            if self._needShootBox == false then
                return
            end

            if event ~= nil then
                local location = event:getScreenLocation()
                local nearP = ax.vec3(location.x, location.y, -1.0)
                local farP = ax.vec3(location.x, location.y, 1.0)
                nearP = self._camera:deprojectScreenToWorld(nearP)
                farP = self._camera:deprojectScreenToWorld(farP)

                local dir = ax.vec3sub(farP, nearP)
                local cameraPosition = self._camera:getPosition3D()
                self:shootBox(ax.vec3add(cameraPosition, ax.vec3mul(dir, 10)))
            end
        end, ax.Handler.EVENT_POINTER_UP)

        self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
    end

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 10
    local label = ax.Label:createWithTTF(ttfConfig, "DebugDraw OFF")
    local menuItem = ax.MenuItemLabel:create(label)
    menuItem:registerScriptTapHandler(function (tag, sender)
        if physics3DWorld:isDebugDrawEnabled() then
            physics3DWorld:setDebugDrawEnabled(false)
            label:setString("DebugDraw OFF")
        else
            physics3DWorld:setDebugDrawEnabled(true)
            label:setString("DebugDraw ON")
        end
    end)

    local menu = ax.Menu:create(menuItem)
    menu:setPosition(ax.p(0.0, 0.0))
    menuItem:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem:setPosition(ax.p(VisibleRect:left().x, VisibleRect:top().y - 50))
    self:addChild(menu)

    self._angle = 0.0
    self:extend()
end

function Physics3DTestDemo:onExit()
end

function Physics3DTestDemo:extend()
end

----------------------------------------
----BasicPhysics3DDemo
----------------------------------------
local BasicPhysics3DDemo = class("BasicPhysics3DDemo", Physics3DTestDemo)

function BasicPhysics3DDemo:subtitle()
    return "Basic Physics3D"
end

function BasicPhysics3DDemo:extend()
    local floor = ax.MeshRenderer:create("MeshRendererTest/box.c3t")
    floor:setTexture("MeshRendererTest/plane.png")
    floor:setScaleX(60)
    floor:setScaleZ(60)
    floor:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(floor)

    local floorBody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(60.0, 1.0, 60.0)), 0.0)
    floor:addComponent(floorBody)
    floorBody:syncNodeToPhysics()
    floorBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.NODE_TO_PHYSICS)

    local start_x = START_POS_X - ARRAY_SIZE_X / 2
    local start_y = START_POS_Y
    local start_z = START_POS_Z - ARRAY_SIZE_Z / 2

    for k = 1, ARRAY_SIZE_Y do
        for i = 1, ARRAY_SIZE_X do
            for j = 1, ARRAY_SIZE_Z do
                local x = 1.0 * (i - 1) + start_x
                local y = 5.0 + 1.0 * (k - 1) + start_y
                local z = 1.0 * (j - 1) + start_z

                local mesh = ax.MeshRenderer:create("MeshRendererTest/box.c3t")
                mesh:setTexture("Images/CyanSquare.png")
                mesh:setPosition3D(ax.vec3(x, y, z))
                mesh:setScale(0.8)
                mesh:setCameraMask(ax.CameraFlag.USER1)
                self:addChild(mesh)

                local rigidBody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(0.8, 0.8, 0.8)), 1.0)
                mesh:addComponent(rigidBody)
                rigidBody:syncNodeToPhysics()
                rigidBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.PHYSICS_TO_NODE)
            end
        end
    end

    self._physicsScene:setDebugCamera(self._camera)
end

----------------------------------------
----Joint3DDemo
----------------------------------------
local Joint3DDemo = class("Joint3DDemo", Physics3DTestDemo)

function Joint3DDemo:subtitle()
    return "Joint3D"
end

function Joint3DDemo:onPickBegan(event)
    if nil == self._camera or event == nil then
        return false
    end

    local location = event:getScreenLocation()
    local nearP = ax.vec3(location.x, location.y, 0.0)
    local farP = ax.vec3(location.x, location.y, 1.0)

    local size = ax.Director:getInstance():getCanvasSize()
    nearP = self._camera:deprojectScreenToWorld(nearP)
    farP = self._camera:deprojectScreenToWorld(farP)

    local physicsWorld = self._physicsScene:getPhysicsWorld3D()
    local ret, hitResult = physicsWorld:rayCast(nearP, farP, {})
    if ret and hitResult ~= nil and hitResult.hitActor ~= nil then
        self._constraint = ax.PivotJoint3D:create()
        self._constraint:setAnchorsInWorldSpace(hitResult.hitPosition)
        self._constraintOwner = hitResult.hitActor:getOwner()
        if self._constraintOwner ~= nil then
            self._constraintOwner:addComponent(self._constraint)
        end
        local diff = ax.vec3sub(hitResult.hitPosition, nearP)
        self._pickingDistance = math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
        return true
    end

    return false
end

function Joint3DDemo:extend()
    local listener = ax.PointerEventListener:create()
    listener:registerScriptHandler(function(event)
        if self:onPickBegan(event) then
            return
        end
        self._needShootBox = false
    end, ax.Handler.EVENT_POINTER_DOWN)

    listener:registerScriptHandler(function(event)
        if self._constraint ~= nil and event ~= nil then
            local location = event:getScreenLocation()
            local nearP = ax.vec3(location.x, location.y, 0.0)
            local farP = ax.vec3(location.x, location.y, 1.0)

            local size = ax.Director:getInstance():getCanvasSize()
            nearP = self._camera:deprojectScreenToWorld(nearP)
            farP = self._camera:deprojectScreenToWorld(farP)
            local dir = ax.vec3normalize(ax.vec3sub(farP, nearP))
            self._constraint:setConnectedAnchor(ax.vec3add(nearP, ax.vec3mul(dir, self._pickingDistance)))
            return
        end

        if event ~= nil and self._camera ~= nil then
            local delta = event:getDelta()
            self._angle = self._angle - delta.x * math.pi / 180.0
            self._camera:setPosition3D(ax.vec3(100.0 * math.sin(self._angle), 50.0, 100.0 * math.cos(self._angle)))
            self._camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))

            if (delta.x * delta.x + delta.y * delta.y) > 16 then
                self._needShootBox = false
            end
        end
    end, ax.Handler.EVENT_POINTER_MOVE)

    listener:registerScriptHandler(function(event)
        if self._constraint ~= nil then
            if self._constraintOwner ~= nil then
                self._constraintOwner:removeComponent(self._constraint)
            end
            self._constraint = nil
            self._constraintOwner = nil
            return
        end

        if self._needShootBox == false then
            return
        end

        if event ~= nil then
            local location = event:getScreenLocation()
            local nearP = ax.vec3(location.x, location.y, -1.0)
            local farP = ax.vec3(location.x, location.y, 1.0)
            nearP = self._camera:deprojectScreenToWorld(nearP)
            farP = self._camera:deprojectScreenToWorld(farP)

            local dir = ax.vec3sub(farP, nearP)
            local cameraPosition = self._camera:getPosition3D()
            self:shootBox(ax.vec3add(cameraPosition, ax.vec3mul(dir, 10)))
        end
    end, ax.Handler.EVENT_POINTER_UP)

    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
    self._physicsScene:setDebugCamera(self._camera)

    local mesh = ax.MeshRenderer:create("MeshRendererTest/orc.c3b")
    local quat = ax.quaternion_createFromAxisAngle(ax.vec3(0, 1, 0), math.pi)
    local rigidBody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(5.0, 5.0, 5.0)), 10.0)
    rigidBody:setTransformInPhysics(ax.vec3(0.0, -3.0, 0.0), quat)
    mesh:addComponent(rigidBody)
    mesh:setCameraMask(ax.CameraFlag.USER1)
    mesh:setScale(0.4)
    mesh:setPosition3D(ax.vec3(-30.0, 5.0, 0.0))
    self:addChild(mesh)
    rigidBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.PHYSICS_TO_NODE)

    local pivotJoint = ax.PivotJoint3D:create()
    pivotJoint:setAnchorsInWorldSpace(ax.vec3(-28.5, 10.5, -2.5))
    mesh:addComponent(pivotJoint)
    mesh:setName("pivot")

    local hingeMesh, _ = createMeshWithRigidbody(
        ax.vec3(8.0, 8.0, 1.0), 1.0, ax.vec3(0.0, 0.0, 0.0),
        "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "hinge")
    self:addChild(hingeMesh)

    local hingeJoint = ax.HingeJoint3D:createInWorldSpace(ax.vec3(4.0, 4.0, 0.5), ax.vec3(0.0, 1.0, 0.0))
    hingeJoint:setUseMotor(true)
    hingeJoint:setMotor({targetVelocity = -90.0, maxForce = 100.0})
    hingeMesh:addComponent(hingeJoint)

    local sliderSmall, sliderSmallBody = createMeshWithRigidbody(
        ax.vec3(3.0, 1.2, 3.0), 1.0, ax.vec3(30.0, 15.0, 0.0),
        "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "slide-small")
    self:addChild(sliderSmall)
    sliderSmallBody:setLinearVelocity(ax.vec3(0.0, 3.0, 0.0))

    local sliderBig, sliderBigBody = createMeshWithRigidbody(
        ax.vec3(3.0, 3.0, 3.0), 0.0, ax.vec3(30.0, 5.0, 0.0),
        "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "slide-big")
    self:addChild(sliderBig)

    local sliderJoint = ax.SliderJoint3D:create(sliderBigBody)
    sliderJoint:setAnchors(ax.vec3(0.0, -5.0, 0.0), ax.vec3(0.0, 5.0, 0.0))
    sliderJoint:setAxes(ax.vec3(0.0, 1.0, 0.0), ax.vec3(0.0, 1.0, 0.0))
    sliderJoint:setLimits({lower = -5.0, upper = 5.0})
    sliderSmall:addComponent(sliderJoint)

    local coneMesh, _ = createMeshWithRigidbody(
        ax.vec3(3.0, 3.0, 3.0), 1.0, ax.vec3(-10.0, 5.0, 0.0),
        "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "cone-twist")
    self:addChild(coneMesh)

    local twistJoint = ax.ConeTwistJoint3D:create()
    twistJoint:setAnchor(ax.vec3(0.0, -10.0, 0.0))
    twistJoint:setAxes(ax.vec3(0.0, 0.0, 1.0), ax.vec3(0.0, 0.0, 1.0))
    twistJoint:setLimits({swingSpan1 = 10.0, swingSpan2 = 10.0, twistSpan = 40.0})
    twistJoint:setSwingMotor({targetVelocity = 45.0, maxForce = 1000.0})
    twistJoint:setTwistMotor({targetVelocity = 45.0, maxForce = 1000.0})
    coneMesh:addComponent(twistJoint)

    local sixDofMesh, _ = createMeshWithRigidbody(
        ax.vec3(3.0, 3.0, 3.0), 1.0, ax.vec3(30.0, -5.0, 0.0),
        "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "six-dof")
    self:addChild(sixDofMesh)

    local sixDofJoint = ax.SixDofJoint3D:createInWorldSpace(ax.vec3(30.0, -5.0, 0.0), ax.vec3(1.0, 0.0, 0.0))
    sixDofJoint:setLinearLimits({lower = ax.vec3(-10.0, 0.0, 0.0), upper = ax.vec3(10.0, 0.0, 0.0)})
    sixDofJoint:setAngularLimits({lower = ax.vec3(0.0, 0.0, 0.0), upper = ax.vec3(0.0, 0.0, 0.0)})
    sixDofMesh:addComponent(sixDofJoint)
end

----------------------------------------
----Physics3DKinematicDemo
----------------------------------------
local Physics3DKinematicDemo = class("Physics3DKinematicDemo", Physics3DTestDemo)

function Physics3DKinematicDemo:subtitle()
    return "Physics3D Kinematic + #879"
end

function Physics3DKinematicDemo:extend()
    local floor = ax.MeshRenderer:create("MeshRendererTest/box.c3t")
    floor:setTexture("MeshRendererTest/plane.png")
    floor:setScaleX(60)
    floor:setScaleZ(60)
    floor:setPosition3D(ax.vec3(0.0, -1.0, 0.0))
    floor:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(floor)

    local floorBody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(60.0, 1.0, 60.0)), 0.0)
    floor:addComponent(floorBody)
    floorBody:syncNodeToPhysics()
    floorBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.NONE)

    local tree = "MeshRendererTest/tree-model/tree1.obj"
    local scale = 12.0
    local trianglesList = ax.Bundle3D:getTrianglesList(tree)
    for i = 1, #trianglesList do
        trianglesList[i] = ax.vec3mul(trianglesList[i], scale)
    end

    local treeMesh = ax.MeshRenderer:create(tree)
    treeMesh:setCameraMask(ax.CameraFlag.USER1)
    treeMesh:setPosition3D(ax.vec3(20.0, 0.0, 0.0))
    treeMesh:setScale(scale)
    treeMesh:getSubMeshMaterial(1, 0):enableTransparentDoubleSided()
    self:addChild(treeMesh)
    treeMesh:addComponent(ax.Rigidbody3D:create(ax.MeshCollider3D:create(trianglesList), 0.0))

    for i = 1, 3 do
        local mesh = ax.MeshRenderer:create("MeshRendererTest/box.c3t")
        mesh:setTexture("Images/CyanSquare.png")
        mesh:setCameraMask(ax.CameraFlag.USER1)
        mesh:setScale(2.0)
        mesh:setPosition3D(ax.vec3(-15.0, 0.0, 15.0 - 15.0 * (i - 1)))
        self:addChild(mesh)

        local rigidBody = ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(2.0, 2.0, 2.0)), 0.001)
        mesh:addComponent(rigidBody)
        rigidBody:setKinematic()

        local moveby = ax.MoveBy:create(2.0 + (i - 1), ax.vec3(30.0, 0.0, 0.0))
        mesh:runAction(ax.RepeatForever:create(ax.Sequence:create(moveby, moveby:reverse())))
    end

    local start_x = START_POS_X - ARRAY_SIZE_X / 2
    local start_y = START_POS_Y + 5.0
    local start_z = START_POS_Z - ARRAY_SIZE_Z / 2

    for k = 1, ARRAY_SIZE_Y do
        for i = 1, ARRAY_SIZE_X do
            for j = 1, ARRAY_SIZE_Z do
                local x = 1.0 * (i - 1) + start_x
                local y = 5.0 + 1.0 * (k - 1) + start_y
                local z = 1.0 * (j - 1) + start_z

                local mesh = ax.MeshRenderer:create("MeshRendererTest/sphere.c3b")
                mesh:setTexture("MeshRendererTest/plane.png")
                mesh:setCameraMask(ax.CameraFlag.USER1)
                mesh:setScale(1.0 / mesh:getContentSize().width)
                mesh:setPosition3D(ax.vec3(x, y, z))
                self:addChild(mesh)

                local rigidBody = ax.Rigidbody3D:create(ax.SphereCollider3D:create(0.5), 1.0)
                mesh:addComponent(rigidBody)
                rigidBody:syncNodeToPhysics()
                rigidBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.PHYSICS_TO_NODE)
            end
        end
    end

    self._physicsScene:setDebugCamera(self._camera)
end

----------------------------------------
----Physics3DCollisionCallbackDemo
----------------------------------------
local Physics3DCollisionCallbackDemo = class("Physics3DCollisionCallbackDemo", Physics3DTestDemo)

function Physics3DCollisionCallbackDemo:subtitle()
    return "Physics3D HitEvent c3b/obj"
end

function Physics3DCollisionCallbackDemo:extend()
    local physicsWorld = self._physicsScene:getPhysicsWorld3D()
    physicsWorld:setGlobalEventEnabled(ContactEventBits.Hit, true)

    for i = 1, #boss do
        local scale = 2.0
        local trianglesList = ax.Bundle3D:getTrianglesList(boss[i])
        for j = 1, #trianglesList do
            trianglesList[j] = ax.vec3mul(trianglesList[j], scale)
        end

        local rigidBody = ax.Rigidbody3D:create(ax.MeshCollider3D:create(trianglesList), 0.0)
        rigidBody:setEventEnabled(ContactEventBits.Hit, true)

        local mesh = ax.MeshRenderer:create(boss[i])
        mesh:addComponent(rigidBody)
        mesh:setRotation3D(ax.vec3(-90.0, 0.0, 0.0))
        mesh:setPosition3D(ax.vec3(-5.0 + 15.0 * (i - 1), -5.0, 0.0))
        mesh:setScale(scale)
        mesh:setCameraMask(ax.CameraFlag.USER1)
        self:addChild(mesh)
    end
    
    local onHit = function (event)
        local hitPosition = event:getContactInfo().points[1].pointB
        local ps = ax.PUParticleSystem3D:create("Particle3D/scripts/mp_hit_04.pu")
        ps:setPosition3D(hitPosition)
        ps:setScale(0.05)
        ps:startParticleSystem()
        ps:setCameraMask(2)
        self:addChild(ps)
        ps:runAction(ax.Sequence:create(ax.DelayTime:create(1.0), ax.CallFunc:create(function()
            ps:removeFromParent()
        end)))
    end
    
    local contactListener = ax.ContactEventListener3D:create()
    
    contactListener:registerScriptHandler(onHit, ax.Handler.EVENT_PHYSICS_3D_HIT)
    
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(contactListener, self)

    self._physicsScene:setDebugCamera(self._camera)
end

----------------------------------------
----Physics3DTerrainDemo
----------------------------------------
local Physics3DTerrainDemo = class("Physics3DTerrainDemo", Physics3DTestDemo)

function Physics3DTerrainDemo:subtitle()
    return "Physics3D Terrain"
end

function Physics3DTerrainDemo:extend()
    local detailMapR = {_detailMapSrc = "TerrainTest/dirt.jpg", _detailMapSize = 35}
    local detailMapG = {_detailMapSrc = "TerrainTest/Grass2.jpg", _detailMapSize = 10}
    local detailMapB = {_detailMapSrc = "TerrainTest/road.jpg", _detailMapSize = 35}
    local detailMapA = {_detailMapSrc = "TerrainTest/GreenSkin.jpg", _detailMapSize = 20}
    local terrainData =
    {
        _heightMapSrc = "TerrainTest/heightmap129.jpg",
        _alphaMapSrc = "TerrainTest/alphamap.png",
        _detailMaps = {detailMapR, detailMapG, detailMapB, detailMapA},
        _detailMapAmount = 4,
        _mapHeight = 20.0,
        _mapScale = 1.0,
        _chunkSize = ax.size(32, 32)
    }

    local terrain = ax.Terrain:create(terrainData, ax.Terrain.CrackFixedType.SKIRT)
    terrain:setMaxDetailMapAmount(4)
    terrain:setCameraMask(2)
    terrain:setDrawWire(false)
    terrain:setSkirtHeightRatio(3)
    terrain:setLODDistance(64, 128, 192)
    terrain:setCameraMask(ax.CameraFlag.USER1)
    self:addChild(terrain)

    local heidata  = terrain:getHeightData()
    local size     = terrain:getTerrainSize()
    local collider = ax.HeightFieldCollider3D:create(size.width, size.height, heidata, 1.0, terrain:getMinHeight(),
                                                  terrain:getMaxHeight(), true, false)
    terrain:addComponent(collider)
    collider:syncNodeToPhysics()
    collider:setSyncFlag(ax.Rigidbody3D.SyncFlag.NONE)

    local start_x = START_POS_X - ARRAY_SIZE_X / 2 + 5.0
    local start_y = START_POS_Y + 20.0
    local start_z = START_POS_Z - ARRAY_SIZE_Z / 2

    for k = 1, ARRAY_SIZE_Y do
        for i = 1, ARRAY_SIZE_X do
            for j = 1, ARRAY_SIZE_Z do
                local x = 1.0 * (i - 1) + start_x
                local y = 5.0 + 1.0 * (k - 1) + start_y
                local z = 1.0 * (j - 1) + start_z

                local mesh = ax.MeshRenderer:create("MeshRendererTest/sphere.c3b")
                mesh:setTexture("MeshRendererTest/plane.png")
                mesh:setCameraMask(ax.CameraFlag.USER1)
                mesh:setScale(1.0 / mesh:getContentSize().width)
                mesh:setPosition3D(ax.vec3(x, y, z))
                self:addChild(mesh)

                local rigidBody = ax.Rigidbody3D:create(ax.SphereCollider3D:create(0.5), 1.0)
                mesh:addComponent(rigidBody)
                rigidBody:syncNodeToPhysics()
                rigidBody:setSyncFlag(ax.Rigidbody3D.SyncFlag.PHYSICS_TO_NODE)
            end
        end
    end

    for i = 1, #boss do
        local trianglesList = ax.Bundle3D:getTrianglesList(boss[i])
        local mesh = ax.MeshRenderer:create(boss[i])
        mesh:setRotation3D(ax.vec3(-90.0, 0.0, 0.0))
        mesh:setPosition3D(ax.vec3(-5.0 + 15.0 * (i - 1), 15.0, 0.0))
        mesh:setCameraMask(2)
        self:addChild(mesh)

        collider = ax.MeshCollider3D:create(trianglesList)
        mesh:addComponent(collider)
        collider:syncNodeToPhysics()
        collider:setSyncFlag(ax.Rigidbody3D.SyncFlag.NONE)
    end

    local orc = ax.MeshRenderer:create("MeshRendererTest/orc.c3b")
    orc:setRotation3D(ax.vec3(0.0, 180.0, 0.0))
    orc:setPosition3D(ax.vec3(-5.0, 20.0, 0.0))
    orc:setScale(0.4)
    orc:setCameraMask(2)
    self:addChild(orc)
    orc:addComponent(ax.Rigidbody3D:create(ax.BoxCollider3D:create(ax.vec3(2.0, 4.0, 2.0)), 10.0))

    self._physicsScene:setDebugCamera(self._camera)
end

function Physics3DTest()
    Helper.usePhysics = true

    local scene = ax.Scene:createWithPhysics()

    Helper.createFunctionTable =
    {
        BasicPhysics3DDemo.create,
        Joint3DDemo.create,
        Physics3DKinematicDemo.create,
        Physics3DCollisionCallbackDemo.create,
        Physics3DTerrainDemo.create,
    }
    Helper.index = 1

    scene:addChild(BasicPhysics3DDemo.create())
    scene:addChild(CreateBackMenuItem())

    return scene
end
