local    MAX_COUNT = 9
local    LINE_SPACE = 40
local    kItemTagBasic = 5432

local Winsize = ax.Director:getInstance():getLogicalSize()

local testNames = {
    "Bug-350",
    "Bug-422",
    "Bug-458",
    "Bug-624",
    "Bug-886",
    "Bug-899",
    "Bug-914",
    "Bug-1159",
    "Bug-1174",
    "Bug-value-type-judge-in-table"
}

local function CreateBugsTestBackMenuItem(pLayer)
    ax.MenuItemFont:setFontName("Arial")
    ax.MenuItemFont:setFontSize(24)
    local pMenuItemFont = ax.MenuItemFont:create("Back")
    pMenuItemFont:setPosition(ax.p(VisibleRect:rightBottom().x - 50, VisibleRect:rightBottom().y + 25))
    local function menuCallback()
        local pScene = BugsTestMain()
        if pScene ~= nil then
            ax.Director:getInstance():replaceScene(pScene)
        end
    end
    pMenuItemFont:registerScriptTapHandler(menuCallback)
    local pMenu = ax.Menu:create()
    pMenu:addChild(pMenuItemFont)
    pMenu:setPosition(ax.p(0, 0))
    pLayer:addChild(pMenu)
end

--BugTest350
local function BugTest350()
    local pLayer = ax.Layer:create()
    local pBackground = ax.Sprite:create("Hello.png")
    pBackground:setPosition(ax.p(Winsize.width/2, Winsize.height/2))
    pLayer:addChild(pBackground)
    return pLayer
end

--BugTest422
local nLocalTag = 0
local function BugTest422()
    local function reset(pResetLayer)
        nLocalTag = nLocalTag + 1
        local pNode = pResetLayer:getChildByTag(nLocalTag - 1)
        if nil ~= pNode then
           --userdata per
           local strLog = "Menu:"..nLocalTag
           print(strLog)
        end

        pResetLayer:removeChild(pNode, false)

        local function menuCallback(tag,pMenuItem)
            if nil ~= pMenuItem then
                local pCallbackLayer = pMenuItem:getParent():getParent()
                reset(pCallbackLayer)
            end
        end
        ax.MenuItemFont:setFontName("Arial")
        ax.MenuItemFont:setFontSize(24)
        local pMenuItem1 = ax.MenuItemFont:create("One")
        pMenuItem1:registerScriptTapHandler(menuCallback)
        local pMenuItem2 = ax.MenuItemFont:create("Two")
        pMenuItem2:registerScriptTapHandler(menuCallback)
        local pMenu = ax.Menu:create(pMenuItem1, pMenuItem2)
        pMenu:alignItemsVertically()
        local fX = math.random() * 50
        local fY = math.random() * 50
        local menuPosX ,menuPosY = pMenu:getPosition()
        pMenu:setPosition(ax.p(menuPosX + fX,menuPosY + fY))
        pResetLayer:addChild(pMenu,0,nLocalTag)
    end

    local pLayer = ax.Layer:create()
    reset(pLayer)
    return pLayer
end

