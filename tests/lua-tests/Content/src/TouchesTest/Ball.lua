require "VisibleRect"
require "TouchesTest/Paddle"

Ball = class("Ball", function(texture)
    return ax.Sprite:createWithTexture(texture)
end)

Ball.__index = Ball

Ball.m_velocity = ax.p(0,0)

local M_PI = 3.1415926

function Ball:radius()
    local  size = self:getTexture():getContentSize()
    return size.width/2
end

function Ball:move(delta)
    local getPosition = ax.p(self:getPosition())
    local position = ax.pMul(self.m_velocity, delta)
    self:setPosition( ax.pAdd(getPosition, position) )

    if (getPosition.x > VisibleRect:right().x - self:radius()) then
        self:setPosition( ax.p( VisibleRect:right().x - self:radius(), getPosition.y) )
        self.m_velocity.x = self.m_velocity.x * -1
    elseif (getPosition.x < VisibleRect:left().x + self:radius()) then
        self:setPosition( ax.p(VisibleRect:left().x + self:radius(), getPosition.y) )
        self.m_velocity.x = self.m_velocity.x * -1
    end
end

function Ball:collideWithPaddle(paddle)
    local paddleRect = paddle:rect()
    local paddleGetPosition = ax.p(paddle:getPosition())
    local selfGetPosition = ax.p(self:getPosition())

    paddleRect.x = paddleRect.x + paddleGetPosition.x
    paddleRect.y = paddleRect.y + paddleGetPosition.y

    local lowY  = ax.rectGetMinY(paddleRect)
    local midY  = ax.rectGetMidY(paddleRect)
    local highY = ax.rectGetMaxY(paddleRect)

    local leftX  = ax.rectGetMinX(paddleRect)
    local rightX = ax.rectGetMaxX(paddleRect)

    if (selfGetPosition.x > leftX and selfGetPosition.x < rightX)  then

        local hit = false
        local angleOffset = 0.0

        if (selfGetPosition.y > midY and selfGetPosition.y <= highY + self:radius()) then
            self:setPosition( ax.p(selfGetPosition.x, highY + self:radius()) )
            hit = true
            angleOffset = M_PI / 2
        elseif (selfGetPosition.y < midY and selfGetPosition.y >= lowY - self:radius()) then
            self:setPosition( ax.p(selfGetPosition.x, lowY - self:radius()) )
            hit = true
            angleOffset = -M_PI / 2
        end

        if (hit) then
            local hitAngle = ax.pToAngleSelf(ax.pSub(paddleGetPosition, paddleGetPosition)) + angleOffset

            local scalarVelocity = ax.pGetLength(self.m_velocity) * 1.05
            local velocityAngle = -ax.pToAngleSelf(self.m_velocity) + 0.5 * hitAngle

            self.m_velocity = ax.pMul(ax.pForAngle(velocityAngle), scalarVelocity)
        end
    end

end

function Ball:setVelocity(velocity)
    self.m_velocity = velocity
end

function Ball:getVelocity()
    return self.m_velocity
end

function Ball.ballWithTexture(aTexture)

    if(aTexture == nil) then
        cclog("in ballWithTexture aTexture == nil")
    end

    local ball = Ball.new(aTexture)
    --ball:autorelease()
    return ball
end

