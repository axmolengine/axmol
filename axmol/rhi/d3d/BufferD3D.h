// -------------------------------------------------------------------------------------------------
// BufferImpl.hpp
// Direct3D-11 implementation of the abstract Buffer interface
// -------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <cassert>
#include <d3d11.h>
#include <wrl/client.h>

#include "axmol/rhi/Buffer.h"

namespace ax::rhi::d3d
{

class BufferImpl final : public Buffer
{
public:
    /**
     * @param device   created device from driver impl
     * @param context  the context related to device
     * @param size     request size of buffer
     * @param type     BufferType::VERTEX or BufferType::INDEX
     * @param usage    BufferUsage::STATIC / DYNAMIC / STREAM
     * @param initial  initial data
     */
    BufferImpl(ID3D11Device* device,
               ID3D11DeviceContext* context,
               std::size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial = nullptr);

    void updateData(const void* data, std::size_t size) override;
    void updateSubData(const void* data, std::size_t offset, std::size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    ID3D11Buffer* internalHandle() const noexcept { return _buffer.Get(); }
    D3D11_BIND_FLAG getBindFlag() const noexcept { return _bindFlag; }

private:
    void createNativeBuffer(const void* initial);

    size_t _capacity{0};

    std::vector<uint8_t> _defaultData;
    bool _needDefaultStoredData = true;

    ID3D11Device* _device;          // weak ref
    ID3D11DeviceContext* _context;  // weak ref
    Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;

    D3D11_USAGE _nativeUsage  = D3D11_USAGE_DYNAMIC;
    UINT _cpuAccess           = D3D11_CPU_ACCESS_WRITE;
    D3D11_BIND_FLAG _bindFlag = D3D11_BIND_VERTEX_BUFFER;
};

}  // namespace ax::rhi::d3d
