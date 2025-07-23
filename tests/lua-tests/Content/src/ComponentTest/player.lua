
local player = {
    generateProjectile = function (self, x, y)
        local projectile = ax.Sprite:create("components/Projectile.png", ax.rect(0, 0, 20, 20))
        local scriptComponent = ax.ComponentLua:create("ComponentTest/projectile.lua")
        projectile:addComponent(scriptComponent)
        self:getOwner():getParent():addChild(projectile)

        -- set position
        local director = ax.Director:getInstance()
        local winSize = director:getVisibleSize()
        local visibleOrigin = director:getVisibleOrigin()
        projectile:setPosition(ax.p(visibleOrigin.x + 20, visibleOrigin.y + winSize.height/2))

        -- run action
        local posX, posY = projectile:getPosition()
        local offX = x - posX
        local offY = y - posY

        if offX <= 0 then
            return
        end

        local contentSize = projectile:getContentSize()
        local realX = visibleOrigin.x + winSize.width + contentSize.width/2
        local ratio = offY / offX
        local realY = (realX * ratio) + posY
        local realDest = ax.p(realX, realY)

        local offRealX = realX - posX
        local offRealY = realY - posY
        local length = math.sqrt((offRealX * offRealX) + (offRealY * offRealY))
        local velocity = 960
        local realMoveDuration = length / velocity

        projectile:runAction(ax.MoveTo:create(realMoveDuration, realDest))
    end,

    onEnter = function(self)
        local function onTouchesEnded(touches, event)
            local location = touches[1]:getLocation()
            self:generateProjectile(location.x, location.y)
            ax.AudioEngine:play2d("pew-pew-lei.wav")
        end

        local listener = ax.EventListenerTouchAllAtOnce:create()
        listener:registerScriptHandler(onTouchesEnded, ax.Handler.EVENT_TOUCHES_ENDED)
        local eventDispatcher = ax.Director:getInstance():getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self:getOwner())
    end,
}

return player
