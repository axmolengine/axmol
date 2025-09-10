local LightTest = class("LightTest",function()
    return ax.Layer:create()
end)

function LightTest:ctor()

  local function onNodeEvent(event)
      if event == "enter" then
          self:init()
      elseif event == "exit" then
          self:unscheduleUpdate()
      end
  end

  self:registerScriptHandler(onNodeEvent)
end

function LightTest:init()

    self:addSprite()
    self:addLights()

    local s = ax.Director:getInstance():getLogicalSize()
    local camera = ax.Camera:createPerspective(60, s.width/s.height, 1.0, 1000.0)
    camera:setCameraFlag(ax.CameraFlag.USER1)
    camera:setPosition3D(ax.vec3(0.0, 100, 100))
    camera:lookAt(ax.vec3(0.0, 0.0, 0.0), ax.vec3(0.0, 1.0, 0.0))
    self:addChild(camera)

    local ttfConfig = {}
    ttfConfig.fontFilePath = "fonts/arial.ttf"
    ttfConfig.fontSize = 15

    local ambientLightLabel = ax.Label:createWithTTF(ttfConfig,"Ambient Light ON")
    local menuItem0 = ax.MenuItemLabel:create(ambientLightLabel)
    menuItem0:registerScriptTapHandler(function (tag, sender)
        local str = nil
        local isON = not self._ambientLight:isEnabled()
        if isON then
            str = "Ambient Light ON"
        else
            str = "Ambient Light OFF"
        end
        self._ambientLight:setEnabled(isON)
        menuItem0:setString(str)
    end)


    local directionalLightLabel = ax.Label:createWithTTF(ttfConfig,"Directional Light OFF")
    local menuItem1 = ax.MenuItemLabel:create(directionalLightLabel)
    menuItem1:registerScriptTapHandler(function (tag, sender)
        local str = nil
        local isON = not self._directionalLight:isEnabled()
        if isON then
            str = "Directional Light ON"
        else
            str = "Directional Light OFF"
        end
        self._directionalLight:setEnabled(isON)
        menuItem1:setString(str)
    end)


    local pointLightLabel = ax.Label:createWithTTF(ttfConfig,"Point Light OFF")
    local menuItem2 = ax.MenuItemLabel:create(pointLightLabel)
    menuItem2:registerScriptTapHandler(function (tag, sender)
        local str = nil
        local isON = not self._pointLight:isEnabled()
        if isON then
            str = "Point Light ON"
        else
            str = "Point Light OFF"
        end
        self._pointLight:setEnabled(isON)
        menuItem2:setString(str)
    end)

    local spotLightLabel = ax.Label:createWithTTF(ttfConfig,"Spot Light OFF")
    local menuItem3 = ax.MenuItemLabel:create(spotLightLabel)
    menuItem3:registerScriptTapHandler(function (tag, sender)
        local str = nil
        local isON = not self._spotLight:isEnabled()
        if isON then
            str = "Spot Light ON"
        else
            str = "Spot Light OFF"
        end
        self._spotLight:setEnabled(isON)
        menuItem3:setString(str)
    end)


    local menu = ax.Menu:create(menuItem0, menuItem1, menuItem2, menuItem3)
    menu:setPosition(ax.p(0, 0))
    menuItem0:setAnchorPoint(ax.p(0.0 ,1.0))
    menuItem0:setPosition( ax.p(VisibleRect:left().x, VisibleRect:top().y-50) )
    menuItem1:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem1:setPosition( ax.p(VisibleRect:left().x, VisibleRect:top().y-100) )
    menuItem2:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem2:setPosition( ax.p(VisibleRect:left().x, VisibleRect:top().y -150))
    menuItem3:setAnchorPoint(ax.p(0.0, 1.0))
    menuItem3:setPosition( ax.p(VisibleRect:left().x, VisibleRect:top().y -200))
    self:addChild(menu)

    local angleDelta = 0.0
    local function update(delta)
        if nil ~= self._directionalLight then
            self._directionalLight:setRotation3D(ax.vec3(-45.0, -angleDelta * 57.29577951, 0.0))
        end

        if nil ~= self._pointLight then
            self._pointLight:setPositionX(100.0 * math.cos(angleDelta + 2.0 * delta))
            self._pointLight:setPositionY(100.0)
            self._pointLight:setPositionZ(100.0 * math.sin(angleDelta + 2.0 * delta))
        end

        if nil ~= self._spotLight then
            self._spotLight:setPositionX(100.0 * math.cos(angleDelta + 4.0 * delta))
            self._spotLight:setPositionY(100.0)
            self._spotLight:setPositionZ(100.0 * math.sin(angleDelta + 4.0 * delta))
            self._spotLight:setDirection(ax.vec3(-math.cos(angleDelta + 4.0 * delta), -1.0, -math.sin(angleDelta + 4.0 * delta)))
        end

        angleDelta = angleDelta + delta
    end

    self:scheduleUpdateWithPriorityLua(update,0)
end

