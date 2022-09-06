--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.
Copyright (c) 2022 Bytedance Inc.

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

-- >= lua-5.4
if (math.pow == nil) then
    math.pow = function (x,y)
        return x ^ y
    end
end

-- < lua-5.4
if (unpack) then
    table.unpack = unpack
end

-- < axis-1.0
cc = ax
cc.Director.setDisplayStats = ax.Director.setStatsDisplay
cc.Sprite3D = ax.MeshRenderer
ccexp = axexp
ccui = axui
ccb = axb

require "axys.cocos2d.Cocos2d"
require "axys.cocos2d.Cocos2dConstants"
require "axys.cocos2d.functions"

__G__TRACKBACK__ = function(msg)
    local msg = debug.traceback(msg, 3)
    print(msg)
    return msg
end

-- cocosstudio
if nil ~= ccs then
    require "axys.cocostudio.CocoStudio"
end
-- ui
if nil ~= ccui then
    require "axys.ui.GuiConstants"
end

-- extensions
require "axys.extension.ExtensionConstants"
-- network
require "axys.network.NetworkConstants"
-- Spine
if nil ~= sp then
    require "axys.spine.SpineConstants"
end

-- require "axys.cocos2d.deprecated"

-- Lua extensions
require "axys.cocos2d.bitExtend"

-- CCLuaEngine
-- require "axys.cocos2d.DeprecatedCocos2dClass"
-- require "axys.cocos2d.DeprecatedCocos2dEnum"
-- require "axys.cocos2d.DeprecatedCocos2dFunc"

-- register_cocostudio_module
-- if nil ~= ccs then
--     require "axys.cocostudio.DeprecatedCocoStudioClass"
--     require "axys.cocostudio.DeprecatedCocoStudioFunc"
-- end

-- register_extension_module
-- require "axys.extension.DeprecatedExtensionClass"
-- require "axys.extension.DeprecatedExtensionEnum"
-- require "axys.extension.DeprecatedExtensionFunc"

-- register_network_module
-- require "axys.network.DeprecatedNetworkClass"
-- require "axys.network.DeprecatedNetworkEnum"
-- require "axys.network.DeprecatedNetworkFunc"

-- register_ui_module
-- if nil ~= ccui then
--     require "axys.ui.DeprecatedUIEnum"
--     require "axys.ui.DeprecatedUIFunc"
-- end

-- physics3d
require "axys.physics3d.physics3d-constants"

if AX_USE_FRAMEWORK then
    require "axys.framework.init"
end
