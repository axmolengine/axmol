local DEFAULT_SPRITE_COUNT = 1000
local SPRITE_STEP = 500
local MAX_SPRITE_COUNT = 50000
local SPRITE_TEXTURE = "Images/stars.png"

function BindingBenchmarkTest()
    local layer = createTestLayer("Binding benchmark", "Lua Sprite calls per frame")
    local visible = ax.Director:getInstance():getCanvasSize()
    local sprites = {}
    local spriteCount = DEFAULT_SPRITE_COUNT
    local phase = 0

    local countLabel = ax.Label:createWithTTF("", s_thonburiPath, 18)
    countLabel:setAnchorPoint(ax.p(0.5, 0.5))
    countLabel:setPosition(visible.width / 2, visible.height - 115)
    layer:addChild(countLabel, 2)

    local function createSprite(index)
        local sprite = ax.Sprite:create(SPRITE_TEXTURE)
        sprite:setScale(0.08)
        sprite:setPosition((index * 37) % visible.width, (index * 67) % visible.height)
        layer:addChild(sprite)
        sprites[index] = sprite
    end

    local function ensureSprites(count)
        while #sprites < count do
            createSprite(#sprites + 1)
        end
        -- Keep the rendered count equal to the displayed count when reducing
        -- or resetting the benchmark, independent of its previous peak.
        while #sprites > count do
            sprites[#sprites]:removeFromParent()
            sprites[#sprites] = nil
        end
    end

    local function setSpriteCount(count)
        spriteCount = math.max(SPRITE_STEP, math.min(MAX_SPRITE_COUNT, count))
        ensureSprites(spriteCount)
        countLabel:setString(string.format("Sprites: %d / %d", spriteCount, MAX_SPRITE_COUNT))
    end

    local function addSprites()
        setSpriteCount(spriteCount + SPRITE_STEP)
    end

    local function removeSprites()
        setSpriteCount(spriteCount - SPRITE_STEP)
    end

    local function resetBenchmark()
        setSpriteCount(DEFAULT_SPRITE_COUNT)
        phase = 0
    end

    ensureSprites(DEFAULT_SPRITE_COUNT)

    local decrease = ax.MenuItemFont:create("- 500")
    local reset = ax.MenuItemFont:create("Reset")
    local increase = ax.MenuItemFont:create("+ 500")
    decrease:registerScriptTapHandler(removeSprites)
    reset:registerScriptTapHandler(resetBenchmark)
    increase:registerScriptTapHandler(addSprites)

    local menu = ax.Menu:create(decrease, reset, increase)
    menu:alignItemsHorizontallyWithPadding(24)
    menu:setPosition(visible.width / 2, 42)
    layer:addChild(menu, 2)

    layer:onUpdate(function(dt)
        phase = phase + dt
        for index = 1, spriteCount do
            local sprite = sprites[index]
            local x = (index * 37 + phase * (20 + index % 11)) % visible.width
            local y = (index * 67 + phase * (15 + index % 13)) % visible.height
            sprite:setPosition(x, y)
            sprite:setOpacity(160 + math.floor((index + phase * 60) % 96))
        end
    end)

    setSpriteCount(spriteCount)
    local scene = ax.Scene:create()
    scene:addChild(layer)
    scene:addChild(CreateBackMenuItem())
    return scene
end
