#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/SamplerCache.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/DXUtils.h"
#include "axmol/base/Logging.h"
#include <cassert>
#include <cstring>

namespace ax::rhi::d3d12
{

static constexpr uint32_t LEVEL_INITIAL_CAPS = 16;
static constexpr uint32_t LAYER_INITIAL_CAPS = 8;

TextureImpl::TextureImpl(DriverImpl* driver, const TextureDesc& desc)
    : _driver(driver), _ownResources(true), _stateTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    updateTextureDesc(desc);
}

TextureImpl::TextureImpl(DriverImpl* driver, ComPtr<ID3D12Resource> existingResource)
    : _driver(driver), _ownResources(false), _stateTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    _nativeTexture.resource = existingResource;
}

TextureImpl::~TextureImpl()
{
    if (_ownResources)
    {
        _nativeTexture.reset();
    }
}

void TextureImpl::transitionState(ID3D12GraphicsCommandList* cmd, D3D12_RESOURCE_STATES newState)
{
    auto oldState = _stateTracker.getState(0, 0);
    if (oldState == newState)
        return;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = _nativeTexture.resource.Get();
    barrier.Transition.StateBefore = oldState;
    barrier.Transition.StateAfter  = newState;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    cmd->ResourceBarrier(1, &barrier);
    _stateTracker.setState(0, 0, newState);
}

D3D12_RESOURCE_STATES TextureImpl::getCurrentState() const
{
    return _stateTracker.getState(0, 0);
}

void TextureImpl::setKnownState(D3D12_RESOURCE_STATES state)
{
    _stateTracker.setState(0, 0, state);
}

void TextureImpl::updateData(const void* data, int width, int height, int level, int layerIndex)
{
    updateSubData(0, 0, width, height, level, data, layerIndex);
}

void TextureImpl::updateSubData(int xoffset,
                                int yoffset,
                                int width,
                                int height,
                                int level,
                                const void* data,
                                int layerIndex)
{
    ensureNativeTexture();
    if (!data)
        return;

    // Create upload buffer
    const uint32_t rowPitch   = ax::rhi::RHIUtils::computeRowPitch(_desc.pixelFormat, width);
    const uint32_t slicePitch = rowPitch * height;

    ComPtr<ID3D12Resource> uploadBuffer;
    auto device = _driver->getDevice();

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufDesc{};
    bufDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width            = slicePitch;
    bufDesc.Height           = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels        = 1;
    bufDesc.Format           = DXGI_FORMAT_UNKNOWN;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr =
        device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr, IID_PPV_ARGS(&uploadBuffer));
    assert(SUCCEEDED(hr));

    // Copy data into upload buffer
    void* mapped = nullptr;
    D3D12_RANGE range{0, 0};
    uploadBuffer->Map(0, &range, &mapped);
    std::memcpy(mapped, data, slicePitch);
    uploadBuffer->Unmap(0, nullptr);

    // Record copy
    auto submission = _driver->startIsolateSubmission();

    transitionState(submission, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource        = _nativeTexture.resource.Get();
    dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = level;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = uploadBuffer.Get();
    src.Type      = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    const auto texDesc = _nativeTexture.resource->GetDesc();
    device->GetCopyableFootprints(&texDesc, level, 1, 0, &src.PlacedFootprint, nullptr, nullptr, nullptr);

    // Copy footprints from upload buffer into texture
    submission->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    // Transition back to shader-readable state
    transitionState(submission, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    _driver->finishIsolateSubmission(submission);
}

void TextureImpl::updateCompressedData(const void* data,
                                       int width,
                                       int height,
                                       std::size_t dataSize,
                                       int level,
                                       int layerIndex)
{
    // For simplicity, compressed formats are treated similarly: upload buffer + CopyTextureRegion
    updateCompressedSubData(0, 0, width, height, dataSize, level, data, layerIndex);
}

void TextureImpl::updateCompressedSubData(int xoffset,
                                          int yoffset,
                                          int width,
                                          int height,
                                          std::size_t dataSize,
                                          int level,
                                          const void* data,
                                          int layerIndex)
{
    ensureNativeTexture();
    if (!data || width <= 0 || height <= 0)
        return;

    // Create upload buffer sized to compressed region
    ComPtr<ID3D12Resource> uploadBuffer;
    auto device = _driver->getDevice();

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufDesc{};
    bufDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width            = dataSize;
    bufDesc.Height           = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels        = 1;
    bufDesc.Format           = DXGI_FORMAT_UNKNOWN;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr =
        device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr, IID_PPV_ARGS(&uploadBuffer));
    assert(SUCCEEDED(hr));

    // Copy compressed data into upload buffer
    void* mapped = nullptr;
    D3D12_RANGE range{0, 0};
    uploadBuffer->Map(0, &range, &mapped);
    std::memcpy(mapped, data, dataSize);
    uploadBuffer->Unmap(0, nullptr);

    // Record copy
    auto submission = _driver->startIsolateSubmission();

    transitionState(submission, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource        = _nativeTexture.resource.Get();
    dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = level;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = uploadBuffer.Get();
    src.Type      = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    const auto nativeTexDesc = _nativeTexture.resource->GetDesc();
    device->GetCopyableFootprints(&nativeTexDesc, level, 1, 0, &src.PlacedFootprint, nullptr, nullptr, nullptr);

    submission->CopyTextureRegion(&dst, xoffset, yoffset, 0, &src, nullptr);

    transitionState(submission, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    _driver->finishIsolateSubmission(submission);
}

void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    assert(_desc.textureType == TextureType::TEXTURE_CUBE);
    updateSubData(0, 0, _desc.width, _desc.height, 0, data, static_cast<int>(side));
}

void TextureImpl::updateSamplerDesc(const SamplerDesc& sampler)
{
    _desc.samplerDesc = sampler;
    // In D3D12, sampler is created via SamplerCache into a descriptor heap
    // Here we just cache the desc; actual SRV/ sampler creation is handled elsewhere
}

void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    _desc = desc;
    Texture::updateTextureDesc(desc);
    updateSamplerDesc(desc.samplerDesc);
}

