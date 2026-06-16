/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Physics/CubismPhysics.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating pyhsucs motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismPhysicsUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismPhysicsUpdater() = delete;

    /**
     * Constructor
     *
     * @param physics `CubismPhysics` reference
     */
    CubismPhysicsUpdater(CubismPhysics& physics);

    /**
     * Constructor
     *
     * @param physics `CubismPhysics` reference
     * @param executionOrder Order of operations
     */
    CubismPhysicsUpdater(CubismPhysics& physics, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismPhysicsUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    CubismPhysics& _physics;
};

}}}
