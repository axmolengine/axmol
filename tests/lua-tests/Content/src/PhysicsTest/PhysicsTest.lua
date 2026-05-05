local size = ax.Director:getInstance():getCanvasSize()
local MATERIAL_DEFAULT = ax.PhysicsMaterial2D(0.1, 0.5, 0.5)
local curLayer = nil
local STATIC_COLOR = ax.color(1.0, 0.0, 0.0, 1.0)
local DRAG_BODYS_TAG = 0x80

local function hasRigidbody2DTag(body, tag)
    return body ~= nil and bit.band(body:getTag(), tag) == tag
end

local function setRigidbody2DTag(node, tag)
    local body = node:getRigidbody2D()
    if body ~= nil then
        body:setTag(tag)
    end
    return body
end

local function getChildByRigidbody2DTag(parent, tag)
    for _, child in ipairs(parent:getChildren()) do
        if hasRigidbody2DTag(child:getRigidbody2D(), tag) then
            return child
        end
    end
    return nil
end

local function range(from, to, step)
    step = step or 1
    return function(_, lastvalue)
        local nextvalue = lastvalue + step
        if step > 0 and nextvalue <= to or step < 0 and nextvalue >= to or
        step == 0
        then
            return nextvalue
        end
    end, nil, from - step
end

local function initWithLayer(layer, callback)
    curLayer = layer
    layer.spriteTexture = ax.SpriteBatchNode:create("Images/grossini_dance_atlas.png", 100):getTexture()

    local debug = false
    local function toggleDebugCallback(sender)
        debug = not debug
        -- TODO:
    end

    layer.toggleDebug = function(self) toggleDebugCallback(nil) end
    ax.MenuItemFont:setFontSize(18)
    local item = ax.MenuItemFont:create("Toggle debug")
    item:registerScriptTapHandler(toggleDebugCallback)
    local menu = ax.Menu:create(item)
    layer:addChild(menu)
    menu:setPosition(size.width - 50, size.height - 10)
    Helper.initWithLayer(layer)

    local function onNodeEvent(event)
        if "enter" == event then
            callback()
        end
    end
    layer:registerScriptHandler(onNodeEvent)
end

local function addGrossiniAtPosition(layer, p, scale)
    scale = scale or 1.0

    local posx = math.random() * 200.0
    local posy = math.random() * 100.0
    posx = (math.floor(posx) % 4) * 85
    posy = (math.floor(posy) % 3) * 121

    local sp = ax.Sprite:createWithTexture(layer.spriteTexture, ax.rect(posx, posy, 85, 121))
    sp:setScale(scale)
    sp:addComponent(ax.Rigidbody2D:createBox(ax.size(48.0, 108.0)))
    layer:addChild(sp)
    sp:setPosition(p)
    return sp
end

local function onTouchBegan(touch, event)
    local location = touch:getLocation()
    local collider = ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():overlapPoint(location)
    if collider == nil then
        return false
    end

    local body = collider:getAttachedBody()
    if not hasRigidbody2DTag(body, DRAG_BODYS_TAG) then
        return false
    end

    local mouse = ax.Node:create()
    local mouseBody = ax.Rigidbody2D:create()
    mouse:addComponent(mouseBody)
    mouseBody:setKinematic()
    curLayer:addChild(mouse)
    local joint = ax.TargetJoint2D:create(location, body)
    joint:setMaxForceScale(100.0)
    mouse:addComponent(joint)
    mouse:setPosition(location)
    touch.mouse = mouse

    return true
end

local function onTouchMoved(touch, event)
    if touch.mouse then
        touch.mouse:setPosition(touch:getLocation())
    end
end

local function onTouchEnded(touch, event)
    if touch.mouse then
        curLayer:removeChild(touch.mouse)
        touch.mouse = nil
    end
end

local function makeBall(layer, point, radius, material)
    material = material or MATERIAL_DEFAULT

    local ball
    if layer.ball then
        ball = ax.Sprite:createWithTexture(layer.ball:getTexture())
    else
        ball = ax.Sprite:create("Images/ball.png")
    end

    ball:setScale(0.13 * radius)

    local body = ax.Rigidbody2D:createCircle(ball:getContentSize().width / 2, material)
    ball:addComponent(body)
    ball:setPosition(point)

    return ball
end

local function makeBox(point, size, color, material)
    material = material or MATERIAL_DEFAULT

    local yellow = false
    if color == 0 then
        yellow = math.random() > 0.5
    else
        yellow = color == 1
    end

    local box = yellow and ax.Sprite:create("Images/YellowSquare.png") or ax.Sprite:create("Images/CyanSquare.png")

    box:setScaleX(size.width/100.0)
    box:setScaleY(size.height/100.0)

    local body = ax.Rigidbody2D:createBox(box:getContentSize(), material)
    box:addComponent(body)
    box:setPosition(ax.p(point.x, point.y))

    return box
end

local function makeTriangle(point, size, color, material)
    material = material or MATERIAL_DEFAULT

    local yellow = false
    if color == 0 then
        yellow = math.random() > 0.5
    else
        yellow = color == 1
    end
    local triangle = yellow and ax.Sprite:create("Images/YellowTriangle.png") or ax.Sprite:create("Images/CyanTriangle.png")

    if size.height == 0 then
        triangle:setScale(size.width/100.0)
    else
        triangle:setScaleX(size.width/50.0)
        triangle:setScaleY(size.height/43.5)
    end

    vers = { ax.p(0, triangle:getContentSize().height/2),
        ax.p(triangle:getContentSize().width/2, -triangle:getContentSize().height/2),
        ax.p(-triangle:getContentSize().width/2, -triangle:getContentSize().height/2)
    }

    local body = ax.Rigidbody2D:createPolygon(vers, material)
    triangle:addComponent(body)
    triangle:setPosition(point)

    return triangle
end

local function PhysicsDemoClickAdd()
    local layer = ax.Layer:create()
    local function onEnter()
        local function onTouchEnded(touch, event)
            local location = touch:getLocation()
            addGrossiniAtPosition(layer, location)
        end

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(function() return true end, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        addGrossiniAtPosition(layer, VisibleRect:center())

        local node = ax.Node:create()
        node:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height
                )
            )
        )
        node:setPosition(VisibleRect:center())
        layer:addChild(node)
    end
    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Grossini")
    Helper.subtitleLabel:setString("multi touch to add grossini")

    return layer
end

