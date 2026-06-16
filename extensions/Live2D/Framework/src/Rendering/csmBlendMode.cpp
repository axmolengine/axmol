/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "csmBlendMode.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

csmString csmBlendMode::ColorBlendModeToString(const csmInt32 type)
{
    switch (type)
    {
    case Core::csmColorBlendType_Normal:
        return "Normal";
    case Core::csmColorBlendType_Add:
        return "Add";
    case Core::csmColorBlendType_AddGlow:
        return "AddGlow";
    case Core::csmColorBlendType_Darken:
        return "Darken";
    case Core::csmColorBlendType_Multiply:
        return "Multiply";
    case Core::csmColorBlendType_ColorBurn:
        return "ColorBurn";
    case Core::csmColorBlendType_LinearBurn:
        return "LinearBurn";
    case Core::csmColorBlendType_Lighten:
        return "Lighten";
    case Core::csmColorBlendType_Screen:
        return "Screen";
    case Core::csmColorBlendType_ColorDodge:
        return "ColorDodge";
    case Core::csmColorBlendType_Overlay:
        return "Overlay";
    case Core::csmColorBlendType_SoftLight:
        return "SoftLight";
    case Core::csmColorBlendType_HardLight:
        return "HardLight";
    case Core::csmColorBlendType_LinearLight:
        return "LinearLight";
    case Core::csmColorBlendType_Hue:
        return "Hue";
    case Core::csmColorBlendType_Color:
        return "Color";
    case Core::csmColorBlendType_AddCompatible:
        return "AddCompatible";
    case Core::csmColorBlendType_MultiplyCompatible:
        return "MultiplyCompatible";
    default:
        return "";
    }
}

csmString csmBlendMode::AlphaBlendModeToString(const csmInt32 type)
{
    switch (type)
    {
    case Core::csmAlphaBlendType_Over:
        return "Over";
    case Core::csmAlphaBlendType_Atop:
        return "Atop";
    case Core::csmAlphaBlendType_Out:
        return "Out";
    case Core::csmAlphaBlendType_ConjointOver:
        return "ConjointOver";
    case Core::csmAlphaBlendType_DisjointOver:
        return "DisjointOver";
    default:
        return "";
    }
}

csmBlendMode::csmBlendMode()
    : _colorBlendType(Core::csmColorBlendType_Normal)
    , _alphaBlendType(Core::csmAlphaBlendType_Over)
{
}

csmBlendMode::~csmBlendMode()
{
}

void csmBlendMode::SetBlendMode(const csmInt32 blendMode)
{
    _colorBlendType = blendMode & 0xFF;
    _alphaBlendType = (blendMode >> 8) & 0xFF;
}

csmInt32 csmBlendMode::GetColorBlendType() const
{
    return _colorBlendType;
}

csmInt32 csmBlendMode::GetAlphaBlendType() const
{
    return _alphaBlendType;
}

csmString csmBlendMode::GetColorBlendTypeString() const
{
    return ColorBlendModeToString(_colorBlendType);
}

csmString csmBlendMode::GetAlphaBlendTypeString() const
{
    return AlphaBlendModeToString(_alphaBlendType);
}

}}}
//--------- LIVE2D NAMESPACE ------------
