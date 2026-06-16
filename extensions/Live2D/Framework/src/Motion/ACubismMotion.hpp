/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Id/CubismId.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueManager;
class CubismMotionQueueEntry;
class CubismModel;

/**
 * Abstract base class for motions.<br>
 * Handles the management of motion playback through the MotionQueueManager.
 */
class ACubismMotion
{
public:
    typedef void (*BeganMotionCallback)(ACubismMotion* self);
    typedef void (*FinishedMotionCallback)(ACubismMotion* self);
    /**
     * Destroys the instance.
     *
     * @param motion instance to destroy
     */
    static void Delete(ACubismMotion* motion);

    /**
     * Constructor
     */
    ACubismMotion();

    /**
     * Updates the model parameters.
     *
     * @param model model to update
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     * @param userTimeSeconds current time in seconds
     */
    void UpdateParameters(CubismModel* model, CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds);

    /**
     * Sets up to start the playback of the motion.
     *
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     * @param userTimeSeconds total playback time in seconds
     */
    void SetupMotionQueueEntry(CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds);

    /**
     * Sets the number of seconds for the motion to complete fading in.
     *
     * @param fadeInSeconds number of seconds for the fade-in to complete
     */
    void SetFadeInTime(csmFloat32 fadeInSeconds);

    /**
     * Sets the number of seconds for the motion to complete fading out.
     *
     * @param fadeOutSeconds number of seconds for the fade-out to complete
     */
    void SetFadeOutTime(csmFloat32 fadeOutSeconds);

    /**
     * Returns the number of seconds for the motion to complete fading out.
     *
     * @return number of seconds for the fade-out to complete
     */
    csmFloat32 GetFadeOutTime() const;

    /**
     * Returns the number of seconds for the motion to complete fading in.
     *
     * @return number of seconds for the fade-in to complete
     */
    csmFloat32 GetFadeInTime() const;

    /**
     * Sets the weight during the application of the motion.
     *
     * @param weight weight during the application of the motion (0.0-1.0)
     */
    void SetWeight(csmFloat32 weight);

    /**
     * Returns the weight during the application of the motion.
     *
     * @return weight during the application of the motion (0.0-1.0)
     */
    csmFloat32 GetWeight() const;

    /**
     * Returns the length of the motion.
     *
     * @return length of the motion in seconds<br>
     *         -1 if the motion is looping.
     *
     * @note When a positive value is returned, the motion ends at the obtained time.<br>
     *       When -1 is returned, the motion is looping and does not end.
     */
    virtual csmFloat32 GetDuration();

    /**
     * Returns the length of one loop of the looping motion.
     *
     * @return length of one loop of the looping motion in seconds<br>
     *         Same value as GetDuration() if the motion is not looping.
     *
     * @note Returns -1 if the length of one loop of the looping motion cannot be determined.
     */
    virtual csmFloat32 GetLoopDuration();


    /**
     * Sets the number of seconds to start the motion playback.
     *
     * @param offsetSeconds number of seconds to start the motion playback
     */
    void SetOffsetTime(csmFloat32 offsetSeconds);

    /**
     * Sets whether the motion should loop.
     *
     * @param loop true to set the motion to loop
     */
    void SetLoop(csmBool loop);

    /**
     * Checks whether the motion is set to loop.
     *
     * @return true if the motion is set to loop; otherwise false.
     */
    csmBool GetLoop() const;

    /**
     * Sets whether to perform fade-in for looping motion.
     *
     * @param loopFadeIn true to perform fade-in for looping motion
     */
    void SetLoopFadeIn(csmBool loopFadeIn);

    /**
     * Checks the setting for fade-in of looping motion.
     *
     * @return true if fade-in for looping motion is set; otherwise false.
     */
    csmBool GetLoopFadeIn() const;

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
    virtual const csmVector<const csmString*>& GetFiredEvent(csmFloat32 beforeCheckTimeSeconds,
                                                                   csmFloat32 motionTimeSeconds);

    /**
     * Sets the motion playback completion callback.
     *
     * Not called in the following cases:
     *   1. When the motion being played is set as "loop"
     *   2. When NULL is set as the callback
     *
     * @param   onBeganMotionHandler     Motion playback start callback function
     */
    void SetBeganMotionHandler(BeganMotionCallback onBeganMotionHandler);

