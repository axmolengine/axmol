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

local Node = ax.Node
local nodeUpdateKey = "axlua.node.update"
local nativeUnscheduleUpdate = Node.unscheduleUpdate

function Node:add(child, zorder, tag)
    if tag then
        self:addChild(child, zorder, tag)
    elseif zorder then
        self:addChild(child, zorder)
    else
        self:addChild(child)
    end
    return self
end

function Node:addTo(parent, zorder, tag)
    if tag then
        parent:addChild(self, zorder, tag)
    elseif zorder then
        parent:addChild(self, zorder)
    else
        parent:addChild(self)
    end
    return self
end

function Node:removeSelf()
    self:removeFromParent()
    return self
end

function Node:align(anchorPoint, x, y)
    self:setAnchorPoint(anchorPoint)
    return self:move(x, y)
end

function Node:show()
    self:setVisible(true)
    return self
end

function Node:hide()
    self:setVisible(false)
    return self
end

function Node:move(x, y)
    if y then
        self:setPosition(x, y)
    else
        self:setPosition(x)
    end
    return self
end

function Node:moveTo(args)
    transition.moveTo(self, args)
    return self
end

function Node:moveBy(args)
    transition.moveBy(self, args)
    return self
end

function Node:fadeIn(args)
    transition.fadeIn(self, args)
    return self
end

function Node:fadeOut(args)
    transition.fadeOut(self, args)
    return self
end

function Node:fadeTo(args)
    transition.fadeTo(self, args)
    return self
end

function Node:rotate(rotation)
    self:setRotation(rotation)
    return self
end

function Node:rotateTo(args)
    transition.rotateTo(self, args)
    return self
end

function Node:rotateBy(args)
    transition.rotateBy(self, args)
    return self
end

function Node:scaleTo(args)
    transition.scaleTo(self, args)
    return self
end

function Node:onUpdate(callback)
    self:schedule(callback, 0, nodeUpdateKey)
    return self
end

Node.scheduleUpdate = Node.onUpdate

function Node:unscheduleUpdate()
    self:unschedule(nodeUpdateKey)
    nativeUnscheduleUpdate(self)
end

-- Compatibility-shaped lifecycle bridge for migrated Lua code.  It keeps the
-- old callback's event names but stores the callable directly in Node's native
-- std::function hooks instead of allocating a ScriptHandler id.
function Node:setLifecycleCallback(callback)
    if callback == nil then
        self:setOnEnterCallback(nil)
        self:setOnExitCallback(nil)
        self:setOnEnterTransitionDidFinishCallback(nil)
        self:setOnExitTransitionDidStartCallback(nil)
        self:setOnCleanupCallback(nil)
        return self
    end

    self:setOnEnterCallback(function() callback("enter") end)
    self:setOnExitCallback(function() callback("exit") end)
    self:setOnEnterTransitionDidFinishCallback(function() callback("enterTransitionFinish") end)
    self:setOnExitTransitionDidStartCallback(function() callback("exitTransitionStart") end)
    self:setOnCleanupCallback(function() callback("cleanup") end)
    return self
end

function Node:onNodeEvent(eventName, callback)
    if "enter" == eventName then
        self.onEnterCallback_ = callback
    elseif "exit" == eventName then
        self.onExitCallback_ = callback
    elseif "enterTransitionFinish" == eventName then
        self.onEnterTransitionFinishCallback_ = callback
    elseif "exitTransitionStart" == eventName then
        self.onExitTransitionStartCallback_ = callback
    elseif "cleanup" == eventName then
        self.onCleanupCallback_ = callback
    end
    self:enableNodeEvents()
end

function Node:enableNodeEvents()
    if self.isNodeEventEnabled_ then
        return self
    end

    self:setOnEnterCallback(function() self:onEnter_() end)
    self:setOnExitCallback(function() self:onExit_() end)
    self:setOnEnterTransitionDidFinishCallback(function() self:onEnterTransitionFinish_() end)
    self:setOnExitTransitionDidStartCallback(function() self:onExitTransitionStart_() end)
    self:setOnCleanupCallback(function() self:onCleanup_() end)
    self.isNodeEventEnabled_ = true

    return self
end

function Node:disableNodeEvents()
    self:setOnEnterCallback(nil)
    self:setOnExitCallback(nil)
    self:setOnEnterTransitionDidFinishCallback(nil)
    self:setOnExitTransitionDidStartCallback(nil)
    self:setOnCleanupCallback(nil)
    self.isNodeEventEnabled_ = false
    return self
end


function Node:onEnter()
end

function Node:onExit()
end

function Node:onEnterTransitionFinish()
end

function Node:onExitTransitionStart()
end

function Node:onCleanup()
end

function Node:onEnter_()
    self:onEnter()
    if not self.onEnterCallback_ then
        return
    end
    self:onEnterCallback_()
end

function Node:onExit_()
    self:onExit()
    if not self.onExitCallback_ then
        return
    end
    self:onExitCallback_()
end

function Node:onEnterTransitionFinish_()
    self:onEnterTransitionFinish()
    if not self.onEnterTransitionFinishCallback_ then
        return
    end
    self:onEnterTransitionFinishCallback_()
end

function Node:onExitTransitionStart_()
    self:onExitTransitionStart()
    if not self.onExitTransitionStartCallback_ then
        return
    end
    self:onExitTransitionStartCallback_()
end

function Node:onCleanup_()
    self:onCleanup()
    if not self.onCleanupCallback_ then
        return
    end
    self:onCleanupCallback_()
end
