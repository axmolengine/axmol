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
#include "axmol/rhi/d3d11/Buffer11.h"

namespace ax::rhi::d3d11
{

// BufferUsage -> D3D11_USAGE / CPU flags
// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage
static void translateUsage(BufferUsage in, D3D11_USAGE& outUsage, UINT& outCpu)
{
    switch (in)
    {

    case BufferUsage::DYNAMIC:  // GPU read, CPU write
        outUsage = D3D11_USAGE_DYNAMIC;
        outCpu   = D3D11_CPU_ACCESS_WRITE;
        break;
    case BufferUsage::STATIC:  // GPU read/write, can use d3d API: UpdateSubresource to update data
        outUsage = D3D11_USAGE_DEFAULT;
        outCpu   = 0;
        break;
    case BufferUsage::IMMUTABLE:  // GPU read, must provide inital data, axmol do delay create when first updateData
                                  // called
        outUsage = D3D11_USAGE_IMMUTABLE;
        outCpu   = 0;
        break;
    }
}

// BufferType -> BindFlag
static D3D11_BIND_FLAG translateBindFlag(BufferType t)
{
    switch (t)
    {
    case BufferType::VERTEX:
        return D3D11_BIND_VERTEX_BUFFER;

    case BufferType::INDEX:
        return D3D11_BIND_INDEX_BUFFER;

    case BufferType::UNIFORM:
        return D3D11_BIND_CONSTANT_BUFFER;

    case BufferType::STORAGE:
        return static_cast<D3D11_BIND_FLAG>(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

    case BufferType::PIXEL_PACK_BUFFER:
        // read backbuffer，map as staging or copy target
        return D3D11_BIND_SHADER_RESOURCE;  // FIXME:

    default:
        AXLOGW("Unknown BufferType: {}", static_cast<uint32_t>(t));
        return static_cast<D3D11_BIND_FLAG>(0);
    }
}

static inline size_t alignTo(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* -------------------------------------------------- ctor */
BufferImpl::BufferImpl(ID3D11Device* device,
                       ID3D11DeviceContext* context,
                       size_t size,
                       BufferType type,
                       BufferUsage usage,
                       const void* initial,
                       uint32_t stride)
    : Buffer(size, type, usage, stride), _device(device), _context(context)
{
    translateUsage(usage, _nativeUsage, _cpuAccess);
    _bindFlag = translateBindFlag(type);

    if (type == BufferType::STORAGE)
    {
        AXASSERT(size > 0 && (size % sizeof(uint32_t)) == 0,
                 "D3D storage buffer size must be a non-zero multiple of 4 bytes");
        AXASSERT(stride == 0 || (size % stride) == 0, "Storage buffer size must be divisible by its logical stride");

        // D3D11 forbids D3D11_BIND_UNORDERED_ACCESS with D3D11_USAGE_DYNAMIC;
        // use DEFAULT usage and update via UpdateSubresource.
        _nativeUsage = D3D11_USAGE_DEFAULT;
        _cpuAccess   = 0;
    }

    _capacity = _bindFlag == D3D11_BIND_CONSTANT_BUFFER ? alignTo(size, 16) : size;

    if (initial && size)
        _defaultData.assign(static_cast<const uint8_t*>(initial), static_cast<const uint8_t*>(initial) + size);

    if (usage != BufferUsage::IMMUTABLE || initial)
        createNativeBuffer(initial);
}

/* -------------------------------------------------- createNativeBuffer */
void BufferImpl::createNativeBuffer(const void* initial)
{
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth      = static_cast<UINT>(_capacity);
    desc.Usage          = _nativeUsage;
    desc.BindFlags      = _bindFlag;
    desc.CPUAccessFlags = _cpuAccess;
    desc.MiscFlags      = 0;
    if (_type == BufferType::STORAGE)
    {
        // axslcc lowers portable storage buffers to ByteAddressBuffer/RWByteAddressBuffer on D3D.
        desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        desc.StructureByteStride = 0;
    }

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = initial;

    HRESULT hr = _device->CreateBuffer(&desc, initial ? &initData : nullptr, &_buffer);

    if (FAILED(hr))
    {
        AXLOGE("Failed to create ID3D11Buffer, size={}, alignedSize={}, hr=0x{:08X}", _size, _capacity,
               static_cast<unsigned int>(hr));
        assert(false && "Failed to create ID3D11Buffer");
    }
}

/* -------------------------------------------------- createViews */
void BufferImpl::createViews() const
{
    if (!_buffer)
        return;

    if (_bindFlag & D3D11_BIND_SHADER_RESOURCE)
    {
        HRESULT hr = S_OK;
        if (_type == BufferType::STORAGE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format                = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.ViewDimension         = D3D11_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements  = static_cast<UINT>(_capacity / sizeof(uint32_t));
            srvDesc.BufferEx.Flags        = D3D11_BUFFEREX_SRV_FLAG_RAW;
            hr                            = _device->CreateShaderResourceView(_buffer.Get(), &srvDesc, &_srv);
        }
        else
        {
            hr = _device->CreateShaderResourceView(_buffer.Get(), nullptr, &_srv);
        }

        AXASSERT(SUCCEEDED(hr) && _srv, "Failed to create D3D11 buffer SRV");
    }

    if (_bindFlag & D3D11_BIND_UNORDERED_ACCESS)
    {
        HRESULT hr = S_OK;
        if (_type == BufferType::STORAGE)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
            uavDesc.Format              = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements  = static_cast<UINT>(_capacity / sizeof(uint32_t));
            uavDesc.Buffer.Flags        = D3D11_BUFFER_UAV_FLAG_RAW;
            hr                          = _device->CreateUnorderedAccessView(_buffer.Get(), &uavDesc, &_uav);
        }
        else
        {
            hr = _device->CreateUnorderedAccessView(_buffer.Get(), nullptr, &_uav);
        }

        AXASSERT(SUCCEEDED(hr) && _uav, "Failed to create D3D11 buffer UAV");
    }
}

ID3D11ShaderResourceView* BufferImpl::getSRV() const noexcept
{
    if (!_srv && _bindFlag & D3D11_BIND_SHADER_RESOURCE)
        createViews();
    return _srv.Get();
}

ID3D11UnorderedAccessView* BufferImpl::getUAV() const noexcept
{
    if (!_uav && _bindFlag & D3D11_BIND_UNORDERED_ACCESS)
        createViews();
    return _uav.Get();
}

/* -------------------------------------------------- updateData */
void BufferImpl::updateData(const void* data, size_t size)
{
    assert(size <= _size);
    assert(data);

    if (_nativeUsage == D3D11_USAGE_DYNAMIC)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        _context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        ::memcpy(mapped.pData, data, size);
        _context->Unmap(_buffer.Get(), 0);
    }
    else if (_nativeUsage == D3D11_USAGE_IMMUTABLE)
    {
        if (!_buffer && size == _capacity)
            createNativeBuffer(data);
        else
            AXLOGE("immutable buffer must update whole data one-time");
    }
    else
    {
        _context->UpdateSubresource(_buffer.Get(), 0, nullptr, data, 0, 0);
    }

    if (_needDefaultStoredData)
        _defaultData.assign(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
}

/* -------------------------------------------------- updateSubData */
void BufferImpl::updateSubData(const void* data, size_t offset, size_t size)
{
    assert(data && (offset + size <= _size));

    if (_nativeUsage == D3D11_USAGE_DYNAMIC)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        _context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped);
        ::memcpy(static_cast<uint8_t*>(mapped.pData) + offset, data, size);
        _context->Unmap(_buffer.Get(), 0);
    }
    else if (_nativeUsage == D3D11_USAGE_IMMUTABLE)
    {
        if (!_buffer && size == _capacity && offset == 0)
            createNativeBuffer(data);
        else
            AXLOGE("immutable buffer must update whole data one-time");
    }
    else
    {
        D3D11_BOX box{};
        box.left   = static_cast<UINT>(offset);
        box.right  = static_cast<UINT>(offset + size);
        box.top    = 0;
        box.bottom = 1;
        box.front  = 0;
        box.back   = 1;

        _context->UpdateSubresource(_buffer.Get(), 0, &box, data, 0, 0);
    }

    if (_needDefaultStoredData)
        ::memcpy(_defaultData.data() + offset, data, size);
}

/* -------------------------------------------------- usingDefaultStoredData */
void BufferImpl::usingDefaultStoredData(bool needDefaultStoredData)
{
    _needDefaultStoredData = needDefaultStoredData;

    if (needDefaultStoredData && !_defaultData.empty())
    {
        // restore to default, write to GPU immidately
        updateData(_defaultData.data(), _defaultData.size());
    }
}

}  // namespace ax::rhi::d3d11
