require "axmol.3d.3dConstants"

----------------------------------------
----TerrainSimple
----------------------------------------
local TerrainSimple = class("TerrainSimple", function ()
    local layer = ax.Layer:create()

    return layer
end)

function TerrainSimple:ctor()
    -- body
    self:init()
end

function TerrainSimple:init()


    local visibleSize = ax.Director:getInstance():getVisibleSize()

    --use custom camera
    self._camera = ax.Camera:createPerspective(60, visibleSize.width/visibleSize.height, 0.1, 800)
    self._camera:setCameraFlag(ax.CameraFlag.USER1)
    self._camera:setPosition3D(ax.vec3(-1, 1.6, 4))
    self:addChild(self._camera)

    Helper.initWithLayer(self)
    Helper.titleLabel:setString(self:title())
    Helper.subtitleLabel:setString(self:subtitle())

    local detailMapR = { _detailMapSrc = "TerrainTest/dirt.jpg", _detailMapSize = 35}
    local detailMapG = { _detailMapSrc = "TerrainTest/Grass2.jpg", _detailMapSize = 35}
    local detailMapB = { _detailMapSrc = "TerrainTest/road.jpg", _detailMapSize = 35}
    local detailMapA = { _detailMapSrc = "TerrainTest/GreenSkin.jpg", _detailMapSize = 35}
    local terrainData = { _heightMapSrc = "TerrainTest/heightmap16.jpg", _alphaMapSrc = "TerrainTest/alphamap.png" , _detailMaps = {detailMapR, detailMapG, detailMapB, detailMapA}, _detailMapAmount = 4 }

    self._terrain = ax.Terrain:create(terrainData,ax.Terrain.CrackFixedType.SKIRT)
    self._terrain:setLODDistance(3.2, 6.4, 9.6)
    self._terrain:setMaxDetailMapAmount(4)
    self:addChild(self._terrain)
    self._terrain:setCameraMask(2)
    self._terrain:setDrawWire(false)

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(function (touches, event)
        local delta = ax.Director:getInstance():getDeltaTime()
        local touch = touches[1]
        local location = touch:getLocation()
        local previousLocation = touch:getPreviousLocation()
        local newPos = {x=previousLocation.x - location.x, y=previousLocation.y - location.y}

        local matTransform = self:getNodeToWorldTransform()

        local cameraDir = {x = -matTransform[9], y = -matTransform[10], z = -matTransform[11]}
        cameraDir = ax.vec3normalize(cameraDir)
        cameraDir.y = 0

        local cameraRightDir = {x = matTransform[1], y = matTransform[2], z = matTransform[3]}
        cameraRightDir = ax.vec3normalize(cameraRightDir)
        cameraRightDir.y = 0

        local cameraPos = self._camera:getPosition3D()
        cameraPos = { x = cameraPos.x + cameraDir.x * newPos.y * 0.5 * delta, y = cameraPos.y + cameraDir.y * newPos.y * 0.5 * delta, z = cameraPos.z + cameraDir.z * newPos.y * 0.5 * delta }
        cameraPos = { x = cameraPos.x + cameraRightDir.x * newPos.x * 0.5 * delta, y = cameraPos.y + cameraRightDir.y * newPos.x * 0.5 * delta, z = cameraPos.z + cameraRightDir.z * newPos.x * 0.5 * delta }
        self._camera:setPosition3D(cameraPos)

    end,ax.Handler.EVENT_TOUCHES_MOVED)

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)

    --add Particle3D for test blend
    local rootps = ax.PUParticleSystem3D:create("Particle3D/scripts/mp_torch.pu")
    rootps:setCameraMask(ax.CameraFlag.USER1)
    rootps:startParticleSystem()
    self:addChild(rootps, 0, 0)
end

function TerrainSimple:title()
    return "Terrain with skirt"
end

function TerrainSimple:subtitle()
    return "Drag to walkThru"
end

----------------------------------------
----TerrainWalkThru
----------------------------------------

local PLAER_STATE =
{
    LEFT     = 0,
    RIGHT    = 1,
    IDLE     = 2,
    FORWARD  = 3,
    BACKWARD = 4,
}

local PLAYER_HEIGHT = 0

local camera_offset = ax.vec3(0, 45, 60)

local Player = class("Player", function(file, cam, terrain)
    local sprite = ax.Sprite3D:create(file)
    if nil ~= sprite then
        sprite._headingAngle = 0
        sprite._playerState = PLAER_STATE.IDLE
        sprite._cam = cam
        sprite._terrain = terrain
    end
    return sprite
end)

