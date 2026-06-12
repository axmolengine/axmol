/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "sceneext/sceneext.h"

namespace ax
{
class EventDispatcher;
class EventListener;
}  // namespace ax

namespace ax::ext
{

class TriggerObj;

class SCNEXT_API ArmatureMovementDispatcher : public ax::Object
{
public:
    ArmatureMovementDispatcher(void);
    ~ArmatureMovementDispatcher(void);

public:
    void addAnimationEventCallBack(ax::Object* pTarget, SEL_MovementEventCallFunc mecf);
    void removeAnnimationEventCallBack(ax::Object* pTarget, SEL_MovementEventCallFunc mecf);
    void animationEvent(Armature* armature, MovementEventType movementType, std::string_view movementID);

private:
    std::unordered_map<ax::Object*, SEL_MovementEventCallFunc>* _mapEventAnimation;
};

class SCNEXT_API TriggerMng
{
public:
    TriggerMng(void);
    virtual ~TriggerMng(void);

public:
    static TriggerMng* getInstance();
    static void destroyInstance();
    static const char* triggerMngVersion();

public:
    void parse(const rapidjson::Value& root);
    void parse(::ax::ext::CocoLoader* pCocoLoader, ::ax::ext::stExpCocoNode* pCocoNode);
    void removeAll(void);
    ax::Vector<TriggerObj*>* get(unsigned int event) const;
    TriggerObj* getTriggerObj(unsigned int id) const;
    bool removeTriggerObj(TriggerObj* Obj);
    bool removeTriggerObj(unsigned int id);
    bool isEmpty(void) const;

    void addArmatureMovementCallBack(Armature* pAr, ax::Object* pTarget, SEL_MovementEventCallFunc mecf);
    void removeArmatureMovementCallBack(Armature* pAr, ax::Object* pTarget, SEL_MovementEventCallFunc mecf);
    void removeArmatureAllMovementCallBack(Armature* pAr);
    void removeAllArmatureMovementCallBack();
    void dispatchEvent(ax::CustomEvent* tEvent);
    void removeEventListener(ax::EventListener* listener);
    void addEventListenerWithFixedPriority(ax::EventListener* listener, int fixedPriority);

private:
    void buildJson(rapidjson::Document& document,
                   ::ax::ext::CocoLoader* pCocoLoader,
                   ::ax::ext::stExpCocoNode* pCocoNode);

private:
    static TriggerMng* _sharedTriggerMng;
    std::unordered_map<unsigned int, TriggerObj*> _triggerObjs;
    std::unordered_map<Armature*, ArmatureMovementDispatcher*>* _movementDispatches;
    ax::EventDispatcher* _eventDispatcher;  ///< event dispatcher used to dispatch all kinds of events
};

}  // namespace ax::ext
