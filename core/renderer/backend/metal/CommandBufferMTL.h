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

#include "../CommandBuffer.h"
#include "DriverMTL.h"
#include <unordered_map>

NS_AX_BACKEND_BEGIN

class RenderPipelineMTL;
class DepthStencilStateMTL;

/**
 * @addtogroup _metal
 * @{
 */

/**
 * @brief Store encoded commands for the GPU to execute.
 * A command buffer stores encoded commands until the buffer is committed for execution by the GPU
 */
class CommandBufferMTL : public CommandBuffer
{
public:
    /// @name Constructor, Destructor and Initializers
    /**
     * @param driver The device for which MTLCommandQueue object was created.
     */
    CommandBufferMTL(DriverMTL* driver);
    ~CommandBufferMTL();

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
     * Create a MTLRenderCommandEncoder object for graphics rendering to an attachment in a RenderPassDescriptor.
     * MTLRenderCommandEncoder is cached if current RenderPassDescriptor is identical to previous one.
     * @param descriptor Specifies a group of render targets that hold the results of a render pass.
     */
    void beginRenderPass(const RenderTarget* renderTarget, const RenderPassDescriptor& descriptor) override;

    /**
     * Update depthStencil status, improvment: for metal backend cache it
     * @param depthStencilState Specifies the depth and stencil status
     */
    void updateDepthStencilState(const DepthStencilDescriptor& descriptor) override;

    /**
     * Update render pipeline status
     * Building a programmable pipeline involves an expensive evaluation of GPU state.
     * So a new render pipeline object will be created only if it hasn't been created before.
     * @param rt Specifies the render target.
     * @param pipelineDescriptor Specifies the pipeline descriptor.
     */
    void updatePipelineState(const RenderTarget* rt, const PipelineDescriptor& descriptor) override;

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
     * Set the uniform data at a given vertex and fragment buffer binding point 1
     * Set a global texture for all vertex and fragment shaders at the given bind location.
     * @param programState A programState object that hold the uniform and texture data.
     */
    void setProgramState(ProgramState* programState) override;

    /**
     * Set indexes when drawing primitives with index list
     * @ buffer A buffer object that the device will read indexes from.
     * @ see `drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)`
     */
    void setIndexBuffer(Buffer* buffer) override;
    
    void setInstanceBuffer(Buffer* buffer) override;

    /**
     * Draw primitives without an index list.
     * @param primitiveType The type of primitives that elements are assembled into.
     * @param start For each instance, the first index to draw
     * @param count For each instance, the number of indexes to draw
     * @see `drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)`
	 *
	 * TODO: Implement a wireframe mode for METAL devices. Refer to: https://forums.ogre3d.org/viewtopic.php?t=95089
     */
    void drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe) override;

    /**
     * Draw primitives with an index list.
     * @param primitiveType The type of primitives that elements are assembled into.
     * @param indexType The type if indexes, either 16 bit integer or 32 bit integer.
     * @param count The number of indexes to read from the index buffer for each instance.
     * @param offset Byte offset within indexBuffer to start reading indexes from.
     * @see `setIndexBuffer(Buffer* buffer)`
     * @see `drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)`
	 *
	 * TODO: Implement a wireframe mode for METAL devices. Refer to: https://forums.ogre3d.org/viewtopic.php?t=95089
     */
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

    /**
     * Read pixels from RenderTarget
     * @param callback A callback to deal with pixel data read.
     */
    void readPixels(RenderTarget* rt, std::function<void(const PixelBufferDescriptor&)> callback) override;
    
    id<MTLRenderCommandEncoder> getRenderCommandEncoder() const { return _mtlRenderEncoder; }

    id<MTLCommandBuffer> getMTLCommandBuffer() const { return _mtlCommandBuffer; }

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
    static void readPixels(TextureBackend* texture,
                           std::size_t origX,
                           std::size_t origY,
                           std::size_t rectWidth,
                           std::size_t rectHeight,
                           PixelBufferDescriptor& pbd);
    static void readPixels(id<MTLTexture> texture,
                           std::size_t origX,
                           std::size_t origY,
                           std::size_t rectWidth,
                           std::size_t rectHeight,
                           PixelBufferDescriptor& pbd);

private:
    void prepareDrawing() const;
    void setTextures() const;
    void doSetTextures(bool isVertex) const;
    void setUniformBuffer() const;
    void afterDraw();
    void flush();
    void flushCaptureCommands();
    void updateRenderCommandEncoder(const RenderTarget* renderTarget, const RenderPassDescriptor& renderPassParams);

    id<MTLCommandBuffer> _mtlCommandBuffer        = nil;
    id<MTLCommandQueue> _mtlCommandQueue          = nil;
    id<MTLRenderCommandEncoder> _mtlRenderEncoder = nil;
    id<MTLBuffer> _mtlIndexBuffer                 = nil;
    id<MTLTexture> _drawableTexture               = nil;

    DepthStencilStateMTL* _depthStencilStateMTL = nullptr;
    RenderPipelineMTL* _renderPipelineMTL       = nullptr;
    ProgramState* _programState                 = nullptr;

    unsigned int _renderTargetWidth  = 0;
    unsigned int _renderTargetHeight = 0;

    dispatch_semaphore_t _frameBoundarySemaphore;
    const RenderTarget* _currentRenderTarget = nil;  // weak ref
    RenderPassDescriptor _currentRenderPassDesc;
    TargetBufferFlags _currentRenderTargetFlags = TargetBufferFlags::NONE;
    NSAutoreleasePool* _autoReleasePool         = nil;

    std::vector<std::pair<TextureBackend*, std::function<void(const PixelBufferDescriptor&)>>> _captureCallbacks;
};

// end of _metal group
/// @}
NS_AX_BACKEND_END
