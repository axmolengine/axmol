local json = require 'cjson'

local function XMLHttpRequestLayer()
    local layer = ax.Layer:create()
    local canvasSize = ax.Director:getInstance():getCanvasSize()
    local margin = 40
    local space  = 35

    local function init()
        local label = ax.Label:createWithTTF("XML Http Request Test", s_arialPath, 28)
        label:setAnchorPoint(ax.p(0.5, 0.5))
        label:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - margin))
        layer:addChild(label, 0)

        --Response Code Label
        local labelStatusCode = ax.Label:createWithTTF("HTTP Status:", s_markerFeltFontPath, 20)
        labelStatusCode:setAnchorPoint(ax.p(0.5, 0.5))
        labelStatusCode:setPosition(ax.p(canvasSize.width / 2,  canvasSize.height - margin - 6 * space))
        layer:addChild(labelStatusCode)

        local menuRequest = ax.Menu:create()
        menuRequest:setPosition(ax.p(0,0))
        layer:addChild(menuRequest)

        --Get
        local function onMenuGetClicked()
            local xhr = ax.XMLHttpRequest:new()
            xhr.responseType = ax.XMLHTTPREQUEST_RESPONSE_STRING
            xhr:open("GET", "https://httpbin.org/get")

            local function onReadyStateChanged()
                if not tolua.isnull(labelStatusCode) then
                    labelStatusCode:setString("Http Status:" .. xhr.statusText)
                else
                    print("ERROR: labelStatusCode is invalid!")
                end
                if xhr.readyState == 4 and (xhr.status >= 200 and xhr.status < 207) then
                    print(xhr.response)
                else
                    print("xhr.readyState is:", xhr.readyState, "xhr.status is: ",xhr.status)
                end
                xhr:unregisterScriptHandler()
            end

            xhr:registerScriptHandler(onReadyStateChanged)
            xhr:send()

            labelStatusCode:setString("waiting...")
        end

        local labelGet  = ax.Label:createWithTTF("Test Get", s_arialPath, 22)
        labelGet:setAnchorPoint(ax.p(0.5, 0.5))
        local itemGet  =  ax.MenuItemLabel:create(labelGet)
        itemGet:registerScriptTapHandler(onMenuGetClicked)
        itemGet:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - margin - space))
        menuRequest:addChild(itemGet)

        --Post
        local function onMenuPostClicked()
            local xhr = ax.XMLHttpRequest:new()
            xhr.responseType = ax.XMLHTTPREQUEST_RESPONSE_STRING
            xhr:open("POST", "https://httpbin.org/post")
            local function onReadyStateChanged()
                if xhr.readyState == 4 and (xhr.status >= 200 and xhr.status < 207) then
                    if not tolua.isnull(labelStatusCode) then
                        labelStatusCode:setString("Http Status:" .. xhr.statusText)
                    else
                        print("ERROR: labelStatusCode is invalid!")
                    end
                    print(xhr.response)
                else
                    print("xhr.readyState is:", xhr.readyState, "xhr.status is: ",xhr.status)
                end
                xhr:unregisterScriptHandler()
            end
            xhr:registerScriptHandler(onReadyStateChanged)
            xhr:send()

            labelStatusCode:setString("waiting...")
        end

        local labelPost = ax.Label:createWithTTF("Test Post", s_arialPath, 22)
        labelPost:setAnchorPoint(ax.p(0.5, 0.5))
        local itemPost =  ax.MenuItemLabel:create(labelPost)
        itemPost:registerScriptTapHandler(onMenuPostClicked)
        itemPost:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - margin - 2 * space))
        menuRequest:addChild(itemPost)

        --Post Binary
        local function onMenuPostBinaryClicked()
            local xhr = ax.XMLHttpRequest:new()
            xhr.responseType = ax.XMLHTTPREQUEST_RESPONSE_ARRAY_BUFFER
            xhr:open("POST", "https://httpbin.org/post")

            local function onReadyStateChanged()
                if xhr.readyState == 4 and (xhr.status >= 200 and xhr.status < 207) then
                    local response   = xhr.response
                    local size     = #response
                    local strInfo = ""

                    for i = 1,size do
                        if 0 == response[i] then
                            strInfo = strInfo.."\'\\0\'"
                        else
                            strInfo = strInfo..string.char(response[i])
                        end
                    end

                    if not tolua.isnull(labelStatusCode) then
                        labelStatusCode:setString("Http Status:" .. xhr.statusText)
                    else
                        print("ERROR: labelStatusCode is invalid!")
                    end

                    print(strInfo)
                else
                    print("xhr.readyState is:", xhr.readyState, "xhr.status is: ",xhr.status)
                end
                xhr:unregisterScriptHandler()
            end

            xhr:registerScriptHandler(onReadyStateChanged)
            xhr:send()

            labelStatusCode:setString("waiting...")
        end

        local labelPostBinary = ax.Label:createWithTTF("Test Post Binary", s_arialPath, 22)
        labelPostBinary:setAnchorPoint(ax.p(0.5, 0.5))
        local itemPostBinary = ax.MenuItemLabel:create(labelPostBinary)
        itemPostBinary:registerScriptTapHandler(onMenuPostBinaryClicked)
        itemPostBinary:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - margin - 3 * space))
        menuRequest:addChild(itemPostBinary)

        --Post Json

        local function onMenuPostJsonClicked()
            local xhr = ax.XMLHttpRequest:new()
            xhr.responseType = ax.XMLHTTPREQUEST_RESPONSE_JSON
            xhr:open("POST", "https://httpbin.org/post")

            local function onReadyStateChanged()
                if xhr.readyState == 4 and (xhr.status >= 200 and xhr.status < 207) then
                    if not tolua.isnull(labelStatusCode) then
                        labelStatusCode:setString("Http Status:" .. xhr.statusText)
                    else
                        print("ERROR: labelStatusCode is invalid!")
                    end
                    local response   = xhr.response
                    local output = json.decode(response)

                    for k,v in pairs(output) do
                        print (k, v)
                    end

                    print("headers are")
                    for k,v in pairs(output.headers) do
                        print (k, v)
                    end
                else
                    print("xhr.readyState is:", xhr.readyState, "xhr.status is: ",xhr.status)
                end
                xhr:unregisterScriptHandler()
            end

            xhr:registerScriptHandler(onReadyStateChanged)
            xhr:send()

            labelStatusCode:setString("waiting...")
        end

        local labelPostJson = ax.Label:createWithTTF("Test Post Json", s_arialPath, 22)
        labelPostJson:setAnchorPoint(ax.p(0.5, 0.5))
        local itemPostJson = ax.MenuItemLabel:create(labelPostJson)
        itemPostJson:registerScriptTapHandler(onMenuPostJsonClicked)
        itemPostJson:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - margin - 4 * space))
        menuRequest:addChild(itemPostJson)
    end

    local function onNodeEvent(eventName)
        if "enter" == eventName then
            init()
        end
    end

    layer:registerScriptHandler(onNodeEvent)

    return layer
end

function XMLHttpRequestTestMain()
    local scene = ax.Scene:create()
    scene:addChild(XMLHttpRequestLayer())
    scene:addChild(CreateBackMenuItem())
    return scene
end
