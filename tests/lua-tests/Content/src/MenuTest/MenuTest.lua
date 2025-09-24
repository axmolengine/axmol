local    kTagMenu = 1
local    kTagMenu0 = 0
local    kTagMenu1 = 1

local MID_CALLBACK     = 1000
local MID_CALLBACK2    = 1001
local MID_DISABLED     = 1002
local MID_ENABLE       = 1003
local MID_CONFIG       = 1004
local MID_QUIT         = 1005
local MID_OPACITY      = 1006
local MID_ALIGN        = 1007
local MID_CALLBACK3    = 1008
local MID_BACKCALLBACK = 1009

--------------------------------------------------------------------
--
-- MenuLayerMainMenu
--
--------------------------------------------------------------------
local function MenuLayerMainMenu()
    local m_disabledItem = nil

    local ret = ax.Layer:create()

    -- Font Item
    local  spriteNormal = ax.Sprite:create(s_MenuItem, ax.rect(0,23*2,115,23))
    local  spriteSelected = ax.Sprite:create(s_MenuItem, ax.rect(0,23*1,115,23))
    local  spriteDisabled = ax.Sprite:create(s_MenuItem, ax.rect(0,23*0,115,23))

    local  item1 = ax.MenuItemSprite:create(spriteNormal, spriteSelected, spriteDisabled)

    local function menuCallback(sender)
        cclog("menuCallback...")
        ret:getParent():switchTo(1, false)
    end

    item1:registerScriptTapHandler(menuCallback)
    -- Image Item
    local function menuCallback2(sender)
        ret:getParent():switchTo(2, false)
    end

    local  item2 = ax.MenuItemImage:create(s_SendScore, s_PressSendScore)
    item2:registerScriptTapHandler(menuCallback2)


    local schedulerEntry = nil
    local scheduler = ax.Director:getInstance():getScheduler()
    local function allowTouches(dt)
        local  pDirector = ax.Director:getInstance()
        --pDirector:getTouchDispatcher():setPriority(ax.MENU_HANDLER_PRIORITY +1, ret)
        if nil ~=  schedulerEntry then
            scheduler:unscheduleScriptEntry(schedulerEntry)
            schedulerEntry = nil
        end
        cclog("TOUCHES ALLOWED AGAIN")
    end


    local function menuCallbackDisabled(sender)
        -- hijack all touch events for 5 seconds
        local  pDirector = ax.Director:getInstance()
        --pDirector:getTouchDispatcher():setPriority(ax.MENU_HANDLER_PRIORITY -1, ret)
        schedulerEntry = scheduler:scheduleScriptFunc(allowTouches, 5, false)
        cclog("TOUCHES DISABLED FOR 5 SECONDS")
    end

    -- Label Item (LabelAtlas)
    local  labelAtlas = ax.LabelAtlas:_create("0123456789", "fonts/labelatlas.png", 16, 24, string.byte('.'))
    local  item3 = ax.MenuItemLabel:create(labelAtlas)
    item3:registerScriptTapHandler(menuCallbackDisabled)
    item3:setDisabledColor( ax.color32(32,32,64) )
    item3:setColor( ax.color32(200,200,255) )

    local function menuCallbackEnable(sender)
        m_disabledItem:setEnabled(not m_disabledItem:isEnabled() )
    end

    -- Font Item
    local item4 = ax.MenuItemFont:create("I toggle enable items")
    item4:registerScriptTapHandler(menuCallbackEnable)

    item4:setFontSizeObj(20)
    ax.MenuItemFont:setFontName("Marker Felt")

    local function menuCallbackConfig(sender)
        ret:getParent():switchTo(3, false)
    end

    -- Label Item (ax.LabelBMFont)
    local  label = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "configuration")
    label:setAnchorPoint(ax.p(0.5, 0.5))
    local  item5 = ax.MenuItemLabel:create(label)
    item5:registerScriptTapHandler(menuCallbackConfig)

    -- Testing issue #500
    item5:setScale( 0.8 )

    -- Events
    ax.MenuItemFont:setFontName("Marker Felt")
    local function menuCallbackBugsTest(pSender)
        ret:getParent():switchTo(4, false)
    end

    -- Bugs Item
    local item6 = ax.MenuItemFont:create("Bugs")
    item6:registerScriptTapHandler(menuCallbackBugsTest)

    local function onQuit(sender)
        cclog("onQuit item is clicked.")
    end

    -- Font Item
    local  item7 = ax.MenuItemFont:create("Quit")
    item7:registerScriptTapHandler(onQuit)

    local function menuMovingCallback(pSender)
        ret:getParent():switchTo(5, false)
    end

    local  item8 = ax.MenuItemFont:create("Remove menu item when moving")
    item8:registerScriptTapHandler(menuMovingCallback)

    local  color_action = ax.TintBy:create(0.5, 0, -255, -255)
    local  color_back = color_action:reverse()
    local  seq = ax.Sequence:create(color_action, color_back)
    item7:runAction(ax.RepeatForever:create(seq))

    local  menu = ax.Menu:create()

    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:addChild(item4)
    menu:addChild(item5)
    menu:addChild(item6)
    menu:addChild(item7)
    menu:addChild(item8)

    menu:alignItemsVertically()

    -- elastic effect
    local s = ax.Director:getInstance():getCanvasSize()

    local i        = 0
    local child    = nil
    local pArray   = menu:getChildren()
    local len      = #(pArray)
    local pObject  = nil
    for i = 0, len-1 do
        pObject = pArray[i + 1]
        if pObject == nil then
            break
        end
        child = pObject
        local dstPointX, dstPointY = child:getPosition()
        local offset = s.width/2 + 50
        if  i % 2 == 0 then
            offset = 0-offset
        end
        child:setPosition( ax.p( dstPointX + offset, dstPointY) )
        child:runAction( ax.EaseElasticOut:create(ax.MoveBy:create(2, ax.p(dstPointX - offset,0)), 0.35) )
    end

    m_disabledItem = item3
    item3:retain()

    m_disabledItem:setEnabled( false )

    ret:addChild(menu)
    menu:setPosition(ax.p(s.width/2, s.height/2))

