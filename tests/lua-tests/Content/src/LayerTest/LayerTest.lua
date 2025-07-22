local scheduler = ax.Director:getInstance():getScheduler()
local kTagLayer = 1

local function createLayerDemoLayer(title, subtitle)
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    local titleStr = title == nil and "No title" or title
    local subTitleStr = subtitle  == nil and "" or subtitle
    Helper.titleLabel:setString(titleStr)
    Helper.subtitleLabel:setString(subTitleStr)

    -- local prev = {x = 0, y = 0}
    -- local function onTouchEvent(eventType, x, y)
    --     if eventType == "began" then
    --         prev.x = x
    --         prev.y = y
    --         return true
    --     elseif  eventType == "moved" then
    --         local node  = layer:getChildByTag(kTagTileMap)
    --         local newX  = node:getPositionX()
    --         local newY  = node:getPositionY()
    --         local diffX = x - prev.x
    --         local diffY = y - prev.y

    --         node:setPosition( ax.p.__add(ax.p(newX, newY), ax.p(diffX, diffY)) )
    --         prev.x = x
    --         prev.y = y
    --     end
    -- end
    -- layer:setTouchEnabled(true)
    -- layer:registerScriptTouchHandler(onTouchEvent)
    return layer
end

-- Cascading support extensions

local function setEnableRecursiveCascading(node, enable)
    if node == nil then
        -- cclog("node == nil, return directly")
        return
    end

    if node.setCascadeColorEnabled ~= nil and node.setCascadeOpacityEnabled ~= nil then
        node:setCascadeColorEnabled(enable)
        node:setCascadeOpacityEnabled(enable)
    end

    local obj = nil
    local children = node:getChildren()
    if children == nil then
        -- cclog("children is nil")
        print("children is nil")
        return
    end

    local i = 0
    local len = #(children)
    for i = 0, len-1, 1 do
        setEnableRecursiveCascading(children[i + 1], enable)
    end
end

-- LayerTestCascadingOpacityA
local function LayerTestCascadingOpacityA()
    local ret = createLayerDemoLayer("Layer: cascading opacity")
    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.Layer:create()

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt", "Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, s.height/2))
    sister2:setPosition( ax.p( s.width*2/3, s.height/2))
    label:setPosition( ax.p( s.width/2, s.height/2))

    layer1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(4, 0),
                                             ax.FadeTo:create(4, 255),
                                             ax.DelayTime:create(1)
    )))


    sister1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(2, 0),
                                              ax.FadeTo:create(2, 255),
                                              ax.FadeTo:create(2, 0),
                                              ax.FadeTo:create(2, 255),
                                              ax.DelayTime:create(1)
                                              )))

    -- Enable cascading in scene
    setEnableRecursiveCascading(ret, true)
    return ret
end

-- LayerTestCascadingOpacityB
local function LayerTestCascadingOpacityB()
    local ret = createLayerDemoLayer("CCLayerColor: cascading opacity")

    local s = ax.Director:getInstance():getWinSize()
    local layer1 = ax.LayerColor:create(ax.color32(192, 0, 0, 255), s.width, s.height/2)
    layer1:setCascadeColorEnabled(false)

    layer1:setPosition( ax.p(0, s.height/2))

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt","Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, 0))
    sister2:setPosition( ax.p( s.width*2/3, 0))
    label:setPosition( ax.p( s.width/2, 0))

    layer1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(4, 0),ax.FadeTo:create(4, 255),ax.DelayTime:create(1))))

    sister1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(2, 0),ax.FadeTo:create(2, 255),ax.FadeTo:create(2, 0),ax.FadeTo:create(2, 255),ax.DelayTime:create(1))))

    -- Enable cascading in scene
    setEnableRecursiveCascading(ret, true)
    return ret
end

