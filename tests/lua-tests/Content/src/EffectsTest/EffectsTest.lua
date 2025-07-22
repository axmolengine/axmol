require "EffectsTest/EffectsName"


local ActionIdx = -1
local size = ax.Director:getInstance():getWinSize()
local kTagTextLayer  = 1
local kTagBackground = 1
local kTagLabel      = 2
local MAX_LAYER      = 22

local testLayer  = nil
local titleLabel = nil
local entry      = nil
local gridNodeTarget = nil

local function checkAnim(dt)

    if nil == gridNodeTarget then
        return
    end

    if gridNodeTarget:getNumberOfRunningActions() == 0 and nil ~=gridNodeTarget:getGrid() then
        gridNodeTarget:setGrid(nil)
    end
end

local function onEnterOrExit(tag)
    local scheduler = ax.Director:getInstance():getScheduler()
    if tag == "enter" then
        entry = scheduler:scheduleScriptFunc(checkAnim, 0, false)
    elseif tag == "exit" then
        scheduler:unscheduleScriptEntry(entry)
    end
end

local function backAction()
    ActionIdx = ActionIdx - 1
    if ActionIdx < 0 then
        ActionIdx = ActionIdx + MAX_LAYER
    end

    return CreateEffectsTestLayer()
end

local function restartAction()
    return CreateEffectsTestLayer()
end

local function nextAction()
    ActionIdx = ActionIdx + 1
    ActionIdx = math.fmod(ActionIdx, MAX_LAYER)

    return CreateEffectsTestLayer()
end

local function backCallback(sender)
    local scene = ax.Scene:create()

    scene:addChild(backAction())
    scene:addChild(CreateBackMenuItem())

    ax.Director:getInstance():replaceScene(scene)
end

local function restartCallback(sender)
    local scene = ax.Scene:create()

    scene:addChild(restartAction())
    scene:addChild(CreateBackMenuItem())

    ax.Director:getInstance():replaceScene(scene)
end

local function nextCallback(sender)
    local scene = ax.Scene:create()

    scene:addChild(nextAction())
    scene:addChild(CreateBackMenuItem())

    ax.Director:getInstance():replaceScene(scene)
end

--------------------------------------
--  Shaky3DDemo
--------------------------------------
local function Shaky3DDemo(t)
    return ax.Shaky3D:create(t, ax.size(15,10), 5, false)
end

--------------------------------------
--  Waves3DDemo
--------------------------------------
local function Waves3DDemo(t)
    return ax.Waves3D:create(t, ax.size(15,10), 5, 40)
end

--------------------------------------
--  FlipX3DDemo
--------------------------------------
local function FlipX3DDemo(t)
    local flipx  = ax.FlipX3D:create(t)
    local flipx_back = flipx:reverse()
    local delay = ax.DelayTime:create(2)

    return ax.Sequence:create(flipx, flipx_back, delay)
end

--------------------------------------
--  FlipY3DDemo
--------------------------------------
local function FlipY3DDemo(t)
    local flipy  = ax.FlipY3D:create(t)
    local flipy_back = flipy:reverse()
    local delay = ax.DelayTime:create(2)

    return ax.Sequence:create(flipy, flipy_back, delay)
end

--------------------------------------
--  Lens3DDemo
--------------------------------------
local function Lens3DDemo(t)
    return ax.Lens3D:create(t, ax.size(15,10), ax.p(size.width/2,size.height/2), 240)
end

--------------------------------------
--  Ripple3DDemo
--------------------------------------
local function Ripple3DDemo(t)
    return ax.Ripple3D:create(t, ax.size(32,24), ax.p(size.width/2,size.height/2), 240, 4, 160)
end

--------------------------------------
--  LiquidDemo
--------------------------------------
local function LiquidDemo(t)
    return ax.Liquid:create(t, ax.size(16,12), 4, 20)
end

--------------------------------------
--  WavesDemo
--------------------------------------
local function WavesDemo(t)
    return ax.Waves:create(t, ax.size(16,12), 4, 20, true, true)
end

