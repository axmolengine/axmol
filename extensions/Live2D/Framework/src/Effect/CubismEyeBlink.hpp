/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Type/csmVector.hpp"
#include "Id/CubismId.hpp"
#include "ICubismModelSetting.hpp"


namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles automatic blinking effects
 */
class CubismEyeBlink
{
public:
    /**
     * Enumeration representing the current state
     */
    enum EyeState
    {
        EyeState_First = 0,         ///< Initial state
        EyeState_Interval,          ///< State of not blinking
        EyeState_Closing,           ///< State of closing the eyelids
        EyeState_Closed,            ///< State where the eyelids are closed
        EyeState_Opening            ///< State of opening the eyelids
    };

    /**
     * Makes an instance of CubismEyeBlink.
     *
     * @param modelSetting Model setting
     *
     * @return Maked instance of CubismEyeBlink
     *
     * @note If the model setting is empty, creates an empty instance with no parameter ID set.
     */
    static CubismEyeBlink* Create(ICubismModelSetting* modelSetting = NULL);

    /**
     * Destroys an instance of CubismEyeBlink.
     *
     * @param eyeBlink Instance of CubismEyeBlink to destroy
     */
    static void Delete(CubismEyeBlink* eyeBlink);

    /**
     * Sets the interval for blinking.
     *
     * @param blinkingInterval Time interval for blinking in seconds
     *
     * @note The time between blinks will be random from 0 seconds to twice the set interval.
     */
    void            SetBlinkingInterval(csmFloat32 blinkingInterval);

    /**
     * Sets the behavior for blinking.
     *
     * @param closing Time taken to close the eyelids in seconds
     * @param closed Time taken to keep the eyelids closed in seconds
     * @param opening Time taken to open the eyelids in seconds
     */
    void            SetBlinkingSettings(csmFloat32 closing, csmFloat32 closed, csmFloat32 opening);

    /**
     * Sets the collection of parameter IDs to apply blinking.
     *
     * @param parameterIds Collection of parameter IDs to apply
     */
    void            SetParameterIds(const csmVector<CubismIdHandle>& parameterIds);

    /**
     * Returns the collection of parameter IDs to apply blinking.
     *
     * @return Collection of parameter IDs
     */
    const csmVector<CubismIdHandle>&     GetParameterIds() const;

    /**
     * Updates the parameters of the model.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Current time in seconds
     */
    void            UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

private:

    CubismEyeBlink(ICubismModelSetting* modelSetting);

    virtual ~CubismEyeBlink();

    csmFloat32        DetermineNextBlinkingTiming() const;

    csmInt32                    _blinkingState;
    csmVector<CubismIdHandle>   _parameterIds;
    csmFloat32                  _nextBlinkingTime;
    csmFloat32                  _stateStartTimeSeconds;
    csmFloat32                  _blinkingIntervalSeconds;
    csmFloat32                  _closingSeconds;
    csmFloat32                  _closedSeconds;
    csmFloat32                  _openingSeconds;
    csmFloat32                  _userTimeSeconds;

};

}}}
