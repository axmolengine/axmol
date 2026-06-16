/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Effect/CubismLook.hpp"
#include "Math/CubismTargetPoint.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating drag motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismLookUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismLookUpdater() = delete;

    /**
     * Constructor
     *
     * @param look        `CubismLook` reference
     * @param dragManager `CubismTargetPoint` reference
     */
    CubismLookUpdater(
        const CubismLook& look,
        CubismTargetPoint& dragManager
    );

    /**
     * Constructor
     *
     * @param look        `CubismLook` reference
     * @param dragManager `CubismTargetPoint` reference
     * @param executionOrder Order of operations
     */
    CubismLookUpdater(
        const CubismLook& look,
        CubismTargetPoint& dragManager,
        csmInt32 executionOrder
    );

    /**
     * Destructor
     */
    virtual ~CubismLookUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    const CubismLook& _look;
    CubismTargetPoint& _dragManager;
};

}}}
