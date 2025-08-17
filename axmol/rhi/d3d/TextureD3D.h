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

#include <array>
#include "axmol/rhi/Texture.h"
#include "axmol/base/EventListenerCustom.h"

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <array> // std::array

namespace ax::rhi::d3d
{
using namespace Microsoft::WRL;

struct TextureHandle
{
    explicit operator bool() const { return tex2d != nullptr; }

    void Release()
    {
        SafeRelease(srv);
        SafeRelease(tex2d);
    }

    ID3D11Texture2D* tex2d{};
    ID3D11ShaderResourceView* srv{}; // Note: default color attachment not create srv yet.
};

/**
 * Store texture resource.
 */
struct TextureResource
{
    TextureResource(ID3D11Device* d3d11device)
    {
        _device = d3d11device;
        _textures.fill(TextureHandle{});
    }
    ~TextureResource() { destroy(); }

    template <typename _Fty>
    void foreachTextures(const _Fty& cb)
    {
        int idx = 0;
        while (_textures[idx])
        {
            auto& h = _textures[idx];
            cb(h, idx++);
        }
    }

    TextureHandle createTexture(UINT mipLevels);

    TextureHandle ensure(int index);

    void destroy()
    {
        foreachTextures([this](TextureHandle& tex, int) {
            if (tex)
                tex.Release();
        });
        _textures.fill(TextureHandle{});
    }

    const TextureHandle& getTexture(int index) const { return _textures[index]; }

    ID3D11Device* _device;

    TextureDesc _desc;

    std::array<TextureHandle, AX_META_TEXTURES + 1> _textures;
    ID3D11SamplerState* _samplerState = nullptr;
    int _maxIdx                       = 0;
    UINT _mipLevels                   = -1; // means only 1 layer
};

/**
 * @addtogroup _d3d
 * @{
 */

/**
 * TextureImpl
 */
class TextureImpl : public rhi::Texture
{
public:
    /**
     * @param descriptor Specifies the texture description.
     */
    TextureImpl(ID3D11Device* device, const TextureDesc& descriptor);
    TextureImpl(ID3D11Device* device, ID3D11Texture2D* texture);
    ~TextureImpl();

    /**
     * Update a two-dimensional texture image
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    void updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index = 0) override;

    /**
     * Update a two-dimensional texture image in a compressed format
     * @param data Specifies a pointer to the compressed image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param dataLen Specifies the totoal size of compressed image in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    void updateCompressedData(uint8_t* data,
                              std::size_t width,
                              std::size_t height,
                              std::size_t dataLen,
                              std::size_t level,
                              int index = 0) override;

    /**
     * Update a two-dimensional texture subimage
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the image data in memory.
     */
    void updateSubData(std::size_t xoffset,
                       std::size_t yoffset,
                       std::size_t width,
                       std::size_t height,
                       std::size_t level,
                       uint8_t* data,
                       int index = 0) override;

    /**
     * Update a two-dimensional texture subimage in a compressed format
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param dataLen Specifies the totoal size of compressed subimage in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the compressed image data in memory.
     */
    void updateCompressedSubData(std::size_t xoffset,
                                 std::size_t yoffset,
                                 std::size_t width,
                                 std::size_t height,
                                 std::size_t dataLen,
                                 std::size_t level,
                                 uint8_t* data,
                                 int index = 0) override;

    /**
     * Update texutre cube data in give slice side.
     * @param side Specifies which slice texture of cube to be update.
     * @param data Specifies a pointer to the image data in memory.
     */
    void updateFaceData(TextureCubeFace side, void* data, int index) override;

    /**
     * Update sampler
     * @param sampler Specifies the sampler descriptor.
     */
    void updateSamplerDesc(const SamplerDesc& sampler) override;

    /**
     * Generate mipmaps.
     */
    void generateMipmaps() override;

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    void updateTextureDesc(const TextureDesc& descriptor, int index = 0) override;

    /**
     * Get texture object.
     * @return Texture object.
     */
    const TextureHandle& internalHandle(int index = 0) const { return _textureRes._textures[index]; }

    ID3D11SamplerState* getSamplerState() const { return _textureRes._samplerState;  }

    const TextureDesc& getDesc() const { return _textureRes._desc; }

    int getCount() const override { return _textureRes._maxIdx + 1; }

private:
    TextureResource _textureRes;
    EventListener* _rendererRecreatedListener = nullptr;
};

// end of d3d group
/// @}
}  // namespace ax::rhi::d3d
