local function ComponentTestLayer()

    local layer = ax.Layer:create()

    -- set background color
    local bgLayer = ax.LayerColor:create(ax.color32(0, 128, 255, 255));
    layer:addChild(bgLayer);

    -- create game scene
    local sceneLuaComponent = ax.ComponentLua:create("ComponentTest/scene.lua");
    sceneLuaComponent:setName("sceneLuaComponent");
    layer:addComponent(sceneLuaComponent);

    -- add player
    local player = ax.Sprite:create("components/Player.png", ax.rect(0, 0, 27, 40));
    local visibleSize = ax.Director:getInstance():getVisibleSize();
    local origin = ax.Director:getInstance():getVisibleOrigin();
    player:setPosition(origin.x + player:getContentSize().width/2,
                       origin.y + visibleSize.height/2);
    local playerComponent = ax.ComponentLua:create("ComponentTest/player.lua");
    player:addComponent(playerComponent);
    layer:addChild(player);

    local function onEvent(event)
        ax.AudioEngine:stopAll()
        local scene = ax.Scene:create()

        scene:addChild(ComponentTestLayer())
        scene:addChild(CreateBackMenuItem())
        ax.Director:getInstance():replaceScene(scene);
    end

    local function onNodeEvent(event)
        if "exit" == event then
            ax.AudioEngine:stopAll()
        end
    end

    layer._listener1 = ax.EventListenerCustom:create("component game over", onEvent)
    local eventDispatcher = ax.Director:getInstance():getEventDispatcher()
    eventDispatcher:addEventListenerWithFixedPriority(layer._listener1, 1)

    layer:registerScriptHandler(onNodeEvent)

    return layer
end


function ComponentTestMain()

    cclog("ComponentTestMain")
    local scene = ax.Scene:create()
    scene:addChild(ComponentTestLayer())
    scene:addChild(CreateBackMenuItem())
    return scene

end
