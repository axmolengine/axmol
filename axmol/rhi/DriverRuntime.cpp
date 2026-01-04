#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/ApplicationBase.h"
#include "axmol/rhi/DriverRuntime.h"
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

std::unique_ptr<DriverBase> DriverRuntime::_currentDriver;
DriverType DriverRuntime::_currentDriverType = DriverType::Unkown;
uint32_t DriverRuntime::_currentShaderLang   = static_cast<uint32_t>(-1);

void DriverRuntime::init(DriverType driverType)
{
    const auto hasPreferredDriverType = driverType != DriverType::Unkown;

    auto& contextAttrs = ApplicationBase::getContextAttrs();

    tlx::inlined_vector<std::unique_ptr<DriverFactory>, (int)DriverType::Count> factories;

#if AX_ENABLE_D3D12
    factories.push_back(std::make_unique<D3D12DriverFactory>(contextAttrs.driverPriorities[(int)DriverType::D3D12]));
#endif
#if AX_ENABLE_VK
    factories.push_back(std::make_unique<VulkanDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::Vulkan]));
#endif
#if AX_ENABLE_D3D11
    factories.push_back(std::make_unique<D3D11DriverFactory>(contextAttrs.driverPriorities[(int)DriverType::D3D11]));
#endif
#if AX_ENABLE_MTL
    factories.push_back(std::make_unique<MetalDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::Metal]));
#endif
#if AX_ENABLE_GL
    factories.push_back(std::make_unique<GLDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::OpenGL]));
#endif

    if (factories.size() > 1)
    {
        std::stable_sort(factories.begin(), factories.end(),
                         [](auto& a, auto& b) { return a->priority() > b->priority(); });
    }

    for (auto& f : factories)
    {
        if (hasPreferredDriverType && driverType != f->type())
            continue;
        auto driver = f->create();
        if (driver && driver->init())
        {
            _currentDriverType = driver->type();
            _currentDriver     = std::move(driver);

            switch (_currentDriverType)
            {
            case DriverType::OpenGL:
#if AX_GLES_PROFILE
                _currentShaderLang = axslc::SHADER_LANG_ESSL;
#else
                _currentShaderLang = axslc::SHADER_LANG_GLSL;
#endif
                break;
            case DriverType::D3D11:
            case DriverType::D3D12:
                _currentShaderLang = axslc::SHADER_LANG_HLSL;
                break;
            case DriverType::Vulkan:
                _currentShaderLang = axslc::SHADER_LANG_SPIRV;
                break;
            case DriverType::Metal:
                _currentShaderLang == axslc::SHADER_LANG_MSL;
                break;
            }

            break;
        }
    }

    if (!_currentDriver && hasPreferredDriverType)
        throw std::runtime_error("DriverRuntime::init failed: no suitable driver initialized");
}

void DriverRuntime::uninit()
{
    _currentDriver.reset();
}

}  // namespace ax::rhi
