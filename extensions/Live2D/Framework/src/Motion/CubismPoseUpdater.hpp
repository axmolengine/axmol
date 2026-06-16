/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Effect/CubismPose.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating pose motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismPoseUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismPoseUpdater() = delete;

    /**
     * Constructor
     *
     * @param pose `CubismPose` reference
     */
    CubismPoseUpdater(CubismPose& pose);

    /**
     * Constructor
     *
     * @param pose `CubismPose` reference
     * @param executionOrder Order of operations
     */
    CubismPoseUpdater(CubismPose& pose, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismPoseUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    CubismPose& _pose;
};

}}}
