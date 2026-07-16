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
