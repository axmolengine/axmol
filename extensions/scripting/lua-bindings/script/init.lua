--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

SPDX-License-Identifier: MIT

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

-- Axmol v3 exposes the ax namespace directly.
ax.Director.setDisplayStats = ax.Director.setStatsDisplay
ax.Sprite3D = ax.MeshRenderer

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
if axui ~= nil then
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
