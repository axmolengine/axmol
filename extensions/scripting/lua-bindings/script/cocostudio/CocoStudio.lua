if nil == ccs then
    return
end

local json = require 'cjson'

require "axmol.cocostudio.StudioConstants"

function axext.sendTriggerEvent(event)
    local triggerObjArr = axext.TriggerMng.getInstance():get(event)

    if nil == triggerObjArr then
        return
    end

    for i = 1, #triggerObjArr do
        local triObj = triggerObjArr[i]
        if nil ~= triObj and triObj:detect() then
            triObj:done()
        end
    end
end

function axext.registerTriggerClass(className, createFunc)
    axext.TInfo.new(className,createFunc)
end

axext.TInfo = class("TInfo")
axext.TInfo._className = ""
axext.TInfo._fun = nil

function axext.TInfo:ctor(c,f)
     -- @param {String|axext.TInfo}c
     -- @param {Function}f
    if nil ~= f then
        self._className = c
        self._fun       = f
    else
        self._className = c._className
        self._fun       = c._fun
    end

    axext.ObjectFactory.getInstance():registerType(self)
end

axext.ObjectFactory = class("ObjectFactory")
axext.ObjectFactory._typeMap = nil
axext.ObjectFactory._instance = nil

function axext.ObjectFactory:ctor()
    self._typeMap = {}
end

function axext.ObjectFactory.getInstance()
    if nil == axext.ObjectFactory._instance then
        axext.ObjectFactory._instance = axext.ObjectFactory.new()
    end

    return axext.ObjectFactory._instance
end

function axext.ObjectFactory.destroyInstance()
    axext.ObjectFactory._instance = nil
end

function axext.ObjectFactory:createObject(classname)
    local obj = nil
    local t   = self._typeMap[classname]
    if nil ~= t then
        obj = t._fun()
    end

    return obj
end

function axext.ObjectFactory:registerType(t)
    self._typeMap[t._className] = t
end

axext.TriggerObj = class("TriggerObj")
axext.TriggerObj._cons = {}
axext.TriggerObj._acts = {}
axext.TriggerObj._enable = false
axext.TriggerObj._id   = 0
axext.TriggerObj._vInt = {}

function axext.TriggerObj.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, axext.TriggerObj)
    return target
end

function axext.TriggerObj:ctor()
    self:init()
end

function axext.TriggerObj:init()
    self._id = 0
    self._enable = true
    self._cons = {}
    self._acts = {}
    self._vInt = {}
end

