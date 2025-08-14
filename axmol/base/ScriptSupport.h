/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

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
 ****************************************************************************/

#pragma once

#include "base/Config.h"
#include "platform/Common.h"
#include "base/Touch.h"
#include "base/EventTouch.h"
#include "base/EventKeyboard.h"
#include <map>
#include <string>
#include <list>

/**
 * @addtogroup base
 * @{
 */

#if AX_ENABLE_SCRIPT_BINDING

typedef struct lua_State lua_State;

namespace ax
{

class TimerScriptHandler;
class Layer;
class MenuItem;
class CallFunc;
class Acceleration;
class Action;

enum ccScriptType
{
    kScriptTypeNone = 0,
    kScriptTypeLua,
};

/**
 * This classes is wrapped to store the handler corresponding to the Lua function pointer and assign the handler a
 * unique id
 */
class ScriptHandlerEntry : public Object
{
public:
    /**
     * create a ScriptHandlerEntry instance by the handler.
     *
     * @param handler corresponding to the Lua function pointer.
     * @return ScriptHandlerEntry instance.
     * @lua NA
     */
    static ScriptHandlerEntry* create(int handler);

    /**
     * Destructor of ScriptHandlerEntry.
     * @lua NA
     */
    virtual ~ScriptHandlerEntry();

    /**
     * Get the handler corresponding to the Lua function pointer.
     *
     * @return the handler corresponding to the Lua function pointer.
     * @lua NA
     */
    int getHandler() { return _handler; }

    /**
     * Get the unique id corresponding to the handler.
     *
     * @return the unique id corresponding to the handler.
     * @lua NA
     */
    int getEntryId() { return _entryId; }

protected:
    ScriptHandlerEntry(int handler) : _handler(handler)
    {
        static int newEntryId = 0;
        newEntryId++;
        _entryId = newEntryId;
    }

    int _handler;
    int _entryId;
};

/**
 * The SchedulerScriptHandlerEntry is used to store the handler corresponding to the Lua function pointer and assign the
 * handler a unique id like ScriptHandlerEntry. Meanwhile,create a timer that named TimerScriptHandler to execute the
 * Lua function corresponding to the handler in the interval time if the SchedulerScriptHandlerEntry object isn't
 * paused.
 */
class SchedulerScriptHandlerEntry : public ScriptHandlerEntry
{
public:
    /**
     * create a SchedulerScriptHandlerEntry object.
     *
     * @param handler the index corresponding to the Lua function pointer.
     * @param interval the interval to execute the Lua function. If the value is 0, then the lua function will be
     * scheduled every frame.
     * @param paused if paused is true, then the timer won't be started until it is resumed.
     * @return a SchedulerScriptHandlerEntry object.
     * @lua NA
     */
    static SchedulerScriptHandlerEntry* create(int handler, float interval, bool paused);

    /**
     * Destructor of SchedulerScriptHandlerEntry.
     * @lua NA
     */
    virtual ~SchedulerScriptHandlerEntry();

    /**
     * Get the pointer of TimerScriptHandler object.
     *
     * @return the pointer of TimerScriptHandler object.
     * @lua NA
     */
    TimerScriptHandler* getTimer() { return _timer; }

    /**
     * Get the flag whether paused or not.
     *
     * @return the flag whether paused or not.
     * @lua NA
     */
    bool isPaused() { return _paused; }
    /**
     * Set the markedForDeletion flag true.
     * @lua NA
     */
    void markedForDeletion() { _markedForDeletion = true; }
    /**
     * Get the flag whether markedForDeletion or not.
     *
     * @return the flag whether markedForDeletion or not.
     * @lua NA
     */
    bool isMarkedForDeletion() { return _markedForDeletion; }

private:
    SchedulerScriptHandlerEntry(int handler)
        : ScriptHandlerEntry(handler), _timer(nullptr), _paused(false), _markedForDeletion(false)
    {}
    bool init(float interval, bool paused);

