/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Id/CubismId.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles the breathing effect
 */
class CubismBreath
{
public:
    /**
     * Data attached to the parameters of breathing
     */
    struct BreathParameterData
    {
        /**
         * Constructor
         */
        BreathParameterData()
                             : ParameterId(NULL)
                             , Offset(0.0f)
                             , Peak(0.0f)
                             , Cycle(0.0f)
                             , Weight(0.0f)
        { }

        /**
         * Constructor<br>
         * Sets the data.
         *
         * @param parameterId ID of the breath parameter to attach
         * @param offset offset of the sine wave
         * @param peak peak value of the sine wave
         * @param cycle cycle of the sine wave
         * @param weight weight of the parameter
         *
         * @note The cyclic motion of breathing is set entirely by sine waves.
         */
        BreathParameterData(CubismIdHandle parameterId, csmFloat32 offset, csmFloat32 peak, csmFloat32 cycle, csmFloat32 weight)
            : ParameterId(parameterId)
            , Offset(offset)
            , Peak(peak)
            , Cycle(cycle)
            , Weight(weight)
        { }

        CubismIdHandle ParameterId;             ///< ID of the breath parameter to attach
        csmFloat32 Offset;                      ///< Offset of the sine wave
        csmFloat32 Peak;                        ///< Peak value of the sine wave
        csmFloat32 Cycle;                       ///< Cycle of the sine wave
        csmFloat32 Weight;                      ///< Weight of the parameter
    };

    /**
     * Makes an instance of CubismBreath.
     *
     * @return Maked instance of CubismBreath
     */
    static CubismBreath* Create();


    /**
     * Destroys an instance of CubismBreath.
     *
     * @param instance Instance of CubismBreath to destroy
     */
    static void Delete(CubismBreath* instance);

    /**
     * Attaches the parameters of breathing.
     *
     * @param breathParameters Collection of breathing parameters to attach
     */
    void SetParameters(const csmVector<BreathParameterData>& breathParameters);


    /**
     * Returns parameters attached to breathing.
     *
     * @return Attached collection of breathing parameters
     */
    const csmVector<BreathParameterData>& GetParameters() const;


    /**
     * Updates the parameters of the model.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Current time in seconds
     *
     * @note Execute after making an instance with #Create() and binding parameters with #setParameters().
     */
    void UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

private:
    CubismBreath();

    virtual ~CubismBreath();

    csmVector<BreathParameterData> _breathParameters;
    csmFloat32 _currentTime;
};

}}}
