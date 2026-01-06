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
 ****************************************************************************/
#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include "axmol/rhi/DriverBase.h"

namespace ax::rhi
{
/**
 * @class DriverContext
 * @brief Centralized manager for graphics driver lifecycle and shader environment.
 *
 * DriverContext provides a unified interface to create, activate, and destroy
 * rendering drivers across multiple backends (D3D, Vulkan, Metal, OpenGL).
 * It abstracts backend differences and ensures consistent shader language/profile
 * setup for the current driver.
 */
class AX_DLL DriverContext
{
public:
    /**
     * @brief Selects and prepares the current driver instance.
     *
     * This function chooses the most suitable driver implementation based on
     * the configured priorities and available backends. For non-OpenGL drivers,
     * the driver is created and automatically initialized. For OpenGL, only the
     * driver object is constructed here; initialization must be deferred until
     * a valid window/context is available.
     */
    static void makeCurrentDriver();

    /**
     * @brief Activates the current driver (OpenGL only).
     *
     * This function is only required when using OpenGL, since its initialization
     * cannot be performed at construction time. It should be called after the
     * window and GL context have been created, to complete the driver setup.
     *
     * For non-OpenGL drivers, initialization is already performed during
     * makeCurrentDriver(), so this function is not needed and will typically
     * be a no-op.
     */
    static void activateCurrentDriver();

    /**
     * @brief Releases and destroys the current driver instance, freeing all associated resources.
     */
    static void destroyCurrentDriver();

    static DriverBase* currentDriver() { return _currentDriver.get(); }
    static DriverType currentDriverType() { return _currentDriverType; }

    static bool isOpenGL() { return _currentDriverType == DriverType::OpenGL; }
    static bool isMetal() { return _currentDriverType == DriverType::Metal; }
    static bool isD3D11() { return _currentDriverType == DriverType::D3D11; }
    static bool isD3D12() { return _currentDriverType == DriverType::D3D12; }
    static bool isVulkan() { return _currentDriverType == DriverType::Vulkan; }

    static int currentShaderLang() { return _currentShaderLang; }
    static int currentShaderProfile() { return _currentShaderProfile; }

private:
    static std::unique_ptr<DriverBase> _currentDriver;
    static DriverType _currentDriverType;
    static int _currentShaderLang;
    static int _currentShaderProfile;
};

}  // namespace ax::rhi
