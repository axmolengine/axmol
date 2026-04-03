/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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
#include "axmol/rhi/metal/DriverMTL.h"
#include <unordered_map>

namespace ax::rhi::mtl
{

class RenderPipelineImpl;
class DepthStencilStateImpl;
class RenderTargetImpl;

/**
 * @addtogroup _metal
 * @{
 */

/**
 * @brief Store encoded commands for the GPU to execute.
 * A command buffer stores encoded commands until the buffer is committed for execution by the GPU
 */
class RenderContextImpl : public RenderContext
{
public:
    /// @name Constructor, Destructor and Initializers
    /**
     * @param driver The device for which MTLCommandQueue object was created.
     */
    RenderContextImpl(DriverImpl* driver, SurfaceHandle surface);
    ~RenderContextImpl();

    RenderTarget* getScreenRenderTarget() const override { return (RenderTarget*)_screenRT; }

    bool updateSurface(SurfaceHandle surface, uint32_t width, uint32_t height) override;

    /**
     * Set depthStencil status
     * @param depthStencilState Specifies the depth and stencil status
     */
    void setDepthStencilState(DepthStencilState* depthStencilState) override;

    /**
     * Sets the current render pipeline state object once
     * @param renderPipeline An object that contains the graphics functions and configuration state used in a render
     * pass.
     */
    void setRenderPipeline(RenderPipeline* renderPipeline) override;

    /// @name Setters & Getters
    /**
     * @brief Indicate the begining of a frame
     * Wait until the inflight command buffer has completed its work.
     * Then create MTLCommandBuffer and enqueue it to MTLCommandQueue.
     * Then start schedule available MTLBuffer
     */
    bool beginFrame() override;

    /**
     * Create a MTLRenderCommandEncoder object for graphics rendering to an attachment in a RenderPassDesc.
     * MTLRenderCommandEncoder is cached if current RenderPassDesc is identical to previous one.
     * @param descriptor Specifies a group of render targets that hold the results of a render pass.
     */
    void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor) override;

    /**
     * Update depthStencil status, improvment: for metal backend cache it
     * @param depthStencilState Specifies the depth and stencil status
     */
    void updateDepthStencilState(const DepthStencilDesc& descriptor) override;

    /**
     * Update render pipeline status
     * Building a programmable pipeline involves an expensive evaluation of GPU state.
     * So a new render pipeline object will be created only if it hasn't been created before.
     * @param rt Specifies the render target.
     * @param desc Specifies the pipeline descriptor.
     */
    void updatePipelineState(const RenderTarget* rt, const PipelineDesc& desc, PrimitiveType primitiveType) override;

    /**
     * Fixed-function state
     * @param x The x coordinate of the upper-left corner of the viewport.
     * @param y The y coordinate of the upper-left corner of the viewport.
     * @param w The width of the viewport, in pixels.
     * @param h The height of the viewport, in pixels.
     */
    void setViewport(int x, int y, unsigned int w, unsigned int h) override;

    /**
     * Fixed-function state
     * @param mode Controls if primitives are culled when front facing, back facing, or not culled at all.
     */
    void setCullMode(CullMode mode) override;

    /**
     * Fixed-function state
     * @param winding The winding order of front-facing primitives.
     */
    void setWinding(Winding winding) override;

    /**
     * Set a global buffer for all vertex shaders at the given bind point index 0.
     * @param buffer The buffer to set in the buffer argument table.
     */
    void setVertexBuffer(Buffer* buffer) override;

    /**
     * Set indexes when drawing primitives with index list
     * @ buffer A buffer object that the device will read indexes from.
     * @ see `drawElements(IndexFormat indexType, unsigned int count, unsigned int offset)`
     */
    void setIndexBuffer(Buffer* buffer) override;

    void setInstanceBuffer(Buffer* buffer) override;

    /**
     * Draw primitives without an index list.
     * @param start For each instance, the first index to draw
     * @param count For each instance, the number of indexes to draw
     * @see `drawElements(IndexFormat indexType, unsigned int count, unsigned int offset)`
     */
    void drawArrays(std::size_t start, std::size_t count, bool wireframe) override;
    void drawArraysInstanced(std::size_t start, std::size_t count, int instanceCount, bool wireframe = false) override;

