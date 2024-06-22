/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __COCOSSTUDIO_CCACTIONTIMELINENODE_H__
#define __COCOSSTUDIO_CCACTIONTIMELINENODE_H__

#include "ActionTimeline.h"
#include "TimelineMacro.h"

NS_TIMELINE_BEGIN

class ActionTimeline;

class CCS_DLL ActionTimelineNode : public ax::Node
{
public:
    static ActionTimelineNode* create(ax::Node* root, ActionTimeline* action);

    ActionTimelineNode();
    ~ActionTimelineNode();

    bool init() override;
    bool init(ax::Node* root, ActionTimeline* action);

    virtual void setRoot(ax::Node* root);
    virtual ax::Node* getRoot();

    virtual void setActionTimeline(ActionTimeline* action);
    virtual ActionTimeline* getActionTimeline();

protected:
    ax::Node* _root;
    ActionTimeline* _action;
};

NS_TIMELINE_END

#endif /*__COCOSSTUDIO_CCACTIONTIMELINENODE_H__*/
