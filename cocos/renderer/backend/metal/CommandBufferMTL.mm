/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 c4games.com

 http://www.cocos2d-x.org

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
 
#include "CommandBufferMTL.h"
#include "BufferMTL.h"
#include "DeviceMTL.h"
#include "RenderPipelineMTL.h"
#include "TextureMTL.h"
#include "UtilsMTL.h"
#include "../Macros.h"
#include "BufferManager.h"
#include "DepthStencilStateMTL.h"
#include "RenderTargetMTL.h"

CC_BACKEND_BEGIN

namespace
{

#define byte(n) ((n) * 8)
#define bit(n) (n)
    static uint8_t getBitsPerElementMTL(MTLPixelFormat pixleFormat)
    {
        switch (pixleFormat)
        {
            case MTLPixelFormatDepth32Float_Stencil8:
                return byte(8);
            case MTLPixelFormatBGRA8Unorm:
            case MTLPixelFormatRGBA8Unorm:
            case MTLPixelFormatDepth32Float:
                return byte(4);
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
            case MTLPixelFormatDepth24Unorm_Stencil8:
                return byte(4);
    #else
            case MTLPixelFormatABGR4Unorm:
            case MTLPixelFormatBGR5A1Unorm:
            case MTLPixelFormatB5G6R5Unorm:
            case MTLPixelFormatA1BGR5Unorm:
                return byte(2);
    #endif
            case MTLPixelFormatA8Unorm:
            case MTLPixelFormatR8Unorm:
                return byte(1);
            default:
                assert(false);
                break;
        }
        return 0;
    }

    static MTLWinding toMTLWinding(Winding winding)
    {
        if (Winding::CLOCK_WISE == winding)
            return MTLWindingClockwise;
        else
            return MTLWindingCounterClockwise;
    }

    static MTLPrimitiveType toMTLPrimitive(PrimitiveType primitiveType)
    {
        MTLPrimitiveType ret = MTLPrimitiveTypeTriangle;
        switch (primitiveType)
        {
            case PrimitiveType::POINT:
                ret = MTLPrimitiveTypePoint;
                break;
            case PrimitiveType::LINE:
                ret = MTLPrimitiveTypeLine;
                break;
            case PrimitiveType::LINE_STRIP:
                ret = MTLPrimitiveTypeLineStrip;
                break;
            case PrimitiveType::TRIANGLE:
                ret = MTLPrimitiveTypeTriangle;
                break;
            case PrimitiveType::TRIANGLE_STRIP:
                ret = MTLPrimitiveTypeTriangleStrip;
                break;
            default:
                break;
        }
        return ret;
    }
    
    static MTLIndexType toMTLIndexType(IndexFormat indexFormat)
    {
        if (IndexFormat::U_SHORT == indexFormat)
            return MTLIndexTypeUInt16;
        else
            return MTLIndexTypeUInt32;
    }
    
    static MTLCullMode toMTLCullMode(CullMode mode)
    {
        switch (mode) {
            case CullMode::NONE:
                return MTLCullModeNone;
            case CullMode::FRONT:
                return MTLCullModeFront;
            case CullMode::BACK:
                return MTLCullModeBack;
        }
    }
    
    static MTLRenderPassDescriptor* toMTLRenderPassDescriptor(const RenderTarget* rt, const RenderPassParams& params)
    {
        MTLRenderPassDescriptor* mtlDescritpor = [MTLRenderPassDescriptor renderPassDescriptor];
        
        auto rtMTL = static_cast<const RenderTargetMTL*>(rt);
        rtMTL->applyRenderPassAttachments(params, mtlDescritpor);
        return mtlDescritpor;
    }
    
    static id<MTLTexture> getMTLTexture(TextureBackend* texture, int index)
    {
        return reinterpret_cast<id<MTLTexture>>(texture->getHandler(index));
    }
    