local function PhysicsDemoLogoSmash()
    local layer = ax.Layer:create()

    local function onEnter()
        local logo_width = 188.0
        local logo_height = 35.0
        local logo_raw_length = 24.0
        local logo_image = {
            15,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,-64,15,63,-32,-2,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,31,-64,15,127,-125,-1,-128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,127,-64,15,127,15,-1,-64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,-64,15,-2,
            31,-1,-64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,-64,0,-4,63,-1,-32,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,1,-1,-64,15,-8,127,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,-1,-64,0,-8,-15,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-31,-1,-64,15,-8,-32,
            -1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,-15,-1,-64,9,-15,-32,-1,-32,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,31,-15,-1,-64,0,-15,-32,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,63,-7,-1,-64,9,-29,-32,127,-61,-16,63,15,-61,-1,-8,31,-16,15,-8,126,7,-31,
            -8,31,-65,-7,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-2,63,-8,31,-4,-1,15,-13,
            -4,63,-1,-3,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-2,63,-8,31,-4,-1,15,-13,
            -2,63,-1,-3,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
            -2,63,-33,-1,-1,-32,9,-25,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
            -1,63,-33,-1,-1,-16,9,-25,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
            -1,63,-49,-1,-1,-8,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
            -1,-65,-49,-1,-1,-4,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
            -1,-65,-57,-1,-1,-2,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
            -1,-1,-57,-1,-1,-1,9,-57,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
            -1,-61,-1,-1,-1,-119,-57,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
            -1,-61,-1,-1,-1,-55,-49,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
            -1,-63,-1,-1,-1,-23,-49,-32,127,-57,-1,-1,-97,-25,-1,-1,63,-1,-1,-4,-1,15,-13,
            -1,-1,-63,-1,-1,-1,-16,-49,-32,-1,-25,-1,-1,-97,-25,-1,-1,63,-33,-5,-4,-1,15,
            -13,-1,-1,-64,-1,-9,-1,-7,-49,-32,-1,-25,-8,127,-97,-25,-1,-1,63,-33,-5,-4,-1,
            15,-13,-1,-1,-64,-1,-13,-1,-32,-49,-32,-1,-25,-8,127,-97,-25,-1,-2,63,-49,-13,
            -4,-1,15,-13,-1,-1,-64,127,-7,-1,-119,-17,-15,-1,-25,-8,127,-97,-25,-1,-2,63,
            -49,-13,-4,-1,15,-13,-3,-1,-64,127,-8,-2,15,-17,-1,-1,-25,-8,127,-97,-25,-1,
            -8,63,-49,-13,-4,-1,15,-13,-3,-1,-64,63,-4,120,0,-17,-1,-1,-25,-8,127,-97,-25,
            -8,0,63,-57,-29,-4,-1,15,-13,-4,-1,-64,63,-4,0,15,-17,-1,-1,-25,-8,127,-97,
            -25,-8,0,63,-57,-29,-4,-1,-1,-13,-4,-1,-64,31,-2,0,0,103,-1,-1,-57,-8,127,-97,
            -25,-8,0,63,-57,-29,-4,-1,-1,-13,-4,127,-64,31,-2,0,15,103,-1,-1,-57,-8,127,
            -97,-25,-8,0,63,-61,-61,-4,127,-1,-29,-4,127,-64,15,-8,0,0,55,-1,-1,-121,-8,
            127,-97,-25,-8,0,63,-61,-61,-4,127,-1,-29,-4,63,-64,15,-32,0,0,23,-1,-2,3,-16,
            63,15,-61,-16,0,31,-127,-127,-8,31,-1,-127,-8,31,-128,7,-128,0,0
        }

        local function get_pixel(x, y)
            return bit.band(bit.rshift(logo_image[bit.rshift(x, 3) + y*logo_raw_length + 1], bit.band(bit.bnot(x), 0x07)), 1)
        end

        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, 0))
        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setUpdateRate(1)

        layer.ball = ax.SpriteBatchNode:create("Images/ball.png", #logo_image)
        layer:addChild(layer.ball)
        for y in range(0, logo_height-1) do
            for x in range(0, logo_width-1) do
                if get_pixel(x, y) == 1 then
                    local x_jitter = 0.05*math.random()
                    local y_jitter = 0.05*math.random()

                    local ball = makeBall(layer,
                        ax.p(2*(x - logo_width/2 + x_jitter) + VisibleRect:getVisibleRect().width/2,
                            2*(logo_height-y + y_jitter) + VisibleRect:getVisibleRect().height/2 - logo_height/2),
                        0.95,
                        ax.PhysicsMaterial2D(0.01, 0.0, 0.0))
                    local Rigidbody2D = ball:getRigidbody2D()
                    layer.ball:addChild(ball)
                end
            end
        end

        local bullet = makeBall(layer, ax.p(400, 0), 10, ax.PhysicsMaterial2D(PHYSICS_INFINITY_2D, 0, 0))

        bullet:getRigidbody2D():setVelocity(ax.p(200, 0))
        bullet:setPosition(ax.p(5, VisibleRect:getVisibleRect().height/2))
        layer.ball:addChild(bullet)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Logo Smash")

    return layer
end

local function PhysicsDemoJoints()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local width = (VisibleRect:getVisibleRect().width - 10) / 4
        local height = (VisibleRect:getVisibleRect().height - 50) / 4

        local node = ax.Node:create()
        local worldBox = ax.Rigidbody2D:create()
        node:addComponent(worldBox)
        worldBox:setKinematic()
        node:setPosition(ax.p(0, 0))
        layer:addChild(node)

        local scene = ax.Director:getInstance():getRunningScene()
        for i in range(0, 3) do
            for j in range(0, 3) do
                local offset = ax.p(VisibleRect:leftBottom().x + 5 + j * width + width/2,
                    VisibleRect:leftBottom().y + 50 + i * height + height/2)
                worldBox:addCollider(ax.EdgeBoxCollider2D:create(ax.size(width, height),
                        ax.PHYSICS_MATERIAL_2D_DEFAULT,
                        offset)
                )
                local index = i*4 + j

                if index == 0 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBall(layer, ax.p(offset.x + 30, offset.y), 10)
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.PivotJoint2D:create(sp2Rigidbody2D)
                    local anchor1 = sp1Rigidbody2D:world2Local(offset)
                    local anchor2 = sp2Rigidbody2D:world2Local(offset)
                    joint:setAnchors(anchor1, anchor2)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif  index == 1 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.FixedJoint2D:create(sp2Rigidbody2D)
                    local anchor1 = sp1Rigidbody2D:world2Local(offset)
                    local anchor2 = sp2Rigidbody2D:world2Local(offset)
                    joint:setAnchors(anchor1, anchor2)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 2 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.DistanceJoint2D:create(sp2Rigidbody2D)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 3 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.DistanceJoint2D:create(sp2Rigidbody2D)
                    joint:setLimits(ax.vec2(30.0, 52.0))
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 4 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.SpringJoint2D:create(500.0, 0.3, sp2Rigidbody2D)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 5 then
                    local sp1 = makeBall(layer, ax.p(offset.x - 30, offset.y), 10)
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.SliderJoint2D:create(sp2Rigidbody2D)
                    local sliderOrigin = ax.vec2(30.0, 15.0)
                    local sliderTarget = ax.vec2(30.0, -15.0)
                    local connectedAnchor = ax.vec2(-30.0, 0.0)
                    joint:setAnchor(sliderOrigin);
                    joint:setConnectedAnchor(connectedAnchor);
                    joint:setLimits(ax.vec2(0.0, ax.pGetDistance(sliderOrigin, sliderTarget)))
                    local radians = ax.pToAngleSelf(ax.pSub(sliderTarget, sliderOrigin))
                    joint:setAngleInRadians(radians)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 6 then
                    local sp1 = makeBox(ax.p(offset.x - 30, offset.y), ax.size(30, 10))
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.SpringJoint2D:create(3000.0, 60.0, sp2Rigidbody2D)
                    joint:setRestLength(60)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 7 then
                    local sp1 = makeBox(ax.p(offset.x - 30, offset.y), ax.size(30, 10))
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.WheelJoint2D:create(sp2Rigidbody2D)
                    joint:setMotor(ax.vec2(60.0, 100000))
                    joint:setLimits(ax.vec2(-1.0, 1.0))
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 8 then
                    local sp1 = makeBox(ax.p(offset.x - 30, offset.y), ax.size(30, 10))
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local joint = ax.FilterJoint2D:create(sp2Rigidbody2D)
                    sp1:addComponent(joint);

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 9 then
                    local sp1 = makeBox(ax.p(offset.x - 30, offset.y), ax.size(30, 10))
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    sp1:addComponent(ax.PinJoint2D:create(ax.vec2(sp1:getPosition()), worldBox));
                    sp2:addComponent(ax.PinJoint2D:create(ax.vec2(sp2:getPosition()), worldBox));

                    local joint = ax.MotorJoint2D:create(sp2Rigidbody2D);
                    joint:setAngularVelocity(0.0)
                    sp1:addComponent(joint)

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                elseif index == 10 then
                    local sp1 = makeBox(ax.p(offset.x - 30, offset.y), ax.size(30, 10))
                    local sp1Rigidbody2D = sp1:getRigidbody2D()
                    sp1Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    local sp2 = makeBox(ax.p(offset.x + 30, offset.y), ax.size(30, 10))
                    local sp2Rigidbody2D = sp2:getRigidbody2D()
                    sp2Rigidbody2D:setTag(DRAG_BODYS_TAG)

                    sp1:addComponent(ax.PinJoint2D:create(ax.vec2(sp1:getPosition()), worldBox))
                    sp2:addComponent(ax.PinJoint2D:create(ax.vec2(sp2:getPosition()), worldBox))
                    local joint = ax.MotorJoint2D:create(sp2Rigidbody2D)
                    sp1:addComponent(joint);

                    layer:addChild(sp1)
                    layer:addChild(sp2)
                end
            end
        end
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Joints")
    return layer
end

local function PhysicsDemoPyramidStack()
    local layer = ax.Layer:create()

    local function onEnter()
        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local node = ax.Node:create()
        node:addComponent(ax.Rigidbody2D:createEdgeSegment(ax.p(VisibleRect:leftBottom().x,
                    VisibleRect:leftBottom().y + 50),
                ax.p(VisibleRect:rightBottom().x,
                    VisibleRect:rightBottom().y + 50)))
        layer:addChild(node)

        local ball = ax.Sprite:create("Images/ball.png")
        ball:setScale(1)
        ball:addComponent(ax.Rigidbody2D:createCircle(10))
        setRigidbody2DTag(ball, DRAG_BODYS_TAG)
        ball:setPosition(ax.p(VisibleRect:bottom().x, VisibleRect:bottom().y + 60))
        layer:addChild(ball)

        ball:runAction(ax.Sequence:create(ax.DelayTime:create(3),
                ax.ScaleTo:create(0, 3)))

        for i in range(0, 13) do
            for j in range(0, i) do
                local x = VisibleRect:bottom().x + (i/2 - j) * 11
                local y = VisibleRect:bottom().y + (14 - i) * 23 + 100
                local sp = addGrossiniAtPosition(layer, ax.p(x, y), 0.2)
                setRigidbody2DTag(sp, DRAG_BODYS_TAG)
            end
        end
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Pyramid Stack")

    return layer
end

local function PhysicsDemoRayCast()
    local layer = ax.Layer:create()

    local function onEnter()
        local function onTouchEnded(touch, event)
            local location = touch:getLocation()

            local r = math.random(3)
            if r ==1 then
                layer:addChild(makeBall(layer, location, 5 + math.random()*10))
            elseif r == 2 then
                layer:addChild(makeBox(location, ax.size(10 + math.random()*15, 10 + math.random()*15)))
            elseif r == 3 then
                layer:addChild(makeTriangle(location, ax.size(10 + math.random()*20, 10 + math.random()*20)))
            end
        end

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(function() return true end, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0,0))

        local node = ax.DrawNode:create()
        node:addComponent(ax.Rigidbody2D:createEdgeSegment(ax.p(VisibleRect:leftBottom().x,
                    VisibleRect:leftBottom().y + 50),
                ax.p(VisibleRect:rightBottom().x,
                    VisibleRect:rightBottom().y + 50)))
        node:drawSegment(ax.p(VisibleRect:leftBottom().x, VisibleRect:leftBottom().y + 50),
            ax.p(VisibleRect:rightBottom().x, VisibleRect:rightBottom().y + 50),
            1,
            STATIC_COLOR)
        layer:addChild(node)

        local mode = 0
        ax.MenuItemFont:setFontSize(18)
        local item = ax.MenuItemFont:create("Toggle debugChange Mode(any)")
        local function changeModeCallback(sender)
            mode = (mode + 1) % 3

            if mode == 0 then
                item:setString("Change Mode(any)")
            elseif mode == 1 then
                item:setString("Change Mode(nearest)")
            elseif mode == 2 then
                item:setString("Change Mode(multiple)")
            end
        end

        item:registerScriptTapHandler(changeModeCallback)

        local menu = ax.Menu:create(item)
        layer:addChild(menu)
        menu:setPosition(ax.p(VisibleRect:left().x+100, VisibleRect:top().y-10))

        local angle = 0
        local drawNode = nil
        local function update(delta)
            local L = 150.0
            local point1 = VisibleRect:center()
            local d = ax.p(L * math.cos(angle), L * math.sin(angle))
            local point2 = ax.p(point1.x + d.x, point1.y + d.y)

            if drawNode then layer:removeChild(drawNode) end
            drawNode = ax.DrawNode:create()
            if mode == 0 then
                local point3 = ax.p(point2.x, point2.y)
                local function func(world, info)
                    point3 = info.point
                    return false
                end

                ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():rayCast(func, point1, point2)
                drawNode:drawSegment(point1, point3, 1.0, STATIC_COLOR)

                if point2.x ~= point3.x or point2.y ~= point3.y then
                    drawNode:drawDot(point3, 2, ax.color(1.0, 1.0, 1.0, 1.0))
                end
                layer:addChild(drawNode)
            elseif mode == 1 then
                local point3 = ax.p(point2.x, point2.y)
                local friction = 1.0
                local function func(world, info)
                    if friction > info.fraction then
                        point3 = info.point
                        friction = info.fraction
                    end
                    return true
                end

                ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():rayCast(func, point1, point2)
                drawNode:drawSegment(point1, point3, 1.0, STATIC_COLOR)

                if point2.x ~= point3.x or point2.y ~= point3.y then
                    drawNode:drawDot(point3, 2, ax.color(1.0, 1.0, 1.0, 1.0))
                end
                layer:addChild(drawNode)
            elseif mode == 2 then
                local points = {}

                local function func(world, info)
                    points[#points + 1] = info.point
                    return true
                end

                ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():rayCast(func, point1, point2)
                drawNode:drawSegment(point1, point2, 1.0, STATIC_COLOR)

                for _, p in ipairs(points) do
                    drawNode:drawDot(p, 2, ax.color(1.0, 1.0, 1.0, 1.0))
                end

                layer:addChild(drawNode)
            end

            angle = angle + 0.25 * math.pi / 180.0

        end

        layer:scheduleUpdateWithPriorityLua(update, 0)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Ray Cast")

    return layer
end

local function PhysicsDemoOneWayPlatform()
    local layer = ax.Layer:create()
    local function onEnter()

        local physicsWorld2D = ax.Director:getInstance():getRunningScene():getPhysicsWorld2D()
        physicsWorld2D:setGlobalEventEnabled(ax.physics2d.CollisionEventBits.PreSolve, true)

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local ground = ax.Node:create()
        ground:addComponent(ax.Rigidbody2D:createEdgeSegment(ax.p(VisibleRect:leftBottom().x,
                    VisibleRect:leftBottom().y + 50),
                ax.p(VisibleRect:rightBottom().x,
                    VisibleRect:rightBottom().y + 50)))
        layer:addChild(ground)

        local platform = makeBox(VisibleRect:center(), ax.size(200, 50))
        local platformRigidbody2D = platform:getRigidbody2D()
        platformRigidbody2D:setKinematic()
        layer:addChild(platform)

        local ball = makeBall(layer, ax.p(VisibleRect:center().x, VisibleRect:center().y - 50), 20)
        local ballRigidbody2D = ball:getRigidbody2D()
        ballRigidbody2D:setVelocity(ax.p(0, 150))
        ballRigidbody2D:setTag(DRAG_BODYS_TAG)
        layer:addChild(ball)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("One Way Platform")

    return layer
end

local function PhysicsDemoActions()
    local layer = ax.Layer:create()
    local function onEnter()
        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local node = ax.Node:create()
        node:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height)))
        node:setPosition(VisibleRect:center())
        layer:addChild(node)

        local sp1 = addGrossiniAtPosition(layer, VisibleRect:center())
        local sp2 = addGrossiniAtPosition(layer, ax.p(VisibleRect:left().x + 50, VisibleRect:left().y))
        local sp3 = addGrossiniAtPosition(layer, ax.p(VisibleRect:right().x - 20, VisibleRect:right().y))
        local sp4 = addGrossiniAtPosition(layer, ax.p(VisibleRect:leftTop().x + 50, VisibleRect:leftTop().y-50))
        sp4:getRigidbody2D():setGravityEnabled(false)


        local actionTo = ax.JumpTo:create(2, ax.p(100,100), 50, 4)
        local actionBy = ax.JumpBy:create(2, ax.p(300,0), 50, 4)
        local actionUp = ax.JumpBy:create(2, ax.p(0,50), 80, 4)
        local actionByBack = actionBy:reverse()
        local rotateBy = ax.RotateBy:create(2, 180)
        local rotateByBack = ax.RotateBy:create(2, -180)

        sp1:runAction(ax.RepeatForever:create(actionUp))
        sp2:runAction(ax.RepeatForever:create(ax.Sequence:create(actionBy, actionByBack)))
        sp3:runAction(actionTo)
        sp4:runAction(ax.RepeatForever:create(ax.Sequence:create(rotateBy, rotateByBack)))
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Actions")

    return layer
