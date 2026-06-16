/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "ACubismMotion.hpp"
#include "CubismMotionQueueManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles the management of motions.
 */
class CubismMotionManager : public CubismMotionQueueManager
{
public:
    /**
     * Constructor.
     */
    CubismMotionManager();

    /**
     * Destructor
     */
    virtual ~CubismMotionManager();

    /**
     * Returns the priority of the playing motion.
     *
     * @return priority of the motion
     */
    csmInt32 GetCurrentPriority() const;

    /**
     * Returns the priority of the reserved motion.
     *
     * @return priority of the motion
     */
    csmInt32 GetReservePriority() const;

    /**
     * Sets the priority of the reserved motion.
     *
     * @param val priority to set
     */
    void SetReservePriority(csmInt32 val);

    /**
     * Plays the motion with the specified priority.
     *
     * @param motion motion to play
     * @param autoDelete true to delete the instance of the motion when playback ends
     * @param priority priority of the motion
     *
     * @return ID of the played motion.<br>
     *         -1 if the motion could not be started.
     *
     * @note The return value can be used as an argument to IsFinished() to determine if the motion has finished playing.
     */
    CubismMotionQueueEntryHandle StartMotionPriority(ACubismMotion* motion, csmBool autoDelete, csmInt32 priority);

    /**
     * Updates the motion.<br>
     * Evaluates the current motion and sets the parameter values on the model.
     *
     * @param model model to update
     * @param deltaTimeSeconds current time in seconds
     * @param opacity opacity to set or get
     *
     * @return true if the motion was updated; otherwise false.
     */
    csmBool UpdateMotion(CubismModel* model, csmFloat32 deltaTimeSeconds);

    /**
     * Reserves the motion for playback.
     *
     * @param priority priority of the motion
     *
     * @return true if the motion was reserved for playback; otherwise false.
     */
    csmBool ReserveMotion(csmInt32 priority);

private:
    csmInt32 _currentPriority;
    csmInt32 _reservePriority;
};

}}}
