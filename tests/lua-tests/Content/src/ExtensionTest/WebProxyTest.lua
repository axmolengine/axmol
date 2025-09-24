--[[ https://websocket.org/
 list of public test servers (Note, on china mainland, may need VPN):
   - wss://ws.ifelse.io (available, test on Sep.5 2025)
   - wss://echo.websocket.org (available, test on Sep.5 2025)
   - wss://echo.websocket.events/.ws (unavailable, test on Sep.5 2025)
   - wss://ws.postman-echo.com/raw (not test)
   - wss://ws.postman-echo.com/socketio handshake path: random, handling (not test)
   - wss://socketsbay.com/wss/v2/1/demo/ (not test)
   - https://blog.postman.com/introducing-postman-websocket-echo-service/
   - other https://www.lob.com/blog/websocket-org-is-down-here-is-an-alternative
]]
local ECHO_SERVER_URL = "wss://ws.ifelse.io"

local function bin2hex(str)
    return (str:gsub('.', function(c)
        return string.format('%02X', string.byte(c))
    end))
end

local function WebSocketTestLayer()
    local layer   = ax.Layer:create()
    local canvasSize = ax.Director:getInstance():getCanvasSize()

    local MARGIN = 40
    local SPACE  = 35

    local wsSendText   = nil
    local wsSendBinary = nil
    local wsError      = nil
    local sendTextStatus  = nil
    local sendBinaryStatus = nil
    local errorStatus  = nil
    local receiveTextTimes = 0
    local receiveBinaryTimes = 0

    local label = ax.Label:createWithTTF("WebSocket Test", s_arialPath, 28)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition(ax.p( canvasSize.width / 2, canvasSize.height - MARGIN))
    layer:addChild(label, 0)

    local menuRequest = ax.Menu:create()
    menuRequest:setPosition(ax.p(0, 0))
    layer:addChild(menuRequest)

    --Send Text
    local function onMenuSendTextClicked()
        if nil ~= wsSendText then
            if ax.WEBSOCKET_STATE_OPEN == wsSendText:getReadyState() then
               sendTextStatus:setString("Send Text WS is waiting...")
               wsSendText:sendString("Hello WebSocket中文, I'm a text message.")
            else
                local warningStr = "send text websocket instance wasn't ready..."
                print(warningStr)
                sendTextStatus:setString(warningStr)
            end
        end
    end
    local labelSendText = ax.Label:createWithTTF("Send Text", s_arialPath, 22)
    labelSendText:setAnchorPoint(0.5, 0.5)
    local itemSendText  = ax.MenuItemLabel:create(labelSendText)
    itemSendText:registerScriptTapHandler(onMenuSendTextClicked)
    itemSendText:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - MARGIN - SPACE))
    menuRequest:addChild(itemSendText)

    --Send Binary
    local function onMenuSendBinaryClicked()
        if nil ~= wsSendBinary then
            if ax.WEBSOCKET_STATE_OPEN == wsSendBinary:getReadyState() then
               sendBinaryStatus:setString("Send Binary WS is waiting...")
               wsSendBinary:sendString("Hello WebSocket中文--,\0 I'm\0 a\0 binary\0 message\0.")
            else
                local warningStr = "send binary websocket instance wasn't ready..."
                sendBinaryStatus:setString(warningStr)
            end
        end
    end
    local labelSendBinary = ax.Label:createWithTTF("Send Binary", s_arialPath, 22)
    labelSendBinary:setAnchorPoint(ax.p(0.5, 0.5))
    local itemSendBinary = ax.MenuItemLabel:create(labelSendBinary)
    itemSendBinary:registerScriptTapHandler(onMenuSendBinaryClicked)
    itemSendBinary:setPosition(ax.p(canvasSize.width / 2, canvasSize.height - MARGIN - 2 * SPACE))
    menuRequest:addChild(itemSendBinary)

    --Send Text Status Label
    sendTextStatus = ax.Label:createWithTTF("Send Text WS is waiting...", s_arialPath, 14,ax.size(160, 100),ax.VERTICAL_TEXT_ALIGNMENT_CENTER,ax.VERTICAL_TEXT_ALIGNMENT_TOP)
    sendTextStatus:setAnchorPoint(ax.p(0, 0))
    sendTextStatus:setPosition(ax.p(0, 25))
    layer:addChild(sendTextStatus)

    --Send Binary Status Label
    sendBinaryStatus = ax.Label:createWithTTF("Send Binary WS is waiting...", s_arialPath, 14, ax.size(160, 100), ax.VERTICAL_TEXT_ALIGNMENT_CENTER, ax.VERTICAL_TEXT_ALIGNMENT_TOP)
    sendBinaryStatus:setAnchorPoint(ax.p(0, 0))
    sendBinaryStatus:setPosition(ax.p(160, 25))
    layer:addChild(sendBinaryStatus)

    --Error Label
    errorStatus = ax.Label:createWithTTF("Error WS is waiting...", s_arialPath, 14, ax.size(160, 100), ax.VERTICAL_TEXT_ALIGNMENT_CENTER, ax.VERTICAL_TEXT_ALIGNMENT_TOP)
    errorStatus:setAnchorPoint(ax.p(0, 0))
    errorStatus:setPosition(ax.p(320, 25))
    layer:addChild(errorStatus)

    local toMainMenu = ax.Menu:create()
    CreateExtensionsBasicLayerMenu(toMainMenu)
    toMainMenu:setPosition(ax.p(0, 0))
    layer:addChild(toMainMenu,10)

    wsSendText   = ax.WebSocket:create(ECHO_SERVER_URL, "cacert.pem")
    wsSendBinary = ax.WebSocket:create(ECHO_SERVER_URL, "cacert.pem")
    wsError      = ax.WebSocket:create("ws://invalid.url.com")

    local function wsSendTextOpen(strData)
        if sendTextStatus ~= nil then
            sendTextStatus:setString("Send Text WS was opened, url:" .. wsSendText.url .. ", protocol: " .. wsSendText.protocol)
        end
    end

    local function wsSendTextMessage(strData, isBinary)
        receiveTextTimes= receiveTextTimes + 1
        local strInfo= "response text msg: "..strData..", "..receiveTextTimes
        if sendTextStatus ~= nil then
            sendTextStatus:setString(strInfo)
        end
    end

    local function wsSendTextClose(strData)
        print("_wsiSendText websocket instance closed.")
        sendTextStatus = nil
        wsSendText = nil
    end

    local function wsSendTextError(strData)
        print("sendText Error was fired")
        if sendTextStatus ~= nil then
            sendTextStatus:setString("sendText Error was fired")
        end
    end

    local function wsSendBinaryOpen(strData)
        if sendBinaryStatus ~= nil then
            sendBinaryStatus:setString("Send Binary WS was opened, url:" .. wsSendBinary.url .. ", protocol: " .. wsSendBinary.protocol)
        end
    end

    local function wsSendBinaryMessage(reply, isBinary)
        local strInfo = nil
        if not isBinary then
            strInfo = reply
        else
            local length = #(reply)
            local i = 1
            receiveBinaryTimes = receiveBinaryTimes + 1
            strInfo = '#' .. receiveBinaryTimes .. " response bin msg: "
            strInfo = strInfo .. bin2hex(reply)
        end
        if sendBinaryStatus ~= nil then
            sendBinaryStatus:setString(strInfo)
        end
    end

    local function wsSendBinaryClose(strData)
        print("_wsiSendBinary websocket instance closed.")
        sendBinaryStatus = nil
        wsSendBinary = nil
    end

    local function wsSendBinaryError(strData)
        print("sendBinary Error was fired")
        if sendBinaryStatus ~= nil then
            sendBinaryStatus:setString("sendBinary Error was fired")
        end
    end

    local function wsErrorOpen(strData)
    end

    local function wsErrorMessage(strData)

    end

    local function wsErrorError(strData)
        print("Error was fired")
        if errorStatus ~= nil then
            errorStatus:setString("an error was fired")
        end
    end

    local function wsErrorClose(strData)
        print("_wsiError websocket instance closed.")
        errorStatus= nil
        wsError = nil
    end

    if nil ~= wsSendText then
        wsSendText:registerScriptHandler(wsSendTextOpen,ax.WEBSOCKET_OPEN)
        wsSendText:registerScriptHandler(wsSendTextMessage,ax.WEBSOCKET_MESSAGE)
        wsSendText:registerScriptHandler(wsSendTextClose,ax.WEBSOCKET_CLOSE)
        wsSendText:registerScriptHandler(wsSendTextError,ax.WEBSOCKET_ERROR)
    end

    if nil ~= wsSendBinary then
        wsSendBinary:registerScriptHandler(wsSendBinaryOpen,ax.WEBSOCKET_OPEN)
        wsSendBinary:registerScriptHandler(wsSendBinaryMessage,ax.WEBSOCKET_MESSAGE)
        wsSendBinary:registerScriptHandler(wsSendBinaryClose,ax.WEBSOCKET_CLOSE)
        wsSendBinary:registerScriptHandler(wsSendBinaryError,ax.WEBSOCKET_ERROR)
    end

    if nil ~= wsError then
        wsError:registerScriptHandler(wsErrorOpen,ax.WEBSOCKET_OPEN)
        wsError:registerScriptHandler(wsErrorMessage,ax.WEBSOCKET_MESSAGE)
        wsError:registerScriptHandler(wsErrorClose,ax.WEBSOCKET_CLOSE)
        wsError:registerScriptHandler(wsErrorError,ax.WEBSOCKET_ERROR)
    end

    local function OnExit(strEventName)
        if "exit" == strEventName then
            if nil ~= wsSendText  then
                sendTextStatus = nil
                wsSendText:close()
            end
            if nil ~= wsSendBinary then
                sendBinaryStatus = nil
                wsSendBinary:close()
            end
            if nil ~=  wsError     then
                errorStatus = nil
                wsError:close()
            end
        end
    end

    layer:registerScriptHandler(OnExit)

    return layer
end

 function runWebSocketTest()
    local scene = ax.Scene:create()
    scene:addChild(WebSocketTestLayer())
    return scene
end
