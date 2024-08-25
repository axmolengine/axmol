/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#ifndef __AX_PU_PARTICLE_3D_EVENT_HANDLER_H__
#define __AX_PU_PARTICLE_3D_EVENT_HANDLER_H__

#include "base/Object.h"
#include "math/Math.h"
#include <vector>
#include <string>
#include "extensions/ExtensionExport.h"

namespace ax
{

struct PUParticle3D;
class PUObserver;
class PUParticleSystem3D;

class AX_EX_DLL PUEventHandler : public Object
{
public:
    /** Todo
     */
    std::string_view getName() const { return _name; };
    void setName(std::string_view name) { _name = name; };

    /** Todo
     */
    PUObserver* getParentObserver() const { return _parentObserver; };
    void setParentObserver(PUObserver* parentObserver) { _parentObserver = parentObserver; };

    /** Todo
     */
    std::string_view getEventHandlerType() const { return _eventHandlerType; };
    void setEventHandlerType(std::string_view eventHandlerType) { _eventHandlerType = eventHandlerType; };

    /** Notify that the event handler is rescaled.
     */
    virtual void notifyRescaled(const Vec3& scale) { _eventHandlerScale = scale; };

    /** Todo
     */
    virtual void handle(PUParticleSystem3D* particleSystem, PUParticle3D* particle, float timeElapsed) = 0;

    virtual void copyAttributesTo(PUEventHandler* eventHandler);

    PUEventHandler();
    virtual ~PUEventHandler();

protected:
    // Observer to which the eventhandler is associated.
    PUObserver* _parentObserver;

    // Type of event handler
    std::string _eventHandlerType;

    // Name of the eventHandler (optional)
    std::string _name;

    /** Although the scale is on a Particle System level, the event handler can also be scaled.
     */
    Vec3 _eventHandlerScale;
};

}

#endif
