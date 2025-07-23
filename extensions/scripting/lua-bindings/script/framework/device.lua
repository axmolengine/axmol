--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

]]

local device = {}

device.platform    = "unknown"
device.model       = "unknown"

local app = ax.Application:getInstance()
local target = app:getTargetPlatform()
if target == ax.PLATFORM_WINDOWS then
    device.platform = "windows"
elseif target == ax.PLATFORM_UWP then
    device.platform = "uwp"
elseif target == ax.PLATFORM_UWP_PHONE then
    device.platform = "uwp phone"
elseif target == ax.PLATFORM_MAC then
    device.platform = "mac"
elseif target == ax.PLATFORM_ANDROID then
    device.platform = "android"
elseif target == ax.PLATFORM_IOS then
    device.platform = "ios"
    local director = ax.Director:getInstance()
    local view = director:getRenderView()
    local framesize = view:getFrameSize()
    local w, h = framesize.width, framesize.height
    if w == 640 and h == 960 then
        device.model = "iphone 4"
    elseif w == 640 and h == 1136 then
        device.model = "iphone 5"
    elseif w == 750 and h == 1334 then
        device.model = "iphone 6"
    elseif w == 1242 and h == 2208 then
        device.model = "iphone 6 plus"
    elseif w == 768 and h == 1024 then
        device.model = "ipad"
    elseif w == 1536 and h == 2048 then
        device.model = "ipad retina"
    end
end

local language_ = app:getCurrentLanguage()
if language_ == ax.LANGUAGE_CHINESE then
    language_ = "cn"
elseif language_ == ax.LANGUAGE_FRENCH then
    language_ = "fr"
elseif language_ == ax.LANGUAGE_ITALIAN then
    language_ = "it"
elseif language_ == ax.LANGUAGE_GERMAN then
    language_ = "gr"
elseif language_ == ax.LANGUAGE_SPANISH then
    language_ = "sp"
elseif language_ == ax.LANGUAGE_RUSSIAN then
    language_ = "ru"
elseif language_ == ax.LANGUAGE_KOREAN then
    language_ = "kr"
elseif language_ == ax.LANGUAGE_JAPANESE then
    language_ = "jp"
elseif language_ == ax.LANGUAGE_HUNGARIAN then
    language_ = "hu"
elseif language_ == ax.LANGUAGE_PORTUGUESE then
    language_ = "pt"
elseif language_ == ax.LANGUAGE_ARABIC then
    language_ = "ar"
else
    language_ = "en"
end

device.language = language_
device.writablePath = ax.FileUtils:getInstance():getWritablePath()
device.directorySeparator = "/"
device.pathSeparator = ":"
if device.platform == "windows" then
    device.directorySeparator = "\\"
    device.pathSeparator = ";"
end

printInfo("# device.platform              = " .. device.platform)
printInfo("# device.model                 = " .. device.model)
printInfo("# device.language              = " .. device.language)
printInfo("# device.writablePath          = " .. device.writablePath)
printInfo("# device.directorySeparator    = " .. device.directorySeparator)
printInfo("# device.pathSeparator         = " .. device.pathSeparator)
printInfo("#")

return device