--------------------------------------
--  TwirlDemo
--------------------------------------
local function TwirlDemo(t)
    return ax.Twirl:create(t, ax.size(12,8), ax.p(size.width/2, size.height/2), 1, 2.5)
end

--------------------------------------
--  ShakyTiles3DDemo
--------------------------------------
local function ShakyTiles3DDemo(t)
    return ax.ShakyTiles3D:create(t, ax.size(16,12), 5, false)
end

--------------------------------------
--  ShatteredTiles3DDemo
--------------------------------------
local function ShatteredTiles3DDemo(t)
    return ax.ShatteredTiles3D:create(t, ax.size(16,12), 5, false)
end

--------------------------------------
--  ShuffleTilesDemo
--------------------------------------
local function ShuffleTilesDemo(t)
    local shuffle = ax.ShuffleTiles:create(t, ax.size(16,12), 25)
    local shuffle_back = shuffle:reverse()
    local delay = ax.DelayTime:create(2)

    return ax.Sequence:create(shuffle, shuffle_back, delay)
end

--------------------------------------
--  FadeOutTRTilesDemo
--------------------------------------
local function FadeOutTRTilesDemo(t)
    local fadeout = ax.FadeOutTRTiles:create(t, ax.size(16,12))
    local back = fadeout:reverse()
    local delay = ax.DelayTime:create(0.5)

    return ax.Sequence:create(fadeout, back, delay)
end

--------------------------------------
--  FadeOutBLTilesDemo
--------------------------------------
local function FadeOutBLTilesDemo(t)
    local fadeout = ax.FadeOutBLTiles:create(t, ax.size(16,12))
    local back = fadeout:reverse()
    local delay = ax.DelayTime:create(0.5)

    return ax.Sequence:create(fadeout, back, delay)
end

--------------------------------------
--  FadeOutUpTilesDemo
--------------------------------------
local function FadeOutUpTilesDemo(t)
    local fadeout = ax.FadeOutUpTiles:create(t, ax.size(16,12))
    local back = fadeout:reverse()
    local delay = ax.DelayTime:create(0.5)

    return ax.Sequence:create(fadeout, back, delay)
end

--------------------------------------
--  FadeOutDownTilesDemo
--------------------------------------
local function FadeOutDownTilesDemo(t)
    local fadeout = ax.FadeOutDownTiles:create(t, ax.size(16,12))
    local back = fadeout:reverse()
    local delay = ax.DelayTime:create(0.5)

    return ax.Sequence:create(fadeout, back, delay)
end

--------------------------------------
--  TurnOffTilesDemo
--------------------------------------
local function TurnOffTilesDemo(t)
    local fadeout = ax.TurnOffTiles:create(t, ax.size(48,32), 25)
    local back = fadeout:reverse()
    local delay = ax.DelayTime:create(0.5)

    return ax.Sequence:create(fadeout, back, delay)
end

--------------------------------------
--  WavesTiles3DDemo
--------------------------------------
local function WavesTiles3DDemo(t)
    return ax.WavesTiles3D:create(t, ax.size(15,10), 4, 120)
end

--------------------------------------
--  JumpTiles3DDemo
--------------------------------------
local function JumpTiles3DDemo(t)
    return ax.JumpTiles3D:create(t, ax.size(15,10), 2, 30)
end

--------------------------------------
--  SplitRowsDemo
--------------------------------------
local function SplitRowsDemo(t)
    return ax.SplitRows:create(t, 9)
end

--------------------------------------
--  SplitColsDemo
--------------------------------------
local function SplitColsDemo(t)
    return ax.SplitCols:create(t, 9)
end

--------------------------------------
--  PageTurn3DDemo
--------------------------------------
local function PageTurn3DDemo(t)
    ax.Director:getInstance():getRenderer():setDepthTest(true)
    return ax.PageTurn3D:create(t, ax.size(15,10))
end

