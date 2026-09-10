/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ActionNode.h"
#include "axmol/2d/ActionInstant.h"
#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class CocoLoader;
struct stExpCocoNode;

/**
 *  @lua NA
 */
class SCNEXT_API ActionObject : public ax::Object
{
public:
    /**
     * Default constructor
     */
    ActionObject();

    /**
     * Default destructor
     */
    virtual ~ActionObject();

    /**
     * Sets name for object
     *
     * @param name    name of object
     */
    void setName(std::string_view name);

    /**
     * Gets name of object
     *
     * @return name of object
     */
    std::string_view getName() const;

    /**
     * Sets if the action will loop play.
     *
     * @param bLoop     that if the action will loop play
     */
    void setLoop(bool bLoop);

    /**
     * Gets if the action will loop play.
     *
     * @return   that if the action will loop play
     */
    bool getLoop();

    /**
     * Sets the time interval of frame.
     *
     * @param fTime   the time interval of frame
     */
    void setUnitTime(float fTime);

    /**
     * Gets the time interval of frame.
     *
     * @return the time interval of frame
     */
    float getUnitTime();

    /**
     * Sets the current time of frame.
     *
     * @param fTime   the current time of frame
     */
    void setCurrentTime(float fTime);

    /**
     * Gets the current time of frame.
     *
     * @return the current time of frame
     */
    float getCurrentTime();

    /**
     * Gets the total time of frame.
     *
     * @return the total time of frame
     */
    float getTotalTime();

    /**
     * Return if the action is playing.
     *
     * @return true if the action is playing, false the otherwise
     */
    bool isPlaying();

    /**
     * Play the action.
     */
    void play();

    /**
     * Play the action.
     *
     * @param func Action Call Back
     */
    void play(ax::CallFunc* func);

    /**
     * Pause the action.
     */
    void pause();

    /**
     * Stop the action.
     */
    void stop();

    /**
     * Adds a ActionNode to play the action.
     *
     * @param node    the ActionNode which will play the action
     */
    void addActionNode(ActionNode* node);

    /**
     * Removes a ActionNode which play the action.
     *
     * @param node    the ActionNode which play the action
     */
    void removeActionNode(ActionNode* node);

    /*update frame method*/
    void updateToFrameByTime(float fTime);

    /*init properties with a json dictionary*/
    void initWithDictionary(const rapidjson::Value& dic, ax::Object* root);

    void initWithBinary(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, ax::Object* root);

    /*scheduler update function*/
    void simulationActionUpdate(float dt);

protected:
    int valueToInt(std::string_view value);
    bool valueToBool(std::string_view value);
    float valueToFloat(std::string_view value);

    ax::Vector<ActionNode*> _actionNodeList;
    std::string _name;
    bool _loop;
    bool _bPause;
    bool _bPlaying;
    float _fUnitTime;
    float _currentTime;
    ax::Scheduler* _pScheduler;
    ax::CallFunc* _CallBack;
    float _fTotalTime;
};

}  // namespace ax::ext
