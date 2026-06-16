/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMotionManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismMotionManager::CubismMotionManager()
    : _currentPriority(0)
    , _reservePriority(0)
{ }

CubismMotionManager::~CubismMotionManager()
{ }

csmInt32 CubismMotionManager::GetCurrentPriority() const
{
    return _currentPriority;
}

csmInt32 CubismMotionManager::GetReservePriority() const
{
    return _reservePriority;
}

void CubismMotionManager::SetReservePriority(csmInt32 val)
{
    _reservePriority = val;
}

CubismMotionQueueEntryHandle CubismMotionManager::StartMotionPriority(ACubismMotion* motion, csmBool autoDelete, csmInt32 priority)
{
    if (priority == _reservePriority)
    {
        _reservePriority = 0;           // 予約を解除
    }

    _currentPriority = priority;        // 再生中モーションの優先度を設定

    return CubismMotionQueueManager::StartMotion(motion, autoDelete);
}

csmBool CubismMotionManager::UpdateMotion(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    _userTimeSeconds += deltaTimeSeconds;

    const csmBool updated = CubismMotionQueueManager::DoUpdateMotion(model, _userTimeSeconds);

    if (IsFinished())
    {
        _currentPriority = 0;           // 再生中モーションの優先度を解除
    }

    return updated;
}

csmBool CubismMotionManager::ReserveMotion(csmInt32 priority)
{
    if ((priority <= _reservePriority) || (priority <= _currentPriority))
    {
        return false;
    }

    _reservePriority = priority;

    return true;
}

}}}
