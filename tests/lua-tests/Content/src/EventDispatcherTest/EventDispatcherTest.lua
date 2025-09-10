local itemTagBasic = 1000

local testArray =
{
    TouchableSprite = 1,
    FixedPriority = 2,
    RemoveListenerWhenDispatching = 3,
    CustomEvent = 4,
    LabelKeyboardEvent = 5,
    SpriteAccelerationEvent = 6,
    RemoveAndRetainNode = 7,
    RemoveListenerAfterAdding = 8,
    GlobalZTouchTest = 9,
    StopPropagationTest = 10,
    PauseResumeTargetTest = 11,
    Issue4129Test = 12,
    Issue4160Test = 13,
}

local curLayerIdx   = testArray.TouchableSprite

local EventDispatcherScene = class("EventDispatcherScene")
EventDispatcherScene.__index = EventDispatcherScene

function EventDispatcherScene.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, EventDispatcherScene)
    return target
end

function EventDispatcherScene.create()
    local scene = EventDispatcherScene.extend(ax.Scene:create())
    return scene
end


local EventDispatcherTestDemo = class("EventDispatcherTestDemo")
EventDispatcherTestDemo.__index = EventDispatcherTestDemo
EventDispatcherTestDemo._backItem   = nil
EventDispatcherTestDemo._restarItem = nil
EventDispatcherTestDemo._nextItem   = nil

function EventDispatcherTestDemo:onEnter()

end

function EventDispatcherTestDemo.title(idx)
    if testArray.TouchableSprite == idx then
        return "Touchable Sprite Test"
    elseif testArray.FixedPriority   == idx then
        return "Fixed priority test"
    elseif testArray.RemoveListenerWhenDispatching == idx then
        return "Add and remove listener\n when dispatching event"
    elseif testArray.CustomEvent == idx then
        return "Send custom event"
    elseif testArray.LabelKeyboardEvent == idx then
        return "Label Receives Keyboard Event"
    elseif testArray.SpriteAccelerationEvent == idx then
        return "Sprite Receives Acceleration Event"
    elseif testArray.RemoveAndRetainNode == idx then
        return "RemoveAndRetainNodeTest"
    elseif testArray.RemoveListenerAfterAdding == idx then
        return "RemoveListenerAfterAddingTest"
    elseif testArray.GlobalZTouchTest == idx then
        return "Global Z Value, Try touch blue sprite"
    elseif testArray.StopPropagationTest == idx then
        return "Stop Propagation Test"
    elseif testArray.PauseResumeTargetTest == idx then
        return "PauseResumeTargetTest"
    elseif testArray.Issue4129Test == idx then
        return "Issue 4129: Remove All Listeners"
    elseif testArray.Issue4160Test == idx then
        return "Issue 4160: Out of range exception"
    end
end

function EventDispatcherTestDemo.subTitle(idx)
    if testArray.TouchableSprite == idx then
        return "Please drag the blocks"
    elseif testArray.FixedPriority   == idx then
        return "Fixed Priority, Blue: 30, Red: 20, Yellow: 10\n The lower value the higher priority will be."
    elseif testArray.RemoveListenerWhenDispatching == idx then
        return ""
    elseif testArray.CustomEvent == idx then
        return ""
    elseif testArray.LabelKeyboardEvent == idx then
        return "Please click keyboard\n(Only available on Desktop and Android)"
    elseif testArray.SpriteAccelerationEvent == idx then
        return "Please move your device\n(Only available on mobile)"
    elseif testArray.RemoveAndRetainNode == idx then
        return "Sprite should be removed after 5s, add to scene again after 5s"
    elseif testArray.RemoveListenerAfterAdding == idx then
        return "Should not crash!"
    elseif testArray.GlobalZTouchTest == idx then
        return "Blue Sprite should change go from foreground to background"
    elseif testArray.StopPropagationTest == idx then
        return "Shouldn't crash and only blue block could be clicked"
    elseif testArray.PauseResumeTargetTest == idx then
        return ""
    elseif testArray.Issue4129Test == idx then
        return "Should see 'Yeah, this issue was fixed.'"
    elseif testArray.Issue4160Test == idx then
        return "Touch the red block twice \n should not crash and the red one couldn't be touched"
    end
end

function EventDispatcherTestDemo.create()
    local layer = EventDispatcherTestDemo.extend(ax.Layer:create())

    if nil ~= layer then
        layer:createMenu()
        layer:onEnter()
        layer:creatTitleAndSubTitle(curLayerIdx)
    end

    return layer
end

function EventDispatcherTestDemo.backCallback()
    local newScene = EventDispatcherScene.create()
    newScene:addChild(backEventDispatcherTest())
    newScene:addChild(CreateBackMenuItem())
    ax.Director:getInstance():replaceScene(newScene)
end

function EventDispatcherTestDemo.restartCallback()
    local newScene = EventDispatcherScene.create()
    newScene:addChild(restartEventDispatcherTest())
    newScene:addChild(CreateBackMenuItem())
    ax.Director:getInstance():replaceScene(newScene)
end

function EventDispatcherTestDemo.nextCallback()
    local newScene = EventDispatcherScene.create()
    newScene:addChild(nextEventDispatcherTest())
    newScene:addChild(CreateBackMenuItem())
    ax.Director:getInstance():replaceScene(newScene)
end

