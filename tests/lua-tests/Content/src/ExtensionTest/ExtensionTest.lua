require "ExtensionTest/WebProxyTest"

local LINE_SPACE = 40
local kItemTagBasic = 1000

local ExtensionTestEnum =
{
    TEST_WEBSOCKET          = 0,
    TEST_EDITBOX            = 1,
    TEST_MAX_COUNT          = 2,
}

local testsName =
{
    "WebSocketTest",
    "EditBoxTest",
}


--Create toMainLayr MenuItem
function CreateExtensionsBasicLayerMenu(pMenu)
	if nil == pMenu then
		return
	end
	local function toMainLayer()
       local pScene = ExtensionsTestMain()
       if pScene ~= nil then
           ax.Director:getInstance():replaceScene(pScene)
       end
    end
    --Create BackMneu
    ax.MenuItemFont:setFontName("Arial")
    ax.MenuItemFont:setFontSize(24)
   	local pMenuItemFont = ax.MenuItemFont:create("Back")
    pMenuItemFont:setPosition(ax.p(VisibleRect:rightBottom().x - 50, VisibleRect:rightBottom().y + 25))
    pMenuItemFont:registerScriptTapHandler(toMainLayer)
    pMenu:addChild(pMenuItemFont)
end

local function runEditBoxTest()
	local newScene = ax.Scene:create()
	local newLayer = ax.Layer:create()
	local visibleOrigin = ax.Director:getInstance():getVisibleOrigin()
    local visibleSize = ax.Director:getInstance():getVisibleSize()

    local pBg = ax.Sprite:create("Images/HelloWorld.png")
    pBg:setPosition(ax.p(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/2))
    newLayer:addChild(pBg)

    local TTFShowEditReturn = ax.Label:createWithSystemFont("No edit control return!", "", 30)
    TTFShowEditReturn:setPosition(ax.p(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y + visibleSize.height - 50))
    newLayer:addChild(TTFShowEditReturn)

    -- Back Menu
	local pToMainMenu = ax.Menu:create()
    CreateExtensionsBasicLayerMenu(pToMainMenu)
    pToMainMenu:setPosition(ax.p(0, 0))
    newLayer:addChild(pToMainMenu,10)

    local editBoxSize = ax.size(visibleSize.width - 100, 60)
    local EditName = nil
    local EditPassword = nil
    local EditEmail = nil

	local function editBoxTextEventHandle(strEventName,pSender)
		local edit = pSender
		local strFmt
		if strEventName == "began" then
			strFmt = string.format("editBox %p DidBegin !", edit)
			print(strFmt)
		elseif strEventName == "ended" then
			strFmt = string.format("editBox %p DidEnd !", edit)
			print(strFmt)
		elseif strEventName == "return" then
			strFmt = string.format("editBox %p was returned !",edit)
			if edit == EditName then
				TTFShowEditReturn:setString("Name EditBox return !")
			elseif edit == EditPassword then
				TTFShowEditReturn:setString("Password EditBox return !")
			elseif edit == EditEmail then
				TTFShowEditReturn:setString("Email EditBox return !")
			end
			print(strFmt)
		elseif strEventName == "changed" then
			strFmt = string.format("editBox %p TextChanged, text: %s ", edit, edit:getText())
			print(strFmt)
		end
	end
    -- top
    EditName = ccui.EditBox:create(editBoxSize, ccui.Scale9Sprite:create("extensions/green_edit.png"))
    EditName:setPosition(ax.p(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height*3/4))
    local targetPlatform = ax.Application:getInstance():getTargetPlatform()
    if kTargetIphone == targetPlatform or kTargetIpad == targetPlatform then
	   EditName:setFontName("Paint Boy")
	else
		EditName:setFontName("fonts/Paint Boy.ttf")
	end
    EditName:setFontSize(25)
    EditName:setFontColor(ax.color32(255,0,0))
    EditName:setPlaceHolder("Name:")
    EditName:setPlaceholderFontColor(ax.color32(255,255,255))
    EditName:setMaxLength(8)
    EditName:setReturnType(ax.KEYBOARD_RETURNTYPE_DONE )
	--Handler
	EditName:registerScriptEditBoxHandler(editBoxTextEventHandle)
    newLayer:addChild(EditName)

    --middle
    EditPassword = ccui.EditBox:create(editBoxSize, ccui.Scale9Sprite:create("extensions/orange_edit.png"))
    EditPassword:setPosition(ax.p(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/2))
	if kTargetIphone == targetPlatform or kTargetIpad == targetPlatform then
		EditPassword:setFont("American Typewriter", 30)
	else
		EditPassword:setFont("fonts/American Typewriter.ttf", 30)
	end


    EditPassword:setFontColor(ax.color32(0,255,0))
    EditPassword:setPlaceHolder("Password:")
    EditPassword:setMaxLength(6)
    EditPassword:setInputFlag(ax.EDITBOX_INPUT_FLAG_PASSWORD)
    EditPassword:setInputMode(ax.EDITBOX_INPUT_MODE_SINGLELINE)
	EditPassword:registerScriptEditBoxHandler(editBoxTextEventHandle)
    newLayer:addChild(EditPassword)

    --bottom
    EditEmail = ccui.EditBox:create(ax.size(editBoxSize.width, editBoxSize.height), ccui.Scale9Sprite:create("extensions/yellow_edit.png"))
    EditEmail:setPosition(ax.p(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/4))
    EditEmail:setAnchorPoint(ax.p(0.5, 1.0))
    EditEmail:setPlaceHolder("Email:")
    EditEmail:setInputMode(ax.EDITBOX_INPUT_MODE_EMAILADDR)
    EditEmail:registerScriptEditBoxHandler(editBoxTextEventHandle)
    newLayer:addChild(EditEmail)
    newLayer:setPosition(ax.p(10, 20))


	newScene:addChild(newLayer)
	return newScene
