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

#include "DecorativeDisplay.h"
#include "2d/Node.h"

using namespace ax;

namespace cocostudio
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
#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    _colliderDetector = nullptr;
#endif
}

DecorativeDisplay::~DecorativeDisplay(void)
{
    AX_SAFE_RELEASE_NULL(_displayData);
    AX_SAFE_RELEASE_NULL(_display);

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
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

}  // namespace cocostudio
