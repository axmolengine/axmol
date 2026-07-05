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
#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/ApplicationCore.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/DriverFactory.h"
#include "axmol/tlx/inlined_vector.hpp"

#include "axmol/rhi/axslc-spec.h"

#if AX_ENABLE_D3D12
#    include "axmol/rhi/d3d12/Driver12.h"
#endif
#if AX_ENABLE_VK
#    include "axmol/rhi/vulkan/DriverVK.h"
#endif
#if AX_ENABLE_D3D11
#    include "axmol/rhi/d3d11/Driver11.h"
#endif
#if AX_ENABLE_MTL
#    include "axmol/rhi/metal/DriverMTL.h"
#endif
#if AX_ENABLE_GL
#    include "axmol/rhi/opengl/DriverGL.h"
#endif

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include <android/api-level.h>
#endif

namespace ax::rhi
{

// refer: https://github.com/KhronosGroup/SPIRV-Cross/blob/main/spirv_msl.hpp#L575
static uint32_t make_msl_version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0)
{
    return (major * 10000) + (minor * 100) + patch;
}

struct GraphicsCore::State
{
    std::unique_ptr<DriverBase> currentDriver;
    DriverPreference driverPreference{DriverType::Auto};
    DriverType currentDriverType;
    int currentShaderLang{axslc::SHADER_LANG_NONE};
    int currentShaderProfile{0};
    int vulkanMinAndroidApiLevel{31};  // Android 12+
    int driverPriorities[(int)rhi::DriverType::Count] = {
        rhi::DefaultDriverPriority::OpenGL, rhi::DefaultDriverPriority::D3D11, rhi::DefaultDriverPriority::D3D12,
        rhi::DefaultDriverPriority::Vulkan, rhi::DefaultDriverPriority::Metal};

    VulkanInterop* vulkanInterop{nullptr};
};

GraphicsCore::State& GraphicsCore::state()
{
    static State s_state;
    return s_state;
}

DriverBase* GraphicsCore::currentDriver()
{
    return state().currentDriver.get();
}

DriverType GraphicsCore::currentDriverType()
{
    return state().currentDriverType;
}

VulkanInterop* GraphicsCore::getVulkanInterop()
{
    return state().vulkanInterop;
}

bool GraphicsCore::isOpenGL()
{
    return state().currentDriverType == DriverType::OpenGL;
}

bool GraphicsCore::isMetal()
{
    return state().currentDriverType == DriverType::Metal;
}

bool GraphicsCore::isD3D11()
{
    return state().currentDriverType == DriverType::D3D11;
}

bool GraphicsCore::isD3D12()
{
    return state().currentDriverType == DriverType::D3D12;
}

bool GraphicsCore::isVulkan()
{
    return state().currentDriverType == DriverType::Vulkan;
}

int GraphicsCore::currentShaderLang()
{
    return state().currentShaderLang;
}

int GraphicsCore::currentShaderProfile()
{
    return state().currentShaderProfile;
}

void GraphicsCore::setDriverPreference(DriverPreference driverPreference)
{
    state().driverPreference = driverPreference;
}

void GraphicsCore::setVulkanMinAndroidApiLevel(int apiLevel)
{
    state().vulkanMinAndroidApiLevel = apiLevel;
}

void GraphicsCore::setVulkanInterop(VulkanInterop* interop)
{
    state().vulkanInterop = interop;
}

void GraphicsCore::setDriverPriority(DriverType driverType, int prio)
{
    if (driverType != DriverType::Auto)
        state().driverPriorities[(int)driverType] = prio;
}

int GraphicsCore::getDriverPriority(DriverType driverType)
{
    return driverType != DriverType::Auto ? state().driverPriorities[(int)driverType] : 0;
}

