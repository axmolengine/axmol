/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Model/CubismModel.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueEntry;
class CubismMotionQueueManager;

typedef void(*CubismMotionEventFunction)(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData);

typedef void* CubismMotionQueueEntryHandle;

extern const CubismMotionQueueEntryHandle InvalidMotionQueueEntryHandleValue;

/**
 * Handles the management of motion playback.<br>
 * Used for playing subclasses of ACubismMotion such as CubismMotion.
 *
 * @note If a different motion is started with StartMotion() during playback,
 *       it transitions smoothly to the new motion, interrupting the old one.<br>
 *       When different motions such as facial expressions and body motions are played together,<br>
 *       use multiple instances of CubismMotionQueueManager.
 */
class CubismMotionQueueManager
{
public:
    /**
     * Constructor
     */
    CubismMotionQueueManager();

    /**
     * Destructor
     */
    virtual ~CubismMotionQueueManager();

    /**
     * Plays the motion.<br>
     * If a motion of the same type is already playing, it ends the currently playing motion and starts fading it out.
     *
     * @param motion motion to play
     * @param autoDelete true to delete the instance of the motion when playback ends
     * @param userTimeSeconds current time in seconds
     *
     * @return ID of the played motion.<br>
     *         -1 if the motion could not be started.
     *
     * @note The return value can be used as an argument to IsFinished() to determine if the motion has finished playing.
     */
    CubismMotionQueueEntryHandle    StartMotion(ACubismMotion* motion, csmBool autoDelete);

    /**
     * Checks whether all motions have finished playing.
     *
     * @return true if all motions have finished playing; otherwise false.
     */
    csmBool     IsFinished();

    /**
     * Checks whether the motion has finished playing.
     *
     * @param motionQueueEntryNumber identifier of the motion to check
     *
     * @return true if the motion has finished playing; otherwise false.
     */
    csmBool     IsFinished(CubismMotionQueueEntryHandle motionQueueEntryNumber);

    /**
     * Ends the playback of all motions.
     */
    void        StopAllMotions();

    /**
     * Returns a reference to the CubismMotionQueueEntry.
     *
     * @param motionQueueEntryNumber identifier of the motion to retrieve
     *
     * @return reference to the CubismMotionQueueEntry
     */
    CubismMotionQueueEntry* GetCubismMotionQueueEntry(CubismMotionQueueEntryHandle motionQueueEntryNumber);

    /**
     * Returns a pointer to the array of CubismMotionQueueEntry.
     *
     * @return pointer to the array of CubismMotionQueueEntry
     */
    csmVector<CubismMotionQueueEntry*>* GetCubismMotionQueueEntries();

    /**
     * Sets the callback function to receive user data events.
     *
     * @param callback   Callback function to receive user data events
     * @param customData User-defined data passed to the callback
     */
    void SetEventCallback(CubismMotionEventFunction callback, void* customData = NULL);

protected:
    virtual csmBool     DoUpdateMotion(CubismModel* model, csmFloat32 userTimeSeconds);


    csmFloat32 _userTimeSeconds;

private:
    csmVector<CubismMotionQueueEntry*>      _motions;

    CubismMotionEventFunction         _eventCallback;
    void*                             _eventCustomData;
};

}}}
