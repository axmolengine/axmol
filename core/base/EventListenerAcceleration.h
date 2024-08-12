/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#ifndef _AX_ACCELEROMETERLISTENER_H_
#define _AX_ACCELEROMETERLISTENER_H_

#include "base/EventListener.h"
#include "base/Types.h"

/**
 * @addtogroup base
 * @{
 */

NS_AX_BEGIN

/** @class EventListenerAcceleration
 * @brief Acceleration event listener.
 * @js NA
 */
class AX_DLL EventListenerAcceleration : public EventListener
{
public:
    static const std::string LISTENER_ID;

    /** Create a acceleration EventListener.
     *
     * @param callback The acceleration callback method.
     * @return An autoreleased EventListenerAcceleration object.
     */
    static EventListenerAcceleration* create(const std::function<void(Acceleration*, Event*)>& callback);

    /** Destructor.
     */
    virtual ~EventListenerAcceleration();

    /// Overrides
    virtual EventListenerAcceleration* clone() override;
    virtual bool checkAvailable() override;

    EventListenerAcceleration();

    bool init(const std::function<void(Acceleration*, Event* event)>& callback);

private:
    std::function<void(Acceleration*, Event*)> onAccelerationEvent;

    friend class LuaEventListenerAcceleration;
};

NS_AX_END

// end of base group
/// @}

#endif /* defined(_AX_ACCELEROMETERLISTENER_H_) */
