/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMotionQueueManager.hpp"
#include "CubismMotionQueueEntry.hpp"
#include "CubismFramework.hpp"
#include "CubismMotion.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

const CubismMotionQueueEntryHandle InvalidMotionQueueEntryHandleValue = reinterpret_cast<CubismMotionQueueEntryHandle*>(-1);

CubismMotionQueueManager::CubismMotionQueueManager()
    : _userTimeSeconds(0.0f)
    , _eventCallback(NULL)
    , _eventCustomData(NULL)
{}

CubismMotionQueueManager::~CubismMotionQueueManager()
{
    for (csmUint32 i = 0; i < _motions.GetSize(); ++i)
    {
        if (_motions[i])
        {
            CSM_DELETE(_motions[i]);
        }
    }
}

CubismMotionQueueEntryHandle CubismMotionQueueManager::StartMotion(ACubismMotion* motion, csmBool autoDelete, csmFloat32 userTimeSeconds)
{
    if (motion == NULL)
    {
        return InvalidMotionQueueEntryHandleValue;
    }

    CubismMotionQueueEntry* motionQueueEntry = NULL;

    // 既にモーションがあれば終了フラグを立てる
    for (csmUint32 i = 0; i < _motions.GetSize(); ++i)
    {
        motionQueueEntry = _motions.At(i);
        if (motionQueueEntry == NULL)
        {
            continue;
        }

        motionQueueEntry->SetFadeout(motionQueueEntry->_motion->GetFadeOutTime());
    }

    motionQueueEntry = CSM_NEW CubismMotionQueueEntry(); // 終了時に破棄する
    motionQueueEntry->_autoDelete = autoDelete;
    motionQueueEntry->_motion = motion;

    _motions.PushBack(motionQueueEntry, false);

    return motionQueueEntry->_motionQueueEntryHandle;
}

csmBool CubismMotionQueueManager::DoUpdateMotion(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32* opacity)
{
    csmBool updated = false;

    // ------- 処理を行う --------
    // 既にモーションがあれば終了フラグを立てる

    for (csmVector<CubismMotionQueueEntry*>::iterator ite = _motions.Begin(); ite != _motions.End();)
    {
        CubismMotionQueueEntry* motionQueueEntry = *ite;

        if (motionQueueEntry == NULL)
        {
            ite = _motions.Erase(ite);          // 削除
            continue;
        }

        ACubismMotion* motion = motionQueueEntry->_motion;

        if (motion == NULL)
        {
            CSM_DELETE(motionQueueEntry);
            ite = _motions.Erase(ite);          // 削除

            continue;
        }

        // ------ 値を反映する ------
        motion->UpdateParameters(model, motionQueueEntry, userTimeSeconds);
        updated = true;

        // ------ 不透明度の値が存在すれば反映する ------
        if (opacity)
        {
            *opacity = motion->GetOpacityValue(userTimeSeconds - motionQueueEntry->GetStartTime());
        }

        // ------ ユーザトリガーイベントを検査する ----
        const csmVector<const csmString*>& firedList = motion->GetFiredEvent(
            motionQueueEntry->GetLastCheckEventTime() - motionQueueEntry->GetStartTime()
            , userTimeSeconds - motionQueueEntry->GetStartTime()
        );

        for (csmUint32 i = 0; i < firedList.GetSize(); ++i)
        {
            _eventCallback(this, *(firedList[i]), _eventCustomData);
        }

        motionQueueEntry->SetLastCheckEventTime(userTimeSeconds);

        // ----- 終了済みの処理があれば削除する ------
        if (motionQueueEntry->IsFinished())
        {
            CSM_DELETE(motionQueueEntry);
            ite = _motions.Erase(ite);          // 削除
        }
        else
        {
            if (motionQueueEntry->IsTriggeredFadeOut())
            {
                motionQueueEntry->StartFadeout(motionQueueEntry->GetFadeOutSeconds(), userTimeSeconds);
            }

            ++ite;
        }
    }

    return updated;
}

CubismMotionQueueEntry* CubismMotionQueueManager::GetCubismMotionQueueEntry(CubismMotionQueueEntryHandle motionQueueEntryNumber)
{
    //------- 処理を行う --------
    //既にモーションがあれば終了フラグを立てる

    for (csmVector<CubismMotionQueueEntry*>::iterator ite = _motions.Begin(); ite != _motions.End(); ++ite)
    {
        CubismMotionQueueEntry* motionQueueEntry = *ite;

        if (motionQueueEntry == NULL)
        {
            continue;
        }

        if (motionQueueEntry->_motionQueueEntryHandle == motionQueueEntryNumber)
        {
            return motionQueueEntry;
        }
    }

    return NULL;
}

csmBool CubismMotionQueueManager::IsFinished()
{
    // ------- 処理を行う --------
    // 既にモーションがあれば終了フラグを立てる

    for (csmVector<CubismMotionQueueEntry*>::iterator ite = _motions.Begin(); ite != _motions.End();)
    {
        CubismMotionQueueEntry* motionQueueEntry = *ite;

        if (motionQueueEntry == NULL)
        {
            ite = _motions.Erase(ite);          // 削除
            continue;
        }

        ACubismMotion* motion = motionQueueEntry->_motion;

        if (motion == NULL)
        {
            CSM_DELETE(motionQueueEntry);
            ite = _motions.Erase(ite);          // 削除
            continue;
        }

        // ----- 終了済みの処理があれば削除する ------
        if (!motionQueueEntry->IsFinished())
        {
            return false;
        }
        else
        {
            ++ite;
        }
    }

    return true;
}

csmBool CubismMotionQueueManager::IsFinished(CubismMotionQueueEntryHandle motionQueueEntryNumber)
{
    // 既にモーションがあれば終了フラグを立てる

    for (csmVector<CubismMotionQueueEntry*>::iterator ite = _motions.Begin(); ite != _motions.End(); ite++)
    {
        CubismMotionQueueEntry* motionQueueEntry = *ite;

        if (motionQueueEntry == NULL)
        {
            continue;
        }

        if (motionQueueEntry->_motionQueueEntryHandle == motionQueueEntryNumber && !motionQueueEntry->IsFinished())
        {
            return false;
        }
    }

    return true;
}

void CubismMotionQueueManager::StopAllMotions()
{
    // ------- 処理を行う --------
    // 既にモーションがあれば終了フラグを立てる

    for (csmVector<CubismMotionQueueEntry*>::iterator ite = _motions.Begin(); ite != _motions.End();)
    {
        CubismMotionQueueEntry* motionQueueEntry = *ite;

        if (motionQueueEntry == NULL)
        {
            ite = _motions.Erase(ite);

            continue;
        }

        // ----- 終了済みの処理があれば削除する ------
        CSM_DELETE(motionQueueEntry);
        ite = _motions.Erase(ite); //削除
    }
}

void CubismMotionQueueManager::SetEventCallback(CubismMotionEventFunction callback, void* customData)
{
    _eventCallback   = callback;
    _eventCustomData = customData;
}

}}}
