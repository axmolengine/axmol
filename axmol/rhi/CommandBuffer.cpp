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

#include "axmol/rhi/CommandBuffer.h"

namespace ax::rhi
{

void CommandBuffer::updatePipelineState(const RenderTarget* rt, const PipelineDesc& desc)
{
    _programState = desc.programState;
    _vertexLayout = desc.vertexLayout;

    assert(_programState);
    assert(_vertexLayout);
}

void CommandBuffer::setStencilReferenceValue(uint32_t value)
{
    _stencilReferenceValue = value;
}

bool CommandBuffer::resizeSwapchain(uint32_t /*width*/, uint32_t /*height*/)
{
    return true;
}

void CommandBuffer::setFrameBufferOnly(bool /*frameBufferOnly*/) {}

}  // namespace ax::rhi
