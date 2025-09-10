
local size = ax.Director:getInstance():getLogicalSize()

local function CreateSpriteLayer()
	local layer = ax.Layer:create()

	local x, y
    x = size.width
    y = size.height

    local sprite = ax.Sprite:create(s_pPathGrossini)
    local spriteSister1 = ax.Sprite:create(s_pPathSister1)
    local spriteSister2 = ax.Sprite:create(s_pPathSister2)

    sprite:setScale(1.5)
    spriteSister1:setScale(1.5)
    spriteSister2:setScale(1.5)

    sprite:setPosition(ax.p(x / 2, y / 2))
    spriteSister1:setPosition(ax.p(40, y / 2))
    spriteSister2:setPosition(ax.p(x - 40, y / 2))

    layer:addChild(sprite)
    layer:addChild(spriteSister1)
    layer:addChild(spriteSister2)

	local rot = ax.RotateBy:create(16, -3600)
    sprite:runAction(rot)

    local jump1 = ax.JumpBy:create(4, ax.p(-400, 0), 100, 4)
    local jump2 = jump1:reverse()

    local rot1 = ax.RotateBy:create(4, 360 * 2)
    local rot2 = rot1:reverse()

    local jump3 = ax.JumpBy:create(4, ax.p(-400, 0), 100, 4)
    local jump4 = jump3:reverse()
    local rot3 = ax.RotateBy:create(4, 360 * 2)
    local rot4 = rot3:reverse()

    spriteSister1:runAction(ax.Repeat:create(ax.Sequence:create(jump2, jump1), 5))
    spriteSister2:runAction(ax.Repeat:create(ax.Sequence:create(jump3, jump4), 5))

    spriteSister1:runAction(ax.Repeat:create(ax.Sequence:create(rot1, rot2), 5))
    spriteSister2:runAction(ax.Repeat:create(ax.Sequence:create(rot4, rot3), 5))

	return layer
end

local function CreateTestLayer()
	local layer = ax.Layer:create()

	local x, y
    x = size.width
    y = size.height

    local label = ax.Label:createWithTTF("Axmol", s_tahomaPath, 64)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(x / 2, y / 2)
    layer:addChild(label)

	return layer
end

local function CreateRotateWorldLayer()
	local layer = ax.Layer:create()

	local x, y
    x = size.width
    y = size.height

    local blue =  ax.LayerColor:create(ax.color32(0,0,255,255))
    local red =   ax.LayerColor:create(ax.color32(255,0,0,255))
    local green = ax.LayerColor:create(ax.color32(0,255,0,255))
    local white = ax.LayerColor:create(ax.color32(255,255,255,255))

    blue:setScale(0.5)
    blue:setPosition(ax.p(- x / 4, - y / 4))
    blue:addChild(CreateSpriteLayer())

    red:setScale(0.5)
    red:setPosition(ax.p(x / 4, - y / 4))

    green:setScale(0.5)
    green:setPosition(ax.p(- x / 4, y / 4))
    green:addChild(CreateTestLayer())

    white:setScale(0.5)
    white:setPosition(ax.p(x / 4, y / 4))
    white:setIgnoreAnchorPointForPosition(false)
    white:setPosition(ax.p(x / 4 * 3, y / 4 * 3))

    layer:addChild(blue, -1)
    layer:addChild(white)
    layer:addChild(green)
    layer:addChild(red)

    local rot = ax.RotateBy:create(8, 720)
    local rot1 = ax.RotateBy:create(8, 720)
    local rot2 = ax.RotateBy:create(8, 720)
    local rot3 = ax.RotateBy:create(8, 720)

    blue:runAction(rot)
    red:runAction(rot1)
    green:runAction(rot2)
    white:runAction(rot3)

	return layer
end

--------------------------------
-- Rotate World Test
--------------------------------
function RotateWorldTest()
	cclog("RotateWorldTest")
	local scene = ax.Scene:create()
	local layer = CreateRotateWorldLayer()

	scene:addChild(layer)
	scene:addChild(CreateBackMenuItem())
	scene:runAction(ax.RotateBy:create(4, -360))

	return scene
end