    static id<MTLSamplerState> getMTLSamplerState(TextureBackend* texture)
    {
        switch (texture->getTextureType())
        {
            case TextureType::TEXTURE_2D:
                return static_cast<TextureMTL*>(texture)->getMTLSamplerState();
            case TextureType::TEXTURE_CUBE:
                return static_cast<TextureCubeMTL*>(texture)->getMTLSamplerState();
            default:
                assert(false);
                return nil;
        }
    }

    inline int clamp(int value, int min, int max) {
        return std::min(max, std::max(min, value));
    }
}

CommandBufferMTL::CommandBufferMTL(DeviceMTL* deviceMTL)
: _mtlCommandQueue(deviceMTL->getMTLCommandQueue())
, _frameBoundarySemaphore(dispatch_semaphore_create(MAX_INFLIGHT_BUFFER))
{
}

CommandBufferMTL::~CommandBufferMTL()
{
    // Wait for all frames to finish by submitting and waiting on a dummy command buffer.
    flush();
    id<MTLCommandBuffer> oneOffBuffer = [_mtlCommandQueue commandBuffer];
    [oneOffBuffer commit];
    [oneOffBuffer waitUntilCompleted];
    
    dispatch_semaphore_signal(_frameBoundarySemaphore);
}

void CommandBufferMTL::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilStateMTL = static_cast<DepthStencilStateMTL*>(depthStencilState);
}

void CommandBufferMTL::setRenderPipeline(RenderPipeline* renderPipeline)
{
    _renderPipelineMTL = static_cast<RenderPipelineMTL*>(renderPipeline);
}

void CommandBufferMTL::beginFrame()
{
    _autoReleasePool = [[NSAutoreleasePool alloc] init];
    dispatch_semaphore_wait(_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    _mtlCommandBuffer = [_mtlCommandQueue commandBuffer];
    // [_mtlCommandBuffer enqueue];
    // commit will enqueue automatically
    [_mtlCommandBuffer retain];

    BufferManager::beginFrame();
}

id<MTLRenderCommandEncoder> CommandBufferMTL::getRenderCommandEncoder(const RenderTarget* renderTarget, const RenderPassParams& renderPassParams)
{
    if(_mtlRenderEncoder != nil &&
       _currentRenderPassParams == renderPassParams &&
       _currentRenderTarget == renderTarget &&
       _currentRenderTargetFlags == renderTarget->getTargetFlags())
    {
        return _mtlRenderEncoder;
    }
    
    _currentRenderTarget = renderTarget;
    _currentRenderPassParams = renderPassParams;
    _currentRenderTargetFlags = renderTarget->getTargetFlags();
    
    if(_mtlRenderEncoder != nil)
    {
        [_mtlRenderEncoder endEncoding];
        [_mtlRenderEncoder release];
        _mtlRenderEncoder = nil;
    }

    auto mtlDescriptor = toMTLRenderPassDescriptor(renderTarget, renderPassParams);
    _renderTargetWidth = (unsigned int)mtlDescriptor.colorAttachments[0].texture.width;
    _renderTargetHeight = (unsigned int)mtlDescriptor.colorAttachments[0].texture.height;
    id<MTLRenderCommandEncoder> mtlRenderEncoder = [_mtlCommandBuffer renderCommandEncoderWithDescriptor:mtlDescriptor];
    [mtlRenderEncoder retain];
    
    return mtlRenderEncoder;
}

void CommandBufferMTL::beginRenderPass(const RenderTarget* renderTarget, const RenderPassParams& descriptor)
{
    _mtlRenderEncoder = getRenderCommandEncoder(renderTarget, descriptor);
//    [_mtlRenderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
}


void CommandBufferMTL::updateDepthStencilState(const DepthStencilDescriptor& descriptor)
{
    _depthStencilStateMTL->update(descriptor);
}

void CommandBufferMTL::updatePipelineState(const RenderTarget* rt, const PipelineDescriptor& descriptor)
{
    _renderPipelineMTL->update(rt, descriptor);
    [_mtlRenderEncoder setRenderPipelineState:_renderPipelineMTL->getMTLRenderPipelineState()];
}

void CommandBufferMTL::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    MTLViewport viewport;
    viewport.originX = x;
    viewport.originY = (int)(_renderTargetHeight - y - h);
    viewport.width = w;
    viewport.height = h;
    viewport.znear = -1;
    viewport.zfar = 1;
    [_mtlRenderEncoder setViewport:viewport];
}