end

local function PhysicsDemoPump()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()

        local distance = 0.0
        local rotationV = 0.0
        local function onTouchBeganEx(touch, event)
            onTouchBegan(touch, event)
            distance = touch:getLocation().x - VisibleRect:center().x
            return true
        end

        local function onTouchMovedEx(touch, event)
            onTouchMoved(touch, event)
            distance = touch:getLocation().x - VisibleRect:center().x
        end

        local function onTouchEndedEx(touch, event)
            onTouchEnded(touch, event)
            distance = 0
        end

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBeganEx, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMovedEx, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEndedEx, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local function update()
            for i, child in ipairs(layer:getChildren()) do
                local body = child:getRigidbody2D()
                local x, y = child:getPosition()
                if hasRigidbody2DTag(body, DRAG_BODYS_TAG) and y < 0.0 then
                    child:setPosition(ax.p(VisibleRect:leftTop().x + 75,
                            VisibleRect:leftTop().y + math.random() * 90))
                    body:setVelocity(ax.p(0, 0))
                end
            end

            local gear = layer:getChildByTag(1)
            local gearBody = gear:getRigidbody2D()
            if gearBody then
                if distance ~= 0.0 then
                    rotationV = rotationV + distance/2500.0
                end
                if rotationV > 30 then rotationV = 30.0 end
                if rotationV < -30 then rotationV = -30.0 end

                gearBody:setAngularVelocity(rotationV)
                rotationV = rotationV * 0.995
            end
        end

        layer:scheduleUpdateWithPriorityLua(update, 0)

        local worldBox = ax.Node:create()
        local worldBoxBody = ax.Rigidbody2D:create()
        worldBoxBody:setKinematic()

        local staticMaterial = ax.PhysicsMaterial2D(ax.PHYSICS_INFINITY_2D, 0, 0.5)
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 50,
                    VisibleRect:leftTop().y),
                ax.p(VisibleRect:leftTop().x + 50,
                    VisibleRect:leftTop().y-130),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 190,
                    VisibleRect:leftTop().y),
                ax.p(VisibleRect:leftTop().x + 100,
                    VisibleRect:leftTop().y-50),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 100,
                    VisibleRect:leftTop().y-50),
                ax.p(VisibleRect:leftTop().x + 100,
                    VisibleRect:leftTop().y-90),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 50,
                    VisibleRect:leftTop().y-130),
                ax.p(VisibleRect:leftTop().x + 100,
                    VisibleRect:leftTop().y-145),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 100,
                    VisibleRect:leftTop().y-145),
                ax.p(VisibleRect:leftBottom().x + 100,
                    VisibleRect:leftBottom().y + 80),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 150,
                    VisibleRect:leftTop().y-80),
                ax.p(VisibleRect:leftBottom().x + 150,
                    VisibleRect:leftBottom().y + 80),
                staticMaterial))
        worldBoxBody:addCollider(ax.EdgeSegmentCollider2D:create(ax.p(VisibleRect:leftTop().x + 150,
                    VisibleRect:leftTop().y-80),
                ax.p(VisibleRect:rightTop().x -100,
                    VisibleRect:rightTop().y-150),
                staticMaterial))

        worldBoxBody:setCategoryBits(1)

        -- balls
        for _ in range(1, 6) do
            local ball = makeBall(layer,
                ax.p(VisibleRect:leftTop().x + 75 + math.random() * 90,
                    VisibleRect:leftTop().y),
                22,
                ax.PhysicsMaterial2D(0.05, 0.0, 0.1))
            setRigidbody2DTag(ball, DRAG_BODYS_TAG)
            layer:addChild(ball)
        end

        worldBox:setName('world-box')
        worldBox:addComponent(worldBoxBody)
        layer:addChild(worldBox)

        local vec =
        {
            ax.p(VisibleRect:leftTop().x + 102, VisibleRect:leftTop().y-148),
            ax.p(VisibleRect:leftTop().x + 148, VisibleRect:leftTop().y-161),
            ax.p(VisibleRect:leftBottom().x + 148, VisibleRect:leftBottom().y + 20),
            ax.p(VisibleRect:leftBottom().x + 102, VisibleRect:leftBottom().y + 20)
        }

        local world = ax.Director:getInstance():getRunningScene():getPhysicsWorld2D()

        -- small gear
        local sgear = ax.Node:create()
        local sgearB = ax.Rigidbody2D:createCircle(44)
        sgear:addComponent(sgearB)
        sgear:setPosition(ax.p(VisibleRect:leftBottom().x + 125, VisibleRect:leftBottom().y))
        layer:addChild(sgear)
        sgearB:setCategoryBits(4)
        sgearB:setCollisionMaskBits(4)
        sgear:setTag(1)
        sgear:setName('small-gear')
        -- pin small gear to body
        sgear:addComponent(ax.PinJoint2D:create(ax.vec2(sgear:getPosition()), worldBoxBody))


        -- big gear
        local bgear = ax.Node:create()
        bgear:setTag(2)
        bgear:setName('big-gear')
        local bgearB = ax.Rigidbody2D:createCircle(100)
        bgear:addComponent(bgearB)
        bgear:setPosition(ax.p(VisibleRect:leftBottom().x + 275, VisibleRect:leftBottom().y))
        layer:addChild(bgear)
        bgearB:setCategoryBits(4)
        bgear:addComponent(ax.PinJoint2D:create(ax.p(bgear:getPosition()), worldBoxBody))
        local motorJoint = ax.MotorJoint2D:create(bgearB)
        motorJoint:setAngularVelocity(-90)
        sgear:addComponent(motorJoint)


        -- pump
        local pump = ax.Node:create()
        local center = ax.Collider2D:getPolygonCenter(vec)
        pump:setPosition(center)
        local pumpB = ax.Rigidbody2D:createPolygon(vec,
            ax.PHYSICS_MATERIAL_2D_DEFAULT,
            ax.p(-center.x, -center.y))
        pump:addComponent(pumpB)
        layer:addChild(pump)
        pumpB:setCategoryBits(2)
        pumpB:setGravityEnabled(false)
        local distanceJoint = ax.DistanceJoint2D:create(sgearB)
        distanceJoint:setConnectedAnchor(ax.p(0, -44))
        pump:addComponent(distanceJoint)
        pump:setName('pump')

        -- plugger
        local seg = {ax.p(VisibleRect:leftTop().x + 75, VisibleRect:leftTop().y-120),
            ax.p(VisibleRect:leftBottom().x + 75, VisibleRect:leftBottom().y-100)}
        local segCenter = ax.p((seg[2].x + seg[1].x)/2, (seg[2].y + seg[1].y)/2)
        seg[2] = ax.p(seg[2].x - segCenter.x, seg[2].y - segCenter.y)
        seg[1] = ax.p(seg[1].x - segCenter.x, seg[1].y - segCenter.y)
        local plugger = ax.Node:create()
        local pluggerB = ax.Rigidbody2D:createEdgeSegment(seg[1],
            seg[2],
            ax.PhysicsMaterial2D(0.01, 0.0, 0.5))
        pluggerB:setDynamic()
        plugger:addComponent(pluggerB)
        plugger:setPosition(segCenter)
        layer:addChild(plugger)
        pluggerB:setCategoryBits(2)
        sgearB:setCollisionMaskBits(5)

        local pinJoint = ax.PinJoint2D:create(ax.p(plugger:getPosition()), worldBoxBody)
        pinJoint:setMotor(ax.vec2(0, 100000))
        plugger:addComponent(pinJoint)

        distanceJoint = ax.DistanceJoint2D:create(sgearB)
        distanceJoint:setConnectedAnchor(ax.vec2(44.0, 0))
        plugger:addComponent(distanceJoint)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Pump")
    Helper.subtitleLabel:setString("touch screen on left or right")

    return layer
