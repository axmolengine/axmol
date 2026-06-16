/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismUpdater.hpp"
#include "IParameterProvider.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating lip sync motion.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class CubismLipSyncUpdater : public ICubismUpdater
{
public:
    /**
     * Delete constructor
     */
    CubismLipSyncUpdater() = delete;

    /**
     * Constructor
     *
     * @param lipSyncIds `csmVector<CubismIdHandle>` reference
     * @param wavFileHandler `CubismWavFileHandler` reference
     */
    CubismLipSyncUpdater(csmVector<CubismIdHandle>& lipSyncIds, IParameterProvider& wavFileHandler);

    /**
     * Constructor
     *
     * @param lipSyncIds `csmVector<CubismIdHandle>` reference
     * @param wavFileHandler `CubismWavFileHandler` reference
     * @param executionOrder Order of operations
     */
    CubismLipSyncUpdater(csmVector<CubismIdHandle>& lipSyncIds, IParameterProvider& wavFileHandler, csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~CubismLipSyncUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) override;

private:
    IParameterProvider& _wavFileHandler;
    const csmVector<CubismIdHandle>& _lipSyncIds;
};

}}}
