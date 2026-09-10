/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <vector>
#include <assert.h>
#include <d3d11.h>

#include "axmol/rhi/Buffer.h"
#include "axmol/rhi/DXUtils.h"
#include "axmol/tlx/byte_buffer.hpp"

namespace ax::rhi::d3d11
{

/**
 * @addtogroup _d3d11
 * @{
 */

/**
 * @brief A D3D11-based Buffer implementation
 *
 */
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
     * @param stride   logical storage element stride in bytes (0 when unspecified)
     */
    BufferImpl(ID3D11Device* device,
               ID3D11DeviceContext* context,
               size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial,
               uint32_t stride = 0);

    void updateData(const void* data, size_t size) override;
    void updateSubData(const void* data, size_t offset, size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    ID3D11Buffer* internalHandle() const noexcept { return _buffer.Get(); }
    D3D11_BIND_FLAG getBindFlag() const noexcept { return _bindFlag; }

    /**
     * Get (and lazily create) a shader-resource view for this storage buffer.
     * @return The SRV, or null if the buffer cannot expose one.
     */
    ID3D11ShaderResourceView* getSRV() const noexcept;

    /**
     * Get (and lazily create) an unordered-access view for this storage buffer.
     * @return The UAV, or null if the buffer cannot expose one.
     */
    ID3D11UnorderedAccessView* getUAV() const noexcept;

private:
    void createNativeBuffer(const void* initial);
    void createViews() const;

    tlx::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    ID3D11Device* _device;          // weak ref
    ID3D11DeviceContext* _context;  // weak ref
    mutable ComPtr<ID3D11Buffer> _buffer;
    mutable ComPtr<ID3D11ShaderResourceView> _srv;
    mutable ComPtr<ID3D11UnorderedAccessView> _uav;

    D3D11_USAGE _nativeUsage  = D3D11_USAGE_DYNAMIC;
    UINT _cpuAccess           = D3D11_CPU_ACCESS_WRITE;
    D3D11_BIND_FLAG _bindFlag = D3D11_BIND_VERTEX_BUFFER;
};

/** @} */

}  // namespace ax::rhi::d3d11
