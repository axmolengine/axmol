local size = ax.Director:getInstance():getCanvasSize()
local layer = nil
local kTagSprite = 1

local function initWithLayer()
    local sprite = ax.Sprite:create(s_pPathGrossini)

    local bgLayer = ax.LayerColor:create(ax.color32(255,255,0,255))
    layer:addChild(bgLayer, -1)

    layer:addChild(sprite, 0, kTagSprite)
    sprite:setPosition(ax.p(20,150))

    sprite:runAction(ax.JumpTo:create(4, ax.p(300,48), 100, 4))

    bgLayer:runAction(ax.RepeatForever:create(ax.Sequence:create(
                                                 ax.FadeIn:create(1),
                                                 ax.FadeOut:create(1))))

    local function onTouchBegan(touch, event)
        return true
    end

    local function onTouchEnded(touch, event)

        local location = touch:getLocation()

        local s = layer:getChildByTag(kTagSprite)
        s:stopAllActions()
        s:runAction(ax.MoveTo:create(1, ax.p(location.x, location.y)))
        local posX, posY = s:getPosition()
        local o = location.x - posX
        local a = location.y - posY
        local at = math.atan(o / a) / math.pi * 180.0

        if a < 0 then
            if o < 0 then
                at = 180 + math.abs(at)
            else
                at = 180 - math.abs(at)
            end
        end
        s:runAction(ax.RotateTo:create(1, at))
    end

    local listener = ax.EventListenerTouchOneByOne:create()
    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = layer:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, layer)

    return layer
end

--------------------------------
-- Click And Move Test
--------------------------------
function ClickAndMoveTest()
    cclog("ClickAndMoveTest")
    local scene = ax.Scene:create()
    layer = ax.Layer:create()

    initWithLayer()
    scene:addChild(layer)
    scene:addChild(CreateBackMenuItem())


    return scene
end
