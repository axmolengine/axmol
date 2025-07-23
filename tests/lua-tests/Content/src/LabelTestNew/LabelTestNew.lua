local size = ax.Director:getInstance():getWinSize()
local scheduler = ax.Director:getInstance():getScheduler()

local kTagTileMap = 1
local kTagSpriteManager = 1
local kTagAnimation1 = 1
local kTagBitmapAtlas1 = 1
local kTagBitmapAtlas2 = 2
local kTagBitmapAtlas3 = 3
local kTagSprite1 = 0
local kTagSprite2 = 1
local kTagSprite3 = 2
local kTagSprite4 = 3
local kTagSprite5 = 4
local kTagSprite6 = 5
local kTagSprite7 = 6
local kTagSprite8 = 7

--------------------------------------------------------------------
--
-- LabelFNTColorAndOpacity
--
--------------------------------------------------------------------
local LabelFNTColorAndOpacity = {}
LabelFNTColorAndOpacity.layer = nil
LabelFNTColorAndOpacity.__index = LabelFNTColorAndOpacity

function LabelFNTColorAndOpacity.onNodeEvent(tag)
    if tag == "exit" then
        LabelFNTColorAndOpacity.layer:unscheduleUpdate()
    elseif tag == "enter" then
        if nil ~= LabelFNTColorAndOpacity.layer then
            LabelFNTColorAndOpacity.layer:scheduleUpdateWithPriorityLua(LabelFNTColorAndOpacity.step, 0)
        end
    end
end

function LabelFNTColorAndOpacity.create()
    cclog("LabelFNTColorAndOpacity.create")
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelFNTColorAndOpacity.layer = layer

    m_time = 0

    local col = ax.LayerColor:create( ax.color32(128,128,128,255) )
    layer:addChild(col, -10)

    local label1 = ax.Label:createWithBMFont("fonts/bitmapFontTest2.fnt", "Test")

    -- testing anchors
    label1:setAnchorPoint( ax.p(0,0) )
    layer:addChild(label1, 0, kTagBitmapAtlas1)

    local fade = ax.FadeOut:create(1.0)
    local fade_in = fade:reverse()

    local seq = ax.Sequence:create(fade,fade_in)
    local repeatAction = ax.RepeatForever:create(seq)
    label1:runAction(repeatAction)

    local label2 = ax.Label:createWithBMFont("fonts/bitmapFontTest2.fnt", "Test")
    -- testing anchors
    label2:setAnchorPoint( ax.p(0.5, 0.5) )
    label2:setColor(ax.color32(255, 0, 0 ))
    layer:addChild(label2, 0, kTagBitmapAtlas2)

    label2:runAction(repeatAction:clone())

    local label3 = ax.Label:createWithBMFont("fonts/bitmapFontTest2.fnt", "Test")
    -- testing anchors
    label3:setAnchorPoint( ax.p(1,1) )
    layer:addChild(label3, 0, kTagBitmapAtlas3)

    label1:setPosition( VisibleRect:leftBottom() )
    label2:setPosition( VisibleRect:center() )
    label3:setPosition( VisibleRect:rightTop() )

    layer:registerScriptHandler(LabelFNTColorAndOpacity.onNodeEvent)

    Helper.titleLabel:setString( "New Label + .FNT file" )
    Helper.subtitleLabel:setString( "Testing opacity + tint" )

    return layer
end

function LabelFNTColorAndOpacity.step(dt)
    m_time = m_time + dt
    local string = string.format("%2.2f Test j", m_time)

    local label1 = LabelFNTColorAndOpacity.layer:getChildByTag(kTagBitmapAtlas1)
    label1:setString(string)

    local label2 = LabelFNTColorAndOpacity.layer:getChildByTag(kTagBitmapAtlas2)
    label2:setString(string)

    local label3 = LabelFNTColorAndOpacity.layer:getChildByTag(kTagBitmapAtlas3)
    label3:setString(string)
end

--------------------------------------------------------------------
--
-- LabelFNTSpriteActions
--
--------------------------------------------------------------------
local LabelFNTSpriteActions = {}
LabelFNTSpriteActions.layer = nil
LabelFNTSpriteActions.__index = LabelFNTSpriteActions
LabelFNTSpriteActions.stepEntry = nil

function LabelFNTSpriteActions.onNodeEvent(tag)
    if tag == "enter" then
        LabelFNTSpriteActions.stepEntry = scheduler:scheduleScriptFunc(LabelFNTSpriteActions.step, 0.1, false)
    elseif tag == "exit" then
        scheduler:unscheduleScriptEntry(LabelFNTSpriteActions.stepEntry)
    end
end

function LabelFNTSpriteActions.create()
    cclog("LabelFNTSpriteActions.create")
    m_time = 0
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelFNTSpriteActions.layer = layer

    local s = ax.Director:getInstance():getWinSize()

    local drawNode = ax.DrawNode:create()
    drawNode:drawLine( ax.p(0, s.height/2), ax.p(s.width, s.height/2), ax.color(1,1,1,1))
    drawNode:drawLine( ax.p(s.width/2, 0), ax.p(s.width/2, s.height), ax.color(1,1,1,1))
    layer:addChild(drawNode, -10)

    -- Upper Label
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt", "Bitmap Font Atlas")
    layer:addChild(label)


    label:setPosition( ax.p(s.width/2, s.height/2) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )


    local BChar = label:getLetter(0)
    local FChar = label:getLetter(7)
    local AChar = label:getLetter(12)


    local rotate = ax.RotateBy:create(2, 360)
    local rot_4ever = ax.RepeatForever:create(rotate)

    local scale = ax.ScaleBy:create(2, 1.5)
    local scale_back = scale:reverse()

    local scale_seq = ax.Sequence:create(scale, scale_back)
    local scale_4ever = ax.RepeatForever:create(scale_seq)

    local jump = ax.JumpBy:create(0.5, ax.p(0, 0), 60, 1)
    local jump_4ever = ax.RepeatForever:create(jump)

    local fade_out = ax.FadeOut:create(1)
    local fade_in = ax.FadeIn:create(1)

    local seq = ax.Sequence:create(fade_out, fade_in)
    local fade_4ever = ax.RepeatForever:create(seq)

    BChar:runAction(rot_4ever)
    BChar:runAction(scale_4ever)
    FChar:runAction(jump_4ever)
    AChar:runAction(fade_4ever)


    -- Bottom Label
    local label2 = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt", "00.0")
    layer:addChild(label2, 0, kTagBitmapAtlas2)
    label2:setPosition( ax.p(s.width/2.0, 80) )

    local lastChar = label2:getLetter(3)
    lastChar:runAction(rot_4ever:clone())

    layer:registerScriptHandler(LabelFNTSpriteActions.onNodeEvent)

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString( "Using fonts as Sprite objects. Some characters should rotate.")
    return layer
end

function LabelFNTSpriteActions.step(dt)
    m_time = m_time + dt

    local string = string.format("%04.1f", m_time)

    local label1 = LabelFNTSpriteActions.layer:getChildByTag(kTagBitmapAtlas2)
    label1:setString(string)
end


--------------------------------------------------------------------
--
-- LabelFNTPadding
--
--------------------------------------------------------------------

