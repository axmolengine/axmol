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
#include "axmol/rhi/vulkan/DepthStencilStateVK.h"
#include "axmol/tlx/hash.hpp"
#include <cstring>

namespace ax::rhi::vk
{
namespace
{
// Convert CompareFunc to VkCompareOp
static VkCompareOp to_vk(CompareFunc func)
{
    switch (func)
    {
    case CompareFunc::NEVER:
        return VK_COMPARE_OP_NEVER;
    case CompareFunc::LESS:
        return VK_COMPARE_OP_LESS;
    case CompareFunc::LESS_EQUAL:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareFunc::GREATER:
        return VK_COMPARE_OP_GREATER;
    case CompareFunc::GREATER_EQUAL:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareFunc::EQUAL:
        return VK_COMPARE_OP_EQUAL;
    case CompareFunc::NOT_EQUAL:
        return VK_COMPARE_OP_NOT_EQUAL;
    case CompareFunc::ALWAYS:
        return VK_COMPARE_OP_ALWAYS;
    default:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    }
}

// Convert StencilOp to VkStencilOp
static VkStencilOp to_vk(StencilOp op)
{
    switch (op)
    {
    case StencilOp::KEEP:
        return VK_STENCIL_OP_KEEP;
    case StencilOp::ZERO:
        return VK_STENCIL_OP_ZERO;
    case StencilOp::REPLACE:
        return VK_STENCIL_OP_REPLACE;
    case StencilOp::INVERT:
        return VK_STENCIL_OP_INVERT;
    case StencilOp::INCREMENT_WRAP:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOp::DECREMENT_WRAP:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default:
        return VK_STENCIL_OP_KEEP;
    }
}

static VkStencilOpState make_op_state(const StencilDesc& s)
{
    VkStencilOpState st{};
    st.failOp      = to_vk(s.stencilFailureOp);
    st.passOp      = to_vk(s.depthStencilPassOp);
    st.depthFailOp = to_vk(s.depthFailureOp);
    st.compareOp   = to_vk(s.stencilCompareFunc);
    st.compareMask = s.readMask;
    st.writeMask   = s.writeMask;
    st.reference   = 0;  // reference value set at draw time
    return st;
}
}  // namespace

DepthStencilStateImpl::DepthStencilStateImpl()
{
    _hash = tlx::hash_bytes(&_dsDesc, sizeof(_dsDesc));

    // Disabled state
    _disableInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    _disableInfo.depthTestEnable   = VK_FALSE;
    _disableInfo.depthWriteEnable  = VK_FALSE;
    _disableInfo.depthCompareOp    = VK_COMPARE_OP_ALWAYS;
    _disableInfo.stencilTestEnable = VK_FALSE;

    _activeInfo = _disableInfo;
}

void DepthStencilStateImpl::update(const DepthStencilDesc& desc)
{
    DepthStencilState::update(desc);

    _hash = tlx::hash_bytes(&_dsDesc, sizeof(_dsDesc));

    if (!isEnabled())
    {
        _activeInfo = _disableInfo;
        return;
    }

    VkPipelineDepthStencilStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    info.depthTestEnable  = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_TEST) ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_WRITE) ? VK_TRUE : VK_FALSE;
    info.depthCompareOp   = to_vk(desc.depthCompareFunc);

    info.stencilTestEnable = bitmask::any(desc.flags, DepthStencilFlags::STENCIL_TEST) ? VK_TRUE : VK_FALSE;
    info.front             = make_op_state(desc.frontFaceStencil);
    info.back              = make_op_state(desc.backFaceStencil);

    _activeInfo = info;
}

}  // namespace ax::rhi::vk
