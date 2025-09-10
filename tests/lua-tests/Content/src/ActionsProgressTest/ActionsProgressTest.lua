
local s = ax.Director:getInstance():getLogicalSize()

------------------------------------
--  SpriteProgressToRadial
------------------------------------
local function SpriteProgressToRadial()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to1 = ax.ProgressTo:create(2, 100)
    local to2 = ax.ProgressTo:create(2, 100)

    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister1))
    left:setType(ax.PROGRESS_TIMER_TYPE_RADIAL)
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(to1))
	layer:addChild(left)

    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathBlock))
    right:setType(ax.PROGRESS_TIMER_TYPE_RADIAL)
    -- Makes the ridial CCW
    right:setReverseDirection(true)
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(to2))
	layer:addChild(right)

	Helper.subtitleLabel:setString("ProgressTo Radial")
	return layer
end

------------------------------------
--  SpriteProgressToHorizontal
------------------------------------
local function SpriteProgressToHorizontal()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to1 = ax.ProgressTo:create(2, 100)
    local to2 = ax.ProgressTo:create(2, 100)

    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister1))
    left:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the left since the midpoint is 0 for the x
    left:setMidpoint(ax.p(0, 0))
    -- Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
    left:setBarChangeRate(ax.p(1, 0))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(to1))
	layer:addChild(left)

    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    right:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the left since the midpoint is 1 for the x
    right:setMidpoint(ax.p(1, 0))
    -- Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
    right:setBarChangeRate(ax.p(1, 0))
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(to2))
	layer:addChild(right)

	Helper.subtitleLabel:setString("ProgressTo Horizontal")
	return layer
end

------------------------------------
--  SpriteProgressToVertical
------------------------------------
local function SpriteProgressToVertical()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to1 = ax.ProgressTo:create(2, 100)
    local to2 = ax.ProgressTo:create(2, 100)

    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister1))
    left:setType(ax.PROGRESS_TIMER_TYPE_BAR)

    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    left:setMidpoint(ax.p(0,0))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    left:setBarChangeRate(ax.p(0, 1))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(to1))
	layer:addChild(left)

    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    right:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    right:setMidpoint(ax.p(0, 1))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    right:setBarChangeRate(ax.p(0, 1))
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(to2))
	layer:addChild(right)

	Helper.subtitleLabel:setString("ProgressTo Vertical")
	return layer
end

------------------------------------
--  SpriteProgressToRadialMidpointChanged
------------------------------------
local function SpriteProgressToRadialMidpointChanged()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local action = ax.ProgressTo:create(2, 100)

	-- Our image on the left should be a radial progress indicator, clockwise
    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathBlock))
    left:setType(ax.PROGRESS_TIMER_TYPE_RADIAL)
    left:setMidpoint(ax.p(0.25, 0.75))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(ax.ProgressTo:create(2, 100)))
	layer:addChild(left)

	-- Our image on the left should be a radial progress indicator, counter clockwise
    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathBlock))
    right:setType(ax.PROGRESS_TIMER_TYPE_RADIAL)
    right:setMidpoint(ax.p(0.75, 0.25))

	--[[
	   Note the reverse property (default=NO) is only added to the right image. That's how
       we get a counter clockwise progress.
	]]
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(ax.ProgressTo:create(2, 100)))
    layer:addChild(right)

	Helper.subtitleLabel:setString("Radial w/ Different Midpoints")
	return layer
end

------------------------------------
--  SpriteProgressBarVarious
------------------------------------
local function SpriteProgressBarVarious()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to = ax.ProgressTo:create(2, 100)

    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister1))
    left:setType(ax.PROGRESS_TIMER_TYPE_BAR)

    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    left:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    left:setBarChangeRate(ax.p(1, 0))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(ax.ProgressTo:create(2, 100)))
	layer:addChild(left)

    local middle = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    middle:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    middle:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    middle:setBarChangeRate(ax.p(1, 1))
    middle:setPosition(ax.p(s.width/2, s.height/2))
    middle:runAction(ax.RepeatForever:create(ax.ProgressTo:create(2, 100)))
	layer:addChild(middle)

    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    right:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    right:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    right:setBarChangeRate(ax.p(0, 1))
    right:setPosition(ax.p(s.width-100, s.height/2))
    right:runAction(ax.RepeatForever:create(ax.ProgressTo:create(2, 100)))
	layer:addChild(right)

	Helper.subtitleLabel:setString("ProgressTo Bar Mid")
	return layer
end

