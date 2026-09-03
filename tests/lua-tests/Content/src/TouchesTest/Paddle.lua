require "VisibleRect"

Paddle = class("Paddle", function(texture)
    return ax.Sprite:createWithTexture(texture)
end)

Paddle.__index = Paddle

local kPaddleStateGrabbed = 0
local kPaddleStateUngrabbed = 1

Paddle._state = kPaddleStateGrabbed


function Paddle:rect()
    local  s = self:getTexture():getContentSize()
    return ax.rect(-s.width / 2, -s.height / 2, s.width, s.height)
end

function Paddle:containsTouchLocation(x,y)
    local position = ax.p(self:getPosition())
    local  s = self:getTexture():getContentSize()
    local touchRect = ax.rect(-s.width / 2 + position.x, -s.height / 2 + position.y, s.width, s.height)
    local b = ax.rectContainsPoint(touchRect, ax.p(x,y))
    return b
end

function Paddle:paddleWithTexture(aTexture)
    local pPaddle = Paddle.new(aTexture)
    pPaddle._state = kPaddleStateUngrabbed
    pPaddle:setLifecycleCallback(function(tag)
    if "enter" == tag then
        pPaddle:onEnter()
    elseif "exit" == tag then
    end
end)
    return pPaddle
end

function Paddle:onEnter()
    local  listenner = ax.PointerEventListener:create()
    listenner.onPointerDown = function(event)
            local worldPos = event:getWorldPoint()
            print(string.format("Paddle::onTouchBegan id = %d, x = %f, y = %f", event:getPointerId(), worldPos.x, worldPos.y))
            if (self._state ~= kPaddleStateUngrabbed) then
                return false
            end

            if not self:containsTouchLocation(worldPos.x,worldPos.y) then
                return false
            end

            self._state = kPaddleStateGrabbed
            return true
        end
    listenner.onPointerMove = function(event)
            local touchPoint = event:getWorldPoint()
            print(string.format("Paddle::onPointerMove id = %d, x = %f, y = %f", event:getPointerId(), touchPoint.x, touchPoint.y))
            assert(self._state == kPaddleStateGrabbed, "Paddle - Unexpected state!")
            local curPosX,curPosY = self:getPosition()
            self:setPosition(ax.p(touchPoint.x,curPosY))
        end
    listenner.onPointerUp = function(event)
            assert(self._state == kPaddleStateGrabbed, "Paddle - Unexpected state!")
            self._state = kPaddleStateUngrabbed
        end
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listenner, self)
end