--BugTest458
local nColorFlag = 0
local function BugTest458()
    local pLayer = ax.Layer:create()

    local function InitQuestionContainerSprite(pSprite)
        --Add label
        local pLabel = ax.Label:createWithTTF("Answer 1", s_arialPath, 12)
        pLabel:setAnchorPoint(ax.p(0.5,0.5))
        pLabel:setTag(100)

        --Add the background
        local pCorner = ax.Sprite:create("Images/bugs/corner.png")
        local nWidth = Winsize.width * 0.9 - (pCorner:getContentSize().width * 2)
        local nHeight = Winsize.height * 0.15  - (pCorner:getContentSize().height * 2)
        local pColorLayer = ax.LayerColor:create(ax.color32(255, 255, 255, 255 * .75), nWidth, nHeight)
        pColorLayer:setPosition(ax.p(-nWidth / 2, -nHeight / 2))
        --First button is blue,Second is red,Used for testing - change later
        if (0 == nColorFlag) then
            pLabel:setColor(ax.color32(0,0,255))
        else
            print("Color changed")
            pLabel:setColor(ax.color32(255,0,0))
        end
        nColorFlag = nColorFlag + 1
        pSprite:addChild(pColorLayer)

        pCorner:setPosition(ax.p(-(nWidth / 2 + pCorner:getContentSize().width / 2), -(nHeight / 2 + pCorner:getContentSize().height / 2)))
        pSprite:addChild(pCorner)

		local nX,nY = pCorner:getPosition()
        local pCorner2 = ax.Sprite:create("Images/bugs/corner.png")
        pCorner2:setPosition(ax.p(-nX, nY))
        pCorner2:setFlippedX(true)
        pSprite:addChild(pCorner2)

        local pCorner3 = ax.Sprite:create("Images/bugs/corner.png")
        pCorner3:setPosition(ax.p(nX, -nY))
        pCorner3:setFlippedY(true)
        pSprite:addChild(pCorner3)

        local pCorner4 = ax.Sprite:create("Images/bugs/corner.png")
        pCorner4:setPosition(ax.p(-nX, -nY))
        pCorner4:setFlippedX(true)
        pCorner4:setFlippedY(true)
        pSprite:addChild(pCorner4)

        local pEdge = ax.Sprite:create("Images/bugs/edge.png")
        pEdge:setScaleX(nWidth)
        pEdge:setPosition(ax.p(nX + (pCorner:getContentSize().width / 2) + (nWidth / 2), nY))
        pSprite:addChild(pEdge)

        local pEdge2 = ax.Sprite:create("Images/bugs/edge.png")
        pEdge2:setScaleX(nWidth)
        pEdge2:setPosition(ax.p(nX + (pCorner:getContentSize().width / 2) + (nWidth / 2), -nY))
        pEdge2:setFlippedY(true)
        pSprite:addChild(pEdge2)

        local pEdge3 = ax.Sprite:create("Images/bugs/edge.png")
        pEdge3:setRotation(90)
        pEdge3:setScaleX(nHeight)
        pEdge3:setPosition(ax.p(nX, nY + (pCorner:getContentSize().height / 2) + (nHeight / 2)))
        pSprite:addChild(pEdge3)

        local pEdge4 = ax.Sprite:create("Images/bugs/edge.png")
        pEdge4:setRotation(270)
        pEdge4:setScaleX(nHeight)
        pEdge4:setPosition(ax.p(-nX, nY + (pCorner:getContentSize().height / 2) + (nHeight / 2)))
        pSprite:addChild(pEdge4)

        pSprite:addChild(pLabel)
    end

    local pQuestion1 = ax.Sprite:create()
    InitQuestionContainerSprite(pQuestion1)
    local pQuestion2 = ax.Sprite:create()
    InitQuestionContainerSprite(pQuestion2)

    local function menuCallback()
        print("Selected")
    end
    local pMenuItemSprite = ax.MenuItemSprite:create(pQuestion1,pQuestion2)
    pMenuItemSprite:registerScriptTapHandler(menuCallback)
    local pLayerColor1 = ax.LayerColor:create(ax.color32(0,0,255,255), 100, 100)
--  question->release()
--  question2->release()

    local pLayerColor2 = ax.LayerColor:create(ax.color32(255,0,0,255), 100, 100)
    local pMenuItemSprite2 = ax.MenuItemSprite:create(pLayerColor1, pLayerColor2)
    pMenuItemSprite2:registerScriptTapHandler(menuCallback)
    local pMenu = ax.Menu:create(pMenuItemSprite, pMenuItemSprite2)
    pMenu:alignItemsVerticallyWithPadding(100)
    pMenu:setPosition(ax.p(Winsize.width / 2, Winsize.height / 2))

    -- add the label as a child to this Layer
    pLayer:addChild(pMenu)

    return pLayer
end

--BugTest624
local BugTest624_entry = nil
local BugTest624_2_entry = nil

