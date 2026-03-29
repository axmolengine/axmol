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
#include "sceneext/TriggerObj.h"
#include "sceneext/TriggerMng.h"
#include "sceneext/SceneExtMacros.h"

#include "axmol/base/ObjectFactory.h"

#define DECLARE_CLASS_INFO                \
public:                                   \
    static ax::ObjectFactory::TInfo Type; \
    static ax::Object* createInstance(void);

#define IMPLEMENT_CLASS_INFO(className)         \
    ax::Object* className::createInstance(void) \
    {                                           \
        auto ret = new className;               \
        ret->autorelease();                     \
        return ret;                             \
    }                                           \
    ax::ObjectFactory::TInfo className::Type(#className, &className::createInstance);

void SCNEXT_API sendEvent(unsigned int event);