--------------------------------------
--  Effects Test
--------------------------------------
local function createEffect(idx, t)
    ax.Director:getInstance():getRenderer():setDepthTest(false)
    local action = nil

    if idx == 0 then
        action = Shaky3DDemo(t)
    elseif idx == 1 then
        action = Waves3DDemo(t)
    elseif idx == 2 then
        action = FlipX3DDemo(t)
    elseif idx == 3 then
        action = FlipY3DDemo(t)
    elseif idx == 4 then
        action = Lens3DDemo(t)
    elseif idx == 5 then
        action = Ripple3DDemo(t)
    elseif idx == 6 then
        action = LiquidDemo(t)
    elseif idx == 7 then
        action = WavesDemo(t)
    elseif idx == 8 then
        action = TwirlDemo(t)
    elseif idx == 9 then
        action = ShakyTiles3DDemo(t)
    elseif idx == 10 then
        action = ShatteredTiles3DDemo(t)
    elseif idx == 11 then
        action = ShuffleTilesDemo(t)
    elseif idx == 12 then
        action = FadeOutTRTilesDemo(t)
    elseif idx == 13 then
        action = FadeOutBLTilesDemo(t)
    elseif idx == 14 then
        action = FadeOutUpTilesDemo(t)
    elseif idx == 15 then
        action = FadeOutDownTilesDemo(t)
    elseif idx == 16 then
        action = TurnOffTilesDemo(t)
    elseif idx == 17 then
        action = WavesTiles3DDemo(t)
    elseif idx == 18 then
        action = JumpTiles3DDemo(t)
    elseif idx == 19 then
        action = SplitRowsDemo(t)
    elseif idx == 20 then
        action = SplitColsDemo(t)
    elseif idx == 21 then
        action = PageTurn3DDemo(t)
    end

    return action
end

function CreateEffectsTestLayer()
    testLayer = ax.LayerColor:create(ax.color32(32,128,32,255))


    gridNodeTarget = ax.NodeGrid:create()
    local effect = createEffect(ActionIdx, 3)
    gridNodeTarget:runAction(effect)
    testLayer:addChild(gridNodeTarget, 0, kTagBackground)

    local bg = ax.Sprite:create(s_back3)
    gridNodeTarget:addChild(bg, 0)
    bg:setPosition(VisibleRect:center())

    local grossini = ax.Sprite:create(s_pPathSister2)
    gridNodeTarget:addChild(grossini, 1)
    grossini:setPosition( ax.p(VisibleRect:left().x+VisibleRect:getVisibleRect().width/3,VisibleRect:center().y) )
    local sc = ax.ScaleBy:create(2, 5)
    local sc_back = sc:reverse()
    grossini:runAction( ax.RepeatForever:create(ax.Sequence:create(sc, sc_back) ) )

    local tamara = ax.Sprite:create(s_pPathSister1)
    gridNodeTarget:addChild(tamara, 1)
    tamara:setPosition( ax.p(VisibleRect:left().x+2*VisibleRect:getVisibleRect().width/3,VisibleRect:center().y) )
    local sc2 = ax.ScaleBy:create(2, 5)
    local sc2_back = sc2:reverse()
    tamara:runAction( ax.RepeatForever:create(ax.Sequence:create(sc2, sc2_back)) )

    local x, y = size.width, size.height

    titleLabel = ax.Label:createWithTTF(EffectsList[ActionIdx], s_markerFeltFontPath, 32)
    titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    titleLabel:setPosition(x / 2, y - 80)
    testLayer:addChild(titleLabel)
    titleLabel:setTag(kTagLabel)

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
    item1:setPosition(ax.p(size.width/2 - item2:getContentSize().width * 2, item2:getContentSize().height / 2))
    item2:setPosition(ax.p(size.width/2, item2:getContentSize().height / 2))
    item3:setPosition(ax.p(size.width/2 + item2:getContentSize().width * 2, item2:getContentSize().height / 2))

    testLayer:addChild(menu, 1)

    testLayer:registerScriptHandler(onEnterOrExit)

    return testLayer
end

function EffectsTest()
    local scene = ax.Scene:create()

    ActionIdx = -1
    scene:addChild(nextAction())
    scene:addChild(CreateBackMenuItem())

    return scene
end