function LightTest:addSprite()
    local s = ax.Director:getInstance():getLogicalSize()

    local fileName = "MeshRendererTest/orc.c3b"
    local sprite1 = ax.Sprite3D:create(fileName)
    sprite1:setRotation3D(ax.vec3(0.0, 180.0, 0.0))
    sprite1:setPosition(ax.p(0.0, 0.0))
    sprite1:setScale(2.0)
    local sp = ax.Sprite3D:create("MeshRendererTest/axe.c3b")
    sprite1:getAttachNode("Bip001 R Hand"):addChild(sp)
    local animation = ax.Animation3D:create(fileName)
    if nil ~=animation then
        local animate = ax.Animate3D:create(animation)
        sprite1:runAction(ax.RepeatForever:create(animate))
    end
    self:addChild(sprite1)
    sprite1:setCameraMask(2)

    local fileName = "MeshRendererTest/sphere.c3b"
    local sprite2 = ax.Sprite3D:create(fileName)
    sprite2:setPosition(ax.p(30.0, 0.0))
    self:addChild(sprite2)
    sprite2:setCameraMask(2)


    local fileName = "MeshRendererTest/sphere.c3b"
    local sprite3   = ax.Sprite3D:create(fileName)
    sprite3:setScale(0.5)
    sprite3:setPosition(ax.p(-50.0, 0.0))
    self:addChild(sprite3)
    sprite3:setCameraMask(2)

    local fileName = "MeshRendererTest/sphere.c3b"
    local sprite4 = ax.Sprite3D:create(fileName)
    sprite4:setScale(0.5)
    sprite4:setPosition(ax.p(-30.0, 10.0))
    self:addChild(sprite4)
    sprite4:setCameraMask(2)
end

function LightTest:addLights()
    local s = ax.Director:getInstance():getLogicalSize()
    self._ambientLight = ax.AmbientLight:create(ax.color32(200, 200, 200))
    self._ambientLight:setEnabled(true)
    self:addChild(self._ambientLight)
    self._ambientLight:setCameraMask(2)

    self._directionalLight = ax.DirectionLight:create(ax.vec3(-1.0, -1.0, 0.0), ax.color32(200, 200, 200))
    self._directionalLight:setEnabled(false)
    self:addChild(self._directionalLight)
    self._directionalLight:setCameraMask(2)

    self._pointLight = ax.PointLight:create(ax.vec3(0.0, 0.0, 0.0), ax.color32(200, 200, 200), 10000.0)
    self._pointLight:setEnabled(false)
    self:addChild(self._pointLight)
    self._pointLight:setCameraMask(2)

    self._spotLight = ax.SpotLight:create(ax.vec3(-1.0, -1.0, 0.0), ax.vec3(0.0, 0.0, 0.0), ax.color32(200, 200, 200), 0.0, 0.5, 10000.0)
    self._spotLight:setEnabled(false)
    self:addChild(self._spotLight)
    self._spotLight:setCameraMask(2)

    local tintto1 = ax.TintTo:create(4, 0, 0, 255)
    local tintto2 = ax.TintTo:create(4, 0, 255, 0)
    local tintto3 = ax.TintTo:create(4, 255, 0, 0)
    local tintto4 = ax.TintTo:create(4, 255, 255, 255)
    local seq = ax.Sequence:create(tintto1,tintto2, tintto3, tintto4)
    self._ambientLight:runAction(ax.RepeatForever:create(seq))

    tintto1 = ax.TintTo:create(4, 255, 0, 0)
    tintto2 = ax.TintTo:create(4, 0, 255, 0)
    tintto3 = ax.TintTo:create(4, 0, 0, 255)
    tintto4 = ax.TintTo:create(4, 255, 255, 255)
    seq = ax.Sequence:create(tintto1,tintto2, tintto3, tintto4)
    self._directionalLight:runAction(ax.RepeatForever:create(seq))


    tintto1 = ax.TintTo:create(4, 255, 0, 0)
    tintto2 = ax.TintTo:create(4, 0, 255, 0)
    tintto3 = ax.TintTo:create(4, 0, 0, 255)
    tintto4 = ax.TintTo:create(4, 255, 255, 255)
    seq = ax.Sequence:create(tintto2, tintto1, tintto3, tintto4)
    self._pointLight:runAction(ax.RepeatForever:create(seq))

    tintto1 = ax.TintTo:create(4, 255, 0, 0)
    tintto2 = ax.TintTo:create(4, 0, 255, 0)
    tintto3 = ax.TintTo:create(4, 0, 0, 255)
    tintto4 = ax.TintTo:create(4, 255, 255, 255)
    seq = ax.Sequence:create(tintto3, tintto2, tintto1, tintto4)
    self._spotLight:runAction(ax.RepeatForever:create(seq))
end

function LightTest.create( ... )
    local layer = LightTest.new()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("Light Test")
    return layer
end

function LightTestMain()
    cclog("LightTestMain")
    local scene = ax.Scene:create()

    Helper.createFunctionTable =
    {
        LightTest.create,
    }
    Helper.index = 1

    scene:addChild(LightTest.create(), 0)
    scene:addChild(CreateBackMenuItem())
    return scene
end
