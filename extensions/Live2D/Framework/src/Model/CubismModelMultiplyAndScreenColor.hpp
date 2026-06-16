/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Rendering/CubismRenderer.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismModel;
struct CubismModelPartInfo;

/**
 * Handling multiply and screen colors of the model.
 */
class CubismModelMultiplyAndScreenColor
{
public:
    /**
     * Structure for color information of drawing object
     */
    struct ColorData
    {
        /**
         * Constructor
         */
        ColorData()
            : IsOverridden(false)
            , Color()
        {
        }

        /**
         * Constructor
         *
         * @param isOverridden whether to be overridden
         * @param color Texture color
         */
        ColorData(const csmBool isOverridden, const Rendering::CubismRenderer::CubismTextureColor& color)
            : IsOverridden(isOverridden)
            , Color(color)
        {
        }

        /**
         * Destructor
         */
        ~ColorData() = default;

        csmBool IsOverridden;                                      ///< Whether to be overridden
        Rendering::CubismRenderer::CubismTextureColor Color;        ///< Color
    };

    /**
     * Delete default constructor.
     */
    CubismModelMultiplyAndScreenColor() = delete;

    /**
     * Constructor.
     *
     * @param model cubism model.
     * @param partsHierarchy parts hierarchy.
     */
    CubismModelMultiplyAndScreenColor(const CubismModel* model, const csmVector<CubismModelPartInfo>* partsHierarchy);

    /**
     * Destructor.
     */
    ~CubismModelMultiplyAndScreenColor() = default;

    /**
     * Initialization for using multiply and screen colors.
     *
     * @param partCount number of parts.
     * @param drawableCount number of drawables.
     * @param offscreenCount number of offscreen.
     */
    void Initialize(csmInt32 partCount, csmInt32 drawableCount, csmInt32 offscreenCount);

    /**
     * Sets the flag indicating whether the color set at runtime is used as the multiply color for the entire model during rendering.
     *
     * @param value true if the color set at runtime is to be used; otherwise false.
     */
    void SetMultiplyColorEnabled(csmBool value);

    /**
     * Returns the flag indicating whether the color set at runtime is used as the multiply color for the entire model during rendering.
     *
     * @return true if the color set at runtime is used; otherwise false.
     */
    csmBool GetMultiplyColorEnabled() const;

    /**
     * Sets the flag indicating whether the color set at runtime is used as the screen color for the entire model during rendering.
     *
     * @param value true if the color set at runtime is to be used; otherwise false.
     */
    void SetScreenColorEnabled(csmBool value);

    /**
     * Returns the flag indicating whether the color set at runtime is used as the screen color for the entire model during rendering.
     *
     * @return true if the color set at runtime is used; otherwise false.
     */
    csmBool GetScreenColorEnabled() const;

    /**
     * Sets whether the part multiply color is overridden by the SDK.
     * Use true to use the color information from the SDK, or false to use the color information from the model.
     *
     * @param partIndex Part index
     * @param value Offscreen True enable override, false to disable
     */
    void SetPartMultiplyColorEnabled(csmInt32 partIndex, csmBool value);

    /**
     * Checks whether the part multiply color is overridden by the SDK.
     *
     * @param partIndex Part index
     *
     * @return true if the color information from the SDK is used; otherwise false.
     */
    csmBool GetPartMultiplyColorEnabled(csmInt32 partIndex) const;

    /**
     * Sets whether the part screen color is overridden by the SDK.
     * Use true to use the color information from the SDK, or false to use the color information from the model.
     *
     * @param partIndex Part index
     * @param value Offscreen True enable override, false to disable
     */
    void SetPartScreenColorEnabled(csmInt32 partIndex, csmBool value);

    /**
     * Checks whether the part screen color is overridden by the SDK.
     *
     * @param partIndex Part index
     *
     * @return true if the color information from the SDK is used; otherwise false.
     */
    csmBool GetPartScreenColorEnabled(csmInt32 partIndex) const;