function EventDispatcherTestDemo:createMenu()
    local menu = ax.Menu:create()

    self._backItem = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
    self._backItem:registerScriptTapHandler(self.backCallback)
    menu:addChild(self._backItem,itemTagBasic)
    self._restarItem = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
    self._restarItem:registerScriptTapHandler(self.restartCallback)
    menu:addChild(self._restarItem,itemTagBasic)
    self._nextItem = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
    menu:addChild(self._nextItem,itemTagBasic)
    self._nextItem:registerScriptTapHandler(self.nextCallback)

    local size = ax.Director:getInstance():getLogicalSize()
    self._backItem:setPosition(ax.p(size.width / 2 - self._restarItem:getContentSize().width * 2, self._restarItem:getContentSize().height / 2))
    self._restarItem:setPosition(ax.p(size.width / 2, self._restarItem:getContentSize().height / 2))
    self._nextItem:setPosition(ax.p(size.width / 2 + self._restarItem:getContentSize().width * 2, self._restarItem:getContentSize().height / 2))

    menu:setPosition(ax.p(0, 0))

    self:addChild(menu, 9999)
end

function EventDispatcherTestDemo:creatTitleAndSubTitle(idx)
    local title = ax.Label:createWithTTF(EventDispatcherTestDemo.title(idx),s_arialPath,18)
    title:setColor(ax.color32(128,128,0))
    self:addChild(title, 1, 10000)
    title:setAnchorPoint(ax.p(0.5, 0.5))
    title:setPosition( ax.p(VisibleRect:center().x, VisibleRect:top().y - 30))
    local subTitle = nil
    if "" ~= EventDispatcherTestDemo.subTitle(idx) then
        local subTitle = ax.Label:createWithTTF(EventDispatcherTestDemo.subTitle(idx), s_arialPath, 18)
        subTitle:setColor(ax.color32(128,128,0))
        self:addChild(subTitle, 1, 10001)
        subTitle:setAnchorPoint(ax.p(0.5, 0.5))
        subTitle:setPosition( ax.p(VisibleRect:center().x, VisibleRect:top().y - 60) )
    end
end


local TouchableSpriteTest = class("TouchableSpriteTest",EventDispatcherTestDemo)
TouchableSpriteTest.__index = TouchableSpriteTest

function TouchableSpriteTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, TouchableSpriteTest)
    return target
end

function TouchableSpriteTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite1 = ax.Sprite:create("Images/CyanSquare.png")
    sprite1:setPosition(ax.p(origin.x + size.width/2 - 80, origin.y + size.height/2 + 80))
    self:addChild(sprite1, 10)

    local sprite2 = ax.Sprite:create("Images/MagentaSquare.png")
    sprite2:setPosition(ax.p(origin.x + size.width/2, origin.x + size.height/2))
    self:addChild(sprite2, 20)

    local sprite3 = ax.Sprite:create("Images/YellowSquare.png")
    sprite3:setPosition(ax.p(0, 0))
    sprite2:addChild(sprite3, 1)

    local function onTouchBegan(touch, event)
        local target = event:getCurrentTarget()

        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        local s = target:getContentSize()
        local rect = ax.rect(0, 0, s.width, s.height)

        if ax.rectContainsPoint(rect, locationInNode) then
            print(string.format("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y))
            target:setOpacity(180)
            return true
        end
        return false
    end

    local function onTouchMoved(touch, event)
        local target = event:getCurrentTarget()
        local posX,posY = target:getPosition()
        local delta = touch:getDelta()
        target:setPosition(ax.p(posX + delta.x, posY + delta.y))
    end

    local function onTouchEnded(touch, event)
        local target = event:getCurrentTarget()
        print("sprite onTouchesEnded..")
        target:setOpacity(255)
        if target == sprite2 then
            sprite1:setLocalZOrder(100)
        elseif target == sprite1 then
            sprite1:setLocalZOrder(0)
        end
    end

    local listener1 = ax.EventListenerTouchOneByOne:create()
    listener1:setSwallowTouches(true)
    listener1:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener1:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCH_MOVED )
    listener1:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener1, sprite1)

    local listener2 = listener1:clone()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener2, sprite2)

    local listener3 = listener1:clone()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener3, sprite3)

    local function removeAllTouchItem(tag, sender)
        sender:setString("Only Next item could be clicked")
        eventDispatcher:removeEventListenersForType(ax.EVENT_TOUCH_ONE_BY_ONE)

        local nextMenuItem = ax.MenuItemFont:create("Next")
        nextMenuItem:setFontSizeObj(16)
        nextMenuItem:setPosition(ax.p(VisibleRect:right().x - 100, VisibleRect:right().y - 30))
        nextMenuItem:registerScriptTapHandler(self.nextCallback)

        local menu2 = ax.Menu:create(nextMenuItem)
        menu2:setPosition(ax.p(0, 0))
        menu2:setAnchorPoint(ax.p(0, 0))
        self:addChild(menu2)
    end

    local menuItem = ax.MenuItemFont:create("Remove All Touch Listeners")
    menuItem:setFontSizeObj(16)
    menuItem:setPosition(ax.p(VisibleRect:right().x - 100, VisibleRect:right().y))
    menuItem:registerScriptTapHandler(removeAllTouchItem)

    local menu  = ax.Menu:create(menuItem)
    menu:setPosition(ax.p(0, 0))
    menu:setAnchorPoint(ax.p(0, 0))
    self:addChild(menu)
end

function TouchableSpriteTest.create()
    local layer = TouchableSpriteTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local TouchableSpriteWithFixedPriority = class("TouchableSpriteWithFixedPriority")
TouchableSpriteWithFixedPriority.__index = TouchableSpriteWithFixedPriority
TouchableSpriteWithFixedPriority._listener = nil
TouchableSpriteWithFixedPriority._fixedPriority = 0
TouchableSpriteWithFixedPriority._useNodePriority = false
TouchableSpriteWithFixedPriority._removeListenerOnTouchEnded = false