    /**
     * Draw primitives with an index list.
     * @param indexType The type if indexes, either 16 bit integer or 32 bit integer.
     * @param count The number of indexes to read from the index buffer for each instance.
     * @param offset Byte offset within indexBuffer to start reading indexes from.
     * @see `setIndexBuffer(Buffer* buffer)`
     * @see `drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)`
     */
    void drawElements(IndexFormat indexType, std::size_t count, std::size_t offset, bool wireframe) override;
    void drawElementsInstanced(IndexFormat indexType,
                               std::size_t count,
                               std::size_t offset,
                               int instanceCount,
                               bool wireframe = false) override;

    /**
     * Do some resources release.
     */
    void endRenderPass() override;

    /**
     * Present a drawable and commit a command buffer so it can be executed as soon as possible.
     */
    void endFrame() override;

    void endEncoding();

    /**
     * Fixed-function state
     * @param x, y Specifies the lower left corner of the scissor box
     * @param wdith Specifies the width of the scissor box
     * @param height Specifies the height of the scissor box
     */
    void setScissorRect(bool isEnabled, float x, float y, float width, float height) override;

    void readPixels(RenderTarget* rt,
                    bool preserveAxisHint,
                    std::function<void(const PixelBufferDesc&)> callback) override;

    id<MTLRenderCommandEncoder> getRenderCommandEncoder() const { return _mtlRenderEncoder; }

    CAMetalLayer* getMetalLayer() { return _mtlLayer; }

    id<CAMetalDrawable> acquireDrawable();

protected:
    /**
     * Read a block of pixels from the given texture
     * @param texture Specifies the texture to get the image.
     * @param origX,origY Specify the window coordinates of the first pixel that is read from the given texture. This
     * location is the lower left corner of a rectangular block of pixels.
     * @param rectWidth,rectHeight Specify the dimensions of the pixel rectangle. rectWidth and rectHeight of one
     * correspond to a single pixel.
     * @param pbd, the output buffer for fill texels data
     * @remark: !!!this function only can call after endFrame, then it's could be works well.
     */
    void readPixels(id<MTLTexture> texture,
                    std::size_t origX,
                    std::size_t origY,
                    std::size_t rectWidth,
                    std::size_t rectHeight,
                    PixelBufferDesc& pbd);

    /**
     * This property controls whether or not the drawables'
     * metal textures may only be used for framebuffer attachments (YES) or
     * whether they may also be used for texture sampling and pixel
     * read/write operations (NO).
     * @param frameBufferOnly A value of YES allows CAMetalLayer to allocate the MTLTexture objects in ways that are
     * optimized for display purposes that makes them unsuitable for sampling. The recommended value for most
     * applications is YES.
     * @note This interface is specificaly designed for metal.
     */
    void setFrameBufferOnly(bool frameBufferOnly) override;

    void releaseDrawable();

private:
    void prepareDrawing() const;
    void setTextures() const;
    void setUniformBuffer() const;
    void afterDraw();
    void flush();
    void flushCaptureCommands();

    CAMetalLayer* _mtlLayer{nil};
    RenderTargetImpl* _screenRT{nullptr};
    id<CAMetalDrawable> _currentDrawable{nil};

    // weak ref, like context, managed by DriverImpl
    id<MTLCommandQueue> _mtlCmdQueue              = nil;
    id<MTLCommandBuffer> _currentCmdBuffer        = nil;
    id<MTLRenderCommandEncoder> _mtlRenderEncoder = nil;
    id<MTLBuffer> _mtlIndexBuffer                 = nil;

    DepthStencilStateImpl* _depthStencilState = nullptr;
    RenderPipelineImpl* _renderPipeline       = nullptr;

    MTLPrimitiveType _primitiveType = MTLPrimitiveTypeTriangle;

    unsigned int _renderTargetWidth  = 0;
    unsigned int _renderTargetHeight = 0;

    dispatch_semaphore_t _frameBoundarySemaphore;
    RenderPassDesc _currentRenderPassDesc;
    NSAutoreleasePool* _autoReleasePool = nil;

    std::vector<std::pair<Texture*, std::function<void(const PixelBufferDesc&)>>> _captureCallbacks;
};

// end of _metal group
/// @}
}  // namespace ax::rhi::mtl
