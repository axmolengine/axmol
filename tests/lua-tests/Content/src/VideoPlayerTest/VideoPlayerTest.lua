local visibleRect = ax.Director:getInstance():getRenderView():getVisibleRect()
local centerPos   = ax.p(visibleRect.x + visibleRect.width / 2,visibleRect.y + visibleRect.height /2)

axui.VideoPlayer = axui.MediaPlayer

local function VideoPlayerTest()
    local layer = ax.Layer:create() --createTestLayer("VideoPlayerTest", "")
    titleLabel = ax.Label:createWithTTF("VideoPlayerTest", s_arialPath, 28)
    titleLabel:setAnchorPoint(ax.p(0.5, 0.5))
    layer:addChild(titleLabel, 1)

    ax.MenuItemFont:setFontSize(16)

    widget = ccs.GUIReader:getInstance():widgetFromJsonFile("cocosui/UITest/UITest.json")
    layer:addChild(widget)

    local videoStateLabel = ax.Label:createWithSystemFont("IDLE","Arial",16)
    videoStateLabel:setAnchorPoint(ax.p(1, 0.5))
    videoStateLabel:setPosition(ax.p(visibleRect.x + visibleRect.width - 10,visibleRect.y + 200))
    layer:addChild(videoStateLabel)

    local function onVideoEventCallback(sener, eventType)
        if eventType == ccui.VideoPlayerEvent.PLAYING then
            videoStateLabel:setString("PLAYING")
        elseif eventType == ccui.VideoPlayerEvent.PAUSED then
            videoStateLabel:setString("PAUSED")
        elseif eventType == ccui.VideoPlayerEvent.STOPPED then
            videoStateLabel:setString("STOPPED")
        elseif eventType == ccui.VideoPlayerEvent.COMPLETED then
            videoStateLabel:setString("COMPLETED")
        end
    end
    local widgetSize = widget:getContentSize()
    local videoPlayer = ccui.VideoPlayer:create()
    videoPlayer:setPosition(centerPos)
    videoPlayer:setAnchorPoint(ax.p(0.5, 0.5))
    videoPlayer:setContentSize(ax.size(widgetSize.width * 0.4,widgetSize.height * 0.4))
    videoPlayer:addEventListener(onVideoEventCallback)
    layer:addChild(videoPlayer)

    local screenSize = ax.Director:getInstance():getWinSize()
    local rootSize = widget:getContentSize()
    layer:setPosition(ax.p((screenSize.width - rootSize.width) / 2,(screenSize.height - rootSize.height) / 2))

    local function menuFullScreenCallback(tag, sender)
        if nil  ~= videoPlayer then
            videoPlayer:setFullScreenEnabled(not videoPlayer:isFullScreenEnabled())
        end
    end
    local fullSwitch = ax.MenuItemFont:create("FullScreenSwitch")
    fullSwitch:setAnchorPoint(ax.p(0.0, 0.0))
    fullSwitch:setPosition(ax.p(visibleRect.x + 10,visibleRect.y + 50))
    fullSwitch:registerScriptTapHandler(menuFullScreenCallback)

    local function menuPauseCallback(tag, sender)
        if nil  ~= videoPlayer then
            videoPlayer:pause()
        end
    end
    local pauseItem = ax.MenuItemFont:create("Pause")
    pauseItem:setAnchorPoint(ax.p(0.0, 0.0))
    pauseItem:setPosition(ax.p(visibleRect.x + 10,visibleRect.y + 100))
    pauseItem:registerScriptTapHandler(menuPauseCallback)

    local function menuResumeCallback(tag, sender)
        if nil  ~= videoPlayer then
            videoPlayer:resume()
        end
    end
    local resumeItem = ax.MenuItemFont:create("Resume")
    resumeItem:setAnchorPoint(ax.p(0.0, 0.0))
    resumeItem:setPosition(ax.p(visibleRect.x + 10,visibleRect.y + 150))
    resumeItem:registerScriptTapHandler(menuResumeCallback)

    local function menuStopCallback(tag, sender)
        if nil  ~= videoPlayer then
            videoPlayer:stop()
        end
    end
    local stopItem = ax.MenuItemFont:create("Stop")
    stopItem:setAnchorPoint(ax.p(0.0, 0.0))
    stopItem:setPosition(ax.p(visibleRect.x + 10,visibleRect.y + 200))
    stopItem:registerScriptTapHandler(menuStopCallback)

    local function menuHintCallback(tag, sender)
        if nil  ~= videoPlayer then
            videoPlayer:setVisible(not videoPlayer:isVisible())
        end
    end
    local hintItem = ax.MenuItemFont:create("Hint")
    hintItem:setAnchorPoint(ax.p(0.0, 0.0))
    hintItem:setPosition(ax.p(visibleRect.x + 10,visibleRect.y + 250))
    hintItem:registerScriptTapHandler(menuHintCallback)

    ------------------------------------------------------------
    local function menuResourceVideoCallback(tag, sender)
        if nil ~= videoPlayer then
            print('start play video')
            local videoFullPath = ax.FileUtils:getInstance():fullPathForFilename("video/h264/1280x720.mp4")
            videoPlayer:setFileName(videoFullPath)
            videoPlayer:play()
            print('start play video succeed')
        end
    end

    local resourceVideo = ax.MenuItemFont:create("Play resource video")
    resourceVideo:setAnchorPoint(ax.p(1, 0.5))
    resourceVideo:setPosition(ax.p(visibleRect.x + visibleRect.width - 10,visibleRect.y + 50))
    resourceVideo:registerScriptTapHandler(menuResourceVideoCallback)

    local function menuOnlineVideoCallback(tag, sender)
        if nil ~= videoPlayer then
            videoPlayer:setURL("http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_1080p_30fps_normal.mp4")
            videoPlayer:play()
        end
    end
    local onlineVideo = ax.MenuItemFont:create("Play online video")
    onlineVideo:setAnchorPoint(ax.p(1, 0.5))
    onlineVideo:setPosition(ax.p(visibleRect.x + visibleRect.width - 10,visibleRect.y + 100))
    onlineVideo:registerScriptTapHandler(menuOnlineVideoCallback)

    local function menuRatioCallback(tag, sender)
        if nil ~= videoPlayer then
            videoPlayer:setKeepAspectRatioEnabled(not videoPlayer:isKeepAspectRatioEnabled())
        end
    end
    local ratioSwitch = ax.MenuItemFont:create("KeepRatioSwitch")
    ratioSwitch:setAnchorPoint(ax.p(1, 0.5))
    ratioSwitch:setPosition(ax.p(visibleRect.x + visibleRect.width - 10,visibleRect.y + 150))
    ratioSwitch:registerScriptTapHandler(menuRatioCallback)

    local menu = ax.Menu:create(fullSwitch, pauseItem, resumeItem, stopItem, hintItem, resourceVideo, onlineVideo, ratioSwitch)
    menu:setPosition(ax.p(0.0, 0.0))
    layer:addChild(menu)

    return layer
end



function VideoPlayerTestMain()
    local scene = ax.Scene:create()
    Helper.createFunctionTable = {
        VideoPlayerTest
    }
    Helper.index = 1

    scene:addChild(VideoPlayerTest())
    scene:addChild(CreateBackMenuItem())
    return scene
end
