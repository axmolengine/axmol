// TODO: consider move textureDesc to base class: rhi::Texture
#pragma once

#include <array>
#include "../Texture.h"
#include "base/EventListenerCustom.h"

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

    void recreateSampler(const SamplerDescriptor& desc);

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

    TextureDescriptor _descriptor;

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
    TextureImpl(ID3D11Device* device, const TextureDescriptor& descriptor);
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
    void updateSamplerDescriptor(const SamplerDescriptor& sampler) override;

    /**
     * Generate mipmaps.
     */
    void generateMipmaps() override;

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    void updateTextureDescriptor(const TextureDescriptor& descriptor, int index = 0) override;

    /**
     * Get texture object.
     * @return Texture object.
     */
    const TextureHandle& internalHandle(int index = 0) const { return _textureRes._textures[index]; }

    ID3D11SamplerState* getSamplerState() const { return _textureRes._samplerState;  }

    const TextureDescriptor& getDesc() const { return _textureRes._descriptor; }

    int getCount() const override { return _textureRes._maxIdx + 1; }

private:
    TextureResource _textureRes;
    EventListener* _rendererRecreatedListener = nullptr;
};

// end of d3d group
/// @}
}  // namespace ax::rhi::d3d
