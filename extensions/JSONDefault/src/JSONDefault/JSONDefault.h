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
#ifndef __SUPPORT_JSONDEFAULT_H__
#define __SUPPORT_JSONDEFAULT_H__

#include "platform/PlatformMacros.h"
#include "extensions/ExtensionMacros.h"
#include <string>

#include "yasio/string_view.hpp"

// rapidjson
#include "rapidjson/document.h"

/**
 * @addtogroup extensions
 * @{
 */
NS_AX_EXT_BEGIN

/**
 * JSONDefault acts as a tiny database. You can save and get base type values by
 * it. For example, setBoolForKey("played", true) will add a bool value true
 * into the database. Its key is "played". You can get the value of the key by
 * getBoolForKey("played").
 *
 * It supports the following base types:
 * bool, int, float, double, string
 *
 */
class JSONDefault
{
public:
    // get value methods

    /**
     * Get bool value by key, if the key doesn't exist, will return false.
     * You can set the default value, or it is false.
     * @param key The key to get value.
     * @return Bool value by `key`.
     * @js NA
     */
    bool getBoolForKey(const char* key);

    /**
     * Get bool value by key, if the key doesn't exist, will return passed default
     * value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @js NA
     */
    virtual bool getBoolForKey(const char* key, bool defaultValue);

    /**
     * Get integer value by key, if the key doesn't exist, will return 0.
     * You can set the default value, or it is 0.
     * @param key The key to get value.
     * @return Integer value of the key.
     * @js NA
     */
    int getIntegerForKey(const char* key);

    /**
     * Get integer value by key, if the key doesn't exist, will return passed
     * default value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @return Integer value of the key.
     * @js NA
     */
    virtual int getIntegerForKey(const char* key, int defaultValue);

    /**
     * Get large integer value by key, if the key doesn't exist, will return 0.
     * You can set the default value, or it is 0.
     * @param key The key to get value.
     * @return Integer value of the key.
     * @js NA
     */
    int64_t getLargeIntForKey(const char* key);

    /**
     * Get large integer value by key, if the key doesn't exist, will return
     * passed default value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @return Integer value of the key.
     * @js NA
     */
    virtual int64_t getLargeIntForKey(const char* key, int64_t defaultValue);

    /**
     * Get float value by key, if the key doesn't exist, will return 0.0.
     * @param key The key to get value.
     * @return Float value of the key.
     * @js NA
     */
    float getFloatForKey(const char* key);

    /**
     * Get float value by key, if the key doesn't exist, will return passed
     * default value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @return Float value of the key.
     * @js NA
     */
    virtual float getFloatForKey(const char* key, float defaultValue);

    /**
     * Get double value by key, if the key doesn't exist, will return 0.0.
     * @param key The key to get value.
     * @return Double value of the key.
     * @js NA
     */
    double getDoubleForKey(const char* key);

    /**
     * Get double value by key, if the key doesn't exist, will return passed
     * default value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @return Double value of the key.
     * @js NA
     */
    virtual double getDoubleForKey(const char* key, double defaultValue);

    /**
     * Get string value by key, if the key doesn't exist, will return an empty
     * string.
     * @param key The key to get value.
     * @return String value of the key.
     * @js NA
     */
    std::string_view getStringForKey(const char* key);

    /**
     * Get string value by key, if the key doesn't exist, will return passed
     * default value.
     * @param key The key to get value.
     * @param defaultValue The default value to return if the key doesn't exist.
     * @return String value of the key.
     * @js NA
     */
    virtual std::string_view getStringForKey(const char* key, std::string_view defaultValue);

    // set value methods

    /**
     * Set bool value by key.
     * @param key The key to set.
     * @param value A bool value to set to the key.
     * @js NA
     */
    virtual void setBoolForKey(const char* key, bool value);
    /**
     * Set integer value by key.
     * @param key The key to set.
     * @param value A integer value to set to the key.
     * @js NA
     */
    virtual void setIntegerForKey(const char* key, int value);
    /**
     * Set large integer value by key.
     * @param key The key to set.
     * @param value A integer value to set to the key.
     * @js NA
     */
    virtual void setLargeIntForKey(const char* key, int64_t value);
    /**
     * Set float value by key.
     * @param key The key to set.
     * @param value A float value to set to the key.
     * @js NA
     */
    virtual void setFloatForKey(const char* key, float value);
    /**
     * Set double value by key.
     * @param key The key to set.
     * @param value A double value to set to the key.
     * @js NA
     */
    virtual void setDoubleForKey(const char* key, double value);
    /**
     * Set string value by key.
     * @param key The key to set.
     * @param value A string value to set to the key.
     * @js NA
     */
    virtual void setStringForKey(const char* key, const char* value);

    /**
     * Since we reimplement JSONDefault with file mapping io,
     * you don't needs call this function manually
     * @param pretty To save json on pretty form.
     * @js NA
     */
    virtual void flush(bool pretty = false);

    /**
     * Remove all keys and empty data-store,
     * be carefull, you will lose all your save data.
     * @js NA
     */
    virtual void reset();

    /**
     * delete any value by key,
     * @param key The key to delete value.
     * @js NA
     */
    virtual void deleteValueForKey(const char* key);

    /** Returns the singleton.
     * @js NA
     * @lua NA
     */
    static JSONDefault* getInstance();
    /**
     * @js NA
     */
    static void destroyInstance();

    /**
     * You can inherit from platform dependent implementation of JSONDefault, such
     * as JSONDefaultAndroid, and use this function to set delegate, then
     * JSONDefault will invoke delegate's implementation. For example, your store
     * native data base or other format store.
     *
     * If you don't want to system default implementation after setting delegate,
     * you can just pass nullptr to this function.
     *
     * @warning It will delete previous delegate
     */
    static void setDelegate(JSONDefault* delegate);

protected:
    JSONDefault();
    virtual ~JSONDefault();

    void lazyInit();

protected:
    rapidjson::Document _doc;

    static JSONDefault* _jsonDefault;
    std::string _filePath;
    bool _initialized = false;

    // encrpyt args
    bool _encryptEnabled = false;
    std::string _key;
    std::string _iv;
};

NS_AX_EXT_END
// end of extensions group
/** @} */

#endif  // __SUPPORT_JSONDEFAULT_H__
