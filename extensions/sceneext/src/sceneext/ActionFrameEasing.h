/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/base/Object.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

enum FrameEasingType
{
    kframeEasingInstant,

    kframeEasingLinear,

    kframeEasingCubicIn,
    kframeEasingCubicOut,
    kframeEasingCubicInOut,

    kframeEasingElasticIn,
    kframeEasingElasticOut,
    kframeEasingElasticInOut,

    kframeEasingBounceIn,
    kframeEasingBounceOut,
    kframeEasingBounceInOut,

    kframeEasingBackIn,
    kframeEasingBackOut,
    kframeEasingBackInOut,
};

/**
 *  @lua NA
 */
class SCNEXT_API ActionFrameEasing : public ax::Object
{
protected:
    FrameEasingType _type;
    float _fValue;

public:
    ActionFrameEasing();
    virtual ~ActionFrameEasing();

    float bounceTime(float t);

    float easeValue(float t);
};

}  // namespace ax::ext