-- LayerTestCascadingOpacityC
local function LayerTestCascadingOpacityC()
    local ret = createLayerDemoLayer("LayerColor: non-cascading opacity")

    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.LayerColor:create(ax.color32(192, 0, 0, 255), s.width, s.height/2)
    layer1:setCascadeColorEnabled(false)
    layer1:setCascadeOpacityEnabled(false)

    layer1:setPosition( ax.p(0, s.height/2))

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt","Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, 0))
    sister2:setPosition( ax.p( s.width*2/3, 0))
    label:setPosition( ax.p( s.width/2, 0))

    layer1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(4, 0),
                    ax.FadeTo:create(4, 255),ax.DelayTime:create(1))))

    sister1:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(2, 0),ax.FadeTo:create(2, 255),
                                              ax.FadeTo:create(2, 0),ax.FadeTo:create(2, 255),ax.DelayTime:create(1))))
    return ret
end

--// Example LayerTestCascadingColor

-- LayerTestCascadingColorA
local function LayerTestCascadingColorA()
    local ret = createLayerDemoLayer("Layer: cascading color")

    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.Layer:create()

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt","Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, s.height/2))
    sister2:setPosition( ax.p( s.width*2/3, s.height/2))
    label:setPosition( ax.p( s.width/2, s.height/2))

    layer1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(6, 255, 0, 255),
                         ax.TintTo:create(6, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))


    sister1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(2, 255, 255, 0),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 0, 255, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 255, 0, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))

    -- Enable cascading in scene
    setEnableRecursiveCascading(ret, true)
    return ret
end

-- LayerTestCascadingColorB
local function LayerTestCascadingColorB()
    local ret = createLayerDemoLayer("LayerColor: cascading color")

    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.LayerColor:create(ax.color32(255, 255, 255, 255), s.width, s.height/2)

    layer1:setPosition( ax.p(0, s.height/2))

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt","Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, 0))
    sister2:setPosition( ax.p( s.width*2/3, 0))
    label:setPosition( ax.p( s.width/2, 0))

    layer1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(6, 255, 0, 255),
                         ax.TintTo:create(6, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))

    sister1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(2, 255, 255, 0),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 0, 255, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 255, 0, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))

    -- Enable cascading in scene
    setEnableRecursiveCascading(ret, true)
    return ret
end

-- LayerTestCascadingColorC
local function LayerTestCascadingColorC()
    local ret = createLayerDemoLayer("LayerColor: non-cascading color")

    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.LayerColor:create(ax.color32(255, 255, 255, 255), s.width, s.height/2)
    layer1:setCascadeColorEnabled(false)
    layer1:setPosition( ax.p(0, s.height/2))

    local sister1 = ax.Sprite:create("Images/grossinis_sister1.png")
    local sister2 = ax.Sprite:create("Images/grossinis_sister2.png")
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt","Test")
    label:setAnchorPoint(ax.p(0.5, 0.5))

    layer1:addChild(sister1)
    layer1:addChild(sister2)
    layer1:addChild(label)
    ret:addChild( layer1, 0, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, 0))
    sister2:setPosition( ax.p( s.width*2/3, 0))
    label:setPosition( ax.p( s.width/2, 0))

    layer1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(6, 255, 0, 255),
                         ax.TintTo:create(6, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))

    sister1:runAction(
     ax.RepeatForever:create(
      ax.Sequence:create(ax.TintTo:create(2, 255, 255, 0),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 0, 255, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.TintTo:create(2, 255, 0, 255),
                         ax.TintTo:create(2, 255, 255, 255),
                         ax.DelayTime:create(1)
       )))
    return ret
end

--------------------------------------------------------------------
--
-- LayerTest1
--
--------------------------------------------------------------------
local function LayerTest1()
    local ret = createLayerDemoLayer("ColorLayer resize (tap & move)")

    local s = ax.Director:getInstance():getWinSize()
    local  layer = ax.LayerColor:create( ax.color32(0xFF, 0x00, 0x00, 0x80), 200, 200)

    layer:setIgnoreAnchorPointForPosition(false)
    layer:setPosition( ax.p(s.width/2, s.height/2) )
    ret:addChild(layer, 1, kTagLayer)

    local function updateSize(x, y)
        local s = ax.Director:getInstance():getWinSize()

        local newSize = ax.size( math.abs(x - s.width/2)*2, math.abs(y - s.height/2)*2)

        local  l = ret:getChildByTag(kTagLayer)

        l:setContentSize( newSize )
    end

    local function onTouchesMoved(touches, event)
        local touchLocation = touches[1]:getLocation()

        updateSize(touchLocation.x, touchLocation.y)
    end

    local function onTouchesBegan(touches, event)
        onTouchesMoved(touches, event)
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )

    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, ret)

    return ret
