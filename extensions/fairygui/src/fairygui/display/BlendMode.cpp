#include "BlendMode.h"

NS_FGUI_BEGIN
using namespace ax;

namespace
{
std::vector<BlendFunc> blendModes {
    {rhi::BlendFactor::SRC_ALPHA, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},  // normal
    {rhi::BlendFactor::ONE, rhi::BlendFactor::ONE},                        // none
    {rhi::BlendFactor::SRC_ALPHA, rhi::BlendFactor::ONE},                  // add
    {rhi::BlendFactor::DST_COLOR, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},  // mul
    {rhi::BlendFactor::ONE, rhi::BlendFactor::ONE_MINUS_SRC_COLOR},        // screen
    {rhi::BlendFactor::ZERO, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},       // erase
    {rhi::BlendFactor::ZERO, rhi::BlendFactor::SRC_ALPHA},                 // mask
    {rhi::BlendFactor::ONE_MINUS_DST_ALPHA, rhi::BlendFactor::DST_ALPHA},  // below
    {rhi::BlendFactor::ONE, rhi::BlendFactor::ZERO},                       // off
    {rhi::BlendFactor::SRC_ALPHA, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom1
    {rhi::BlendFactor::SRC_ALPHA, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom2
    {rhi::BlendFactor::SRC_ALPHA, rhi::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom2
};
}

void BlendModeUtils::apply(ax::Node* node, BlendMode blendMode)
{
    if (!node)
        return;

    auto blendIndex = static_cast<size_t>(blendMode);
    if (blendIndex >= blendModes.size())
    {
        AXLOGE("[FairyGUI] Invalid BlendMode: {}", blendIndex);
        return;
    }

    auto& blendFunc = blendModes[blendIndex];
    apply(node, blendFunc);
}

void BlendModeUtils::apply(ax::Node* node, const ax::BlendFunc& blendFunc)
{
    auto blendProtocol = dynamic_cast<BlendProtocol*>(node);
    if (blendProtocol)
    {
        auto& currentBlendFunc = blendProtocol->getBlendFunc();
        if (currentBlendFunc.src != blendFunc.src || currentBlendFunc.dst != blendFunc.dst)
        {
            blendProtocol->setBlendFunc(blendFunc);
        }
    }

    auto&& children = node->getChildren();
    for (auto&& child : children)
    {
        apply(child, blendFunc);
    }
}

void BlendModeUtils::overrideBlendMode(BlendMode blendMode,
                              ax::rhi::BlendFactor srcFactor,
                              ax::rhi::BlendFactor dstFactor)
{
    auto blendIndex = static_cast<size_t>(blendMode);
    if (blendIndex >= blendModes.size())
    {
        AXLOGE("[FairyGUI] Invalid BlendMode: {}", blendIndex);
        return;
    }

    blendModes[blendIndex].src = srcFactor;
    blendModes[blendIndex].dst = dstFactor;
}

NS_FGUI_END
