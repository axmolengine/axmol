local winSize = ax.Director:getInstance():getLogicalSize()
local kTagSprite = 1
local childTag   = 119

local function createLayer()
    local layer = ax.Layer:create()
    local filename = ""

    local title = ax.Label:createWithTTF("New Renderer", "fonts/arial.ttf", 36)
    title:setColor(ax.color32(255,255,0))
    layer:addChild(title, 1, 10000)
    title:setPosition( ax.p(VisibleRect:center().x, VisibleRect:top().y - 30))

    local subTitle = ax.Label:createWithTTF("Capture screen test, press the menu items to capture the screen", "fonts/arial.ttf", 12)
    subTitle:setColor(ax.color32(255,255,0))
    layer:addChild(subTitle, 1, 10001)
    subTitle:setPosition( ax.p(VisibleRect:center().x, VisibleRect:top().y - 60) )

    local left  = ax.p(winSize.width / 4, winSize.height / 2)
    local right = ax.p(winSize.width / 4 * 3, winSize.height / 2)

    local sp1 = ax.Sprite:create("Images/grossini.png")
    sp1:setPosition(left)
    local move1 = ax.MoveBy:create(1, ax.p(winSize.width/2, 0))
    local seq1  = ax.RepeatForever:create(ax.Sequence:create(move1, move1:reverse()))
    layer:addChild(sp1)
    sp1:runAction(seq1)
    local sp2 = ax.Sprite:create("Images/grossinis_sister1.png")
    sp2:setPosition(right)
    local move2 = ax.MoveBy:create(1, ax.p(-winSize.width/2, 0))
    local seq2  = ax.RepeatForever:create(ax.Sequence:create(move2, move2:reverse()))
    layer:addChild(sp2)
    sp2:runAction(seq2)

    local function afterCaptured(succeed, outputFile)
        if succeed then
            local sp = ax.Sprite:create(outputFile)
            layer:addChild(sp, 0, childTag)
            sp:setPosition(winSize.width / 2, winSize.height / 2)
            sp:setScale(0.25)
            fileName = outputFile
        else
            cclog("Capture screen failed.")
        end
    end

    local function onCaptured(tag, sender)
        ax.Director:getInstance():getTextureCache():removeTextureForKey(fileName)
        layer:removeChildByTag(childTag)
        fileName = "CaptureScreenTest.png"
        ax.utils:captureScreen(afterCaptured, fileName)
    end

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize     = 24
    local label1 = ax.Label:createWithTTF(ttfConfig, "capture all", ax.TEXT_ALIGNMENT_CENTER, winSize.width)
    local mi1 = ax.MenuItemLabel:create(label1)
    mi1:registerScriptTapHandler(onCaptured)
    local menu = ax.Menu:create(mi1)
    layer:addChild(menu)
    menu:setPosition(winSize.width / 2, winSize.height / 4)

    return layer
end

--------------------------------
-- CaptureScreen
--------------------------------
function CaptureScreenTestMain()
    local scene = ax.Scene:create()
    scene:addChild(createLayer())
    scene:addChild(CreateBackMenuItem())
    return scene
end