function TouchableSpriteWithFixedPriority.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, TouchableSpriteWithFixedPriority)
    return target
end

function TouchableSpriteWithFixedPriority:onEnter()
    local eventDispatcher = self:getEventDispatcher()

    local function onTouchBegan(touch, event)
        local locationInNode = self:convertToNodeSpace(touch:getLocation())
        local s = self:getContentSize()
        local rect = ax.rect(0, 0, s.width, s.height)

        if ax.rectContainsPoint(rect, locationInNode) then
            self:setColor(ax.color32(255, 0, 0))
            return true
        end

        return false
    end

    local function onTouchMoved(touch, event)

    end

    local  function onTouchEnded(touch, event)
        self:setColor(ax.color32(255, 255, 255))
        if self._removeListenerOnTouchEnded then
            eventDispatcher:removeEventListener(self._listener)
            self._listener = nil
        end

    end

    local listener = ax.EventListenerTouchOneByOne:create()
    self._listener = listener
    listener:setSwallowTouches(true)

    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )

    if 0 == self._fixedPriority then
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
    else
        eventDispatcher:addEventListenerWithFixedPriority(listener,self._fixedPriority)
    end
end

function TouchableSpriteWithFixedPriority:onExit()
    if self._listener ~= nil then
        local eventDispatcher = self:getEventDispatcher()
        eventDispatcher:removeEventListener(self._listener)
    end
end

function TouchableSpriteWithFixedPriority:setPriority(fixedPriority)
    self._fixedPriority = fixedPriority
    self._useNodePriority = false
end

function TouchableSpriteWithFixedPriority:removeListenerOnTouchEnded(toRemove)
    self._removeListenerOnTouchEnded = toRemove
end

function TouchableSpriteWithFixedPriority:setPriorityWithThis(useNodePriority)
    self._fixedPriority = 0
    self._useNodePriority = useNodePriority
end

function TouchableSpriteWithFixedPriority.create()
    local touchableSprite = TouchableSpriteWithFixedPriority.extend(ax.Sprite:create())

    local function onNodeEvent(event)
        if event == "enter" then
            touchableSprite:onEnter()
        elseif event == "exit" then
            touchableSprite:onExit()
        end
    end

    touchableSprite:registerScriptHandler(onNodeEvent)
    return touchableSprite
end

local FixedPriorityTest = class("FixedPriorityTest",EventDispatcherTestDemo)
FixedPriorityTest.__index = FixedPriorityTest

function FixedPriorityTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, FixedPriorityTest)
    return target
end

function FixedPriorityTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite1 = TouchableSpriteWithFixedPriority.create()
    sprite1:setTexture("Images/CyanSquare.png")
    sprite1:setPriority(30)
    sprite1:setPosition(ax.p(origin.x + size.width/2 - 80, origin.y + size.height/2 + 40))
    self:addChild(sprite1, 10)

    local sprite2 = TouchableSpriteWithFixedPriority.create()
    sprite2:setTexture("Images/MagentaSquare.png")
    sprite2:setPriority(20)
    sprite2:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2) )
    self:addChild(sprite2, 20)

    local sprite3 = TouchableSpriteWithFixedPriority:create()
    sprite3:setTexture("Images/YellowSquare.png")
    sprite3:setPriority(10)
    sprite3:setPosition(ax.p(0, 0))
    sprite2:addChild(sprite3, 1)
end

function FixedPriorityTest.create()
    local layer = FixedPriorityTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local RemoveListenerWhenDispatchingTest = class("RemoveListenerWhenDispatchingTest",EventDispatcherTestDemo)
RemoveListenerWhenDispatchingTest.__index = RemoveListenerWhenDispatchingTest

function RemoveListenerWhenDispatchingTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, RemoveListenerWhenDispatchingTest)
    return target
end

function RemoveListenerWhenDispatchingTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite1 = ax.Sprite:create("Images/CyanSquare.png")
    sprite1:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))
    self:addChild(sprite1, 10)

    local function onTouchBegan(touch, event)
        local locationInNode = sprite1:convertToNodeSpace(touch:getLocation())
        local s = sprite1:getContentSize()
        local rect = ax.rect(0, 0, s.width, s.height)

         if ax.rectContainsPoint(rect, locationInNode) then
            sprite1:setColor(ax.color32(255, 0, 0))
            return true
        end

        return false
    end

    local function onTouchEnded(touch, event)
        sprite1:setColor(ax.color32(255, 255, 255))
    end

    local listener1 = ax.EventListenerTouchOneByOne:create()
    listener1:setSwallowTouches(true)
    self:setUserObject(listener1)

    listener1:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener1:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener1, sprite1)

    local statusLabel = ax.Label:createWithSystemFont("The sprite could be touched!", "", 20)
    statusLabel:setAnchorPoint(ax.p(0.5, 0.5))
    statusLabel:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height - 90))
    self:addChild(statusLabel)

    local enable = true
    local function toggleCallback(tag, sender)
        if enable then
            eventDispatcher:removeEventListener(listener1)
            statusLabel:setString("The sprite could not be touched!")
            enable = false
        else
            eventDispatcher:addEventListenerWithSceneGraphPriority(listener1, sprite1)
            statusLabel:setString("The sprite could be touched!")
            enable = true
        end
    end

    local toggleItem = ax.MenuItemToggle:create(ax.MenuItemFont:create("Enabled"), ax.MenuItemFont:create("Disabled"))
    toggleItem:setPosition(ax.p(origin.x + size.width/2, origin.y + 80))
    toggleItem:registerScriptTapHandler(toggleCallback)
    local menu = ax.Menu:create(toggleItem)
    menu:setPosition(ax.p(0, 0))
    menu:setAnchorPoint(ax.p(0, 0))
    self:addChild(menu, -1)