function Player:ctor()
    -- body
    self:init()
end

function Player:init()
    self._headingAxis = ax.vec3(0.0, 0.0, 0.0)
    self:scheduleUpdateWithPriorityLua(function(dt)
        local curPos = self:getPosition3D()
        if self._playerState == PLAER_STATE.IDLE then

        elseif self._playerState == PLAER_STATE.FORWARD then
            local newFaceDir = ax.vec3sub(self._targetPos, curPos)
            newFaceDir.y = 0.0
            newFaceDir = ax.vec3normalize(newFaceDir)
            local offset = ax.vec3mul(newFaceDir, 25.0 * dt)
            curPos = ax.vec3add(curPos, offset)
            self:setPosition3D(curPos)
        elseif self._playerState ==  PLAER_STATE.BACKWARD then

            local transform   = self:getNodeToWorldTransform()
            local forward_vec = ax.vec3(-transform[9], -transform[10], -transform[11])
            forward_vec = ax.vec3normalize(forward_vec)
            self:setPosition3D(ax.vec3sub(curPos, ax.vec3mul(forward_vec, 15 * dt)))
        elseif self._playerState == PLAER_STATE.LEFT then
            player:setRotation3D(ax.vec3(curPos.x, curPos.y + 25 * dt, curPos.z))
        elseif self._playerState == PLAER_STATE.RIGHT then
            player:setRotation3D(ax.vec3(curPos.x, curPos.y - 25 * dt, curPos.z))
        end

        local normal = ax.vec3(0.0, 0.0, 0.0)
        local player_h, normal = self._terrain:getHeight(self:getPositionX(), self:getPositionZ(), normal)
        self:setPositionY(player_h + PLAYER_HEIGHT)

        --need to scriptfile
        local q2 = ax.quaternion_createFromAxisAngle(ax.vec3(0, 1, 0), -math.pi)
        local headingQ = ax.quaternion_createFromAxisAngle(self._headingAxis, self._headingAngle)
        local x = headingQ.w * q2.x + headingQ.x * q2.w + headingQ.y * q2.z - headingQ.z * q2.y
        local y = headingQ.w * q2.y - headingQ.x * q2.z + headingQ.y * q2.w + headingQ.z * q2.x
        local z = headingQ.w * q2.z + headingQ.x * q2.y - headingQ.y * q2.x + headingQ.z * q2.w
        local w = headingQ.w * q2.w - headingQ.x * q2.x - headingQ.y * q2.y - headingQ.z * q2.z
        headingQ = ax.quaternion(x, y, z, w)
        self:setRotationQuat(headingQ)

        local vec_offset = ax.vec4(camera_offset.x, camera_offset.y, camera_offset.z, 1)
        local transform  = self:getNodeToWorldTransform()
        vec_offset = mat4_transformVector(transform, vec_offset)
        local playerPos = self:getPosition3D()
        self._cam:setPosition3D(ax.vec3add(playerPos, camera_offset))
        self:updateState()
    end, 0)

    self:registerScriptHandler(function (event)
        -- body
        if "exit" == event then
            self:unscheduleUpdate()
        end
    end)
end

function Player:updateState()
    if self._playerState == PLAER_STATE.FORWARD then
        local player_pos = ax.p(self:getPositionX(),self:getPositionZ())
        local targetPos = ax.p(self._targetPos.x, self._targetPos.z)
        local dist = ax.pGetDistance(player_pos, targetPos)
        if dist < 1 then
            self._playerState = PLAER_STATE.IDLE
        end
    end
end

local TerrainWalkThru = class("TerrainWalkThru", function ()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    return layer
end)

function TerrainWalkThru:ctor()
    -- body
    self:init()
end