--  local schedulerEntry = nil
    local function onNodeEvent(event)
        if event == "exit" then
            if (schedulerEntry ~= nil) then
                scheduler:unscheduleScriptEntry(schedulerEntry)
            end
            if m_disabledItem ~= nil then
--                m_disabledItem:release()
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end

--------------------------------------------------------------------
--
-- MenuLayer2
--
--------------------------------------------------------------------
local function MenuLayer2()
    local ret = ax.Layer:create()
    local m_centeredMenu = nil
    local m_alignedH = false

    local function alignMenusH()
        local i = 0
        for i=0, 1 do
            local menu = ret:getChildByTag(100+i)
            menu:setPosition( m_centeredMenu )
            if i==0 then
                -- TIP: if no padding, padding = 5
                menu:alignItemsHorizontally()
                local x, y = menu:getPosition()
                menu:setPosition( ax.pAdd(ax.p(x, y), ax.p(0,30)) )
            else
                -- TIP: but padding is configurable
                menu:alignItemsHorizontallyWithPadding(40)
                local x, y = menu:getPosition()
                menu:setPosition( ax.pSub(ax.p(x, y), ax.p(0,30)) )
            end
        end
    end

    local function alignMenusV()
        local i = 0
        for i=0, 1 do
            local menu = ret:getChildByTag(100+i)
            menu:setPosition( m_centeredMenu )
            if i==0 then
                -- TIP: if no padding, padding = 5
                menu:alignItemsVertically()
                local x, y = menu:getPosition()
                menu:setPosition( ax.pAdd(ax.p(x, y), ax.p(100,0)) )
            else
                -- TIP: but padding is configurable
                menu:alignItemsVerticallyWithPadding(40)
                local x, y = menu:getPosition()
                menu:setPosition( ax.pSub(ax.p(x, y), ax.p(100,0)) )
            end
        end
    end

    local function menuCallback(sender)
        ret:getParent():switchTo(0, false)
    end

    local function menuCallbackOpacity(tag, sender)
        local menu = sender:getParent()
        local opacity = menu:getOpacity()
        if opacity == 128 then
            menu:setOpacity(255)
        else
            menu:setOpacity(128)
        end
    end

    local function menuCallbackAlign(sender)
        m_alignedH = not m_alignedH

        if m_alignedH then
            alignMenusH()
        else
            alignMenusV()
        end
    end

    local i = 0
    for i=0, 1 do
        local  item1 = ax.MenuItemImage:create(s_PlayNormal, s_PlaySelect)
        item1:registerScriptTapHandler(menuCallback)

        local  item2 = ax.MenuItemImage:create(s_HighNormal, s_HighSelect)
        item2:registerScriptTapHandler(menuCallbackOpacity)

        local  item3 = ax.MenuItemImage:create(s_AboutNormal, s_AboutSelect)
        item3:registerScriptTapHandler(menuCallbackAlign)

        item1:setScaleX( 1.5 )
        item2:setScaleX( 0.5 )
        item3:setScaleX( 0.5 )

        local  menu = ax.Menu:create()

        menu:addChild(item1)
        menu:addChild(item2)
        menu:addChild(item3)

        local s = ax.Director:getInstance():getCanvasSize()
        menu:setPosition(ax.p(s.width/2, s.height/2))

        menu:setTag( kTagMenu )

        ret:addChild(menu, 0, 100+i)
        local x, y = menu:getPosition()
        m_centeredMenu = ax.p(x, y)
    end

    m_alignedH = true
    alignMenusH()

    return ret