end

--------------------------------------------------------------------
--
-- LayerTest2
--
--------------------------------------------------------------------
local function LayerTest2()
    local ret = createLayerDemoLayer("ColorLayer: fade and tint")

    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.LayerColor:create( ax.color32(255, 255, 0, 80), 100, 300)
    layer1:setPosition(ax.p(s.width/3, s.height/2))
    layer1:setIgnoreAnchorPointForPosition(false)
    ret:addChild(layer1, 1)

    local  layer2 = ax.LayerColor:create( ax.color32(0, 0, 255, 255), 100, 300)
    layer2:setPosition(ax.p((s.width/3)*2, s.height/2))
    layer2:setIgnoreAnchorPointForPosition(false)
    ret:addChild(layer2, 1)

    local  actionTint = ax.TintBy:create(2, -255, -127, 0)
    local  actionTintBack = actionTint:reverse()
    local  seq1 = ax.Sequence:create(actionTint,actionTintBack)
    layer1:runAction(seq1)

    local  actionFade = ax.FadeOut:create(2.0)
    local  actionFadeBack = actionFade:reverse()
    local  seq2 = ax.Sequence:create(actionFade,actionFadeBack)
    layer2:runAction(seq2)

    return ret
end

--------------------------------------------------------------------
--
-- LayerTestBlend
--
--------------------------------------------------------------------

local function LayerTestBlend()
    local ret = createLayerDemoLayer("ColorLayer: blend")
    local s = ax.Director:getInstance():getWinSize()
    local  layer1 = ax.LayerColor:create( ax.color32(255, 255, 255, 80) )

    local  sister1 = ax.Sprite:create(s_pPathSister1)
    local  sister2 = ax.Sprite:create(s_pPathSister2)

    ret:addChild(sister1)
    ret:addChild(sister2)
    ret:addChild(layer1, 100, kTagLayer)

    sister1:setPosition( ax.p( s.width*1/3, s.height/2) )
    sister2:setPosition( ax.p( s.width*2/3, s.height/2) )

    local blend = true

    local function newBlend(dt)
        local layer = ret:getChildByTag(kTagLayer)

        local src = 0
        local dst = 0

        if  blend  then
            src = ccb.BlendFactor.SRC_ALPHA
            dst = ccb.BlendFactor.ONE_MINUS_SRC_ALPHA
        else
            src = ccb.BlendFactor.ONE_MINUS_DST_COLOR
            dst = ccb.BlendFactor.ZERO
        end

        layer:setBlendFunc(ax.blendFunc(src, dst))
        blend = not blend
    end


    local schedulerEntry = nil
    local function onNodeEvent(event)
        if event == "enter" then
            schedulerEntry = scheduler:scheduleScriptFunc(newBlend, 1.0, false)
        elseif event == "exit" then
            scheduler:unscheduleScriptEntry(schedulerEntry)
        end
    end

    ret:registerScriptHandler(onNodeEvent)
    return ret
end