function TerrainWalkThru:init()
    Helper.titleLabel:setString(self:title())
    Helper.subtitleLabel:setString(self:subtitle())

    local listener = ax.EventListenerTouchAllAtOnce:create()

    listener:registerScriptHandler(function (touches, event)

    end,ax.Handler.EVENT_TOUCHES_BEGAN)

    listener:registerScriptHandler(function (touches, event)

        local touch = touches[1]
        local location = touch:getLocationInView()
        if self._camera ~= nil then
            if self._player ~= nil then
                local nearP = ax.vec3(location.x, location.y, 0.0)
                local farP  = ax.vec3(location.x, location.y, 1.0)

                local size = ax.Director:getInstance():getWinSize()
                nearP = self._camera:unproject(size, nearP, nearP)
                farP  = self._camera:unproject(size, farP, farP)
                local dir = ax.vec3sub(farP, nearP)
                dir = ax.vec3normalize(dir)

                local collisionPoint = ax.vec3(-999,-999,-999)
                local ray =  ax.Ray:new(nearP, dir)
                local isInTerrain = true
                isInTerrain, collisionPoint = self._terrain:getIntersectionPoint(ray, collisionPoint)

                if( not isInTerrain) then
                    self._player._playerState = PLAER_STATE.IDLE
                    return
                end

                local playerPos = self._player:getPosition3D()
                dir = ax.vec3sub(collisionPoint, playerPos)
                dir.y = 0
                dir = ax.vec3normalize(dir)
                self._player._headingAngle =  -1 * math.acos(-dir.z)

                self._player._headingAxis = vec3_cross(dir, ax.vec3(0, 0, -1), self._player._headingAxis)
                self._player._targetPos = collisionPoint
                -- self._player:forward()
                self._player._playerState = PLAER_STATE.FORWARD
            end
        end
    end,ax.Handler.EVENT_TOUCHES_ENDED)

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)

    local visibleSize = ax.Director:getInstance():getVisibleSize()
    self._camera = ax.Camera:createPerspective(60, visibleSize.width/visibleSize.height, 0.1, 200)
    self._camera:setCameraFlag(ax.CameraFlag.USER1)
    self:addChild(self._camera)

    local detailMapR = { _detailMapSrc = "TerrainTest/dirt.jpg", _detailMapSize = 35}
    local detailMapG = { _detailMapSrc = "TerrainTest/Grass2.jpg", _detailMapSize = 10}
    local detailMapB = { _detailMapSrc = "TerrainTest/road.jpg", _detailMapSize = 35}
    local detailMapA = { _detailMapSrc = "TerrainTest/GreenSkin.jpg", _detailMapSize = 20}
    local terrainData = { _heightMapSrc = "TerrainTest/heightmap16.jpg", _alphaMapSrc = "TerrainTest/alphamap.png" , _detailMaps = {detailMapR, detailMapG, detailMapB, detailMapA}, _detailMapAmount = 4, _mapHeight = 40.0, _mapScale = 2.0 }

    self._terrain = ax.Terrain:create(terrainData,ax.Terrain.CrackFixedType.SKIRT)
    self._terrain:setMaxDetailMapAmount(4)
    self._terrain:setCameraMask(2)
    self._terrain:setDrawWire(false)

    self._terrain:setSkirtHeightRatio(3)
    self._terrain:setLODDistance(64,128,192)

    self._player = Player:create("MeshRendererTest/girl.c3b", self._camera, self._terrain)
    self._player:setCameraMask(2)
    self._player:setScale(0.08)
    self._player:setPositionY(self._terrain:getHeight(self._player:getPositionX(), self._player:getPositionZ()) + PLAYER_HEIGHT)

    --add Particle3D for test blend
    local rootps = ax.PUParticleSystem3D:create("Particle3D/scripts/mp_torch.pu")
    rootps:setCameraMask(ax.CameraFlag.USER1)
    rootps:setScale(30.0)
    rootps:startParticleSystem()
    self._player:addChild(rootps)

    --add BillBoard for test blend
    local billboard = ax.BillBoard:create("Images/btn-play-normal.png")
    billboard:setPosition3D(ax.vec3(0,180,0))
    billboard:setCameraMask(ax.CameraFlag.USER1)
    self._player:addChild(billboard)

    local animation = ax.Animation3D:create("MeshRendererTest/girl.c3b","Take 001")
    if nil ~= animation then
        local animate = ax.Animate3D:create(animation)
        self._player:runAction(ax.RepeatForever:create(animate))
    end

    local playerPos = self._player:getPosition3D()
    self._camera:setPosition3D(ax.vec3add(playerPos, camera_offset))
    self._camera:setRotation3D(ax.vec3(-45,0,0))

    self:addChild(self._player)
    self:addChild(self._terrain)
end

function TerrainWalkThru:title()
    return "Player walk around in terrain"
end

function TerrainWalkThru:subtitle()
    return "touch to move"
end

function TerrainTest()
    local scene = ax.Scene:create()

    Helper.createFunctionTable =
    {
        TerrainSimple.create,
        TerrainWalkThru.create,
    }
    Helper.index = 1

    scene:addChild(TerrainSimple.create())
    scene:addChild(CreateBackMenuItem())

    return scene
end