local function BugTest624()
    local pLayer = ax.Layer:create()

    local pLabel = ax.Label:createWithTTF("Layer1", s_markerFeltFontPath, 36)
    pLabel:setAnchorPoint(ax.p(0.5, 0.5))
    pLabel:setPosition(ax.p(Winsize.width / 2, Winsize.height / 2))
    pLayer:addChild(pLabel)
--  schedule(schedule_selector(Bug624Layer::switchLayer), 5.0f)

	local function BugTest624_SwitchLayer()
		local scheduler = ax.Director:getInstance():getScheduler()
		scheduler:unscheduleScriptEntry(BugTest624_entry)

		local pScene = ax.Scene:create()
		local pNewPlayer = BugTest624_2()
		CreateBugsTestBackMenuItem(pNewPlayer)
        pScene:addChild(pNewPlayer)
        ax.Director:getInstance():replaceScene(ax.TransitionFade:create(2.0, pScene, ax.color32(255,255,255)))
	end


    local function BugTest624_OnEnterOrExit(tag)
    	local scheduler = ax.Director:getInstance():getScheduler()
		if tag == "enter" then
			BugTest624_entry = scheduler:scheduleScriptFunc(BugTest624_SwitchLayer, 5.0, false)
		elseif tag == "exit" then
			scheduler:unscheduleScriptEntry(BugTest624_entry)
		end
    end
    pLayer:registerScriptHandler(BugTest624_OnEnterOrExit)

    return pLayer
end

function BugTest624_2()
	local pLayer = ax.Layer:create()

	local pLabel = ax.Label:createWithTTF("Layer2", s_markerFeltFontPath, 36)
    pLabel:setAnchorPoint(ax.p(0.5, 0.5))
	pLabel:setPosition(ax.p(Winsize.width / 2, Winsize.height / 2))
    pLayer:addChild(pLabel)

    local function BugTest624_2_SwitchLayer()
		local scheduler = ax.Director:getInstance():getScheduler()
		scheduler:unscheduleScriptEntry(BugTest624_2_entry)

		local pScene = ax.Scene:create()
		local pNewPlayer = BugTest624()
		CreateBugsTestBackMenuItem(pNewPlayer)
        pScene:addChild(pNewPlayer)
        ax.Director:getInstance():replaceScene(ax.TransitionFade:create(2.0, pScene, ax.color32(255,0,0)))
	end

	local function BugTest624_2_OnEnterOrExit(tag)
    	local scheduler = ax.Director:getInstance():getScheduler()
		if tag == "enter" then
			BugTest624_2_entry = scheduler:scheduleScriptFunc(BugTest624_2_SwitchLayer, 5.0, false)
		elseif tag == "exit" then
			scheduler:unscheduleScriptEntry(BugTest624_2_entry)
		end
    end
    pLayer:registerScriptHandler(BugTest624_2_OnEnterOrExit)
end

--BugTest886
local function BugTest886()
    local pLayer = ax.Layer:create()

    local pSprite1 = ax.Sprite:create("Images/bugs/bug886.jpg")
    pSprite1:setAnchorPoint(ax.p(0, 0))
    pSprite1:setPosition(ax.p(0, 0))
    pSprite1:setScaleX(0.6)
    pLayer:addChild(pSprite1)

    local pSprite2 = ax.Sprite:create("Images/bugs/bug886.jpg")
    pSprite2:setAnchorPoint(ax.p(0, 0))
    pSprite2:setScaleX(0.6)
    pSprite2:setPosition(ax.p(pSprite1:getContentSize().width * 0.6 + 10, 0))
    pLayer:addChild(pSprite2)

    return pLayer
end

--BugTest899
local function BugTest899()
    local pLayer = ax.Layer:create()

    local pBg = ax.Sprite:create("Images/bugs/RetinaDisplay.jpg")
    pLayer:addChild(pBg,0)
    pBg:setAnchorPoint(ax.p(0, 0))

    return pLayer
end

