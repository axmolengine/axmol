#include "RenderPipelineD3D.h"
#include "ProgramD3D.h"
#include "xxhash.h"

namespace ax::backend::d3d
{
static D3D11_BLEND_OP toD3DBlendOp(BlendOperation op)
{
    switch (op)
    {
    case BlendOperation::ADD:
        return D3D11_BLEND_OP_ADD;
    case BlendOperation::SUBTRACT:
        return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation::REVERSE_SUBTRACT:
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

static uint64_t hashBlendDesc(const BlendDescriptor& bd)
{
    struct PODBlend
    {
        uint32_t writeMask;
        uint8_t blendEnabled;
        uint8_t rgbBlendOperation;
        uint8_t alphaBlendOperation;
        uint8_t pad0;
        uint8_t sourceRGBBlendFactor;
        uint8_t destinationRGBBlendFactor;
        uint8_t sourceAlphaBlendFactor;
        uint8_t destinationAlphaBlendFactor;
        uint8_t pad1[4];
    } pod{static_cast<uint32_t>(bd.writeMask),
          static_cast<uint8_t>(bd.blendEnabled),
          static_cast<uint8_t>(bd.rgbBlendOperation),
          static_cast<uint8_t>(bd.alphaBlendOperation),
          0,
          static_cast<uint8_t>(bd.sourceRGBBlendFactor),
          static_cast<uint8_t>(bd.destinationRGBBlendFactor),
          static_cast<uint8_t>(bd.sourceAlphaBlendFactor),
          static_cast<uint8_t>(bd.destinationAlphaBlendFactor),
          {0, 0, 0, 0}};

    return XXH64(&pod, sizeof(pod), 0);
}

void RenderPipelineImpl::update(const RenderTarget*, const PipelineDescriptor& desc)
{
    ComPtr<ID3D11BlendState> blendState;

    uint64_t key = hashBlendDesc(desc.blendDescriptor);
    auto it      = _blendCache.find(key);

    if (it != _blendCache.end())
    {
        blendState = it->second;
    }
    else
    {
        D3D11_BLEND_DESC bd{};

        auto& bd0                 = bd.RenderTarget[0];
        bd0.RenderTargetWriteMask = toD3DColorWriteMask(desc.blendDescriptor.writeMask);

        if (desc.blendDescriptor.blendEnabled)
        {
            bd0.BlendEnable = TRUE;

            bd0.SrcBlend  = toD3DBlendRGB(desc.blendDescriptor.sourceRGBBlendFactor);
            bd0.DestBlend = toD3DBlendRGB(desc.blendDescriptor.destinationRGBBlendFactor);

            bd0.SrcBlendAlpha  = toD3DBlendAlpha(desc.blendDescriptor.sourceAlphaBlendFactor);
            bd0.DestBlendAlpha = toD3DBlendAlpha(desc.blendDescriptor.destinationAlphaBlendFactor);

            bd0.BlendOp      = toD3DBlendOp(desc.blendDescriptor.rgbBlendOperation);
            bd0.BlendOpAlpha = toD3DBlendOp(desc.blendDescriptor.alphaBlendOperation);
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

    if (desc.blendDescriptor.blendEnabled)
    {
        // axmol don't call glBlendColor, so the default blendFactor shoud be transparent
        const FLOAT blendColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        _context->OMSetBlendState(blendState.Get(), blendColor, sampleMask);
    }
    else
    {
        _context->OMSetBlendState(nullptr, nullptr, sampleMask);
    }
}
}  // namespace ax::backend::d3d
