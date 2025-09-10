require "TransitionsTest/TransitionsName"


local SceneIdx = -1
local CurSceneNo = 2
local TRANSITION_DURATION = 1.2
local s = ax.Director:getInstance():getLogicalSize()

local function switchSceneTypeNo()
    if CurSceneNo == 1 then
        CurSceneNo = 2
    else
        CurSceneNo = 1
    end
end

local function backAction()
    SceneIdx = SceneIdx - 1
    if SceneIdx < 0 then
        SceneIdx = SceneIdx + Transition_Table.MAX_LAYER
    end

    switchSceneTypeNo()
    return generateTranScene()
end

local function restartAction()
    return generateTranScene()
end

local function nextAction()
    SceneIdx = SceneIdx + 1
    SceneIdx = math.fmod(SceneIdx, Transition_Table.MAX_LAYER)

    switchSceneTypeNo()
    return generateTranScene()
end

local function backCallback(sender)
    local scene = backAction()
    ax.Director:getInstance():replaceScene(scene)
end

local function restartCallback(sender)
    local scene = restartAction()
    ax.Director:getInstance():replaceScene(scene)
end

local function nextCallback(sender)
    local scene = nextAction()
    ax.Director:getInstance():replaceScene(scene)
end

-----------------------------
-- TestLayer1
-----------------------------
local function createLayer1()
    local layer = ax.Layer:create()
    local x, y = s.width, s.height

    local bg1 = ax.Sprite:create(s_back1)
    bg1:setPosition(ax.p(s.width / 2, s.height / 2))
    layer:addChild(bg1, -1)

    local titleLabel = ax.Label:createWithTTF(Transition_Name[SceneIdx], s_thonburiPath, 32)
    layer:addChild(titleLabel)
    titleLabel:setColor(ax.color32(255,32,32))
    titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    titleLabel:setPosition(x / 2, y - 100)

    local label = ax.Label:createWithTTF("SCENE 1", s_markerFeltFontPath, 38)
    label:setColor(ax.color32(16,16,255))
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(x / 2, y / 2)
    layer:addChild(label)

    -- menu
    local item1 = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
    local item2 = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
    local item3 = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
    item1:registerScriptTapHandler(backCallback)
    item2:registerScriptTapHandler(restartCallback)
    item3:registerScriptTapHandler(nextCallback)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:setPosition(ax.p(0, 0))
    item1:setPosition(ax.p(s.width / 2 - item2:getContentSize().width * 2, item2:getContentSize().height / 2))
    item2:setPosition(ax.p(s.width / 2, item2:getContentSize().height / 2))
    item3:setPosition(ax.p(s.width / 2 + item2:getContentSize().width * 2, item2:getContentSize().height / 2))

    layer:addChild(menu, 1)

    return layer
end

-----------------------------
-- TestLayer2
-----------------------------
local function createLayer2()
    local layer = ax.Layer:create()
    local x, y = s.width, s.height

    local bg1 = ax.Sprite:create(s_back2)
    bg1:setPosition(ax.p(s.width / 2, s.height / 2))
    layer:addChild(bg1, -1)

    local titleLabel = ax.Label:createWithTTF(Transition_Name[SceneIdx], s_thonburiPath, 32 )
    layer:addChild(titleLabel)
    titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    titleLabel:setColor(ax.color32(255,32,32))
    titleLabel:setPosition(x / 2, y - 100)

    local label = ax.Label:createWithTTF("SCENE 2", s_markerFeltFontPath, 38)
    label:setColor(ax.color32(16,16,255))
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(x / 2, y / 2)
    layer:addChild(label)

    -- menu
    local item1 = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
    local item2 = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
    local item3 = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
    item1:registerScriptTapHandler(backCallback)
    item2:registerScriptTapHandler(restartCallback)
    item3:registerScriptTapHandler(nextCallback)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:setPosition(ax.p(0, 0))
    item1:setPosition(ax.p(s.width / 2 - item2:getContentSize().width * 2, item2:getContentSize().height / 2))
    item2:setPosition(ax.p(s.width / 2, item2:getContentSize().height / 2))
    item3:setPosition(ax.p(s.width / 2 + item2:getContentSize().width * 2, item2:getContentSize().height / 2))

    layer:addChild(menu, 1)

    return layer
end