end


function RemoveListenerWhenDispatchingTest.create()
    local layer = RemoveListenerWhenDispatchingTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local CustomEventTest = class("CustomEventTest",EventDispatcherTestDemo)
CustomEventTest.__index = CustomEventTest
CustomEventTest._listener1 = nil
CustomEventTest._listener2 = nil

function CustomEventTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, CustomEventTest)
    return target
end

function CustomEventTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()
    local count1 = 0
    local count2 = 0

    ax.MenuItemFont:setFontSize(20)

    local statusLabel1 = ax.Label:createWithSystemFont("No custom event 1 received!", "", 20)
    statusLabel1:setAnchorPoint(ax.p(0.5, 0.5))
    statusLabel1:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height-90 ))
    self:addChild(statusLabel1)

    local function eventCustomListener1(event)
        local str = "Custom event 1 received, "..event._usedata.." times"
        statusLabel1:setString(str)
    end

    local listener1 = ax.EventListenerCustom:create("game_custom_event1",eventCustomListener1)
    self._listener1 = listener1
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithFixedPriority(listener1, 1)

    local function sendCallback1(tag, sender)
        count1 = count1 + 1

        local event = ax.EventCustom:new("game_custom_event1")
        event._usedata = string.format("%d",count1)
        eventDispatcher:dispatchEvent(event)
    end
    local sendItem1 = ax.MenuItemFont:create("Send Custom Event 1")
    sendItem1:registerScriptTapHandler(sendCallback1)
    sendItem1:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))

    local statusLabel2 = ax.Label:createWithSystemFont("No custom event 2 received!", "", 20)
    statusLabel2:setAnchorPoint(ax.p(0.5, 0.5))
    statusLabel2:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height-120 ))
    self:addChild(statusLabel2)

    local function eventCustomListener2(event)
        local str = "Custom event 2 received, "..event._usedata.." times"
        statusLabel2:setString(str)
    end

    local listener2 = ax.EventListenerCustom:create("game_custom_event2",eventCustomListener2)
    CustomEventTest._listener2 = listener2
    eventDispatcher:addEventListenerWithFixedPriority(listener2, 1)

    local function sendCallback2(tag, sender)
        count2 = count2 + 1

        local event = ax.EventCustom:new("game_custom_event2")
        event._usedata = string.format("%d",count2)
        eventDispatcher:dispatchEvent(event)
    end
    local sendItem2 = ax.MenuItemFont:create("Send Custom Event 2")
    sendItem2:registerScriptTapHandler(sendCallback2)
    sendItem2:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2 - 40))

    local menu = ax.Menu:create(sendItem1, sendItem2)
    menu:setPosition(ax.p(0, 0))
    menu:setAnchorPoint(ax.p(0, 0))
    self:addChild(menu, -1)
end

function CustomEventTest:onExit()
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:removeEventListener(self._listener1)
    eventDispatcher:removeEventListener(self._listener2)
end

function CustomEventTest.create()
    local layer = CustomEventTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end



local LabelKeyboardEventTest = class("LabelKeyboardEventTest",EventDispatcherTestDemo)
LabelKeyboardEventTest.__index = LabelKeyboardEventTest

function LabelKeyboardEventTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, LabelKeyboardEventTest)
    return target
end

function LabelKeyboardEventTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local statusLabel = ax.Label:createWithSystemFont("No keyboard event received!", "", 20)
    statusLabel:setAnchorPoint(ax.p(0.5, 0.5))
    statusLabel:setPosition(ax.p(origin.x + size.width/2,origin.y + size.height/2))
    self:addChild(statusLabel)

    local function onKeyPressed(keyCode, event)
        local buf = string.format("Key %d was pressed!",keyCode)
        local label = event:getCurrentTarget()
        label:setString(buf)
    end

    local function onKeyReleased(keyCode, event)
        local buf = string.format("Key %d was released!",keyCode)
        local label = event:getCurrentTarget()
        label:setString(buf)
    end

    local listener = ax.EventListenerKeyboard:create()
    listener:registerScriptHandler(onKeyPressed, ax.Handler.EVENT_KEYBOARD_PRESSED )
    listener:registerScriptHandler(onKeyReleased, ax.Handler.EVENT_KEYBOARD_RELEASED )

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, statusLabel)
end


function LabelKeyboardEventTest.create()
    local layer = LabelKeyboardEventTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local SpriteAccelerationEventTest = class("SpriteAccelerationEventTest",EventDispatcherTestDemo)
SpriteAccelerationEventTest.__index = SpriteAccelerationEventTest

function SpriteAccelerationEventTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, SpriteAccelerationEventTest)
    return target
end

function SpriteAccelerationEventTest:onEnter()
    self:setAccelerometerEnabled(true)
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size   = ax.Director:getInstance():getVisibleSize()
    local sprite = ax.Sprite:create("Images/ball.png")
    sprite:setPosition(ax.p(VisibleRect:center().x, VisibleRect:center().y))
    self:addChild(sprite)

    local function accelerometerListener(event,x,y,z,timestamp)
        local target  = event:getCurrentTarget()
        local ballSize = target:getContentSize()
        local ptNowX,ptNowY    = target:getPosition()
        ptNowX = ptNowX + x * 9.81
        ptNowY = ptNowY + y * 9.81

        local minX  = math.floor(VisibleRect:left().x + ballSize.width / 2.0)
        local maxX  = math.floor(VisibleRect:right().x - ballSize.width / 2.0)
        if ptNowX <   minX then
            ptNowX = minX
        elseif ptNowX > maxX then
            ptNowX = maxX
        end

        local minY  = math.floor(VisibleRect:bottom().y + ballSize.height / 2.0)
        local maxY  = math.floor(VisibleRect:top().y   - ballSize.height / 2.0)
        if ptNowY <   minY then
            ptNowY = minY
        elseif ptNowY > maxY then
            ptNowY = maxY
        end

        target:setPosition(ax.p(ptNowX , ptNowY))
    end

    local listener = ax.EventListenerAcceleration:create(accelerometerListener)

    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, sprite)
