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
#include "axmol/rhi/vulkan/VertexLayoutVK.h"
#include "axmol/rhi/vulkan/RenderContextVK.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::vk
{

// Map VertexFormat to VkFormat
static VkFormat toVkFormat(VertexFormat format, bool unorm)
{
    switch (format)
    {
    case VertexFormat::FLOAT4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case VertexFormat::FLOAT3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case VertexFormat::FLOAT2:
        return VK_FORMAT_R32G32_SFLOAT;
    case VertexFormat::FLOAT:
        return VK_FORMAT_R32_SFLOAT;

    case VertexFormat::INT4:
        return VK_FORMAT_R32G32B32A32_SINT;
    case VertexFormat::INT3:
        return VK_FORMAT_R32G32B32_SINT;
    case VertexFormat::INT2:
        return VK_FORMAT_R32G32_SINT;
    case VertexFormat::INT:
        return VK_FORMAT_R32_SINT;

    case VertexFormat::USHORT4:
        return unorm ? VK_FORMAT_R16G16B16A16_UNORM : VK_FORMAT_R16G16B16A16_UINT;
    case VertexFormat::USHORT2:
        return unorm ? VK_FORMAT_R16G16_UNORM : VK_FORMAT_R16G16_UINT;

    case VertexFormat::UBYTE4:
        return unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_UINT;

    case VertexFormat::MAT4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;

    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VertexLayoutImpl::VertexLayoutImpl(VertexLayoutDesc&& desc) : VertexLayout(std::move(desc))
{
    auto& bindingsDesc = getBindings();
    _bindings.clear();
    _attributes.clear();

    for (auto& inputDesc : bindingsDesc)
    {
        VkVertexInputBindingDescription binding{};
        binding.binding   = inputDesc.instanceStepRate ? RenderContextImpl::VI_INSTANCING_BINDING_INDEX
                                                       : RenderContextImpl::VI_BINDING_INDEX;
        binding.stride    = inputDesc.instanceStepRate ? static_cast<uint32_t>(getInstanceStride())
                                                       : static_cast<uint32_t>(getStride());
        binding.inputRate = inputDesc.instanceStepRate ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;

        auto it = std::find_if(_bindings.begin(), _bindings.end(),
                               [&](const VkVertexInputBindingDescription& b) { return b.binding == binding.binding; });
        if (it == _bindings.end())
            _bindings.push_back(binding);

        // Attribute descriptions
        if (inputDesc.format != VertexFormat::MAT4)
        {
            VkVertexInputAttributeDescription attr{};
            attr.location = inputDesc.index;
            attr.binding  = binding.binding;
            attr.format   = toVkFormat(inputDesc.format, inputDesc.needToBeNormallized);
            attr.offset   = inputDesc.offset;
            _attributes.push_back(attr);
        }
        else
        {
            // MAT4 expands into 4 vec4 attributes
            for (uint32_t i = 0; i < 4; ++i)
            {
                VkVertexInputAttributeDescription attr{};
                attr.location = inputDesc.index + i;
                attr.binding  = binding.binding;
                attr.format   = VK_FORMAT_R32G32B32A32_SFLOAT;
                attr.offset   = inputDesc.offset + sizeof(float) * 4 * i;
                _attributes.push_back(attr);
            }
        }
    }

    _vkCreateInfo                                 = {};
    _vkCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    _vkCreateInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(_bindings.size());
    _vkCreateInfo.pVertexBindingDescriptions      = _bindings.data();
    _vkCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributes.size());
    _vkCreateInfo.pVertexAttributeDescriptions    = _attributes.data();
}

}  // namespace ax::rhi::vk
