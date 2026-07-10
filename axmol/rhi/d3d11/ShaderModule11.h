#pragma once
#include "axmol/rhi/ShaderModule.h"
#include "axmol/platform/msw/ComPtr.h"
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
    std::span<uint8_t> getBytecode() const { return _blob; }

private:
    IUnknown* _shader = nullptr;
    std::span<uint8_t> _blob;
    ComPtr<IUnknown> _nativeBlob;
};

}  // namespace ax::rhi::d3d11