local LabelFNTPadding = {}
LabelFNTPadding.layer = nil
function LabelFNTPadding:create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelFNTPadding.layer = layer

    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest4.fnt", "abcdefg")
    layer:addChild(label)

    local s = ax.Director:getInstance():getWinSize()

    label:setPosition( ax.p(s.width/2, s.height/2) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Testing padding")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTOffset
--
--------------------------------------------------------------------
local LabelFNTOffset = {}
LabelFNTOffset.layer = nil

function LabelFNTOffset:create()
    cclog("LabelFNTOffset:create")
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelFNTOffset.layer = layer

    local s = ax.Director:getInstance():getWinSize()
    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "FaFeFiFoFu")
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, s.height/2+50) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )

    label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "fafefifofu")
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, s.height/2) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )

    label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "aeiou")
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, s.height/2-50) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Rendering should be OK. Testing offset")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTColor
--
--------------------------------------------------------------------
local LabelFNTColor = { layer= nil }
function LabelFNTColor:create()
    local layer = ax.Layer:create()
    LabelFNTColor.layer = layer
    Helper.initWithLayer(layer)

    local s = ax.Director:getInstance():getWinSize()

    local label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "Blue")
    label:setColor( ax.color32(0, 0, 255 ))
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, s.height/4) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )

    label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "Red")
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, 2*s.height/4) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )
    label:setColor( ax.color32(255, 0, 0) )

    label = ax.Label:createWithBMFont("fonts/bitmapFontTest5.fnt", "Green")
    layer:addChild(label)
    label:setPosition( ax.p(s.width/2, 3*s.height/4) )
    label:setAnchorPoint( ax.p(0.5, 0.5) )
    label:setColor( ax.color32(0, 255, 0 ))
    label:setString("Green")

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Testing color")
    return layer
end


--------------------------------------------------------------------
--
-- LabelTTFColor
--
--------------------------------------------------------------------
local LabelTTFColor = { layer= nil }
function LabelTTFColor:create()
    local layer = ax.Layer:create()
    LabelTTFColor.layer = layer
    Helper.initWithLayer(layer)

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath="fonts/arial.ttf"
    ttfConfig.fontSize=18

    local label1 = ax.Label:createWithTTF(ttfConfig,"Green", ax.VERTICAL_TEXT_ALIGNMENT_CENTER, s.width)
    label1:setTextColor( ax.color32(0, 255, 0, 255))
    layer:addChild(label1)
    label1:setPosition( ax.p(s.width/2, s.height/5 * 1.5) )
    label1:setAnchorPoint( ax.p(0.5, 0.5) )

    local label2 = ax.Label:createWithTTF(ttfConfig, "Red", ax.VERTICAL_TEXT_ALIGNMENT_CENTER, s.width)
    layer:addChild(label2)
    label2:setPosition( ax.p(s.width/2, s.height/5 * 2.0) )
    label2:setAnchorPoint( ax.p(0.5, 0.5) )
    label2:setTextColor( ax.color32(255, 0, 0, 255) )

    local label3 = ax.Label:createWithTTF(ttfConfig, "Blue", ax.VERTICAL_TEXT_ALIGNMENT_CENTER, s.width)
    layer:addChild(label3)
    label3:setPosition( ax.p(s.width/2, s.height/5 * 2.5) )
    label3:setAnchorPoint( ax.p(0.5, 0.5) )
    label3:setTextColor( ax.color32(0, 0, 255, 255))

    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Uses the new Label with TTF. Testing Color")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTHundredLabels
--
--------------------------------------------------------------------

local LabelFNTHundredLabels = { layer = nil }
function LabelFNTHundredLabels:create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelFNTHundredLabels.layer = layer

    math.randomseed(os.time())
    -- Upper Label
    local i = 0
    for i = 0, 100, 1 do
        local str = string.format("-%d-", i)
        local label = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt", str)
        layer:addChild(label)

        local s = ax.Director:getInstance():getWinSize()

        local p = ax.p( math.random() * s.width, math.random() * s.height)
        label:setPosition( p )
        label:setAnchorPoint(ax.p(0.5, 0.5))
    end

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Creating several Labels using the same FNT file should be fast")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTMultiLine
--
--
--------------------------------------------------------------------
local LabelFNTMultiLine = {}

function LabelFNTMultiLine:create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    local s = nil

    -- Left
    local label1 = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", " Multi line\nLeft")
    label1:setAnchorPoint(ax.p(0,0))
    layer:addChild(label1, 0, kTagBitmapAtlas1)

    s = label1:getContentSize()
    cclog("content size: %.2fx%.2f", s.width, s.height)


    -- Center
    local label2 = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "Multi line\nCenter")
    label2:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(label2, 0, kTagBitmapAtlas2)

    s= label2:getContentSize()
    cclog("content size: %.2fx%.2f", s.width, s.height)

    -- right
    local label3 = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "Multi line\nRight\nThree lines Three")
    label3:setAnchorPoint(ax.p(1, 1))
    layer:addChild(label3, 0, kTagBitmapAtlas3)

    s = label3:getContentSize()
    cclog("content size: %.2fx%.2f", s.width, s.height)

    label1:setPosition(VisibleRect:leftBottom())
    label2:setPosition(VisibleRect:center())
    label3:setPosition(VisibleRect:rightTop())
    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Multiline + anchor point")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTandTTFEmpty
--
--------------------------------------------------------------------

local LabelFNTandTTFEmpty = {}
LabelFNTandTTFEmpty.layer = nil
LabelFNTandTTFEmpty.setEmpty = false
LabelFNTandTTFEmpty.updateEntry = nil

function LabelFNTandTTFEmpty.onNodeEvent(tag)
    if tag == "enter" then
        LabelFNTandTTFEmpty.updateEntry = scheduler:scheduleScriptFunc(LabelFNTandTTFEmpty.updateStrings, 1.0, false)
    elseif tag == "exit" then
        scheduler:unscheduleScriptEntry(LabelFNTandTTFEmpty.updateEntry)
    end
end


function LabelFNTandTTFEmpty.create()
    cclog("LabelFNTandTTFEmpty.create")

    local layer = ax.Layer:create()
    LabelFNTandTTFEmpty.layer = layer
    Helper.initWithLayer(layer)

    local s = ax.Director:getInstance():getWinSize()

    -- ax.LabelBMFont
    local label1 = ax.Label:createWithBMFont("fonts/bitmapFontTest3.fnt", "", ax.TEXT_ALIGNMENT_CENTER,s.width)
    layer:addChild(label1, 0, kTagBitmapAtlas1)
    label1:setPosition(ax.p(s.width/2, s.height-100))

    -- ax.LabelTTF
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize     = 24
    local label2 = ax.Label:createWithTTF(ttfConfig, "", ax.TEXT_ALIGNMENT_CENTER, s.width)
    layer:addChild(label2, 0, kTagBitmapAtlas2)
    label2:setAnchorPoint(ax.p(0.5, 0.5))
    label2:setPosition(ax.p(s.width/2, s.height/2))

    layer:registerScriptHandler(LabelFNTandTTFEmpty.onNodeEvent)

    LabelFNTandTTFEmpty.setEmpty = false
    Helper.titleLabel:setString("New Label : .FNT file & .TTF file")
    Helper.subtitleLabel:setString("2 empty labels: new Label + .FNT and new Label + .TTF")
    return layer
end

function LabelFNTandTTFEmpty.updateStrings(dt)
    local label1 = LabelFNTandTTFEmpty.layer:getChildByTag(kTagBitmapAtlas1)
    local label2 = LabelFNTandTTFEmpty.layer:getChildByTag(kTagBitmapAtlas2)

    if( LabelFNTandTTFEmpty.setEmpty == false) then
        label1:setString("not empty")
        label2:setString("not empty")
        LabelFNTandTTFEmpty.setEmpty = true
    else
        label1:setString("")
        label2:setString("")
        LabelFNTandTTFEmpty.setEmpty = false
    end
end

--------------------------------------------------------------------
--
-- LabelFNTRetina
--
--------------------------------------------------------------------
local LabelFNTRetina = {

}

function LabelFNTRetina.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)

    local s = ax.Director:getInstance():getWinSize()

    -- ax.LabelBMFont
    local label1 = ax.Label:createWithBMFont("fonts/konqa32.fnt", "TESTING RETINA DISPLAY")
    label1:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label1)
    label1:setPosition(ax.p(s.width/2, s.height/2))

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("loading arista16 or arista16-hd")
    return layer
