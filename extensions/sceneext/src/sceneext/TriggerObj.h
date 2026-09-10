/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/sceneext.h"
#include "axmol/base/Vector.h"
#include "axmol/base/CustomEventListener.h"

namespace ax::ext
{

class SCNEXT_API BaseTriggerCondition : public ax::Object
{
protected:
    BaseTriggerCondition(void);

public:
    virtual ~BaseTriggerCondition(void);
    virtual bool init();
    virtual bool detect();
    virtual void serialize(const rapidjson::Value& val);
    virtual void serialize(::ax::ext::CocoLoader* cocoLoader, ::ax::ext::stExpCocoNode* cocoNode);
    virtual void removeAll();
};

class SCNEXT_API BaseTriggerAction : public ax::Object
{
protected:
    BaseTriggerAction(void);

public:
    virtual ~BaseTriggerAction(void);
    virtual bool init();
    virtual void done();
    virtual void serialize(const rapidjson::Value& val);
    virtual void serialize(::ax::ext::CocoLoader* cocoLoader, ::ax::ext::stExpCocoNode* cocoNode);
    virtual void removeAll();
};

class SCNEXT_API TriggerObj : public ax::Object
{
public:
    TriggerObj(void);
    virtual ~TriggerObj(void);
    virtual bool init();
    static TriggerObj* create(void);

    virtual bool detect();
    virtual void done();
    virtual void removeAll();
    virtual void serialize(const rapidjson::Value& val);
    virtual void serialize(::ax::ext::CocoLoader* cocoLoader, ::ax::ext::stExpCocoNode* cocoNode);
    unsigned int getId();
    void setEnabled(bool enabled);

private:
    ax::Vector<BaseTriggerCondition*> _cons;
    ax::Vector<BaseTriggerAction*> _acts;
    unsigned int _id;
    bool _enabled;
    ax::Vector<ax::CustomEventListener*> _listeners;
};

}  // namespace ax::ext
