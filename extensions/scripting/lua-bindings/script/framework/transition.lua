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

local unpack = table.unpack
local transition = {}

local ACTION_EASING = {}
ACTION_EASING["BACKIN"]           = {ax.EaseBackIn, 1}
ACTION_EASING["BACKINOUT"]        = {ax.EaseBackInOut, 1}
ACTION_EASING["BACKOUT"]          = {ax.EaseBackOut, 1}
ACTION_EASING["BOUNCE"]           = {ax.EaseBounce, 1}
ACTION_EASING["BOUNCEIN"]         = {ax.EaseBounceIn, 1}
ACTION_EASING["BOUNCEINOUT"]      = {ax.EaseBounceInOut, 1}
ACTION_EASING["BOUNCEOUT"]        = {ax.EaseBounceOut, 1}
ACTION_EASING["ELASTIC"]          = {ax.EaseElastic, 2, 0.3}
ACTION_EASING["ELASTICIN"]        = {ax.EaseElasticIn, 2, 0.3}
ACTION_EASING["ELASTICINOUT"]     = {ax.EaseElasticInOut, 2, 0.3}
ACTION_EASING["ELASTICOUT"]       = {ax.EaseElasticOut, 2, 0.3}
ACTION_EASING["EXPONENTIALIN"]    = {ax.EaseExponentialIn, 1}
ACTION_EASING["EXPONENTIALINOUT"] = {ax.EaseExponentialInOut, 1}
ACTION_EASING["EXPONENTIALOUT"]   = {ax.EaseExponentialOut, 1}
ACTION_EASING["IN"]               = {ax.EaseIn, 2, 1}
ACTION_EASING["INOUT"]            = {ax.EaseInOut, 2, 1}
ACTION_EASING["OUT"]              = {ax.EaseOut, 2, 1}
ACTION_EASING["RATEACTION"]       = {ax.EaseRateAction, 2, 1}
ACTION_EASING["SINEIN"]           = {ax.EaseSineIn, 1}
ACTION_EASING["SINEINOUT"]        = {ax.EaseSineInOut, 1}
ACTION_EASING["SINEOUT"]          = {ax.EaseSineOut, 1}

local actionManager = ax.Director:getInstance():getActionManager()

function transition.newEasing(action, easingName, more)
    local key = string.upper(tostring(easingName))
    local easing
    if ACTION_EASING[key] then
        local cls, count, default = unpack(ACTION_EASING[key])
        if count == 2 then
            easing = cls:create(action, more or default)
        else
            easing = cls:create(action)
        end
    end
    return easing or action
end

function transition.create(action, args)
    args = checktable(args)
    if args.easing then
        if type(args.easing) == "table" then
            action = transition.newEasing(action, unpack(args.easing))
        else
            action = transition.newEasing(action, args.easing)
        end
    end

    local actions = {}
    local delay = checknumber(args.delay)
    if delay > 0 then
        actions[#actions + 1] = ax.DelayTime:create(delay)
    end
    actions[#actions + 1] = action

    local onComplete = args.onComplete
    if type(onComplete) ~= "function" then onComplete = nil end
    if onComplete then
        actions[#actions + 1] = ax.CallFunc:create(onComplete)
    end

    if args.removeSelf then
        actions[#actions + 1] = ax.RemoveSelf:create()
    end

    if #actions > 1 then
        return transition.sequence(actions)
    else
        return actions[1]
    end
end

function transition.execute(target, action, args)
    assert(not tolua.isnull(target), "transition.execute() - target is not ax.Node")
    local action = transition.create(action, args)
    target:runAction(action)
    return action
end

function transition.moveTo(target, args)
    assert(not tolua.isnull(target), "transition.moveTo() - target is not ax.Node")
    local x = args.x or target:getPositionX()
    local y = args.y or target:getPositionY()
    local action = ax.MoveTo:create(args.time, ax.p(x, y))
    return transition.execute(target, action, args)
end

function transition.moveBy(target, args)
    assert(not tolua.isnull(target), "transition.moveBy() - target is not ax.Node")
    local x = args.x or 0
    local y = args.y or 0
    local action = ax.MoveBy:create(args.time, ax.p(x, y))
    return transition.execute(target, action, args)
end

function transition.fadeIn(target, args)
    assert(not tolua.isnull(target), "transition.fadeIn() - target is not ax.Node")
    local action = ax.FadeIn:create(args.time)
    return transition.execute(target, action, args)
end

function transition.fadeOut(target, args)
    assert(not tolua.isnull(target), "transition.fadeOut() - target is not ax.Node")
    local action = ax.FadeOut:create(args.time)
    return transition.execute(target, action, args)
end

function transition.fadeTo(target, args)
    assert(not tolua.isnull(target), "transition.fadeTo() - target is not ax.Node")
    local opacity = checkint(args.opacity)
    if opacity < 0 then
        opacity = 0
    elseif opacity > 255 then
        opacity = 255
    end
    local action = ax.FadeTo:create(args.time, opacity)
    return transition.execute(target, action, args)
end

function transition.scaleTo(target, args)
    assert(not tolua.isnull(target), "transition.scaleTo() - target is not ax.Node")
    local action
    if args.scale then
        action = ax.ScaleTo:create(checknumber(args.time), checknumber(args.scale))
    elseif args.scaleX or args.scaleY then
        local scaleX, scaleY
        if args.scaleX then
            scaleX = checknumber(args.scaleX)
        else
            scaleX = target:getScaleX()
        end
        if args.scaleY then
            scaleY = checknumber(args.scaleY)
        else
            scaleY = target:getScaleY()
        end
        action = ax.ScaleTo:create(checknumber(args.time), scaleX, scaleY)
    end
    return transition.execute(target, action, args)
end

function transition.rotateTo(target, args)
    assert(not tolua.isnull(target), "transition.rotateTo() - target is not ax.Node")
    local rotation = args.rotation or target:getRotation()
    local action = ax.RotateTo:create(args.time, rotation)
    return transition.execute(target, action, args)
end

function transition.rotateBy(target, args)
    assert(not tolua.isnull(target), "transition.rotateTo() - target is not ax.Node")
    local rotation = args.rotation or 0
    local action = ax.RotateBy:create(args.time, rotation)
    return transition.execute(target, action, args)
end

function transition.sequence(actions)
    if #actions < 1 then return end
    if #actions < 2 then return actions[1] end
    return ax.Sequence:create(actions)
end

function transition.removeAction(action)
    if not tolua.isnull(action) then
        actionManager:removeAction(action)
    end
end

function transition.stopTarget(target)
    if not tolua.isnull(target) then
        actionManager:removeAllActionsFromTarget(target)
    end
end

function transition.pauseTarget(target)
    if not tolua.isnull(target) then
        actionManager:pauseTarget(target)
    end
end

function transition.resumeTarget(target)
    if not tolua.isnull(target) then
        actionManager:resumeTarget(target)
    end
end

return transition