    TimerScriptHandler* _timer;
    bool _paused;
    bool _markedForDeletion;
};

/**
 * @cond
 */
class TouchScriptHandlerEntry : public ScriptHandlerEntry
{
public:
    static TouchScriptHandlerEntry* create(int handler, bool isMultiTouches, int priority, bool swallowsTouches);

    virtual ~TouchScriptHandlerEntry();

    bool isMultiTouches() { return _isMultiTouches; }

    int getPriority() { return _priority; }

    bool getSwallowsTouches() { return _swallowsTouches; }

private:
    TouchScriptHandlerEntry(int handler)
        : ScriptHandlerEntry(handler), _isMultiTouches(false), _priority(0), _swallowsTouches(false)
    {}
    bool init(bool isMultiTouches, int priority, bool swallowsTouches);

    bool _isMultiTouches;
    int _priority;
    bool _swallowsTouches;
};

/**
 * @endcond
 */

/** ScriptEventType enum*/
enum ScriptEventType
{
    kNodeEvent = 0,
    kMenuClickedEvent,
    kCallFuncEvent,
    kScheduleEvent,
    kTouchEvent,
    kTouchesEvent,
    kKeypadEvent,
    kAccelerometerEvent,
    kControlEvent,
    kCommonEvent,
    kComponentEvent,
    kRestartGame,
    kScriptActionEvent
};

/**
 * For Lua, Wrapper the script data that should be used to find the handler corresponding to the Lua function by the
 * nativeobject pointer and store the value pointer which would be converted concretely by the different events,then the
 * converted data would be passed into the Lua stack.
 */
struct BasicScriptData
{
    /**
     * For Lua, nativeobject is used to get handler corresponding to the Lua function.
     *
     * @lua NA
     */
    void* nativeObject;

    /**
     * A pointer point to the value data which would be converted by different events.
     *
     * @lua NA
     */
    void* value;

    /**
     * Constructor of BasicScriptData.
     *
     * @lua NA
     */
    BasicScriptData(void* inObject, void* inValue = nullptr) : nativeObject(inObject), value(inValue) {}
};

/**
 * For Lua, Wrapper the script data that should be used to find the handler corresponding to the Lua function by the
 * nativeobject pointer and store the value pointer which would be converted concretely by the different events,then the
 * converted data would be passed into the Lua stack.
 */
struct ActionObjectScriptData
{
    /**
     * For Lua, nativeobject is used to get handler corresponding to the Lua function.
     *
     * @lua NA
     */
    void* nativeObject;

    /**
     * A pointer point to the value data which event action
     *
     * @lua NA
     */
    int* eventType;

    /**
     * A pointer point to the value data which would be converted by different events.
     *
     * @lua NA
     */
    void* param;

    /**
     * Constructor of BasicScriptData.
     *
     * @lua NA
     */
    ActionObjectScriptData(void* inObject, int* inValue = nullptr, void* inParam = nullptr)
        : nativeObject(inObject), eventType(inValue), param(inParam)
    {}
};

/**
 * For Lua, the SchedulerScriptData is used to find the Lua function pointer by the handler, then call the Lua function
 * by push the elapse into the Lua stack as a parameter when scheduler update event is triggered.
 */
struct SchedulerScriptData
{
    /**
     * the handler corresponding to the Lua function pointer, only use in the Lua.
     *
     * @lua NA
     */
    int handler;
    /**
     * the parameter would be passed in to the Lua function, only use in the Lua.
     *
     * @lua NA
     */
    float elapse;
    // js use
    void* node;

