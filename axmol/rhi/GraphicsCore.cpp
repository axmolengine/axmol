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
#include "axmol/rhi/GraphicsDeviceFactory.h"
#include "axmol/tlx/inlined_vector.hpp"

#include "axmol/rhi/axslc-spec.h"

#if AX_ENABLE_D3D12
#    include "axmol/rhi/d3d12/GraphicsDevice12.h"
#endif
#if AX_ENABLE_VK
#    include "axmol/rhi/vulkan/GraphicsDeviceVK.h"
#endif
#if AX_ENABLE_D3D11
#    include "axmol/rhi/d3d11/GraphicsDevice11.h"
#endif
#if AX_ENABLE_MTL
#    include "axmol/rhi/metal/GraphicsDeviceMTL.h"
#endif
#if AX_ENABLE_GL
#    include "axmol/rhi/opengl/GraphicsDeviceGL.h"
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
    std::unique_ptr<GraphicsDevice> device;
    GraphicsBackend preferredBackend{GraphicsBackend::Auto};
    GraphicsBackend backend;
    int shaderLanguage{axslc::SHADER_LANG_NONE};
    int shaderProfile{0};
    int shaderILProfile{0};            // 0=unset, write by Driver init (D3D12: 60 DXIL or 51 DXBC)
    int vulkanMinAndroidApiLevel{31};  // Android 12+
    int backendPriorities[(int)rhi::GraphicsBackend::Count] = {
        rhi::DefaultDriverPriority::OpenGL, rhi::DefaultDriverPriority::D3D11, rhi::DefaultDriverPriority::D3D12,
        rhi::DefaultDriverPriority::Vulkan, rhi::DefaultDriverPriority::Metal};

    VulkanInterop* vulkanInterop{nullptr};
};

GraphicsCore::State& GraphicsCore::state()
{
    static State s_state;
    return s_state;
}

GraphicsDevice* GraphicsCore::device()
{
    return state().device.get();
}

GraphicsBackend GraphicsCore::backend()
{
    return state().backend;
}

VulkanInterop* GraphicsCore::getVulkanInterop()
{
    return state().vulkanInterop;
}

bool GraphicsCore::isOpenGL()
{
    return state().backend == GraphicsBackend::OpenGL;
}

bool GraphicsCore::isMetal()
{
    return state().backend == GraphicsBackend::Metal;
}

bool GraphicsCore::isD3D11()
{
    return state().backend == GraphicsBackend::D3D11;
}

bool GraphicsCore::isD3D12()
{
    return state().backend == GraphicsBackend::D3D12;
}

bool GraphicsCore::isVulkan()
{
    return state().backend == GraphicsBackend::Vulkan;
}

int GraphicsCore::shaderLanguage()
{
    return state().shaderLanguage;
}

int GraphicsCore::shaderProfile()
{
    return state().shaderProfile;
}

int GraphicsCore::shaderILProfile()
{
    return state().shaderILProfile;
}

void GraphicsCore::setShaderILProfile(int profile)
{
    state().shaderILProfile = profile;
}

void GraphicsCore::setPreferredBackend(GraphicsBackend backend)
{
    state().preferredBackend = backend;
}

void GraphicsCore::setVulkanMinAndroidApiLevel(int apiLevel)
{
    state().vulkanMinAndroidApiLevel = apiLevel;
}

void GraphicsCore::setVulkanInterop(VulkanInterop* interop)
{
    state().vulkanInterop = interop;
}

void GraphicsCore::setBackendPriority(GraphicsBackend backend, int prio)
{
    if (backend != GraphicsBackend::Auto)
        state().backendPriorities[(int)backend] = prio;
}

int GraphicsCore::getBackendPriority(GraphicsBackend backend)
{
    return backend != GraphicsBackend::Auto ? state().backendPriorities[(int)backend] : 0;
}

void GraphicsCore::initialize()
{
    tlx::inlined_vector<std::unique_ptr<GraphicsDeviceFactory>, (int)GraphicsBackend::Count> factories;

#if AX_ENABLE_D3D12
    factories.push_back(
        std::make_unique<D3D12GraphicsDeviceFactory>(state().backendPriorities[(int)GraphicsBackend::D3D12]));
#endif

#if AX_ENABLE_VK
#    if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    int apiLevel = android_get_device_api_level();
    if (apiLevel >= state().vulkanMinAndroidApiLevel)
        factories.push_back(
            std::make_unique<VulkanGraphicsDeviceFactory>(state().backendPriorities[(int)GraphicsBackend::Vulkan]));
    else
        AXLOGI("Vulkan skipped: device API level {} < required {}", apiLevel, state().vulkanMinAndroidApiLevel);
#    else
    factories.push_back(
        std::make_unique<VulkanGraphicsDeviceFactory>(state().backendPriorities[(int)GraphicsBackend::Vulkan]));
#    endif
#endif

#if AX_ENABLE_D3D11
    factories.push_back(
        std::make_unique<D3D11GraphicsDeviceFactory>(state().backendPriorities[(int)GraphicsBackend::D3D11]));
#endif

#if AX_ENABLE_MTL
    factories.push_back(
        std::make_unique<MetalGraphicsDeviceFactory>(state().backendPriorities[(int)GraphicsBackend::Metal]));
#endif

    if (factories.size() > 1)
    {
        std::stable_sort(factories.begin(), factories.end(),
                         [](auto& a, auto& b) { return a->priority() > b->priority(); });
    }

    for (auto& f : factories)
    {
        if (state().preferredBackend != GraphicsBackend::Auto && f->type() != state().preferredBackend)
            continue;
        auto driver = f->create();
        if (driver->init())
        {
            state().backend = driver->type();
            state().device  = std::move(driver);

            switch (state().backend)
            {
            case GraphicsBackend::D3D11:
                state().shaderLanguage  = axslc::SHADER_LANG_HLSL;
                state().shaderProfile   = 50;
                state().shaderILProfile = 50;
                break;
            case GraphicsBackend::D3D12:
                state().shaderLanguage  = axslc::SHADER_LANG_HLSL;
                state().shaderProfile   = 51;
                state().shaderILProfile = 51;
                break;
            case GraphicsBackend::Vulkan:
                state().shaderLanguage  = axslc::SHADER_LANG_SPIRV;
                state().shaderProfile   = 100;
                state().shaderILProfile = 100;
                break;
            case GraphicsBackend::Metal:
                state().shaderLanguage  = axslc::SHADER_LANG_MSL;
                state().shaderProfile   = make_msl_version(2, 0);
                state().shaderILProfile = make_msl_version(2, 0);
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
    if (!state().device)
    {
#if AX_ENABLE_GL
        state().device         = std::make_unique<gl::GraphicsDeviceImpl>();
        state().backend        = state().device->type();
        state().shaderLanguage = AX_GLES_PROFILE ? axslc::SHADER_LANG_ESSL : axslc::SHADER_LANG_GLSL;
#else
        throw std::runtime_error(
            "GraphicsCore::initialize failed: no suitable driver initialized "
            "and OpenGL fallback is not available (AX_ENABLE_GL disabled).");
#endif
    }
}

void GraphicsCore::activate()
{
#if AX_ENABLE_GL
    if (state().device && isOpenGL() && !state().shaderProfile)
    {
        state().device->init();
        auto& st           = state();
        auto* glDevice     = static_cast<gl::GraphicsDeviceImpl*>(st.device.get());
        st.shaderProfile   = glDevice->getShaderProfile();
        st.shaderILProfile = st.shaderProfile;
    }
#endif
}

void GraphicsCore::shutdown()
{
    state().device.reset();
}

}  // namespace ax::rhi
