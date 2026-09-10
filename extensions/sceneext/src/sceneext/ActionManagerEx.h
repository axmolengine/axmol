/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ActionObject.h"
#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class CocoLoader;
struct stExpCocoNode;

class SCNEXT_API ActionManagerEx : public ax::Object
{
public:
    /**
     * Default constructor
     */
    ActionManagerEx();

    /**
     * Default destructor
     * @lua NA
     */
    virtual ~ActionManagerEx();

    /**
     * Gets the static instance of ActionManager.
     * @lua getInstance
     */
    static ActionManagerEx* getInstance();

    /**
     * Purges ActionManager point.
     * @lua destroyActionManager
     */
    static void destroyInstance();

    /**
     * Gets an ActionObject with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UI file.
     *
     * @return  ActionObject which named as the param name
     */
    ActionObject* getActionByName(std::string_view jsonName, std::string_view actionName);

    /**
     * Play an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @return  ActionObject which named as the param name
     */
    ActionObject* playActionByName(std::string_view jsonName, std::string_view actionName);

    /**
     * Play an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @param func ui action call back
     */
    ActionObject* playActionByName(std::string_view jsonName, std::string_view actionName, ax::CallFunc* func);

    /**
     * Stop an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @return  ActionObject which named as the param name
     */
    ActionObject* stopActionByName(std::string_view jsonName, std::string_view actionName);

    /*init properties with json dictionary*/
    void initWithDictionary(std::string_view jsonName, const rapidjson::Value& dic, Object* root, int version = 1600);
    void initWithBinary(std::string_view file, Object* root, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);

    /**
     * Release all actions.
     *
     */
    void releaseActions();

    int getStudioVersionNumber() const;

protected:
    std::unordered_map<std::string, ax::Vector<ActionObject*>> _actionDic;
    int _studioVersionNumber;
};

}  // namespace ax::ext