--BugTest914
local function BugTest914()
    local layer = ax.Layer:create()

    local layerColor = nil

    for i = 0, 4 do
       layerColor = ax.LayerColor:create(ax.color32(i*20, i*20, i*20,255))
       layerColor:setContentSize(ax.size(i*100, i*100))
       layerColor:setPosition(ax.p(Winsize.width/2, Winsize.height/2))
       layerColor:setAnchorPoint(ax.p(0.5, 0.5))
       layerColor:setIgnoreAnchorPointForPosition(false)
       layer:addChild(layerColor, -1-i)
    end

    --create and initialize a Label
    local function restart()
       local scene = ax.Scene:create()
       local newLayer = BugTest914()
       CreateBugsTestBackMenuItem(newLayer)
       scene:addChild(newLayer)
       ax.Director:getInstance():replaceScene(scene)
	end

    local label = ax.Label:createWithTTF("Hello World", s_markerFeltFontPath, 64)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    --position the label on the center of the screen
    label:setPosition(ax.p( Winsize.width /2 , Winsize.height/2 ))
    layer:addChild(label)

    local item1 = ax.MenuItemFont:create("restart")
    item1:registerScriptTapHandler(restart)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:alignItemsVertically()
    menu:setPosition(ax.p(Winsize.width/2, 100))
    layer:addChild(menu)

    -- handling touch events
    local function onTouchMoved(touches, event)
		local count = #(touches)
		print("Number of touches: ",count)
    end
    local function onTouchBegan(touches, event)
		onTouchMoved(touches, event)
    end
    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    listener:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCHES_MOVED )

    local eventDispatcher = layer:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, layer)

    return layer
end

--BugTest1159
local function BugTest1159()
    local pLayer = ax.Layer:create()

    local background = ax.LayerColor:create(ax.color32(255, 0, 255, 255))
    pLayer:addChild(background)

    local sprite_a   = ax.LayerColor:create(ax.color32(255, 0, 0, 255), 700, 700)
    sprite_a:setAnchorPoint(ax.p(0.5, 0.5))
    sprite_a:setIgnoreAnchorPointForPosition(false)
    sprite_a:setPosition(ax.p(0.0, Winsize.height/2))
    pLayer:addChild(sprite_a)
    local seq = ax.Sequence:create(ax.MoveTo:create(1.0, ax.p(1024.0, 384.0)), ax.MoveTo:create(1.0, ax.p(0.0, 384.0)))
    sprite_a:runAction(ax.RepeatForever:create(seq))

    local sprite_b = ax.LayerColor:create(ax.color32(0, 0, 255, 255), 400, 400)
    sprite_b:setAnchorPoint(ax.p(0.5, 0.5))
    sprite_b:setIgnoreAnchorPointForPosition(false)
    sprite_b:setPosition(ax.p(Winsize.width/2, Winsize.height/2))
    pLayer:addChild(sprite_b)

	local function menuCallback()
	   local pScene = ax.Scene:create()
       local pLayer = BugTest1159()
       CreateBugsTestBackMenuItem(pLayer)
       pScene:addChild(pLayer)
       ax.Director:getInstance():replaceScene(ax.TransitionPageTurn:create(1.0, pScene, false))
	end
    local label = ax.MenuItemLabel:create(ax.Label:createWithSystemFont("Flip Me", "Helvetica", 24))
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:registerScriptTapHandler(menuCallback)
    local menu = ax.Menu:create()
    menu:addChild(label)
    menu:setPosition(ax.p(Winsize.width - 200.0, 50.0))
    pLayer:addChild(menu)

    local function onNodeEvent(event)
        if event == "exit" then
        --[[
            if schedulerEntry ~= nil then
                scheduler:unscheduleScriptEntry(schedulerEntry)
            end
            ]]--
        end
    end

    pLayer:registerScriptHandler(onNodeEvent)

	return pLayer
end

