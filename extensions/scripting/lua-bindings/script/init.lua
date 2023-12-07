--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

-- < axmol-1.0
cc = ax
cc.Director.setDisplayStats = ax.Director.setStatsDisplay
cc.Sprite3D = ax.MeshRenderer
ccexp = axexp
ccui = axui
ccb = axb

require "axmol.core.Axmol"
require "axmol.core.Constants"
require "axmol.core.functions"

__G__TRACKBACK__ = function(msg)
    local msg = debug.traceback(msg, 3)
    print(msg)
    return msg
end

-- cocosstudio
if nil ~= ccs then
    require "axmol.cocostudio.CocoStudio"
end
-- ui
if nil ~= ccui then
    require "axmol.ui.GuiConstants"
end

-- extensions
require "axmol.extension.ExtensionConstants"
-- network
require "axmol.network.NetworkConstants"
-- Spine
if nil ~= sp then
    require "axmol.spine.SpineConstants"
end

-- Lua extensions
require "axmol.core.bitExtend"

-- physics3d
require "axmol.physics3d.physics3d-constants"

if AX_USE_FRAMEWORK then
    require "axmol.framework.init"
end