void CommandBufferMTL::setCullMode(CullMode mode)
{
    [_mtlRenderEncoder setCullMode:toMTLCullMode(mode)];
}

void CommandBufferMTL::setWinding(Winding winding)
{
    [_mtlRenderEncoder setFrontFacingWinding:toMTLWinding(winding)];
}

void CommandBufferMTL::setVertexBuffer(Buffer* buffer)
{
    // Vertex buffer is bound in index 0.
    [_mtlRenderEncoder setVertexBuffer:static_cast<BufferMTL*>(buffer)->getMTLBuffer()
                                offset:0
                               atIndex:0];
}

void CommandBufferMTL::setProgramState(ProgramState* programState)
{
    CC_SAFE_RETAIN(programState);
    CC_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferMTL::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (!buffer)
        return;
    
    _mtlIndexBuffer = static_cast<BufferMTL*>(buffer)->getMTLBuffer();
    [_mtlIndexBuffer retain];
}

void CommandBufferMTL::drawArrays(PrimitiveType primitiveType, std::size_t start,  std::size_t count)
{
    prepareDrawing();
    [_mtlRenderEncoder drawPrimitives:toMTLPrimitive(primitiveType)
                          vertexStart:start
                          vertexCount:count];
}

void CommandBufferMTL::drawElements(PrimitiveType primitiveType, IndexFormat indexType, std::size_t count, std::size_t offset)
{
    prepareDrawing();
    [_mtlRenderEncoder drawIndexedPrimitives:toMTLPrimitive(primitiveType)
                                  indexCount:count
                                   indexType:toMTLIndexType(indexType)
                                 indexBuffer:_mtlIndexBuffer
                           indexBufferOffset:offset];
    
}

void CommandBufferMTL::endRenderPass()
{
    afterDraw();

}

void CommandBufferMTL::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDescriptor&)> callback)
{
    auto rtMTL = static_cast<RenderTargetMTL*>(rt);
 
    // we only read form color attachment 0
    // if it's nullptr, will regard as screen to perform capture
    auto texture = rtMTL->_color[0].texture;
    CC_SAFE_RETAIN(texture);
    _captureCallbacks.emplace_back(texture, std::move(callback));
}

void CommandBufferMTL::endFrame()
{
    [_mtlRenderEncoder endEncoding];
    [_mtlRenderEncoder release];
    _mtlRenderEncoder = nil;
    
    [_mtlCommandBuffer presentDrawable:DeviceMTL::getCurrentDrawable()];
    _drawableTexture = DeviceMTL::getCurrentDrawable().texture;
    [_mtlCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
        // GPU work is complete
        // Signal the semaphore to start the CPU work
        dispatch_semaphore_signal(_frameBoundarySemaphore);
    }];

    flush();

    DeviceMTL::resetCurrentDrawable();
    [_autoReleasePool drain];
}

void CommandBufferMTL::flush()
{
    if(_mtlCommandBuffer) {
        assert(_mtlCommandBuffer.status != MTLCommandBufferStatusCommitted);
        [_mtlCommandBuffer commit];
        
        flushCaptureCommands();
        
        [_mtlCommandBuffer release];
        _mtlCommandBuffer = nil;
    }
}