end

local function PhysicsDemoSlice()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()
        local sliceTag = 1

        local function clipPoly(shape, normal, distance)
            local body = shape:getBody()
            local count = shape:getPointsCount()
            local points = {}

            local j = count - 1
            for i in range(0, count-1) do
                local a = body:local2World(shape:getPoint(j))
                local aDist = ax.pDot(a, normal) - distance

                if aDist < 0.0 then
                    points[#points + 1] = a
                end

                local b = body:local2World(shape:getPoint(i))
                local bDist = ax.pDot(b, normal) - distance

                if aDist*bDist < 0.0 then
                    local t = math.abs(aDist)/(math.abs(aDist) + math.abs(bDist))
                    points[#points + 1] = ax.pLerp(a, b, t)
                end
                j = i
            end

            local center = ax.Collider2D:getPolygonCenter(points)
            local node = ax.Node:create()
            local polygon = ax.Rigidbody2D:createPolygon(points,
                ax.PHYSICS_MATERIAL_2D_DEFAULT,
                ax.p(-center.x, -center.y))
            node:setPosition(center)
            node:addComponent(polygon)
            polygon:setVelocity(body:getVelocityAtWorldPoint(center))
            polygon:setAngularVelocity(body:getAngularVelocity())
            polygon.tag = sliceTag
            layer:addChild(node)
        end


        local function slice(world, info)
            if info.shape:getBody().tag ~= sliceTag then
                return true
            end

            if not info.shape:containsPoint(info.start) and not info.shape:containsPoint(info.ended) then
                local normal = ax.p(info.ended.x - info.start.x, info.ended.y - info.start.y)
                normal = ax.pNormalize(ax.pPerp(normal))
                local dist = ax.pDot(info.start, normal)

                clipPoly(info.shape, normal, dist)
                clipPoly(info.shape, ax.p(-normal.x, -normal.y), -dist)

                info.shape:getBody():removeFromWorld()
            end
            return true
        end

        local function onTouchEnded(touch, event)
            ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():rayCast(slice,
                touch:getStartLocation(),
                touch:getLocation())
        end

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(function() return true end, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local ground = ax.Node:create()
        ground:addComponent(ax.Rigidbody2D:createEdgeSegment(ax.p(VisibleRect:leftBottom().x,
                    VisibleRect:leftBottom().y + 50),
                ax.p(VisibleRect:rightBottom().x,
                    VisibleRect:rightBottom().y + 50)))
        layer:addChild(ground)

        local box = ax.Node:create()
        local points = {ax.p(-100, -100), ax.p(-100, 100), ax.p(100, 100), ax.p(100, -100)}
        box:addComponent(ax.Rigidbody2D:createPolygon(points))
        box:setPosition(VisibleRect:center())
        box:getRigidbody2D().tag = sliceTag
        layer:addChild(box)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Slice")
    Helper.subtitleLabel:setString("click and drag to slice up the block")

    return layer
end


local function PhysicsDemoBug3988()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()
        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, 0))

        local ball  = ax.Sprite:create("Images/YellowSquare.png")
        ball:setPosition(ax.p(VisibleRect:center().x-100, VisibleRect:center().y))
        ball:setRotation(30.0)
        layer:addChild(ball)

        local physicsBall = makeBox(ax.p(VisibleRect:center().x+100,
                VisibleRect:center().y),
            ax.size(100, 100))
        physicsBall:setRotation(30.0)
        layer:addChild(physicsBall)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Bug3988")
    Helper.subtitleLabel:setString("All the Rectangles should have same rotation angle")

    return layer
end

local function PhysicsContactTest()
    local layer = ax.Layer:create()
    local function onEnter()
        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, 0))
        local s = ax.size(VisibleRect:getVisibleRect().width, VisibleRect:getVisibleRect().height)

        layer.yellowBoxNum = 50
        layer.blueBoxNum = 50
        layer.yellowTriangleNum = 50
        layer.blueTriangleNum = 50

        local function resetTest()
            layer:removeChildByTag(10)
            local root = ax.Node:create()
            root:setTag(10)
            layer:addChild(root)

            local s = ax.size(VisibleRect:getVisibleRect().width, VisibleRect:getVisibleRect().height)
            local subtitleLabelPosX, subtitleLabelPosY = Helper.subtitleLabel:getPosition()
            local restartTestItemPosX, restartTestItemPosY = Helper.restartTestItem:getPosition()
            local prevMenuPos = subtitleLabelPosY -  Helper.subtitleLabel:getContentSize().height;
            local menuStep = (subtitleLabelPosY  -restartTestItemPosY) * 0.25;

            local label = ax.Label:createWithTTF(tostring(layer.yellowBoxNum), s_arialPath, 32)
            root:addChild(label, 1)
            label:setAnchorPoint(ax.p(0.5, 0.5))
            label:setPosition(ax.p(s.width/2, prevMenuPos))

            prevMenuPos = prevMenuPos - menuStep
            label = ax.Label:createWithTTF(tostring(layer.blueBoxNum), s_arialPath, 32)
            root:addChild(label, 1)
            label:setAnchorPoint(ax.p(0.5, 0.5))
            label:setPosition(ax.p(s.width/2, prevMenuPos))

            prevMenuPos = prevMenuPos - menuStep
            label = ax.Label:createWithTTF(tostring(layer.yellowTriangleNum), s_arialPath, 32)
            root:addChild(label, 1)
            label:setAnchorPoint(ax.p(0.5, 0.5))
            label:setPosition(ax.p(s.width/2, prevMenuPos))

            prevMenuPos = prevMenuPos - menuStep
            label = ax.Label:createWithTTF(tostring(layer.blueTriangleNum), s_arialPath, 32)
            root:addChild(label, 1)
            label:setAnchorPoint(ax.p(0.5, 0.5))
            label:setPosition(ax.p(s.width/2, prevMenuPos))

            local wall = ax.Node:create()
            wall:addComponent(ax.Rigidbody2D:createEdgeBox(s, ax.PhysicsMaterial2D(0.1, 1, 0.0)))
            wall:setPosition(VisibleRect:center())
            root:addChild(wall)

            -- yellow box, will collide with itself and blue box.
            for i = 1, layer.yellowBoxNum do
                local size = ax.size(10 + math.random()*10, 10 + math.random()*10)
                local canvasSize = ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height)
                local position = ax.p(canvasSize.width - size.width, canvasSize.height - size.height)
                position.x = position.x * math.random()
                position.y = position.y * math.random()
                position = ax.p(VisibleRect:leftBottom().x + position.x + size.width/2,
                    VisibleRect:leftBottom().y + position.y + size.height/2)
                local velocity = ax.p((math.random() - 0.5)*200, (math.random() - 0.5)*200)
                local box = makeBox(position, size, 1, ax.PhysicsMaterial2D(0.1, 1, 0.0))
                local boxRigidbody2D = box:getRigidbody2D()
                boxRigidbody2D:setVelocity(velocity)
                boxRigidbody2D:setCategoryBits(1)    -- 0001
                boxRigidbody2D:setCollisionMaskBits(3)   -- 0011
                root:addChild(box)
            end

            -- blue box, will collide with blue box.
            for i = 1, layer.blueBoxNum do
                local size = ax.size(10 + math.random()*10, 10 + math.random()*10)
                local canvasSize = ax.size(VisibleRect:getVisibleRect().width, VisibleRect:getVisibleRect().height)
                local position = ax.p(canvasSize.width - size.width, canvasSize.height - size.height)
                position.x = position.x * math.random()
                position.y = position.y * math.random()
                position = ax.p(VisibleRect:leftBottom().x + position.x + size.width/2,
                    VisibleRect:leftBottom().y + position.y + size.height/2)
                local velocity = ax.p((math.random() - 0.5)*200, (math.random() - 0.5)*200)
                local box = makeBox(position, size, 2, ax.PhysicsMaterial2D(0.1, 1, 0.0))
                local boxRigidbody2D = box:getRigidbody2D()
                boxRigidbody2D:setVelocity(velocity)
                boxRigidbody2D:setCategoryBits(2)    -- 0010
                boxRigidbody2D:setCollisionMaskBits(1)   -- 0001
                root:addChild(box)
            end

            -- yellow triangle, will collide with itself and blue box.
            for i = 1, layer.yellowTriangleNum do
                local size = ax.size(10 + math.random()*10, 10 + math.random()*10)
                local canvasSize = ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height)
                local position = ax.p(canvasSize.width - size.width, canvasSize.height - size.height)
                position.x = position.x * math.random()
                position.y = position.y * math.random()
                position = ax.p(VisibleRect:leftBottom().x + position.x + size.width/2,
                    VisibleRect:leftBottom().y + position.y + size.height/2)
                local velocity = ax.p((math.random() - 0.5)*200, (math.random() - 0.5)*200)
                local triangle = makeTriangle(position, size, 1, ax.PhysicsMaterial2D(0.1, 1, 0.0))
                local triangleRigidbody2D = triangle:getRigidbody2D()
                triangleRigidbody2D:setVelocity(velocity)
                triangleRigidbody2D:setCategoryBits(4)    -- 0100
                triangleRigidbody2D:setCollisionMaskBits(6)   -- 0110
                root:addChild(triangle)
            end

            -- blue triangle, will collide with yellow box.
            for i = 1, layer.blueTriangleNum do
                local size = ax.size(10 + math.random()*10, 10 + math.random()*10)
                local canvasSize = ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height)
                local position = ax.p(canvasSize.width - size.width, canvasSize.height - size.height)
                position.x = position.x * math.random()
                position.y = position.y * math.random()
                position = ax.p(VisibleRect:leftBottom().x + position.x + size.width/2,
                    VisibleRect:leftBottom().y + position.y + size.height/2)
                local velocity = ax.p((math.random() - 0.5)*200, (math.random() - 0.5)*200)
                local triangle = makeTriangle(position, size, 2, ax.PhysicsMaterial2D(0.1, 1, 0.0))
                local triangleRigidbody2D = triangle:getRigidbody2D()
                triangleRigidbody2D:setVelocity(velocity)
                triangleRigidbody2D:setCategoryBits(8)    -- 1000
                triangleRigidbody2D:setCollisionMaskBits(1)   -- 0001
                root:addChild(triangle)
            end
        end

        local function onDecrease(tag, sender)
            if tag == 1 then
                if layer.yellowBoxNum > 0 then layer.yellowBoxNum = layer.yellowBoxNum - 50 end
            elseif tag == 2 then
                if layer.blueBoxNum > 0 then layer.blueBoxNum = layer.blueBoxNum - 50 end
            elseif tag == 3 then
                if layer.yellowTriangleNum > 0 then layer.yellowTriangleNum = layer.yellowTriangleNum - 50 end
            elseif tag == 4 then
                if layer.blueTriangleNum > 0 then layer.blueTriangleNum = layer.blueTriangleNum - 50 end
            end

            resetTest()
        end

        local function onIncrease(tag, sender)
            if tag == 1 then
                layer.yellowBoxNum = layer.yellowBoxNum + 50
            elseif tag == 2 then
                layer.blueBoxNum = layer.blueBoxNum + 50
            elseif tag == 3 then
                layer.yellowTriangleNum = layer.yellowTriangleNum + 50
            elseif tag == 4 then
                layer.blueTriangleNum = layer.blueTriangleNum + 50
            end

            resetTest()
        end

        local subtitleLabelPosX, subtitleLabelPosY = Helper.subtitleLabel:getPosition()
        local restartTestItemPosX, restartTestItemPosY = Helper.restartTestItem:getPosition()
        local prevMenuPos = subtitleLabelPosY -  Helper.subtitleLabel:getContentSize().height;
        local menuStep = (subtitleLabelPosY  -restartTestItemPosY) * 0.25;

        ax.MenuItemFont:setFontSize(30)
        local decrease1 = ax.MenuItemFont:create(" - ")
        decrease1:setColor(ax.color32(0,200,20))
        local increase1 = ax.MenuItemFont:create(" + ")
        increase1:setColor(ax.color32(0,200,20))
        decrease1:setTag(1)
        increase1:setTag(1)
        decrease1:registerScriptTapHandler(onDecrease)
        increase1:registerScriptTapHandler(onIncrease)

        local menu1 = ax.Menu:create(decrease1, increase1)
        menu1:alignItemsHorizontally()
        menu1:setPosition(ax.p(s.width/2, prevMenuPos))
        layer:addChild(menu1, 1)

        local label = ax.Label:createWithTTF("yellow box", s_arialPath, 32)
        layer:addChild(label, 1)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(s.width/2 - 150, prevMenuPos))

        local decrease2 = ax.MenuItemFont:create(" - ")
        decrease2:setColor(ax.color32(0,200,20))
        local increase2 = ax.MenuItemFont:create(" + ")
        increase2:setColor(ax.color32(0,200,20))
        decrease2:setTag(2)
        increase2:setTag(2)
        decrease2:registerScriptTapHandler(onDecrease)
        increase2:registerScriptTapHandler(onIncrease)

        prevMenuPos = prevMenuPos - menuStep
        local menu2 = ax.Menu:create(decrease2, increase2)
        menu2:alignItemsHorizontally()
        menu2:setPosition(ax.p(s.width/2,  prevMenuPos))
        layer:addChild(menu2, 1)

        label = ax.Label:createWithTTF("blue box", s_arialPath, 32)
        layer:addChild(label, 1)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(s.width/2 - 150, prevMenuPos))

        local decrease3 = ax.MenuItemFont:create(" - ")
        decrease3:setColor(ax.color32(0,200,20))
        local increase3 = ax.MenuItemFont:create(" + ")
        increase3:setColor(ax.color32(0,200,20))
        decrease3:setTag(3)
        increase3:setTag(3)
        decrease3:registerScriptTapHandler(onDecrease)
        increase3:registerScriptTapHandler(onIncrease)

        prevMenuPos = prevMenuPos - menuStep
        local menu3 = ax.Menu:create(decrease3, increase3)
        menu3:alignItemsHorizontally()
        menu3:setPosition(ax.p(s.width/2, prevMenuPos))
        layer:addChild(menu3, 1)

        label = ax.Label:createWithTTF("yellow triangle", s_arialPath, 32)
        layer:addChild(label, 1)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(s.width/2 - 150, prevMenuPos))

        local decrease4 = ax.MenuItemFont:create(" - ")
        decrease4:setColor(ax.color32(0,200,20))
        local increase4 = ax.MenuItemFont:create(" + ")
        increase4:setColor(ax.color32(0,200,20))
        decrease4:setTag(4)
        increase4:setTag(4)
        decrease4:registerScriptTapHandler(onDecrease)
        increase4:registerScriptTapHandler(onIncrease)

        prevMenuPos = prevMenuPos - menuStep
        local menu4 = ax.Menu:create(decrease4, increase4)
        menu4:alignItemsHorizontally()
        menu4:setPosition(ax.p(s.width/2, prevMenuPos))
        layer:addChild(menu4, 1)

        label = ax.Label:createWithTTF("blue triangle", s_arialPath, 32)
        layer:addChild(label, 1)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(s.width/2 - 150, prevMenuPos))

        resetTest()
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Contact Test")

    return layer
