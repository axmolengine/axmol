local SliderEx = class("SliderEx", function()
    local slider = ax.Sprite:create("cocosui/sliderTrack.png")
    return slider
end)

SliderEx.TouchEventDown = 0
SliderEx.TouchEventMove = 1
SliderEx.TouchEventUp = 2
SliderEx.TouchEventCancel = 3

function SliderEx:create()
    local slider = SliderEx:new()
    slider:registerScriptHandler(function(tag)
        if "enter" == tag then
            slider:onEnter()
        elseif "exit" == tag then
        end
    end)
    return slider
end

function SliderEx:onEnter()
    self._ratio = 0
    self._thumbX = 0

    local progressBar = ax.Sprite:create("cocosui/sliderProgress.png")
    progressBar:setAnchorPoint(ax.p(0,0))
    self:addChild(progressBar)

    local sliderSize = self:getContentSize()
    self._width = sliderSize.width
    self._thumb = ax.Sprite:create("cocosui/sliderThumb.png")
    self._thumb:setPosition(ax.p(0,sliderSize.height/2))
    self:addChild(self._thumb)

    local thumbSize = self._thumb:getContentSize()
    self._thumbRect = ax.rect(0, 0, thumbSize.width, thumbSize.height)

    local  listenner = ax.EventListenerTouchOneByOne:create()
    listenner:setSwallowTouches(true)
    listenner:registerScriptHandler(function(touch, event)
        local location = touch:getLocation()
        local locationInNode = self._thumb:convertToNodeSpace(location)
        if not ax.rectContainsPoint(self._thumbRect, locationInNode) then
            return false
        end

        self._touchBeganX = self:convertToNodeSpace(location).x
        self._thumbBeganX = self._thumbX
        if self._callback then
            self._callback(self,self._ratio,SliderEx.TouchEventDown)
        end
        return true
    end, ax.Handler.EVENT_TOUCH_BEGAN )

    listenner:registerScriptHandler(function(touch, event)
        local locationInNodeX = self:convertToNodeSpace(touch:getLocation()).x
        self:setThumbPosX(self._thumbBeganX + locationInNodeX - self._touchBeganX)

        if self._callback then
            self._callback(self,self._ratio,SliderEx.TouchEventMove)
        end
    end, ax.Handler.EVENT_TOUCH_MOVED )

    listenner:registerScriptHandler(function(touch, event)
        local locationInNodeX = self:convertToNodeSpace(touch:getLocation()).x
        self:setThumbPosX(self._thumbBeganX + locationInNodeX - self._touchBeganX)

        if self._callback then
            self._callback(self,self._ratio,SliderEx.TouchEventUp)
        end
    end, ax.Handler.EVENT_TOUCH_ENDED )

    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listenner, self)
end

function SliderEx:setThumbPosX(newX)
    self._thumbX = newX
    if self._thumbX < 0 then
        self._thumbX = 0
    elseif self._thumbX > self._width then
        self._thumbX = self._width
    end

    self._ratio = self._thumbX / self._width
    self._thumb:setPositionX(self._thumbX)
end

function SliderEx:addRatioListener(callback)
    self._callback = callback
end

function SliderEx:setRatio(ratio)
    if ratio < 0 then
        ratio = 0
    elseif ratio > 1.0 then
        ratio = 1.0
    end

    self._ratio = ratio
    self._thumbX = self._ratio * self._width
    self._thumb:setPositionX(self._thumbX)
end

------------------------------------------------------------------------
local AudioControlTest = {}

function AudioControlTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("audio control test")
    -- Helper.subtitleLabel:setString("subtitleLabel")
    local layerSize = layer:getContentSize()

    AudioControlTest._audioID = ax.AUDIO_INVAILD_ID
    AudioControlTest._loopEnabled = false
    AudioControlTest._volume = 1.0
    AudioControlTest._duration = ax.AUDIO_TIME_UNKNOWN
    AudioControlTest._timeRatio = 0.0
    AudioControlTest._updateTimeSlider = true

    ------playItem
    local function playAudio(tag, sender)
        if AudioControlTest._audioID == ax.AUDIO_INVAILD_ID then
            AudioControlTest._audioID = ax.AudioEngine:play2d("background.mp3", AudioControlTest._loopEnabled, AudioControlTest._volume)

            if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
                AudioControlTest._playItem:setEnabled(false)

                local function finishCallback(audioID,filePath)
                    AudioControlTest._audioID = ax.AUDIO_INVAILD_ID
                    AudioControlTest._playItem:setEnabled(true)
                end

                ax.AudioEngine:setFinishCallback(AudioControlTest._audioID,finishCallback)
            end
        end
    end

    AudioControlTest._playItem = ax.MenuItemFont:create("play")
    AudioControlTest._playItem:setPosition(ax.p(layerSize.width * 0.3,layerSize.height * 0.7))
    AudioControlTest._playItem:registerScriptTapHandler(playAudio)

    ------stopItem
    local function stopAudio(tag, sender)
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            ax.AudioEngine:stop(AudioControlTest._audioID)
            AudioControlTest._audioID = ax.AUDIO_INVAILD_ID
            AudioControlTest._playItem:setEnabled(true)
        end
    end

    local stopItem = ax.MenuItemFont:create("stop")
    stopItem:setPosition(ax.p(layerSize.width * 0.7,layerSize.height * 0.7))
    stopItem:registerScriptTapHandler(stopAudio)

    ------pauseItem
    local function pauseAudio(tag, sender)
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            ax.AudioEngine:pause(AudioControlTest._audioID)
        end
    end

    local pauseItem = ax.MenuItemFont:create("pause")
    pauseItem:setPosition(ax.p(layerSize.width * 0.3,layerSize.height * 0.6))
    pauseItem:registerScriptTapHandler(pauseAudio)

    ------resumeItem
    local function resumeAudio(tag, sender)
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            ax.AudioEngine:resume(AudioControlTest._audioID)
        end
    end

    local resumeItem = ax.MenuItemFont:create("resume")
    resumeItem:setPosition(ax.p(layerSize.width * 0.7,layerSize.height * 0.6))
    resumeItem:registerScriptTapHandler(resumeAudio)

    ------loopItem
    local function switchLoop(tag, sender)
        AudioControlTest._loopEnabled = not AudioControlTest._loopEnabled
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            ax.AudioEngine:setLoop(AudioControlTest._audioID, AudioControlTest._loopEnabled)
        end
        if AudioControlTest._loopEnabled then
            AudioControlTest.loopItem:setString("disable-loop")
        else
            AudioControlTest.loopItem:setString("enable-loop")
        end
    end

    AudioControlTest.loopItem = ax.MenuItemFont:create("enable-loop")
    AudioControlTest.loopItem:setPosition(ax.p(layerSize.width * 0.3,layerSize.height * 0.5))
    AudioControlTest.loopItem:registerScriptTapHandler(switchLoop)

    ------uncacheItem
    local function uncache(tag, sender)
        ax.AudioEngine:uncache("background.mp3")
        AudioControlTest._audioID = ax.AUDIO_INVAILD_ID
        AudioControlTest._playItem:setEnabled(true)
    end

    local uncacheItem = ax.MenuItemFont:create("uncache")
    uncacheItem:setPosition(ax.p(layerSize.width * 0.7,layerSize.height * 0.5))
    uncacheItem:registerScriptTapHandler(uncache)

    local menu = ax.Menu:create()
    menu:addChild(AudioControlTest._playItem)
    menu:addChild(stopItem)
    menu:addChild(pauseItem)
    menu:addChild(resumeItem)
    menu:addChild(AudioControlTest.loopItem)
    menu:addChild(uncacheItem)
    menu:setPosition(ax.p(0, 0))
    layer:addChild(menu)

    local function volumeSliderChangedEvent(sender,eventType)
        AudioControlTest._volume = sender:getPercent() / 100.0
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            ax.AudioEngine:setVolume(AudioControlTest._audioID, AudioControlTest._volume)
        end
    end
    local volumeSlider = ccui.Slider:create()
    volumeSlider:setTouchEnabled(true)
    volumeSlider:loadBarTexture("cocosui/sliderTrack.png")
    volumeSlider:loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "")
    volumeSlider:loadProgressBarTexture("cocosui/sliderProgress.png")
    volumeSlider:setPosition(ax.p(layerSize.width * 0.5,layerSize.height * 0.35))
    volumeSlider:addEventListener(volumeSliderChangedEvent)
    volumeSlider:setPercent(100)
    layer:addChild(volumeSlider)

    local sliderSize = volumeSlider:getContentSize()
    local volumeLabel = ax.Label:createWithTTF("volume:  ","fonts/arial.ttf", 20)
    volumeLabel:setAnchorPoint(ax.p(1.0,0.5))
    volumeLabel:setPosition(ax.p((layerSize.width - sliderSize.width)/2, layerSize.height * 0.35))
    layer:addChild(volumeLabel)

    local timeLabel = ax.Label:createWithTTF("time:  ","fonts/arial.ttf", 20)
    timeLabel:setAnchorPoint(ax.p(1.0,0.5))
    timeLabel:setPosition(ax.p((layerSize.width - sliderSize.width)/2, layerSize.height * 0.25))
    layer:addChild(timeLabel)

    local function timeSlideChangeCallback(sender,ratio,eventType)
        if (eventType == SliderEx.TouchEventDown or eventType == SliderEx.TouchEventMove) then
            AudioControlTest._updateTimeSlider = false
        else
            if (AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID and AudioControlTest._duration ~= ax.AUDIO_TIME_UNKNOWN) then
                ax.AudioEngine:setCurrentTime(AudioControlTest._audioID,AudioControlTest._duration * ratio)
            end
            AudioControlTest._updateTimeSlider = true
        end
    end

    AudioControlTest._timeSlider = SliderEx:create()
    AudioControlTest._timeSlider:setPosition(ax.p(layerSize.width * 0.5,layerSize.height * 0.25))
    AudioControlTest._timeSlider:addRatioListener(timeSlideChangeCallback)
    layer:addChild(AudioControlTest._timeSlider)

    local function step(dt)
        if AudioControlTest._audioID ~= ax.AUDIO_INVAILD_ID then
            if AudioControlTest._duration == ax.AUDIO_TIME_UNKNOWN then
                AudioControlTest._duration = ax.AudioEngine:getDuration(AudioControlTest._audioID)
            end
            if AudioControlTest._duration ~= ax.AUDIO_TIME_UNKNOWN then
                local time = ax.AudioEngine:getCurrentTime(AudioControlTest._audioID)
                AudioControlTest._timeRatio = time / AudioControlTest._duration
                if AudioControlTest._updateTimeSlider then
                    AudioControlTest._timeSlider:setRatio(AudioControlTest._timeRatio)
                end
            end
        end
    end

    layer:scheduleUpdateWithPriorityLua(step, 0.05)

    function onNodeEvent(tag)
        if tag == "exit" then
            ax.AudioEngine:stopAll()
        end
    end
    layer:registerScriptHandler(onNodeEvent)

    return layer
