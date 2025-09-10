
local function DrawNodeMainLayer()
    local kItemTagBasic = 1000
    local testCount = 1
    local maxCases = testCount
    local curCase  = 0
    local size = ax.Director:getInstance():getLogicalSize()
    local curLayer = nil

    local  function orderCallbackMenu()
        local function backCallback()
            curCase = curCase - 1
            if curCase < 0 then
                curCase = curCase + maxCases
            end
            showCurrentTest()
        end

        local function restartCallback()
            showCurrentTest()
        end

        local function nextCallback()
            curCase = curCase + 1
            curCase = curCase % maxCases
            showCurrentTest()
        end

        local ordercallbackmenu = ax.Menu:create()
        local size = ax.Director:getInstance():getLogicalSize()
        local item1 = ax.MenuItemImage:create(s_pPathB1, s_pPathB2)
        item1:registerScriptTapHandler(backCallback)
        ordercallbackmenu:addChild(item1,kItemTagBasic)
        local item2 = ax.MenuItemImage:create(s_pPathR1, s_pPathR2)
        item2:registerScriptTapHandler(restartCallback)
        ordercallbackmenu:addChild(item2,kItemTagBasic)
        local item3 = ax.MenuItemImage:create(s_pPathF1, s_pPathF2)
        ordercallbackmenu:addChild(item3,kItemTagBasic)
        item3:registerScriptTapHandler(nextCallback)

        item1:setPosition(ax.p(size.width / 2 - item2:getContentSize().width * 2, item2:getContentSize().height / 2))
        item2:setPosition(ax.p(size.width / 2, item2:getContentSize().height / 2))
        item3:setPosition(ax.p(size.width / 2 + item2:getContentSize().width * 2, item2:getContentSize().height / 2))

        ordercallbackmenu:setPosition(ax.p(0, 0))

        return ordercallbackmenu
    end

    local function GetTitle()
        return "Test DrawNode"
    end

    local function GetSubTitle()
        return "Testing DrawNode - batched draws. Concave polygons are BROKEN"
    end

    local function InitTitle(layer)
        --Title
        local lableTitle = ax.Label:createWithTTF(GetTitle(), s_arialPath, 40)
        layer:addChild(lableTitle, 15)
        lableTitle:setAnchorPoint(ax.p(0.5, 0.5))
        lableTitle:setPosition(ax.p(size.width / 2, size.height - 32))
        lableTitle:setColor(ax.color32(255, 255, 40, 255))
        --SubTitle
        local subLabelTitle = ax.Label:createWithTTF(GetSubTitle(), s_thonburiPath, 16)
        layer:addChild(subLabelTitle, 15)
        subLabelTitle:setAnchorPoint(ax.p(0.5, 0.5))
        subLabelTitle:setPosition(ax.p(size.width / 2, size.height - 80))
    end

    local function createDrawNodeTest()
        local layer = ax.Layer:create()

        InitTitle(layer)

        local draw = ax.DrawNode:create()
        layer:addChild(draw, 10)

        --draw 1 point
        draw:drawPoint(ax.p(60,60), 4, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 1))

        --draw 4 small points
        local fourpoints = { ax.p(60,60), ax.p(70,70), ax.p(60,70), ax.p(70,60) }
        draw:drawPoints(fourpoints, 4, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 1))

        --draw a line
        draw:drawLine(ax.p(0,0), ax.p(size.width, size.height), ax.color(0,1,0,1))

        --draw a rectangle
        draw:drawRect(ax.p(23,23), ax.p(7,7), ax.color(1,1,0,1))

        --draw a solid polygon
        local vertices3 = { ax.p(60,160), ax.p(70,190), ax.p(100,190), ax.p(90,160) }
        draw:drawSolidPoly( vertices3, 4, ax.color(0,0,1,1) )

        --draw a solid rectangle
        draw:drawSolidRect(ax.p(10,10), ax.p(20,20), ax.color(1,1,0,1))

        --draw a solid circle
        draw:drawSolidCircle(ax.p(VisibleRect:center().x + 140 ,VisibleRect:center().y), 100, math.pi/2, 50, 1.0, 2.0, ax.color(1,0,0,0.2))

        --draw open random color poly
        local vertices = { ax.p(0,0), ax.p(50,50), ax.p(100,50), ax.p(100,100), ax.p(50,100) }
        draw:drawPoly( vertices, 5, false, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 1))

        --draw closed random color poly
        local vertices2 = { ax.p(30,130), ax.p(30,230), ax.p(50,200) }
        draw:drawPoly( vertices2, 3, true, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 1))

        --draw two circle
        draw:drawCircle(ax.p(VisibleRect:center().x + 140 ,VisibleRect:center().y), 110, math.pi/2, 50, true, 1.0, 2.0, ax.color(1.0, 0.0, 0.0, 0.5))

        draw:drawCircle(ax.p(VisibleRect:center().x - 140 ,VisibleRect:center().y), 50, math.pi/2, 30, false, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 1))

        --draw some beziers
        draw:drawQuadBezier(ax.p(size.width - 150, size.height - 150), ax.p(size.width - 70, size.height - 10), ax.p(size.width - 10, size.height - 10), 10, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))

        draw:drawQuadBezier(ax.p(0, size.height), ax.p(size.width/2, size.height/2), ax.p(size.width, size.height), 50, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))

        draw:drawCubicBezier(ax.p(VisibleRect:center()), ax.p(VisibleRect:center().x+30,VisibleRect:center().y+50), ax.p(VisibleRect:center().x+60,VisibleRect:center().y-50),VisibleRect:right(),100, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))

        --draw Cardinal spline and catmullrom
        local array = {
                ax.p(0, 0),
                ax.p(size.width / 2 - 30, 0),
                ax.p(size.width / 2 - 30, size.height - 80),
                ax.p(0, size.height - 80),
                ax.p(0, 0) }
        draw:drawCardinalSpline(array, 0.5, 50, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))

        local array2 = {
                ax.p(size.width / 2, 30),
                ax.p(size.width - 80, 30),
                ax.p(size.width - 80, size.height - 80),
                ax.p(size.width / 2, size.height - 80),
                ax.p(size.width / 2, 30) }
        draw:drawCatmullRom(array2, 50, ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))

        --Draw 10 circles
        for i=1, 10 do
            draw:drawDot(ax.p(size.width/2, size.height/2), 10*(10-i), ax.color(math.random(0,1), math.random(0,1), math.random(0,1), 0.5))
        end

        --Draw polygons
        points = { ax.p(size.height/4, 0), ax.p(size.width, size.height / 5), ax.p(size.width / 3 * 2, size.height) }
        draw:drawPolygon(points, #(points), ax.color(1,0,0,0.5), 4, ax.color(0,0,1,1))

        local o = 80
        local w = 20
        local h = 50
        local star1 = { ax.p( o + w, o - h), ax.p(o + w * 2, o), ax.p(o + w * 2 + h, o + w), ax.p(o + w * 2, o + w * 2) }

        draw:drawPolygon(star1, #(star1), ax.color(1,0,0,0.5), 1, ax.color(0,0,1,1))

        o = 180
        w = 20
        h = 50
        local star2 = {
            ax.p(o, o), ax.p(o + w, o - h), ax.p(o + w * 2, o),        --lower spike
            ax.p(o + w * 2 + h, o + w ), ax.p(o + w * 2, o + w * 2),      --right spike
            ax.p(o + w, o + w * 2 + h), ax.p(o, o + w * 2),               --top spike
            ax.p(o - h, o + w),                                              --left spike
        };

        draw:drawPolygon(star2, #(star2), ax.color(1,0,0,0.5), 1, ax.color(0,0,1,1))

        draw:drawSegment(ax.p(20,size.height), ax.p(20,size.height/2), 10, ax.color(0, 1, 0, 1))

        draw:drawSegment(ax.p(10,size.height/2), ax.p(size.width/2, size.height/2), 40, ax.color(1, 0, 1, 0.5))

        return layer
    end

    local function createLayerByCurCase(curCase)
        return createDrawNodeTest()
    end

    function showCurrentTest()
        local curScene = ax.Scene:create()
        if nil ~= curScene then
            curLayer = createLayerByCurCase(curCase)
            if nil ~= curLayer then
                curScene:addChild(curLayer)
                curLayer:addChild(orderCallbackMenu(),15)
                curScene:addChild(CreateBackMenuItem())
                ax.Director:getInstance():replaceScene(curScene)
            end
        end
    end

    curLayer = createLayerByCurCase(curCase)
    curLayer:addChild(orderCallbackMenu(),15)
    return curLayer
end

function DrawNodeTest()
    local scene = ax.Scene:create()
    scene:addChild(DrawNodeMainLayer())
    scene:addChild(CreateBackMenuItem())
    return scene
end