--------------------------------------------------------------------
--
-- LayerGradient
--
--------------------------------------------------------------------
local function LayerGradient()
    local ret = createLayerDemoLayer("LayerGradient", "Touch the screen and move your finger")
    local  layer1 = ax.LayerGradient:create(ax.color32(255,0,0,255), ax.color32(0,255,0,255), ax.p(0.9, 0.9))
    ret:addChild(layer1, 0, kTagLayer)

    local label1 = ax.Label:createWithTTF("Compressed Interpolation: Enabled", s_markerFeltFontPath, 26)
    label1:setAnchorPoint(ax.p(0.5, 0.5))
    local label2 = ax.Label:createWithTTF("Compressed Interpolation: Disabled", s_markerFeltFontPath, 26)
    label2:setAnchorPoint(ax.p(0.5, 0.5))
    local item1 = ax.MenuItemLabel:create(label1)
    local item2 = ax.MenuItemLabel:create(label2)
    local item = ax.MenuItemToggle:create(item1)
    item:addSubItem(item2)

    local function toggleItem(sender)
        -- cclog("toggleItem")
        local gradient = ret:getChildByTag(kTagLayer)
        gradient:setCompressedInterpolation(not gradient:isCompressedInterpolation())
    end

    item:registerScriptTapHandler(toggleItem)

    local menu = ax.Menu:create(item)
    ret:addChild(menu)
    local s = ax.Director:getInstance():getWinSize()
    menu:setPosition(ax.p(s.width / 2, 100))

    local function onTouchesMoved(touches, event)
        local s = ax.Director:getInstance():getWinSize()
        local start = touches[1]:getLocation()
        local movingPos = ax.p(s.width/2,s.height/2)
        local diff = ax.p(movingPos.x - start.x, movingPos.y - start.y)
        diff = ax.pNormalize(diff)

        local gradient = ret:getChildByTag(1)
        gradient:setVector(diff)
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )

    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, ret)

    return ret
end


-- LayerIgnoreAnchorPointPos

local kLayerIgnoreAnchorPoint = 1000

local function LayerIgnoreAnchorPointPos()
    local ret = createLayerDemoLayer("IgnoreAnchorPoint - Position", "Ignoring Anchor Point for position")

    local s = ax.Director:getInstance():getWinSize()

    local l = ax.LayerColor:create(ax.color32(255, 0, 0, 255), 150, 150)

    l:setAnchorPoint(ax.p(0.5, 0.5))
    l:setPosition(ax.p( s.width/2, s.height/2))

    local move = ax.MoveBy:create(2, ax.p(100,2))
    local  back = move:reverse()
    local seq = ax.Sequence:create(move, back)
    l:runAction(ax.RepeatForever:create(seq))
    ret:addChild(l, 0, kLayerIgnoreAnchorPoint)

    local child = ax.Sprite:create("Images/grossini.png")
    l:addChild(child)
    local lsize = l:getContentSize()
    child:setPosition(ax.p(lsize.width/2, lsize.height/2))

    local function onToggle(pObject)
        local  pLayer = ret:getChildByTag(kLayerIgnoreAnchorPoint)
        local ignore = pLayer:isIgnoreAnchorPointForPosition()
        pLayer:setIgnoreAnchorPointForPosition(not ignore)
    end

    local item = ax.MenuItemFont:create("Toggle ignore anchor point")
    item:registerScriptTapHandler(onToggle)

    local menu = ax.Menu:create(item)
    ret:addChild(menu)

    menu:setPosition(ax.p(s.width/2, s.height/2))
    return ret
end

-- LayerIgnoreAnchorPointRot

local function LayerIgnoreAnchorPointRot()
    local ret = createLayerDemoLayer("IgnoreAnchorPoint - Rotation", "Ignoring Anchor Point for rotations")

    local s = ax.Director:getInstance():getWinSize()

    local l = ax.LayerColor:create(ax.color32(255, 0, 0, 255), 200, 200)

    l:setAnchorPoint(ax.p(0.5, 0.5))
    l:setPosition(ax.p( s.width/2, s.height/2))

    ret:addChild(l, 0, kLayerIgnoreAnchorPoint)

    local rot = ax.RotateBy:create(2, 360)
    l:runAction(ax.RepeatForever:create(rot))


    local child = ax.Sprite:create("Images/grossini.png")
    l:addChild(child)
    local lsize = l:getContentSize()
    child:setPosition(ax.p(lsize.width/2, lsize.height/2))

    local function onToggle(pObject)
        local  pLayer = ret:getChildByTag(kLayerIgnoreAnchorPoint)
        local ignore = pLayer:isIgnoreAnchorPointForPosition()
        pLayer:setIgnoreAnchorPointForPosition(not ignore)
    end

    local item = ax.MenuItemFont:create("Toggle ignore anchor point")
    item:registerScriptTapHandler(onToggle)

    local menu = ax.Menu:create(item)
    ret:addChild(menu)

    menu:setPosition(ax.p(s.width/2, s.height/2))
    return ret