    /**
     * Constructor of SchedulerScriptData.
     *
     * @lua NA
     */
    SchedulerScriptData(int inHandler, float inElapse, void* inNode = nullptr)
        : handler(inHandler), elapse(inElapse), node(inNode)
    {}
};

/**
 * For Lua, the TouchesScriptData is used to find the Lua function pointer by the nativeObject, then call the Lua
 * function by push touches data and actionType into the Lua stack as the parameters when the touches event is
 * triggered.
 */
struct TouchesScriptData
{
    /**
     * The EventTouch::EventCode type.
     *
     * @lua NA
     */
    EventTouch::EventCode actionType;
    /**
     * For Lua, it Used to find the Lua function pointer by the ScriptHandlerMgr.
     *
     * @lua NA
     */
    void* nativeObject;
    /**
     * The vector of Touch.For Lua, it would be convert to the Lua table form to be pushed into the Lua stack.
     *
     * @lua NA
     */
    const std::vector<Touch*>& touches;
    /**
     * event information, it is useless for Lua.
     *
     * @lua NA
     */
    Event* event;

    /**
     * Constructor of TouchesScriptData.
     *
     * @lua NA
     */
    TouchesScriptData(EventTouch::EventCode inActionType,
                      void* inNativeObject,
                      const std::vector<Touch*>& inTouches,
                      Event* evt)
        : actionType(inActionType), nativeObject(inNativeObject), touches(inTouches), event(evt)
    {}
};

/**
 * For Lua, the TouchScriptData is used to find the Lua function pointer by the nativeObject, then call the Lua function
 * by push touch data and actionType converted to string type into the Lua stack as the parameters when the touch event
 * is triggered.
 */
struct TouchScriptData
{
    /**
     * The EventTouch::EventCode type.
     *
     * @lua NA
     */
    EventTouch::EventCode actionType;
    /**
     * For Lua, it Used to find the Lua function pointer by the ScriptHandlerMgr.
     *
     * @lua NA
     */
    void* nativeObject;
    /**
     * touch information. it would be in x,y form to push into the Lua stack.
     *
     * @lua NA
     */
    Touch* touch;
    /**
     * event information,it is useless for Lua.
     *
     * @lua NA
     */
    Event* event;

    /**
     * Constructor of TouchScriptData.
     *
     * @lua NA
     */
    TouchScriptData(EventTouch::EventCode inActionType, void* inNativeObject, Touch* inTouch, Event* evt)
        : actionType(inActionType), nativeObject(inNativeObject), touch(inTouch), event(evt)
    {}
};

/**
 * For Lua, the KeypadScriptData is used to find the Lua function pointer by the nativeObject, then call the Lua
 * function by push the actionType converted to string type into the Lua stack as the parameters when the Keypad event
 * is triggered.
 */
struct KeypadScriptData
{
    /**
     * The specific type of EventKeyboard::KeyCode
     *
     * @lua NA
     */
    EventKeyboard::KeyCode actionType;
    /**
     * For Lua, it Used to find the Lua function pointer by the ScriptHandlerMgr.
     *
     * @lua NA
     */
    void* nativeObject;

    /**
     * Constructor of KeypadScriptData.
     *
     * @lua NA
     */
    KeypadScriptData(EventKeyboard::KeyCode inActionType, void* inNativeObject)
        : actionType(inActionType), nativeObject(inNativeObject)
    {}
};

/**
 * For Lua, the CommonScriptData is used to find the Lua function pointer by the handler, then call the Lua function by
 * push the eventName, eventSource(if it not nullptr), eventSourceClassName(if it is nullptr or "", and the eventSource
 * is not nullptr,would give the default string "cc.Object") into the Lua stack as the parameter when the common event such
 * as is triggered.
 */
struct CommonScriptData
{
    /**
     * The index to find the corresponding to the Lua function pointer.
     *
     * @lua NA
     */
    int handler;
    /**
     * The string value to be pushed into the Lua stack.
     *
     * @lua NA
     */
    char eventName[64];
    /**
     * The source object trigger the event,could be nullptr.
     *
     * @lua NA
     */
    Object* eventSource;
    /**
     * The class name of source object trigger the event, could be nullptr.
     *
     * @lua NA
     */
    char eventSourceClassName[64];

