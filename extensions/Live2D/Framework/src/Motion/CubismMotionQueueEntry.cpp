/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMotionQueueEntry.hpp"
#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismMotionQueueEntry::CubismMotionQueueEntry()
    : _autoDelete(false)
    , _motion(NULL)
    , _available(true)
    , _finished(false)
    , _started(false)
    , _startTimeSeconds(-1.0f)
    , _fadeInStartTimeSeconds(0.0f)
    , _endTimeSeconds(-1.0f)
    , _stateTimeSeconds(0.0f)
    , _stateWeight(0.0f)
    , _lastEventCheckSeconds(0.0f)
    , _motionQueueEntryHandle(NULL)
    , _fadeOutSeconds(0.0f)
    , _IsTriggeredFadeOut(false)
{
    this->_motionQueueEntryHandle = this;
}

CubismMotionQueueEntry::~CubismMotionQueueEntry()
{
    if (_autoDelete && _motion)
    {
        ACubismMotion::Delete(_motion); //
    }
}

void CubismMotionQueueEntry::SetFadeout(csmFloat32 fadeOutSeconds)
{
    _fadeOutSeconds = fadeOutSeconds;
    _IsTriggeredFadeOut = true;
}

void CubismMotionQueueEntry::StartFadeout(csmFloat32 fadeOutSeconds, csmFloat32 userTimeSeconds)
{
    const csmFloat32 newEndTimeSeconds = userTimeSeconds + fadeOutSeconds;
    _IsTriggeredFadeOut = true;

    if (_endTimeSeconds < 0.0f || newEndTimeSeconds < _endTimeSeconds)
    {
        _endTimeSeconds = newEndTimeSeconds;
    }
}

csmBool CubismMotionQueueEntry::IsFinished() const
{
    return _finished;
}

csmBool CubismMotionQueueEntry::IsStarted() const
{
    return _started;
}

csmFloat32 CubismMotionQueueEntry::GetStartTime() const
{
    return _startTimeSeconds;
}

csmFloat32 CubismMotionQueueEntry::GetFadeInStartTime() const
{
    return _fadeInStartTimeSeconds;
}

csmFloat32 CubismMotionQueueEntry::GetEndTime() const
{
    return _endTimeSeconds;
}

void CubismMotionQueueEntry::SetStartTime(csmFloat32 startTime)
{
    this->_startTimeSeconds = startTime;
}

void CubismMotionQueueEntry::SetFadeInStartTime(csmFloat32 startTime)
{
    this->_fadeInStartTimeSeconds = startTime;
}

void CubismMotionQueueEntry::SetEndTime(csmFloat32 endTime)
{
    this->_endTimeSeconds = endTime;
}

void CubismMotionQueueEntry::IsFinished(csmBool f)
{
    this->_finished = f;
}

void CubismMotionQueueEntry::IsStarted(csmBool f)
{
    this->_started = f;
}

csmBool CubismMotionQueueEntry::IsAvailable() const
{
    return _available;
}

void CubismMotionQueueEntry::IsAvailable(csmBool v)
{
    this->_available = v;
}

void CubismMotionQueueEntry::SetState(csmFloat32 timeSeconds, csmFloat32 weight)
{
    this->_stateTimeSeconds = timeSeconds;
    this->_stateWeight = weight;
}

csmFloat32 CubismMotionQueueEntry::GetStateTime() const
{
    return this->_stateTimeSeconds;
}

csmFloat32 CubismMotionQueueEntry::GetStateWeight() const
{
    return this->_stateWeight;
}


csmFloat32 CubismMotionQueueEntry::GetLastCheckEventTime() const
{
    return this->_lastEventCheckSeconds;
}

void CubismMotionQueueEntry::SetLastCheckEventTime(csmFloat32 checkTime)
{
    this->_lastEventCheckSeconds = checkTime;
}

csmBool CubismMotionQueueEntry::IsTriggeredFadeOut()
{
    return this->_IsTriggeredFadeOut;
}

csmFloat32 CubismMotionQueueEntry::GetFadeOutSeconds()
{
    return this->_fadeOutSeconds;
}

ACubismMotion* CubismMotionQueueEntry::GetCubismMotion()
{
    return _motion;
}

}}}
