/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

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
#include "base/CCUserDefault.h"
#include "platform/CCCommon.h"
#include "platform/CCFileUtils.h"
#include "pugixml/pugixml_imp.hpp"
#include "base/base64.h"
#include "base/ccUtils.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS && CC_TARGET_PLATFORM != CC_PLATFORM_MAC && CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)

// root name of xml
#define USERDEFAULT_ROOT_NAME    "userDefaultRoot"

#define XML_FILE_NAME "UserDefault.xml"

using namespace std;

NS_CC_BEGIN

/**
 * define the functions here because we don't want to
 * export xmlNodePtr and other types in "CCUserDefault.h"
 */

static pugi::xml_node getXMLNodeForKey(const char* pKey, pugi::xml_node& rootNode, pugi::xml_document& doc)
{
    pugi::xml_node curNode;

    // check the key value
    if (! pKey)
    {
        return pugi::xml_node{};
    }

    do 
    {
        std::string xmlBuffer = FileUtils::getInstance()->getStringFromFile(UserDefault::getInstance()->getXMLFilePath());

        if (!xmlBuffer.empty())
        {
            doc.load_buffer_inplace(&xmlBuffer.front(), xmlBuffer.size());

            // get root node
            rootNode = doc.document_element();
        }

        if (!rootNode)
        {
            // create root element
            auto rootEle = doc.append_child(USERDEFAULT_ROOT_NAME);
            if (!rootEle)
                break;

            rootNode = rootEle;
        }

        curNode = rootNode.child(pKey);

    } while (0);

    return curNode;
}

static void setValueForKey(const char* pKey, const char* pValue)
{
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    // check the params
    if (! pKey || ! pValue)
    {
        return;
    }
    // find the node
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // if node exist, change the content
    if (node)
    {
        node.set_value(pValue);
    }
    else
    {
        if (rootNode)
        {
            node = rootNode.append_child(pKey);
            node.set_value(pValue);
        }    
    }

    // save file and free doc
    if (doc)
    {
        doc.save_file(UserDefault::getInstance()->getXMLFilePath().c_str(), "  ");
    }
}

/**
 * implements of UserDefault
 */

UserDefault* UserDefault::_userDefault = nullptr;
string UserDefault::_filePath = string("");
bool UserDefault::_isFilePathInitialized = false;

UserDefault::~UserDefault()
{
}

UserDefault::UserDefault()
{
}

bool UserDefault::getBoolForKey(const char* pKey)
{
 return getBoolForKey(pKey, false);
}

bool UserDefault::getBoolForKey(const char* pKey, bool defaultValue)
{
    const char* value = nullptr;
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // find the node
    if (node)
    {
        value = (const char*)node.text().as_string();
    }

    bool ret = defaultValue;

    if (value)
    {
        ret = (! strcmp(value, "true"));
    }

    return ret;
}

int UserDefault::getIntegerForKey(const char* pKey)
{
    return getIntegerForKey(pKey, 0);
}

int UserDefault::getIntegerForKey(const char* pKey, int defaultValue)
{
    const char* value = nullptr;
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // find the node
    if (node)
    {
        value = (const char*)(const char*)node.text().as_string();
    }

    int ret = defaultValue;

    if (value)
    {
        ret = atoi(value);
    }

    return ret;
}

float UserDefault::getFloatForKey(const char* pKey)
{
    return getFloatForKey(pKey, 0.0f);
}

float UserDefault::getFloatForKey(const char* pKey, float defaultValue)
{
    float ret = (float)getDoubleForKey(pKey, (double)defaultValue);
 
    return ret;
}

double  UserDefault::getDoubleForKey(const char* pKey)
{
    return getDoubleForKey(pKey, 0.0);
}

double UserDefault::getDoubleForKey(const char* pKey, double defaultValue)
{
    const char* value = nullptr;
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // find the node
    if (node)
    {
        value = (const char*)(const char*)node.text().as_string();
    }

    double ret = defaultValue;

    if (value)
    {
        ret = utils::atof(value);
    }

    return ret;
}

std::string UserDefault::getStringForKey(const char* pKey)
{
    return getStringForKey(pKey, "");
}

string UserDefault::getStringForKey(const char* pKey, const std::string & defaultValue)
{
    const char* value = nullptr;
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // find the node
    if (node)
    {
        value = (const char*)(const char*)node.text().as_string();
    }

    string ret = defaultValue;

    if (value)
    {
        ret = string(value);
    }

    return ret;
}

