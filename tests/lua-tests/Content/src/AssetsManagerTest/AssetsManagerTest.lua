local targetPlatform = ax.Application:getInstance():getTargetPlatform()

local lineSpace = 40
local itemTagBasic = 1000
local menuItemNames =
{
    "enter",
    "reset",
    "update",
}

local canvasSize = ax.Director:getInstance():getCanvasSize()

local function updateLayer()
    local layer = ax.Layer:create()

    local support  = false
    if (ax.PLATFORM_IPHONE == targetPlatform) or (ax.PLATFORM_IPAD == targetPlatform)
        or (ax.PLATFORM_WINDOWS == targetPlatform) or (ax.PLATFORM_ANDROID == targetPlatform)
        or (ax.PLATFORM_MAC  == targetPlatform) then
        support = true
    end

    if not support then
        print("Platform is not supported!")
        return layer
    end

    local isUpdateItemClicked = false
    local assetsManager       = nil
    local pathToSave          = ""

    local menu = ax.Menu:create()
    menu:setPosition(ax.p(0, 0))
    ax.MenuItemFont:setFontName("Arial")
    ax.MenuItemFont:setFontSize(24)

    local progressLable = ax.Label:createWithTTF("",s_arialPath,30)
    progressLable:setAnchorPoint(ax.p(0.5, 0.5))
    progressLable:setPosition(ax.p(140,50))
    layer:addChild(progressLable)

    pathToSave = createDownloadDir()

    local function onError(errorCode)
        if errorCode == ax.ASSETSMANAGER_NO_NEW_VERSION then
            progressLable:setString("no new version")
        elseif errorCode == ax.ASSETSMANAGER_NETWORK then
            progressLable:setString("network error")
        end
    end

    local function onProgress( percent )
        local progress = string.format("downloading %d%%",percent)
        progressLable:setString(progress)
    end

    local function onSuccess()
        progressLable:setString("downloading ok")
    end

    local function getAssetsManager()
        if nil == assetsManager then
            assetsManager = ax.AssetsManager:new("https://raw.github.com/samuele3hu/AssetsManagerTest/master/package.zip",
                                           "https://raw.github.com/samuele3hu/AssetsManagerTest/master/version",
                                           pathToSave)
            assetsManager:retain()
            assetsManager:setDelegate(onError, ax.ASSETSMANAGER_PROTOCOL_ERROR )
            assetsManager:setDelegate(onProgress, ax.ASSETSMANAGER_PROTOCOL_PROGRESS)
            assetsManager:setDelegate(onSuccess, ax.ASSETSMANAGER_PROTOCOL_SUCCESS )
            assetsManager:setConnectionTimeout(3)
        end

        return assetsManager
    end

    local function update(sender)
        progressLable:setString("")
        getAssetsManager():checkUpdate()
    end

    local function reset(sender)
        progressLable:setString("")

        deleteDownloadDir(pathToSave)

        getAssetsManager():deleteVersion()

        createDownloadDir()
    end

    local function reloadModule( moduleName )

        package.loaded[moduleName] = nil

        return require(moduleName)
    end

    local function enter(sender)

        if not isUpdateItemClicked then
            local realPath = pathToSave .. "/package"
            addSearchPath(realPath,true)
        end

        assetsManagerModule = reloadModule("AssetsManagerTest/AssetsManagerModule")

        assetsManagerModule.newScene(AssetsManagerTestMain)
    end

    local callbackFuncs =
    {
        enter,
        reset,
        update,
    }

    local function menuCallback(tag, menuItem)
        local scene = nil
        local nIdx = menuItem:getLocalZOrder() - itemTagBasic
        local ExtensionsTestScene = CreateExtensionsTestScene(nIdx)
        if nil ~= ExtensionsTestScene then
            ax.Director:getInstance():replaceScene(ExtensionsTestScene)
        end
    end

    for i = 1, #(menuItemNames) do
        local item = ax.MenuItemFont:create(menuItemNames[i])
        item:registerScriptTapHandler(callbackFuncs[i])
        item:setPosition(canvasSize.width / 2, canvasSize.height - i * lineSpace)
        if not support then
            item:setEnabled(false)
        end
        menu:addChild(item, itemTagBasic + i)
    end

    local function onNodeEvent(msgName)
        if nil ~= assetsManager then
            assetsManager:release()
            assetsManager = nil
        end
    end

    layer:registerScriptHandler(onNodeEvent)

    layer:addChild(menu)

    return layer
end

-------------------------------------
--  AssetsManager Test
-------------------------------------
function AssetsManagerTestMain()
    local scene = ax.Scene:create()
    scene:addChild(updateLayer())
    scene:addChild(CreateBackMenuItem())
    return scene
end
