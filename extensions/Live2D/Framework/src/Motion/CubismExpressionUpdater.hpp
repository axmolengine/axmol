/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Motion/CubismExpressionMotion.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating expressions motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismExpressionUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismExpressionUpdater() = delete;

    /**
     * Constructor
     *
     * @param expressionManager `CubismExpressionMotionManager` reference
     */
    CubismExpressionUpdater(CubismExpressionMotionManager& expressionManager);

    /**
     * Constructor
     *
     * @param expressionManager `CubismExpressionMotionManager` reference
     * @param executionOrder Order of operations
     */
    CubismExpressionUpdater(CubismExpressionMotionManager& expressionManager, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismExpressionUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    CubismExpressionMotionManager& _expressionManager;
};

}}}