void CommandBufferMTL::flushCaptureCommands()
{
    if(!_captureCallbacks.empty()) {
        // !!!important, if have capture request, must wait pending commandBuffer finish at this frame,
        // because readPixels require sync operation to get screen pixels properly without data race issue,
        // otherwise, will lead dead-lock
        // !!!Notes, MTL is mutli-threading, all GPU handler is dispatch at GPU threads
        [_mtlCommandBuffer waitUntilCompleted];
        
        PixelBufferDescriptor screenPixelData;
        for(auto& cb : _captureCallbacks) {
            if(cb.first == nil) { // screen capture
                if(!screenPixelData) {
                    CommandBufferMTL::readPixels(_drawableTexture, 0, 0, [_drawableTexture width], [_drawableTexture height], screenPixelData);
                    // screen framebuffer copied, restore screen framebuffer only to true
                    backend::Device::getInstance()->setFrameBufferOnly(true);
                }
                cb.second(screenPixelData);
            }
            else {
                PixelBufferDescriptor pixelData;
                auto texture = cb.first;
                assert(texture != nullptr);
                CommandBufferMTL::readPixels(texture, 0, 0, texture->getWidth(), texture->getHeight(), pixelData);
                CC_SAFE_RELEASE(texture);
                cb.second(pixelData);
            }
        }
        _captureCallbacks.clear();
    }
}

void CommandBufferMTL::afterDraw()
{
    if (_mtlIndexBuffer)
    {
        [_mtlIndexBuffer release];
        _mtlIndexBuffer = nullptr;
    }
    
    CC_SAFE_RELEASE_NULL(_programState);
}

void CommandBufferMTL::prepareDrawing() const
{
    setUniformBuffer();
    setTextures();
    
    auto mtlDepthStencilState = _depthStencilStateMTL->getMTLDepthStencilState();
    if (mtlDepthStencilState)
    {
        [_mtlRenderEncoder setDepthStencilState:mtlDepthStencilState];
        [_mtlRenderEncoder setStencilFrontReferenceValue:_stencilReferenceValueFront
                                      backReferenceValue:_stencilReferenceValueBack];
    }
}

void CommandBufferMTL::setTextures() const
{
    if (_programState)
    {
        doSetTextures(true);
        doSetTextures(false);
    }
}

void CommandBufferMTL::doSetTextures(bool isVertex) const
{
    const auto& bindTextureInfos = (isVertex) ? _programState->getVertexTextureInfos() : _programState->getFragmentTextureInfos();

    for(const auto& iter : bindTextureInfos)
    {
        /* About mutli textures support
        *  a. TODO: sampler2DArray, not implemented in Metal Renderer currently
        *  b. texture slot, one BackendTexture, multi GPU texture handlers, used by etc1, restrict: textures must have same size
        *  c. Bind multi BackendTexture to 1 Shader Program, see the ShaderTest
        *  d. iter.second.slots not used for Metal Renderer
        */
        auto location = iter.first;
        auto& textures = iter.second.textures;
        auto& indexs = iter.second.indexs;
        
        auto texture = textures[0];
        auto index = indexs[0];

        if (isVertex)
        {
            [_mtlRenderEncoder setVertexTexture:getMTLTexture(texture, index)
                                    atIndex:location];
            [_mtlRenderEncoder setVertexSamplerState:getMTLSamplerState(texture)
                                         atIndex:location];
        }
        else
        {
            [_mtlRenderEncoder setFragmentTexture:getMTLTexture(texture, index)
                                      atIndex:location];
            [_mtlRenderEncoder setFragmentSamplerState:getMTLSamplerState(texture)
                                           atIndex:location];
        }
    }
}

void CommandBufferMTL::setUniformBuffer() const
{
    if (_programState)
    {
        auto &callbackUniforms = _programState->getCallbackUniforms();	
        for(auto &cb : callbackUniforms)	
            cb.second(_programState, cb.first);	

        // Uniform buffer is bound to index 1.
        std::size_t bufferSize = 0;
        char* vertexBuffer = nullptr;
        _programState->getVertexUniformBuffer(&vertexBuffer, bufferSize);
        if(vertexBuffer)
        {
            [_mtlRenderEncoder setVertexBytes:vertexBuffer
                                       length:bufferSize 
                                       atIndex:1];
        }
        
        char* fragmentBuffer = nullptr;
        _programState->getFragmentUniformBuffer(&fragmentBuffer, bufferSize);
        if(fragmentBuffer)
        {
            [_mtlRenderEncoder setFragmentBytes:fragmentBuffer
                                         length:bufferSize
                                        atIndex:1];
        }
    }
}

