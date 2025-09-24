local kTagAction1 = 1
local kTagAction2 = 2
local kTagSlider = 1

local s = ax.Director:getInstance():getCanvasSize()
local scheduler = ax.Director:getInstance():getScheduler()

local function createSimpleMoveBy()
    return ax.MoveBy:create(3, ax.p(s.width - 130, 0))
end

local function createSimpleDelayTime()
    return ax.DelayTime:create(0.25)
end

local function positionForTwo()
	grossini:setPosition(ax.p(60, s.height * 1 / 5))
    tamara:setPosition(ax.p(60, s.height * 4 / 5))
    kathia:setVisible(false)
end

local function getBaseLayer()
	local layer = ax.Layer:create()

	grossini = ax.Sprite:create(s_pPathGrossini)
    tamara = ax.Sprite:create(s_pPathSister1)
    kathia = ax.Sprite:create(s_pPathSister2)

    layer:addChild(grossini, 3)
    layer:addChild(kathia, 2)
    layer:addChild(tamara, 1)

    grossini:setPosition(ax.p(60, s.height * 1 / 5))
    kathia:setPosition(ax.p(60, s.height * 2.5 / 5))
    tamara:setPosition(ax.p(60, s.height * 4 / 5))

	Helper.initWithLayer(layer)

	return layer
end

-----------------------------------
--  SpriteEase
-----------------------------------
local SpriteEase_entry = nil

local function testStopAction(dt)
	scheduler:unscheduleScriptEntry(SpriteEase_entry)
    tamara:stopActionByTag(1)
    kathia:stopActionByTag(1)
    grossini:stopActionByTag(1)
end

local function SpriteEase_onEnterOrExit(tag)
	if tag == "enter" then
		SpriteEase_entry = scheduler:scheduleScriptFunc(testStopAction, 6.25, false)
	elseif tag == "exit" then
		scheduler:unscheduleScriptEntry(SpriteEase_entry)
	end
end

local function SpriteEase()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseIn:create(createSimpleMoveBy(), 2.5)
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseOut:create(createSimpleMoveBy(), 2.5)
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease_in,createSimpleDelayTime(),move_ease_in_back,createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out,createSimpleDelayTime(),move_ease_out_back,createSimpleDelayTime())

    local a2 = grossini:runAction(ax.RepeatForever:create(seq1))
    a2:setTag(1)
    local a1 = tamara:runAction(ax.RepeatForever:create(seq2))
    a1:setTag(1)
    local a = kathia:runAction(ax.RepeatForever:create(seq3))
    a:setTag(1)

	layer:registerScriptHandler(SpriteEase_onEnterOrExit)

	Helper.titleLabel:setString("EaseIn - EaseOut - Stop")
	return layer
end

-----------------------------------
--	SpriteEaseInOut
-----------------------------------
local function SpriteEaseInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()

    local move_ease_inout1 = ax.EaseInOut:create(createSimpleMoveBy(), 0.65)
    local move_ease_inout_back1 = move_ease_inout1:reverse()

    local move_ease_inout2 = ax.EaseInOut:create(createSimpleMoveBy(), 1.35)
    local move_ease_inout_back2 = move_ease_inout2:reverse()

    local move_ease_inout3 = ax.EaseInOut:create(createSimpleMoveBy(), 1.0)
    local move_ease_inout_back3 = move_ease_inout3:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move_ease_inout1,delay,move_ease_inout_back1,createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease_inout2,createSimpleDelayTime(),move_ease_inout_back2,createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_inout3, createSimpleDelayTime(), move_ease_inout_back3, createSimpleDelayTime() )

    tamara:runAction(ax.RepeatForever:create(seq1))
    kathia:runAction(ax.RepeatForever:create(seq2))
    grossini:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("EaseInOut and rates")
	return layer
end

-----------------------------------
--	SpriteEaseExponential
-----------------------------------
local function SpriteEaseExponential()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseExponentialIn:create(createSimpleMoveBy())
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseExponentialOut:create(createSimpleMoveBy())
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease_in, createSimpleDelayTime(), move_ease_in_back, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out, createSimpleDelayTime(), move_ease_out_back, createSimpleDelayTime())

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))
    kathia:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("ExpIn - ExpOut actions")
	return layer
