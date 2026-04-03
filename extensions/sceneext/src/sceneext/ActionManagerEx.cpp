/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "sceneext/ActionManagerEx.h"
#include "sceneext/CocoLoader.h"

using namespace ax;

namespace ax::ext
{

static ActionManagerEx* sharedActionManager = nullptr;

ActionManagerEx* ActionManagerEx::getInstance()
{
    if (!sharedActionManager)
    {
        sharedActionManager = new ActionManagerEx();
    }
    return sharedActionManager;
}

void ActionManagerEx::destroyInstance()
{
    if (sharedActionManager != nullptr)
    {
        sharedActionManager->releaseActions();
        AX_SAFE_DELETE(sharedActionManager);
    }
}

ActionManagerEx::ActionManagerEx() : _studioVersionNumber(0) {}

ActionManagerEx::~ActionManagerEx()
{
    _actionDic.clear();
}

void ActionManagerEx::initWithDictionary(std::string_view jsonName,
                                         const rapidjson::Value& dic,
                                         Object* root,
                                         int version)
{
    std::string path{jsonName};
    this->_studioVersionNumber = version;
    ssize_t pos                = path.find_last_of("/");
    std::string fileName       = path.substr(pos + 1, path.length());
    ax::Vector<ActionObject*> actionList;
    int actionCount = DICTOOL->getArrayCount_json(dic, "actionlist");
    for (int i = 0; i < actionCount; i++)
    {
        ActionObject* action = new ActionObject();
        action->autorelease();
        const rapidjson::Value& actionDic = DICTOOL->getDictionaryFromArray_json(dic, "actionlist", i);
        action->initWithDictionary(actionDic, root);
        actionList.pushBack(action);
    }
    _actionDic[fileName] = actionList;
}

void ActionManagerEx::initWithBinary(std::string_view file,
                                     ax::Object* root,
                                     CocoLoader* cocoLoader,
                                     stExpCocoNode* pCocoNode)
{
    std::string path{file};
    ssize_t pos          = path.find_last_of("/");
    std::string fileName = path.substr(pos + 1, path.length());
    ax::Vector<ActionObject*> actionList;

    stExpCocoNode* stChildArray = pCocoNode->GetChildArray(cocoLoader);
    stExpCocoNode* actionNode   = nullptr;
    for (int i = 0; i < pCocoNode->GetChildNum(); ++i)
    {
        std::string key = stChildArray[i].GetName(cocoLoader);
        if (key == "actionlist")
        {
            actionNode = &stChildArray[i];
            break;
        }
    }
    if (nullptr != actionNode)
    {
        int actionCount = actionNode->GetChildNum();
        for (int i = 0; i < actionCount; ++i)
        {
            ActionObject* action = new ActionObject();
            action->autorelease();

            action->initWithBinary(cocoLoader, &actionNode->GetChildArray(cocoLoader)[i], root);

            actionList.pushBack(action);
        }
    }
    _actionDic[fileName] = actionList;
}

ActionObject* ActionManagerEx::getActionByName(std::string_view jsonName, std::string_view actionName)
{
    std::string path{jsonName};
    ssize_t pos          = path.find_last_of("/");
    std::string fileName = path.substr(pos + 1, path.length());
    auto iterator        = _actionDic.find(fileName);
    if (iterator == _actionDic.end())
    {
        return nullptr;
    }
    auto actionList = iterator->second;
    for (int i = 0; i < actionList.size(); i++)
    {
        ActionObject* action = actionList.at(i);
        if (actionName == action->getName())
        {
            return action;
        }
    }
    return nullptr;
}

ActionObject* ActionManagerEx::playActionByName(std::string_view jsonName, std::string_view actionName)
{
    ActionObject* action = getActionByName(jsonName, actionName);
    if (action)
    {
        action->play();
    }
    return action;
}

ActionObject* ActionManagerEx::playActionByName(std::string_view jsonName, std::string_view actionName, CallFunc* func)
{
    ActionObject* action = getActionByName(jsonName, actionName);
    if (action)
    {
        action->play(func);
    }
    return action;
}

ActionObject* ActionManagerEx::stopActionByName(std::string_view jsonName, std::string_view actionName)
{
    ActionObject* action = getActionByName(jsonName, actionName);
    if (action)
    {
        action->stop();
    }
    return action;
}

void ActionManagerEx::releaseActions()
{
    for (auto&& iter : _actionDic)
    {
        ax::Vector<ActionObject*> objList = iter.second;
        ssize_t listCount                 = objList.size();
        for (ssize_t i = 0; i < listCount; i++)
        {
            ActionObject* action = objList.at(i);
            if (action != nullptr)
            {
                action->stop();
            }
        }
        objList.clear();
    }

    _actionDic.clear();
}

int ActionManagerEx::getStudioVersionNumber() const
{
    return this->_studioVersionNumber;
}

}  // namespace ax::ext
