/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Type/CubismBasicType.hpp"
#include "Type/csmVector.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueEntry;
struct CubismMotionData;

/**
 * Handles motions.
 */
class CubismMotion : public ACubismMotion
{
public:
    /**
     * Enumerator for version control of Motion Behavior
        For details, see the SDK Manual.
     */
    enum MotionBehavior
    {
        MotionBehavior_V1,
        MotionBehavior_V2,
    };

    /**
     * Makes an instance.
     *
     * @param buf buffer containing the loaded motion file
     * @param size size of the buffer in bytes
     * @param onFinishedMotionHandler callback function for when motion playback ends
     * @param onBeganMotionHandler callback function for when motion playback starts
     * @param shouldCheckMotionConsistency flag to validate the consistency of motion3.json
     *
     * @return created instance
     */
    static CubismMotion* Create(const csmByte* buffer, csmSizeInt size, FinishedMotionCallback onFinishedMotionHandler = NULL, BeganMotionCallback onBeganMotionHandler = NULL, csmBool shouldCheckMotionConsistency = false);

    /**
     * Updates the model parameters.
     *
     * @param model model to update
     * @param userTimeSeconds current time in seconds
     * @param weight weight during the application of the motion (0.0-1.0)
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     */
    virtual void        DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 fadeWeight, CubismMotionQueueEntry* motionQueueEntry);

    /**
     * Sets the version of the Motion Behavior.
     *
     * @param Specifies the version of the Motion Behavior.
     */
    void SetMotionBehavior(MotionBehavior motionBehavior);

    /**
     * Gets the version of the Motion Behavior.
     *
     * @return Returns the version of the Motion Behavior.
     */
    MotionBehavior GetMotionBehavior() const;

    /**
     * Returns the length of the motion.
     *
     * @return length of the motion in seconds<br>
     *         -1 if the motion is looping.
     *
     * @note When a positive value is returned, the motion ends at the obtained time.<br>
     *       When -1 is returned, the motion is looping and does not end.
     */
    virtual csmFloat32  GetDuration();

    /**
     * Returns the length of one loop of the looping motion.
     *
     * @return length of one loop of the looping motion in seconds<br>
     *         Same value as GetDuration() if the motion is not looping.
     *
     * @note Returns -1 if the length of one loop of the looping motion cannot be determined.
     */
    virtual csmFloat32  GetLoopDuration();

    /**
     * Sets the number of seconds for the motion parameter to complete fading in.
     *
     * @param parameterId parameter ID
     * @param value number of seconds for the fade-in to complete
     */
    void        SetParameterFadeInTime(CubismIdHandle parameterId, csmFloat32 value);

    /**
     * Sets the number of seconds for the motion parameter to complete fading out.
     *
     * @param parameterId parameter ID
     * @param value number of seconds for the fade-out to complete
     */
    void        SetParameterFadeOutTime(CubismIdHandle parameterId, csmFloat32 value);

    /**
     * Sets the number of seconds for the motion parameter to complete fading in.
     *
     * @param parameterId parameter ID
     * @param value number of seconds for the fade-in to complete
     */
    csmFloat32    GetParameterFadeInTime(CubismIdHandle parameterId) const;

    /**
     * Sets the number of seconds for the motion parameter to complete fading out.
     *
     * @param parameterId parameter ID
     * @param value number of seconds for the fade-out to complete
     */
    csmFloat32    GetParameterFadeOutTime(CubismIdHandle parameterId) const;

    /**
     * Sets the collections of parameter IDs associated with the eye blink and lip sync settings.
     *
     * @param eyeBlinkParameterIds collection of parameter IDs associated with the eye blink settings
     * @param lipSyncParameterIds collection of parameter IDs associated with the lip sync settings
     */
    void SetEffectIds(const csmVector<CubismIdHandle>& eyeBlinkParameterIds, const csmVector<CubismIdHandle>& lipSyncParameterIds);

    /**
     * Returns the triggered user data events.
     *
     * @param beforeCheckTimeSeconds previous playback time in seconds
     * @param motionTimeSeconds current playback time in seconds
     *
     * @return instance of the collection of triggered user data events
     *
     * @note The input times should be in seconds, with the motion timing set to zero.
     */
    virtual const csmVector<const csmString*>& GetFiredEvent(csmFloat32 beforeCheckTimeSeconds, csmFloat32 motionTimeSeconds);

    /**
     * Checks whether there is an opacity curve.
     *
     * @return true if the key exists; otherwise false.
     */
    csmBool IsExistModelOpacity() const;

    /**
     * Returns the index of the opacity curve.
     *
     * @return index of the opacity curve on success
     */
    csmInt32 GetModelOpacityIndex() const;

    /**
     * Returns the ID of the opacity.
     *
     * @return ID of the opacity on success
     */
    CubismIdHandle GetModelOpacityId(csmInt32 index);

protected:
    csmFloat32 GetModelOpacityValue() const;

private:
    CubismMotion();

    virtual ~CubismMotion();

    CubismMotion(const CubismMotion&);
    CubismMotion& operator=(const CubismMotion&);

    void UpdateForNextLoop(CubismMotionQueueEntry* motionQueueEntry, const csmFloat32 userTimeSeconds, const csmFloat32 time);

    void Parse(const csmByte* motionJson, const csmSizeInt size, csmBool shouldCheckMotionConsistency);

    csmFloat32      _sourceFrameRate;
    csmFloat32      _loopDurationSeconds;
    MotionBehavior  _motionBehavior;
    csmFloat32      _lastWeight;

    CubismMotionData*    _motionData;

    csmVector<CubismIdHandle>  _eyeBlinkParameterIds;
    csmVector<CubismIdHandle>  _lipSyncParameterIds;

    CubismIdHandle _modelCurveIdEyeBlink;
    CubismIdHandle _modelCurveIdLipSync;
    CubismIdHandle _modelCurveIdOpacity;

    csmFloat32 _modelOpacity;
};

}}}

//--------- LIVE2D NAMESPACE ------------
