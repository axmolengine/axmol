
if(version() >= 504) then
    collectgarbage("incremental")
else
    -- avoid memory leak
    collectgarbage("setpause", 100)
    collectgarbage("setstepmul", 5000)
end

----------------
-- run
AX_USE_DEPRECATED_API = true
require "axmol.init"

local director = ax.Director:getInstance()
local renderView   = director:getRenderView()
if nil == renderView then
    renderView = ax.RenderViewImpl:createWithRect("Lua Tests", ax.rect(0,0,960,640), 1.0, true)
    director:setRenderView(renderView)
end

--turn on display FPS
director:setDisplayStats(true)

--set FPS. the default value is 1.0/60 if you don't call this
director:setAnimationInterval(1.0 / 60)

local screenSize = renderView:getFrameSize()

local designSize = {width = 480, height = 320}

if screenSize.height > 320 then
    local resourceSize = {width = 960, height = 640}
    ax.Director:getInstance():setContentScaleFactor(resourceSize.height/designSize.height)
end

renderView:setDesignResolutionSize(designSize.width, designSize.height, ax.ResolutionPolicy.SHOW_ALL)

local fileUtils = ax.FileUtils:getInstance()
local function addSearchPath(resPrefix, height)
    local searchPaths = fileUtils:getSearchPaths()
    table.insert(searchPaths, 1, resPrefix)
    table.insert(searchPaths, 1, resPrefix .. "cocosbuilderRes")

    if screenSize.height > 320 then
        table.insert(searchPaths, 1, resPrefix .. "hd")
        table.insert(searchPaths, 1, resPrefix .. "ccs-res")
        table.insert(searchPaths, 1, resPrefix .. "ccs-res/hd")
    end

    fileUtils:setSearchPaths(searchPaths)

end

addSearchPath("res/", screenSize.height)
addSearchPath("", screenSize.height)

require "mainMenu"

local scene = ax.Scene:create()
scene:addChild(CreateTestMenu())
if ax.Director:getInstance():getRunningScene() then
    ax.Director:getInstance():replaceScene(scene)
else
    ax.Director:getInstance():runWithScene(scene)
end