end
------------------------------------------------------------------------
local PlaySimultaneouslyTest = {}

function PlaySimultaneouslyTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("Simultaneously play multiple audio")

    PlaySimultaneouslyTest._playingcount = 0
    PlaySimultaneouslyTest.files = {}
    for index=1,10 do
        PlaySimultaneouslyTest.files[index] = string.format("audio/SoundEffectsFX009/FX0%d.mp3",80 + index)
    end

    local function playAudio(tag, sender)
        local audioID = ax.AUDIO_INVAILD_ID
        --for k, v in pairs(PlaySimultaneouslyTest.files) do
        for index=1,10 do
            audioID = ax.AudioEngine:play2d(PlaySimultaneouslyTest.files[index])

            if audioID ~= ax.AUDIO_INVAILD_ID then
                PlaySimultaneouslyTest._playItem:setEnabled(false)
                PlaySimultaneouslyTest._playingcount = PlaySimultaneouslyTest._playingcount + 1

                local function finishCallback(audioID,filePath)
                    PlaySimultaneouslyTest._playingcount = PlaySimultaneouslyTest._playingcount - 1
                    if PlaySimultaneouslyTest._playingcount <= 0 then
                        PlaySimultaneouslyTest._playItem:setEnabled(true)
                    end
                end

                ax.AudioEngine:setFinishCallback(audioID,finishCallback)
            end
        end
    end

    PlaySimultaneouslyTest._playItem = ax.MenuItemFont:create("play-simultaneously")
    PlaySimultaneouslyTest._playItem:setNormalizedPosition(ax.p(0.5,0.5))
    PlaySimultaneouslyTest._playItem:registerScriptTapHandler(playAudio)

    local menu = ax.Menu:create(PlaySimultaneouslyTest._playItem)
    menu:setPosition(ax.p(0, 0))
    layer:addChild(menu)

    function onNodeEvent(tag)
        if tag == "exit" then
            ax.AudioEngine:stopAll()
        end
    end
    layer:registerScriptHandler(onNodeEvent)

    return layer
