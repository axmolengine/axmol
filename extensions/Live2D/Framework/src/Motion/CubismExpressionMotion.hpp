/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Utils/CubismJson.hpp"
#include "Model/CubismModel.hpp"
#include "CubismExpressionMotionManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles facial expression motions.
 */
class CubismExpressionMotion : public ACubismMotion
{
public:
    /**
     * Blending calculation method for facial expression parameters
     */
    enum ExpressionBlendType
    {
        Additive = 0,           ///< Addition
        Multiply = 1,           ///< Multiplication
        Overwrite = 2           ///< Overwrite
    };

    /**
     * Data for facial expression parameter information
     */
    struct ExpressionParameter
    {
        CubismIdHandle      ParameterId;        ///< Attached parameter ID
        ExpressionBlendType BlendType;          ///< Blending calculation method for facial expression parameters attached to the parameter
        csmFloat32          Value;              ///< Parameter value
    };

    /**
     * Makes an instance.
     *
     * @param buf buffer containing the loaded facial expression settings file
     * @param size size of the buffer in bytes
     *
     * @return created instance
     */
    static CubismExpressionMotion* Create(const csmByte* buf, csmSizeInt size);

    /**
     * Updates the model parameters.
     *
     * @param model model to update
     * @param userTimeSeconds current time in seconds
     * @param weight weight during the application of the motion (0.0-1.0)
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     */
    virtual void DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 weight, CubismMotionQueueEntry* motionQueueEntry);

    /**
     * Computes the parameters related to the model's facial expressions.
     *
     * @param model model to update
     * @param userTimeSeconds cumulative delta time in seconds
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     * @param expressionParameterValues values of each parameter to be applied to the model
     * @param expressionIndex index of the facial expression
     */
    void CalculateExpressionParameters(CubismModel* model, csmFloat32 userTimeSeconds, CubismMotionQueueEntry* motionQueueEntry,
        csmVector<CubismExpressionMotionManager::ExpressionParameterValue>* expressionParameterValues, csmInt32 expressionIndex, csmFloat32 fadeWeight);

    /**
     * Returns the parameters referenced by the facial expression.
     */
    csmVector<ExpressionParameter> GetExpressionParameters();

    static const csmFloat32 DefaultAdditiveValue;
    static const csmFloat32 DefaultMultiplyValue;

protected:
    CubismExpressionMotion();

    virtual ~CubismExpressionMotion();

    void Parse(const csmByte* exp3Json, csmSizeInt size);

    csmVector<ExpressionParameter> _parameters;

private:

    csmFloat32 CalculateValue(csmFloat32 source, csmFloat32 destination, csmFloat32 fadeWeight);
};

}}}
