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

#include "axmol/rhi/DriverBase.h"
#include <glad/vulkan.h>
#include <optional>
#include <string>
#include <mutex>
#include <deque>

namespace ax::rhi::vk
{

class RenderContextImpl;
class DriverImpl;
class TextureImpl;

struct DisposableResource
{
    enum class Type
    {
        Sampler,
        Image,
        ImageView,
        Buffer,
        Memory
    };
    uint32_t frameMask;
    Type type;
    union
    {
        VkSampler sampler;
        VkImage image;
        VkImageView view;
        VkBuffer buffer;
        VkDeviceMemory memory;
    };
};

struct IsolateSubmission
{
    VkCommandBuffer cmd{VK_NULL_HANDLE};
    VkFence fence{VK_NULL_HANDLE};

    operator VkCommandBuffer() { return cmd; }
};

using CreateSurfaceFunc = std::function<VkResult(VkInstance, void* window, VkSurfaceKHR* surface)>;
struct SurfaceCreateInfo
{
    void* window{};
    int width{0};
    int height{0};
    CreateSurfaceFunc createFunc{};
};

class DriverImpl : public DriverBase
{
    friend class RenderContextImpl;

public:
    static constexpr uint32_t MAX_VERTEX_ATTRIBS            = 16;
    static constexpr uint32_t VBO_BINDING_INDEX_START       = 0;
    static constexpr uint32_t VBO_INSTANCING_BINDING_INDEX  = VBO_BINDING_INDEX_START + 1;
    static constexpr uint32_t DEFAULT_ATTRIBS_BINDING_INDEX = VBO_BINDING_INDEX_START + MAX_VERTEX_ATTRIBS;

    DriverImpl();
    ~DriverImpl();

    void init();

    bool recreateSurface(const SurfaceCreateInfo& info);
    VkSurfaceKHR getSurface() const { return _surface; }

    const VkExtent2D& getSurfaceInitialExtent() const { return _surfaceInitalExtent; }

    RenderContext* createRenderContext(void* surfaceContext) override;
    Buffer* createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial) override;
    Texture* createTexture(const TextureDesc& descriptor) override;
    RenderTarget* createDefaultRenderTarget() override;
    RenderTarget* createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment) override;
    DepthStencilState* createDepthStencilState() override;
    RenderPipeline* createRenderPipeline() override;
    Program* createProgram(std::string_view vertexShader, std::string_view fragmentShader) override;
    VertexLayout* createVertexLayout(VertexLayoutDesc&& desc) override;

    std::string getVendor() const override;
    std::string getRenderer() const override;
    std::string getVersion() const override;
    std::string getShaderVersion() const override;

    bool checkForFeatureSupported(FeatureType feature) override;

    void cleanPendingResources() override;

    VkPhysicalDevice getPhysical() const { return _physical; }
    VkDevice getDevice() const { return _device; }

    VkQueue getGraphicsQueue() const { return _graphicsQueue; }
    uint32_t getGraphicsQueueFamily() const { return _graphicsQueueFamily; }

    VkQueue getPresentQueue() const { return _presentQueue; }
    uint32_t getPresentQueueFamily() const { return _presentQueueFamily; }

    // Find suitable memory type index
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    VkResult allocateCommandBuffers(VkCommandBuffer* cmds, uint32_t count);
    void freeCommandBuffers(VkCommandBuffer* cmds, uint32_t count);

    IsolateSubmission startIsolateSubmission()
    {
        auto submission = allocateIsolateSubmission();
        beginRecordingIsolateSubmission(submission);
        return submission;
    }

    void finishIsolateSubmission(IsolateSubmission& submission)
    {
        commitIsolateSubmission(submission);
        freeIsolateSubmission(submission);
    }

    IsolateSubmission allocateIsolateSubmission();
    void freeIsolateSubmission(IsolateSubmission& submission);

    void beginRecordingIsolateSubmission(const IsolateSubmission& submission);
    void commitIsolateSubmission(const IsolateSubmission& submission);

    void destroyFramebuffer(VkFramebuffer);
    void destroyRenderPass(VkRenderPass);

    void queueDisposal(VkImage image);
    void queueDisposal(VkImageView view);
    void queueDisposal(VkBuffer buffer);
    void queueDisposal(VkDeviceMemory memory);
    void queueDisposal(VkSampler sampler);

    void processDisposalQueue(uint32_t completedMask);

    void rebuildSwapchainAttachments(const axstd::pod_vector<VkImage>& images,
                                     const axstd::pod_vector<VkImageView>&,
                                     const VkExtent2D&,
                                     PixelFormat imagePF);

    void destroySwapchainAttachments();

    void setSwapchainCurrentImageIndex(uint32_t imageIndex);

    // Get the current swapchain color attachment by recorded swapchain image index
    TextureImpl* getSwapchainColorAttachment();

    // Get the current swapchain depth-stencil attachment
    TextureImpl* getSwapchainDepthStencilAttachment();

    void waitDeviceIdle() { vkDeviceWaitIdle(_device); }

protected:
    void queueDisposalInternal(DisposableResource&& res);
    ShaderModule* createShaderModule(ShaderStage stage, std::string_view source) override;
    SamplerHandle createSampler(const SamplerDesc& desc) override;
    void destroySampler(SamplerHandle& h) override;

private:
    void initializeFactory();
    void initializeDevice();

    RenderContextImpl* _lastRenderContext{nullptr};

    VkDebugUtilsMessengerCreateInfoEXT _debugCreateInfo{};
    VkDebugUtilsMessengerEXT _debugMessenger{VK_NULL_HANDLE};

    VkInstance _factory{VK_NULL_HANDLE};
    VkPhysicalDevice _physical{VK_NULL_HANDLE};
    VkDevice _device{VK_NULL_HANDLE};
    VkSurfaceKHR _surface{VK_NULL_HANDLE};
    VkExtent2D _surfaceInitalExtent{};

    VkQueue _graphicsQueue{VK_NULL_HANDLE};
    VkQueue _presentQueue{VK_NULL_HANDLE};

    // command pool
    VkCommandPool _commandPool{VK_NULL_HANDLE};
    std::mutex _commandPoolMutex;

    axstd::pod_vector<DisposableResource> _disposalQueue;

    uint32_t _graphicsQueueFamily{0};
    uint32_t _presentQueueFamily{0};

    std::string _vendor;
    std::string _renderer;
    std::string _version;
    std::string _shaderVersion;

    // store and provide swapchain render target attachments
    axstd::pod_vector<TextureImpl*> _swapchainColorAttachments;
    uint32_t _currentSwapchainImageIndex          = 0;
    TextureImpl* _swapchainDepthStencilAttachment = nullptr;
};

}  // namespace ax::rhi::vk
