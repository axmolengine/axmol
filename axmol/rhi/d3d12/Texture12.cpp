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
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/SamplerCache.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/base/Logging.h"
#include <cassert>
#include <cstring>

namespace ax::rhi::d3d12
{

static constexpr uint32_t LEVEL_INITIAL_CAPS = 16;
static constexpr uint32_t LAYER_INITIAL_CAPS = 8;

static void D3D12TexDescToTexDesc(TextureDesc& td, const D3D12_RESOURCE_DESC& desc)
{
    td.textureType = (desc.DepthOrArraySize == 6 && desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
                         ? TextureType::TEXTURE_CUBE
                         : TextureType::TEXTURE_2D;

    td.width     = static_cast<uint16_t>(desc.Width);
    td.height    = static_cast<uint16_t>(desc.Height);
    td.arraySize = desc.DepthOrArraySize;

    switch (desc.Format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        td.pixelFormat = PixelFormat::RGBA8;
        break;
    case DXGI_FORMAT_R8_UNORM:
        td.pixelFormat = PixelFormat::R8;
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        td.pixelFormat = PixelFormat::D24S8;
        break;
    default:
        td.pixelFormat = PixelFormat::NONE;
        break;
    }

    if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        td.textureUsage = TextureUsage::RENDER_TARGET;
}

static inline UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize)
{
    return MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySize;
}

TextureImpl::TextureImpl(DriverImpl* driver, const TextureDesc& desc)
    : _driver(driver), _stateTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    updateTextureDesc(desc);
}

TextureImpl::TextureImpl(DriverImpl* driver, ComPtr<ID3D12Resource> existingResource)
    : _driver(driver), _stateTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    D3D12_RESOURCE_DESC d3dDesc = existingResource->GetDesc();
    D3D12TexDescToTexDesc(_desc, d3dDesc);
    _nativeTexture.resource = existingResource;
}