    /**
     * Sets the multiply color of the part.
     *
     * @param partIndex Part index
     * @param color Multiply color to be set (CubismTextureColor)
     */
    void SetPartMultiplyColor(csmInt32 partIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the multiply color of the part.
     *
     * @param partIndex Part index
     * @param r Red value of the multiply color to be set
     * @param g Green value of the multiply color to be set
     * @param b Blue value of the multiply color to be set
     * @param a Alpha value of the multiply color to be set
     */
    void SetPartMultiplyColor(csmInt32 partIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the multiply color of the part.
     *
     * @param partIndex Part index
     *
     * @return Multiply color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetPartMultiplyColor(csmInt32 partIndex) const;

    /**
     * Sets the screen color of the part.
     *
     * @param partIndex Part index
     * @param color Screen color to be set (CubismTextureColor)
     */
    void SetPartScreenColor(csmInt32 partIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the screen color of the part.
     *
     * @param partIndex Part index
     * @param r Red value of the screen color to be set
     * @param g Green value of the screen color to be set
     * @param b Blue value of the screen color to be set
     * @param a Alpha value of the screen color to be set
     */
    void SetPartScreenColor(csmInt32 partIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the screen color of the part.
     *
     * @param partIndex Part index
     *
     * @return Screen color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetPartScreenColor(csmInt32 partIndex) const;

    /**
     * Sets the flag indicating whether the color set at runtime is used as the multiply color for the drawable during rendering.
     *
     * @param drawableIndex Drawable index
     * @param value true if the color set at runtime is to be used; otherwise false.
     */
    void SetDrawableMultiplyColorEnabled(csmInt32 drawableIndex, csmBool value);

    /**
     * Returns the flag indicating whether the color set at runtime is used as the multiply color for the drawable during rendering.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the color set at runtime is used; otherwise false.
     */
    csmBool GetDrawableMultiplyColorEnabled(csmInt32 drawableIndex) const;

    /**
     * Sets the flag indicating whether the color set at runtime is used as the screen color for the drawable during rendering.
     *
     * @param drawableIndex Drawable index
     * @param value true if the color set at runtime is to be used; otherwise false.
     */
    void SetDrawableScreenColorEnabled(csmInt32 drawableIndex, csmBool value);

    /**
     * Returns the flag indicating whether the color set at runtime is used as the screen color for the drawable during rendering.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the color set at runtime is used; otherwise false.
     */
    csmBool GetDrawableScreenColorEnabled(csmInt32 drawableIndex) const;

    /**
     * Sets the multiply color of the drawable.
     *
     * @param drawableIndex Drawable index
     * @param color Multiply color to be set (CubismTextureColor)
     */
    void SetDrawableMultiplyColor(csmInt32 drawableIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the multiply color of the drawable.
     *
     * @param drawableIndex Drawable index
     * @param r Red value of the multiply color to be set
     * @param g Green value of the multiply color to be set
     * @param b Blue value of the multiply color to be set
     * @param a Alpha value of the multiply color to be set
     */
    void SetDrawableMultiplyColor(csmInt32 drawableIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the multiply color from the list of drawables.
     *
     * @param drawableIndex Drawable index
     *
     * @return Multiply color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetDrawableMultiplyColor(csmInt32 drawableIndex) const;

    /**
     * Sets the screen color of the drawable.
     *
     * @param drawableIndex Drawable index
     * @param color Screen color to be set (CubismTextureColor)
     */
    void SetDrawableScreenColor(csmInt32 drawableIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the screen color of the drawable.
     *
     * @param drawableIndex Drawable index
     * @param r Red value of the screen color to be set
     * @param g Green value of the screen color to be set
     * @param b Blue value of the screen color to be set
     * @param a Alpha value of the screen color to be set
     */
    void SetDrawableScreenColor(csmInt32 drawableIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the screen color from the list of drawables.
     *
     * @param drawableIndex Drawable index
     *
     * @return Screen color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetDrawableScreenColor(csmInt32 drawableIndex) const;

    /**
     * Sets whether the offscreen multiply color is overridden by the SDK.
     * Use true to use the color information from the SDK, or false to use the color information from the model.
     *
     * @param offscreenIndex Offscreen index
     * @param value Offscreen True enable override, false to disable
     */
    void SetOffscreenMultiplyColorEnabled(csmInt32 offscreenIndex, csmBool value);

    /**
     * Checks whether the offscreen multiply color is overridden by the SDK.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return true if the color information from the SDK is used; otherwise false.
     */
    csmBool GetOffscreenMultiplyColorEnabled(csmInt32 offscreenIndex) const;

    /**
     * Sets whether the offscreen screen color is overridden by the SDK.
     * Use true to use the color information from the SDK, or false to use the color information from the model.
     *
     * @param offscreenIndex Offscreen index
     * @param value Offscreen True enable override, false to disable
     */
    void SetOffscreenScreenColorEnabled(csmInt32 offscreenIndex, csmBool value);

    /**
     * Checks whether the offscreen screen color is overridden by the SDK.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return true if the color information from the SDK is used; otherwise false.
     */
    csmBool GetOffscreenScreenColorEnabled(csmInt32 offscreenIndex) const;

    /**
     * Sets the multiply color of the offscreen.
     *
     * @param offscreenIndex Offsscreen index
     * @param color Multiply color to be set (CubismTextureColor)
     */
    void SetOffscreenMultiplyColor(csmInt32 offscreenIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the multiply color of the offscreen.
     *
     * @param offscreenIndex Offsscreen index
     * @param r Red value of the multiply color to be set
     * @param g Green value of the multiply color to be set
     * @param b Blue value of the multiply color to be set
     * @param a Alpha value of the multiply color to be set
     */
    void SetOffscreenMultiplyColor(csmInt32 offscreenIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the multiply color from the list of offscreen.
     *
     * @param offscreenIndex Offsscreen index
     *
     * @return Multiply color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetOffscreenMultiplyColor(csmInt32 offscreenIndex) const;

    /**
     * Sets the screen color of the offscreen.
     *
     * @param offscreenIndex Offsscreen index
     * @param color Screen color to be set (CubismTextureColor)
     */
    void SetOffscreenScreenColor(csmInt32 offscreenIndex, const Rendering::CubismRenderer::CubismTextureColor& color);

    /**
     * Sets the screen color of the offscreen.
     *
     * @param offscreenIndex Offsscreen index
     * @param r Red value of the screen color to be set
     * @param g Green value of the screen color to be set
     * @param b Blue value of the screen color to be set
     * @param a Alpha value of the screen color to be set
     */
    void SetOffscreenScreenColor(csmInt32 offscreenIndex, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a = 1.0f);

    /**
     * Returns the screen color from the list of offscreen.
     *
     * @param offscreenIndex Offsscreen index
     *
     * @return Screen color (CubismTextureColor)
     */
    Rendering::CubismRenderer::CubismTextureColor GetOffscreenScreenColor(csmInt32 offscreenIndex) const;

private:
    void SetPartColor(
        csmInt32 partIndex,
        csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a,
        csmVector<ColorData>& partColors,
        csmVector<ColorData>& drawableColors,
        csmVector<ColorData>& offscreenColors);

    void SetPartColorEnabled(
        csmInt32 partIndex,
        csmBool value,
        csmVector<ColorData>& partColors,
        csmVector<ColorData>& drawableColors,
        csmVector<ColorData>& offscreenColors);

    const CubismModel* const _model;
    const csmVector<CubismModelPartInfo>* _partsHierarchy;
    csmBool _isOverriddenModelMultiplyColors;
    csmBool _isOverriddenModelScreenColors;
    csmVector<ColorData> _userPartScreenColors;
    csmVector<ColorData> _userPartMultiplyColors;
    csmVector<ColorData> _userDrawableScreenColors;
    csmVector<ColorData> _userDrawableMultiplyColors;
    csmVector<ColorData> _userOffscreenScreenColors;
    csmVector<ColorData> _userOffscreenMultiplyColors;
};

}}}