end

--------------------------------------------------------------------
--
-- LabelFNTGlyphDesigner
--
--------------------------------------------------------------------
local LabelFNTGlyphDesigner = {}
function LabelFNTGlyphDesigner.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)

    local s = ax.Director:getInstance():getWinSize()

    local label1 = ax.Label:createWithBMFont("fonts/futura-48.fnt", "TESTING RETINA DISPLAY")
    label1:setAnchorPoint(ax.p(0.5, 0.5))

    layer:addChild(label1)
    label1:setPosition(ax.p(s.width/2, s.height/2))

    Helper.titleLabel:setString("New Label + .FNT file")
    Helper.subtitleLabel:setString("Testing Glyph Designer: you should see a font with shadows and outline")
    return layer
end

--/ LabelTTFUnicodeChinese
local LabelTTFUnicodeChinese = {}
function LabelTTFUnicodeChinese.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF file Chinese")
    Helper.subtitleLabel:setString("Testing new Label + TTF with Chinese character")

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath="fonts/HKYuanMini.ttf"
    ttfConfig.fontSize=28
    ttfConfig.glyphs=ax.GLYPHCOLLECTION_CUSTOM
    ttfConfig.customGlyphs="美好的一天啊"

    local label1 = ax.Label:createWithTTF(ttfConfig,"美好的一天啊", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label1)
    label1:setPosition(ax.p(s.width/2, s.height/2))

    return layer
end

------------
-- LabelFNTUnicodeChinese
------------

local LabelFNTUnicodeChinese = {}
function LabelFNTUnicodeChinese.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)

    local size = ax.Director:getInstance():getWinSize()
    local lable = ax.Label:createWithBMFont("fonts/bitmapFontChinese.fnt", "中国")
    lable:setAnchorPoint(ax.p(0.5,0.5))
    lable:setPosition(ax.p(size.width / 2, size.height /2))
    layer:addChild(lable)

    Helper.titleLabel:setString("New Label + .FNT file Chinese")
    Helper.subtitleLabel:setString("Testing new Label + FNT with Chinese character")

    return layer
end

-- LabelFNTMultiLineAlignment

local  LongSentencesExample =  "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
local  LineBreaksExample    = "Lorem ipsum dolor\nsit amet\nconsectetur adipisicing elit\nblah\nblah"
local  MixedExample         = "ABC\nLorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt\nDEF"

local  ArrowsMax             = 0.95
local  ArrowsMin             = 0.7

local  LeftAlign             = 0
local  CenterAlign           = 1
local  RightAlign            = 2

local  LongSentences         = 0
local  LineBreaks            = 1
local  Mixed                 = 2

local  alignmentItemPadding  = 50
local  menuItemPaddingCenter = 50

local LabelFNTMultiLineAlignment = {
    _pLabelShouldRetain = nil,
    _pArrowsBarShouldRetain = nil,
    _pArrowsShouldRetain = nil,
    _pLastSentenceItem = nil,
    _drag = false,
}

function LabelFNTMultiLineAlignment.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    -- ask director the the window size
    local size = ax.Director:getInstance():getWinSize()

    -- create and initialize a Label
    LabelFNTMultiLineAlignment._pLabelShouldRetain = ax.Label:createWithBMFont("fonts/markerFelt.fnt", LongSentencesExample, ax.TEXT_ALIGNMENT_CENTER, size.width/1.5)
    LabelFNTMultiLineAlignment._pLabelShouldRetain:setAnchorPoint(ax.p(0.5, 0.5))
    LabelFNTMultiLineAlignment._pLabelShouldRetain:retain()

    LabelFNTMultiLineAlignment._pArrowsBarShouldRetain = ax.Sprite:create("Images/arrowsBar.png")
    LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:retain()
    LabelFNTMultiLineAlignment._pArrowsShouldRetain = ax.Sprite:create("Images/arrows.png")
    LabelFNTMultiLineAlignment._pArrowsShouldRetain:retain()

    ax.MenuItemFont:setFontSize(20)
    local longSentences = ax.MenuItemFont:create("Long Flowing Sentences")
    longSentences:registerScriptTapHandler(LabelFNTMultiLineAlignment.stringChanged)
    local lineBreaks    = ax.MenuItemFont:create("Short Sentences With Intentional Line Breaks")
    lineBreaks:registerScriptTapHandler(LabelFNTMultiLineAlignment.stringChanged)
    local mixed         = ax.MenuItemFont:create("Long Sentences Mixed With Intentional Line Breaks")
    mixed:registerScriptTapHandler(LabelFNTMultiLineAlignment.stringChanged)
    local stringMenu    = ax.Menu:create()
    stringMenu:addChild(longSentences)
    stringMenu:addChild(lineBreaks)
    stringMenu:addChild(mixed)
    stringMenu:alignItemsVertically()

    longSentences:setColor(ax.color32(255, 0, 0))
    LabelFNTMultiLineAlignment._pLastSentenceItem = longSentences

    longSentences:setTag(LongSentences)
    lineBreaks:setTag(LineBreaks)
    mixed:setTag(Mixed)

    ax.MenuItemFont:setFontSize(30)

    local left          = ax.MenuItemFont:create("Left")
    left:registerScriptTapHandler(LabelFNTMultiLineAlignment.alignmentChanged)
    local center        = ax.MenuItemFont:create("Center")
    center:registerScriptTapHandler(LabelFNTMultiLineAlignment.alignmentChanged)
    local right         = ax.MenuItemFont:create("Right")
    right:registerScriptTapHandler(LabelFNTMultiLineAlignment.alignmentChanged)

    local alignmentMenu = ax.Menu:create()
    alignmentMenu:addChild(left)
    alignmentMenu:addChild(center)
    alignmentMenu:addChild(right)

    alignmentMenu:alignItemsHorizontallyWithPadding(alignmentItemPadding)

    center:setColor(ax.color32(255, 0, 0))
    LabelFNTMultiLineAlignment._pLastAlignmentItem = center
    left:setTag(LeftAlign)
    center:setTag(CenterAlign)
    right:setTag(RightAlign)

    -- position the label on the center of the screen
    LabelFNTMultiLineAlignment._pLabelShouldRetain:setPosition(ax.p(size.width/2, size.height/2))

    LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:setVisible(false)

    local arrowsWidth = (ArrowsMax - ArrowsMin) * size.width
    LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:setScaleX(arrowsWidth / LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:getContentSize().width)
    LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:setPosition( ((ArrowsMax + ArrowsMin) / 2) * size.width, LabelFNTMultiLineAlignment._pLabelShouldRetain:getPositionY() )

    LabelFNTMultiLineAlignment.snapArrowsToEdge()

    stringMenu:setPosition(ax.p(size.width/2, size.height - menuItemPaddingCenter))
    alignmentMenu:setPosition(ax.p(size.width/2, menuItemPaddingCenter+15))

    layer:addChild(LabelFNTMultiLineAlignment._pLabelShouldRetain)
    layer:addChild(LabelFNTMultiLineAlignment._pArrowsBarShouldRetain)
    layer:addChild(LabelFNTMultiLineAlignment._pArrowsShouldRetain)
    layer:addChild(stringMenu)
    layer:addChild(alignmentMenu)
    layer:registerScriptHandler(LabelFNTMultiLineAlignment.onNodeEvent)

    local function onTouchesBegan(touches, event)
        local location = touches[1]:getLocationInView()
        if ax.rectContainsPoint(LabelFNTMultiLineAlignment._pArrowsShouldRetain:getBoundingBox(), ax.p(location.x, location.y)) then
            LabelFNTMultiLineAlignment._drag = true
            LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:setVisible(true)
        end
    end

    local function onTouchesMoved(touches, event)
       if LabelFNTMultiLineAlignment._drag == false then
            return
        end

        local winSize = ax.Director:getInstance():getWinSize()
        local location = touches[1]:getLocationInView()

        LabelFNTMultiLineAlignment._pArrowsShouldRetain:setPosition(
            math.max(math.min(location.x, ArrowsMax*winSize.width), ArrowsMin*winSize.width),
            LabelFNTMultiLineAlignment._pArrowsShouldRetain:getPositionY())

        local labelWidth = math.abs(LabelFNTMultiLineAlignment._pArrowsShouldRetain:getPositionX() - LabelFNTMultiLineAlignment._pLabelShouldRetain:getPositionX()) * 2

        LabelFNTMultiLineAlignment._pLabelShouldRetain:setMaxLineWidth(labelWidth)
    end

    local  function onTouchesEnded(touch, event)
        LabelFNTMultiLineAlignment._drag = false
        LabelFNTMultiLineAlignment.snapArrowsToEdge()
        LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:setVisible(false)
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesBegan,ax.Handler.EVENT_TOUCHES_BEGAN )
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )
    listener:registerScriptHandler(onTouchesEnded,ax.Handler.EVENT_TOUCHES_ENDED )

    local eventDispatcher = layer:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, layer)

    return layer