end
------------------------------------------------------------------------
local AudioProfileTest = {}

function AudioProfileTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("AudioProfileTest")
    Helper.subtitleLabel:setString("See the console.")
    local layerSize = layer:getContentSize()

    AudioProfileTest._files = {}
    AudioProfileTest._files[1] = "background.mp3"
    local targetPlatform = ax.Application:getInstance():getTargetPlatform()
    if (ax.PLATFORM_OS_MAC == targetPlatform or ax.PLATFORM_OS_IPHONE == targetPlatform or ax.PLATFORM_OS_IPAD == targetPlatform) then
        AudioProfileTest._files[2] = "background.caf"
    else
        AudioProfileTest._files[2] = "background.ogg"
    end

    AudioProfileTest._profile = ax.AudioProfile:new()
    AudioProfileTest._profile.name = "AudioProfileTest"
    AudioProfileTest._profile.maxInstances = 3
    AudioProfileTest._profile.minDelay = 1.0

    AudioProfileTest._audioCount = 0
    AudioProfileTest._minDelay = 1.0
    AudioProfileTest._time = 0.0

    local menu = ax.Menu:create()
    menu:setPosition(ax.p(0, 0))
    layer:addChild(menu)

    local heightRatio = 0.7
    for index=1,2 do
        local function itemClickCallback(tag, sender)
            local audioID = ax.AudioEngine:play2d(AudioProfileTest._files[tag],false,1.0,AudioProfileTest._profile)
            if audioID ~= ax.AUDIO_INVAILD_ID then
                AudioProfileTest._time = AudioProfileTest._minDelay
                AudioProfileTest._audioCount = AudioProfileTest._audioCount + 1
                AudioProfileTest._showLabel:setString(string.format("audio count:%d",AudioProfileTest._audioCount))

                local function finishCallback(audioID,filePath)
                    AudioProfileTest._audioCount = AudioProfileTest._audioCount - 1
                    AudioProfileTest._showLabel:setString(string.format("audio count:%d",AudioProfileTest._audioCount))
                end

                ax.AudioEngine:setFinishCallback(audioID,finishCallback)
            end
        end

        local item = ax.MenuItemFont:create("play "..AudioProfileTest._files[index])
        item:setFontSizeObj(20)
        item:setPosition(ax.p(layerSize.width * 0.5,layerSize.height * heightRatio))
        item:registerScriptTapHandler(itemClickCallback)
        menu:addChild(item,0,index)
        heightRatio = heightRatio - 0.1
    end

    local origin = ax.Director:getInstance():getVisibleOrigin()
    local size = ax.Director:getInstance():getVisibleSize()

    local profileInfoLabel = ax.Label:createWithTTF("AudioProfile Info:\n    max instance:3  \n    minimum delay:1.0","fonts/arial.ttf", 12)
    profileInfoLabel:setAnchorPoint(ax.p(0,0.5))
    profileInfoLabel:setPosition(ax.p(origin.x, origin.y + size.height * 0.65))
    layer:addChild(profileInfoLabel)

    AudioProfileTest._showLabel = ax.Label:createWithTTF("audio count:0","fonts/arial.ttf", 12)
    AudioProfileTest._showLabel:setAnchorPoint(ax.p(0,0.5))
    AudioProfileTest._showLabel:setPosition(ax.p(origin.x, origin.y + size.height * 0.5))
    layer:addChild(AudioProfileTest._showLabel)

    AudioProfileTest._timeSlider = ccui.Slider:create()
    AudioProfileTest._timeSlider:setTouchEnabled(false)
    AudioProfileTest._timeSlider:loadBarTexture("cocosui/sliderTrack.png")
    AudioProfileTest._timeSlider:loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "")
    AudioProfileTest._timeSlider:loadProgressBarTexture("cocosui/sliderProgress.png")
    AudioProfileTest._timeSlider:setPosition(ax.p(layerSize.width * 0.5,layerSize.height * heightRatio))
    layer:addChild(AudioProfileTest._timeSlider)

    local function step(dt)
        if AudioProfileTest._time > 0 then
            AudioProfileTest._time = AudioProfileTest._time - dt
            if AudioProfileTest._time < 0 then
                AudioProfileTest._time = 0
            end
            AudioProfileTest._timeSlider:setPercent(100 * AudioProfileTest._time / AudioProfileTest._minDelay)
        end
    end
    layer:scheduleUpdateWithPriorityLua(step, 0.05)

    function onNodeEvent(tag)
        if tag == "exit" then
            ax.AudioEngine:stopAll()
        end
    end
    layer:registerScriptHandler(onNodeEvent)

    return layer