    /**
     * Constructor of  CommonScriptData.
     *
     * @lua NA
     */
    CommonScriptData(int inHandler, const char* inName, Object* inSource = nullptr, const char* inClassName = nullptr)
        : handler(inHandler), eventSource(inSource)
    {
        if (nullptr == inName)
        {
            memset(eventName, 0, sizeof(eventName));
        }
        else
        {
            strncpy(eventName, inName, sizeof(eventName));
        }

        if (nullptr == inClassName)
        {
            memset(eventSourceClassName, 0, sizeof(eventSourceClassName));
        }
        else
        {
            strncpy(eventSourceClassName, inClassName, 64);
        }
    }
};

/**
 * The ScriptEvent wrapper the different script data corresponding to the ScriptEventType in to the unified struct.
 * when the corresponding event is triggered, we could call the `sendEvent` of ScriptEngineProtocol to handle the event.
 */
struct ScriptEvent
{
    /**
     * The specific type of ScriptEventType.
     *
     * @lua NA
     */
    ScriptEventType type;
    /**
     * Pointer point to the different data.
     *
     * @lua NA
     */
    void* data;

    /**
     * Constructor of ScriptEvent.
     *
     * @lua NA
     */
    ScriptEvent(ScriptEventType inType, void* inData) : type(inType), data(inData) {}
};

/**
 * Don't make ScriptEngineProtocol inherits from Object since setScriptEngine is invoked only once in AppDelegate.cpp,
 * It will affect the lifecycle of ScriptEngine instance, the autorelease pool will be destroyed before destructing
 * ScriptEngine. So a crash will appear on Win32 if you click the close button.
 */
class AX_DLL ScriptEngineProtocol
{
public:
    /**
     * Constructor of ScriptEngineProtocol.
     *
     * @lua NA
     */
    ScriptEngineProtocol() {}

    /**
     * Destructor of ScriptEngineProtocol.
     *
     * @lua NA
     */
    virtual ~ScriptEngineProtocol() {}

    /**
     * Get the specific script type.
     *
     * @return the specific script type.
     *
     * @lua NA
     */
    virtual ccScriptType getScriptType() { return kScriptTypeNone; }

    /**
     * Reflect the retain relationship to script scope
     */
    virtual void retainScriptObject(Object* /*owner*/, Object* /*target*/) {}

    /**
     * Add the script object to root object
     */
    virtual void rootScriptObject(Object* /*target*/) {}

    /**
     * Reflect the release relationship to script scope
     */
    virtual void releaseScriptObject(Object* /*owner*/, Object* /*target*/) {}

    /**
     * Remove the script object from root object
     */
    virtual void unrootScriptObject(Object* /*target*/) {}

    /**
     * Release all children native refs for the given node in script scope
     */
    virtual void releaseAllChildrenRecursive(Node* /*node*/) {}

    /**
     * Release all native refs for the given owner in script scope
     */
    virtual void releaseAllNativeRefs(ax::Object* /*owner*/) {}

    /**
     * Remove script object,The specific meaning should refer to the ScriptType.
     * For Lua, @see removeScriptObjectByObject of LuaEngine.
     *
     * @lua NA
     */
    virtual void removeScriptObjectByObject(Object* /*obj*/) {}

    /**
     * Remove script function handler, only LuaEngine class need to implement this function.
     * @see removeScriptHandler of LuaEngine.
     * @lua NA
     */
    virtual void removeScriptHandler(int /*handler*/) {}

    /**
     * Reallocate script function handler, only LuaEngine class need to implement this function.
     * @see reallocateScriptHandler of LuaEngine.
     * @lua NA
     */
    virtual int reallocateScriptHandler(int /*handler*/) { return 0; }

    /**
     * Execute script code contained in the given string.
     *
     * @param codes holding the valid script code that should be executed.
     * @return 0 if the string is executed correctly.
     * @return other if the string is executed wrongly.
     * @lua NA
     */
    virtual int executeString(const char* codes) = 0;

    /**
     * Execute a script file.
     *
     * @param filename String object holding the filename of the script file that is to be executed.
     * @return 0 if it happen the error or it hasn't return value, otherwise it return the value by calling the lua
     * function.
     * @lua NA
     */
    virtual int executeScriptFile(const char* filename) = 0;