--BugTest1174
local function BugTest1174()
    local layer = ax.Layer:create()

    local size = ax.Director:getInstance():getLogicalSize()


    local subtitleLabel = ax.Label:createWithTTF("The results output on the console", s_thonburiPath, 24)
    subtitleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(subtitleLabel, 1)
    subtitleLabel:setPosition(size.width / 2, size.height - 80)

    local function check_for_error(p1,p2,p3,p4,s,t)
        local p4_p3 = ax.pSub(p4,p3)
        local p4_p3_t = ax.pMul(p4_p3,t)
        local hitp1 = ax.pAdd(p3,p4_p3_t)

        local p2_p1 = ax.pSub(p2,p1)
        local p2_p1_s = ax.pMul(p2_p1,s)
        local hitp2 = ax.pAdd(p1,p2_p1_s)

        if math.abs(hitp1.x - hitp2.x ) > 0.1 or math.abs(hitp1.y - hitp2.y) > 0.1 then
        	local strErr = "ERROR: ("..hitp1.x..","..hitp1.y..") != ("..hitp2.x..","..hitp2.y..")"
            print(strErr)
        	return 1
        end
        return 0
    end
    local A,B,C,D,p1,p2,p3,p4
    local s = 0.0
    local t = 0.0
    local err = 0
    local ok  = 0
    local bRet = false

    print("Test1 - Start")
    local i = 0
    for i = 0, 9999 do
        --[[
        A|b
        -----
        c|d
        ]]--
        local ax = math.random() * -5000
        local ay = math.random() * 5000
        --[[
        a|b
        -----
        c|D
        ]]--
        local dx = math.random() * 5000
        local dy = math.random() * -5000

        --[[
        a|B
        -----
        c|d
        ]]--
        local bx = math.random() * 5000
        local by = math.random() * 5000

        --[[
        a|b
        -----
        C|d
        ]]--
        local cx = math.random() * -5000
        local cy = math.random() * -5000

        A = ax.p(ax,ay)
        B = ax.p(bx,by)
        C = ax.p(cx,cy)
        D = ax.p(dx,dy)

        bRet,s,t = ax.pIsLineIntersect( A, D, B, C, s, t)
        if true == bRet then
            if 1 == check_for_error(A,D,B,C,s,t) then
                err = err + 1
            else
                ok = ok + 1
            end
        end
    end
    local strLog = "Test1 - End. OK="..ok..", Err="..err
    print(strLog)
    --------
    --Test 2
    --------
    print("Test2 - Start")

    p1 = ax.p(220,480)
    p2 = ax.p(304,325)
    p3 = ax.p(264,416)
    p4 = ax.p(186,416)
    s  = 0.0
    t  = 0.0
    bRet,s,t = ax.pIsLineIntersect( p1, p2, p3, p4, s, t)
    if true == bRet then
    	check_for_error(p1, p2, p3, p4, s, t)
    end
    print("Test2 - End")

    ------
    --Test 3
    ------
    print("Test3 - Start")

    ok=0
    err=0
    for i = 0 , 9999 do
        -- A | b
        -- -----
        -- c | d
        local ax = math.random() * -500
        local ay = math.random() * 500
        p1 = ax.p(ax,ay)
        -- a | b
        -- -----
        -- c | D
        local dx = math.random() * 500
        local dy = math.random() * -500
        p2 = ax.p(dx,dy)

        -------

        local y = ay - ((ay - dy) / 2.0)

        -- a | b
        -- -----
        -- C | d
        local cx = math.random() * -500
        p3 = ax.p(cx,y)

        -- a | B
        -- -----
        -- c | d
        local bx = math.random() * 500
        p4 = ax.p(bx,y)

        s = 0.0
        t = 0.0
        bRet,s,t = ax.pIsLineIntersect(p1, p2, p3, p4, s, t)
        if true == bRet then
          if 1 == check_for_error(p1, p2, p3, p4, s,t ) then
             err = err + 1
          else
             ok = ok + 1
          end
        end
    end
    strLog = "Test3 - End. OK="..ok..", Err="..err
    print(strLog)

    return layer
end

