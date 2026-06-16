/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include <Type/csmString.hpp>

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief   A class that defines blend modes.
 *
 */
class csmBlendMode
{
public:
    /**
     * @brief   Get the color blend string.
     *
     * @return Returns the color blend type string.
     */
    static csmString ColorBlendModeToString(csmInt32 type);

    /**
     * @brief   Get the alpha blend string.
     *
     * @return Returns the alpha blend type string.
     */
    static csmString AlphaBlendModeToString(csmInt32 type);

    /**
     * @brief   Constructor.
     *
     */
    csmBlendMode();

    /**
     * @brief   Destructor.
     *
     */
    ~csmBlendMode();

    /**
     * @brief   Set the blend mode.
     *
     * @param blendMode The value of blendMode.
     */
    void SetBlendMode(csmInt32 blendMode);

    /**
     * @brief   Get the color blend.
     *
     * @return Returns the color blend type.
     */
    csmInt32 GetColorBlendType() const;

    /**
     * @brief   Get the alpha blend.
     *
     * @return Returns the alpha blend type.
     */
    csmInt32 GetAlphaBlendType() const;

    /**
     * @brief   Get the color blend string.
     *
     * @return Returns the color blend type string.
     */
    csmString GetColorBlendTypeString() const;

    /**
     * @brief   Get the alpha blend string.
     *
     * @return Returns the alpha blend type string.
     */
    csmString GetAlphaBlendTypeString() const;

private:
    csmInt32 _colorBlendType;
    csmInt32 _alphaBlendType;
};
}}}

//------------------------- LIVE2D NAMESPACE ------------
