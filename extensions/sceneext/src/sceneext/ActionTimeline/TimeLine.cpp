/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/ActionTimeline/TimeLine.h"
#include "sceneext/ActionTimeline/ActionTimeline.h"

NS_TIMELINE_BEGIN

Timeline* Timeline::create()
{
    Timeline* ret = new Timeline();
    ret->autorelease();
    return ret;
}

Timeline::Timeline()
    : _currentKeyFrame(nullptr)
    , _currentKeyFrameIndex(0)
    , _fromIndex(0)
    , _toIndex(0)
    , _betweenDuration(0)
    , _actionTag(0)
    , _ActionTimeline(nullptr)
    , _node(nullptr)
{}

Timeline::~Timeline() {}

void Timeline::gotoFrame(int frameIndex)
{
    if (_frames.size() == 0)
        return;

    binarySearchKeyFrame(frameIndex);
    apply(frameIndex);
}

void Timeline::stepToFrame(int frameIndex)
{
    if (_frames.size() == 0)
        return;

    updateCurrentKeyFrame(frameIndex);
    apply(frameIndex);
}

Timeline* Timeline::clone()
{
    Timeline* timeline   = Timeline::create();
    timeline->_actionTag = _actionTag;

    for (auto&& frame : _frames)
    {
        Frame* newFrame = frame->clone();
        timeline->addFrame(newFrame);
    }

    return timeline;
}

void Timeline::addFrame(Frame* frame)
{
    _frames.pushBack(frame);
    frame->setTimeline(this);
}

void Timeline::insertFrame(Frame* frame, int index)
{
    _frames.insert(index, frame);
    frame->setTimeline(this);
}

void Timeline::removeFrame(Frame* frame)
{
    _frames.eraseObject(frame);
    frame->setTimeline(nullptr);
}

void Timeline::setNode(Node* node)
{
    for (auto&& frame : _frames)
    {
        frame->setNode(node);
    }
}

Node* Timeline::getNode() const
{
    return _node;
}

void Timeline::apply(unsigned int frameIndex)
{
    if (_currentKeyFrame)
    {
        float currentPercent =
            _betweenDuration == 0 ? 0 : (frameIndex - _currentKeyFrameIndex) / (float)_betweenDuration;
        _currentKeyFrame->apply(currentPercent);
    }
}

void Timeline::binarySearchKeyFrame(unsigned int frameIndex)
{
    Frame* from = nullptr;
    Frame* to   = nullptr;

    long length         = _frames.size();
    bool needEnterFrame = false;

    do
    {
        if (frameIndex < _frames.at(0)->getFrameIndex())
        {
            if (_currentKeyFrameIndex >= _frames.at(0)->getFrameIndex())
                needEnterFrame = true;

            _fromIndex = 0;
            _toIndex   = 0;

            from = to             = _frames.at(0);
            _currentKeyFrameIndex = 0;
            _betweenDuration      = _frames.at(0)->getFrameIndex();
            break;
        }
        else if (frameIndex >= _frames.at(length - 1)->getFrameIndex())
        {
            _fromIndex = (int)(length - 1);
            _toIndex   = 0;

            from = to = _frames.at(length - 1);
            if (from->isEnterWhenPassed())
                needEnterFrame = true;

            _currentKeyFrameIndex = _frames.at(length - 1)->getFrameIndex();
            _betweenDuration      = 0;
            break;
        }

        long target = -1;
        long low = 0, high = length - 1;
        while (low <= high)
        {
            auto mid = (low + high) / 2;
            if (frameIndex >= _frames.at(mid)->getFrameIndex() && frameIndex < _frames.at(mid + 1)->getFrameIndex())
            {
                target = mid;
                break;
            }
            if (_frames.at(mid)->getFrameIndex() > frameIndex)
                high = mid - 1;
            else
                low = mid + 1;
        }

        _fromIndex = (int)target;

        if (length > 1)
            _toIndex = (int)(target + 1);
        else
            _toIndex = (int)target;

        from = _frames.at(_fromIndex);
        to   = _frames.at(_toIndex);

        if (target == 0 && _currentKeyFrameIndex < from->getFrameIndex())
            needEnterFrame = true;

        _currentKeyFrameIndex = from->getFrameIndex();
        _betweenDuration      = to->getFrameIndex() - from->getFrameIndex();
    } while (0);

    if (needEnterFrame || _currentKeyFrame != from)
    {
        _currentKeyFrame = from;
        _currentKeyFrame->onEnter(to, frameIndex);
    }
}

void Timeline::updateCurrentKeyFrame(unsigned int frameIndex)
{
    //! If play to current frame's front or back, then find current frame again
    if (frameIndex < _currentKeyFrameIndex || frameIndex >= _currentKeyFrameIndex + _betweenDuration)
    {
        Frame* from = nullptr;
        Frame* to   = nullptr;

        do
        {
            long length = _frames.size();

            if (frameIndex < _frames.at(0)->getFrameIndex())
            {
                from = to             = _frames.at(0);
                _currentKeyFrameIndex = 0;
                _betweenDuration      = _frames.at(0)->getFrameIndex();
                break;
            }
            else if (frameIndex >= _frames.at(length - 1)->getFrameIndex())
            {
                unsigned int lastFrameIndex = _frames.at(length - 1)->getFrameIndex();
                if (_currentKeyFrameIndex >= lastFrameIndex)
                    return;
                frameIndex = lastFrameIndex;
            }

            do
            {
                _fromIndex            = _toIndex;
                from                  = _frames.at(_fromIndex);
                _currentKeyFrameIndex = from->getFrameIndex();

                _toIndex = _fromIndex + 1;
                if ((ssize_t)_toIndex >= length)
                {
                    _toIndex = 0;
                }

                to = _frames.at(_toIndex);

                if (frameIndex == from->getFrameIndex())
                    break;
                if (frameIndex > from->getFrameIndex() && frameIndex < to->getFrameIndex())
                    break;
                if (from->isEnterWhenPassed())
                    from->onEnter(to, from->getFrameIndex());
            } while (true);

            if (_fromIndex == length - 1)
                to = from;

            _betweenDuration = to->getFrameIndex() - from->getFrameIndex();

        } while (0);

        _currentKeyFrame = from;
        _currentKeyFrame->onEnter(to, frameIndex);
    }
}

NS_TIMELINE_END
