#pragma once
#include "axmol/rhi/ShaderModule.h"
#include <d3d11.h>
#include <d3dcompiler.h>

namespace ax::rhi::d3d11
{

class DriverImpl;

class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(DriverImpl* driver, ShaderStage stage, Data& data);
    ~ShaderModuleImpl();

    IUnknown* internalHandle() const { return _shader; }

private:
    IUnknown* _shader = nullptr;
};

}  // namespace ax::rhi::d3d11