------------------------------------
--  SpriteProgressBarTintAndFade
------------------------------------
local function SpriteProgressBarTintAndFade()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to = ax.ProgressTo:create(6, 100)
    local tint = ax.Sequence:create(ax.TintTo:create(1, 255, 0, 0), ax.TintTo:create(1, 0, 255, 0), ax.TintTo:create(1, 0, 0, 255))
    local fade = ax.Sequence:create(ax.FadeTo:create(1.0, 0),ax.FadeTo:create(1.0, 255))
    local left = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister1))
    left:setType(ax.PROGRESS_TIMER_TYPE_BAR)

    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    left:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    left:setBarChangeRate(ax.p(1, 0))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))
    left:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.TintTo:create(1, 255, 0, 0), ax.TintTo:create(1, 0, 255, 0), ax.TintTo:create(1, 0, 0, 255))))
	layer:addChild(left)

    left:addChild(ax.Label:createWithTTF("Tint", "fonts/Marker Felt.ttf", 20.0))

    local middle = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    middle:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    middle:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    middle:setBarChangeRate(ax.p(1, 1))
    middle:setPosition(ax.p(s.width / 2, s.height / 2))
    middle:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))

    local fade2 = ax.Sequence:create(ax.FadeTo:create(1.0, 0), ax.FadeTo:create(1.0, 255))
    middle:runAction(ax.RepeatForever:create(fade2))
	layer:addChild(middle)

    middle:addChild(ax.Label:createWithTTF("Fade", "fonts/Marker Felt.ttf", 20.0))

    local right = ax.ProgressTimer:create(ax.Sprite:create(s_pPathSister2))
    right:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    right:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    right:setBarChangeRate(ax.p(0, 1))
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))
    right:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.TintTo:create(1, 255, 0, 0), ax.TintTo:create(1, 0, 255, 0), ax.TintTo:create(1, 0, 0, 255))))
    right:runAction(ax.RepeatForever:create(ax.Sequence:create(ax.FadeTo:create(1.0, 0), ax.FadeTo:create(1.0, 255))))
	layer:addChild(right)

    right:addChild(ax.Label:createWithTTF("Tint and Fade", "fonts/Marker Felt.ttf", 20.0))

	Helper.subtitleLabel:setString("ProgressTo Bar Mid")
	return layer
end

------------------------------------
--  SpriteProgressWithSpriteFrame
------------------------------------
local function SpriteProgressWithSpriteFrame()
	local layer = ax.Layer:create()
	Helper.initWithLayer(layer)

	local to = ax.ProgressTo:create(6, 100)

    ax.SpriteFrameCache:getInstance():addSpriteFrames("zwoptex/grossini.plist")

    local left = ax.ProgressTimer:create(ax.Sprite:createWithSpriteFrameName("grossini_dance_01.png"))
    left:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    left:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    left:setBarChangeRate(ax.p(1, 0))
    left:setPosition(ax.p(100, s.height / 2))
    left:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))
	layer:addChild(left)

    local middle = ax.ProgressTimer:create(ax.Sprite:createWithSpriteFrameName("grossini_dance_02.png"))
    middle:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    middle:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    middle:setBarChangeRate(ax.p(1, 1))
    middle:setPosition(ax.p(s.width / 2, s.height / 2))
    middle:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))
	layer:addChild(middle)

    local right = ax.ProgressTimer:create(ax.Sprite:createWithSpriteFrameName("grossini_dance_03.png"))
    right:setType(ax.PROGRESS_TIMER_TYPE_BAR)
    -- Setup for a bar starting from the bottom since the midpoint is 0 for the y
    right:setMidpoint(ax.p(0.5, 0.5))
    -- Setup for a vertical bar since the bar change rate is 0 for x meaning no horizontal change
    right:setBarChangeRate(ax.p(0, 1))
    right:setPosition(ax.p(s.width - 100, s.height / 2))
    right:runAction(ax.RepeatForever:create(ax.ProgressTo:create(6, 100)))
	layer:addChild(right)

	Helper.subtitleLabel:setString("Progress With Sprite Frame")
	return layer
end

function ProgressActionsTest()
	local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        SpriteProgressToRadial,
        SpriteProgressToHorizontal,
        SpriteProgressToVertical,
        SpriteProgressToRadialMidpointChanged,
        SpriteProgressBarVarious,
        SpriteProgressBarTintAndFade,
        SpriteProgressWithSpriteFrame
    }
    Helper.index = 1

	scene:addChild(SpriteProgressToRadial())
	scene:addChild(CreateBackMenuItem())

	ax.Director:getInstance():replaceScene(scene)
end