end

-- LayerIgnoreAnchorPointScale
local function LayerIgnoreAnchorPointScale()
    local ret = createLayerDemoLayer("IgnoreAnchorPoint - Scale", "Ignoring Anchor Point for scale")
    local s = ax.Director:getInstance():getWinSize()

    local l = ax.LayerColor:create(ax.color32(255, 0, 0, 255), 200, 200)

    l:setAnchorPoint(ax.p(0.5, 1.0))
    l:setPosition(ax.p( s.width/2, s.height/2))


    local scale = ax.ScaleBy:create(2, 2)
    local  back = scale:reverse()
    local seq = ax.Sequence:create(scale, back)

    l:runAction(ax.RepeatForever:create(seq))

    ret:addChild(l, 0, kLayerIgnoreAnchorPoint)

    local child = ax.Sprite:create("Images/grossini.png")
    l:addChild(child)
    local lsize = l:getContentSize()
    child:setPosition(ax.p(lsize.width/2, lsize.height/2))

    local function onToggle(pObject)
        local  pLayer = ret:getChildByTag(kLayerIgnoreAnchorPoint)
        local ignore = pLayer:isIgnoreAnchorPointForPosition()
        pLayer:setIgnoreAnchorPointForPosition(not ignore)
        return ret
    end

    local item = ax.MenuItemFont:create("Toggle ignore anchor point")
    item:registerScriptTapHandler(onToggle)

    local menu = ax.Menu:create(item)
    ret:addChild(menu)

    menu:setPosition(ax.p(s.width/2, s.height/2))
    return ret
end


local function LayerExtendedBlendOpacityTest()
    local ret = createLayerDemoLayer("Extended Blend & Opacity", "You should see 3 layers")
    local  layer1 = ax.LayerGradient:create(ax.color32(255, 0, 0, 255), ax.color32(255, 0, 255, 255))
    layer1:setContentSize(ax.size(80, 80))
    layer1:setPosition(ax.p(50,50))
    ret:addChild(layer1)

    local  layer2 = ax.LayerGradient:create(ax.color32(0, 0, 0, 127), ax.color32(255, 255, 255, 127))
    layer2:setContentSize(ax.size(80, 80))
    layer2:setPosition(ax.p(100,90))
    ret:addChild(layer2)

    local  layer3 = ax.LayerGradient:create()
    layer3:setContentSize(ax.size(80, 80))
    layer3:setPosition(ax.p(150,140))
    layer3:setStartColor(ax.color32(255, 0, 0))
    layer3:setEndColor(ax.color32(255, 0, 255))
    layer3:setStartOpacity(255)
    layer3:setEndOpacity(255)
    layer3:setBlendFunc(ax.blendFunc(ccb.BlendFactor.SRC_ALPHA, ccb.BlendFactor.ONE_MINUS_SRC_ALPHA))
    ret:addChild(layer3)
    return ret
end

function LayerTestMain()
    cclog("LayerTestMain")
    Helper.index = 1
    local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        LayerTestCascadingOpacityA,
        LayerTestCascadingOpacityB,
        LayerTestCascadingOpacityC,
        LayerTestCascadingColorA,
        LayerTestCascadingColorB,
        LayerTestCascadingColorC,
        LayerTest1,
        LayerTest2,
        LayerTestBlend,
        LayerGradient,
        LayerIgnoreAnchorPointPos,
        LayerIgnoreAnchorPointRot,
        LayerIgnoreAnchorPointScale,
        LayerExtendedBlendOpacityTest
    }
    Helper.index = 1

    scene:addChild(LayerTestCascadingOpacityA())
    scene:addChild(CreateBackMenuItem())
    return scene
end