end

-----------------------------------
--	SpriteEaseExponentialInOut
-----------------------------------
local function SpriteEaseExponentialInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease = ax.EaseExponentialInOut:create(createSimpleMoveBy())
    local move_ease_back = move_ease:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease, createSimpleDelayTime(), move_ease_back, createSimpleDelayTime() )

    positionForTwo()

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))

	Helper.titleLabel:setString("EaseExponentialInOut action")
	return layer
end

-----------------------------------
--	SpriteEaseSine
-----------------------------------
local function SpriteEaseSine()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseSineIn:create(createSimpleMoveBy())
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseSineOut:create(createSimpleMoveBy())
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime() )
    local seq2 = ax.Sequence:create(move_ease_in, createSimpleDelayTime(), move_ease_in_back, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out, createSimpleDelayTime(), move_ease_out_back,createSimpleDelayTime())

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))
    kathia:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("EaseSineIn - EaseSineOut")
	return layer
end

-----------------------------------
--	SpriteEaseSineInOut
-----------------------------------
local function SpriteEaseSineInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease = ax.EaseSineInOut:create(createSimpleMoveBy())
    local move_ease_back = move_ease:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease, createSimpleDelayTime(), move_ease_back, createSimpleDelayTime())

    positionForTwo()

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))

	Helper.titleLabel:setString("EaseSineInOut action")
	return layer
end

-----------------------------------
--	SpriteEaseElastic
-----------------------------------
local function SpriteEaseElastic()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseElasticIn:create(createSimpleMoveBy())
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseElasticOut:create(createSimpleMoveBy())
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime() )
    local seq2 = ax.Sequence:create(move_ease_in, createSimpleDelayTime(), move_ease_in_back, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out, createSimpleDelayTime(), move_ease_out_back, createSimpleDelayTime())

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))
    kathia:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("Elastic In - Out actions")
	return layer
end

-----------------------------------
--	SpriteEaseElasticInOut
-----------------------------------
local function SpriteEaseElasticInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()

    local move_ease_inout1 = ax.EaseElasticInOut:create(createSimpleMoveBy(), 0.3)
    local move_ease_inout_back1 = move_ease_inout1:reverse()

    local move_ease_inout2 = ax.EaseElasticInOut:create(createSimpleMoveBy(), 0.45)
    local move_ease_inout_back2 = move_ease_inout2:reverse()

    local move_ease_inout3 = ax.EaseElasticInOut:create(createSimpleMoveBy(), 0.6)
    local move_ease_inout_back3 = move_ease_inout3:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move_ease_inout1, delay, move_ease_inout_back1, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease_inout2, createSimpleDelayTime(), move_ease_inout_back2, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_inout3, createSimpleDelayTime(), move_ease_inout_back3, createSimpleDelayTime())

    tamara:runAction(ax.RepeatForever:create(seq1))
    kathia:runAction(ax.RepeatForever:create(seq2))
    grossini:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("EaseElasticInOut action")
	return layer
end

-----------------------------------
--	SpriteEaseBounce
-----------------------------------
local function SpriteEaseBounce()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseBounceIn:create(createSimpleMoveBy())
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseBounceOut:create(createSimpleMoveBy())
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime() )
    local seq2 = ax.Sequence:create(move_ease_in, createSimpleDelayTime(), move_ease_in_back, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out, createSimpleDelayTime(), move_ease_out_back, createSimpleDelayTime())

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))
    kathia:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("Bounce In - Out actions")
	return layer
end

-----------------------------------
--	SpriteEaseBounceInOut
-----------------------------------
local function SpriteEaseBounceInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease = ax.EaseBounceInOut:create(createSimpleMoveBy())
    local move_ease_back = move_ease:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease, createSimpleDelayTime(), move_ease_back, createSimpleDelayTime())

    positionForTwo()

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))

	Helper.titleLabel:setString("EaseBounceInOut action")
	return layer
end

