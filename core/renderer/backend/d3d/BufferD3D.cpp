#include "BufferD3D.h"

namespace ax::backend::d3d
{

// BufferUsage -> D3D11_USAGE / CPU flags
static void translateUsage(BufferUsage in, D3D11_USAGE& outUsage, UINT& outCpu)
{
    switch (in)
    {
    case BufferUsage::STATIC:
        outUsage = D3D11_USAGE_IMMUTABLE;
        outCpu   = 0;
        break;
    case BufferUsage::DYNAMIC:
        outUsage = D3D11_USAGE_DYNAMIC;
        outCpu   = D3D11_CPU_ACCESS_WRITE;
        break;
    // case BufferUsage::STREAM:
    default:
        outUsage = D3D11_USAGE_STAGING;  // FIXME:
        outCpu   = D3D11_CPU_ACCESS_WRITE;
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

    case BufferType::PIXEL_PACK_BUFFER:
        // read backbuffer，map as staging or copy target
        return D3D11_BIND_SHADER_RESOURCE;  // FIXME:

    default:
        AXLOGW("Unknown BufferType: {}", static_cast<uint32_t>(t));
        return static_cast<D3D11_BIND_FLAG>(0);
    }
}

static inline std::size_t alignTo(std::size_t value, std::size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}


/* -------------------------------------------------- ctor */
BufferImpl::BufferImpl(ID3D11Device* device,
                       ID3D11DeviceContext* context,
                       std::size_t size,
                       BufferType type,
                       BufferUsage usage,
                       const void* initial)
    : Buffer(size, type, usage), _device(device), _context(context)
{
    translateUsage(usage, _nativeUsage, _cpuAccess);
    _bindFlag = translateBindFlag(type);

    _capacity = _bindFlag == D3D11_BIND_CONSTANT_BUFFER ? alignTo(size, 16) : size;

    if (initial && size)
        _defaultData.assign(static_cast<const uint8_t*>(initial), static_cast<const uint8_t*>(initial) + size);

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

/* -------------------------------------------------- updateData */
void BufferImpl::updateData(const void* data, std::size_t size)
{
    assert(size <= _size);
    assert(data);

    if (_nativeUsage == D3D11_USAGE_DYNAMIC)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        _context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        std::memcpy(mapped.pData, data, size);
        _context->Unmap(_buffer.Get(), 0);
    }
    else
    {
        _context->UpdateSubresource(_buffer.Get(), 0, nullptr, data, 0, 0);
    }

    if (_needDefaultStoredData)
        _defaultData.assign(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
}

/* -------------------------------------------------- updateSubData */
void BufferImpl::updateSubData(const void* data, std::size_t offset, std::size_t size)
{
    assert(data && (offset + size <= _size));

    if (_nativeUsage == D3D11_USAGE_DYNAMIC)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        _context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped);
        std::memcpy(static_cast<uint8_t*>(mapped.pData) + offset, data, size);
        _context->Unmap(_buffer.Get(), 0);
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
        std::memcpy(_defaultData.data() + offset, data, size);
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

}  // namespace ax::d3d