end

function SpriteAccelerationEventTest:onExit()
    ax.Device:setAccelerometerEnabled(false)
end

function SpriteAccelerationEventTest.create()
    local layer = SpriteAccelerationEventTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end


local RemoveAndRetainNodeTest = class("RemoveAndRetainNodeTest",EventDispatcherTestDemo)
RemoveAndRetainNodeTest.__index = RemoveAndRetainNodeTest
RemoveAndRetainNodeTest._spriteSaved = false
RemoveAndRetainNodeTest._sprite      = nil

function RemoveAndRetainNodeTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, RemoveAndRetainNodeTest)
    return target
end

function RemoveAndRetainNodeTest:onEnter()
    self._spriteSaved = false
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite = ax.Sprite:create("Images/CyanSquare.png")
    sprite:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))
    self._sprite = sprite
    self:addChild(sprite, 10)

    local function onTouchBegan(touch,event)
        local target = event:getCurrentTarget()
        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        local s = target:getContentSize()
        local rect = ax.rect(0, 0, s.width, s.height)

        if ax.rectContainsPoint(rect, locationInNode) then
            print(string.format("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y))
            target:setOpacity(180)
            return true
        end
        return false
    end

    local function onTouchMoved(touch,event)
        local target = event:getCurrentTarget()
        local posX,posY = target:getPosition()
        local delta = touch:getDelta()
        local force = touch:getCurrentForce()
        local maxForce = touch:getMaxForce()
        if force > 0.0 and (force / maxForce) > 0.8 then
            local origin = ax.Director:getInstance():getVisibleOrigin()
            local size = ax.Director:getInstance():getVisibleSize()
            target:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))
            print(string.format("3D touch detected, reset to default position. force = %f, max force = %f", force, maxForce))
        else
            target:setPosition(ax.p(posX + delta.x, posY + delta.y))
        end
    end

    local function onTouchEnded(touch,event)
        local target = event:getCurrentTarget()
        print("sprite onTouchesEnded.. ")
        target:setOpacity(255)
    end

    local listener1 = ax.EventListenerTouchOneByOne:create()
    listener1:setSwallowTouches(true)
    listener1:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN)
    listener1:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCH_MOVED)
    listener1:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED)
    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener1, sprite)

    local function retainSprite()
        self._spriteSaved = true
        self._sprite:retain()
        self._sprite:removeFromParent(false)
    end

    local function releaseSprite()
        self._spriteSaved = false
        self:addChild(self._sprite)
        self._sprite:release()
    end

    self:runAction( ax.Sequence:create(ax.DelayTime:create(5.0),
                                     ax.CallFunc:create(retainSprite),
                                     ax.DelayTime:create(5.0),
                                     ax.CallFunc:create(releaseSprite)
                                    ))

end

function RemoveAndRetainNodeTest:onExit()
    if self._spriteSaved then
        self._sprite:release()
    end
end

function RemoveAndRetainNodeTest.create()
    local layer = RemoveAndRetainNodeTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local RemoveListenerAfterAddingTest = class("RemoveListenerAfterAddingTest",EventDispatcherTestDemo)
RemoveListenerAfterAddingTest.__index = RemoveListenerAfterAddingTest


function RemoveListenerAfterAddingTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, RemoveListenerAfterAddingTest)
    return target
end

function RemoveListenerAfterAddingTest:onEnter()

    local eventDispatcher = self:getEventDispatcher()

    local function item1Callback(tag, sender)

        local function onTouchBegan(touch, event)
            AXASSERT(false, "Should not come here!")
            return true
        end

        local listener = ax.EventListenerTouchOneByOne:create()
        listener:registerScriptHandler(onTouchBegan, ax.Handler.EVENT_TOUCH_BEGAN)
        eventDispatcher:addEventListenerWithFixedPriority(listener, -1)
        eventDispatcher:removeEventListener(listener)
    end

    local item1 = ax.MenuItemFont:create("Click Me 1")
    item1:registerScriptTapHandler(item1Callback)
    item1:setPosition(ax.p(VisibleRect:center().x, VisibleRect:center().y + 80))

    local function addNextButton()

        local function nextButtonCallback(tag, sender)
            self.restartCallback()
        end

        local nextButton = ax.MenuItemFont:create("Please Click Me To Reset!")
        nextButton:registerScriptTapHandler(nextButtonCallback)
        nextButton:setPosition(ax.p(VisibleRect:center().x,  VisibleRect:center().y - 40))

        local menu = ax.Menu:create(nextButton)
        menu:setPosition(VisibleRect:leftBottom())
        menu:setAnchorPoint(ax.p(0,0))
        self:addChild(menu)
    end

    local function item2Callback( tag, sender )

        local function onTouchBegan(touch, event)
            print("Should not come here!")
            return true
        end

        local listener = ax.EventListenerTouchOneByOne:create()
        listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN)

        eventDispatcher:addEventListenerWithFixedPriority(listener, -1)
        eventDispatcher:removeEventListenersForType(ax.EVENT_TOUCH_ONE_BY_ONE)

        addNextButton()

    end

    local item2 = ax.MenuItemFont:create("Click Me 2")
    item2:registerScriptTapHandler(item2Callback)
    item2:setPosition(ax.p(VisibleRect:center().x, VisibleRect:center().y + 40))

    local function item3Callback( tag, sender )

        local function onTouchBegan(touch, event)
            print("Should not come here!")
            return true
        end

        local listener = ax.EventListenerTouchOneByOne:create()
        listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN)

        eventDispatcher:addEventListenerWithFixedPriority(listener, -1)
        eventDispatcher:removeAllEventListeners()

        addNextButton()

    end

    local item3 = ax.MenuItemFont:create("Click Me 3")
    item3:registerScriptTapHandler(item3Callback)
    item3:setPosition(VisibleRect:center())

    local menu = ax.Menu:create(item1, item2, item3)
    menu:setPosition(VisibleRect:leftBottom())
    menu:setAnchorPoint(ax.p(0,0))

    self:addChild(menu)
