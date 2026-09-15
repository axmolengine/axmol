/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "rapidjson/document-wrapper.h"
#include "sceneext/SceneExtMacros.h"

#define DICTOOL DictionaryHelper::getInstance()

namespace ax::ext
{
/**
 *  @lua NA
 */
class SCNEXT_API DictionaryHelper
{
public:
    DictionaryHelper();
    ~DictionaryHelper();
    static DictionaryHelper* getInstance();
    static void destroyInstance();

    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value& root, const char* key);
    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value& root, const char* key, int idx);
    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value& root, int idx);

    int getIntValue_json(const rapidjson::Value& root, const char* key, int def = 0);
    float getFloatValue_json(const rapidjson::Value& root, const char* key, float def = 0.0f);
    bool getBooleanValue_json(const rapidjson::Value& root, const char* key, bool def = false);
    const char* getStringValue_json(const rapidjson::Value& root, const char* key, const char* def = NULL);
    int getArrayCount_json(const rapidjson::Value& root, const char* key, int def = 0);

    int getIntValueFromArray_json(const rapidjson::Value& root, const char* arrayKey, int idx, int def = 0);
    float getFloatValueFromArray_json(const rapidjson::Value& root, const char* arrayKey, int idx, float def = 0.0f);
    bool getBoolValueFromArray_json(const rapidjson::Value& root, const char* arrayKey, int idx, bool def = false);
    const char* getStringValueFromArray_json(const rapidjson::Value& root,
                                             const char* arrayKey,
                                             int idx,
                                             const char* def = NULL);
    const rapidjson::Value& getDictionaryFromArray_json(const rapidjson::Value& root, const char* key, int idx);
    bool checkObjectExist_json(const rapidjson::Value& root);
    bool checkObjectExist_json(const rapidjson::Value& root, const char* key);
    bool checkObjectExist_json(const rapidjson::Value& root, int index);
};

}  // namespace ax::ext
