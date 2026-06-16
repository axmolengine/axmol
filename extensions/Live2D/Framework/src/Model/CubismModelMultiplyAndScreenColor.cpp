/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismModelMultiplyAndScreenColor.hpp"
#include "CubismModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

namespace
{
    // 警告ログメッセージ
    void WarnIndexOutOfRange(const csmChar* const funcName, const csmInt32 partIndex, const csmInt32 range)
    {
        CubismLogWarning("%s() : index is out of range. index = %d, valid range = [0, %d].", funcName, partIndex, range);
    }
}

CubismModelMultiplyAndScreenColor::CubismModelMultiplyAndScreenColor(const CubismModel* model, const csmVector<CubismModelPartInfo>* partsHierarchy)
    : _model(model)
    , _partsHierarchy(partsHierarchy)
    , _isOverriddenModelMultiplyColors(false)
    , _isOverriddenModelScreenColors(false)
{
}

void CubismModelMultiplyAndScreenColor::Initialize(const csmInt32 partCount, const csmInt32 drawableCount, const csmInt32 offscreenCount)
{
    // 乗算色
    ColorData userMultiplyColor;
    userMultiplyColor.IsOverridden = false;
    userMultiplyColor.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // スクリーン色
    ColorData userScreenColor;
    userScreenColor.IsOverridden = false;
    userScreenColor.Color = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Part
    _userPartMultiplyColors.PrepareCapacity(partCount);
    _userPartScreenColors.PrepareCapacity(partCount);
    for (csmInt32 i = 0; i < partCount; ++i)
    {
        _userPartMultiplyColors.PushBack(userMultiplyColor);
        _userPartScreenColors.PushBack(userScreenColor);
    }

    // Drawable
    _userDrawableMultiplyColors.PrepareCapacity(drawableCount);
    _userDrawableScreenColors.PrepareCapacity(drawableCount);
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        _userDrawableMultiplyColors.PushBack(userMultiplyColor);
        _userDrawableScreenColors.PushBack(userScreenColor);
    }

    // Offscreen
    _userOffscreenMultiplyColors.PrepareCapacity(offscreenCount);
    _userOffscreenScreenColors.PrepareCapacity(offscreenCount);
    for (csmInt32 i = 0; i < offscreenCount; ++i)
    {
        _userOffscreenMultiplyColors.PushBack(userMultiplyColor);
        _userOffscreenScreenColors.PushBack(userScreenColor);
    }
}

void CubismModelMultiplyAndScreenColor::SetMultiplyColorEnabled(const csmBool value)
{
    _isOverriddenModelMultiplyColors = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetMultiplyColorEnabled() const
{
    return _isOverriddenModelMultiplyColors;
}

void CubismModelMultiplyAndScreenColor::SetScreenColorEnabled(const csmBool value)
{
    _isOverriddenModelScreenColors = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetScreenColorEnabled() const
{
    return _isOverriddenModelScreenColors;
}

void CubismModelMultiplyAndScreenColor::SetPartMultiplyColorEnabled(const csmInt32 partIndex, const csmBool value)
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return;
    }

    SetPartColorEnabled(partIndex, value, _userPartMultiplyColors, _userDrawableMultiplyColors, _userOffscreenMultiplyColors);
}

csmBool CubismModelMultiplyAndScreenColor::GetPartMultiplyColorEnabled(const csmInt32 partIndex) const
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return false;
    }

    return _userPartMultiplyColors[partIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetPartScreenColorEnabled(const csmInt32 partIndex, const csmBool value)
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return;
    }

    SetPartColorEnabled(partIndex, value, _userPartScreenColors, _userDrawableScreenColors, _userOffscreenScreenColors);
}

