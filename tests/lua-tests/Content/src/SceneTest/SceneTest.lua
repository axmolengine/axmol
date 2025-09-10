local    MID_PUSHSCENE        = 100
local    MID_PUSHSCENETRAN    = 101
local    MID_QUIT             = 102
local    MID_REPLACESCENE     = 103
local    MID_REPLACESCENETRAN = 104
local    MID_GOBACK           = 105

local SceneTestLayer1 = nil
local SceneTestLayer2 = nil
local SceneTestLayer3 = nil

--------------------------------------------------------------------
--
-- SceneTestLayer1
--
--------------------------------------------------------------------

SceneTestLayer1 = function()
    local ret = ax.Layer:create()

    local function onPushScene(tag, pSender)
        local scene = ax.Scene:create()
        local  layer = SceneTestLayer2()
        scene:addChild(layer, 0)
        scene:addChild(CreateBackMenuItem())
        ax.Director:getInstance():pushScene( scene )
    end

    local function onPushSceneTran(tag, pSender)
        local scene = ax.Scene:create()
        local  layer = SceneTestLayer2()
        scene:addChild(layer, 0)
        scene:addChild(CreateBackMenuItem())
        ax.Director:getInstance():pushScene( ax.TransitionSlideInT:create(1, scene) )
    end


    local function onQuit(tag, pSender)
        cclog("onQuit")
    end

    local  item1 = ax.MenuItemFont:create( "Test pushScene")
    item1:registerScriptTapHandler(onPushScene)
    local  item2 = ax.MenuItemFont:create( "Test pushScene w/transition")
    item2:registerScriptTapHandler(onPushSceneTran)
    local  item3 = ax.MenuItemFont:create( "Quit")
    item3:registerScriptTapHandler(onQuit)

    local  menu = ax.Menu:create(item1, item2, item3)
    menu:alignItemsVertically()

    ret:addChild( menu )

    local s = ax.Director:getInstance():getLogicalSize()
    local  sprite = ax.Sprite:create(s_pPathGrossini)
    ret:addChild(sprite)
    sprite:setPosition( ax.p(s.width-40, s.height/2) )
    local  rotate = ax.RotateBy:create(2, 360)
    local  repeatAction = ax.RepeatForever:create(rotate)
    sprite:runAction(repeatAction)

    local function onNodeEvent(event)
        if event == "enter" then
            cclog("SceneTestLayer1#onEnter")
        elseif event == "enterTransitionFinish" then
            cclog("SceneTestLayer1#onEnterTransitionDidFinish")
        end
    end

    ret:registerScriptHandler(onNodeEvent)
    return ret
end

--------------------------------------------------------------------
--
-- SceneTestLayer2
--
--------------------------------------------------------------------

SceneTestLayer2 = function()
    local ret = ax.Layer:create()
    local m_timeCounter = 0

    local function onGoBack(tag, pSender)
        ax.Director:getInstance():popScene()
    end

    local function onReplaceScene(tag, pSender)
        local scene = ax.Scene:create()
        local  layer = SceneTestLayer3()
        scene:addChild(layer, 0)
        scene:addChild(CreateBackMenuItem())
        ax.Director:getInstance():replaceScene( scene )
    end


    local function onReplaceSceneTran(tag, pSender)
        local scene = ax.Scene:create()
        local  layer = SceneTestLayer3()
        scene:addChild(layer, 0)
        scene:addChild(CreateBackMenuItem())
        ax.Director:getInstance():replaceScene( ax.TransitionFlipX:create(2, scene) )
    end

    local  item1 = ax.MenuItemFont:create( "replaceScene")
    item1:registerScriptTapHandler(onReplaceScene)
    local  item2 = ax.MenuItemFont:create( "replaceScene w/transition")
    item2:registerScriptTapHandler(onReplaceSceneTran)
    local  item3 = ax.MenuItemFont:create( "Go Back")
    item3:registerScriptTapHandler(onGoBack)
    local  menu = ax.Menu:create(item1, item2, item3)
    menu:alignItemsVertically()

    ret:addChild( menu )

    local s = ax.Director:getInstance():getLogicalSize()
    local  sprite = ax.Sprite:create(s_pPathGrossini)
    ret:addChild(sprite)
    sprite:setPosition( ax.p(s.width-40, s.height/2) )
    local  rotate = ax.RotateBy:create(2, 360)
    local  repeat_action = ax.RepeatForever:create(rotate)
    sprite:runAction(repeat_action)

    return ret
end

--------------------------------------------------------------------
--
-- SceneTestLayer3
--
--------------------------------------------------------------------

SceneTestLayer3 = function()
    local ret = ax.LayerColor:create(ax.color32(0,0,255,255))
    local s = ax.Director:getInstance():getLogicalSize()

    local function item0Clicked(tag, pSender)
        local newScene = ax.Scene:create()
        newScene:addChild(SceneTestLayer3())
        ax.Director:getInstance():pushScene(ax.TransitionFade:create(0.5, newScene, ax.color32(0,255,255)))
    end

    local function item1Clicked(tag, pSender)
        ax.Director:getInstance():popScene()
    end

    local function item2Clicked(tag, pSender)
        ax.Director:getInstance():popToRootScene()
    end

    local item0 = ax.MenuItemFont:create("Touch to pushScene (self)")
    item0:registerScriptTapHandler(item0Clicked)
    local item1 = ax.MenuItemFont:create("Touch to popScene")
    item1:registerScriptTapHandler(item1Clicked)
    local item2 = ax.MenuItemFont:create("Touch to popToRootScene")
    item2:registerScriptTapHandler(item2Clicked)

    local menu = ax.Menu:create(item0, item1, item2)
    ret:addChild(menu)
    menu:alignItemsVertically()

    local  sprite = ax.Sprite:create(s_pPathGrossini)
    ret:addChild(sprite)
    sprite:setPosition( ax.p(s.width/2, 40) )
    local  rotate = ax.RotateBy:create(2, 360)
    local  repeatAction = ax.RepeatForever:create(rotate)
    sprite:runAction(repeatAction)
    return ret
end



function SceneTestMain()
    cclog("SceneTestMain")
    local scene = ax.Scene:create()
    local  layer = SceneTestLayer1()
    scene:addChild(layer, 0)
    scene:addChild(CreateBackMenuItem())
    return scene
end