end

--------------------------------------------------------------------
--
-- MenuLayer3
--
--------------------------------------------------------------------
local function MenuLayer3()
    local m_disabledItem = nil
    local ret = ax.Layer:create()
    local function menuCallback(sender)
        ret:getParent():switchTo(0, false)
    end

    local function menuCallback2(sender)
        cclog("Label clicked. Toggling AtlasSprite")
        m_disabledItem:setEnabled( not m_disabledItem:isEnabled() )
        m_disabledItem:stopAllActions()
    end

    local function menuCallback3(sender)
        cclog("MenuItemSprite clicked")
    end

    ax.MenuItemFont:setFontName("Marker Felt")
    ax.MenuItemFont:setFontSize(28)

    local  label = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "Enable AtlasItem")
    label:setAnchorPoint(ax.p(0.5, 0.5))
    local  item1 = ax.MenuItemLabel:create(label)
    item1:registerScriptTapHandler(menuCallback2)

    local  item2 = ax.MenuItemFont:create("--- Go Back ---")
    item2:registerScriptTapHandler(menuCallback)

    local spriteNormal   = ax.Sprite:create(s_MenuItem,  ax.rect(0,23*2,115,23))
    local spriteSelected = ax.Sprite:create(s_MenuItem,  ax.rect(0,23*1,115,23))
    local spriteDisabled = ax.Sprite:create(s_MenuItem,  ax.rect(0,23*0,115,23))


    local  item3 = ax.MenuItemSprite:create(spriteNormal, spriteSelected, spriteDisabled)
    item3:registerScriptTapHandler(menuCallback3)
    m_disabledItem = item3
    item3:retain()
    m_disabledItem:setEnabled( false )

    local menu = ax.Menu:create()

    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)

    menu:setPosition( ax.p(0,0) )

    local s = ax.Director:getInstance():getCanvasSize()

    item1:setPosition( ax.p(s.width/2 - 150, s.height/2) )
    item2:setPosition( ax.p(s.width/2 - 200, s.height/2) )
    item3:setPosition( ax.p(s.width/2, s.height/2 - 100) )

    local  jump = ax.JumpBy:create(3, ax.p(400,0), 50, 4)
    item2:runAction( ax.RepeatForever:create(ax.Sequence:create( jump, jump:reverse())))

    local  spin1 = ax.RotateBy:create(3, 360)
    local  spin2 = spin1:clone()
    local  spin3 = spin1:clone()

    item1:runAction( ax.RepeatForever:create(spin1) )
    item2:runAction( ax.RepeatForever:create(spin2) )
    item3:runAction( ax.RepeatForever:create(spin3) )

    ret:addChild( menu )

    menu:setPosition(ax.p(0,0))

    local function onNodeEvent(event)
        if event == "exit" then
            if m_disabledItem ~= nil then
                --m_disabledItem:release()
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end


