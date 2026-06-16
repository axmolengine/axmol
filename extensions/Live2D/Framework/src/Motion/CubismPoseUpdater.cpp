/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismPoseUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismPoseUpdater::CubismPoseUpdater(CubismPose& pose)
    : ICubismUpdater(CubismUpdateOrder_Pose)
    , _pose(pose)
{
}

CubismPoseUpdater::CubismPoseUpdater(CubismPose& pose, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _pose(pose)
{
}

void CubismPoseUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    _pose.UpdateParameters(model, deltaTimeSeconds);
}

}}}
