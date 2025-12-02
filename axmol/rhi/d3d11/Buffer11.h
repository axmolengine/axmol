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
#pragma once

#include <vector>
#include <cassert>
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
     */
    BufferImpl(ID3D11Device* device,
               ID3D11DeviceContext* context,
               std::size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial);

    void updateData(const void* data, std::size_t size) override;
    void updateSubData(const void* data, std::size_t offset, std::size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    ID3D11Buffer* internalHandle() const noexcept { return _buffer.Get(); }
    D3D11_BIND_FLAG getBindFlag() const noexcept { return _bindFlag; }

private:
    void createNativeBuffer(const void* initial);

    tlx::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    ID3D11Device* _device;          // weak ref
    ID3D11DeviceContext* _context;  // weak ref
    ComPtr<ID3D11Buffer> _buffer;

    D3D11_USAGE _nativeUsage  = D3D11_USAGE_DYNAMIC;
    UINT _cpuAccess           = D3D11_CPU_ACCESS_WRITE;
    D3D11_BIND_FLAG _bindFlag = D3D11_BIND_VERTEX_BUFFER;
};

/** @} */

}  // namespace ax::rhi::d3d11