function axext.TriggerObj:detect()
    if (not self._enable) or (#self._cons == 0) then
        return true
    end

    local ret = true
    local obj = nil
    for i = 1 , #self._cons do
        obj = self._cons[i]
        if nil ~= obj and nil ~= obj.detect then
            ret = ret and obj:detect()
        end
    end
    return ret
end

function axext.TriggerObj:done()
    if (not self._enable) or (#self._acts == 0) then
        return
    end

    local obj = nil
    for i = 1, #self._acts do
        obj = self._acts[i]
        if nil ~= obj and obj.done then
            obj:done()
        end
    end
end

function axext.TriggerObj:removeAll()
    local obj = nil
    for i=1, #self._cons do
        obj = self._cons[i]
        if nil ~= obj then
            obj:removeAll()
        end
    end
    self._cons = {}

    for i=1, #self._acts do
        obj = self._acts[i]
        if nil ~= obj then
            obj:removeAll()
        end
    end
    self._acts = {}
end

function axext.TriggerObj:serialize(jsonValue)
    self._id = jsonValue["id"]
    local count = 0

    --condition
    local cons = jsonValue["conditions"]
    if nil ~= cons then
        count = #cons
        for i = 1, count do
            local subDict = cons[i]
            local className = subDict["classname"]
            if nil ~= className then
                local obj = axext.ObjectFactory.getInstance():createObject(className)
                assert(nil ~= obj, string.format("class named %s can not implement!",className))
                obj:serialize(subDict)
                obj:init()
                table.insert(self._cons, obj)
            end
        end
    end

    local actions =  jsonValue["actions"]
    if nil ~= actions then
        count = #actions
        for i = 1,count do
            local  subAction = actions[i]
            local  className = subAction["classname"]
            if nil ~= className then
                local act = axext.ObjectFactory.getInstance():createObject(className)
                assert(nil ~= act ,string.format("class named %s can not implement!",className))
                act:serialize(subAction)
                act:init()
                table.insert(self._acts,act)
            end
        end
    end

    local events = jsonValue["events"]
    if nil ~= events then
        count = #events
        for i = 1, count do
            local subEveent = events[i]
            local eventID   = subEveent["id"]
            if eventID >= 0 then
                table.insert(self._vInt,eventID)
            end
        end
    end
end

function axext.TriggerObj:getId()
    return self._id
end

function axext.TriggerObj:setEnable(enable)
    self._enable = enable
end

function axext.TriggerObj:getEvents()
    return self._vInt
end

axext.TriggerMng = class("TriggerMng")
axext.TriggerMng._eventTriggers = nil
axext.TriggerMng._triggerObjs = nil
axext.TriggerMng._movementDispatches =  nil
axext.TriggerMng._instance  = nil

function axext.TriggerMng:ctor()
    self._triggerObjs = {}
    self._movementDispatches = {}
    self._eventTriggers = {}
end

function axext.TriggerMng.getInstance()
    if axext.TriggerMng._instance == nil then
        axext.TriggerMng._instance = axext.TriggerMng.new()
    end

    return axext.TriggerMng._instance
end

function axext.TriggerMng.destroyInstance()
    if axext.TriggerMng._instance ~= nil then
        axext.TriggerMng._instance:removeAll()
        axext.TriggerMng._instance = nil
    end
end

function axext.TriggerMng:triggerMngVersion()
    return "1.0.0.0"
end

function axext.TriggerMng:parse(jsonStr)
    local parseTable = json.decode(jsonStr,1)
    if nil == parseTable then
        return
    end

    local count = #parseTable
    for i = 1, count do
        local subDict = parseTable[i]
        local triggerObj = axext.TriggerObj.new()
        triggerObj:serialize(subDict)
        local events = triggerObj:getEvents()
        for j = 1, #events do
            local event = events[j]
            self:add(event, triggerObj)
        end

        self._triggerObjs[triggerObj:getId()] =  triggerObj
    end
end

function axext.TriggerMng:get(event)
    return self._eventTriggers[event]
end

function axext.TriggerMng:getTriggerObj(id)
    return self._triggerObjs[id]
end

function axext.TriggerMng:add(event,triggerObj)
    local eventTriggers = self._eventTriggers[event]
    if nil == eventTriggers then
        eventTriggers = {}
    end

    local exist = false
    for i = 1, #eventTriggers do
        if eventTriggers[i] == triggerObj then
            exist = true
            break
        end
    end

    if not exist then
        table.insert(eventTriggers,triggerObj)
        self._eventTriggers[event] = eventTriggers
    end
end

function axext.TriggerMng:removeAll( )
    for k in pairs(self._eventTriggers) do
        local triObjArr = self._eventTriggers[k]
        for j = 1, #triObjArr do
            local  obj = triObjArr[j]
            obj:removeAll()
        end
    end
    self._eventTriggers = {}
end

function axext.TriggerMng:remove(event, obj)

    if nil ~= obj then
        return self:removeObjByEvent(event, obj)
    end

    assert(event >= 0,"event must be larger than 0")
    if nil == self._eventTriggers then
        return false
    end

    local triObjects =  self._eventTriggers[event]
    if nil == triObjects then
        return false
    end

    for i = 1, #triObjects do
        local triObject = triObjects[i]
        if nil ~= triObject then
            triObject:remvoeAll()
        end
    end

    self._eventTriggers[event] = nil
    return true
end

function axext.TriggerMng:removeObjByEvent(event, obj)
    assert(event >= 0,"event must be larger than 0")
    if nil == self._eventTriggers then
        return false
    end

    local triObjects = self._eventTriggers[event]
    if nil == triObjects then
        return false
    end

    for i = 1,#triObjects do
        local triObject = triObjects[i]
        if  nil ~= triObject and triObject == obj then
            triObject:remvoeAll()
            table.remove(triObjects, i)
            return true
        end
    end
end

function axext.TriggerMng:removeTriggerObj(id)
    local obj = self.getTriggerObj(id)

    if nil == obj then
        return false
    end

    local events = obj:getEvents()
    for i = 1, #events do
        self:remove(events[i],obj)
    end

    return true
end

function axext.TriggerMng:isEmpty()
    return (self._eventTriggers ~= nil) or #self._eventTriggers <= 0
end

function __onParseConfig(configType,jasonStr)
    if configType == ax.ConfigType.COCOSTUDIO then
        axext.TriggerMng.getInstance():parse(jasonStr)
    end
end

function axext.AnimationInfo(_name, _startIndex, _endIndex)
    assert(nil ~= _name and type(_name) == "string" and _startIndex ~= nil and type(_startIndex) == "number" and _endIndex ~= nil and  type(_endIndex) == "number", "axext.AnimationInfo() - invalid input parameters")
    return { name = _name, startIndex = _startIndex,  endIndex = _endIndex}
end
