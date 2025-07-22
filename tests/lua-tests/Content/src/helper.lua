
AX_CONTENT_SCALE_FACTOR = function()
    return ax.Director:getInstance():getContentScaleFactor()
end


AX_POINT_PIXELS_TO_POINTS = function(pixels)
    return ax.p(pixels.x/AX_CONTENT_SCALE_FACTOR(), pixels.y/AX_CONTENT_SCALE_FACTOR())
end

AX_POINT_POINTS_TO_PIXELS = function(points)
    return ax.p(points.x*AX_CONTENT_SCALE_FACTOR(), points.y*AX_CONTENT_SCALE_FACTOR())
end


-- cclog
cclog = function(...)
    print(string.format(...))
end

-- change table to enum type
function CreateEnumTable(tbl, index)
    local enumTable = {}
    local enumIndex = index or -1
    for i, v in ipairs(tbl) do
        enumTable[v] = enumIndex + i
    end
    return enumTable
end

Helper = {
    index = 1,
    createFunctioinTable = nil,
    currentLayer = nil,
    titleLabel = nil,
    subtitleLabel = nil,
	restartTestItem = nil
}

function Helper.nextAction()
    Helper.index = Helper.index + 1
    if Helper.index > #(Helper.createFunctionTable) then
        Helper.index = 1
    end

    return Helper.newScene()
end

function  Helper.backAction()
    Helper.index = Helper.index - 1
    if Helper.index == 0 then
        Helper.index = #(Helper.createFunctionTable)
    end

    return Helper.newScene()
end

function Helper.restartAction()
    return Helper.newScene()
end

function Helper.newScene()
    local scene
    if Helper.usePhysics then
       scene = ax.Scene:createWithPhysics()
    else
       scene = ax.Scene:create()
    end
    Helper.currentLayer = Helper.createFunctionTable[Helper.index]()
    scene:addChild(Helper.currentLayer)
    scene:addChild(CreateBackMenuItem())

    ax.Director:getInstance():replaceScene(scene)
end

function Helper.initWithLayer(layer)
    Helper.currentLayer = layer

    local size = ax.Director:getInstance():getRenderView():getVisibleRect()
    Helper.titleLabel = ax.Label:createWithTTF("", s_arialPath, 28)
    Helper.titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(Helper.titleLabel, 1)
    Helper.titleLabel:setPosition(size.width / 2, size.height - 50)

    Helper.subtitleLabel = ax.Label:createWithTTF("", s_thonburiPath, 16)
    Helper.subtitleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(Helper.subtitleLabel, 1)
    Helper.subtitleLabel:setPosition(size.width / 2, size.height - 80)

    -- menu
    local item1 = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
    local item2 = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
    local item3 = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
    item1:registerScriptTapHandler(Helper.backAction)
    item2:registerScriptTapHandler(Helper.restartAction)
    item3:registerScriptTapHandler(Helper.nextAction)
	Helper.restartTestItem = item2

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:setPosition(ax.p(0, 0))
    item1:setPosition(ax.p(size.width / 2 - item2:getContentSize().width * 2, size.y + item2:getContentSize().height / 2))
    item2:setPosition(ax.p(size.width / 2, size.y + item2:getContentSize().height / 2))
    item3:setPosition(ax.p(size.width / 2 + item2:getContentSize().width * 2, size.y + item2:getContentSize().height / 2))
    layer:addChild(menu, 1)

    local background = ax.Layer:create()
    layer:addChild(background, -10)
end

-- back menu callback
local function MainMenuCallback()
    local scene = ax.Scene:create()
    scene:addChild(CreateTestMenu())
    Helper.usePhysics = false
    ax.Director:getInstance():getRenderer():setDepthTest(false)
    ax.Director:getInstance():replaceScene(scene)
end

-- add the menu item for back to main menu
function CreateBackMenuItem()
    local label = ax.Label:createWithTTF("MainMenu", s_arialPath, 20)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    local MenuItem = ax.MenuItemLabel:create(label)
    MenuItem:registerScriptTapHandler(MainMenuCallback)

    local s = ax.Director:getInstance():getRenderView():getVisibleRect()
    local Menu = ax.Menu:create()
    Menu:addChild(MenuItem)
    Menu:setPosition(0, 0)
    MenuItem:setPosition(s.width - 50, s.y + 25)

    return Menu
end

function createTestLayer(title, subtitle)
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    local titleStr = title == nil and "No title" or title
    local subTitleStr = subtitle  == nil and "" or subtitle
    Helper.titleLabel:setString(titleStr)
    Helper.subtitleLabel:setString(subTitleStr)
    return layer
end

TestCastScene = {
    index = 1,
    createFunctioinTable = nil,
    titleLabel = nil,
    subtitleLabel = nil
}

function TestCastScene.nextAction()
    TestCastScene.index = TestCastScene.index + 1
    if TestCastScene.index > #(TestCastScene.createFunctionTable) then
        TestCastScene.index = 1
    end

    return TestCastScene.newScene()
end

function  TestCastScene.backAction()
    TestCastScene.index = TestCastScene.index - 1
    if TestCastScene.index == 0 then
        TestCastScene.index = #(TestCastScene.createFunctionTable)
    end

    return TestCastScene.newScene()
end

function TestCastScene.restartAction()
    return TestCastScene.newScene()
end

function TestCastScene.newScene()
    local scene =  TestCastScene.createFunctionTable[TestCastScene.index]()
    scene:addChild(CreateBackMenuItem())
    ax.Director:getInstance():replaceScene(scene)
end

function TestCastScene.initWithLayer(scene)
    local size = ax.Director:getInstance():getVisibleSize()
    local origin = ax.Director:getInstance():getVisibleOrigin()

    TestCastScene.titleLabel = ax.Label:createWithTTF("", s_arialPath, 28)
    TestCastScene.titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    scene:addChild(TestCastScene.titleLabel, 1)
    TestCastScene.titleLabel:setPosition(origin.x + size.width / 2, origin.y + size.height - 50)

    TestCastScene.subtitleLabel = ax.Label:createWithTTF("", s_thonburiPath, 16)
    TestCastScene.subtitleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    scene:addChild(TestCastScene.subtitleLabel, 1)
    TestCastScene.subtitleLabel:setPosition(origin.x + size.width / 2, origin.y + size.height - 80)

    -- menu
    local item1 = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
    local item2 = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
    local item3 = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
    item1:registerScriptTapHandler(TestCastScene.backAction)
    item2:registerScriptTapHandler(TestCastScene.restartAction)
    item3:registerScriptTapHandler(TestCastScene.nextAction)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:setPosition(ax.p(0, 0))
    item1:setPosition(ax.p(origin.x + size.width / 2 - item2:getContentSize().width * 2, origin.y + size.height + item2:getContentSize().height / 2))
    item2:setPosition(ax.p(origin.x + size.width / 2, origin.y + size.height + item2:getContentSize().height / 2))
    item3:setPosition(ax.p(origin.x + size.width / 2 + item2:getContentSize().width * 2, origin.y + size.height + item2:getContentSize().height / 2))
    scene:addChild(menu, 1)

    local background = ax.Layer:create()
    scene:addChild(background, -10)
end