void TextureImpl::ensureNativeTexture()
{
    if (_nativeTexture)
        return;

    auto device = _driver->getDevice();

    DXGI_FORMAT dxgiFmt = dxutils::toDxgiFormatInfo(_desc.pixelFormat)->fmtSrv;
    if (dxgiFmt == DXGI_FORMAT_UNKNOWN)
    {
        AXLOGE("axmol: D3D12 does not support pixel format: {}", (int)_desc.pixelFormat);
        return;
    }

    const bool isCube = (_desc.textureType == TextureType::TEXTURE_CUBE);
    const UINT mipLevels =
        (_desc.mipLevels != 0) ? _desc.mipLevels : ax::rhi::RHIUtils::computeMipLevels(_desc.width, _desc.height);
    const UINT arrayLayers = isCube ? 6u : static_cast<UINT>(_desc.arraySize);

    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment          = 0;
    texDesc.Width              = _desc.width;
    texDesc.Height             = _desc.height;
    texDesc.DepthOrArraySize   = arrayLayers;
    texDesc.MipLevels          = mipLevels;
    texDesc.Format             = dxgiFmt;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    if (_desc.textureUsage == TextureUsage::RENDER_TARGET)
    {
        if (_desc.pixelFormat != PixelFormat::D24S8)
            texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        else
            texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr =
        device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON,
                                        nullptr, IID_PPV_ARGS(&_nativeTexture.resource));
    assert(SUCCEEDED(hr));
}

void TextureImpl::generateMipmaps(ID3D12GraphicsCommandList* cmd)
{
    // D3D12 does not provide automatic mipmap generation.
    // You need to implement a custom compute shader or blit path.
    AXLOGW("generateMipmaps: Not implemented in D3D12. Please precompute mip levels.");
}

}  // namespace ax::rhi::d3d12
