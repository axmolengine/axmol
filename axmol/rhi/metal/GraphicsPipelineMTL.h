/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/GraphicsPipeline.h"
#include <string>
#include <vector>
#include <memory>
#include "axmol/tlx/hlookup.hpp"
#import <Metal/Metal.h>

namespace ax::rhi::mtl
{
/**
 * @addtogroup _metal
 * @{
 */

class RenderTargetImpl;

/**
 * Create and compile a new MTLRenderPipelineState object synchronously.
 */
class GraphicsPipelineImpl : public GraphicsPipeline
{
public:
    /**
     * @param mtlDevice The device for which MTLRenderPipelineState object was created.
     * @param descriptor Specify the render pipeline description.
     */
    GraphicsPipelineImpl(id<MTLDevice> mtlDevice);
    ~GraphicsPipelineImpl();
    void update(const RenderTarget* renderTarget, const PipelineDesc&);

    /**
     * Get a MTLRenderPipelineState object.
     * @return A MTLRenderPipelineState object.
     */
    inline id<MTLRenderPipelineState> getMTLRenderPipelineState() const { return _mtlRenderPipelineState; }

private:
    void setVertexLayout(MTLRenderPipelineDescriptor*, const PipelineDesc&);
    void setBlendState(MTLRenderPipelineColorAttachmentDescriptor*, const BlendDesc&);
    void setShaderModules(Program*);
    void setBlendStateAndFormat(const BlendDesc&, const RenderTargetImpl* rt);

    id<MTLRenderPipelineState> _mtlRenderPipelineState = nil;
    id<MTLDevice> _mtlDevice                           = nil;

    MTLRenderPipelineDescriptor* _mtlRenderPipelineDesc = nil;
    tlx::hash_map<uint32_t, id<MTLRenderPipelineState>> _mtlStateCache;
};

// end of _metal group
/// @}
}  // namespace ax::rhi::mtl
