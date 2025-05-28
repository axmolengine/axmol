#include "BlendMode.h"
#include "renderer/backend/Enums.h"

NS_FGUI_BEGIN
using namespace ax;

namespace
{
std::vector<BlendFunc> blendModes {
    {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},  // normal
    {backend::BlendFactor::ONE, backend::BlendFactor::ONE},                        // none
    {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE},                  // add
    {backend::BlendFactor::DST_COLOR, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},  // mul
    {backend::BlendFactor::ONE, backend::BlendFactor::ONE_MINUS_SRC_COLOR},        // screen
    {backend::BlendFactor::ZERO, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},       // erase
    {backend::BlendFactor::ZERO, backend::BlendFactor::SRC_ALPHA},                 // mask
    {backend::BlendFactor::ONE_MINUS_DST_ALPHA, backend::BlendFactor::DST_ALPHA},  // below
    {backend::BlendFactor::ONE, backend::BlendFactor::ZERO},                       // off
    {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom1
    {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom2
    {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE_MINUS_SRC_ALPHA},  // custom2
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
                              ax::backend::BlendFactor srcFactor,
                              ax::backend::BlendFactor dstFactor)
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
