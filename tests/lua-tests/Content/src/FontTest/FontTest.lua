local  kTagLabel1 = 0
local  kTagLabel2 = 1
local  kTagLabel3 = 2
local  kTagLabel4 = 3

local  fontList = {
    "fonts/A Damn Mess.ttf",
    "fonts/Abberancy.ttf",
    "fonts/Abduction.ttf",
    "fonts/Paint Boy.ttf",
    "fonts/Schwarzwald.ttf",
    "fonts/Scissor Cuts.ttf"
}

local fontCount = #(fontList)
cclog("font count = "..fontCount)

local vAlignIdx = 1
local verticalAlignment = {
    ax.VERTICAL_TEXT_ALIGNMENT_TOP,
    ax.VERTICAL_TEXT_ALIGNMENT_CENTER,
    ax.VERTICAL_TEXT_ALIGNMENT_BOTTOM,
}

local vAlignCount = #(verticalAlignment)

local function showFont(ret, pFont)

    cclog("vAlignIdx="..vAlignIdx)
    local s = ax.Director:getInstance():getWinSize()

    local blockSize = ax.size(s.width/3, 200)
    local fontSize = 26

    local top = ax.Label:createWithTTF(pFont, pFont, 24)
    local left = ax.Label:createWithTTF("alignment left", pFont, fontSize,
                                   blockSize, ax.TEXT_ALIGNMENT_LEFT, verticalAlignment[vAlignIdx])
    local center = ax.Label:createWithTTF("alignment center", pFont, fontSize,
                                     blockSize, ax.TEXT_ALIGNMENT_CENTER, verticalAlignment[vAlignIdx])
    local right = ax.Label:createWithTTF("alignment right", pFont, fontSize,
                                    blockSize, ax.TEXT_ALIGNMENT_RIGHT, verticalAlignment[vAlignIdx])

    local leftColor = ax.LayerColor:create(ax.color32(100, 100, 100, 255), blockSize.width, blockSize.height)
    local centerColor = ax.LayerColor:create(ax.color32(200, 100, 100, 255), blockSize.width, blockSize.height)
    local rightColor = ax.LayerColor:create(ax.color32(100, 100, 200, 255), blockSize.width, blockSize.height)

    leftColor:setIgnoreAnchorPointForPosition(false)
    centerColor:setIgnoreAnchorPointForPosition(false)
    rightColor:setIgnoreAnchorPointForPosition(false)

    top:setAnchorPoint(ax.p(0.5, 1))
    left:setAnchorPoint(ax.p(0,0.5))
    leftColor:setAnchorPoint(ax.p(0,0.5))
    center:setAnchorPoint(ax.p(0,0.5))
    centerColor:setAnchorPoint(ax.p(0,0.5))
    right:setAnchorPoint(ax.p(0,0.5))
    rightColor:setAnchorPoint(ax.p(0,0.5))

    top:setPosition(ax.p(s.width/2,s.height-20))
    left:setPosition(ax.p(0,s.height/2))
    leftColor:setPosition(left:getPosition())
    center:setPosition(ax.p(blockSize.width, s.height/2))
    centerColor:setPosition(center:getPosition())
    right:setPosition(ax.p(blockSize.width*2, s.height/2))
    rightColor:setPosition(right:getPosition())

    ret:addChild(leftColor, -1)
    ret:addChild(left, 0, kTagLabel1)
    ret:addChild(rightColor, -1)
    ret:addChild(right, 0, kTagLabel2)
    ret:addChild(centerColor, -1)
    ret:addChild(center, 0, kTagLabel3)
    ret:addChild(top, 0, kTagLabel4)
end

local isFirst = true
local originCreateLayer = createTestLayer
local function createTestLayer()
    if isFirst == false then
        if Helper.index == 1 then
            vAlignIdx =  vAlignIdx % vAlignCount + 1
        end
    else
        isFirst = false
    end

    local ret = originCreateLayer("")
    showFont(ret, fontList[Helper.index])
    return ret
end

function FontTestMain()
    cclog("FontTestMain")
    Helper.index = 1
    vAlignIdx = 1
    local scene = ax.Scene:create()
    Helper.createFunctionTable = {
        createTestLayer,
        createTestLayer,
        createTestLayer,
        createTestLayer,
        createTestLayer,
        createTestLayer
    }
    Helper.index = 1

    scene:addChild(createTestLayer())
    scene:addChild(CreateBackMenuItem())
    return scene
end