Data UserDefault::getDataForKey(const char* pKey)
{
    return getDataForKey(pKey, Data::Null);
}

Data UserDefault::getDataForKey(const char* pKey, const Data& defaultValue)
{
    const char* encodedData = nullptr;
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;
    node = getXMLNodeForKey(pKey, rootNode, doc);
    // find the node
    if (node)
    {
        encodedData = (const char*)(const char*)node.text().as_string();
    }
    
    Data ret;
    
    if (encodedData)
    {
        unsigned char * decodedData = nullptr;
        int decodedDataLen = base64Decode((unsigned char*)encodedData, (unsigned int)strlen(encodedData), &decodedData);
        
        if (decodedData) {
            ret.fastSet(decodedData, decodedDataLen);
        }
    }
    else
    {
        ret = defaultValue;
    }

    return ret;    
}


void UserDefault::setBoolForKey(const char* pKey, bool value)
{
    // save bool value as string

    if (true == value)
    {
        setStringForKey(pKey, "true");
    }
    else
    {
        setStringForKey(pKey, "false");
    }
}

void UserDefault::setIntegerForKey(const char* pKey, int value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, 50);
    sprintf(tmp, "%d", value);

    setValueForKey(pKey, tmp);
}

void UserDefault::setFloatForKey(const char* pKey, float value)
{
    setDoubleForKey(pKey, value);
}

void UserDefault::setDoubleForKey(const char* pKey, double value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, 50);
    sprintf(tmp, "%f", value);

    setValueForKey(pKey, tmp);
}

void UserDefault::setStringForKey(const char* pKey, const std::string & value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    setValueForKey(pKey, value.c_str());
}

void UserDefault::setDataForKey(const char* pKey, const Data& value) {
    // check key
    if (! pKey)
    {
        return;
    }

    char *encodedData = nullptr;
    
    base64Encode(value.getBytes(), static_cast<unsigned int>(value.getSize()), &encodedData);
        
    setValueForKey(pKey, encodedData);
    
    if (encodedData)
        free(encodedData);
}

UserDefault* UserDefault::getInstance()
{
    if (!_userDefault)
    {
        initXMLFilePath();

        // only create xml file one time
        // the file exists after the program exit
        if ((!isXMLFileExist()) && (!createXMLFile()))
        {
            return nullptr;
        }

        _userDefault = new (std::nothrow) UserDefault();
    }

    return _userDefault;
}

void UserDefault::destroyInstance()
{
    CC_SAFE_DELETE(_userDefault);
}

void UserDefault::setDelegate(UserDefault *delegate)
{
    if (_userDefault)
        delete _userDefault;

    _userDefault = delegate;
}


bool UserDefault::isXMLFileExist()
{
    return FileUtils::getInstance()->isFileExist(_filePath);
}

void UserDefault::initXMLFilePath()
{
    if (! _isFilePathInitialized)
    {
        _filePath += FileUtils::getInstance()->getWritablePath() + XML_FILE_NAME;
        _isFilePathInitialized = true;
    }    
}

// create new xml file
bool UserDefault::createXMLFile()
{
    bool bRet = false;  

    pugi::xml_document doc;
    doc.load_string(R"(<?xml version="1.0" encoding="UTF-8" ?>)"
"<" USERDEFAULT_ROOT_NAME " />", pugi::parse_full);

    bRet = doc.save_file(_filePath.c_str(), " ");

    return bRet;
}

const string& UserDefault::getXMLFilePath()
{
    return _filePath;
}

void UserDefault::flush()
{
}

void UserDefault::deleteValueForKey(const char* key)
{
    pugi::xml_node rootNode;
    pugi::xml_document doc;
    pugi::xml_node node;

    // check the params
    if (!key)
    {
        CCLOG("the key is invalid");
        return;
    }

    // find the node
    node = getXMLNodeForKey(key, rootNode, doc);

    // if node not exist, don't need to delete
    if (!node)
    {
        return;
    }

    // save file and free doc
    if (doc)
    {
        rootNode.remove_child(node);
        doc.save_file(UserDefault::getInstance()->getXMLFilePath().c_str(), "  ");
    }

    flush();
}

NS_CC_END

#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_IOS && CC_PLATFORM != CC_PLATFORM_ANDROID)