end

local function PhysicsPositionRotationTest()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()

        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, 0))

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local wall = ax.Node:create()
        local visibleRect = VisibleRect:getVisibleRect()
        wall:addComponent(ax.Rigidbody2D:createEdgeBox(ax.vec2(visibleRect.width, visibleRect.height)))
        wall:setPosition(VisibleRect:center())
        layer:addChild(wall)

        -- anchor test
        local anchorNode = ax.Sprite:create("Images/YellowSquare.png")
        anchorNode:setAnchorPoint(ax.p(0.1, 0.9))
        anchorNode:setPosition(100, 100)
        anchorNode:setScale(0.25)
        anchorNode:addComponent(ax.Rigidbody2D:createBox(anchorNode:getContentSize()))
        setRigidbody2DTag(anchorNode, DRAG_BODYS_TAG)
        layer:addChild(anchorNode)

        --parent test
        local parent = ax.Sprite:create("Images/YellowSquare.png")
        parent:setPosition(200, 100)
        parent:setScale(0.25)
        local parentRigidbody2D = ax.Rigidbody2D:createBox(anchorNode:getContentSize())
        parent:addComponent(parentRigidbody2D)
        parentRigidbody2D:setTag(DRAG_BODYS_TAG)
        layer:addChild(parent)

        local leftBall = ax.Sprite:create("Images/ball.png")
        leftBall:setPosition(-30, 0)
        leftBall:setScale(2)
        leftBall:addComponent(ax.Rigidbody2D:createCircle(leftBall:getContentSize().width / 2))
        setRigidbody2DTag(leftBall, DRAG_BODYS_TAG)
        parent:addChild(leftBall)

        -- offset position rotation test
        local offsetPosNode = ax.Sprite:create("Images/YellowSquare.png")
        offsetPosNode:setPosition(100, 200)
        local offsetPosNodeRigidbody2D = ax.Rigidbody2D:createBox(ax.size(offsetPosNode:getContentSize().width/2,
                offsetPosNode:getContentSize().height/2))
        offsetPosNode:addComponent(offsetPosNodeRigidbody2D)
        offsetPosNodeRigidbody2D:setPositionOffset(ax.p(-offsetPosNode:getContentSize().width/2,
                -offsetPosNode:getContentSize().height/2))
        offsetPosNodeRigidbody2D:setRotationOffset(45)
        offsetPosNodeRigidbody2D:setTag(DRAG_BODYS_TAG)
        layer:addChild(offsetPosNode)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Position/Rotation Test")

    return layer
