local    kTagNode = 0
local    kTagGrossini = 1

local function createParallaxLayer(title, subtitle)
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    local titleStr = title == nil and "No title" or title
    local subTitleStr = subtitle  == nil and "" or subtitle
    Helper.titleLabel:setString(titleStr)
    Helper.subtitleLabel:setString(subTitleStr)
    return layer
end

--------------------------------------------------------------------
--
-- Parallax1
--
--------------------------------------------------------------------

local function Parallax1()
    local ret = createParallaxLayer("Parallax: parent and 3 children")
    -- Top Layer, a simple image
    local  cocosImage = ax.Sprite:create(s_Power)
    -- scale the image (optional)
    cocosImage:setScale( 2.5 )
    -- change the transform anchor point to 0,0 (optional)
    cocosImage:setAnchorPoint( ax.p(0,0) )


    -- Middle layer: a Tile map atlas
    local tilemap = ax.TileMapAtlas:create(s_TilesPng, s_LevelMapTga, 16, 16)
    tilemap:releaseMap()

    -- change the transform anchor to 0,0 (optional)
    tilemap:setAnchorPoint( ax.p(0, 0) )

    -- Anti Aliased images
    tilemap:getTexture():setAntiAliasTexParameters()


    -- background layer: another image
    local  background = ax.Sprite:create(s_back)
    -- scale the image (optional)
    background:setScale( 1.5 )
    -- change the transform anchor point (optional)
    background:setAnchorPoint( ax.p(0,0) )


    -- create a void node, a parent node
    local  voidNode = ax.ParallaxNode:create()

    -- NOW add the 3 layers to the 'void' node

    -- background image is moved at a ratio of 0.4x, 0.5y
    voidNode:addChild(background, -1, ax.p(0.4,0.5), ax.p(0,0))

    -- tiles are moved at a ratio of 2.2x, 1.0y
    voidNode:addChild(tilemap, 1, ax.p(2.2,1.0), ax.p(0,-200) )

    -- top image is moved at a ratio of 3.0x, 2.5y
    voidNode:addChild(cocosImage, 2, ax.p(3.0,2.5), ax.p(200,800) )


    -- now create some actions that will move the 'void' node
    -- and the children of the 'void' node will move at different
    -- speed, thus, simulation the 3D environment
    local  goUp = ax.MoveBy:create(4, ax.p(0,-500) )
    local  goDown = goUp:reverse()
    local  go = ax.MoveBy:create(8, ax.p(-1000,0) )
    local  goBack = go:reverse()
    local  seq = ax.Sequence:create(goUp, go, goDown, goBack)
    voidNode:runAction( (ax.RepeatForever:create(seq) ))

    ret:addChild( voidNode )
    return ret
end

--------------------------------------------------------------------
--
-- Parallax2
--
--------------------------------------------------------------------

local function Parallax2()
    local ret = createParallaxLayer("Parallax: drag screen")

    -- Top Layer, a simple image
    local  cocosImage = ax.Sprite:create(s_Power)
    -- scale the image (optional)
    cocosImage:setScale( 2.5 )
    -- change the transform anchor point to 0,0 (optional)
    cocosImage:setAnchorPoint( ax.p(0,0) )


    -- Middle layer: a Tile map atlas
    local  tilemap = ax.TileMapAtlas:create(s_TilesPng, s_LevelMapTga, 16, 16)
    tilemap:releaseMap()

    -- change the transform anchor to 0,0 (optional)
    tilemap:setAnchorPoint( ax.p(0, 0) )

    -- Anti Aliased images
    tilemap:getTexture():setAntiAliasTexParameters()


    -- background layer: another image
    local  background = ax.Sprite:create(s_back)
    -- scale the image (optional)
    background:setScale( 1.5 )
    -- change the transform anchor point (optional)
    background:setAnchorPoint( ax.p(0,0) )


    -- create a void node, a parent node
    local  voidNode = ax.ParallaxNode:create()

    -- NOW add the 3 layers to the 'void' node

    -- background image is moved at a ratio of 0.4x, 0.5y
    voidNode:addChild(background, -1, ax.p(0.4,0.5), ax.p(0, 0))

    -- tiles are moved at a ratio of 1.0, 1.0y
    voidNode:addChild(tilemap, 1, ax.p(1.0,1.0), ax.p(0,-200) )

    -- top image is moved at a ratio of 3.0x, 2.5y
    voidNode:addChild( cocosImage, 2, ax.p(3.0,2.5), ax.p(200,1000) )
    ret:addChild(voidNode, 0, kTagNode)

    local function onTouchesMoved(touches, event)
        local diff = touches[1]:getDelta()

        local node = ret:getChildByTag(kTagNode)
        local currentPosX, currentPosY = node:getPosition()
        node:setPosition(ax.p(currentPosX + diff.x, currentPosY + diff.y))
    end

    local listener = ax.EventListenerTouchAllAtOnce:create()
    listener:registerScriptHandler(onTouchesMoved,ax.Handler.EVENT_TOUCHES_MOVED )
    local eventDispatcher = ret:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, ret)

    return ret
end

function ParallaxTestMain()
    cclog("ParallaxMain")
    Helper.index = 1
    local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        Parallax1,
        Parallax2
    }
    Helper.index = 1

    scene:addChild(Parallax1())
    scene:addChild(CreateBackMenuItem())
    return scene
end
