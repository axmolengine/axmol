local    kTagNode = 0
local    kTagGrossini = 1
local    kTagSequence = 2
local    scheduler = ax.Director:getInstance():getScheduler()
--------------------------------------------------------------------
--
-- Test1
--
--------------------------------------------------------------------

local function CrashTest()
    local ret = createTestLayer("Test 1. Should not crash")

    local  child = ax.Sprite:create(s_pPathGrossini)
    child:setPosition( 200,200 )
    ret:addChild(child, 1)

    --Sum of all action's duration is 1.5 second.
    child:runAction(ax.RotateBy:create(1.5, 90))
    child:runAction(ax.Sequence:create(ax.DelayTime:create(1.4),ax.FadeOut:create(1.1)))

    local function removeThis()
        ret:getParent():removeChild(ret, true)
        Helper.nextAction()
    end

    --After 1.5 second, self will be removed.
    ret:runAction( ax.Sequence:create(ax.DelayTime:create(1.4),ax.CallFunc:create(removeThis)))
    return ret
end


--------------------------------------------------------------------
--
-- LogicTest
--
--------------------------------------------------------------------
local function LogicTest()
    local ret = createTestLayer("Logic test")
    local  grossini = ax.Sprite:create(s_pPathGrossini)
    ret:addChild(grossini, 0, 2)
    grossini:setPosition(200,200)
    local function bugMe(node)
        node:stopAllActions() --After this stop next action not working, if remove this stop everything is working
        node:runAction(ax.ScaleTo:create(2, 2))
    end
    grossini:runAction( ax.Sequence:create(ax.MoveBy:create(1, ax.p(150,0)) ,ax.CallFunc:create(bugMe)))
    return ret
end

--------------------------------------------------------------------
--
-- PauseTest
--
--------------------------------------------------------------------

local function PauseTest()
    local ret = createTestLayer("Pause Test")
    local schedulerEntry = nil
    local function unpause(dt)
        scheduler:unscheduleScriptEntry(schedulerEntry)
        schedulerEntry = nil
        local  node = ret:getChildByTag( kTagGrossini )
        local  pDirector = ax.Director:getInstance()
        pDirector:getActionManager():resumeTarget(node)
    end

    local function onNodeEvent(event)
        if event == "enter" then
            local  s = ax.Director:getInstance():getCanvasSize()
            local  l = ax.Label:createWithTTF("After 3 seconds grossini should move", "fonts/Thonburi.ttf", 16)
            ret:addChild(l)
            l:setAnchorPoint(ax.p(0.5, 0.5))
            l:setPosition( ax.p(s.width / 2, 245) )

            local  grossini = ax.Sprite:create(s_pPathGrossini)
            ret:addChild(grossini, 0, kTagGrossini)
            grossini:setPosition(ax.p(200,200))

            local  action = ax.MoveBy:create(1, ax.p(150,0))

            local  pDirector = ax.Director:getInstance()
            pDirector:getActionManager():addAction(action, grossini, true)

            schedulerEntry = scheduler:scheduleScriptFunc(unpause, 3.0, false)

        elseif event == "exit" then
            if schedulerEntry ~= nil then
                scheduler:unscheduleScriptEntry(schedulerEntry)
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)
    return ret
end

--------------------------------------------------------------------
--
-- RemoveTest
--
--------------------------------------------------------------------
local function RemoveTest()
    local ret = createTestLayer("Remove Test")
    local  l = ax.Label:createWithTTF("Should not crash", "fonts/Thonburi.ttf", 16)
    local  s = ax.Director:getInstance():getCanvasSize()
    ret:addChild(l)
    l:setAnchorPoint(ax.p(0.5, 0.5))
    l:setPosition( ax.p(s.width / 2, 245))

    local  pMove = ax.MoveBy:create(2, ax.p(200, 0))
    local function stopAction()
        local  pSprite = ret:getChildByTag(kTagGrossini)
        pSprite:stopActionByTag(kTagSequence)
    end

    local callfunc = ax.CallFunc:create(stopAction)
    local  pSequence = ax.Sequence:create(pMove,callfunc)
    pSequence:setTag(kTagSequence)

    local  pChild = ax.Sprite:create(s_pPathGrossini)
    pChild:setPosition( 200, 200 )

    ret:addChild(pChild, 1, kTagGrossini)
    pChild:runAction(pSequence)
    return ret
end


--------------------------------------------------------------------
--
-- ResumeTest
--
--------------------------------------------------------------------
local function ResumeTest()
    local ret = createTestLayer("Resume Test")

    local schedulerEntry = nil
    local function resumeGrossini(time)
        scheduler:unscheduleScriptEntry(schedulerEntry)
        schedulerEntry = nil
        local  pGrossini = ret:getChildByTag(kTagGrossini)
        local  pDirector = ax.Director:getInstance()
        pDirector:getActionManager():resumeTarget(pGrossini)
    end


    local function onNodeEvent(event)
        if event == "enter" then
            local  l = ax.Label:createWithTTF("Grossini only rotate/scale in 3 seconds", "fonts/Thonburi.ttf", 16)
            ret:addChild(l)
            local  s = ax.Director:getInstance():getCanvasSize()
            l:setAnchorPoint(ax.p(0.5, 0.5))
            l:setPosition( s.width / 2, 245)

            local  pGrossini = ax.Sprite:create(s_pPathGrossini)
            ret:addChild(pGrossini, 0, kTagGrossini)
            pGrossini:setPosition(200,200)

            pGrossini:runAction(ax.ScaleBy:create(2, 2))

            local  pDirector = ax.Director:getInstance()
            pDirector:getActionManager():pauseTarget(pGrossini)
            pGrossini:runAction(ax.RotateBy:create(2, 360))

            schedulerEntry = scheduler:scheduleScriptFunc(resumeGrossini, 3.0, false)
        elseif event == "exit" then
            if schedulerEntry ~= nil then
                scheduler:unscheduleScriptEntry(schedulerEntry)
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end


function ActionManagerTestMain()
    cclog("ActionManagerTestMain")
    Helper.index = 1
    ax.Director:getInstance():getRenderer():setDepthTest(true)
    local scene = ax.Scene:create()

    Helper.createFunctionTable = {
        CrashTest,
        LogicTest,
        PauseTest,
        RemoveTest,
        ResumeTest
    }
    Helper.index = 1

    scene:addChild(CrashTest())
    scene:addChild(CreateBackMenuItem())
    return scene
end

