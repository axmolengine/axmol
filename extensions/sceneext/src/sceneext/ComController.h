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

#include "sceneext/ComBase.h"
#include "sceneext/SceneExtMacros.h"
#include "axmol/scene/Component.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/Types.h"
#include "axmol/base/KeyboardEvent.h"
#include "axmol/base/AccelerationEvent.h"
#include "axmol/base/EventListener.h"

namespace ax::ext
{

class SCNEXT_API ComController : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO
public:
    /**
     */
    ComController();

public:
    const static std::string COMPONENT_NAME;

    static ComController* create();

    /**
     * @lua NA
     */
    virtual ~ComController();

    bool init() override;

    /**
     * @lua NA
     */
    void onEnter() override;

    /**
     * @lua NA
     */
    void onExit() override;

    /**
     * @lua NA
     */
    void onAdd() override;

    /**
     * @lua NA
     */
    void onRemove() override;
    void update(float delta) override;

    ///////////////////////////////////////////////
    // Events
    //////////////////////////////////////////////
    virtual bool isPointerEnabled() const;
    virtual void setPointerEnabled(bool value);
    virtual bool isAccelerometerEnabled() const;
    virtual void setAccelerometerEnabled(bool value);
    virtual bool isKeypadEnabled() const;
    virtual void setKeypadEnabled(bool value);
    virtual void setPointerPriority(int priority);
    virtual int getPointerPriority() const;

protected:
    /**
     */
    virtual void onAcceleration(ax::AccelerationEvent* event);
    /**
     */
    virtual void onKeyPressed(ax::KeyboardEvent* event);
    /**
     */
    virtual void onKeyReleased(ax::KeyboardEvent* event);
    /**
     */
    virtual bool onPointerDown(ax::PointerEvent* event);
    /**
     */
    virtual void onPointerMove(ax::PointerEvent* event);
    /**
     */
    virtual void onPointerUp(ax::PointerEvent* event);
    /**
     */
    virtual void onPointerCancel(ax::PointerEvent* event);

    bool _pointerEnabled{false};
    ax::EventListener* _pointerListener{nullptr};
    bool _accelerometerEnabled{false};
    ax::EventListener* _accelerometerListener{nullptr};
    bool _keypadEnabled{false};
    ax::EventListener* _keyboardListener{nullptr};

private:
    int _pointerPriority{-1};
};

}  // namespace ax::ext