end


local CreateExtensionsTestTable =
{
    runWebSocketTest,
    runEditBoxTest,
}


local function ExtensionsMainLayer()

	local s = ax.Director:getInstance():getWinSize()

	local function CreateExtensionsTestScene(nPerformanceNo)
	  	local pNewscene = CreateExtensionsTestTable[nPerformanceNo]()
  		return pNewscene
	end

	local function menuCallback(tag, pMenuItem)
		local scene = nil
    	local nIdx = pMenuItem:getLocalZOrder() - kItemTagBasic
		local ExtensionsTestScene = CreateExtensionsTestScene(nIdx)
    	if nil ~= ExtensionsTestScene then
         	ax.Director:getInstance():replaceScene(ExtensionsTestScene)
    	end
	end

	local layer = ax.Layer:create()
	local menu = ax.Menu:create()
    menu:setPosition(ax.p(0, 0))
    ax.MenuItemFont:setFontName("Arial")
    ax.MenuItemFont:setFontSize(24)
    local targetPlatform = ax.Application:getInstance():getTargetPlatform()
    local bSupportWebSocket = false
    if (ax.PLATFORM_OS_IPHONE == targetPlatform) or (ax.PLATFORM_OS_IPAD == targetPlatform) or (ax.PLATFORM_OS_ANDROID == targetPlatform) or (ax.PLATFORM_OS_WINDOWS == targetPlatform) or (ax.PLATFORM_OS_MAC == targetPlatform) then
        bSupportWebSocket = true
    end
    local bSupportEdit = false
    if (ax.PLATFORM_OS_IPHONE == targetPlatform) or (ax.PLATFORM_OS_IPAD == targetPlatform) or
        (ax.PLATFORM_OS_ANDROID == targetPlatform) or (ax.PLATFORM_OS_WINDOWS == targetPlatform) or
        (ax.PLATFORM_OS_MAC == targetPlatform) or (ax.PLATFORM_OS_TIZEN  == targetPlatform) then
        bSupportEdit = true
    end
    for i = 1, ExtensionTestEnum.TEST_MAX_COUNT do
		local item = ax.MenuItemFont:create(testsName[i])
	    item:registerScriptTapHandler(menuCallback)
        item:setPosition(s.width / 2, s.height - i * LINE_SPACE)
        menu:addChild(item, kItemTagBasic + i)
        if ((i == ExtensionTestEnum.TEST_WEBSOCKET + 1) and (false == bSupportWebSocket))
        or ((i == ExtensionTestEnum.TEST_EDITBOX + 1) and (false == bSupportEdit)) then
            item:setEnabled(false)
        end
	end

    layer:addChild(menu)

    -- handling touch events
    local beginPos = {x = 0, y = 0}
    local function onTouchesBegan(touches, event)
         beginPos = touches[1]:getLocation()
    end

    local function onTouchesMoved(touches, event)
        local location = touches[1]:getLocation()

        local nMoveY = location.y - beginPos.y
        local curPosx, curPosy = menu:getPosition()
        local nextPosy = curPosy + nMoveY
        local winSize = ax.Director:getInstance():getWinSize()
        if nextPosy < 0 then
            menu:setPosition(0, 0)
            return
        end

        if nextPosy > ((ExtensionTestEnum.TEST_MAX_COUNT + 1) * LINE_SPACE - winSize.height) then
            menu:setPosition(0, ((ExtensionTestEnum.TEST_MAX_COUNT + 1) * LINE_SPACE - winSize.height))
            return
        end

        menu:setPosition(curPosx, nextPosy)
        beginPos = {x = location.x, y = location.y}
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )

    local eventDispatcher = layer:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, layer)

	return layer
end

-------------------------------------
--  Extensions Test
-------------------------------------
function ExtensionsTestMain()
	local scene = ax.Scene:create()

	scene:addChild(ExtensionsMainLayer())
	scene:addChild(CreateBackMenuItem())

	return scene
end
