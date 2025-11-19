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

#include <cstdint>

#include "axmol/base/Object.h"
#include "axmol/base/Data.h"

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/RenderPassDesc.h"
#include "axmol/rhi/PixelBufferDesc.h"
#include "axmol/platform/StdC.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/VertexLayout.h"

#include <memory>
#include <vector>

namespace ax
{
struct PipelineDesc;
}

namespace ax::rhi
{

class RenderPass;
class RenderPipeline;
class Buffer;
class DepthStencilState;
class Texture;
class RenderTarget;
struct DepthStencilDesc;

/**
 * @addtogroup _rhi
 * @{
 */

/**
 * @brief Store encoded commands for the GPU to execute.
 * A Render Context stores encoded commands until the buffer is committed for execution by the GPU
 */
class RenderContext : public ax::Object
{
public:
    /**
     * @brief Set the Screen Render Target object
     *
     * @param rt
     */
    void setScreenRenderTarget(RenderTarget* rt) { _screenRT = rt; }

    /**
     * @brief Updates surface when the window resized or surface recretad
     * @param surface
     * @param width
     * @param height
     * @return true
     * @return false
     */
    virtual bool updateSurface(void* surface, uint32_t width, uint32_t height);

    /**
     * Set depthStencil status once
     * @param depthStencilState Specifies the depth and stencil status
     */
    virtual void setDepthStencilState(DepthStencilState* depthStencilState) = 0;

    /**
     * Sets the current render pipeline state object once
     * @param renderPipeline An object that contains the graphics functions and configuration state used in a render
     * pass.
     */
    virtual void setRenderPipeline(RenderPipeline* renderPipeline) = 0;

    /// @name Setters & Getters
    /**
     * @brief Indicate the begining of a frame
     */
    virtual bool beginFrame() = 0;

    /**
     * Begin a render pass, initial color, depth and stencil attachment.
     * @param desc Specifies a group of render targets that hold the results of a render pass.
     */
    virtual void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& desc) = 0;

    /**
     * Update depthStencil status, improvment: for metal backend cache it
     * @param depthStencilState Specifies the depth and stencil status
     */
    virtual void updateDepthStencilState(const DepthStencilDesc& desc) = 0;

    /**
     * Update render pipeline status
     * Building a programmable pipeline involves an expensive evaluation of GPU state.
     * So a new render pipeline object will be created only if it hasn't been created before.
     * @param rt Specifies the render target.
     * @param pipelineDesc Specifies the pipeline desc.
     */
    virtual void updatePipelineState(const RenderTarget* rt, const PipelineDesc& desc) = 0;

    /**
     * Fixed-function state
     * @param x The x coordinate of the upper-left corner of the viewport.
     * @param y The y coordinate of the upper-left corner of the viewport.
     * @param w The width of the viewport, in pixels.
     * @param h The height of the viewport, in pixels.
     */
    virtual void setViewport(int x, int y, unsigned int w, unsigned int h) = 0;

    /**
     * Fixed-function state
     * @param mode Controls if primitives are culled when front facing, back facing, or not culled at all.
     */
    virtual void setCullMode(CullMode mode) = 0;

    /**
     * Fixed-function state
     * @param winding The winding order of front-facing primitives.
     */
    virtual void setWinding(Winding winding) = 0;

    /**
     * Set a global buffer for all vertex shaders at the given bind point index 0.
     * @param buffer The vertex buffer to be setted in the buffer argument table.
     */
    virtual void setVertexBuffer(Buffer* buffer) = 0;

    /**
     * Set indexes when drawing primitives with index list
     * @ buffer A buffer object that the device will read indexes from.
     * @ see `drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)`
     */
    virtual void setIndexBuffer(Buffer* buffer) = 0;

    /**
     * Set matrix tranform when drawing instances of the same model
     * @ buffer A buffer object that the device will read matrices from.
     */
    virtual void setInstanceBuffer(Buffer* buffer) = 0;

    /**
     * Draw primitives without an index list.
     * @param primitiveType The type of primitives that elements are assembled into.
     * @param start For each instance, the first index to draw
     * @param count For each instance, the number of indexes to draw
     * @see `drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)`
     */
    virtual void drawArrays(PrimitiveType primitiveType,
                            std::size_t start,
                            std::size_t count,
                            bool wireframe = false) = 0;

    virtual void drawArraysInstanced(PrimitiveType primitiveType,
                                     std::size_t start,
                                     std::size_t count,
                                     int instanceCount,
                                     bool wireframe = false) = 0;

    /**
     * Draw primitives with an index list.
     * @param primitiveType The type of primitives that elements are assembled into.
     * @param indexType The type if indexes, either 16 bit integer or 32 bit integer.
     * @param count The number of indexes to read from the index buffer for each instance.
     * @param offset Byte offset within indexBuffer to start reading indexes from.
     * @see `setIndexBuffer(Buffer* buffer)`
     * @see `drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)`
     */
    virtual void drawElements(PrimitiveType primitiveType,
                              IndexFormat indexType,
                              std::size_t count,
                              std::size_t offset,
                              bool wireframe = false) = 0;

    /**
     * Draw primitives with an index list instanced.
     * @param primitiveType The type of primitives that elements are assembled into.
     * @param indexType The type if indexes, either 16 bit integer or 32 bit integer.
     * @param count The number of indexes to read from the index buffer for each instance.
     * @param offset Byte offset within indexBuffer to start reading indexes from.
     * @param instance Count of instances to draw at once.
     * @see `setIndexBuffer(Buffer* buffer)`
     * @see `drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)`
     */
    virtual void drawElementsInstanced(PrimitiveType primitiveType,
                                       IndexFormat indexType,
                                       std::size_t count,
                                       std::size_t offset,
                                       int instanceCount,
                                       bool wireframe = false) = 0;

    /**
     * Do some resources release.
     */
    virtual void endRenderPass() = 0;

    /**
     * Present a drawable and commit a command buffer so it can be executed as soon as possible.
     */
    virtual void endFrame() = 0;

    /**
     * Fixed-function state
     * @param x, y Specifies the lower left corner of the scissor box
     * @param wdith Specifies the width of the scissor box
     * @param height Specifies the height of the scissor box
     */
    virtual void setScissorRect(bool isEnabled, float x, float y, float width, float height) = 0;

    /**
     * Read pixels from the specified render target.
     *
     * @param rt              The render target to read pixels from.
     * @param preserveAxisHint A hint indicating whether to preserve the original axis orientation
     *                         (e.g., avoid vertical flip). Note: support may vary across drivers/backends.
     * @param callback        A callback invoked with the resulting pixel buffer description.
     */
    virtual void readPixels(RenderTarget* rt,
                            bool preserveAxisHint,
                            std::function<void(const PixelBufferDesc&)> callback) = 0;

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
    virtual void setFrameBufferOnly(bool frameBufferOnly);

    /**
     * Update both front and back stencil reference value.
     * @param value Specifies stencil reference value.
     */
    virtual void setStencilReferenceValue(uint32_t value);

protected:
    virtual ~RenderContext() = default;

    RenderTarget* _screenRT{nullptr};         // weak ref (managed by Renderer)
    RenderTarget* _currentRT{nullptr};        // weak ref (managed by Renderer)
    ProgramState* _programState{nullptr};     // weak ref
    VertexLayout* _vertexLayout{nullptr};     // weak ref
    unsigned int _stencilReferenceValue = 0;  ///< front stencil reference value
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
