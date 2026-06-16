/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismUpdateScheduler.hpp"
#include "Type/csmVectorSort.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismUpdateScheduler::CubismUpdateScheduler()
    : _needsSort(true)
{
}

CubismUpdateScheduler::~CubismUpdateScheduler()
{
    for (csmUint32 i = 0; i < _cubismUpdatableList.GetSize(); ++i)
    {
        CSM_DELETE(_cubismUpdatableList[i]);
    }
}

void CubismUpdateScheduler::AddUpdatableList(ICubismUpdater* updatable)
{
    if (updatable == nullptr)
    {
        return;
    }

    _cubismUpdatableList.PushBack(updatable);
    _needsSort = true;
}

void CubismUpdateScheduler::SortUpdatableList()
{
    csmVectorSort::MergeSort(_cubismUpdatableList.Begin(), _cubismUpdatableList.End(), ICubismUpdater::SortFunction);
    _needsSort = false;
}

void CubismUpdateScheduler::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (_needsSort)
    {
        SortUpdatableList();
    }

    for (csmUint32 i = 0; i < _cubismUpdatableList.GetSize(); ++i)
    {
        _cubismUpdatableList[i]->OnLateUpdate(model, deltaTimeSeconds);
    }
}

}}}