void GraphicsCore::makeCurrentDriver()
{
    tlx::inlined_vector<std::unique_ptr<DriverFactory>, (int)DriverType::Count> factories;

#if AX_ENABLE_D3D12
    factories.push_back(std::make_unique<D3D12DriverFactory>(state().driverPriorities[(int)DriverType::D3D12]));
#endif

#if AX_ENABLE_VK
#    if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    int apiLevel = android_get_device_api_level();
    if (apiLevel >= state().vulkanMinAndroidApiLevel)
        factories.push_back(std::make_unique<VulkanDriverFactory>(state().driverPriorities[(int)DriverType::Vulkan]));
    else
        AXLOGI("Vulkan skipped: device API level {} < required {}", apiLevel, state().vulkanMinAndroidApiLevel);
#    else
    factories.push_back(std::make_unique<VulkanDriverFactory>(state().driverPriorities[(int)DriverType::Vulkan]));
#    endif
#endif

#if AX_ENABLE_D3D11
    factories.push_back(std::make_unique<D3D11DriverFactory>(state().driverPriorities[(int)DriverType::D3D11]));
#endif

#if AX_ENABLE_MTL
    factories.push_back(std::make_unique<MetalDriverFactory>(state().driverPriorities[(int)DriverType::Metal]));
#endif

    if (factories.size() > 1)
    {
        std::stable_sort(factories.begin(), factories.end(),
                         [](auto& a, auto& b) { return a->priority() > b->priority(); });
    }

    for (auto& f : factories)
    {
        if (state().driverPreference != DriverPreference::Auto && f->type() != state().driverPreference)
            continue;
        auto driver = f->create();
        if (driver->init())
        {
            state().currentDriverType = driver->type();
            state().currentDriver     = std::move(driver);

            switch (state().currentDriverType)
            {
            case DriverType::D3D11:
                state().currentShaderLang    = axslc::SHADER_LANG_HLSL;
                state().currentShaderProfile = 50;
                break;
            case DriverType::D3D12:
                state().currentShaderLang    = axslc::SHADER_LANG_HLSL;
                state().currentShaderProfile = 51;
                break;
            case DriverType::Vulkan:
                state().currentShaderLang    = axslc::SHADER_LANG_SPIRV;
                state().currentShaderProfile = 100;
                break;
            case DriverType::Metal:
                state().currentShaderLang    = axslc::SHADER_LANG_MSL;
                state().currentShaderProfile = make_msl_version(2, 0);
                break;
            }

            break;
        }
    }

    // Fallback branch:
    // - If no driver was successfully created from the preferred list,
    //   and OpenGL support is compiled in (AX_ENABLE_GL),
    //   we construct an OpenGL driver instance here as a fallback.
    // - IMPORTANT: OpenGL cannot be fully initialized until a valid
    //   window/context has been created (GLFW/SDL/Native window).
    //   Therefore, only the driver object is constructed at this stage;
    //   the actual driver->init() call must be invoked later,
    //   after the window is created and the GL context is current.
    // - If AX_ENABLE_GL is disabled, no fallback is possible and
    //   we throw an exception to indicate that no suitable driver
    //   could be initialized.
    if (!state().currentDriver)
    {
#if AX_ENABLE_GL
        state().currentDriver     = std::make_unique<gl::DriverImpl>();
        state().currentDriverType = state().currentDriver->type();
        state().currentShaderLang = AX_GLES_PROFILE ? axslc::SHADER_LANG_ESSL : axslc::SHADER_LANG_GLSL;
#else
        throw std::runtime_error(
            "GraphicsCore::makeCurrentDriver failed: no suitable driver initialized "
            "and OpenGL fallback is not available (AX_ENABLE_GL disabled).");
#endif
    }
}

void GraphicsCore::activateCurrentDriver()
{
    if (state().currentDriver && isOpenGL() && !state().currentShaderProfile)
    {
        state().currentDriver->init();
        state().currentShaderProfile = AX_GLES_PROFILE ? 300 : 330;
    }
}

void GraphicsCore::destroyCurrentDriver()
{
    state().currentDriver.reset();
}

}  // namespace ax::rhi
