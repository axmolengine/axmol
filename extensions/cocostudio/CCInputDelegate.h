/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

https://axis-project.github.io/

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

#ifndef __CC_EXTENTIONS_CCINPUTDELEGATE_H__
#define __CC_EXTENTIONS_CCINPUTDELEGATE_H__

#include "platform/CCPlatformMacros.h"
#include "base/CCTouch.h"
#include "base/CCEvent.h"
#include "base/ccTypes.h"
#include "base/CCEventKeyboard.h"
#include "base/CCEventListener.h"
#include "CocosStudioExport.h"

NS_AX_BEGIN
class __Set;
NS_AX_END

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
    virtual void setTouchMode(axis::Touch::DispatchMode mode);
    virtual axis::Touch::DispatchMode getTouchMode() const;
    virtual void setTouchPriority(int priority);
    virtual int getTouchPriority() const;
    /** @deprecated Please override onAcceleration */
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void didAccelerate(axis::Acceleration* accelerationValue) final;
    // Deprecated touch callbacks.
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual bool ccTouchBegan(axis::Touch* touch, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchMoved(axis::Touch* touch, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchEnded(axis::Touch* touch, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchCancelled(axis::Touch* touch, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchesBegan(axis::__Set* touches, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchesMoved(axis::__Set* touches, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchesEnded(axis::__Set* touches, axis::Event* event) final;
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE virtual void ccTouchesCancelled(axis::__Set* touches, axis::Event* event) final;
    /**
     * @js NA
     */
    virtual void onAcceleration(axis::Acceleration* acc, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onKeyPressed(axis::EventKeyboard::KeyCode keyCode, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onKeyReleased(axis::EventKeyboard::KeyCode keyCode, axis::Event* event);
    /**
     * @js NA
     */
    virtual bool onTouchBegan(axis::Touch* touch, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchMoved(axis::Touch* touch, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchEnded(axis::Touch* touch, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchCancelled(axis::Touch* touch, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesBegan(const std::vector<axis::Touch*>& touches, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesEnded(const std::vector<axis::Touch*>& touches, axis::Event* event);
    /**
     * @js NA
     */
    virtual void onTouchesCancelled(const std::vector<axis::Touch*>& touches, axis::Event* event);

protected:
    bool _touchEnabled;
    axis::EventListener* _touchListener;
    bool _accelerometerEnabled;
    axis::EventListener* _accelerometerListener;
    bool _keypadEnabled;
    axis::EventListener* _keyboardListener;

private:
    int _touchPriority;
    axis::Touch::DispatchMode _touchMode;
};

}  // namespace cocostudio

#endif  // __CC_EXTENTIONS_CCINPUTDELEGATE_H__