end

function RemoveListenerAfterAddingTest:onExit()

end

function RemoveListenerAfterAddingTest.create()
    local layer = RemoveListenerAfterAddingTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    return layer
end

local GlobalZTouchTest = class("GlobalZTouchTest",EventDispatcherTestDemo)
GlobalZTouchTest.__index = GlobalZTouchTest
GlobalZTouchTest._sprite = nil
GlobalZTouchTest._accum  = 0
GlobalZTouchTest._layer  = nil

function GlobalZTouchTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, GlobalZTouchTest)
    return target
end

function GlobalZTouchTest:onEnter()
    local function onTouchBegan(touch, event)
        local target = event:getCurrentTarget()

        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        local s = target:getContentSize()
        local rect = ax.rect(0, 0, s.width, s.height)

        if ax.rectContainsPoint(rect, locationInNode) then
            print(string.format("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y))
            target:setOpacity(180)
            return true
        end
        return false
    end

    local function onTouchMoved(touch, event)
        local target = event:getCurrentTarget()
        local posX,posY = target:getPosition()
        local delta = touch:getDelta()
        target:setPosition(ax.p(posX + delta.x, posY + delta.y))
    end

    local function onTouchEnded(touch, event)
        local target = event:getCurrentTarget()
        print("sprite onTouchesEnded..")
        target:setOpacity(255)
    end

    local listener = ax.EventListenerTouchOneByOne:create()
    listener:setSwallowTouches(true)
    listener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,ax.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )

    local SPRITE_COUNT = 8
    for i = 0, SPRITE_COUNT - 1 do
        local sprite = nil
        if 4 == i then
            sprite = ax.Sprite:create("Images/CyanSquare.png")
            self._sprite = sprite
            self._sprite:setGlobalZOrder(-1)
        else
            sprite = ax.Sprite:create("Images/YellowSquare.png")
        end

        local eventDispatcher = self:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener:clone(), sprite)
        self._layer:addChild(sprite)
        local visibleSize = ax.Director:getInstance():getVisibleSize()
        sprite:setPosition(VisibleRect:left().x + visibleSize.width / (SPRITE_COUNT - 1) * i, VisibleRect:center().y)
    end

    local function update(dt)
        self._accum = self._accum + dt
        if self._accum > 2.0 then
            local z = self._sprite:getGlobalZOrder()
            self._sprite:setGlobalZOrder(-z)
            self._accum = 0
        end
    end
    self._layer:scheduleUpdateWithPriorityLua(update, 0)
end

function GlobalZTouchTest:onExit()
    self._layer:unscheduleUpdate()
end

function GlobalZTouchTest.create()
    local layer = GlobalZTouchTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)
    GlobalZTouchTest._layer  = layer
    return layer
end


local StopPropagationTest = class("StopPropagationTest",EventDispatcherTestDemo)
local TAG_BLUE_SPRITE = 101
local TAG_BLUE_SPRITE2 = 102
local SPRITE_COUNT    = 8

function StopPropagationTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, StopPropagationTest)
    return target
end

