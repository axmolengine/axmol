/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Type/csmVector.hpp"
#include "Model/CubismUserModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotion;

/**
 * Handles adding information to the motion data for use by the CubismMotionQueueManager.
 */
class CubismMotionQueueEntry
{
    friend class CubismMotionQueueManager;
    friend class ACubismMotion;
    friend class CubismMotion;

public:
    /**
     * Constructor
     */
    CubismMotionQueueEntry();

    /**
     * Destructor
     */
    virtual ~CubismMotionQueueEntry();

    /**
     * Sets the number of seconds for the motion to complete fading out.
     *
     * @param fadeOutSeconds number of seconds for the fade-out to complete
     */
    void        SetFadeout(csmFloat32 fadeOutSeconds);

    /**
     * Sets the number of seconds for the motion to complete fading out and starts the fade out.
     *
     * @param fadeOutSeconds number of seconds for the fade-out to complete
     * @param userTimeSeconds current time in seconds
     */
    void        StartFadeout(csmFloat32 fadeOutSeconds, csmFloat32 userTimeSeconds);

    /**
     * Checks whether the motion playback has finished.
     *
     * @return true if the motion playback has finished; otherwise false.
     */
    csmBool     IsFinished() const;

    /**
     * Checks whether the motion playback has started.
     *
     * @return true if the motion playback has started; otherwise false.
     */
    csmBool     IsStarted() const;

    /**
     * Returns the time at which the motion playback started.
     *
     * @return time at which the motion playback started in seconds
     */
    csmFloat32    GetStartTime() const;

    /**
     * Returns the time at which the fade-in of the motion starts.
     *
     * @return time at which the fade-in starts in seconds
     */
    csmFloat32    GetFadeInStartTime() const;

    /**
     * Returns the time at which the fade-in of the motion ends.
     *
     * @return time at which the fade-in ends in seconds
     */
    csmFloat32    GetEndTime() const;

    /**
     * Sets the time to start playing the motion.
     *
     * @param startTime time to start playing the motion in seconds
     */
    void        SetStartTime(csmFloat32 startTime);

    /**
     * Sets the time to start the fade-in of the motion.
     *
     * @param startTime time to start the fade-in in seconds
     */
    void        SetFadeInStartTime(csmFloat32 startTime);

    /**
     * Sets the time to end the fade-in of the motion.
     *
     * @param endTime time to end the fade-in in seconds
     */
    void        SetEndTime(csmFloat32 endTime);

    /**
     * Sets whether to end the motion playback.
     *
     * @param f true to end the motion playback
     */
    void        IsFinished(csmBool f);

    /**
     * Sets whether to play the motion.
     *
     * @param f true to play the motion
     */
    void        IsStarted(csmBool f);

    /**
     * Checks whether the motion is active.
     *
     * @return true if the motion is active; otherwise false.
     */
    csmBool     IsAvailable() const;

    /**
     * Sets whether the motion is active.
     *
     * @param v true to activate the motion
     */
    void        IsAvailable(csmBool v);

    /**
     * Sets the state of the motion.
     *
     * @param timeSeconds current time to set in seconds
     * @param weight weight of the motion to set
     */
    void        SetState(csmFloat32 timeSeconds, csmFloat32 weight);

    /**
     * Returns the current time from the state of the motion.
     *
     * @return current time in seconds
     */
    csmFloat32  GetStateTime() const;

    /**
     * Returns the weight of the motion from its state.
     *
     * @return weight of the motion
     */
    csmFloat32  GetStateWeight() const;

    /**
     * Returns the last time the user data event was confirmed to fire.
     *
     * @return last time the user data event was confirmed to fire in seconds
     */
    csmFloat32  GetLastCheckEventTime() const;

    /**
     * Sets the time when the user data event was last confirmed to fire.
     *
     * @param checkTime last confirmed time in seconds
     */
    void        SetLastCheckEventTime(csmFloat32 checkTime);

    /**
     * Checks whether the motion is currently fading out.
     *
     * @return true if the motion is currently fading out; otherwise false.
     */
    csmBool     IsTriggeredFadeOut();

    /**
     * Returns the number of seconds for the motion to complete fading out.
     *
     * @return number of seconds for the fade-out to complete
     */
    csmFloat32     GetFadeOutSeconds();

    ACubismMotion* GetCubismMotion();

private:
    csmBool         _autoDelete;
    ACubismMotion*  _motion;

    csmBool         _available;
    csmBool         _finished;
    csmBool         _started;
    csmFloat32      _startTimeSeconds;
    csmFloat32      _fadeInStartTimeSeconds;
    csmFloat32      _endTimeSeconds;
    csmFloat32      _stateTimeSeconds;
    csmFloat32      _stateWeight;
    csmFloat32      _lastEventCheckSeconds;
    csmFloat32      _fadeOutSeconds;
    csmBool         _IsTriggeredFadeOut;

    CubismMotionQueueEntryHandle  _motionQueueEntryHandle;
};

}}}
