/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/DecorativeDisplay.h"
#include "axmol/scene/Node.h"

using namespace ax;

namespace ax::ext
{

DecorativeDisplay* DecorativeDisplay::create()
{
    DecorativeDisplay* pDisplay = new DecorativeDisplay();
    if (pDisplay->init())
    {
        pDisplay->autorelease();
        return pDisplay;
    }
    AX_SAFE_DELETE(pDisplay);
    return nullptr;
}

DecorativeDisplay::DecorativeDisplay() : _display(nullptr), _displayData(nullptr)

{
#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    _colliderDetector = nullptr;
#endif
}

DecorativeDisplay::~DecorativeDisplay(void)
{
    AX_SAFE_RELEASE_NULL(_displayData);
    AX_SAFE_RELEASE_NULL(_display);

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    AX_SAFE_RELEASE_NULL(_colliderDetector);
#endif
}

bool DecorativeDisplay::init()
{
    return true;
}

void DecorativeDisplay::setDisplay(ax::Node* display)
{
    if (_display != display)
    {
        AX_SAFE_RETAIN(display);
        AX_SAFE_RELEASE(_display);
        _display = display;
    }
}

}  // namespace ax::ext