function StopPropagationTest:onEnter()

    local function onTouchBegan(touch, event)
        if not self:isPointInTopHalfAreaOfScreen(touch:getLocation()) then
            return false
        end
        local target = event:getCurrentTarget()
        assert(target:getTag() == TAG_BLUE_SPRITE, "Yellow blocks shouldn't response event.")
        if self:isPointInNode(touch:getLocation(), target) then
            target:setOpacity(180)
            return true
        end
        event:stopPropagation()
        return false
    end

    local function onTouchEnded(touch, event)
        local target = event:getCurrentTarget()
        target:setOpacity(255)
    end

    local touchOneByOneListener = ax.EventListenerTouchOneByOne:create()
    touchOneByOneListener:setSwallowTouches(true)
    touchOneByOneListener:registerScriptHandler(onTouchBegan,ax.Handler.EVENT_TOUCH_BEGAN )
    touchOneByOneListener:registerScriptHandler(onTouchEnded,ax.Handler.EVENT_TOUCH_ENDED )

    local function onTouchesBegan(touches, event)
        if self:isPointInTopHalfAreaOfScreen(touches[1]:getLocation()) then
            return
        end
        local target = event:getCurrentTarget()
        assert(target:getTag() == TAG_BLUE_SPRITE2, "Yellow blocks shouldn't response event.")

        if self:isPointInNode(touches[1]:getLocation(), target) then
            target:setOpacity(180)
        end
        event:stopPropagation()
    end

    local function onTouchesEnd(touches, event)
        if self:isPointInTopHalfAreaOfScreen(touches[1]:getLocation()) then
            return
        end
        local target = event:getCurrentTarget()
        assert(target:getTag() == TAG_BLUE_SPRITE2, "Yellow blocks shouldn't response event.")

        if self:isPointInNode(touches[1]:getLocation(), target) then
            target:setOpacity(255)
        end
        event:stopPropagation()
    end

    local touchAllAtOnceListener = ax.EventListenerTouchAllAtOnce:create()
    touchAllAtOnceListener:registerScriptHandler(onTouchesBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    touchAllAtOnceListener:registerScriptHandler(onTouchesEnd,ax.Handler.EVENT_TOUCHES_ENDED )

    local function onKeyPressed(key, event)
        local target = event:getCurrentTarget()
        assert(target:getTag() == TAG_BLUE_SPRITE or target:getTag() == TAG_BLUE_SPRITE2, "Yellow blocks shouldn't response event.")
        event:stopPropagation()
    end

    local keyboardEventListener = ax.EventListenerKeyboard:create()
    keyboardEventListener:registerScriptHandler(onKeyPressed, ax.Handler.EVENT_KEYBOARD_PRESSED )

    local eventDispatcher = self:getEventDispatcher()

    for i = 0, SPRITE_COUNT - 1 do
        local sprite = nil
        local sprite2 = nil

        if 4 == i then
            sprite = ax.Sprite:create("Images/CyanSquare.png")
            sprite:setTag(TAG_BLUE_SPRITE)
            self:addChild(sprite, 100)

            sprite2 = ax.Sprite:create("Images/CyanSquare.png")
            sprite2:setTag(TAG_BLUE_SPRITE2)
            self:addChild(sprite2, 100)
        else
            sprite = ax.Sprite:create("Images/YellowSquare.png")
            self:addChild(sprite, 0)

            sprite2 = ax.Sprite:create("Images/YellowSquare.png")
            self:addChild(sprite2, 0)
        end

        eventDispatcher:addEventListenerWithSceneGraphPriority(touchOneByOneListener:clone(), sprite)
        eventDispatcher:addEventListenerWithSceneGraphPriority(keyboardEventListener:clone(), sprite)

        eventDispatcher:addEventListenerWithSceneGraphPriority(touchAllAtOnceListener:clone(), sprite2)
        eventDispatcher:addEventListenerWithSceneGraphPriority(keyboardEventListener:clone(), sprite2)

        local visibleSize = ax.Director:getInstance():getVisibleSize()
        sprite:setPosition(VisibleRect:left().x + visibleSize.width / (SPRITE_COUNT - 1) * i, VisibleRect:center().y + sprite2:getContentSize().height / 2 + 10)
        sprite2:setPosition(VisibleRect:left().x + visibleSize.width / (SPRITE_COUNT - 1) * i, VisibleRect:center().y - sprite2:getContentSize().height / 2 - 10)

    end


end

function StopPropagationTest:onExit()

end

function StopPropagationTest:isPointInNode(pt, node)
    local locationInNode = node:convertToNodeSpace(pt)
    local s = node:getContentSize()
    local rect = ax.rect(0, 0, s.width, s.height)
    if ax.rectContainsPoint(rect, locationInNode) then
        return true
    end
    return false
end

function StopPropagationTest:isPointInTopHalfAreaOfScreen(pt)
    local winSize = ax.Director:getInstance():getLogicalSize()
    if pt.y >= winSize.height / 2 then
        return true
    end

    return false
end

function StopPropagationTest.create()
    local layer = StopPropagationTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)

    return layer
end

local PauseResumeTargetTest = class("PauseResumeTargetTest",EventDispatcherTestDemo)

function PauseResumeTargetTest.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, PauseResumeTargetTest)
    return target
end

function PauseResumeTargetTest:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite1 = TouchableSpriteWithFixedPriority.create()
    sprite1:setTexture("Images/CyanSquare.png")
    sprite1:setPosition(ax.p(origin.x + size.width/2 - 80, origin.y + size.height/2 +  40))
    self:addChild(sprite1, -10)

    local sprite2 = TouchableSpriteWithFixedPriority.create()
    sprite2:setTexture("Images/MagentaSquare.png")
    sprite2:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))
    self:addChild(sprite2, -20)

    local sprite3 = TouchableSpriteWithFixedPriority.create()
    sprite3:setTexture("Images/YellowSquare.png")
    sprite3:setPosition(ax.p(0, 0))
    sprite2:addChild(sprite3, -1)


    local function popUp(tag, sender)
        local eventDispatcher = self:getEventDispatcher()
        eventDispatcher:pauseEventListenersForTarget(self, true)

        local colorLayer = ax.LayerColor:create(ax.color32(0, 0, 255, 100))
        self:addChild(colorLayer, 99999)

        local function closePopUp(tag, sender)
            colorLayer:removeFromParent()
            eventDispatcher:resumeEventListenersForTarget(self, true)
        end
        local closeItem = ax.MenuItemFont:create("close")
        closeItem:registerScriptTapHandler(closePopUp)
        closeItem:setPosition(VisibleRect:center())

        local closeMenu = ax.Menu:create(closeItem)
        closeMenu:setAnchorPoint(ax.p(0.0, 0.0))
        closeMenu:setPosition(ax.p(0.0, 0.0))

        colorLayer:addChild(closeMenu)

    end
    local popUpItem = ax.MenuItemFont:create("Popup")
    popUpItem:registerScriptTapHandler(popUp)
    popUpItem:setAnchorPoint(ax.p(1.0, 0.5))
    popUpItem:setPosition(VisibleRect:right())

    local menu = ax.Menu:create(popUpItem)
    menu:setAnchorPoint(ax.p(0.0, 0.0))
    menu:setPosition(ax.p(0.0, 0.0))

    self:addChild(menu)
end

function PauseResumeTargetTest:onExit()

end

function PauseResumeTargetTest.create()
    local layer = PauseResumeTargetTest.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)

    return layer
end