end

local function PhysicsSetGravityEnableTest()
    local layer = ax.Layer:create()
    local function onEnter()

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local wall = ax.Node:create()
        wall:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height),
                ax.PhysicsMaterial2D(0.1, 1.0, 0.0)))
        wall:setPosition(VisibleRect:center());
        layer:addChild(wall)

        local commonBox = makeBox(ax.p(100, 100), ax.size(50, 50), 1)
        setRigidbody2DTag(commonBox, DRAG_BODYS_TAG)
        layer:addChild(commonBox)

        local box = makeBox(ax.p(200, 100), ax.size(50, 50), 2)
        local boxBody = box:getRigidbody2D()
        boxBody:setTag(DRAG_BODYS_TAG)
        boxBody:setGravityEnabled(false)
        layer:addChild(box)

        local ball = makeBall(layer,ax.p(200,200),50)
        local ballBody = ball:getRigidbody2D()
        ballBody:setTag(bit.bor(2, DRAG_BODYS_TAG))
        ballBody:setGravityEnabled(false)
        layer:addChild(ball)

        local function onScheduleOnce()
            cclog("onScheduleOnce")
            local ball = getChildByRigidbody2DTag(layer, 2)
            local rigidbody = ball and ball:getRigidbody2D()
            if rigidbody then
                rigidbody:setAutoMass(false)
                rigidbody:setMass(200)
            end
            ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, -9.8))
        end
        --layer:scheduleOnce(onScheduleOnce,1.0)
        local action = ax.Sequence:create(ax.DelayTime:create(1.0),
            ax.CallFunc:create(onScheduleOnce))
        layer:runAction(action)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Set Gravity Enable Test")
    Helper.subtitleLabel:setString("only yellow box drop down")
    return layer
