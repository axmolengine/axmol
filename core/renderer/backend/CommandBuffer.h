/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#include "base/Object.h"
#include "base/Data.h"

#include "Macros.h"
#include "Types.h"
#include "RenderPassDescriptor.h"
#include "PixelBufferDescriptor.h"
#include "platform/StdC.h"
#include "ProgramState.h"
#include "VertexLayout.h"

#include <memory>
#include <vector>

namespace ax
{
struct PipelineDescriptor;
}

NS_AX_BACKEND_BEGIN

class RenderPass;
class RenderPipeline;
class Buffer;
class DepthStencilState;
class TextureBackend;
class RenderTarget;
struct DepthStencilDescriptor;

/**
 * @addtogroup _backend
 * @{
 */

/**
 * @brief Store encoded commands for the GPU to execute.
 * A command buffer stores encoded commands until the buffer is committed for execution by the GPU
 */
class CommandBuffer : public ax::Object
{
public:
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
     * @param descriptor Specifies a group of render targets that hold the results of a render pass.
     */
    virtual void beginRenderPass(const RenderTarget* renderTarget, const RenderPassDescriptor& descriptor) = 0;

    /**
     * Update depthStencil status, improvment: for metal backend cache it
     * @param depthStencilState Specifies the depth and stencil status
     */
    virtual void updateDepthStencilState(const DepthStencilDescriptor& descriptor) = 0;

    /**
     * Update render pipeline status
     * Building a programmable pipeline involves an expensive evaluation of GPU state.
     * So a new render pipeline object will be created only if it hasn't been created before.
     * @param rt Specifies the render target.
     * @param pipelineDescriptor Specifies the pipeline descriptor.
     */
    virtual void updatePipelineState(const RenderTarget* rt, const PipelineDescriptor& descriptor) = 0;

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
     * Set unifroms and textures
     * @param programState A programState object that hold the uniform and texture data.
     */
    virtual void setProgramState(ProgramState* programState) = 0;

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
     * Get a screen snapshot
     * @param callback A callback to deal with screen snapshot image.
     */
    virtual void readPixels(RenderTarget* rt, std::function<void(const PixelBufferDescriptor&)> callback) = 0;

    /**
     * Update both front and back stencil reference value.
     * @param value Specifies stencil reference value.
     */
    void setStencilReferenceValue(unsigned int value);

    /**
     * Update stencil reference value.
     * @param frontRef Specifies front stencil reference value.
     * @param backRef Specifies back stencil reference value.
     */
    void setStencilReferenceValue(unsigned int frontRef, unsigned int backRef);

protected:
    virtual ~CommandBuffer() = default;

    unsigned int _stencilReferenceValueFront = 0;  ///< front stencil reference value.
    unsigned int _stencilReferenceValueBack  = 0;  ///< back stencil reference value.
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