local Issue4129Test = class("Issue4129Test",EventDispatcherTestDemo)
Issue4129Test._customListener = nil

function Issue4129Test.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, Issue4129Test)
    return target
end

function Issue4129Test:onEnter()
    local bugFixed = false
    local eventDispatcher = self:getEventDispatcher()

    local function eventCustomListener(event)
        local label = ax.Label:createWithSystemFont("Yeah, this issue was fixed.", "", 20)
        label:setAnchorPoint(ax.p(0, 0.5))
        label:setPosition(VisibleRect:left())
        self:addChild(label)

        eventDispatcher:removeEventListener(self._customListener)
        self._customListener = nil

        bugFixed = true
    end

    self._customListener = ax.EventListenerCustom:create("event_come_to_background",eventCustomListener)
    eventDispatcher:addEventListenerWithFixedPriority(self._customListener, 1)

    local function removeAllTouch(tag, sender)
        local senderItem = sender
        senderItem:setString("Only 'Reset' item could be clicked")

        eventDispatcher:removeAllEventListeners()

        local function next(tag, sender)
            assert(bugFixed, "This issue was not fixed!")
            self.restartCallback()
        end
        local nextItem = ax.MenuItemFont:create("Reset")
        nextItem:registerScriptTapHandler(next)
        nextItem:setFontSizeObj(16)
        nextItem:setPosition(ax.p(VisibleRect:right().x - 100 , VisibleRect:right().y - 30))

        local menu2 = ax.Menu:create(nextItem)
        menu2:setPosition(ax.p(0, 0))
        menu2:setAnchorPoint(ax.p(0, 0))
        self:addChild(menu2)

        --Simulate to dispatch 'come to background' event
        local event = ax.EventCustom:new("event_come_to_background")
        eventDispatcher:dispatchEvent(event)
    end

    local removeAllTouchItem = ax.MenuItemFont:create("Remove All Listeners")
    removeAllTouchItem:registerScriptTapHandler(removeAllTouch)
    removeAllTouchItem:setFontSizeObj(16)
    removeAllTouchItem:setPosition(ax.p(VisibleRect:right().x - 100, VisibleRect:right().y))

    local menu = ax.Menu:create(removeAllTouchItem)
    menu:setPosition(ax.p(0, 0))
    menu:setAnchorPoint(ax.p(0, 0))
    self:addChild(menu)
end

function Issue4129Test:onExit()
    if nil ~= self._customListener then
        local eventDispatcher = self:getEventDispatcher()
        eventDispatcher:removeEventListener(self._customListener)
    end
end

function Issue4129Test.create()
    local layer = Issue4129Test.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)

    return layer
end

local Issue4160Test = class("Issue4160Test",EventDispatcherTestDemo)
Issue4160Test._customListener = nil

function Issue4160Test.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, Issue4160Test)
    return target
end

function Issue4160Test:onEnter()
    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local sprite1 = TouchableSpriteWithFixedPriority.create()
    sprite1:setPriority(-30)
    sprite1:setTexture("Images/CyanSquare.png")
    sprite1:setPosition(ax.p(origin.x + size.width/2 - 80, origin.y + size.height/2 + 40))
    self:addChild(sprite1, -10)

    local sprite2 = TouchableSpriteWithFixedPriority.create()
    sprite2:setPriority(-20)
    sprite2:setTexture("Images/MagentaSquare.png")
    sprite2:removeListenerOnTouchEnded(true)
    sprite2:setPosition(ax.p(origin.x + size.width/2, origin.y + size.height/2))
    self:addChild(sprite2, -20)

    local sprite3 = TouchableSpriteWithFixedPriority.create()
    sprite3:setPriority(-10)
    sprite3:setTexture("Images/YellowSquare.png")
    sprite3:setPosition(ax.p(0, 0))
    sprite2:addChild(sprite3, -1)
end

function Issue4160Test:onExit()

end

function Issue4160Test.create()
    local layer = Issue4160Test.extend(ax.Layer:create())

    local function onNodeEvent(event)
        if event == "enter" then
            layer:onEnter()
        elseif event == "exit" then
            layer:onExit()
        end
    end

    layer:createMenu()
    layer:creatTitleAndSubTitle(curLayerIdx)
    layer:registerScriptHandler(onNodeEvent)

    return layer
end


local createFunction =
{
    TouchableSpriteTest.create,
    FixedPriorityTest.create,
    RemoveListenerWhenDispatchingTest.create,
    CustomEventTest.create,
    LabelKeyboardEventTest.create,
    SpriteAccelerationEventTest.create,
    RemoveAndRetainNodeTest.create,
    RemoveListenerAfterAddingTest.create,
    GlobalZTouchTest.create,
    StopPropagationTest.create,
    PauseResumeTargetTest.create,
    Issue4129Test.create,
    Issue4160Test.create,
}

function nextEventDispatcherTest()
    curLayerIdx = curLayerIdx + 1
    curLayerIdx = curLayerIdx % #(createFunction)
    if 0 == curLayerIdx then
        curLayerIdx = #(createFunction)
    end
    return createFunction[curLayerIdx]()
end

function backEventDispatcherTest()
    curLayerIdx = curLayerIdx - 1
    if curLayerIdx <= 0 then
        curLayerIdx = curLayerIdx + #(createFunction)
    end

    return createFunction[curLayerIdx]()
end

function restartEventDispatcherTest()
    return createFunction[curLayerIdx]()
end


function NewEventDispatcherTest()
    local scene = EventDispatcherScene.create()
    curLayerIdx   = testArray.TouchableSprite
    scene:addChild(restartEventDispatcherTest())
    scene:addChild(CreateBackMenuItem())
    return scene
end