--------------------------------------------------------------------
--
-- MenuLayer4
--
--------------------------------------------------------------------
local function MenuLayer4()
    local ret = ax.Layer:create()
    ax.MenuItemFont:setFontName("American Typewriter")
    ax.MenuItemFont:setFontSize(18)
    local title1 = ax.MenuItemFont:create("Sound")
    title1:setEnabled(false)
    ax.MenuItemFont:setFontName( "Marker Felt" )
    ax.MenuItemFont:setFontSize(34)
    local  item1 = ax.MenuItemToggle:create(ax.MenuItemFont:create( "On" ))

    local function menuCallback(tag, sender)
        cclog("selected item: tag: %d, index:%d", tag, sender:getSelectedIndex() )
    end

    local function backCallback(tag, sender)
        ret:getParent():switchTo(0, false)
    end

    item1:registerScriptTapHandler(menuCallback)
    item1:addSubItem(ax.MenuItemFont:create( "Off"))

    ax.MenuItemFont:setFontName( "American Typewriter" )
    ax.MenuItemFont:setFontSize(18)
    local  title2 = ax.MenuItemFont:create( "Music" )
    title2:setEnabled(false)
    ax.MenuItemFont:setFontName( "Marker Felt" )
    ax.MenuItemFont:setFontSize(34)
    local item2 = ax.MenuItemToggle:create(ax.MenuItemFont:create( "On" ))
    item2:registerScriptTapHandler(menuCallback)
    item2:addSubItem(ax.MenuItemFont:create( "Off"))

    ax.MenuItemFont:setFontName( "American Typewriter" )
    ax.MenuItemFont:setFontSize(18)
    local  title3 = ax.MenuItemFont:create( "Quality" )
    title3:setEnabled( false )
    ax.MenuItemFont:setFontName( "Marker Felt" )
    ax.MenuItemFont:setFontSize(34)
    local item3 = ax.MenuItemToggle:create(ax.MenuItemFont:create( "High" ))
    item3:registerScriptTapHandler(menuCallback)
    item3:addSubItem(ax.MenuItemFont:create( "Low" ))

    ax.MenuItemFont:setFontName( "American Typewriter" )
    ax.MenuItemFont:setFontSize(18)
    local  title4 = ax.MenuItemFont:create( "Orientation" )
    title4:setEnabled(false)
    ax.MenuItemFont:setFontName( "Marker Felt" )
    ax.MenuItemFont:setFontSize(34)
    local item4 = ax.MenuItemToggle:create(ax.MenuItemFont:create( "Off"),
                                          ax.MenuItemFont:create( "33%" ),
                                          ax.MenuItemFont:create( "66%" ),
                                          ax.MenuItemFont:create( "100%"))
    item4:registerScriptTapHandler(menuCallback)

    -- you can change the one of the items by doing this
    item4:setSelectedIndex( 2 )

    ax.MenuItemFont:setFontName( "Marker Felt" )
    ax.MenuItemFont:setFontSize( 34 )

    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "go back")
    label:setAnchorPoint(ax.p(0.5, 0.5))
    local  back = ax.MenuItemLabel:create(label)
    back:registerScriptTapHandler(backCallback)

    local menu = ax.Menu:create()

    menu:addChild(title1)
    menu:addChild(title2)
    menu:addChild(item1 )
    menu:addChild(item2 )
    menu:addChild(title3)
    menu:addChild(title4)
    menu:addChild(item3 )
    menu:addChild(item4 )
    menu:addChild(back  )

    menu:alignItemsInColumns(2, 2, 2, 2, 1)

    ret:addChild(menu)

    local s = ax.Director:getInstance():getCanvasSize()
    menu:setPosition(ax.p(s.width/2, s.height/2))
    return ret
