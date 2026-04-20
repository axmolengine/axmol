/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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
#include "axmol/platform/Device.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/StdC.h"
#include "ntcvt/ntcvt.hpp"

namespace ax
{
int Device::getDPI()
{
    static int dpi = -1;
    if (dpi == -1)
    {
        HDC hScreenDC = GetDC(nullptr);
        int PixelsX   = GetDeviceCaps(hScreenDC, HORZRES);
        int MMX       = GetDeviceCaps(hScreenDC, HORZSIZE);
        ReleaseDC(nullptr, hScreenDC);
        dpi = (int)(254.0f * PixelsX / MMX / 10);
    }
    return dpi;
}

float Device::getPixelRatio()
{
    const HDC dc    = GetDC(nullptr);
    const auto xdpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);
    return xdpi / 96.0f;
}

void Device::setAccelerometerEnabled(bool isEnabled) {}

void Device::setAccelerometerInterval(float interval) {}

void Device::setKeepScreenOn(bool value)
{
    AX_UNUSED_PARAM(value);
}

void Device::vibrate(float duration)
{
    AX_UNUSED_PARAM(duration);
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style)
{
    AX_UNUSED_PARAM(style);
}

void Device::impactOccurred(ImpactFeedbackStyle style)
{
    AX_UNUSED_PARAM(style);
}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType type)
{
    AX_UNUSED_PARAM(type);
}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged() {}

}  // namespace ax
