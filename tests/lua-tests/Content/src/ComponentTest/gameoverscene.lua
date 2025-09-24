local GameOverLayer = {

    create = function(self, content)
        local label = ax.Label:createWithTTF(content, "fonts/Marker Felt.ttf", 32)
        local canvasSize = ax.Director:getInstance():getCanvasSize()
        label:setColor(ax.color32(0, 0, 0))
        label:setPosition(canvasSize.width/2, canvasSize.height/2)

        local layer = ax.LayerColor:create(ax.color32(0, 128, 255, 255))
        layer:addChild(label)
        layer:runAction(ax.Sequence:create(ax.DelayTime:create(3), ax.CallFunc:create(self.gameOverDone)))

        return layer
    end,

    gameOverDone = function(self, node)
        -- send event
        ax.Director:getInstance():getEventDispatcher():dispatchCustomEvent("component game over")
    end
}

GameOverScene = {
    create = function(label)
        local scene = ax.Scene:create()
        local layer = GameOverLayer:create(label)
        scene:addChild(layer)

        return scene
    end
}
