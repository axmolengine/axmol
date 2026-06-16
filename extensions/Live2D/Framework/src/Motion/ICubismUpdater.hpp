/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <climits>
#include "CubismFramework.hpp"
#include "Model/CubismModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

enum CubismUpdateOrder
{
    CubismUpdateOrder_EyeBlink = 200,
    CubismUpdateOrder_Expression = 300,
    CubismUpdateOrder_Look = 400,
    CubismUpdateOrder_Breath = 500,
    CubismUpdateOrder_Physics = 600,
    CubismUpdateOrder_LipSync = 700,
    CubismUpdateOrder_Pose = 800,
    CubismUpdateOrder_Max = INT_MAX
};

/**
 * Abstract base class for motions.<br>
 * Handles the management of motion playback through the CubismUpdateScheduler.
 */
class ICubismUpdater
{
public:
    /**
     * Comparison function used when sorting ICubismUpdater objects.
     *
     * @param left Pointer to the first ICubismUpdater object to be compared.
     * @param right Pointer to the second ICubismUpdater object to be compared.
     *
     * @return true if left should be placed before right in the sorted sequence.
     *         false otherwise.
     */
    static csmBool SortFunction(ICubismUpdater* left, ICubismUpdater* right);

    /**
     * Constructor
     */
    ICubismUpdater();

    /**
     * Constructor
     *
     * @param executionOrder Order of operations
     */
    ICubismUpdater(csmInt32 executionOrder);

    /**
     * Destructor
     */
    virtual ~ICubismUpdater() = default;

    /**
     * Update process.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    virtual void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds) = 0;

private:
    csmInt32 _executionOrder;
};

}}}
