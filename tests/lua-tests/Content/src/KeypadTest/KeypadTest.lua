local function KeypadMainLayer()
    local layer = ax.Layer:create()

    local function onEnter()
        print("come in")
        local  s = ax.Director:getInstance():getWinSize()
        local  label = ax.Label:createWithTTF("Keypad Test", s_thonburiPath, 28)
        layer:addChild(label, 0)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(s.width/2, s.height-50))

        local labelTip = ax.Label:createWithTTF("Please press any key...", s_thonburiPath, 22)
        labelTip:setAnchorPoint(ax.p(0.5, 0.5))
        labelTip:setPosition(ax.p(s.width / 2, s.height / 2))
        layer:addChild(labelTip, 0)

        local function onKeyReleased(keyCode, event)
            local label = event:getCurrentTarget()
            label:setString("key: " .. keyCode .. " was clicked")
        end

        local listener = ax.EventListenerKeyboard:create()
        listener:registerScriptHandler(onKeyReleased, ax.Handler.EVENT_KEYBOARD_RELEASED )

        local eventDispatcher = layer:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener, labelTip)
    end

    local function onNodeEvent(event)
        if event == "enter" then
            onEnter()
        end
    end

    layer:registerScriptHandler(onNodeEvent)

    return layer
end


function KeypadTestMain()
	cclog("KeypadTestMain")
	local scene = ax.Scene:create()
	scene:addChild(KeypadMainLayer())
	scene:addChild(CreateBackMenuItem())
	return scene
end