end
------------------------------------------------------------------------
local InvalidAudioFileTest = {}

function InvalidAudioFileTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("Test invalid audio file")
    Helper.subtitleLabel:setString("Not crash,please see the console.")
    local layerSize = layer:getContentSize()

    -- unsupported media type
    local function playUnsupportedAudio(tag, sender)
        local targetPlatform = ax.Application:getInstance():getTargetPlatform()
        if (ax.PLATFORM_OS_MAC == currPlatform or ax.PLATFORM_OS_IPHONE == targetPlatform or ax.PLATFORM_OS_IPAD == targetPlatform) then
            ax.AudioEngine:play2d("background.ogg")
        end
        if (ax.PLATFORM_OS_ANDROID == targetPlatform) then
            ax.AudioEngine:play2d("background.caf")
        end
    end

    local playItem1 = ax.MenuItemFont:create("play unsupported media type")
    playItem1:setNormalizedPosition(ax.p(0.5,0.6))
    playItem1:registerScriptTapHandler(playUnsupportedAudio)

    -- not-existent audio
    local function playNotExistentAudio(tag, sender)
        ax.AudioEngine:play2d("not-existent file.mp3")
    end

    local playItem2 = ax.MenuItemFont:create("play not-existent file")
    playItem2:setNormalizedPosition(ax.p(0.5,0.4))
    playItem2:registerScriptTapHandler(playNotExistentAudio)

    local menu = ax.Menu:create()
    menu:addChild(playItem1)
    menu:addChild(playItem2)
    menu:setPosition(ax.p(0, 0))
    layer:addChild(menu)

    return layer
end
------------------------------------------------------------------------
local LargeAudioFileTest = {}

function LargeAudioFileTest.create()
    local layer = ax.Layer:create()
    Helper.initWithLayer(layer)
    Helper.titleLabel:setString("Test large audio file")
    local layerSize = layer:getContentSize()

    local function playAudio(tag, sender)
        ax.AudioEngine:play2d("audio/LuckyDay.mp3")
    end

    local playItem = ax.MenuItemFont:create("play large audio file")
    playItem:setNormalizedPosition(ax.p(0.5,0.5))
    playItem:registerScriptTapHandler(playAudio)

    local menu = ax.Menu:create(playItem)
    menu:setPosition(ax.p(0, 0))
    layer:addChild(menu)

    function onNodeEvent(tag)
        if tag == "exit" then
            ax.AudioEngine:stopAll()
        end
    end
    layer:registerScriptHandler(onNodeEvent)

    return layer
end
------------------------------------------------------------------------
function AudioEngineTest()
    local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        AudioControlTest.create,
        PlaySimultaneouslyTest.create,
        AudioProfileTest.create,
        InvalidAudioFileTest.create,
        LargeAudioFileTest.create
    }
    Helper.index = 1

    scene:addChild(AudioControlTest.create())
    scene:addChild(CreateBackMenuItem())
    return scene
end
