
/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
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
