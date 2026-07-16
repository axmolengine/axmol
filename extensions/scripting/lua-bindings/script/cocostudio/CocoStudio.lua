if nil == ccs then
    return
end

local json = require 'cjson'

require "axmol.cocostudio.StudioConstants"

function ax.sendTriggerEvent(event)
    local triggerObjArr = ax.TriggerMng.getInstance():get(event)

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

function ax.registerTriggerClass(className, createFunc)
    ax.TInfo.new(className,createFunc)
end

ax.TInfo = class("TInfo")
ax.TInfo._className = ""
ax.TInfo._fun = nil

function ax.TInfo:ctor(c,f)
     -- @param {String|ax.TInfo}c
     -- @param {Function}f
    if nil ~= f then
        self._className = c
        self._fun       = f
    else
        self._className = c._className
        self._fun       = c._fun
    end

    ax.ObjectFactory.getInstance():registerType(self)
end

ax.ObjectFactory = class("ObjectFactory")
ax.ObjectFactory._typeMap = nil
ax.ObjectFactory._instance = nil

function ax.ObjectFactory:ctor()
    self._typeMap = {}
end

function ax.ObjectFactory.getInstance()
    if nil == ax.ObjectFactory._instance then
        ax.ObjectFactory._instance = ax.ObjectFactory.new()
    end

    return ax.ObjectFactory._instance
end

function ax.ObjectFactory.destroyInstance()
    ax.ObjectFactory._instance = nil
end

function ax.ObjectFactory:createObject(classname)
    local obj = nil
    local t   = self._typeMap[classname]
    if nil ~= t then
        obj = t._fun()
    end

    return obj
end

function ax.ObjectFactory:registerType(t)
    self._typeMap[t._className] = t
end

ax.TriggerObj = class("TriggerObj")
ax.TriggerObj._cons = {}
ax.TriggerObj._acts = {}
ax.TriggerObj._enable = false
ax.TriggerObj._id   = 0
ax.TriggerObj._vInt = {}

function ax.TriggerObj.extend(target)
    local t = tolua.getpeer(target)
    if not t then
        t = {}
        tolua.setpeer(target, t)
    end
    setmetatable(t, ax.TriggerObj)
    return target
end

function ax.TriggerObj:ctor()
    self:init()
end

function ax.TriggerObj:init()
    self._id = 0
    self._enable = true
    self._cons = {}
    self._acts = {}
    self._vInt = {}
end

function ax.TriggerObj:detect()
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

function ax.TriggerObj:done()
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

function ax.TriggerObj:removeAll()
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

function ax.TriggerObj:serialize(jsonValue)
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
                local obj = ax.ObjectFactory.getInstance():createObject(className)
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
                local act = ax.ObjectFactory.getInstance():createObject(className)
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

function ax.TriggerObj:getId()
    return self._id
end

function ax.TriggerObj:setEnable(enable)
    self._enable = enable
end

function ax.TriggerObj:getEvents()
    return self._vInt
end

ax.TriggerMng = class("TriggerMng")
ax.TriggerMng._eventTriggers = nil
ax.TriggerMng._triggerObjs = nil
ax.TriggerMng._movementDispatches =  nil
ax.TriggerMng._instance  = nil

function ax.TriggerMng:ctor()
    self._triggerObjs = {}
    self._movementDispatches = {}
    self._eventTriggers = {}
end

function ax.TriggerMng.getInstance()
    if ax.TriggerMng._instance == nil then
        ax.TriggerMng._instance = ax.TriggerMng.new()
    end

    return ax.TriggerMng._instance
end

function ax.TriggerMng.destroyInstance()
    if ax.TriggerMng._instance ~= nil then
        ax.TriggerMng._instance:removeAll()
        ax.TriggerMng._instance = nil
    end
end

function ax.TriggerMng:triggerMngVersion()
    return "1.0.0.0"
end

function ax.TriggerMng:parse(jsonStr)
    local parseTable = json.decode(jsonStr,1)
    if nil == parseTable then
        return
    end

    local count = #parseTable
    for i = 1, count do
        local subDict = parseTable[i]
        local triggerObj = ax.TriggerObj.new()
        triggerObj:serialize(subDict)
        local events = triggerObj:getEvents()
        for j = 1, #events do
            local event = events[j]
            self:add(event, triggerObj)
        end

        self._triggerObjs[triggerObj:getId()] =  triggerObj
    end
end

function ax.TriggerMng:get(event)
    return self._eventTriggers[event]
end

function ax.TriggerMng:getTriggerObj(id)
    return self._triggerObjs[id]
end

function ax.TriggerMng:add(event,triggerObj)
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

function ax.TriggerMng:removeAll( )
    for k in pairs(self._eventTriggers) do
        local triObjArr = self._eventTriggers[k]
        for j = 1, #triObjArr do
            local  obj = triObjArr[j]
            obj:removeAll()
        end
    end
    self._eventTriggers = {}
end

function ax.TriggerMng:remove(event, obj)

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

function ax.TriggerMng:removeObjByEvent(event, obj)
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

function ax.TriggerMng:removeTriggerObj(id)
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

function ax.TriggerMng:isEmpty()
    return (self._eventTriggers ~= nil) or #self._eventTriggers <= 0
end

function __onParseConfig(configType,jasonStr)
    if configType == ax.ConfigType.COCOSTUDIO then
        ax.TriggerMng.getInstance():parse(jasonStr)
    end
end

function ax.AnimationInfo(_name, _startIndex, _endIndex)
    assert(nil ~= _name and type(_name) == "string" and _startIndex ~= nil and type(_startIndex) == "number" and _endIndex ~= nil and  type(_endIndex) == "number", "ax.AnimationInfo() - invalid input parameters")
    return { name = _name, startIndex = _startIndex,  endIndex = _endIndex}
end
