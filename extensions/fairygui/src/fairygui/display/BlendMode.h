#pragma once

#include "axmol/2d/Node.h"
#include "axmol/base/Types.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

enum class BlendMode
{
    Normal,
    None,
    Add,
    Multiply,
    Screen,
    Erase,
    Mask,
    Below,
    Off,
    Custom1,
    Custom2,
    Custom3
};

class BlendModeUtils
{
public:
    static void apply(ax::Node* node, BlendMode blendMode);
    static void overrideBlendMode(BlendMode blendMode,
                                  ax::rhi::BlendFactor srcFactor,
                                  ax::rhi::BlendFactor dstFactor);

protected:
    static void apply(ax::Node* node, const ax::BlendFunc& blendFunc);
};

NS_FGUI_END