TextureImpl::~TextureImpl()
{
    if (_nativeTexture.resource)
        _driver->queueDisposal(_nativeTexture.resource.Detach(), _lastFenceValue);

    if (_nativeTexture.srv)
        _driver->queueDisposal(_nativeTexture.srv, DisposableResource::Type::ShaderResourceView, _lastFenceValue);
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
    bool prepareForCopyDest = data && width > 0 && height > 0 && _desc.textureUsage != TextureUsage::RENDER_TARGET;
    auto resourceState      = ensureNativeTexture(prepareForCopyDest);
    if (!prepareForCopyDest || !resourceState)
        return;

    auto device = _driver->getDevice();

    // Query footprint for the subresource
    const auto texDesc = _nativeTexture.resource->GetDesc();

    assert(xoffset >= 0 && yoffset >= 0);
    assert(width > 0 && height > 0);
    assert(xoffset + width <= static_cast<int>(texDesc.Width));
    assert(yoffset + height <= static_cast<int>(texDesc.Height));

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT numRows;
    UINT64 rowSizeInBytes;
    UINT64 totalBytes;

    device->GetCopyableFootprints(&texDesc, level, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

    // Allocate upload memory from allocator
    auto allocator  = _driver->getUploadBufferAllocator();  // raw pointer
    UploadSpan span = allocator->allocTextureFootprint(totalBytes);

    // Copy data into upload memory row by row
    const UINT bytesPerPixel = ax::rhi::RHIUtils::getBitsPerPixel(_desc.pixelFormat) / 8;
    const UINT rowCopySize   = width * bytesPerPixel;
    const BYTE* srcData      = reinterpret_cast<const BYTE*>(data);
    BYTE* dstPtr             = span.cpuPtr;

    for (int row = 0; row < height; ++row)
    {
        memcpy(dstPtr + row * footprint.Footprint.RowPitch, srcData + row * rowCopySize, rowCopySize);
    }

    // Record copy
    auto& submission = _driver->startIsolateSubmission();

    if (resourceState != D3D12_RESOURCE_STATE_COPY_DEST)
        transitionState(submission, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource        = _nativeTexture.resource.Get();
    dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = D3D12CalcSubresource(level, layerIndex, 0, _desc.mipLevels, _desc.arraySize);

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource              = span.heap;
    src.Type                   = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint        = footprint;
    src.PlacedFootprint.Offset = span.offset;  // use allocator offset

    D3D12_BOX srcBox{};
    srcBox.left   = 0;
    srcBox.top    = 0;
    srcBox.front  = 0;
    srcBox.right  = width;
    srcBox.bottom = height;
    srcBox.back   = 1;

    submission->CopyTextureRegion(&dst, xoffset, yoffset, 0, &src, &srcBox);
    // Transition back to shader-readable state
    transitionState(submission, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    if (shouldGenMipmaps(level))
        generateMipmaps(submission);

    // Submit and get fence value
    _driver->finishIsolateSubmission(submission, true);

    allocator->retireSync(span);
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
    bool prepareForCopyDest = data && width > 0 && height > 0;
    auto resourceState      = ensureNativeTexture(prepareForCopyDest);
    if (!prepareForCopyDest || !resourceState)
        return;

    auto device = _driver->getDevice();

    // Query format descriptor from RHI
    const auto& fmtDesc = ax::rhi::RHIUtils::getFormatDesc(_desc.pixelFormat);
    AXASSERT(fmtDesc.bpp != 0, "Invalid pixel format");
    AXASSERT(fmtDesc.blockWidth > 0 && fmtDesc.blockHeight > 0, "Not a block-compressed format");
    AXASSERT(fmtDesc.blockSize > 0, "Invalid block size");

    // Compute mip dimensions
    const auto texDesc   = _nativeTexture.resource->GetDesc();
    const UINT mipWidth  = std::max(1u, static_cast<UINT>(texDesc.Width >> level));
    const UINT mipHeight = std::max(1u, static_cast<UINT>(texDesc.Height >> level));

    // Device footprint for the target subresource
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    UINT numRows          = 0;
    UINT64 rowSizeInBytes = 0;
    UINT64 totalBytes     = 0;
    device->GetCopyableFootprints(&texDesc, level, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

    // Prepare source box
    D3D12_BOX srcBox{};
    srcBox.front = 0;
    srcBox.back  = 1;

    // --- Handle small mips first ---
    const UINT bw       = fmtDesc.blockWidth;   // typically 4
    const UINT bh       = fmtDesc.blockHeight;  // typically 4
    const UINT bs       = fmtDesc.blockSize;    // DXT1=8, DXT3/DXT5=16, etc.
    const bool smallMip = (mipWidth < bw) || (mipHeight < bh);

    UINT rowCopySize     = 0;
    UINT blockRowsToCopy = 0;

    if (smallMip)
    {
        AXASSERT(xoffset == 0 && yoffset == 0, "Small mip must start at (0,0)");
        AXASSERT(width == mipWidth && height == mipHeight, "Small mip must be uploaded as a whole");

        rowCopySize     = bs;
        blockRowsToCopy = 1;

        srcBox.left   = 0;
        srcBox.top    = 0;
        srcBox.right  = bw;
        srcBox.bottom = bh;
    }
    else
    {
        AXASSERT(xoffset >= 0 && yoffset >= 0, "Negative offsets not allowed");
        AXASSERT(xoffset % bw == 0 && yoffset % bh == 0, "Offsets must be block-aligned for BC formats");

        const UINT alignedWidth  = ((UINT)width + (bw - 1)) / bw * bw;
        const UINT alignedHeight = ((UINT)height + (bh - 1)) / bh * bh;

        const UINT reqMinW = (fmtDesc.minBlockX ? fmtDesc.minBlockX : bw);
        const UINT reqMinH = (fmtDesc.minBlockY ? fmtDesc.minBlockY : bh);
        AXASSERT(alignedWidth >= reqMinW, "Region width smaller than minimum block requirement");
        AXASSERT(alignedHeight >= reqMinH, "Region height smaller than minimum block requirement");

        const UINT mipBlocksW = (mipWidth + bw - 1) / bw;
        const UINT mipBlocksH = (mipHeight + bh - 1) / bh;

        const UINT xBlocks       = static_cast<UINT>(xoffset) / bw;
        const UINT yBlocks       = static_cast<UINT>(yoffset) / bh;
        const UINT blocksToCopyW = alignedWidth / bw;
        const UINT blocksToCopyH = alignedHeight / bh;

        AXASSERT(xBlocks + blocksToCopyW <= mipBlocksW, "Compressed update exceeds mip width (in blocks)");
        AXASSERT(yBlocks + blocksToCopyH <= mipBlocksH, "Compressed update exceeds mip height (in blocks)");

        rowCopySize     = blocksToCopyW * bs;
        blockRowsToCopy = blocksToCopyH;

        if (dataSize != 0)
        {
            const size_t expectedSize = static_cast<size_t>(rowCopySize) * static_cast<size_t>(blockRowsToCopy);
            AXASSERT(dataSize >= expectedSize, "Provided compressed dataSize is smaller than required for sub-region");
        }

        srcBox.left   = 0;
        srcBox.top    = 0;
        srcBox.right  = alignedWidth;
        srcBox.bottom = alignedHeight;
    }

    // Allocate upload memory from allocator
    auto* allocator = _driver->getUploadBufferAllocator();  // raw pointer
    auto span       = allocator->allocTextureFootprint(totalBytes);

    // Copy compressed data row by row
    const BYTE* srcData = reinterpret_cast<const BYTE*>(data);
    BYTE* dstPtr        = span.cpuPtr;
    for (UINT row = 0; row < blockRowsToCopy; ++row)
    {
        BYTE* dstRow       = dstPtr + row * footprint.Footprint.RowPitch;
        const BYTE* srcRow = srcData + row * rowCopySize;
        memcpy(dstRow, srcRow, rowCopySize);
    }

    // Record copy commands
    auto& submission = _driver->startIsolateSubmission();

    if (resourceState != D3D12_RESOURCE_STATE_COPY_DEST)
        transitionState(submission, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource        = _nativeTexture.resource.Get();
    dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = D3D12CalcSubresource(level, layerIndex, 0, _desc.mipLevels, _desc.arraySize);

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource              = span.heap;
    src.Type                   = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint        = footprint;
    src.PlacedFootprint.Offset = span.offset;

    submission->CopyTextureRegion(&dst, xoffset, yoffset, 0, &src, &srcBox);

    transitionState(submission, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    if (shouldGenMipmaps(level))
    {
        AXLOGW(
            "Warning: Compressed textures do not support runtime mipmap generation. "
            "Please upload precomputed mip levels instead.");
    }

    _driver->finishIsolateSubmission(submission, true);
    allocator->retireSync(span);
}

void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    assert(_desc.textureType == TextureType::TEXTURE_CUBE);
    updateSubData(0, 0, _desc.width, _desc.height, 0, data, static_cast<int>(side));
}

void TextureImpl::updateSamplerDesc(const SamplerDesc& samplerDesc)
{
    _desc.samplerDesc = samplerDesc;

    auto samplerCache = SamplerCache::getInstance();
    auto samplerIndex = samplerCache->registerSampler(samplerDesc);
    _sampler          = static_cast<DescriptorHandle*>(samplerCache->getSampler(samplerIndex));
}

void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    _desc = desc;
    Texture::updateTextureDesc(desc);
    updateSamplerDesc(desc.samplerDesc);
}

D3D12_RESOURCE_STATES TextureImpl::ensureNativeTexture(bool prepareForCopyDest)
{
    if (_nativeTexture)
        return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    auto device = _driver->getDevice();

    auto fmtInfo = dxutils::toDxgiFormatInfo(_desc.pixelFormat);
    if (fmtInfo->format == DXGI_FORMAT_UNKNOWN)
    {
        assert(false);
        AXLOGE("axmol: D3D12 does not support pixel format: {}", (int)_desc.pixelFormat);
        return D3D12_RESOURCE_STATE_COMMON;
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
    texDesc.Format             = fmtInfo->format;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    if (shouldGenMipmaps())
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_CLEAR_VALUE clearValue{};
    D3D12_CLEAR_VALUE* pClearValue{nullptr};

    D3D12_RESOURCE_STATES initialResourceState{};
    if (_desc.textureUsage == TextureUsage::RENDER_TARGET) [[unlikely]]
    {
        D3D12_CLEAR_VALUE clearValue{};
        if (_desc.pixelFormat == PixelFormat::D24S8)
        {  // depth-stencil attachment: screen/offscreen render target
            clearValue.Format               = fmtInfo->fmtDsv;
            clearValue.DepthStencil.Depth   = 1.0f;
            clearValue.DepthStencil.Stencil = 0;

            initialResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

            texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }
        else
        {  // color attachment: offscreen render target
            clearValue.Format   = fmtInfo->fmtSrv;
            clearValue.Color[0] = 0.0f;
            clearValue.Color[1] = 0.0f;
            clearValue.Color[2] = 0.0f;
            clearValue.Color[3] = 0.0f;  // default alpha

            initialResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

            texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }

        pClearValue = &clearValue;
    }
    else
    {
        initialResourceState =
            prepareForCopyDest ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }

    // create texture resource
    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, initialResourceState,
                                                 pClearValue, IID_PPV_ARGS(&_nativeTexture.resource));

    // AXLOGD("TextureImpl: Created resource:{} for {}", fmt::ptr(_nativeTexture.resource.Get()), fmt::ptr(this));

    // non depth-stencil texture, we need create SRV for sampling, even through it's textureUsage=RENDER_TARGET
    if (_desc.pixelFormat != PixelFormat::D24S8)
        createShaderResourceView(fmtInfo, mipLevels, arrayLayers, isCube, device);
    assert(SUCCEEDED(hr));

    setKnownState(initialResourceState);

    return initialResourceState;
}

void TextureImpl::createShaderResourceView(const dxutils::PixelFormatInfo* fmtInfo,
                                           uint32_t mipLevels,
                                           uint32_t arrayLayers,
                                           bool isCube,
                                           ID3D12Device* device)
{
    // --- Create SRV for normal texture ---
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = fmtInfo->fmtSrv;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (isCube)
    {
        srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip     = 0;
        srvDesc.TextureCube.MipLevels           = mipLevels;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    }
    else if (arrayLayers > 1)
    {
        srvDesc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip     = 0;
        srvDesc.Texture2DArray.MipLevels           = mipLevels;
        srvDesc.Texture2DArray.FirstArraySlice     = 0;
        srvDesc.Texture2DArray.ArraySize           = arrayLayers;
        srvDesc.Texture2DArray.PlaneSlice          = 0;
        srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
    }
    else
    {
        srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip     = 0;
        srvDesc.Texture2D.MipLevels           = mipLevels;
        srvDesc.Texture2D.PlaneSlice          = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    _nativeTexture.srv = _driver->allocateDescriptor(DisposableResource::Type::ShaderResourceView);
    device->CreateShaderResourceView(_nativeTexture.resource.Get(), &srvDesc, _nativeTexture.srv->cpu);
}

void TextureImpl::generateMipmaps(ID3D12GraphicsCommandList* cmd)
{
    if (_desc.textureUsage == TextureUsage::RENDER_TARGET)
        return;

    auto resource = _nativeTexture.resource.Get();
    if (_driver->generateMipmaps(cmd, resource))
    {
        auto desc          = resource->GetDesc();
        _overrideMipLevels = desc.MipLevels;
    }
}

}  // namespace ax::rhi::d3d12
