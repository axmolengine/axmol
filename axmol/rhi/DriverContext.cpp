#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/ApplicationBase.h"
#include "axmol/rhi/DriverContext.h"
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

namespace ax::rhi
{

std::unique_ptr<DriverBase> DriverContext::_currentDriver;
DriverType DriverContext::_currentDriverType = DriverType::Auto;
int DriverContext::_currentShaderLang        = axslc::SHADER_LANG_NONE;
int DriverContext::_currentShaderProfile     = 0;

static int _driverPriorities[(int)rhi::DriverType::Count] = {
    rhi::DefaultDriverPriority::OpenGL, rhi::DefaultDriverPriority::D3D11, rhi::DefaultDriverPriority::D3D12,
    rhi::DefaultDriverPriority::Vulkan, rhi::DefaultDriverPriority::Metal};

// refer: https://github.com/KhronosGroup/SPIRV-Cross/blob/main/spirv_msl.hpp#L575
static uint32_t make_msl_version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0)
{
    return (major * 10000) + (minor * 100) + patch;
}

void DriverContext::setDriverPriority(DriverType driverType, int prio)
{
    if (driverType != DriverType::Auto)
        _driverPriorities[(int)driverType] = prio;
}

int DriverContext::getDriverPriority(DriverType driverType)
{
    return driverType != DriverType::Auto ? _driverPriorities[(int)driverType] : 0;
}

void DriverContext::makeCurrentDriver()
{
    auto& contextAttrs = ApplicationBase::getContextAttrs();

    tlx::inlined_vector<std::unique_ptr<DriverFactory>, (int)DriverType::Count> factories;

#if AX_ENABLE_D3D12
    factories.push_back(std::make_unique<D3D12DriverFactory>(_driverPriorities[(int)DriverType::D3D12]));
#endif

#if AX_ENABLE_VK
    factories.push_back(std::make_unique<VulkanDriverFactory>(_driverPriorities[(int)DriverType::Vulkan]));
#endif

#if AX_ENABLE_D3D11
    factories.push_back(std::make_unique<D3D11DriverFactory>(_driverPriorities[(int)DriverType::D3D11]));
#endif

#if AX_ENABLE_MTL
    factories.push_back(std::make_unique<MetalDriverFactory>(_driverPriorities[(int)DriverType::Metal]));
#endif

    if (factories.size() > 1)
    {
        std::stable_sort(factories.begin(), factories.end(),
                         [](auto& a, auto& b) { return a->priority() > b->priority(); });
    }

    for (auto& f : factories)
    {
        auto driver = f->create();
        if (contextAttrs.driverPreference != DriverPreference::Auto && driver->type() != contextAttrs.driverPreference)
            continue;
        if (driver->init())
        {
            _currentDriverType = driver->type();
            _currentDriver     = std::move(driver);

            switch (_currentDriverType)
            {
            case DriverType::D3D11:
                _currentShaderLang    = axslc::SHADER_LANG_HLSL;
                _currentShaderProfile = 50;
                break;
            case DriverType::D3D12:
                _currentShaderLang    = axslc::SHADER_LANG_HLSL;
                _currentShaderProfile = 51;
                break;
            case DriverType::Vulkan:
                _currentShaderLang    = axslc::SHADER_LANG_SPIRV;
                _currentShaderProfile = 100;
                break;
            case DriverType::Metal:
                _currentShaderLang    = axslc::SHADER_LANG_MSL;
                _currentShaderProfile = make_msl_version(2, 0);
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
    if (!_currentDriver)
    {
#if AX_ENABLE_GL
        _currentDriver     = std::make_unique<gl::DriverImpl>();
        _currentDriverType = _currentDriver->type();
        _currentShaderLang = AX_GLES_PROFILE ? axslc::SHADER_LANG_ESSL : axslc::SHADER_LANG_GLSL;
#else
        throw std::runtime_error(
            "DriverContext::makeCurrentDriver failed: no suitable driver initialized "
            "and OpenGL fallback is not available (AX_ENABLE_GL disabled).");
#endif
    }
}

void DriverContext::activateCurrentDriver()
{
    if (_currentDriver && isOpenGL() && !_currentShaderProfile)
    {
        _currentDriver->init();
        _currentShaderProfile = AX_GLES_PROFILE ? 300 : 330;
    }
}

void DriverContext::destroyCurrentDriver()
{
    _currentDriver.reset();
}

}  // namespace ax::rhi
