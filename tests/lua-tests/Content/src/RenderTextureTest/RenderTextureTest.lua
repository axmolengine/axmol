-- Test #1 by Jason Booth (slipster216)
-- Test #3 by David Deaco (ddeaco)

--/**
--    * Implementation of RenderTextureSave
--*/
local function RenderTextureSave()
    local ret = createTestLayer("Touch the screen",
                                "Press 'Save Image' to create an snapshot of the render texture")
    local s = ax.Director:getInstance():getWinSize()
    local target = nil
    local counter = 0
    local brushes = {}
    local function clearImage(tag, pSender)
        target:clear(math.random(), math.random(), math.random(), math.random())
    end

    local function saveImage(tag, pSender)
        local png = string.format("image-%d.png", counter)
        local jpg = string.format("image-%d.jpg", counter)

        target:saveToFile(png, ax.IMAGE_FORMAT_PNG)
        target:saveToFile(jpg, ax.IMAGE_FORMAT_JPEG)

        local function callback(image)
            local tex = ax.Director:getInstance():getTextureCache():addImage(image, png)
            local sprite = ax.Sprite:createWithTexture(tex)
            sprite:setScale(0.3)
            ret:addChild(sprite)
            sprite:setPosition(ax.p(40, 40))
            sprite:setRotation(counter * 3)
        end

        target:newImage(callback)

        cclog("Image saved %s and %s", png, jpg)
        counter = counter + 1
    end

    local function onNodeEvent(event)
        if event == "exit" then
            target:release()
            ax.Director:getInstance():getTextureCache():removeUnusedTextures()
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    -- create a render texture, this is what we are going to draw into
    target = ax.RenderTexture:create(s.width, s.height, ax.TEXTURE_PF_RGBA8)
    target:retain()
    target:setPosition(ax.p(s.width / 2, s.height / 2))
    target:setAnchorPoint(ax.p(0.5, 0.5))

    -- note that the render texture is a ax.Node, and contains a sprite of its texture for convenience,
    -- so we can just parent it to the scene like any other ax.Node
    ret:addChild(target, -1)

    local function onTouchesMoved(touches, event)
        local start = touches[1]:getLocation()
        local ended = touches[1]:getPreviousLocation()

        target:begin()

        local distance = ax.pGetDistance(start, ended)
        if distance > 1 then
            brushes = {}
            local d = distance
            local i = 0

            for i = 0,d -1 do
                -- create a brush image to draw into the texture with
                local sprite = ax.Sprite:create("Images/fire.png")
                sprite:setColor(ax.color32(255, 0, 0, 20))
                brushes[i + 1] = sprite
            end

            for i = 0,d -1 do
                local difx = ended.x - start.x
                local dify = ended.y - start.y
                local delta = i / distance
                local brush = brushes[i + 1]
                brush:setPosition(ax.p(start.x + (difx * delta), start.y + (dify * delta)))
                brush:setRotation(math.random(0, 359))
                local r = math.random(0, 49) / 50.0 + 0.25
                brush:setScale(r)

                -- Use ax.RANDOM_0_1() will cause error when loading libtests.so on android, I don't know why.
                brush:setColor(ax.color32(math.random(0, 126) + 128, 255, 255, brush:getOpacity()))
                -- Call visit to draw the brush, don't call draw..
                brush:visit()
            end
        end

        -- finish drawing and return context back to the screen
        target:endToLua()
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )
    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, ret)
    -- Save Image menu
    ax.MenuItemFont:setFontSize(16)
    local item1 = ax.MenuItemFont:create("Save Image")
    item1:setAnchorPoint(1, 1)
    item1:setPosition(VisibleRect:rightTop().x, VisibleRect:rightTop().y)
    item1:registerScriptTapHandler(saveImage)
    local item2 = ax.MenuItemFont:create("Clear")
    item2:setAnchorPoint(1, 1)
    item2:setPosition(VisibleRect:rightTop().x, VisibleRect:rightTop().y - item1:getContentSize().height)
    item2:registerScriptTapHandler(clearImage)
    local menu = ax.Menu:create(item1, item2)
    ret:addChild(menu)
    menu:setPosition(0, 0)
    return ret
end

function RenderTextureTestMain()
    cclog("RenderTextureTestMain")
    Helper.index = 1
    local scene = ax.Scene:create()
    Helper.createFunctionTable = {
        RenderTextureSave,
    }
    Helper.index = 1

    scene:addChild(RenderTextureSave())
    scene:addChild(CreateBackMenuItem())
    return scene
end
