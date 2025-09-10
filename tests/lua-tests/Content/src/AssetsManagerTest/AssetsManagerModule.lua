local AssetManagerModule = {}

function AssetManagerModule.newScene(backfunc)

    local winSize = ax.Director:getInstance():getLogicalSize()

    local newScene = ax.Scene:create()
    local layer    = ax.Layer:create()

    local function backToUpdate()
        local scene = backfunc()
        if scene ~= nil then
            ax.Director:getInstance():replaceScene(scene)
        end
    end

    --Create BackMneu
    ax.MenuItemFont:setFontName("Arial")
    ax.MenuItemFont:setFontSize(24)
    local backMenuItem = ax.MenuItemFont:create("Back")
    backMenuItem:setPosition(ax.p(VisibleRect:rightBottom().x - 50, VisibleRect:rightBottom().y + 25))
    backMenuItem:registerScriptTapHandler(backToUpdate)

    local backMenu = ax.Menu:create()
    backMenu:setPosition(0, 0)
    backMenu:addChild(backMenuItem)
    layer:addChild(backMenu,6)

    local helloLabel =  ax.Label:createWithTTF("Hello World", s_arialPath, 38)
    helloLabel:setAnchorPoint(ax.p(0.5, 0.5))
    helloLabel:setPosition(ax.p(winSize.width / 2, winSize.height - 40))
    layer:addChild(helloLabel, 5)

    local sprite = ax.Sprite:create("Images/background.png")
    sprite:setAnchorPoint(ax.p(0.5, 0.5))
    sprite:setPosition(ax.p(winSize.width / 2, winSize.height / 2))
    layer:addChild(sprite, 0)

    newScene:addChild(layer)
    ax.Director:getInstance():replaceScene(newScene)
end


return AssetManagerModule
