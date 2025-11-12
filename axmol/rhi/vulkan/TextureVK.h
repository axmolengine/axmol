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
#include "axmol/base/EventListenerCustom.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
/**
 * @addtogroup _vk
 * @{
 */

class DriverImpl;

/**
 * @brief A TextureHandle holds Vulkan VkImage & VkImageView
 *
 */
struct TextureHandle
{
    explicit operator bool() const { return image != VK_NULL_HANDLE; }

    void destroy(VkDevice device)
    {
        if (view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
        if (image != VK_NULL_HANDLE)
        {
            vkDestroyImage(device, image, nullptr);
            image = VK_NULL_HANDLE;
        }
        if (memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
    }

    TextureHandle detach()
    {
        auto ret = *this;
        image    = VK_NULL_HANDLE;
        view     = VK_NULL_HANDLE;
        memory   = VK_NULL_HANDLE;
        return ret;
    }

    VkImage image{VK_NULL_HANDLE};
    VkImageView view{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
};

/**
 * The texture implementation supports 2D, cubemap, and 2D array textures.
 */
class TextureImpl : public rhi::Texture
{
public:
    /**
     * @param desc Specifies the texture description.
     */
    TextureImpl(DriverImpl*, const TextureDesc& desc);
    TextureImpl(DriverImpl*, VkImage existingImage, VkImageView existingImageView);
    ~TextureImpl();

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
    VkSampler getSampler() const { return _sampler; }
    const TextureDesc& getDesc() const { return _desc; }

private:
    void ensureNativeTexture();
    void generateMipmaps(VkCommandBuffer cmd);

    DriverImpl* _driver{nullptr};  // weak pointer
    TextureHandle _nativeTexture{};
    VkSampler _sampler{VK_NULL_HANDLE};
    TextureDesc _desc{};

    bool _ownResources{false};
    std::atomic<bool> _initialized{false};
};

/** @} */

}  // namespace ax::rhi::vk