end

local function PhysicsDemoBug5482()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()
        local _bodyInA = false

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        touchListener:registerScriptHandler(onTouchMoved, ax.Handler.EVENT_TOUCH_MOVED)
        touchListener:registerScriptHandler(onTouchEnded, ax.Handler.EVENT_TOUCH_ENDED)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        -- wall
        local wall = ax.Node:create()
        wall:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height),
                ax.PhysicsMaterial2D(0.1, 1.0, 0.0)))
        wall:setPosition(VisibleRect:center());
        layer:addChild(wall)

        local _nodeA = ax.Sprite:create("Images/YellowSquare.png")
        _nodeA:setPosition(ax.p(VisibleRect:center().x-150,100))
        layer:addChild(_nodeA)

        local _nodeB = ax.Sprite:create("Images/YellowSquare.png")
        _nodeB:setPosition(ax.p(VisibleRect:center().x+150,100))
        layer:addChild(_nodeB)

        local _body = ax.Rigidbody2D:createBox(_nodeA:getContentSize())
        _body:setTag(bit.bor(1, DRAG_BODYS_TAG))
        _body:retain()

        local _button = nil
        local function changeBodyCallback(sender)
            local node = nil
            if _bodyInA then
                node = _nodeB
                _button:setString("Set Body To A")
                cclog("_nodeB")
            else
                node = _nodeA
                _button:setString("Set Body To B")
                cclog("_nodeA")
            end
            local owner = _body:getOwner()
            if owner ~= nil then
                owner:removeComponent(_body)
            end

            node:addComponent(_body)
            _bodyInA = not _bodyInA
        end

        ax.MenuItemFont:setFontSize(18)
        _button = ax.MenuItemFont:create("Set Body To A");
        _button:registerScriptTapHandler(changeBodyCallback)

        local menu = ax.Menu:create(_button)
        layer:addChild(menu)
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("bug 5482: addComponentTest")
    Helper.subtitleLabel:setString("change physics body to the other.")
    return layer
