/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Effect/CubismBreath.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating breathing motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismBreathUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismBreathUpdater() = delete;

    /**
     * Constructor
     *
     * @param breath `CubismBreath` reference
     */
    CubismBreathUpdater(CubismBreath& breath);

    /**
     * Constructor
     *
     * @param breath `CubismBreath` reference
     * @param executionOrder Order of operations
     */
    CubismBreathUpdater(CubismBreath& breath, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismBreathUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    CubismBreath& _breath;
};

}}}