end

function LabelFNTMultiLineAlignment.onNodeEvent(tag)
    if tag == "onexit" then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:release()
        LabelFNTMultiLineAlignment._pArrowsBarShouldRetain:release()
        LabelFNTMultiLineAlignment._pArrowsShouldRetain:release()
    end
end



function LabelFNTMultiLineAlignment.stringChanged(tag, sender)
    local item = sender
    item:setColor(ax.color32(255, 0, 0))
    LabelFNTMultiLineAlignment._pLastAlignmentItem:setColor(ax.color32(255, 255, 255))
    LabelFNTMultiLineAlignment._pLastAlignmentItem = item

    if item:getTag() == LongSentences then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setString(LongSentencesExample)
    elseif item:getTag() == LineBreaks then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setString(LineBreaksExample)
    elseif item:getTag() == Mixed then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setString(MixedExample)
    end

    LabelFNTMultiLineAlignment.snapArrowsToEdge()
end

function LabelFNTMultiLineAlignment.alignmentChanged(tag, sender)
    -- cclog("LabelFNTMultiLineAlignment.alignmentChanged, tag:"..tag)
    local item = sender
    item:setColor(ax.color32(255, 0, 0))
    LabelFNTMultiLineAlignment._pLastAlignmentItem:setColor(ax.color32(255, 255, 255))
    LabelFNTMultiLineAlignment._pLastAlignmentItem = item

    if tag == LeftAlign then
        cclog("LeftAlign")
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setAlignment(ax.TEXT_ALIGNMENT_LEFT)
    elseif tag == CenterAlign then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setAlignment(ax.TEXT_ALIGNMENT_CENTER)
    elseif tag == RightAlign then
        LabelFNTMultiLineAlignment._pLabelShouldRetain:setAlignment(ax.TEXT_ALIGNMENT_RIGHT)
    end

    LabelFNTMultiLineAlignment.snapArrowsToEdge()
end

function LabelFNTMultiLineAlignment.snapArrowsToEdge()
    LabelFNTMultiLineAlignment._pArrowsShouldRetain:setPosition(
        LabelFNTMultiLineAlignment._pLabelShouldRetain:getPositionX() + LabelFNTMultiLineAlignment._pLabelShouldRetain:getContentSize().width/2, LabelFNTMultiLineAlignment._pLabelShouldRetain:getPositionY()
    )
end

------------------------------------
----- LabelFNTUNICODELanguages
------------------------------------
local LabelFNTUNICODELanguages = {}
function LabelFNTUNICODELanguages.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .FNT + UNICODE")
    Helper.subtitleLabel:setString("You should see 3 different labels:\nIn Spanish, Chinese, and Japanese")

    local s = ax.Director:getInstance():getWinSize()

    local label1 = ax.Label:createWithBMFont("fonts/arial-unicode-26.fnt", "Buen día", ax.TEXT_ALIGNMENT_CENTER, 200)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label1)
    label1:setPosition(ax.p(s.width/2, s.height/4*3))

    local label2 = ax.Label:createWithBMFont("fonts/arial-unicode-26.fnt", "美好的一天")
    label2:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label2)
    label2:setPosition(ax.p(s.width/2, s.height/4*2))

    local label3 = ax.Label:createWithBMFont("fonts/arial-unicode-26.fnt", "良い一日を")
    label3:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label3)
    label3:setPosition(ax.p(s.width/2, s.height/4*1))

    return layer
end

----------------------------------------------------------
----- LabelTTFAlignmentNew
----------------------------------------------------------
local LabelTTFAlignmentNew = {}
function LabelTTFAlignmentNew.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + TTF")
    Helper.subtitleLabel:setString("Tests alignment values")

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath="fonts/tahoma.ttf"
    ttfConfig.fontSize=16
    local ttf0 = ax.Label:createWithTTF(ttfConfig, "Alignment 0\nnew line", ax.TEXT_ALIGNMENT_LEFT)
    ttf0:setPosition(ax.p(s.width/2,(s.height/6)*2 - 30))
    ttf0:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(ttf0)

    local ttf1 = ax.Label:createWithTTF(ttfConfig, "Alignment 1\nnew line", ax.TEXT_ALIGNMENT_CENTER)
    ttf1:setPosition(ax.p(s.width/2,(s.height/6)*3 - 30))
    ttf1:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(ttf1)

    local ttf2 = ax.Label:createWithTTF(ttfConfig, "Alignment 2\nnew line", ax.TEXT_ALIGNMENT_RIGHT)
    ttf2:setPosition(ax.p(s.width/2,(s.height/6)*4 - 30))
    ttf2:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(ttf2)

    return layer
end


