/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2018 DragonBones team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DRAGONBONES_CC_SLOT_H
#define DRAGONBONES_CC_SLOT_H

#include "DragonBonesHeaders.h"
#include "cocos2d.h"

DRAGONBONES_NAMESPACE_BEGIN
/**
 * The Cocos2d slot.
 * @version DragonBones 3.0
 * @language en_US
 */
class CCSlot : public Slot
{
    BIND_CLASS_TYPE_A(CCSlot);

private:
    float _textureScale;
    cocos2d::Node* _renderDisplay;

protected:
    virtual void _onClear() override;
    virtual void _initDisplay(void* value, bool isRetain) override;
    virtual void _disposeDisplay(void* value, bool isRelease) override;
    virtual void _onUpdateDisplay() override;
    virtual void _addDisplay() override;
    virtual void _replaceDisplay(void* value, bool isArmatureDisplay) override;
    virtual void _removeDisplay() override;
    virtual void _updateZOrder() override;

public:
    virtual void _updateVisible() override;
    virtual void _updateBlendMode() override;
    virtual void _updateColor() override;

protected:
    virtual void _updateFrame() override;
    virtual void _updateMesh() override;
    virtual void _updateTransform() override;
    virtual void _identityTransform() override;

public:
    /**
     * The display object that the slot displays at this time.
     * @see #display
     * @version DragonBones 3.0
     * @language en_US
     */
    inline cocos2d::Node* getCCDisplay() const
    {
        return static_cast<cocos2d::Node*>(_display);
    }
    inline void setCCDisplay(cocos2d::Node* value)
    {
        setDisplay(value, DisplayType::Image);
    }
};

DRAGONBONES_NAMESPACE_END
#endif // DRAGONBONES_CC_SLOT_H