end

-- BugsTest
local function BugsTest()
    local ret = ax.Layer:create()
    local function issue1410MenuCallback(tag, pSender)
        local menu = pSender:getParent()
        menu:setEnabled(false)
        menu:setEnabled(true)
        cclog("NO CRASHES")
    end

    local function issue1410v2MenuCallback(tag, pSender)
        local menu = pSender:getParent()
        menu:setEnabled(true)
        menu:setEnabled(false)
        cclog("NO CRASHES. AND MENU SHOULD STOP WORKING")
    end

    local function backMenuCallback(tag, pSender)
        ret:getParent():switchTo(0, false)
    end


    local issue1410 = ax.MenuItemFont:create("Issue 1410")
    issue1410:registerScriptTapHandler(issue1410MenuCallback)
    local issue1410_2 = ax.MenuItemFont:create("Issue 1410 #2")
    issue1410_2:registerScriptTapHandler(issue1410v2MenuCallback)
    local back = ax.MenuItemFont:create("Back")
    back:registerScriptTapHandler(backMenuCallback)

    local menu = ax.Menu:create()
    menu:addChild(issue1410)
    menu:addChild(issue1410_2)
    menu:addChild(back)
    ret:addChild(menu)
    menu:alignItemsVertically()

    local s = ax.Director:getInstance():getCanvasSize()
    menu:setPosition(ax.p(s.width/2, s.height/2))
    return ret
end


local function RemoveMenuItemWhenMove()
    local ret = ax.Layer:create()
    local s = ax.Director:getInstance():getCanvasSize()

    local  label = ax.Label:createWithTTF("click item and move, should not crash", s_arialPath, 20)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(ax.p(s.width/2, s.height - 30))
    ret:addChild(label)

    local item = ax.MenuItemFont:create("item 1")
    item:retain()

    local back = ax.MenuItemFont:create("go back")
    local function goBack(tag, pSender)
        ret:getParent():switchTo(0, false)
    end

    back:registerScriptTapHandler(goBack)

    local menu = ax.Menu:create()
    menu:addChild(item)
    menu:addChild(back)

    ret:addChild(menu)
    menu:alignItemsVertically()

    menu:setPosition(ax.p(s.width/2, s.height/2))

    local function onTouchBegan(touch, event)
        return true
    end

    local function onTouchMoved(touch, event)
        if item ~= nil then
            item:removeFromParent(true)
            --item:release()
            --item = nil
        end
    end

    local listener = ax.EventListenerTouchOneByOne:create()
    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCH_MOVED )
    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithFixedPriority(listener, -129)

    local function onNodeEvent(event)
        if event == "exit" then
           ret:getEventDispatcher():removeEventListener(listener)
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end

function MenuTestMain()
    cclog("MenuTestMain")
    local scene = ax.Scene:create()

    local  pLayer1 = MenuLayerMainMenu()
    local  pLayer2 = MenuLayer2()

    local  pLayer3 = MenuLayer3()
    local  pLayer4 = MenuLayer4()
    local  pLayer5 = BugsTest()
    local  pLayer6 = RemoveMenuItemWhenMove()

    local  layer = ax.LayerMultiplex:create(pLayer1,
                                            pLayer2,
                                            pLayer3,
                                            pLayer4,
                                            pLayer5,
                                            pLayer6 )

    scene:addChild(layer, 0)
    scene:addChild(CreateBackMenuItem())
    return scene
end
