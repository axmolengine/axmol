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
#include "axmol/rhi/RenderContext.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
class BufferImpl;
class DepthStencilStateImpl;
class RenderPipelineImpl;
class RenderTargetImpl;
class DriverImpl;
class SemaphorePool;

class RenderContextImpl : public RenderContext
{
public:
    // Maximum number of VkCommandBuffer handles managed simultaneously by VulkanCommands.
    //
    // This includes the "current" command buffer that is being written into, as well as any command
    // buffers that have been submitted but have not yet finished rendering. Note that Filament can
    // issue multiple commit calls in a single frame, and that we use a triple buffered swap chain on
    // some platforms.
    //
    // Heuristic: Triple Buffering (3) multiplied by maximum number of renderpasses (15).
    static constexpr int FVK_MAX_COMMAND_BUFFERS     = 3 * 15;
    static constexpr int IMAGE_READY_SEMAPHORE_COUNT = FVK_MAX_COMMAND_BUFFERS;

    static constexpr int VS_UBO_BINDING_INDEX = 0;
    static constexpr int FS_UBO_BINDING_INDEX = 1;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    RenderContextImpl(DriverImpl* driver, VkSurfaceKHR surface);
    ~RenderContextImpl() override;

    bool resizeSwapchain(uint32_t width, uint32_t height) override;

    void setDepthStencilState(DepthStencilState* depthStencilState) override;
    void setRenderPipeline(RenderPipeline* renderPipeline) override;

    bool beginFrame() override;
    void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor) override;
    void updateDepthStencilState(const DepthStencilDesc& descriptor) override;
    void updatePipelineState(const RenderTarget* rt, const PipelineDesc& descriptor) override;

    void setViewport(int x, int y, unsigned int w, unsigned int h) override;
    void setCullMode(CullMode mode) override;
    void setWinding(Winding winding) override;
    void setScissorRect(bool isEnabled, float x, float y, float width, float height) override;

    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;
    void setInstanceBuffer(Buffer* buffer) override;

    void drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe) override;
    void drawArraysInstanced(PrimitiveType primitiveType,
                             std::size_t start,
                             std::size_t count,
                             int instanceCount,
                             bool wireframe) override;
    void drawElements(PrimitiveType primitiveType,
                      IndexFormat indexType,
                      std::size_t count,
                      std::size_t offset,
                      bool wireframe) override;
    void drawElementsInstanced(PrimitiveType primitiveType,
                               IndexFormat indexType,
                               std::size_t count,
                               std::size_t offset,
                               int instanceCount,
                               bool wireframe) override;

    void endRenderPass() override;
    void endFrame() override;

    void readPixels(RenderTarget* rt,
                    bool preserveAxisHint,
                    std::function<void(const PixelBufferDesc&)> callback) override;

    void prepareDrawing();

private:
    void rebuildSwapchain();
    void createCommandBuffers();
    void createDescriptorPool();

    void readPixelsImpl(RenderTarget* rt, bool preserveAxisHint, std::function<void(const PixelBufferDesc&)>& callback);

    DriverImpl* _driver{nullptr};
    VkSurfaceKHR _surface{VK_NULL_HANDLE};
    VkDevice _device{VK_NULL_HANDLE};

    VkQueue _graphicsQueue{VK_NULL_HANDLE};
    VkQueue _presentQueue{VK_NULL_HANDLE};

    VkSwapchainKHR _swapchain{VK_NULL_HANDLE};
    uint32_t _currentImageIndex{0};  // current swapchain image index

    VkCommandPool _commandPool{VK_NULL_HANDLE};

    axstd::pod_vector<VkImage> _swapchainImages;
    axstd::pod_vector<VkImageView> _swapchainImageViews;

    uint32_t _currentFrame{0};
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> _presentCompleteSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> _inFlightFences{};
    std::array<VkDescriptorPool, MAX_FRAMES_IN_FLIGHT> _descriptorPools{};
    axstd::pod_vector<VkSemaphore> _renderFinishedSemaphores;

    axstd::pod_vector<VkWriteDescriptorSet> _descriptorWritesPerFrame;
    axstd::pod_vector<VkDescriptorImageInfo> _descriptorImageInfosPerFrame;

    VkCommandBuffer _currentCmdBuffer{VK_NULL_HANDLE};  // weak pointer

    DepthStencilStateImpl* _depthStencilState{nullptr};
    RenderPipelineImpl* _renderPipeline{nullptr};
    BufferImpl* _vertexBuffer{nullptr};
    BufferImpl* _indexBuffer{nullptr};
    BufferImpl* _instanceBuffer{nullptr};

#pragma region Uniform ring buffer

    // Per-frame uniform ring buffer (persistently mapped)
    struct UniformRingBuffer
    {
        VkBuffer buffer{VK_NULL_HANDLE};
        VkDeviceMemory memory{VK_NULL_HANDLE};
        uint8_t* mapped{nullptr};  // persistently mapped host pointer
        std::size_t capacity{0};   // total bytes
        std::size_t writeHead{0};  // current write offset
        std::size_t align{256};    // device minUniformBufferOffsetAlignment (fallback 256)
        bool isCoherent{true};     // memory coherency hint
    };

    // Allocate an aligned slice in current frame ring
    struct UniformSlice
    {
        std::size_t offset{0};
        std::size_t size{0};
        uint8_t* cpuPtr{nullptr};
    };
    std::array<UniformRingBuffer, MAX_FRAMES_IN_FLIGHT> _uniformRings{};

    // Create ring buffers for all frames-in-flight
    void createUniformRingBuffers(std::size_t capacityBytes);
    // Destroy ring buffers
    void destroyUniformRingBuffers();
    // Reset current frame ring write head (after waiting its fence)
    void resetUniformRingForCurrentFrame();

    UniformSlice allocateUniformSlice(std::size_t size);
#pragma endregion

    std::vector<std::function<void()>> _postFrameOps;

    uint32_t _renderTargetWidth{0};
    uint32_t _renderTargetHeight{0};

    uint32_t _screenWidth{0};
    uint32_t _screenHeight{0};

    VkViewport _cachedViewport{};
    VkRect2D _cachedScissor{};
    VkCullModeFlags _cachedCullMode{};
    VkFrontFace _cachedFrontFace{};
    bool _scissorEnabled{false};

    bool _swapchainDirty{false};
    bool _suboptimal{false};

    bool _inFrame{false};
};
}  // namespace ax::rhi::vk
