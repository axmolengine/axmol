/****************************************************************************
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __AX_LUA_VALUE_H_
#define __AX_LUA_VALUE_H_

#include <string>
#include <map>
#include <list>

extern "C" {
#include "lua.h"
}

#include "base/Types.h"
#include "base/Object.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_BLACKBERRY
using std::memcpy;
using std::memset;
#endif

/**
 * @addtogroup lua
 * @{
 */

NS_AX_BEGIN

typedef int LUA_FUNCTION;
typedef int LUA_TABLE;
typedef int LUA_STRING;

class LuaValue;

typedef std::map<std::string, LuaValue> LuaValueDict;
typedef LuaValueDict::const_iterator LuaValueDictIterator;
typedef std::list<LuaValue> LuaValueArray;
typedef LuaValueArray::const_iterator LuaValueArrayIterator;

/// @cond
typedef enum
{
    LuaValueTypeInt,
    LuaValueTypeFloat,
    LuaValueTypeBoolean,
    LuaValueTypeString,
    LuaValueTypeDict,
    LuaValueTypeArray,
    LuaValueTypeObject
} LuaValueType;
/// @endcond

/// @cond
typedef union
{
    int intValue;
    float floatValue;
    bool booleanValue;
    std::string* stringValue;
    LuaValueDict* dictValue;
    LuaValueArray* arrayValue;
    Object* ccobjectValue;
} LuaValueField;
/// @endcond

/**
 * Wrap different general types of data into a same specific type named LuaValue.
 * The general types supported as follows:int,float,bool,std::string,const char*,LuaValueDict,LuaValueArray,Object.
 *
 * @lua NA
 * @js NA
 */
class LuaValue
{
public:
    /**
     * Construct a LuaValue object by a int value.
     *
     * @param intValue a int value.
     * @return a LuaValue object.
     */
    static const LuaValue intValue(const int intValue);

    /**
     * Construct a LuaValue object by a float value.
     *
     * @param floatValue a float value.
     * @return a LuaValue object.
     */
    static const LuaValue floatValue(const float floatValue);

    /**
     * Construct a LuaValue object by a boolean value.
     *
     * @param booleanValue a bool value.
     * @return a LuaValue object.
     */
    static const LuaValue booleanValue(const bool booleanValue);

    /**
     * Construct a LuaValue object by a string pointer.
     *
     * @param stringValue a string pointer.
     * @return a LuaValue object.
     */
    static const LuaValue stringValue(const char* stringValue);

    /**
     * Construct a LuaValue object by a std::string object.
     *
     * @param stringValue a std::string object.
     * @return a LuaValue object.
     */
    static const LuaValue stringValue(std::string_view stringValue);

    /**
     * Construct a LuaValue object by a LuaValueDict value.
     *
     * @param dictValue a LuaValueDict object.
     * @return a LuaValue object.
     */
    static const LuaValue dictValue(const LuaValueDict& dictValue);

    /**
     * Construct a LuaValue object by a LuaValueArray value.
     *
     * @param arrayValue a LuaValueArray object.
     * @return a LuaValue object.
     */
    static const LuaValue arrayValue(const LuaValueArray& arrayValue);

    /**
     * Construct a LuaValue object by a Object object.
     *
     * @param ccobjectValue a Object object.
     * @param objectTypename a string pointer point to the typename of object.
     * @return a LuaValue object.
     */
    static const LuaValue ccobjectValue(Object* ccobjectValue, const char* objectTypename);

    /**
     * Construct a LuaValue object by a Object object.
     *
     * @param ccobjectValue a Object object.
     * @param objectTypename a std::string object represent the typename of object.
     * @return a LuaValue object.
     */
    static const LuaValue ccobjectValue(Object* ccobjectValue, std::string_view objectTypename);

    /**
     * Default constructor of LuaValue.
     * Set the default value for _type(LuaValueTypeInt) and _ccobjectType(nullptr),and init the _field.
     */
    LuaValue() : _type(LuaValueTypeInt), _ccobjectType(nullptr) { memset(&_field, 0, sizeof(_field)); }

    /**
     * Copy constructor of Data.
     */
    LuaValue(const LuaValue& rhs);

    /**
     * Override of operator= .
     */
    LuaValue& operator=(const LuaValue& rhs);

    /**
     * Destructor.
     */
    ~LuaValue();

    /**
     * Get the type of LuaValue object.
     *
     * @return the type of LuaValue object.
     */
    LuaValueType getType() const { return _type; }

    /**
     * Get the typename of the Object object.
     *
     * @return the reference of _ccobjectType.
     */
    std::string_view getObjectTypename() const { return *_ccobjectType; }

    /**
     * Get the int value of LuaValue object.
     *
     * @return the int value.
     */
    int intValue() const { return _field.intValue; }

    /**
     * Get the float value of LuaValue object.
     *
     * @return the float value.
     */
    float floatValue() const { return _field.floatValue; }

    /**
     * Get the boolean value of LuaValue object.
     *
     * @return the boolean value.
     */
    bool booleanValue() const { return _field.booleanValue; }

    /**
     * Get the boolean value of LuaValue object.
     *
     * @return the reference about string value.
     */
    std::string_view stringValue() const { return *_field.stringValue; }

    /**
     * Get the LuaValueDict value of LuaValue object.
     *
     * @return the LuaValueDict value.
     */
    const LuaValueDict& dictValue() const { return *_field.dictValue; }

    /**
     * Get the LuaValueArray value of LuaValue object.
     *
     * @return the LuaValueArray value.
     */
    const LuaValueArray& arrayValue() const { return *_field.arrayValue; }

    /**
     * Get the Object object of LuaValue object.
     *
     * @return the pointer point to a Object object.
     */
    Object* ccobjectValue() const { return _field.ccobjectValue; }

private:
    LuaValueField _field;
    LuaValueType _type;
    std::string* _ccobjectType;

    void copy(const LuaValue& rhs);
};

inline std::string axlua_tostr(lua_State* L, int arg)
{
    size_t l      = 0;
    const char* s = lua_tolstring(L, arg, &l);
    return std::string{s, l};
}

inline std::string_view axlua_tosv(lua_State* L, int arg)
{
    size_t l      = 0;
    const char* s = lua_tolstring(L, arg, &l);
    return std::string_view{s, l};
}

NS_AX_END

// end group
/// @}
#endif  // __AX_LUA_VALUE_H_
