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
#include "axmol/rhi/d3d11/RenderPipeline11.h"
#include "axmol/rhi/d3d11/Program11.h"
#include "xxhash/xxhash.h"

namespace ax::rhi::d3d11
{
static D3D11_BLEND_OP toD3DBlendOp(BlendOp op)
{
    switch (op)
    {
    case BlendOp::ADD:
        return D3D11_BLEND_OP_ADD;
    case BlendOp::SUBTRACT:
        return D3D11_BLEND_OP_SUBTRACT;
    case BlendOp::REVERSE_SUBTRACT:
        return D3D11_BLEND_OP_REV_SUBTRACT;
    default:
        return D3D11_BLEND_OP_ADD;
    }
}

static D3D11_BLEND toD3DBlendRGB(BlendFactor f)
{
    switch (f)
    {
    case BlendFactor::ZERO:
        return D3D11_BLEND_ZERO;
    case BlendFactor::ONE:
        return D3D11_BLEND_ONE;
    case BlendFactor::SRC_COLOR:
        return D3D11_BLEND_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        return D3D11_BLEND_INV_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
        return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DST_COLOR:
        return D3D11_BLEND_DEST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        return D3D11_BLEND_INV_DEST_COLOR;
    case BlendFactor::DST_ALPHA:
        return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::CONSTANT_ALPHA:
        return D3D11_BLEND_BLEND_FACTOR;  // same wiht BLEND_COLOR
    case BlendFactor::SRC_ALPHA_SATURATE:
        return D3D11_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return D3D11_BLEND_INV_BLEND_FACTOR;
    case BlendFactor::BLEND_COLOR:
        return D3D11_BLEND_BLEND_FACTOR;  // blendColor.xyz
    default:
        return D3D11_BLEND_ONE;
    }
}

static D3D11_BLEND toD3DBlendAlpha(BlendFactor f)
{
    switch (f)
    {
    case BlendFactor::ZERO:
        return D3D11_BLEND_ZERO;
    case BlendFactor::ONE:
        return D3D11_BLEND_ONE;
    case BlendFactor::SRC_COLOR:
        return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DST_COLOR:
        return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::SRC_ALPHA:
        return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
        return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::CONSTANT_ALPHA:
        return D3D11_BLEND_BLEND_FACTOR;  // blendColor.w
    case BlendFactor::SRC_ALPHA_SATURATE:
        return D3D11_BLEND_ONE;  // alpha factor is one
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return D3D11_BLEND_INV_BLEND_FACTOR;  // 1 - blendColor.w
    case BlendFactor::BLEND_COLOR:
        return D3D11_BLEND_BLEND_FACTOR;  // Alpha = blendColor.w
    default:
        return D3D11_BLEND_ONE;
    }
}

static D3D11_BLEND toD3DBlend(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::ZERO:
        return D3D11_BLEND_ZERO;
    case BlendFactor::ONE:
        return D3D11_BLEND_ONE;
    case BlendFactor::SRC_COLOR:
        return D3D11_BLEND_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        return D3D11_BLEND_INV_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
        return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DST_COLOR:
        return D3D11_BLEND_DEST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        return D3D11_BLEND_INV_DEST_COLOR;
    case BlendFactor::DST_ALPHA:
        return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::CONSTANT_ALPHA:
        return D3D11_BLEND_BLEND_FACTOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
        return D3D11_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return D3D11_BLEND_INV_BLEND_FACTOR;
    case BlendFactor::BLEND_COLOR:
        return D3D11_BLEND_BLEND_FACTOR;
    default:
        return D3D11_BLEND_ONE;
    }
}

static UINT toD3DColorWriteMask(ColorWriteMask mask)
{
    UINT result = 0;
    if (static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::RED))
        result |= D3D11_COLOR_WRITE_ENABLE_RED;
    if (static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::GREEN))
        result |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    if (static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::BLUE))
        result |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    if (static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::ALPHA))
        result |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    return result;
}

static uint32_t hashBlendDesc(const BlendDesc& bd)
{
    struct PODBlend
    {
        uint32_t writeMask;
        uint8_t blendEnabled;
        uint8_t rgbBlendOp;
        uint8_t alphaBlendOp;
        uint8_t pad0;
        uint8_t sourceRGBBlendFactor;
        uint8_t destinationRGBBlendFactor;
        uint8_t sourceAlphaBlendFactor;
        uint8_t destinationAlphaBlendFactor;
        uint32_t pad1;
    } pod{static_cast<uint32_t>(bd.writeMask),
          static_cast<uint8_t>(bd.blendEnabled),
          static_cast<uint8_t>(bd.rgbBlendOp),
          static_cast<uint8_t>(bd.alphaBlendOp),
          0,
          static_cast<uint8_t>(bd.sourceRGBBlendFactor),
          static_cast<uint8_t>(bd.destinationRGBBlendFactor),
          static_cast<uint8_t>(bd.sourceAlphaBlendFactor),
          static_cast<uint8_t>(bd.destinationAlphaBlendFactor),
          0};

    return XXH32(&pod, sizeof(pod), 0);
}

void RenderPipelineImpl::update(const RenderTarget*, const PipelineDesc& desc)
{
    ComPtr<ID3D11BlendState> blendState;

    auto key = hashBlendDesc(desc.blendDesc);
    auto it  = _blendCache.find(key);

    if (it != _blendCache.end())
    {
        blendState = it->second;
    }
    else
    {
        D3D11_BLEND_DESC bd{};

        auto& bd0                 = bd.RenderTarget[0];
        bd0.RenderTargetWriteMask = toD3DColorWriteMask(desc.blendDesc.writeMask);

        if (desc.blendDesc.blendEnabled)
        {
            bd0.BlendEnable = TRUE;

            bd0.SrcBlend  = toD3DBlendRGB(desc.blendDesc.sourceRGBBlendFactor);
            bd0.DestBlend = toD3DBlendRGB(desc.blendDesc.destinationRGBBlendFactor);

            bd0.SrcBlendAlpha  = toD3DBlendAlpha(desc.blendDesc.sourceAlphaBlendFactor);
            bd0.DestBlendAlpha = toD3DBlendAlpha(desc.blendDesc.destinationAlphaBlendFactor);

            bd0.BlendOp      = toD3DBlendOp(desc.blendDesc.rgbBlendOp);
            bd0.BlendOpAlpha = toD3DBlendOp(desc.blendDesc.alphaBlendOp);
        }
        else
        {
            bd0.BlendEnable    = FALSE;
            bd0.SrcBlend       = D3D11_BLEND_ONE;
            bd0.DestBlend      = D3D11_BLEND_ZERO;
            bd0.BlendOp        = D3D11_BLEND_OP_ADD;
            bd0.SrcBlendAlpha  = D3D11_BLEND_ONE;
            bd0.DestBlendAlpha = D3D11_BLEND_ZERO;
            bd0.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
        }

        if (SUCCEEDED(_device->CreateBlendState(&bd, &blendState)))
        {
            _blendCache.emplace(key, blendState);
        }
        else
        {
            AXLOGW("create blend state fail");
        }
    }

    if (!blendState)
        return;

    constexpr UINT sampleMask = 0xFFFFFFFF;

    const FLOAT blendColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    _context->OMSetBlendState(blendState.Get(), blendColor, sampleMask);
}
}  // namespace ax::rhi::d3d11
