/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/AnimationController.h"

using namespace ax;

namespace ax::ext
{

AnimationController::AnimationController(void)
    : _animationScale(1)
    , _isPause(true)
    , _isComplete(true)
    , _isPlaying(false)
    , _currentPercent(0.0f)
    , _rawDuration(0)
    , _loopType(ANIMATION_LOOP_BACK)
    , _tweenEasing(ax::tweenfunc::Linear)
    , _animationInternal(1 / 60.0f)
    , _durationTween(0)
    , _currentFrame(0)
    , _curFrameIndex(0)
    , _nextFrameIndex(0)
    , _isLoopBack(false)
{}

AnimationController::~AnimationController(void) {}

void AnimationController::pause()
{
    _isPause   = true;
    _isPlaying = false;
}

void AnimationController::resume()
{
    _isPause   = false;
    _isPlaying = true;
}

void AnimationController::stop()
{
    _isComplete = true;
    _isPlaying  = false;
}

void AnimationController::play(int durationTo, int /*durationTween*/, int /*loop*/, int tweenEasing)
{
    _isComplete   = false;
    _isPause      = false;
    _isPlaying    = true;
    _currentFrame = 0;

    /*
     *  Set m_iTotalFrames to durationTo, it is used for change tween between two animation.
     *  When changing end, m_iTotalFrames will be set to _durationTween
     */
    _nextFrameIndex = durationTo;
    _tweenEasing    = (ax::tweenfunc::TweenType)tweenEasing;
}

void AnimationController::update(float dt)
{

    if (_isComplete || _isPause)
    {
        return;
    }

    /*
     *  Filter the m_iDuration <=0 and dt >1
     *  If dt>1, generally speaking  the reason is the device is stuck.
     */
    if (_rawDuration <= 0 || dt > 1)
    {
        return;
    }

    if (_nextFrameIndex <= 0)
    {
        _currentPercent = 1;
        _currentFrame   = 0;
    }
    else
    {
        /*
         *  update _currentFrame, every update add the frame passed.
         *  dt/_animationInternal determine it is not a frame animation. If frame speed changed, it will not make our
         *  animation speed slower or quicker.
         */
        _currentFrame += _animationScale * (dt / _animationInternal);

        _currentPercent = _currentFrame / _nextFrameIndex;

        /*
         *	if _currentFrame is bigger or equal than m_iTotalFrames, then reduce it until _currentFrame is
         *  smaller than m_iTotalFrames
         */
        _currentFrame = fmodf(_currentFrame, _nextFrameIndex);
    }

    updateHandler();
}

void AnimationController::gotoFrame(int frameIndex)
{
    if (_loopType == ANIMATION_NO_LOOP)
    {
        _loopType = ANIMATION_MAX;
    }
    else if (_loopType == ANIMATION_TO_LOOP_FRONT)
    {
        _loopType = ANIMATION_LOOP_FRONT;
    }

    _curFrameIndex = frameIndex;

    _nextFrameIndex = _durationTween;
}

int AnimationController::getCurrentFrameIndex()
{
    _curFrameIndex = (_rawDuration - 1) * _currentPercent;
    return _curFrameIndex;
}

}  // namespace ax::ext