-----------------------------------
--	SpriteEaseBack
-----------------------------------
local function SpriteEaseBack()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease_in = ax.EaseBackIn:create(createSimpleMoveBy())
    local move_ease_in_back = move_ease_in:reverse()

    local move_ease_out = ax.EaseBackOut:create(createSimpleMoveBy())
    local move_ease_out_back = move_ease_out:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease_in, createSimpleDelayTime(), move_ease_in_back, createSimpleDelayTime())
    local seq3 = ax.Sequence:create(move_ease_out, createSimpleDelayTime(), move_ease_out_back, createSimpleDelayTime())

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))
    kathia:runAction(ax.RepeatForever:create(seq3))

	Helper.titleLabel:setString("Back In - Out actions")
	return layer
end

-----------------------------------
--	SpriteEaseBackInOut
-----------------------------------
local function SpriteEaseBackInOut()
	local layer = getBaseLayer()

	local move = createSimpleMoveBy()
    local move_back = move:reverse()

    local move_ease = ax.EaseBackInOut:create(createSimpleMoveBy())
    local move_ease_back = move_ease:reverse()

    local delay = createSimpleDelayTime()
    local seq1 = ax.Sequence:create(move, delay, move_back, createSimpleDelayTime())
    local seq2 = ax.Sequence:create(move_ease,createSimpleDelayTime(), move_ease_back, createSimpleDelayTime())

    positionForTwo()

    grossini:runAction(ax.RepeatForever:create(seq1))
    tamara:runAction(ax.RepeatForever:create(seq2))

	Helper.titleLabel:setString("EaseBackInOut action")
	return layer
end

-----------------------------------
--	SpeedTest
-----------------------------------
local SpeedTest_entry = nil
local SpeedTest_action1 = nil
local SpeedTest_action2 = nil
local SpeedTest_action3 = nil

local function altertime(dt)
    SpeedTest_action1:setSpeed(math.random() * 2)
    SpeedTest_action2:setSpeed(math.random() * 2)
    SpeedTest_action3:setSpeed(math.random() * 2)
end

local function SpeedTest_onEnterOrExit(tag)
	if tag == "enter" then
		SpeedTest_entry = scheduler:scheduleScriptFunc(altertime, 1.0, false)
	elseif tag == "exit" then
		scheduler:unscheduleScriptEntry(SpeedTest_entry)
	end
end

local function SpeedTest()
	local layer = getBaseLayer()

	local jump1 = ax.JumpBy:create(4, ax.p(- s.width + 80, 0), 100, 4)
    local jump2 = jump1:reverse()
    local rot1 = ax.RotateBy:create(4, 360 * 2)
    local rot2 = rot1:reverse()

    local seq3_1 = ax.Sequence:create(jump2, jump1)
    local seq3_2 = ax.Sequence:create(rot1, rot2)

    local spawn = ax.Spawn:create(seq3_1, seq3_2)
    SpeedTest_action1 = ax.Speed:create(ax.RepeatForever:create(spawn), 1.0)

	local spawn2 = spawn:clone()
    SpeedTest_action2 = ax.Speed:create(ax.RepeatForever:create(spawn2), 1.0)

	local spawn3 = spawn:clone()
	SpeedTest_action3 = ax.Speed:create(ax.RepeatForever:create(spawn3), 1.0)

    grossini:runAction(SpeedTest_action2)
    tamara:runAction(SpeedTest_action3)
    kathia:runAction(SpeedTest_action1)

	layer:registerScriptHandler(SpeedTest_onEnterOrExit)

	Helper.titleLabel:setString("Speed action")
	return layer
end

function EaseActionsTest()
	local scene = ax.Scene:create()
	cclog("EaseActionsTest")

	Helper.createFunctionTable = {
		SpriteEase,
		SpriteEaseInOut,
		SpriteEaseExponential,
		SpriteEaseExponentialInOut,
		SpriteEaseSine,
		SpriteEaseSineInOut,
		SpriteEaseElastic,
		SpriteEaseElasticInOut,
		SpriteEaseBounce,
		SpriteEaseBounceInOut,
		SpriteEaseBack,
		SpriteEaseBackInOut,
		SpeedTest
    }
    Helper.index = 1

	scene:addChild(SpriteEase())
	scene:addChild(CreateBackMenuItem())

	return scene
end