-------------------------------------
----- LabelTTFUnicodeNew
-------------------------------------
local LabelTTFUnicodeNew = {}
function LabelTTFUnicodeNew.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + TTF unicode")
    Helper.subtitleLabel:setString("Uses the new Label with TTF. Testing unicode")

    local s = ax.Director:getInstance():getWinSize()
    local vStep = s.height/9
    local vSize = s.height

    local ttfConfig = {}
    ttfConfig.fontFilePath="fonts/arial.ttf"
    ttfConfig.fontSize=23
    ttfConfig.glyphs=ax.GLYPHCOLLECTION_ASCII

    local label1 =  ax.Label:createWithTTF(ttfConfig,"Buen día, ¿cómo te llamas?", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    label1:setPosition(ax.p(s.width/2, vSize - vStep * 4.5))
    layer:addChild(label1)

    local label2 = ax.Label:createWithTTF(ttfConfig, "In welcher Straße haben Sie gelebt?", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label2:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label2)
    label2:setPosition(ax.p(s.width/2, vSize - vStep * 5.5))

    ttfConfig.fontFilePath = "fonts/HKYuanMini.ttf"
    ttfConfig.glyphs = ax.GLYPHCOLLECTION_CUSTOM
    ttfConfig.customGlyphs = "美好的一天"
    local label3 = ax.Label:createWithTTF(ttfConfig, "美好的一天", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label3:setAnchorPoint(ax.p(0.5,0.5))
    layer:addChild(label3)
    label3:setPosition(ax.p(s.width/2, vSize - vStep * 6.5))

    return layer
end


----------------------------------
--
--LabelFNTBounds
--
----------------------------------
local LabelFNTBounds = {}
function LabelFNTBounds.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .FNT + Bounds")
    Helper.subtitleLabel:setString("You should see string enclosed by a box")

    local s = ax.Director:getInstance():getWinSize()

    local colorlayer = ax.LayerColor:create(ax.color32(128,128,128,255))
    layer:addChild(colorlayer, -10)

    -- ax.LabelBMFont
    local label1 = ax.Label:createWithBMFont("fonts/boundsTestFont.fnt", "Testing Glyph Designer", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label1:setAnchorPoint(ax.p(0.5, 0.5))

    layer:addChild(label1)
    label1:setPosition(ax.p(s.width/2, s.height/2))
    return layer
end

function LabelFNTBounds.draw()
    -- ax.size labelSize = label1:getContentSize()
    -- ax.size origin = ax.Director:getInstance():getWinSize()

    -- origin.width = origin.width / 2 - (labelSize.width / 2)
    -- origin.height = origin.height / 2 - (labelSize.height / 2)

    -- ax.p vertices[4]=

    --     ax.p(origin.width, origin.height),
    --     ax.p(labelSize.width + origin.width, origin.height),
    --     ax.p(labelSize.width + origin.width, labelSize.height + origin.height),
    --     ax.p(origin.width, labelSize.height + origin.height)
    -- end
    -- ccDrawPoly(vertices, 4, true)
end


--------------------------------------------------------
----- LabelTTFLongLineWrapping
--------------------------------------------------------
local LabelTTFLongLineWrapping = {}
function LabelTTFLongLineWrapping.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Uses the new Label with TTF. Testing auto-wrapping")

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 14
    local label1 = ax.Label:createWithTTF(ttfConfig, LongSentencesExample, ax.TEXT_ALIGNMENT_LEFT, s.width)
    label1:setAnchorPoint(ax.p(0.5,1.0))
    label1:setPosition(ax.p(s.width/2, s.height/2))
    layer:addChild(label1)
    return layer
end

--------------------------------------------------------------------
--
-- LabelTTFDynamicAlignment
--
--------------------------------------------------------------------

local LabelTTFDynamicAlignment = {
    _layer       = nil,
    _label      = nil,
    _eHorizAlign = ax.TEXT_ALIGNMENT_LEFT,
}

function LabelTTFDynamicAlignment.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    LabelTTFDynamicAlignment._layer       = layer
    LabelTTFDynamicAlignment._label       = nil
    LabelTTFDynamicAlignment._eHorizAlign = ax.TEXT_ALIGNMENT_LEFT

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 23
    LabelTTFDynamicAlignment._label = ax.Label:createWithTTF(ttfConfig, LongSentencesExample, ax.TEXT_ALIGNMENT_CENTER, s.width)
    LabelTTFDynamicAlignment._label:setPosition( ax.p(s.width/2, s.height/2) )
    LabelTTFDynamicAlignment._label:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(LabelTTFDynamicAlignment._label)

    ax.MenuItemFont:setFontSize(30)
    local item1 = ax.MenuItemFont:create("Left")
    item1:registerScriptTapHandler(LabelTTFDynamicAlignment.setAlignmentLeft)
    local item2 = ax.MenuItemFont:create("Center")
    item2:registerScriptTapHandler(LabelTTFDynamicAlignment.setAlignmentCenter)
    local item3 = ax.MenuItemFont:create("Right")
    item3:registerScriptTapHandler(LabelTTFDynamicAlignment.setAlignmentRight)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:alignItemsVerticallyWithPadding(4)
    menu:setPosition(ax.p(50, s.height / 4))
    layer:addChild(menu)

    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Uses the new Label with TTF. Testing alignment")

    LabelTTFDynamicAlignment.updateAlignment()

    return layer
end

function LabelTTFDynamicAlignment.updateAlignment()
    local s = ax.Director:getInstance():getWinSize()
    LabelTTFDynamicAlignment._label:setAlignment(LabelTTFDynamicAlignment._eHorizAlign)
end

function LabelTTFDynamicAlignment.setAlignmentLeft(pSender)
    LabelTTFDynamicAlignment._eHorizAlign = ax.TEXT_ALIGNMENT_LEFT
    LabelTTFDynamicAlignment.updateAlignment()
end

function LabelTTFDynamicAlignment.setAlignmentCenter(pSender)
    LabelTTFDynamicAlignment._eHorizAlign = ax.TEXT_ALIGNMENT_CENTER
    LabelTTFDynamicAlignment.updateAlignment()
end

function LabelTTFDynamicAlignment.setAlignmentRight(pSender)
    LabelTTFDynamicAlignment._eHorizAlign = ax.TEXT_ALIGNMENT_RIGHT
    LabelTTFDynamicAlignment.updateAlignment()
end


--------------------------------------------------------
----- LabelTTFCJKWrappingTest
--------------------------------------------------------
local LabelTTFCJKWrappingTest = {}
function LabelTTFCJKWrappingTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString(
        "New Label with CJK + ASCII characters\n"
        .. "Characters should stay in the correct position")

    local size = ax.Director:getInstance():getVisibleSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/HKYuanMini.ttf"
    ttfConfig.fontSize = 25
    ttfConfig.glyphs = ax.GLYPHCOLLECTION_DYNAMIC
    ttfConfig.customGlyphs = nil
    ttfConfig.distanceFieldEnabled = false

    local drawNode = ax.DrawNode:create()
    drawNode:setAnchorPoint(ax.p(0, 0))
    layer:addChild(drawNode)
    drawNode:drawSegment(
        ax.p(size.width * 0.1, size.height * 0.8),
        ax.p(size.width * 0.1, 0), 1, ax.color(1, 0, 0, 1))
    drawNode:drawSegment(
        ax.p(size.width * 0.85, size.height * 0.8),
        ax.p(size.width * 0.85, 0), 1, ax.color(1, 0, 0, 1))

    local label1 = ax.Label:createWithTTF(
        ttfConfig, "你好，Axmol Label。",
        ax.TEXT_ALIGNMENT_LEFT, size.width * 0.75)
    label1:setTextColor(ax.color32(128, 255, 255, 255))
    label1:setPosition(ax.p(size.width * 0.1, size.height * 0.6))
    label1:setAnchorPoint(ax.p(0, 0.5))
    layer:addChild(label1)

    local label2 = ax.Label:createWithTTF(
        ttfConfig, "早上好，Axmol Label。",
        ax.TEXT_ALIGNMENT_LEFT, size.width * 0.75)
    label2:setTextColor(ax.color32(255, 128, 255, 255))
    label2:setPosition(ax.p(size.width * 0.1, size.height * 0.4))
    label2:setAnchorPoint(ax.p(0, 0.5))
    layer:addChild(label2)

    local label3 = ax.Label:createWithTTF(
        ttfConfig, "美好的一天啊美好的一天啊美好的一天啊",
        ax.TEXT_ALIGNMENT_LEFT, size.width * 0.75)
    label3:setColor(ax.color32(255, 255, 128, 255))
    label3:setPosition(ax.p(size.width * 0.1, size.height * 0.2))
    label3:setAnchorPoint(ax.p(0, 0.5))
    layer:addChild(label3)

    return layer
end


--------------------------------------------------------
----- LabelTTFFontsTestNew
--------------------------------------------------------
local LabelTTFFontsTestNew = {}
function LabelTTFFontsTestNew.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + TTF")
    Helper.subtitleLabel:setString("")

    local s = ax.Director:getInstance():getWinSize()

    local ttfPaths =
    {
        "fonts/A Damn Mess.ttf",
        "fonts/Abberancy.ttf",
        "fonts/Abduction.ttf",
        "fonts/American Typewriter.ttf",
        "fonts/Paint Boy.ttf",
        "fonts/Schwarzwald.ttf",
        "fonts/Scissor Cuts.ttf",
    }
    local ttfConfig = {}
    ttfConfig.fontFilePath = ttfPaths[0]
    ttfConfig.fontSize = 20
    for i=1, #(ttfPaths) do
        ttfConfig.fontFilePath = ttfPaths[i]
        local label = ax.Label:createWithTTF( ttfConfig, ttfPaths[i], ax.TEXT_ALIGNMENT_CENTER, 0)
        if nil ~= label then
            label:setPosition( ax.p(s.width/2, ((s.height * 0.6)/#(ttfPaths) * (i -1)) + (s.height/5)))
            layer:addChild(label)

            label:setAnchorPoint(ax.p(0.5, 0.5))
        else
            print(string.format("ERROR: Cannot load: %s",ttfPaths[i]))
        end
    end

    return layer
end


--------------------------------------------------------
----- LabelBMFontTestNew
--------------------------------------------------------
local LabelBMFontTestNew = {}
function LabelBMFontTestNew.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + FNT")
    Helper.subtitleLabel:setString("Uses the new Label with .FNT file")

    local s = ax.Director:getInstance():getWinSize()

    local label1 = ax.Label:createWithBMFont("fonts/bitmapFontTest2.fnt", "Hello World, this is testing the new Label using fnt file", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    label1:setPosition(ax.p(s.width/2, s.height/2))
    layer:addChild(label1)
    return layer
end


--------------------------------------------------------
----- LabelTTFDistanceField
--------------------------------------------------------
local LabelTTFDistanceField = {}
function LabelTTFDistanceField.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Testing rendering base on DistanceField")

    local s = ax.Director:getInstance():getWinSize()
    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 40
    ttfConfig.glyphs   = ax.GLYPHCOLLECTION_DYNAMIC
    ttfConfig.customGlyphs = nil
    ttfConfig.distanceFieldEnabled = true

    local label1 = ax.Label:createWithTTF(ttfConfig,"Distance Field",ax.TEXT_ALIGNMENT_CENTER,s.width)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    label1:setPosition(ax.p(s.width/2, s.height/2))
    label1:setTextColor( ax.color32(0, 255, 0, 255) )
    local action = ax.Sequence:create(ax.DelayTime:create(1.0),
        ax.ScaleTo:create(6.0,5.0,5.0),
        ax.ScaleTo:create(6.0,1.0,1.0))
    label1:runAction(ax.RepeatForever:create(action))
    layer:addChild(label1)

    local label2 = ax.Label:createWithTTF(ttfConfig,"Distance Field",ax.TEXT_ALIGNMENT_CENTER,s.width)
    label2:setPosition( ax.p(s.width/2, s.height/5) )
    label2:setTextColor( ax.color32(255, 0, 0, 255))
    label2:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(label2)

    return layer
end

--------------------------------------------------------
----- LabelOutlineAndGlowTest
--------------------------------------------------------
local LabelOutlineAndGlowTest = {}
function LabelOutlineAndGlowTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Testing outline and glow of label")

    local s = ax.Director:getInstance():getWinSize()

    local col = ax.LayerColor:create( ax.color32(200, 191, 231, 255))
    layer:addChild(col)

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 40
    ttfConfig.glyphs   = ax.GLYPHCOLLECTION_DYNAMIC
    ttfConfig.customGlyphs = nil
    ttfConfig.distanceFieldEnabled = true
    ttfConfig.outlineSize = 0

    local label1 = ax.Label:createWithTTF(ttfConfig,"Glow",ax.TEXT_ALIGNMENT_CENTER,s.width)
    label1:setAnchorPoint(ax.p(0.5,0.5))
    label1:setPosition(ax.p(s.width/2, s.height * 0.7))
    label1:setTextColor( ax.color32(0, 255, 0, 255) )
    label1:enableGlow(ax.color32(255, 255, 0, 255))
    layer:addChild(label1)

    ttfConfig.outlineSize = 1
    local label2 = ax.Label:createWithTTF(ttfConfig,"Outline",ax.TEXT_ALIGNMENT_CENTER,s.width)
    label2:setPosition( ax.p(s.width/2, s.height * 0.6))
    label2:setTextColor( ax.color32(255, 0, 0, 255))
    label2:setAnchorPoint(ax.p(0.5, 0.5))
    label2:enableOutline(ax.color32(0,0,255,255))
    layer:addChild(label2)

    ttfConfig.outlineSize = 2
    local label3 = ax.Label:createWithTTF(ttfConfig,"Outline",ax.TEXT_ALIGNMENT_CENTER,s.width)
    label3:setPosition( ax.p(s.width/2, s.height * 0.48))
    label3:setTextColor( ax.color32(255, 0, 0, 255))
    label3:setAnchorPoint(ax.p(0.5, 0.5))
    label3:enableOutline(ax.color32(0,0,255,255))
    layer:addChild(label3)

    return layer
end


--------------------------------------------------------------------
--
-- LabelCharMapTest
--
--------------------------------------------------------------------
local LabelCharMapTest = {}

function LabelCharMapTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + char map file")
    Helper.subtitleLabel:setString("Updating label should be fast.")

    time = 0

    local label1 = ax.Label:createWithCharMap("fonts/tuffy_bold_italic-charmap.plist")
    layer:addChild(label1, 0, kTagSprite1)
    label1:setAnchorPoint(ax.p(0, 0))
    label1:setPosition( ax.p(10,100) )
    label1:setOpacity( 200 )

    local label2 = ax.Label:createWithCharMap("fonts/tuffy_bold_italic-charmap.plist")
    layer:addChild(label2, 0, kTagSprite2)
    label2:setAnchorPoint(ax.p(0, 0))
    label2:setPosition( ax.p(10,200) )
    label2:setOpacity( 32 )

    local function step(dt)
        time = time + dt
        local info = string.format("%2.2f Test", time)

        local label1 = layer:getChildByTag(kTagSprite1)
        label1:setString(info)

        local label2 = layer:getChildByTag(kTagSprite2)
        info = string.format("%d",time)
        label2:setString(info)
    end

    layer:scheduleUpdateWithPriorityLua(step, 0)

    function onNodeEvent(tag)
        if tag == "exit" then
            layer:unscheduleUpdate()
        end
    end

    layer:registerScriptHandler(onNodeEvent)

    return layer
end


--------------------------------------------------------
----- LabelCrashTest
--------------------------------------------------------
local LabelCrashTest = {}
function LabelCrashTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label Crash Test")
    Helper.subtitleLabel:setString("Not crash and show [Test123] when using unknown character.")

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize     = 40
    ttfConfig.glyphs       = ax.GLYPHCOLLECTION_DYNAMIC
    ttfConfig.customGlyphs = nil
    ttfConfig.distanceFieldEnabled = true
    local s = ax.Director:getInstance():getWinSize()
    local label1 = ax.Label:createWithTTF(ttfConfig,"Test崩溃123", ax.TEXT_ALIGNMENT_CENTER, s.width)
    label1:setPosition( ax.p(s.width/2, s.height/2) )
    label1:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(label1)

    return layer
end

--------------------------------------------------------
----- LabelShadowTest
--------------------------------------------------------
local LabelShadowTest = {}
function LabelShadowTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + .TTF")
    Helper.subtitleLabel:setString("Testing shadow of label")

    local size = ax.Director:getInstance():getWinSize()

    local bg = ax.LayerColor:create(ax.color32(200,191,231,255))
    layer:addChild(bg)

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize     = 40
    ttfConfig.glyphs       = ax.GLYPHCOLLECTION_DYNAMIC
    ttfConfig.customGlyphs = nil
    ttfConfig.distanceFieldEnabled = true

    local shadowLabelTTF = ax.Label:createWithTTF(ttfConfig, "TTF:Shadow")
    shadowLabelTTF:setPosition(ax.p(size.width/2, size.height*0.65))
    shadowLabelTTF:setTextColor(ax.color32(255, 0, 0, 255))
    shadowLabelTTF:enableShadow(ax.color32(0, 0, 0, 255))
    layer:addChild(shadowLabelTTF)

    local shadowLabelOutline = ax.Label:createWithTTF(ttfConfig,"TTF:Shadow")
    shadowLabelOutline:setPosition(ax.p(size.width/2, size.height*0.5))
    shadowLabelOutline:setTextColor(ax.color32(255, 0, 0, 255))
    shadowLabelOutline:enableOutline(ax.color32(255, 255, 0, 255), 1)
    shadowLabelOutline:enableShadow(ax.color32(0, 0, 0, 255))
    layer:addChild(shadowLabelOutline)

    local shadowLabelBMFont = ax.Label:createWithBMFont("fonts/bitmapFontTest.fnt", "BMFont:Shadow")
    shadowLabelBMFont:setPosition( ax.p(size.width/2, size.height*0.35) )
    shadowLabelBMFont:setColor(ax.color32(255, 0, 0))
    shadowLabelBMFont:enableShadow(ax.color32(0, 255, 0, 255))
    layer:addChild(shadowLabelBMFont)

    local function sliderEvent(sender, eventType)
        if eventType == ccui.SliderEventType.percentChanged then
            local  slider = layer:getChildByTag(1)
            local  slider2 = layer:getChildByTag(2)
            local offset = ax.size(slider:getPercent() - 50,50 - slider2:getPercent())
            shadowLabelTTF:enableShadow(ax.color32(0, 0, 0, 255),offset)
            shadowLabelBMFont:enableShadow(ax.color32(0, 255, 0, 255),offset)
            shadowLabelOutline:enableShadow(ax.color32(0, 0, 0, 255),offset)
        end
    end

    local slider = ccui.Slider:create()
    slider:setTag(1)
    slider:setTouchEnabled(true)
    slider:loadBarTexture("cocosui/sliderTrack.png")
    slider:loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "")
    slider:loadProgressBarTexture("cocosui/sliderProgress.png")
    slider:setPosition(ax.p(size.width / 2.0, size.height * 0.15 + slider:getLayoutSize().height * 2.0))
    slider:setPercent(52)
    slider:addEventListener(sliderEvent)
    layer:addChild(slider)

    local slider2 = ccui.Slider:create()
    slider2:setTag(2)
    slider2:setTouchEnabled(true)
    slider2:loadBarTexture("cocosui/sliderTrack.png")
    slider2:loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "")
    slider2:loadProgressBarTexture("cocosui/sliderProgress.png")
    slider2:setPosition(ax.p(size.width * 0.15, size.height / 2.0))
    slider2:setRotation(90)
    slider2:setPercent(52)
    slider2:addEventListener(sliderEvent)
    layer:addChild(slider2)

    return layer
end

--------------------------------------------------------
----- LabelCharMapColorTest
--------------------------------------------------------

local LabelCharMapColorTest = {}
function LabelCharMapColorTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label + CharMap")
    Helper.subtitleLabel:setString("Opacity + Color should work at the same time")

    local label1 = ax.Label:createWithCharMap("fonts/tuffy_bold_italic-charmap.plist")--32 means Space key
    layer:addChild(label1, 0, kTagSprite1)
    label1:setAnchorPoint(ax.p(0.0, 0.0))
    label1:setPosition( ax.p(10,100) )
    label1:setOpacity( 200 )

    local label2 = ax.Label:createWithCharMap("fonts/tuffy_bold_italic-charmap.plist")--32 means Space key
    layer:addChild(label2, 0, kTagSprite2)
    label2:setAnchorPoint(ax.p(0.0, 0.0))
    label2:setPosition( ax.p(10, 200) )
    label2:setColor( ax.color32(255, 0, 0) )

    local fade = ax.FadeOut:create(1.0)
    local fade_in = fade:reverse()
    local function actionFinishCallback(sender)
        print("Action finished")
    end
    local cb = ax.CallFunc:create(actionFinishCallback)
    local seq = ax.Sequence:create(fade, fade_in, cb)
    local repeatForever = ax.RepeatForever:create( seq )
    label2:runAction( repeatForever )

    local scheduleTime = 0
    local function step(dt)
        scheduleTime = scheduleTime + dt
        local strInfo = string.format("%2.2f Test", scheduleTime)
        local label1 = layer:getChildByTag(kTagSprite1)
        label1:setString(strInfo)

        local label2 = layer:getChildByTag(kTagSprite2)
        strInfo = string.format("%d", scheduleTime)
        label2:setString(strInfo)
    end

    local function onNodeEvent(event)
        if tag == "exit" then
            layer:unscheduleUpdate()
        end
    end


    layer:registerScriptHandler(onNodeEvent)
    layer:scheduleUpdateWithPriorityLua(step, 0)

    return layer
end



--------------------------------------------------------
----- LabelFontNameTest
--------------------------------------------------------

local LabelFontNameTest = {}
function LabelFontNameTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label Test")
    Helper.subtitleLabel:setString("create label by font name,compatible with old labelTTF")

    local  size = ax.Director:getInstance():getWinSize()

    local label1 = ax.Label:create()
    label1:setString("Default Font")
    label1:setPosition( ax.p(size.width/2, size.height * 0.7) )
    layer:addChild(label1)

    local label3 = ax.Label:createWithTTF("fonts/Marker Felt.ttf","fonts/Marker Felt.ttf",32)
    label3:setPosition( ax.p(size.width/2, size.height * 0.5) )
    layer:addChild(label3)

    return layer
end


--------------------------------------------------------
----- LabelAlignmentTest
--------------------------------------------------------

local LabelAlignmentTest =
{
    _layer       = nil,
    _label      = nil,
    _horizAlign = ax.TEXT_ALIGNMENT_LEFT,
    _vertAlign  = ax.VERTICAL_TEXT_ALIGNMENT_TOP,
}
function LabelAlignmentTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("Testing New Label")
    Helper.subtitleLabel:setString("Select the buttons on the sides to change alignment")

    local blockSize = ax.size(200, 160)
    local s = ax.Director:getInstance():getWinSize()

    local pos = ax.p((s.width - blockSize.width) / 2, (s.height - blockSize.height) / 2)
    local colorLayer = ax.LayerColor:create(ax.color32(100, 100, 100, 255), blockSize.width, blockSize.height)
    colorLayer:setAnchorPoint(ax.p(0.0, 0.0))
    colorLayer:setPosition(pos)

    layer:addChild(colorLayer)

    ax.MenuItemFont:setFontSize(30)
    local item1 = ax.MenuItemFont:create("Left")
    item1:registerScriptTapHandler(LabelAlignmentTest.setAlignmentLeft)
    local item2 = ax.MenuItemFont:create("Center")
    item2:registerScriptTapHandler(LabelAlignmentTest.setAlignmentCenter)
    local item3 = ax.MenuItemFont:create("Right")
    item3:registerScriptTapHandler(LabelAlignmentTest.setAlignmentRight)

    local menu = ax.Menu:create()
    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:alignItemsVerticallyWithPadding(4)
    menu:setPosition(ax.p(50, s.height / 2 - 20))
    layer:addChild(menu)

    menu = ax.Menu:create()

    item1 = ax.MenuItemFont:create("Top")
    item1:registerScriptTapHandler(LabelAlignmentTest.setAlignmentTop)
    item2 = ax.MenuItemFont:create("Middle")
    item2:registerScriptTapHandler(LabelAlignmentTest.setAlignmentMiddle)
    item3 = ax.MenuItemFont:create("Bottom")
    item3:registerScriptTapHandler(LabelAlignmentTest.setAlignmentBottom)

    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)

    menu:alignItemsVerticallyWithPadding(4)
    menu:setPosition(ax.p(s.width - 50, s.height / 2 - 20))
    layer:addChild(menu)

    LabelAlignmentTest._layer      = layer
    LabelAlignmentTest._label      = nil
    LabelAlignmentTest._horizAlign = ax.TEXT_ALIGNMENT_LEFT
    LabelAlignmentTest._vertAlign  = ax.VERTICAL_TEXT_ALIGNMENT_TOP


    local ttfConfig  = {}
    ttfConfig.fontFilePath="fonts/arial.ttf"
    ttfConfig.fontSize = 32
    local label = ax.Label:create()
    label:setDimensions(200,160)
    label:setAlignment(LabelAlignmentTest._horizAlign,LabelAlignmentTest._vertAlign)
    label:setTTFConfig(ttfConfig)
    label:setString(LabelAlignmentTest.getCurrentAlignment())
    label:setAnchorPoint(ax.p(0.0, 0.0))
    label:setPosition(pos)
    layer:addChild(label)
    LabelAlignmentTest._label  = label

    return layer
end

function LabelAlignmentTest.updateAlignment()
    if nil ~= LabelAlignmentTest._label then
        LabelAlignmentTest._label:setHorizontalAlignment(LabelAlignmentTest._horizAlign)
        LabelAlignmentTest._label:setVerticalAlignment(LabelAlignmentTest._vertAlign)
        LabelAlignmentTest._label:setString(LabelAlignmentTest.getCurrentAlignment())
    end
end

function LabelAlignmentTest.setAlignmentLeft(sender)
    LabelAlignmentTest._horizAlign = ax.TEXT_ALIGNMENT_LEFT
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.setAlignmentCenter(sender)
    LabelAlignmentTest._horizAlign = ax.TEXT_ALIGNMENT_CENTER
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.setAlignmentRight(sender)
    LabelAlignmentTest._horizAlign = ax.TEXT_ALIGNMENT_RIGHT
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.setAlignmentTop(sender)
    LabelAlignmentTest._vertAlign = ax.VERTICAL_TEXT_ALIGNMENT_TOP
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.setAlignmentMiddle(sender)
    LabelAlignmentTest._vertAlign = ax.VERTICAL_TEXT_ALIGNMENT_CENTER
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.setAlignmentBottom(sender)
    LabelAlignmentTest._vertAlign = ax.VERTICAL_TEXT_ALIGNMENT_BOTTOM
    LabelAlignmentTest.updateAlignment()
end

function LabelAlignmentTest.getCurrentAlignment()
    local vertical = nil
    local horizontal = nil
    if LabelAlignmentTest._vertAlign == ax.VERTICAL_TEXT_ALIGNMENT_TOP then
        vertical = "Top"
    elseif LabelAlignmentTest._vertAlign ==  ax.VERTICAL_TEXT_ALIGNMENT_CENTER then
        vertical = "Middle"
    elseif LabelAlignmentTest._vertAlign ==  ax.VERTICAL_TEXT_ALIGNMENT_BOTTOM then
        vertical = "Bottom"
    end

    if LabelAlignmentTest._horizAlign == ax.TEXT_ALIGNMENT_LEFT then
        horizontal = "Left"
    elseif LabelAlignmentTest._horizAlign == ax.TEXT_ALIGNMENT_CENTER then
        horizontal = "Center"
    elseif LabelAlignmentTest._horizAlign == ax.TEXT_ALIGNMENT_RIGHT then
        horizontal = "Right"
    end

    return string.format("Alignment %s %s", vertical, horizontal)
end


--------------------------------------------------------
----- LabelIssue4428Test
--------------------------------------------------------

local LabelIssue4428Test = { }
function LabelIssue4428Test.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New Label Bugs Test")
    Helper.subtitleLabel:setString("Reorder issue #4428.The label should be flipped vertically.")


    local size = ax.Director:getInstance():getWinSize()

    local label = ax.Label:createWithBMFont( "fonts/bitmapFontTest3.fnt", "123\n456")
    label:setPosition(ax.p(size.width /2.0, size.height / 2.0))
    label:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(label)

    local len = label:getStringLength()
    for i = 0, len -1 do
        local sprite = label:getLetter(i)
        if nil ~= sprite then
            sprite:setFlippedY(true)
        end
    end

    return layer
end

--------------------------------------------------------
----- LabelTTFOldNew
--------------------------------------------------------

local LabelTTFOldNew = { }
function LabelTTFOldNew.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("New / Old TTF")
    Helper.subtitleLabel:setString("Comparison between old(red) and new(white) TTF label")

    local s = ax.Director:getInstance():getWinSize()
    local delta = s.height/4

    local label1 = ax.Label:createWithSystemFont("Axmol Label Test", "arial", 24)
    layer:addChild(label1, 0, kTagBitmapAtlas1)
    label1:setPosition(ax.p(s.width/2, delta * 2))
    label1:setColor(ax.color32(255, 0, 0))

    local labelSize = label1:getContentSize()
    local origin    = ax.Director:getInstance():getWinSize()
    origin.width = origin.width   / 2 - (labelSize.width / 2)
    origin.height = origin.height / 2 - (labelSize.height / 2)
    local vertices =
    {
        ax.p(origin.width, origin.height),
        ax.p(labelSize.width + origin.width, origin.height),
        ax.p(labelSize.width + origin.width, labelSize.height + origin.height),
        ax.p(origin.width, labelSize.height + origin.height),
    }
    local drawNode = ax.DrawNode:create()
    drawNode:drawPoly(vertices, 4, true, ax.color(1,0,0,1))
    layer:addChild(drawNode)

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize     = 24
    local label2 = ax.Label:createWithTTF(ttfConfig, "Axmol Label Test")
    layer:addChild(label2, 0, kTagBitmapAtlas2)
    label2:setPosition(ax.p(s.width/2, delta * 2))

    labelSize = label2:getContentSize()
    origin    = ax.Director:getInstance():getWinSize()
    origin.width = origin.width   / 2 - (labelSize.width / 2)
    origin.height = origin.height / 2 - (labelSize.height / 2)
    local vertices2 =
    {
        ax.p(origin.width, origin.height),
        ax.p(labelSize.width + origin.width, origin.height),
        ax.p(labelSize.width + origin.width, labelSize.height + origin.height),
        ax.p(origin.width, labelSize.height + origin.height),
    }
    local drawNode2 = ax.DrawNode:create()
    drawNode2:drawPoly(vertices2, 4, true, ax.color(1,1,1,1))
    layer:addChild(drawNode2)

    return layer
end


------------
function LabelTestNew()
    cclog("LabelTestNew")
    m_time = 0
    local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        LabelFNTColorAndOpacity.create,
        LabelFNTSpriteActions.create,
        LabelFNTPadding.create,
        LabelFNTOffset.create,
        LabelFNTColor.create,
        LabelTTFLongLineWrapping.create,
        LabelTTFColor.create,
        LabelFNTHundredLabels.create,
        LabelFNTMultiLine.create,
        LabelFNTandTTFEmpty.create,
        LabelFNTRetina.create,
        LabelFNTGlyphDesigner.create,
        LabelTTFUnicodeChinese.create,
        LabelFNTUnicodeChinese.create,
        LabelFNTMultiLineAlignment.create,
        LabelFNTUNICODELanguages.create,
        LabelTTFUnicodeNew.create,
        LabelTTFAlignmentNew.create,
        LabelFNTBounds.create,
        LabelTTFDynamicAlignment.create,
        LabelTTFCJKWrappingTest.create,
        LabelTTFFontsTestNew.create,
        LabelBMFontTestNew.create,
        LabelTTFDistanceField.create,
        LabelOutlineAndGlowTest.create,
        LabelShadowTest.create,
        LabelCharMapTest.create,
        LabelCharMapColorTest.create,
        LabelCrashTest.create,
        LabelTTFOldNew.create,
        LabelFontNameTest.create,
        LabelAlignmentTest.create,
        LabelIssue4428Test.create,
    }
    Helper.index = 1

    scene:addChild(LabelFNTColorAndOpacity.create())
    scene:addChild(CreateBackMenuItem())
    return scene
end
