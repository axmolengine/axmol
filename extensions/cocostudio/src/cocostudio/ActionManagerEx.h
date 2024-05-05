/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

https://axmolengine.github.io/

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

#ifndef __ActionMANAGER_H__
#define __ActionMANAGER_H__

#include "ActionObject.h"
#include "DictionaryHelper.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

class CocoLoader;
struct stExpCocoNode;

class CCS_DLL ActionManagerEx : public ax::Object
{
public:
    /**
     * Default constructor
     * @js ctor
     */
    ActionManagerEx();

    /**
     * Default destructor
     * @js NA
     * @lua NA
     */
    virtual ~ActionManagerEx();

    /**
     * Gets the static instance of ActionManager.
     * @js getInstance
     * @lua getInstance
     */
    static ActionManagerEx* getInstance();

    /**
     * Purges ActionManager point.
     * @js purge
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
    ActionObject* getActionByName(const char* jsonName, const char* actionName);

    /**
     * Play an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @return  ActionObject which named as the param name
     */
    ActionObject* playActionByName(const char* jsonName, const char* actionName);

    /**
     * Play an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @param func ui action call back
     */
    ActionObject* playActionByName(const char* jsonName, const char* actionName, ax::CallFunc* func);

    /**
     * Stop an Action with a name.
     *
     * @param jsonName  UI file name
     *
     * @param actionName  action name in the UIfile.
     *
     * @return  ActionObject which named as the param name
     */
    ActionObject* stopActionByName(const char* jsonName, const char* actionName);

    /*init properties with json dictionary*/
    void initWithDictionary(const char* jsonName, const rapidjson::Value& dic, Object* root, int version = 1600);
    void initWithBinary(const char* file, Object* root, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);

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

}  // namespace cocostudio

#endif
