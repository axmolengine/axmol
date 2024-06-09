/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "JSONDefault.h"
#include "base/Utils.h"
#include "platform/Common.h"
#include "platform/FileUtils.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define JSON_DEFAULT_FILENAME "JSONDefault.json"

NS_AX_EXT_BEGIN

/**
 * implements of JSONDefault
 */

JSONDefault* JSONDefault::_jsonDefault = nullptr;

JSONDefault::~JSONDefault() {}

JSONDefault::JSONDefault()
{
    lazyInit();
}

JSONDefault* JSONDefault::getInstance()
{
    if (_jsonDefault)
        return _jsonDefault;

    return (_jsonDefault = new JSONDefault());
}

void JSONDefault::destroyInstance()
{
    AX_SAFE_DELETE(_jsonDefault);
}

void JSONDefault::setDelegate(JSONDefault* delegate)
{
    if (_jsonDefault)
        delete _jsonDefault;

    _jsonDefault = delegate;
}

void JSONDefault::lazyInit()
{
    if (_initialized)
        return;

    _doc.SetObject();

    _filePath = FileUtils::getInstance()->getWritablePath() + JSON_DEFAULT_FILENAME;

    if (FileUtils::getInstance()->isFileExist(_filePath))
    {
        auto data_str = FileUtils::getInstance()->getStringFromFile(_filePath);

        _doc.Parse(data_str.c_str());

        if (_doc.HasParseError())
        {
            AXLOGW("JSONDefault::init load json file: {} failed", _filePath);
            return;
        }
    }

    _initialized = true;
}

// Getters
bool JSONDefault::getBoolForKey(const char* key)
{
    return getBoolForKey(key, false);
}

bool JSONDefault::getBoolForKey(const char* key, bool defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetBool();
    return defaultValue;
}

int JSONDefault::getIntegerForKey(const char* key)
{
    return getIntegerForKey(key, 0);
}

int JSONDefault::getIntegerForKey(const char* key, int defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetInt();
    return defaultValue;
}

int64_t JSONDefault::getLargeIntForKey(const char* key)
{
    return getLargeIntForKey(key, 0);
}

int64_t JSONDefault::getLargeIntForKey(const char* key, int64_t defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetInt64();
    return defaultValue;
}

float JSONDefault::getFloatForKey(const char* key)
{
    return getFloatForKey(key, 0.0f);
}

float JSONDefault::getFloatForKey(const char* key, float defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetFloat();
    return defaultValue;
}

double JSONDefault::getDoubleForKey(const char* key)
{
    return getDoubleForKey(key, 0.0);
}

double JSONDefault::getDoubleForKey(const char* key, double defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetDouble();
    return defaultValue;
}

std::string_view JSONDefault::getStringForKey(const char* key)
{
    return getStringForKey(key, "");
}

std::string_view JSONDefault::getStringForKey(const char* key, std::string_view defaultValue)
{
    if (_doc.HasMember(key))
        return _doc[key].GetString();
    return defaultValue;
}

// Setters
void JSONDefault::setBoolForKey(const char* key, bool value)
{
    if (!key)
        return;
    if (_doc.HasMember(key))
    {
        _doc[key].SetBool(value);
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::Value(value), _doc.GetAllocator());
    }
    flush();
}

void JSONDefault::setIntegerForKey(const char* key, int value)
{
    if (!key)
        return;
    if (_doc.HasMember(key))
    {
        _doc[key].SetInt(value);
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::Value(value), _doc.GetAllocator());
    }
    flush();
}

void JSONDefault::setLargeIntForKey(const char* key, int64_t value)
{  // check key
    if (!key)
        return;
    if (_doc.HasMember(key))
    {
        _doc[key].SetInt64(value);
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::Value(value), _doc.GetAllocator());
    }
    flush();
}

void JSONDefault::setFloatForKey(const char* key, float value)
{
    if (!key)
        return;
    if (_doc.HasMember(key))
    {
        _doc[key].SetFloat(value);
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::Value(value), _doc.GetAllocator());
    }
    flush();
}

void JSONDefault::setDoubleForKey(const char* key, double value)
{
    if (!key)
        return;
    if (_doc.HasMember(key))
    {
        _doc[key].SetDouble(value);
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::Value(value), _doc.GetAllocator());
    }
    flush();
}

void JSONDefault::setStringForKey(const char* key, const char* value)
{
    if (!key)
        return;

    if (_doc.HasMember(key))
    {
        _doc[key].SetString(rapidjson::GenericStringRef(value));
    }
    else
    {
        _doc.AddMember(rapidjson::GenericStringRef(key), rapidjson::GenericStringRef(value), _doc.GetAllocator());
    }
    flush();
}

// Delete
void JSONDefault::deleteValueForKey(const char* key)
{
    if (!this->_doc.HasMember(key))
        return;

    this->_doc.RemoveMember(key);
    flush();
}

// Save
void JSONDefault::flush(bool pretty /*= false*/)
{
    rapidjson::StringBuffer buffer;

    if (pretty)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        _doc.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        _doc.Accept(writer);
    }
    FileUtils::getInstance()->writeStringToFile(buffer.GetString(), _filePath);
}

// Reset
void JSONDefault::reset()
{
    _doc.RemoveAllMembers();
    flush();
}

NS_AX_EXT_END
