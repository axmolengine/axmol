#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/ApplicationBase.h"
#include "axmol/rhi/DriverRuntime.h"
#include "axmol/rhi/DriverFactory.h"
#include "axmol/tlx/inlined_vector.hpp"

#define AX_ENABLE_D3D12 1

#ifdef AX_ENABLE_D3D12
#    include "axmol/rhi/d3d12/Driver12.h"
#endif
#ifdef AX_ENABLE_VULKAN
#    include "axmol/rhi/vulkan/DriverVK.h"
#endif
#ifdef AX_ENABLE_D3D11
#    include "axmol/rhi/d3d11/Driver11.h"
#endif
#ifdef AX_ENABLE_METAL
#    include "axmol/rhi/metal/DriverMTL.h"
#endif
#ifdef AX_ENABLE_GL
#    include "axmol/rhi/opengl/DriverGL.h"
#endif

namespace ax::rhi
{

DriverBase* currentDriver()
{
    return DriverRuntime::currentDriver();
}

std::unique_ptr<DriverBase> DriverRuntime::_currentDriver;
DriverType DriverRuntime::_currentDriverType = DriverType::Unkown;

void DriverRuntime::init()
{
    auto& contextAttrs = ApplicationBase::getContextAttrs();

    tlx::inlined_vector<std::unique_ptr<DriverFactory>, (int)DriverType::Count> factories;

#ifdef AX_ENABLE_D3D12
    factories.push_back(std::make_unique<D3D12DriverFactory>(contextAttrs.driverPriorities[(int)DriverType::D3D12]));
#endif
#ifdef AX_ENABLE_VULKAN
    factories.push_back(std::make_unique<VulkanDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::Vulkan]));
#endif
#ifdef AX_ENABLE_D3D11
    factories.push_back(std::make_unique<D3D11DriverFactory>(contextAttrs.driverPriorities[(int)DriverType::D3D11]));
#endif
#ifdef AX_ENABLE_METAL
    factories.push_back(std::make_unique<MetalDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::Metal]));
#endif
#ifdef AX_ENABLE_GL
    factories.push_back(std::make_unique<GLDriverFactory>(contextAttrs.driverPriorities[(int)DriverType::OpenGL])));
#endif

    if (factories.size() > 1)
    {
        std::stable_sort(factories.begin(), factories.end(),
                         [](auto& a, auto& b) { return a->priority() > b->priority(); });
    }

    for (auto& f : factories)
    {
        auto driver = f->create();
        if (driver && driver->init())
        {
            _currentDriverType = driver->type();
            _currentDriver = std::move(driver);
            break;
        }
    }

    if (!_currentDriver)
        throw std::runtime_error("DriverRuntime::init failed: no suitable driver initialized");
}

void DriverRuntime::uninit()
{
    _currentDriver.reset();
}

}  // namespace ax::rhi
