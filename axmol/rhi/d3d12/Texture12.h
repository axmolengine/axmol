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

#include "axmol/rhi/Texture.h"
#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"
#include "axmol/rhi/DXUtils.h"
#include <d3d12.h>
#include <vector>
#include "axmol/tlx/vector.hpp"

namespace ax::rhi::d3d12
{
class DriverImpl;
struct DescriptorHandle;

/**
 * @brief A TextureHandle holds D3D12 resource
 */
struct TextureHandle
{
    ComPtr<ID3D12Resource> resource;
    DescriptorHandle* srv;  // SRV for color

    explicit operator bool() const { return resource != nullptr; }
};

/**
 * @brief Track resource states per mip/array slice
 */
class ResourceStateTracker
{
public:
    ResourceStateTracker(size_t levelCap, size_t layerCap)
    {
        _states.resize(levelCap);
        for (auto& v : _states)
            v.resize(layerCap, D3D12_RESOURCE_STATE_COMMON);
    }

    D3D12_RESOURCE_STATES getState(uint32_t level, uint32_t layer) const
    {
        if (level < _states.size() && layer < _states[level].size())
            return _states[level][layer];
        return D3D12_RESOURCE_STATE_COMMON;
    }

    void setState(uint32_t level, uint32_t layer, D3D12_RESOURCE_STATES state)
    {
        if (level >= _states.size())
            _states.resize(level + 1);
        if (layer >= _states[level].size())
            _states[level].resize(layer + 1, D3D12_RESOURCE_STATE_COMMON);
        _states[level][layer] = state;
    }

private:
    std::vector<tlx::pod_vector<D3D12_RESOURCE_STATES>> _states;
};

/**
 * @brief D3D12 texture implementation (2D, cube, array)
 */
class TextureImpl : public rhi::Texture
{
public:
    TextureImpl(DriverImpl*, const TextureDesc& desc);
    TextureImpl(DriverImpl*, ComPtr<ID3D12Resource> existingResource);
    ~TextureImpl();

    void transitionState(ID3D12GraphicsCommandList* cmd, D3D12_RESOURCE_STATES newState);

    D3D12_RESOURCE_STATES getCurrentState() const;
    void setKnownState(D3D12_RESOURCE_STATES state);

    void updateData(const void* data, int width, int height, int level, int layerIndex = 0) override;
    void updateCompressedData(const void* data,
                              int width,
                              int height,
                              std::size_t dataSize,
                              int level,
                              int layerIndex = 0) override;

    void updateSubData(int xoffset, int yoffset, int width, int height, int level, const void* data, int layerIndex = 0)
        override;
    void updateCompressedSubData(int xoffset,
                                 int yoffset,
                                 int width,
                                 int height,
                                 std::size_t dataSize,
                                 int level,
                                 const void* data,
                                 int layerIndex = 0) override;

    void updateFaceData(TextureCubeFace side, const void* data) override;

    void updateSamplerDesc(const SamplerDesc& sampler) override;
    void updateTextureDesc(const TextureDesc& desc) override;

    const TextureHandle& internalHandle() const { return _nativeTexture; }
    const TextureDesc& getDesc() const { return _desc; }

    DescriptorHandle* getSampler() const { return _sampler; }

    void setLastFenceValue(uint64_t value) { _lastFenceValue = value; }
    uint64_t getLastFenceValue() const { return _lastFenceValue; }

private:
    D3D12_RESOURCE_STATES ensureNativeTexture(bool prepareForCopyDest);
    void createShaderResourceView(const dxutils::PixelFormatInfo* fmtInfo,
                                  uint32_t mipLevels,
                                  uint32_t arrayLayers,
                                  bool isCube,
                                  ID3D12Device* device);
    void generateMipmaps(ID3D12GraphicsCommandList* cmd);

    uint64_t _lastFenceValue{0};
    DriverImpl* _driver{nullptr};  // weak pointer
    ResourceStateTracker _stateTracker;
    TextureHandle _nativeTexture{};
    TextureDesc _desc{};
    DescriptorHandle* _sampler{nullptr};  // weak ref
};

}  // namespace ax::rhi::d3d12
