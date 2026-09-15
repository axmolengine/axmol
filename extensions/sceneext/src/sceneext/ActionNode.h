/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ActionFrame.h"
#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class CocoLoader;
struct stExpCocoNode;
/**
 *  @lua NA
 */
class SCNEXT_API ActionNode : public ax::Object
{
public:
    /**
     * Default constructor
     */
    ActionNode();

    /**
     * Default destructor
     */
    virtual ~ActionNode();
    /**
     * Sets the time interval of frame.
     *
     * @param fTime   the time interval of frame
     */
    void setUnitTime(float fTime);

    /**
     * Gets the time interval of frame.
     *
     * @return fTime   the time interval of frame
     */
    float getUnitTime();
    /**
     * Sets tag for ActionNode
     *
     * @param tag    tag of ActionNode
     */
    void setActionTag(int tag);

    /**
     * Gets tag for ActionNode
     *
     * @return tag    tag of ActionNode
     */
    int getActionTag();

    /**
     * Sets node which will run a action.
     *
     * @param  node which will run a action
     */
    void setObject(ax::Object* node);

    /**
     * Gets node which will run a action.
     *
     * @return  node which will run a action
     */
    ax::Object* getObject();

    /**
     * Insets a ActionFrame to ActionNode.
     *
     * @param index  the index of ActionFrame
     *
     * @param frame  the ActionFrame which will be inserted
     */
    void insertFrame(int index, ActionFrame* frame);

    /**
     * Pushes back a ActionFrame to ActionNode.
     *
     * @param frame  the ActionFrame which will be added
     */
    void addFrame(ActionFrame* frame);

    /**
     * Remove a ActionFrame from ActionNode.
     *
     * @param frame  the ActionFrame which will be removed
     */
    void deleteFrame(ActionFrame* frame);

    /**
     * Remove all ActionFrames from ActionNode.
     */
    void clearAllFrame();

    /**
     * Gets index of first ActionFrame.
     *
     * @return  index of first ActionFrame
     */
    int getFirstFrameIndex();

    /**
     * Gets index of last ActionFrame.
     *
     * @return  index of last ActionFrame
     */
    int getLastFrameIndex();

    /**
     * Updates action states to some time.
     *
     * @param fTime   the time when need to update
     */
    virtual bool updateActionToTimeLine(float fTime);

    /**
     * Play the action.
     */
    virtual void playAction();

    /**
     * Stop the action.
     */
    virtual void stopAction();

    /*init properties with a json dictionary*/
    virtual void initWithDictionary(const rapidjson::Value& dic, ax::Object* root);
    virtual void initWithBinary(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, Object* root);

    /**
     * Gets if the action is done once time.
     *
     * @return   that if the action is done once time
     */
    virtual bool isActionDoneOnce();

protected:
    int valueToInt(std::string_view value);
    bool valueToBool(std::string_view value);
    float valueToFloat(std::string_view value);

    int _currentFrameIndex;
    int _destFrameIndex;

    float _fUnitTime;

    int _actionTag;
    ax::Spawn* _actionSpawn;
    ax::Action* _action;
    ax::Object* _object;

    std::vector<ax::Vector<ActionFrame*>*> _frameArray;
    int _frameArrayNum;

protected:
    virtual ax::Node* getActionNode();
    virtual ax::Spawn* refreshActionProperty();
    virtual void runAction();
    virtual void initActionNodeFromRoot(ax::Object* root);
    virtual void easingToFrame(float duration, float delayTime, ActionFrame* srcFrame, ActionFrame* destFrame);
};

}  // namespace ax::ext
