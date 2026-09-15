/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ActionTimeline/Frame.h"
#include "sceneext/ActionTimeline/TimelineMacro.h"
#include "sceneext/SceneExtMacros.h"

NS_TIMELINE_BEGIN

class ActionTimeline;

class SCNEXT_API Timeline : public ax::Object
{
public:
    static Timeline* create();

    Timeline();
    virtual ~Timeline();

    virtual void gotoFrame(int frameIndex);
    virtual void stepToFrame(int frameIndex);

    virtual const ax::Vector<Frame*>& getFrames() const { return _frames; }

    virtual void addFrame(Frame* frame);
    virtual void insertFrame(Frame* frame, int index);
    virtual void removeFrame(Frame* frame);

    virtual void setActionTag(int tag) { _actionTag = tag; }
    virtual int getActionTag() const { return _actionTag; }

    virtual void setNode(ax::Node* node);
    virtual ax::Node* getNode() const;

    virtual void setActionTimeline(ActionTimeline* action) { _ActionTimeline = action; }
    virtual ActionTimeline* getActionTimeline() const { return _ActionTimeline; }

    virtual Timeline* clone();

protected:
    virtual void apply(unsigned int frameIndex);

    virtual void binarySearchKeyFrame(unsigned int frameIndex);
    virtual void updateCurrentKeyFrame(unsigned int frameIndex);

    ax::Vector<Frame*> _frames;
    Frame* _currentKeyFrame;
    unsigned int _currentKeyFrameIndex;

    unsigned int _fromIndex;
    unsigned int _toIndex;
    unsigned int _betweenDuration;
    unsigned int _actionTag;

    ActionTimeline* _ActionTimeline;
    ax::Node* _node;
};

NS_TIMELINE_END
