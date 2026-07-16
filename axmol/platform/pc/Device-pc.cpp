
/****************************************************************************
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
****************************************************************************/ \
#include "axmol/platform/Device.h"
#include "GLFW/glfw3.h"

namespace ax
{
int Device::getDisplayRefreshRate()
{
    // Retrieve the display refresh rate from GLFW.
    // Known behavior when targeting WebAssembly with emsdk:
    //   - emsdk 3.x: glfwGetVideoMode() returns 60 Hz
    //   - emsdk 4.x: glfwGetVideoMode() may return 0
    //
    // Apply a safety clamp: if the reported value is out of a reasonable range
    // (e.g. <MIN_REFRESH_RATE Hz or >MAX_REFRESH_RATE Hz), fall back to DEFAULT_REFRESH_RATE Hz as a safe default.
    auto hz = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
    if (hz < MIN_REFRESH_RATE || hz > MAX_REFRESH_RATE)
        hz = DEFAULT_REFRESH_RATE;
    return hz;
}

void Device::setPreferredOrientation(Device::Orientation /*orientation*/) {}

Device::Orientation Device::getPreferredOrientation()
{
    return Orientation::Unknown;
}

Device::OrientationMask Device::getSupportedOrientations()
{
    return OrientationMask::All;
}

Device::Orientation Device::getCurrentOrientation()
{
    return Orientation::Unknown;
}

Device::Orientation Device::getPhysicalOrientation()
{
    return Orientation::Unknown;
}

Device::Orientation Device::resolveOrientation()
{
    return Orientation::Unknown;
}

}  // namespace ax
