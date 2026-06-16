/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "Effect/CubismEyeBlink.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating eye blink motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismEyeBlinkUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismEyeBlinkUpdater() = delete;

    /**
     * Constructor
     *
     * @param motionUpdated Motion update flag reference
     * @param eyeBlink `CubismEyeBlink` reference
     */
    CubismEyeBlinkUpdater(const csmBool& motionUpdated, CubismEyeBlink& eyeBlink);

    /**
     * Constructor
     *
     * @param motionUpdated Motion update flag reference
     * @param eyeBlink `CubismEyeBlink` reference
     * @param executionOrder Order of operations
     */
    CubismEyeBlinkUpdater(const csmBool& motionUpdated, CubismEyeBlink& eyeBlink, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismEyeBlinkUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    const csmBool& _motionUpdated;
    CubismEyeBlink& _eyeBlink;
};

}}}
