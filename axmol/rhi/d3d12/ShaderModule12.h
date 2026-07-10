#pragma once
#include "axmol/rhi/ShaderModule.h"
#include "axmol/rhi/DXUtils.h"
#include <d3d12.h>
#include <span>

namespace ax::rhi::d3d12
{

class DriverImpl;

struct SLCReflectContext;

struct D3D12BlobHandle
{
    ComPtr<IUnknown> blob;
    std::span<uint8_t> view;
};

struct UniformBlockInfo
{
    int binding;
    uint32_t sizeBytes;
    uint16_t numMembers;
    std::string name;
};

class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(DriverImpl* driver, ShaderStage stage, Data& chunk);
    ~ShaderModuleImpl();

    std::span<uint8_t> getBlob() const { return _blob; }

private:
    std::span<uint8_t> _blob;
    ComPtr<IUnknown> _nativeBlob;
};

}  // namespace ax::rhi::d3d12