csmBool CubismModelMultiplyAndScreenColor::GetPartScreenColorEnabled(const csmInt32 partIndex) const
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return false;
    }

    return _userPartScreenColors[partIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetPartMultiplyColor(const csmInt32 partIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetPartMultiplyColor(partIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetPartMultiplyColor(const csmInt32 partIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return;
    }

    SetPartColor(partIndex, r, g, b, a, _userPartMultiplyColors, _userDrawableMultiplyColors, _userOffscreenMultiplyColors);
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetPartMultiplyColor(const csmInt32 partIndex) const
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    return _userPartMultiplyColors[partIndex].Color;
}

void CubismModelMultiplyAndScreenColor::SetPartScreenColor(const csmInt32 partIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetPartScreenColor(partIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetPartScreenColor(const csmInt32 partIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return;
    }

    SetPartColor(partIndex, r, g, b, a, _userPartScreenColors, _userDrawableScreenColors, _userOffscreenScreenColors);
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetPartScreenColor(const csmInt32 partIndex) const
{
    if (partIndex < 0 || _model->GetPartCount() <= partIndex)
    {
        WarnIndexOutOfRange(__func__, partIndex, _model->GetPartCount() - 1);
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    return _userPartScreenColors[partIndex].Color;
}

void CubismModelMultiplyAndScreenColor::SetDrawableMultiplyColorEnabled(const csmInt32 drawableIndex, const csmBool value)
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return;
    }

    _userDrawableMultiplyColors[drawableIndex].IsOverridden = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetDrawableMultiplyColorEnabled(const csmInt32 drawableIndex) const
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return false;
    }

    return _userDrawableMultiplyColors[drawableIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetDrawableScreenColorEnabled(const csmInt32 drawableIndex, const csmBool value)
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return;
    }

    _userDrawableScreenColors[drawableIndex].IsOverridden = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetDrawableScreenColorEnabled(const csmInt32 drawableIndex) const
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return false;
    }

    return _userDrawableScreenColors[drawableIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetDrawableMultiplyColor(const csmInt32 drawableIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetDrawableMultiplyColor(drawableIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetDrawableMultiplyColor(const csmInt32 drawableIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return;
    }

    _userDrawableMultiplyColors[drawableIndex].Color.R = r;
    _userDrawableMultiplyColors[drawableIndex].Color.G = g;
    _userDrawableMultiplyColors[drawableIndex].Color.B = b;
    _userDrawableMultiplyColors[drawableIndex].Color.A = a;
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetDrawableMultiplyColor(const csmInt32 drawableIndex) const
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    if (GetMultiplyColorEnabled() || GetDrawableMultiplyColorEnabled(drawableIndex))
    {
        return _userDrawableMultiplyColors[drawableIndex].Color;
    }
    const Core::csmVector4 tmpColor = _model->GetDrawableMultiplyColor(drawableIndex);

    return Rendering::CubismRenderer::CubismTextureColor(tmpColor.X, tmpColor.Y, tmpColor.Z, tmpColor.W);
}

void CubismModelMultiplyAndScreenColor::SetDrawableScreenColor(const csmInt32 drawableIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetDrawableScreenColor(drawableIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetDrawableScreenColor(const csmInt32 drawableIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return;
    }

    _userDrawableScreenColors[drawableIndex].Color.R = r;
    _userDrawableScreenColors[drawableIndex].Color.G = g;
    _userDrawableScreenColors[drawableIndex].Color.B = b;
    _userDrawableScreenColors[drawableIndex].Color.A = a;
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetDrawableScreenColor(const csmInt32 drawableIndex) const
{
    if (drawableIndex < 0 || _model->GetDrawableCount() <= drawableIndex)
    {
        WarnIndexOutOfRange(__func__, drawableIndex, _model->GetDrawableCount() - 1);
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (GetScreenColorEnabled() || GetDrawableScreenColorEnabled(drawableIndex))
    {
        return _userDrawableScreenColors[drawableIndex].Color;
    }
    const Core::csmVector4 tmpColor = _model->GetDrawableScreenColor(drawableIndex);

    return Rendering::CubismRenderer::CubismTextureColor(tmpColor.X, tmpColor.Y, tmpColor.Z, tmpColor.W);
}

void CubismModelMultiplyAndScreenColor::SetOffscreenMultiplyColorEnabled(const csmInt32 offscreenIndex, const csmBool value)
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return;
    }

    _userOffscreenMultiplyColors[offscreenIndex].IsOverridden = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetOffscreenMultiplyColorEnabled(const csmInt32 offscreenIndex) const
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return false;
    }

    return _userOffscreenMultiplyColors[offscreenIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetOffscreenScreenColorEnabled(const csmInt32 offscreenIndex, const csmBool value)
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return;
    }

    _userOffscreenScreenColors[offscreenIndex].IsOverridden = value;
}

csmBool CubismModelMultiplyAndScreenColor::GetOffscreenScreenColorEnabled(const csmInt32 offscreenIndex) const
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return false;
    }

    return _userOffscreenScreenColors[offscreenIndex].IsOverridden;
}

void CubismModelMultiplyAndScreenColor::SetOffscreenMultiplyColor(const csmInt32 offscreenIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetOffscreenMultiplyColor(offscreenIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetOffscreenMultiplyColor(const csmInt32 offscreenIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return;
    }

    _userOffscreenMultiplyColors[offscreenIndex].Color.R = r;
    _userOffscreenMultiplyColors[offscreenIndex].Color.G = g;
    _userOffscreenMultiplyColors[offscreenIndex].Color.B = b;
    _userOffscreenMultiplyColors[offscreenIndex].Color.A = a;
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetOffscreenMultiplyColor(const csmInt32 offscreenIndex) const
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    if (GetMultiplyColorEnabled() || GetOffscreenMultiplyColorEnabled(offscreenIndex))
    {
        return _userOffscreenMultiplyColors[offscreenIndex].Color;
    }
    const Core::csmVector4 tmpColor = _model->GetOffscreenMultiplyColor(offscreenIndex);

    return Rendering::CubismRenderer::CubismTextureColor(tmpColor.X, tmpColor.Y, tmpColor.Z, tmpColor.W);
}

void CubismModelMultiplyAndScreenColor::SetOffscreenScreenColor(const csmInt32 offscreenIndex, const Rendering::CubismRenderer::CubismTextureColor& color)
{
    SetOffscreenScreenColor(offscreenIndex, color.R, color.G, color.B, color.A);
}

void CubismModelMultiplyAndScreenColor::SetOffscreenScreenColor(const csmInt32 offscreenIndex, const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a)
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return;
    }

    _userOffscreenScreenColors[offscreenIndex].Color.R = r;
    _userOffscreenScreenColors[offscreenIndex].Color.G = g;
    _userOffscreenScreenColors[offscreenIndex].Color.B = b;
    _userOffscreenScreenColors[offscreenIndex].Color.A = a;
}

Rendering::CubismRenderer::CubismTextureColor CubismModelMultiplyAndScreenColor::GetOffscreenScreenColor(const csmInt32 offscreenIndex) const
{
    if (offscreenIndex < 0 || _model->GetOffscreenCount() <= offscreenIndex)
    {
        WarnIndexOutOfRange(__func__, offscreenIndex, _model->GetOffscreenCount() - 1);
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (GetScreenColorEnabled() || GetOffscreenScreenColorEnabled(offscreenIndex))
    {
        return _userOffscreenScreenColors[offscreenIndex].Color;
    }
    const Core::csmVector4 tmpColor = _model->GetOffscreenScreenColor(offscreenIndex);

    return Rendering::CubismRenderer::CubismTextureColor(tmpColor.X, tmpColor.Y, tmpColor.Z, tmpColor.W);
}

void CubismModelMultiplyAndScreenColor::SetPartColor(
    const csmInt32 partIndex,
    const csmFloat32 r, const csmFloat32 g, const csmFloat32 b, const csmFloat32 a,
    csmVector<ColorData>& partColors,
    csmVector<ColorData>& drawableColors,
    csmVector<ColorData>& offscreenColors)
{
    partColors[partIndex].Color.R = r;
    partColors[partIndex].Color.G = g;
    partColors[partIndex].Color.B = b;
    partColors[partIndex].Color.A = a;

    if (partColors[partIndex].IsOverridden)
    {
        const csmInt32 offscreenIndex = Core::csmGetPartOffscreenIndices(_model->GetModel())[partIndex];
        if (offscreenIndex == CubismModel::CubismNoIndex_Offscreen)
        {
            // If no offscreen buffer is attached, the effect is applied to the children.
            for (csmUint32 i = 0; i < (*_partsHierarchy)[partIndex].Objects.GetSize(); ++i)
            {
                if ((*_partsHierarchy)[partIndex].Objects[i].ObjectType == CubismModelObjectInfo::ObjectType_Drawable)
                {
                    const csmUint32 drawableIndex = (*_partsHierarchy)[partIndex].Objects[i].ObjectIndex;
                    drawableColors[drawableIndex].Color.R = r;
                    drawableColors[drawableIndex].Color.G = g;
                    drawableColors[drawableIndex].Color.B = b;
                    drawableColors[drawableIndex].Color.A = a;
                }
                else
                {
                    const csmUint32 childPartIndex = (*_partsHierarchy)[partIndex].Objects[i].ObjectIndex;
                    SetPartColor(childPartIndex, r, g, b, a, partColors, drawableColors, offscreenColors);
                }
            }
        }
        else
        {
            // If an offscreen buffer is attached, only that offscreen buffer is affected.
            offscreenColors[offscreenIndex].Color.R = r;
            offscreenColors[offscreenIndex].Color.G = g;
            offscreenColors[offscreenIndex].Color.B = b;
            offscreenColors[offscreenIndex].Color.A = a;
        }
    }
}

void CubismModelMultiplyAndScreenColor::SetPartColorEnabled(
    const csmInt32 partIndex,
    const csmBool value,
    csmVector<ColorData>& partColors,
    csmVector<ColorData>& drawableColors,
    csmVector<ColorData>& offscreenColors)
{
    partColors[partIndex].IsOverridden = value;

    const csmInt32 offscreenIndex = Core::csmGetPartOffscreenIndices(_model->GetModel())[partIndex];
    if (offscreenIndex == CubismModel::CubismNoIndex_Offscreen)
    {
        // If no offscreen buffer is attached, the effect is applied to the children.
        for (csmUint32 i = 0; i < (*_partsHierarchy)[partIndex].Objects.GetSize(); ++i)
        {
            if ((*_partsHierarchy)[partIndex].Objects[i].ObjectType == CubismModelObjectInfo::ObjectType_Drawable)
            {
                const csmUint32 drawableIndex = (*_partsHierarchy)[partIndex].Objects[i].ObjectIndex;
                drawableColors[drawableIndex].IsOverridden = value;
                if (value)
                {
                    drawableColors[drawableIndex].Color.R = partColors[partIndex].Color.R;
                    drawableColors[drawableIndex].Color.G = partColors[partIndex].Color.G;
                    drawableColors[drawableIndex].Color.B = partColors[partIndex].Color.B;
                    drawableColors[drawableIndex].Color.A = partColors[partIndex].Color.A;
                }
            }
            else
            {
                const csmUint32 childPartIndex = (*_partsHierarchy)[partIndex].Objects[i].ObjectIndex;
                if (value)
                {
                    partColors[childPartIndex].Color.R = partColors[partIndex].Color.R;
                    partColors[childPartIndex].Color.G = partColors[partIndex].Color.G;
                    partColors[childPartIndex].Color.B = partColors[partIndex].Color.B;
                    partColors[childPartIndex].Color.A = partColors[partIndex].Color.A;
                }
                SetPartColorEnabled(childPartIndex, value, partColors, drawableColors, offscreenColors);
            }
        }
    }
    else
    {
        // If an offscreen buffer is attached, only that offscreen buffer is affected.
        offscreenColors[offscreenIndex].IsOverridden = value;
        if (value)
        {
            offscreenColors[offscreenIndex].Color.R = partColors[partIndex].Color.R;
            offscreenColors[offscreenIndex].Color.G = partColors[partIndex].Color.G;
            offscreenColors[offscreenIndex].Color.B = partColors[partIndex].Color.B;
            offscreenColors[offscreenIndex].Color.A = partColors[partIndex].Color.A;
        }
    }
}

}}}
