/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __AX_EXTENTIONS_CCINPUTDELEGATE_H__
#define __AX_EXTENTIONS_CCINPUTDELEGATE_H__

#include "platform/PlatformMacros.h"
#include "base/Touch.h"
#include "base/Event.h"
#include "base/Types.h"
#include "base/EventKeyboard.h"
#include "base/EventListener.h"
#include "CocosStudioExport.h"

namespace ax
{
class __Set;
}

namespace cocostudio
{

/**
 *  @lua NA
 */
class CCS_DLL InputDelegate
{
public:
    /**
     * @js ctor
     */
    InputDelegate(void);
    /**
     * @js NA
     */
    virtual ~InputDelegate(void);
    virtual bool isTouchEnabled() const;
    virtual void setTouchEnabled(bool value);
    virtual bool isAccelerometerEnabled() const;
    virtual void setAccelerometerEnabled(bool value);
    virtual bool isKeypadEnabled() const;
    virtual void setKeypadEnabled(bool value);
    virtual void setTouchMode(ax::Touch::DispatchMode mode);
    virtual ax::Touch::DispatchMode getTouchMode() const;
    virtual void setTouchPriority(int priority);
    virtual int getTouchPriority() const;
    /**
     * @js NA
     */
    virtual void onAcceleration(ax::Acceleration* acc, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    /**
     * @js NA
     */
    virtual bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchMoved(ax::Touch* touch, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchEnded(ax::Touch* touch, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchCancelled(ax::Touch* touch, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesCancelled(const std::vector<ax::Touch*>& touches, ax::Event* event);

protected:
    bool _touchEnabled;
    ax::EventListener* _touchListener;
    bool _accelerometerEnabled;
    ax::EventListener* _accelerometerListener;
    bool _keypadEnabled;
    ax::EventListener* _keyboardListener;

private:
    int _touchPriority;
    ax::Touch::DispatchMode _touchMode;
};

}  // namespace cocostudio

#endif  // __AX_EXTENTIONS_CCINPUTDELEGATE_H__