-----------------------------
-- Create Transition Test
-----------------------------
local function createTransition(index, t, scene)
    ax.Director:getInstance():getRenderer():setDepthTest(false)

    if firstEnter == true then
        firstEnter = false
        return scene
    end

    if index == Transition_Table.CCTransitionJumpZoom then
        scene = ax.TransitionJumpZoom:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressRadialCCW then
        scene = ax.TransitionProgressRadialCCW:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressRadialCW then
        scene = ax.TransitionProgressRadialCW:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressHorizontal then
        scene = ax.TransitionProgressHorizontal:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressVertical then
        scene = ax.TransitionProgressVertical:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressInOut then
        scene = ax.TransitionProgressInOut:create(t, scene)
    elseif index == Transition_Table.CCTransitionProgressOutIn then
        scene = ax.TransitionProgressOutIn:create(t, scene)
    elseif index == Transition_Table.CCTransitionCrossFade then
        scene = ax.TransitionCrossFade:create(t, scene)
    elseif index == Transition_Table.TransitionPageForward then
        ax.Director:getInstance():getRenderer():setDepthTest(true)
        scene = ax.TransitionPageTurn:create(t, scene, false)
    elseif index == Transition_Table.TransitionPageBackward then
        ax.Director:getInstance():getRenderer():setDepthTest(true)
        scene = ax.TransitionPageTurn:create(t, scene, true)
    elseif index == Transition_Table.CCTransitionFadeTR then
        scene = ax.TransitionFadeTR:create(t, scene)
    elseif index == Transition_Table.CCTransitionFadeBL then
        scene = ax.TransitionFadeBL:create(t, scene)
    elseif index == Transition_Table.CCTransitionFadeUp then
        scene = ax.TransitionFadeUp:create(t, scene)
    elseif index == Transition_Table.CCTransitionFadeDown then
        scene = ax.TransitionFadeDown:create(t, scene)
    elseif index == Transition_Table.CCTransitionTurnOffTiles then
        scene = ax.TransitionTurnOffTiles:create(t, scene)
    elseif index == Transition_Table.CCTransitionSplitRows then
        scene = ax.TransitionSplitRows:create(t, scene)
    elseif index == Transition_Table.CCTransitionSplitCols then
        scene = ax.TransitionSplitCols:create(t, scene)
    elseif index == Transition_Table.CCTransitionFade then
        scene = ax.TransitionFade:create(t, scene)
    elseif index == Transition_Table.FadeWhiteTransition then
        scene = ax.TransitionFade:create(t, scene, ax.color32(255, 255, 255))
    elseif index == Transition_Table.FlipXLeftOver then
        scene = ax.TransitionFlipX:create(t, scene, ax.TRANSITION_ORIENTATION_LEFT_OVER )
    elseif index == Transition_Table.FlipXRightOver then
        scene = ax.TransitionFlipX:create(t, scene, ax.TRANSITION_ORIENTATION_RIGHT_OVER )
    elseif index == Transition_Table.FlipYUpOver then
        scene = ax.TransitionFlipY:create(t, scene, ax.TRANSITION_ORIENTATION_UP_OVER)
    elseif index == Transition_Table.FlipYDownOver then
        scene = ax.TransitionFlipY:create(t, scene, ax.TRANSITION_ORIENTATION_DOWN_OVER )
    elseif index == Transition_Table.FlipAngularLeftOver then
        scene = ax.TransitionFlipAngular:create(t, scene, ax.TRANSITION_ORIENTATION_LEFT_OVER )
    elseif index == Transition_Table.FlipAngularRightOver then
        scene = ax.TransitionFlipAngular:create(t, scene, ax.TRANSITION_ORIENTATION_RIGHT_OVER )
    elseif index == Transition_Table.ZoomFlipXLeftOver then
        scene = ax.TransitionZoomFlipX:create(t, scene, ax.TRANSITION_ORIENTATION_LEFT_OVER )
    elseif index == Transition_Table.ZoomFlipXRightOver then
        scene = ax.TransitionZoomFlipX:create(t, scene, ax.TRANSITION_ORIENTATION_RIGHT_OVER )
    elseif index == Transition_Table.ZoomFlipYUpOver then
        scene = ax.TransitionZoomFlipY:create(t, scene, ax.TRANSITION_ORIENTATION_UP_OVER)
    elseif index == Transition_Table.ZoomFlipYDownOver then
        scene = ax.TransitionZoomFlipY:create(t, scene, ax.TRANSITION_ORIENTATION_DOWN_OVER )
    elseif index == Transition_Table.ZoomFlipAngularLeftOver then
        scene = ax.TransitionZoomFlipAngular:create(t, scene, ax.TRANSITION_ORIENTATION_LEFT_OVER )
    elseif index == Transition_Table.ZoomFlipAngularRightOver then
        scene = ax.TransitionZoomFlipAngular:create(t, scene, ax.TRANSITION_ORIENTATION_RIGHT_OVER )
    elseif index == Transition_Table.CCTransitionShrinkGrow then
        scene = ax.TransitionShrinkGrow:create(t, scene)
    elseif index == Transition_Table.CCTransitionRotoZoom then
        scene = ax.TransitionRotoZoom:create(t, scene)
    elseif index == Transition_Table.CCTransitionMoveInL then
        scene = ax.TransitionMoveInL:create(t, scene)
    elseif index == Transition_Table.CCTransitionMoveInR then
        scene = ax.TransitionMoveInR:create(t, scene)
    elseif index == Transition_Table.CCTransitionMoveInT then
        scene = ax.TransitionMoveInT:create(t, scene)
    elseif index == Transition_Table.CCTransitionMoveInB then
        scene = ax.TransitionMoveInB:create(t, scene)
    elseif index == Transition_Table.CCTransitionSlideInL then
        scene = ax.TransitionSlideInL:create(t, scene)
    elseif index == Transition_Table.CCTransitionSlideInR then
        scene = ax.TransitionSlideInR:create(t, scene)
    elseif index == Transition_Table.CCTransitionSlideInT then
        scene = ax.TransitionSlideInT:create(t, scene)
    elseif index == Transition_Table.CCTransitionSlideInB then
        scene = ax.TransitionSlideInB:create(t, scene)
    end

    return scene
end

function generateTranScene()
    local scene = ax.Scene:create()
    local layer = nil

    if CurSceneNo == 1 then
        layer = createLayer1()
    elseif CurSceneNo == 2 then
        layer = createLayer2()
    end

    scene:addChild(layer)
    scene:addChild(CreateBackMenuItem())

    return createTransition(SceneIdx, TRANSITION_DURATION, scene)
end

function TransitionsTest()
    cclog("TransitionsTest")
    local scene = ax.Scene:create()

    SceneIdx = -1
    CurSceneNo = 2
    firstEnter = true

    return nextAction()
end
