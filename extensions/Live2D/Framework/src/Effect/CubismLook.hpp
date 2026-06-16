/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <climits>
#include "CubismFramework.hpp"
#include "Model/CubismModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Parameter Following Feature by Target.<br>
 * Provides parameter following functionality for drag input.
 */
class CubismLook
{
public:
    /**
     * Data attached to the parameters of look.
     */
    struct LookParameterData
    {
        /**
         * Constructor
         */
        LookParameterData()
            : ParameterId(nullptr)
            , FactorX(0.0f)
            , FactorY(0.0f)
            , FactorXY(0.0f)
        {
        }

        /**
         * Constructor<br>
         * Sets the data.
         *
         * @param parameterId ID of the breath parameter to attach.
         * @param factorX Coefficient for drag input along the X-axis.
         * @param factorY Coefficient for drag input along the Y-axis.
         * @param factorXY Coefficient for the combined X-Y (cross) drag input.
         */
        LookParameterData(CubismIdHandle parameterId, const csmFloat32 factorX = 0.0f, const csmFloat32 factorY = 0.0f, const csmFloat32 factorXY = 0.0f)
            : ParameterId(parameterId)
            , FactorX(factorX)
            , FactorY(factorY)
            , FactorXY(factorXY)
        {
        }

        CubismIdHandle ParameterId;
        csmFloat32 FactorX;
        csmFloat32 FactorY;
        csmFloat32 FactorXY;
    };

    /**
     * Makes an instance of CubismLook.
     *
     * @return Maked instance of CubismLook
     */
    static CubismLook* Create();

    /**
     * Destroys an instance of CubismLook.
     *
     * @param instance Instance of CubismLook to destroy
     */
    static void Delete(CubismLook* instance);

    /**
     * Attaches the parameters of look.
     *
     * @param lookParameters Collection of look parameters to attach
     */
    void SetParameters(const csmVector<LookParameterData>& lookParameters);

    /**
     * Returns parameters attached to look.
     *
     * @return Attached collection of look parameters
     */
    const csmVector<LookParameterData>& GetParameters() const;

    /**
     * Updates the parameters of the model.
     *
     * @param model Model to update
     * @param dragX X coordinate of the target
     * @param dragY Y coordinate of the target
     *
     * @note Execute after making an instance with #Create() and binding parameters with #setParameters().
     */
    void UpdateParameters(CubismModel* model, const csmFloat32 dragX, const csmFloat32 dragY) const;

private:
    CubismLook() = default;
    ~CubismLook() = default;

    csmVector<LookParameterData> _lookParameters;
};

}}}
