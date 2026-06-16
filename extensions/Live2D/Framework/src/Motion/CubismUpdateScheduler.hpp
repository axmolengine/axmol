/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/csmVector.hpp"
#include "ICubismUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Class for updating registered motions in sequence.
 */
class CubismUpdateScheduler
{
public:
    /**
     * Constructor
     */
    CubismUpdateScheduler();

    /**
     * Delete Copy Constructor
     */
    CubismUpdateScheduler(const CubismUpdateScheduler&) = delete;

    /**
     * Delete Move Constructor
     */
    CubismUpdateScheduler(CubismUpdateScheduler&&) = delete;

    /**
     * Destructor
     */
    ~CubismUpdateScheduler();

    /**
     * Adds ICubismUpdater to the update list.
     *
     * @param updatable The ICubismUpdater instance to be added.
     */
    void AddUpdatableList(ICubismUpdater* updatable);

    /**
     * Sorts the update list.
     */
    void SortUpdatableList();

    /**
     * Updates every element in the list.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Delta time in seconds.
     */
    void OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds);

    /**
     * Deleting the assignment operators
     */
    CubismUpdateScheduler& operator=(const CubismUpdateScheduler&) = delete;
    CubismUpdateScheduler& operator=(CubismUpdateScheduler&&) = delete;

private:
    csmVector<ICubismUpdater*> _cubismUpdatableList;
    csmBool _needsSort;
};

}}}