end

local function PhysicsFixedUpdate()
    local layer = ax.Layer:create()
    local function onEnter()
        -- ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setDebugDrawMask(ax.PhysicsWorld.DEBUGDRAW_ALL)
        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0, 0))

        local function addBall()
            local ball = ax.Sprite:create("Images/ball.png")
            ball:setPosition(ax.p(100,100))
            ball:addComponent(ax.Rigidbody2D:createCircle(ball:getContentSize().width/2, ax.PhysicsMaterial2D(0.1, 1, 0.0)))
            local ballBody = setRigidbody2DTag(ball, DRAG_BODYS_TAG)
            ballBody:setVelocity(ax.p(1000,20))
            layer:addChild(ball)
        end

        local  function update(delta)
            for i=1,3 do
                ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():step(1/180.0)
            end
        end

        local function updateStart(delta)
            addBall()
            ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setAutoStep(false)
            layer:scheduleUpdateWithPriorityLua(update, 0)
        end

        -- wall
        local wall = ax.Node:create()
        wall:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height),
                ax.PhysicsMaterial2D(0.1, 1.0, 0.0)))
        wall:setPosition(VisibleRect:center());
        layer:addChild(wall)

        addBall()
        layer:runAction(ax.Sequence:create(ax.DelayTime:create(2.0),
                ax.CallFunc:create(updateStart)))

    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Fixed Update Test")
    Helper.subtitleLabel:setString("The second ball should not run across the wall")
    return layer
end

local function PhysicsTransformTest()
    local layer = ax.Layer:create()
    local function onEnter()
        layer:toggleDebug()
        ax.Director:getInstance():getRunningScene():getPhysicsWorld2D():setGravity(ax.p(0,0))

        local touchListener = ax.EventListenerTouchOneByOne:create()
        touchListener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, layer)

        local _rootLayer = ax.Layer:create()
        layer:addChild(_rootLayer)

        local wall = ax.Node:create()
        wall:addComponent(ax.Rigidbody2D:createEdgeBox(ax.size(VisibleRect:getVisibleRect().width,
                    VisibleRect:getVisibleRect().height),
                ax.PhysicsMaterial2D(0.1, 1.0, 0.0)))
        wall:setPosition(VisibleRect:center());
        _rootLayer:addChild(wall)

        local _parentSprite = ax.Sprite:create("Images/YellowSquare.png")
        _parentSprite:setPosition(ax.p(200,100))
        _parentSprite:setScale(0.25)
        _parentSprite:addComponent(ax.Rigidbody2D:createBox(_parentSprite:getContentSize(),ax.PhysicsMaterial2D(0.1, 1.0, 0.0)))
        setRigidbody2DTag(_parentSprite, bit.bor(1, DRAG_BODYS_TAG))
        _rootLayer:addChild(_parentSprite)

        local leftBall = ax.Sprite:create("Images/ball.png")
        leftBall:setPosition(ax.p(-30,0))
        leftBall:setScale(2)
        leftBall:addComponent(ax.Rigidbody2D:createCircle(leftBall:getContentSize().width/2,ax.PhysicsMaterial2D(0.1,1.0,0.0)))
        setRigidbody2DTag(leftBall, DRAG_BODYS_TAG)
        _parentSprite:addChild(leftBall)

        local scaleTo = ax.ScaleTo:create(2.0,0.5)
        local scaleBack = ax.ScaleTo:create(2.0,1.0)
        _parentSprite:runAction(ax.RepeatForever:create(ax.Sequence:create(scaleTo,scaleBack)))

        local normal = ax.Sprite:create("Images/YellowSquare.png")
        normal:setPosition(ax.p(300,100))
        normal:setScale(0.25,0.5)
        normal:addComponent(ax.Rigidbody2D:createBox(normal:getContentSize(),ax.PhysicsMaterial2D(0.1,1.0,0.0)))
        setRigidbody2DTag(normal, DRAG_BODYS_TAG)
        _rootLayer:addChild(normal)

        local  bullet = ax.Sprite:create("Images/ball.png")
        bullet:setPosition(ax.p(200,200))
        bullet:addComponent(ax.Rigidbody2D:createCircle(bullet:getContentSize().width/2,ax.PhysicsMaterial2D(0.1,1.0,0.0)))
        bullet:getRigidbody2D():setVelocity(ax.p(100,100))
        _rootLayer:addChild(bullet)

        local move = ax.MoveBy:create(2.0,ax.p(100,100))
        local move2 = ax.MoveBy:create(2.0,ax.p(-200,0))
        local move3 = ax.MoveBy:create(2.0,ax.p(100,-100))
        local scale = ax.ScaleTo:create(3.0,0.3)
        local scale2 = ax.ScaleTo:create(3.0,1.0)
        local rotate = ax.RotateBy:create(6.0,360)

        _rootLayer:runAction(ax.RepeatForever:create(ax.Sequence:create(move,move2,move3)))
        _rootLayer:runAction(ax.RepeatForever:create(ax.Sequence:create(scale,scale2)))
        _rootLayer:runAction(ax.RepeatForever:create(ax.Sequence:create(rotate)))
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Physics transform test")
    return layer
end

local function PhysicsIssue9959()
    local layer = ax.Layer:create()
    local function onEnter()
        local origin = ax.Director:getInstance():getVisibleOrigin()
        local visibleSize = ax.Director:getInstance():getVisibleSize()

        local scale9Sprite1 = ccui.Scale9Sprite:create("Images/ball.png")
        scale9Sprite1:setPosition(ax.p(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2))
        layer:addChild(scale9Sprite1)
        scale9Sprite1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.MoveBy:create(2.0,ax.p(100.0,0.0)),
                    ax.MoveBy:create(2.0,ax.p(-100,0.0)))))

        local scale9Sprite2 = ccui.Scale9Sprite:create("Images/ball.png")
        scale9Sprite2:setPosition(ax.p(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2+50))
        layer:addChild(scale9Sprite2)
        scale9Sprite2:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.ScaleTo:create(2.0,1.5),
                    ax.ScaleTo:create(2.0,1.0))))

        local scale9Sprite3 = ccui.Scale9Sprite:create("Images/ball.png")
        scale9Sprite3:setPosition(ax.p(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2-50))
        layer:addChild(scale9Sprite3)
        scale9Sprite3:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.RotateBy:create(2.0,360))))
    end

    initWithLayer(layer, onEnter)
    Helper.titleLabel:setString("Reorder issue #9959")
    Helper.subtitleLabel:setString("Test Scale9Sprite run scale/move/rotation action in physics scene")
    return layer
end

function PhysicsTest()
    cclog("PhysicsTest")
    local scene = ax.Scene:createWithPhysics()


    Helper.usePhysics = true
    Helper.createFunctionTable = {
        PhysicsDemoLogoSmash,
        PhysicsDemoPyramidStack,
        PhysicsDemoClickAdd,
        PhysicsDemoRayCast,
        PhysicsDemoJoints,
        PhysicsDemoActions,
        PhysicsDemoPump,
        PhysicsDemoOneWayPlatform,
        PhysicsDemoSlice,
        PhysicsDemoBug3988,
        PhysicsContactTest,
        PhysicsPositionRotationTest,
        PhysicsSetGravityEnableTest,
        PhysicsDemoBug5482,
        PhysicsFixedUpdate,
        PhysicsTransformTest,
        PhysicsIssue9959
    }
    Helper.index = 1

    scene:addChild(Helper.createFunctionTable[1]())
    scene:addChild(CreateBackMenuItem())
    return scene
end