--BugTestValueTypeJudgeInTable
local function BugTestValueTypeJudgeInTable()
    local layer = ax.Layer:create()
    local label = ax.Label:createWithTTF("Value Type Judge Error in the table:number -> string", "fonts/arial.ttf", 24)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(ax.p(VisibleRect:center().x, VisibleRect:top().y - 40))
    layer:addChild(label)
    local outLable = ax.Label:createWithTTF("You should see the following output in the console: \n     The type of index 0 is number \n     The type of index 1 is number \n     The type of index 2 is number", "fonts/arial.ttf", 18)
    outLable:setAnchorPoint(ax.p(0.5, 0.5))
    outLable:setPosition(ax.p(VisibleRect:center().x, VisibleRect:top().y - 160))
    layer:addChild(outLable)

    local valueTypeJudge = ax.ValueTypeJudgeInTable:create({index1 = 111, index2 = 112, index3 = 113})
    layer:addChild(valueTypeJudge)

    return layer
end

local CreateBugsTestTable = {
   BugTest350,
   BugTest422,
   BugTest458,
   BugTest624,
   BugTest886,
   BugTest899,
   BugTest914,
   BugTest1159,
   BugTest1174,
   BugTestValueTypeJudgeInTable,
}

local function CreateBugsTestScene(nBugNo)
  local pNewscene = ax.Scene:create()
  local pLayer = CreateBugsTestTable[nBugNo]()
  CreateBugsTestBackMenuItem(pLayer)
  pNewscene:addChild(pLayer)
  ax.Director:getInstance():replaceScene(pNewscene)
--pLayer:autorelease()
end


local function BugsTestMainLayer()
	local ret = ax.Layer:create()

    --menu callback
    local function menuCallback(tag, pMenuItem)
        local nIdx = pMenuItem:getLocalZOrder() - kItemTagBasic
        local BugTestScene = CreateBugsTestScene(nIdx)
        if nil ~= testScene then
            ax.Director:getInstance():replaceScene(testScene)
        end
    end

	-- add menu items for tests
    local pItemMenu = ax.Menu:create()
    local nTestCount = #(testNames)

    local i = 1
    for  i = 1, nTestCount do
        local  label = ax.Label:createWithTTF(testNames[i], s_arialPath, 24)
        print("test label index " .. i)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        local  pMenuItem = ax.MenuItemLabel:create(label)
        pMenuItem:registerScriptTapHandler(menuCallback)
        pItemMenu:addChild(pMenuItem, i + kItemTagBasic)
       pMenuItem:setPosition( ax.p( VisibleRect:center().x, (VisibleRect:top().y - i * LINE_SPACE) ))
    end

    pItemMenu:setPosition(ax.p(0, 0))
    ret:addChild(pItemMenu)

    -- handling touch events
    local ptBeginPos = {x = 0, y = 0}
    local ptCurPos  = {x = 0, y = 0}

    -- handling touch events
    local function onTouchMoved(touches, event)
        local touchLocation = touches[1]:getLocation()
        local nMoveY = touchLocation.y - ptBeginPos.y
        local curPosx, curPosy = pItemMenu:getPosition()
        local nextPosy = curPosy + nMoveY
        if nextPosy < 0 then
            pItemMenu:setPosition(0, 0)
            return
        end

        if nextPosy > ((nTestCount + 1) * LINE_SPACE - Winsize.height) then
            pItemMenu:setPosition(0, ((nTestCount + 1) * LINE_SPACE - Winsize.height))
            return
        end

        pItemMenu:setPosition(curPosx, nextPosy)
        ptBeginPos = touchLocation
        ptCurPos = {x = curPosx, y = nextPosy}
    end
    local function onTouchBegan(touches, event)
        ptBeginPos = touches[1]:getLocation()
    end
    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    listener:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCHES_MOVED )

    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, ret)

    return ret
end
function BugsTestMain()
	cclog("BugsTestMain")
	local scene = ax.Scene:create()
	scene:addChild(BugsTestMainLayer())
	scene:addChild(CreateBackMenuItem())
	return scene
end