void CommandBufferMTL::setLineWidth(float lineWidth)
{
}

void CommandBufferMTL::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    MTLScissorRect scissorRect;
    if(isEnabled)
    {
        y = _renderTargetHeight - height - y;
        int minX = clamp((int)x, 0, (int)_renderTargetWidth);
        int minY = clamp((int)y, 0, (int)_renderTargetHeight);
        int maxX = clamp((int)(x + width), 0, (int)_renderTargetWidth);
        int maxY = clamp((int)(y + height), 0, (int)_renderTargetHeight);
        scissorRect.x = minX;
        scissorRect.y = minY;
        scissorRect.width = maxX - minX;
        scissorRect.height = maxY - minY;
        if (scissorRect.width == 0 || scissorRect.height == 0) {
            scissorRect.width = 0;
            scissorRect.height = 0;
        }
    }
    else
    {
        scissorRect.x = 0;
        scissorRect.y = 0;
        scissorRect.width = _renderTargetWidth;
        scissorRect.height = _renderTargetHeight;
    }
    [_mtlRenderEncoder setScissorRect:scissorRect];
}

void CommandBufferMTL::readPixels(TextureBackend* texture, std::size_t origX, std::size_t origY, std::size_t rectWidth, std::size_t rectHeight, PixelBufferDescriptor& pbd)
{
    CommandBufferMTL::readPixels(reinterpret_cast<id<MTLTexture>>(texture->getHandler()), origX, origY, rectWidth, rectHeight, pbd);
}

void CommandBufferMTL::readPixels(id<MTLTexture> texture, std::size_t origX, std::size_t origY, std::size_t rectWidth, std::size_t rectHeight, PixelBufferDescriptor& pbd)
{
    NSUInteger texWidth = texture.width;
    NSUInteger texHeight = texture.height;
    MTLRegion region = MTLRegionMake2D(0, 0, texWidth, texHeight);
    MTLRegion imageRegion = MTLRegionMake2D(origX, origY, rectWidth, rectHeight);
    
    MTLTextureDescriptor* textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:[texture pixelFormat]
                                                       width:texWidth
                                                      height:texHeight
                                                   mipmapped:NO];
    id<MTLDevice> device = static_cast<DeviceMTL*>(DeviceMTL::getInstance())->getMTLDevice();
    id<MTLTexture> readPixelsTexture = [device newTextureWithDescriptor:textureDescriptor];
    
    id<MTLCommandQueue> commandQueue = static_cast<DeviceMTL*>(DeviceMTL::getInstance())->getMTLCommandQueue();
    auto commandBuffer = [commandQueue commandBuffer];
    // [commandBuffer enqueue];
    
    id<MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
    [blitCommandEncoder copyFromTexture:texture sourceSlice:0 sourceLevel:0 sourceOrigin:region.origin sourceSize:region.size toTexture:readPixelsTexture destinationSlice:0 destinationLevel:0 destinationOrigin:region.origin];
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    [blitCommandEncoder synchronizeResource:readPixelsTexture];
#endif
    [blitCommandEncoder endEncoding];
   
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBufferMTL) {
       auto bytePerRow = rectWidth * getBitsPerElementMTL(texture.pixelFormat) / 8;
       auto texelData = pbd._data.resize(bytePerRow * rectHeight);
       if(texelData != nullptr)
       {
          [readPixelsTexture getBytes:texelData bytesPerRow:bytePerRow fromRegion:imageRegion mipmapLevel:0];
          UtilsMTL::swizzleImage(texelData, rectWidth, rectHeight, readPixelsTexture.pixelFormat);
          pbd._width = rectWidth;
          pbd._height = rectHeight;
       }
       [readPixelsTexture release];
    }];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
}

CC_BACKEND_END