    /**
     * Sets the motion playback start callback.
     *
     * @return  Set motion playback start callback function. NULL if no function is set.
     */
    BeganMotionCallback GetBeganMotionHandler() const;

    /**
     * Sets the user-defined data.
     *
     * @param   onBeganMotionCustomData  User-defined data
     */
    void SetBeganMotionCustomData(void* onBeganMotionCustomData);

    /**
     * Returns the user-defined data.
     *
     * @return  Set user-defined data.
     */
    void* GetBeganMotionCustomData() const;

    /**
     * Sets the motion playback start callback.
     *
     * Not called in the following cases:
     *   1. When the motion being played is set as "loop"
     *   2. When NULL is set as the callback
     *
     * @param   onBeganMotionHandler     Motion playback start callback function
     * @param   onBeganMotionCustomData  User-defined data
     */
    void SetBeganMotionHandlerAndMotionCustomData(BeganMotionCallback onBeganMotionHandler, void* onBeganMotionCustomData);

    /**
     * Sets the callback function for when motion playback ends.
     *
     * @param onFinishedMotionHandler callback function for when motion playback ends
     *
     * @note The callback function is called at the timing of setting the IsFinished flag.<br>
     *       Not called in the following states:
     *       1. When the playing motion is a loop motion
     *       2. When the callback function is not set
     */
    void SetFinishedMotionHandler(FinishedMotionCallback onFinishedMotionHandler);

    /**
     * Returns the callback function for when motion playback ends.
     *
     * @return set callback function for when motion playback ends
     */
    FinishedMotionCallback GetFinishedMotionHandler();

    /**
     * Sets the user-defined data.
     *
     * @param   onFinishedMotionCustomData  User-defined data
     */
    void SetFinishedMotionCustomData(void* onFinishedMotionCustomData);

    /**
     * Returns the user-defined data.
     *
     * @return  Set user-defined data.
     */
    void* GetFinishedMotionCustomData();

    /**
     * Sets the motion playback completion callback.
     *
     * Called when the IsFinished flag is set.
     * Not called in the following cases:
     *   1. When the motion being played is set as "loop"
     *   2. When NULL is set as the callback
     *
     * @param   onFinishedMotionHandler     Motion playback completion callback function
     * @param   onFinishedMotionCustomData  User-defined data
     */
    void SetFinishedMotionHandlerAndMotionCustomData(FinishedMotionCallback onFinishedMotionHandler, void* onFinishedMotionCustomData);

    /**
     * Checks whether there is an opacity curve.
     *
     * @return true if the key exists; otherwise false.
     */
    virtual csmBool IsExistModelOpacity() const;

    /**
     * Returns the index of the opacity curve.
     *
     * @return index of the opacity curve on success
     */
    virtual csmInt32 GetModelOpacityIndex() const;

    /**
     * Returns the ID of the opacity.
     *
     * @return ID of the opacity on success
     */
    virtual CubismIdHandle GetModelOpacityId(csmInt32 index);

    /**
     * Updates the fade weight of the motion.
     *
     * @param motionQueueEntry motion managed by the CubismMotionQueueManager
     * @param userTimeSeconds cumulative delta time in seconds
     *
     * @return true if the fade weight of the motion is updated; otherwise false.
     */
    csmFloat32 UpdateFadeWeight(CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds);

private:
    // Prevention of copy Constructor
    ACubismMotion(const ACubismMotion&);
    ACubismMotion& operator=(const ACubismMotion&);

protected:
    virtual ~ACubismMotion();

    virtual csmFloat32 GetModelOpacityValue() const;

    virtual void DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 weight, CubismMotionQueueEntry* motionQueueEntry) = 0;

    void AdjustEndTime(CubismMotionQueueEntry* motionQueueEntry);

    csmFloat32    _fadeInSeconds;
    csmFloat32    _fadeOutSeconds;
    csmFloat32    _weight;
    csmFloat32    _offsetSeconds;
    csmBool       _isLoop;
    csmBool       _isLoopFadeIn;
    csmBool       _previousLoopState;

    csmVector<const csmString*>    _firedEventValues;

    BeganMotionCallback _onBeganMotion;
    void* _onBeganMotionCustomData;
    FinishedMotionCallback _onFinishedMotion;
    void* _onFinishedMotionCustomData;
};

}}}