    /**
     * Execute a scripted global function.
     * The function should not take any parameters and should return an integer.
     *
     * @param functionName String object holding the name of the function, in the global script environment, that is to
     * be executed.
     * @return The integer value returned from the script function.
     * @lua NA
     */
    virtual int executeGlobalFunction(const char* functionName) = 0;

    /**
     * When trigger a script event ,call this func,add params needed into ScriptEvent object.nativeObject is object
     * triggering the event, can be nullptr in Lua.
     *
     *
     * @lua NA
     */
    virtual int sendEvent(const ScriptEvent& evt) = 0;

    /**
     * Handle the assert message.
     *
     * @return true if the assert was handled by the script engine, false otherwise.
     *
     * @lua NA
     */
    virtual bool handleAssert(const char* msg) = 0;

    /**
     * Useless for Lua.
     *
     * @lua NA
     */
    virtual void setCalledFromScript(bool /*callFromScript*/) {}

    /**
     * Useless for Lua.
     *
     * @lua NA
     */
    virtual bool isCalledFromScript() { return false; };

    /** ConfigType enum. */
    enum class ConfigType
    {
        NONE,
        COCOSTUDIO
    };
    /**
     * Parse configuration file.
     *
     * @param type the specific type value.
     * @param str the information data.
     *
     * @lua NA
     */
    virtual bool parseConfig(ConfigType type, std::string_view str) = 0;

    /** Root a Reference.
     It tells the Garbage Collector that the associated Scripting object should not be collected
     */
    virtual void rootObject(Object* /*obj*/) {}

    /** Unroot a Reference.
     It tells the Garbage Collector that the associated Scripting object can be collected
     */
    virtual void unrootObject(Object* /*obj*/) {}

    /** Remove proxy for a native object
     */
    virtual void removeObjectProxy(Object* obj) {}

    /** Triggers the garbage collector */
    virtual void garbageCollect() {}
};

class Node;
/**
 * ScriptEngineManager is a singleton which manager an object instance of ScriptEngineProtocol, such as LuaEngine.
 *
 * @since v0.99.5-x-0.8.5
 */
class AX_DLL ScriptEngineManager
{
public:
    /**
     * Constructor of ScriptEngineManager.
     *
     * @lua NA
     */
    ~ScriptEngineManager();
    /**
     * Get the ScriptEngineProtocol object.
     *
     * @return the ScriptEngineProtocol object.
     *
     * @lua NA
     */
    ScriptEngineProtocol* getScriptEngine() { return _scriptEngine; }
    /**
     * Set the ScriptEngineProtocol object should be managed.
     *
     * @param scriptEngine should be managed.
     *
     * @lua NA
     */
    void setScriptEngine(ScriptEngineProtocol* scriptEngine);

    /**
     * Remove the ScriptEngineProtocol object managed.
     *
     *
     * @lua NA
     */
    void removeScriptEngine();
    /**
     * Get the instance of ScriptEngineManager object.
     *
     * @return the instance of ScriptEngineManager object.
     *
     * @lua NA
     */
    static ScriptEngineManager* getInstance();
    /**
     * Destroy the singleton about ScriptEngineManager.
     *
     * @lua NA
     */
    static void destroyInstance();

    /**
     * Call the Lua function when the event of node is triggered.
     *
     * @param node the nativeobject triggers the event.
     * @param action the specific type.
     *
     * @lua NA
     */
    static void sendNodeEventToLua(Node* node, int action);

    /**
     * Send a event to lua script
     *
     * @param event: the script event
     *
     * @lua NA
     */
    static int sendEventToLua(const ScriptEvent& event);

private:
    ScriptEngineManager() : _scriptEngine(nullptr) {}

    ScriptEngineProtocol* _scriptEngine;
};

}

#endif  // #if AX_ENABLE_SCRIPT_BINDING

// end group
/// @}

