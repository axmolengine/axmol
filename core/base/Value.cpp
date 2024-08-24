/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies
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

#include "base/Value.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include "base/Utils.h"

namespace ax
{

const ValueVector ValueVectorNull;
const ValueMap ValueMapNull;
const ValueMapIntKey ValueMapIntKeyNull;

const Value Value::Null;

const std::string Value::EmptyString;

Value::Value() : _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

Value::Value(unsigned char v) : _type(Type::INT_UI32)
{
    _field.uintVal = v;
}

Value::Value(int v) : _type(Type::INT_I32)
{
    _field.intVal = v;
}

Value::Value(unsigned int v) : _type(Type::INT_UI32)
{
    _field.uintVal = v;
}

/** Create a Value by an integer value. */
Value::Value(int64_t v) : _type(Type::INT_I64)
{
    _field.int64Val = v;
}

/** Create a Value by an integer value. */
Value::Value(uint64_t v) : _type(Type::INT_UI64)
{
    _field.uint64Val = v;
}

Value::Value(float v) : _type(Type::FLOAT)
{
    _field.floatVal = v;
}

Value::Value(double v) : _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

Value::Value(bool v) : _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

Value::Value(const char* v) : _type(Type::STRING)
{
    _field.strVal = new std::string(v ? v : "");
}

Value::Value(std::string_view v) : _type(Type::STRING)
{
    _field.strVal = new std::string(v);
}

Value::Value(std::string&& v) : _type(Type::STRING)
{
    _field.strVal = new std::string(std::move(v));
}

Value::Value(const ValueVector& v) : _type(Type::VECTOR)
{
    _field.vectorVal = new ValueVector(v);
}

Value::Value(ValueVector&& v) : _type(Type::VECTOR)
{
    _field.vectorVal = new ValueVector(std::move(v));
}

Value::Value(const ValueMap& v) : _type(Type::MAP)
{
    _field.mapVal = new ValueMap(v);
}

Value::Value(ValueMap&& v) : _type(Type::MAP)
{
    _field.mapVal = new ValueMap(std::move(v));
}

Value::Value(const ValueMapIntKey& v) : _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new ValueMapIntKey(v);
}

Value::Value(ValueMapIntKey&& v) : _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new ValueMapIntKey(std::move(v));
}

Value::Value(const Value& other) : _type(Type::NONE)
{
    *this = other;
}

Value::Value(Value&& other) : _type(Type::NONE)
{
    *this = std::move(other);
}

Value::~Value()
{
    clear();
}

Value& Value::operator=(const Value& other)
{
    if (this != &other)
    {
        reset(other._type);

        switch (other.getTypeFamily())
        {
        case Type::INTEGER:
            _field.uint64Val = other._field.uint64Val;
            break;
        case Type::FLOAT:
            _field.floatVal = other._field.floatVal;
            break;
        case Type::DOUBLE:
            _field.doubleVal = other._field.doubleVal;
            break;
        case Type::BOOLEAN:
            _field.boolVal = other._field.boolVal;
            break;
        case Type::STRING:
            if (_field.strVal == nullptr)
            {
                _field.strVal = new std::string();
            }
            *_field.strVal = *other._field.strVal;
            break;
        case Type::VECTOR:
            if (_field.vectorVal == nullptr)
            {
                _field.vectorVal = new ValueVector();
            }
            *_field.vectorVal = *other._field.vectorVal;
            break;
        case Type::MAP:
            if (_field.mapVal == nullptr)
            {
                _field.mapVal = new ValueMap();
            }
            *_field.mapVal = *other._field.mapVal;
            break;
        case Type::INT_KEY_MAP:
            if (_field.intKeyMapVal == nullptr)
            {
                _field.intKeyMapVal = new ValueMapIntKey();
            }
            *_field.intKeyMapVal = *other._field.intKeyMapVal;
            break;
        default:
            break;
        }
    }
    return *this;
}

Value& Value::operator=(Value&& other)
{
    if (this != &other)
    {
        clear();
        switch (other.getTypeFamily())
        {
        case Type::INTEGER:
            _field.uint64Val = other._field.uint64Val;
            break;
        case Type::FLOAT:
            _field.floatVal = other._field.floatVal;
            break;
        case Type::DOUBLE:
            _field.doubleVal = other._field.doubleVal;
            break;
        case Type::BOOLEAN:
            _field.boolVal = other._field.boolVal;
            break;
        case Type::STRING:
            _field.strVal = other._field.strVal;
            break;
        case Type::VECTOR:
            _field.vectorVal = other._field.vectorVal;
            break;
        case Type::MAP:
            _field.mapVal = other._field.mapVal;
            break;
        case Type::INT_KEY_MAP:
            _field.intKeyMapVal = other._field.intKeyMapVal;
            break;
        default:
            break;
        }
        _type = other._type;

        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }

    return *this;
}

Value& Value::operator=(unsigned char v)
{
    reset(Type::INT_UI32);
    _field.uintVal = v;
    return *this;
}

Value& Value::operator=(int v)
{
    reset(Type::INT_I32);
    _field.intVal = v;
    return *this;
}

Value& Value::operator=(unsigned int v)
{
    reset(Type::INT_UI32);
    _field.uintVal = v;
    return *this;
}

Value& Value::operator=(int64_t v)
{
    reset(Type::INT_I64);
    _field.int64Val = v;
    return *this;
}

Value& Value::operator=(uint64_t v)
{
    reset(Type::INT_UI64);
    _field.uint64Val = v;
    return *this;
}

Value& Value::operator=(float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

Value& Value::operator=(double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

Value& Value::operator=(bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

Value& Value::operator=(const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

Value& Value::operator=(std::string_view v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

Value& Value::operator=(std::string&& v)
{
    reset(Type::STRING);
    *_field.strVal = std::move(v);
    return *this;
}

Value& Value::operator=(const ValueVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

Value& Value::operator=(ValueVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

Value& Value::operator=(const ValueMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

Value& Value::operator=(ValueMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

Value& Value::operator=(const ValueMapIntKey& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = v;
    return *this;
}

Value& Value::operator=(ValueMapIntKey&& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = std::move(v);
    return *this;
}

bool Value::operator!=(const Value& v)
{
    return !(*this == v);
}
bool Value::operator!=(const Value& v) const
{
    return !(*this == v);
}

bool Value::operator==(const Value& v)
{
    const auto& t = *this;
    return t == v;
}
bool Value::operator==(const Value& v) const
{
    if (this == &v)
        return true;
    if (v._type != this->_type)
        return false;
    if (this->isNull())
        return true;
    switch (getType())
    {
    case Type::INT_I32:
        return v._field.intVal == this->_field.intVal;
    case Type::INT_UI32:
        return v._field.uintVal == this->_field.uintVal;
    case Type::INT_I64:
        return v._field.int64Val == this->_field.int64Val;
    case Type::INT_UI64:
        return v._field.int64Val == this->_field.int64Val;
    case Type::BOOLEAN:
        return v._field.boolVal == this->_field.boolVal;
    case Type::STRING:
        return *v._field.strVal == *this->_field.strVal;
    case Type::FLOAT:
        return std::abs(v._field.floatVal - this->_field.floatVal) <= FLT_EPSILON;
    case Type::DOUBLE:
        return std::abs(v._field.doubleVal - this->_field.doubleVal) <= DBL_EPSILON;
    case Type::VECTOR:
    {
        const auto& v1  = *(this->_field.vectorVal);
        const auto& v2  = *(v._field.vectorVal);
        const auto size = v1.size();
        if (size == v2.size())
        {
            for (size_t i = 0; i < size; i++)
            {
                if (v1[i] != v2[i])
                    return false;
            }
            return true;
        }
        return false;
    }
    case Type::MAP:
    {
        const auto& map1 = *(this->_field.mapVal);
        const auto& map2 = *(v._field.mapVal);
        for (const auto& kvp : map1)
        {
            auto it = map2.find(kvp.first);
            if (it == map2.end() || it->second != kvp.second)
            {
                return false;
            }
        }
        return true;
    }
    case Type::INT_KEY_MAP:
    {
        const auto& map1 = *(this->_field.intKeyMapVal);
        const auto& map2 = *(v._field.intKeyMapVal);
        for (const auto& kvp : map1)
        {
            auto it = map2.find(kvp.first);
            if (it == map2.end() || it->second != kvp.second)
            {
                return false;
            }
        }
        return true;
    }
    default:
        break;
    };

    return false;
}

/// Convert value to a specified type
unsigned char Value::asByte(unsigned char defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");

    switch (_type)
    {
    case Type::INT_UI32:
        return static_cast<unsigned char>(_field.uintVal);
    case Type::INT_I32:
        return static_cast<unsigned char>(_field.intVal);
    case Type::INT_I64:
        return static_cast<unsigned char>(_field.int64Val);
    case Type::INT_UI64:
        return static_cast<unsigned char>(_field.uint64Val);

    case Type::STRING:
        return static_cast<unsigned char>(atoi(_field.strVal->c_str()));

    case Type::FLOAT:
        return static_cast<unsigned char>(_field.floatVal);

    case Type::DOUBLE:
        return static_cast<unsigned char>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1 : 0;

    default:
        return defaultValue;
    }
}

int Value::asInt(int defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::INT_I32:
        return (_field.intVal);
    case Type::INT_UI32:
        return static_cast<int>(_field.uintVal);
    case Type::INT_I64:
        return static_cast<int>(_field.int64Val);
    case Type::INT_UI64:
        return static_cast<int>(_field.uint64Val);

    case Type::STRING:
        return atoi(_field.strVal->c_str());

    case Type::FLOAT:
        return static_cast<int>(_field.floatVal);

    case Type::DOUBLE:
        return static_cast<int>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1 : 0;

    default:
        return defaultValue;
    }
}

unsigned int Value::asUint(unsigned int defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::INT_UI32:
        return (_field.uintVal);
    case Type::INT_I32:
        return static_cast<unsigned int>(_field.intVal);
    case Type::INT_I64:
        return static_cast<unsigned int>(_field.int64Val);
    case Type::INT_UI64:
        return static_cast<unsigned int>(_field.uint64Val);

    case Type::STRING:
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<unsigned int>(strtoul(_field.strVal->c_str(), nullptr, 10));

    case Type::FLOAT:
        return static_cast<unsigned int>(_field.floatVal);

    case Type::DOUBLE:
        return static_cast<unsigned int>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1u : 0u;

    default:
        return defaultValue;
    }
}

int64_t Value::asInt64(int64_t defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::INT_I64:
        return (_field.int64Val);
    case Type::INT_I32:
        return static_cast<int64_t>(_field.intVal);
    case Type::INT_UI32:
        return static_cast<int64_t>(_field.uintVal);
    case Type::INT_UI64:
        return static_cast<int64_t>(_field.uint64Val);

    case Type::STRING:
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<int64_t>(strtoul(_field.strVal->c_str(), nullptr, 10));

    case Type::FLOAT:
        return static_cast<int64_t>(_field.floatVal);

    case Type::DOUBLE:
        return static_cast<int64_t>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1u : 0u;

    default:
        return defaultValue;
    }
}

uint64_t Value::asUint64(uint64_t defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::INT_UI64:
        return (_field.uint64Val);
    case Type::INT_I32:
        return static_cast<uint64_t>(_field.intVal);
    case Type::INT_UI32:
        return static_cast<uint64_t>(_field.uintVal);
    case Type::INT_I64:
        return static_cast<uint64_t>(_field.int64Val);

    case Type::STRING:
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<uint64_t>(strtoull(_field.strVal->c_str(), nullptr, 10));

    case Type::FLOAT:
        return static_cast<uint64_t>(_field.floatVal);

    case Type::DOUBLE:
        return static_cast<uint64_t>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1u : 0u;

    default:
        return defaultValue;
    }
}

float Value::asFloat(float defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::FLOAT:
        return _field.floatVal;

    case Type::STRING:
        return static_cast<float>(utils::atof(_field.strVal->c_str()));

    case Type::INT_I32:
        return static_cast<float>(_field.intVal);
    case Type::INT_UI32:
        return static_cast<float>(_field.uintVal);
    case Type::INT_I64:
        return static_cast<float>(_field.int64Val);
    case Type::INT_UI64:
        return static_cast<float>(_field.uint64Val);

    case Type::DOUBLE:
        return static_cast<float>(_field.doubleVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1.0f : 0.0f;

    default:
        return defaultValue;
    }
}

double Value::asDouble(double defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::DOUBLE:
        return _field.doubleVal;

    case Type::STRING:
        return static_cast<double>(utils::atof(_field.strVal->c_str()));

    case Type::INT_I32:
        return static_cast<double>(_field.intVal);
    case Type::INT_UI32:
        return static_cast<double>(_field.uintVal);
    case Type::INT_I64:
        return static_cast<double>(_field.int64Val);
    case Type::INT_UI64:
        return static_cast<double>(_field.uint64Val);

    case Type::FLOAT:
        return static_cast<double>(_field.floatVal);

    case Type::BOOLEAN:
        return _field.boolVal ? 1.0 : 0.0;

    default:
        return defaultValue;
    }
}

bool Value::asBool(bool defaultValue) const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");
    switch (_type)
    {
    case Type::BOOLEAN:
        return _field.boolVal;

    case Type::STRING:
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;

    case Type::INT_I32:
        return !!_field.intVal;
    case Type::INT_UI32:
        return !!_field.uintVal;
    case Type::INT_I64:
        return !!_field.int64Val;
    case Type::INT_UI64:
        return !!_field.uint64Val;

    case Type::FLOAT:
        return _field.floatVal == 0.0f ? false : true;

    case Type::DOUBLE:
        return _field.doubleVal == 0.0 ? false : true;

    default:
        return defaultValue;
    }
}

std::string Value::asString() const
{
    AXASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP,
             "Only base type (bool, string, float, double, int) could be converted");

    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }

    enum
    {
        NUMBER_MAX_DIGITS = 63
    };

    std::string ret;
    size_t n = 0;
    switch (_type)
    {
    case Type::INT_I32:
        ret = std::to_string(_field.intVal);
        break;
    case Type::INT_UI32:
        ret = std::to_string(_field.uintVal);
        break;
    case Type::INT_I64:
        ret = std::to_string(_field.int64Val);
        break;
    case Type::INT_UI64:
        ret = std::to_string(_field.uint64Val);
        break;
    case Type::FLOAT:
        ret.resize(NUMBER_MAX_DIGITS);
        n = snprintf(&ret.front(), NUMBER_MAX_DIGITS + 1, "%.*g", 7 /*precision*/, _field.floatVal);
        if (n > 0)
            ret.resize(n);
        break;
    case Type::DOUBLE:
        ret.resize(NUMBER_MAX_DIGITS);
        n = snprintf(&ret.front(), NUMBER_MAX_DIGITS + 1, "%.*g", 17 /*precision*/, _field.doubleVal);
        if (n > 0)
            ret.resize(n);
        break;
    case Type::BOOLEAN:
        ret = (_field.boolVal ? "true" : "false");
        break;
    default:
        break;
    }

    return ret;
}

std::string_view Value::asStringRef() const
{
    if (_type == Type::STRING)
        return *_field.strVal;
    return Value::EmptyString;
}

ValueVector& Value::asValueVector()
{
    AXASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const ValueVector& Value::asValueVector() const
{
    AXASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

ValueMap& Value::asValueMap()
{
    AXASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const ValueMap& Value::asValueMap() const
{
    AXASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

ValueMapIntKey& Value::asIntKeyMap()
{
    AXASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

const ValueMapIntKey& Value::asIntKeyMap() const
{
    AXASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

static std::string getTabs(int depth)
{
    std::string tabWidth;

    for (int i = 0; i < depth; ++i)
    {
        tabWidth += "\t";
    }

    return tabWidth;
}

static std::string visit(const Value& v, int depth);

static std::string visitVector(const ValueVector& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "[\n";

    int i = 0;
    for (const auto& child : v)
    {
        ret << getTabs(depth + 1) << i << ": " << visit(child, depth + 1);
        ++i;
    }

    ret << getTabs(depth) << "]\n";

    return ret.str();
}

template <class T>
static std::string visitMap(const T& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "{\n";

    for (auto&& iter : v)
    {
        ret << getTabs(depth + 1) << iter.first << ": ";
        ret << visit(iter.second, depth + 1);
    }

    ret << getTabs(depth) << "}\n";

    return ret.str();
}

static std::string visit(const Value& v, int depth)
{
    std::stringstream ret;

    switch (v.getTypeFamily())
    {
    case Value::Type::NONE:
    case Value::Type::INTEGER:
    case Value::Type::FLOAT:
    case Value::Type::DOUBLE:
    case Value::Type::BOOLEAN:
    case Value::Type::STRING:
        ret << v.asString() << "\n";
        break;
    case Value::Type::VECTOR:
        ret << visitVector(v.asValueVector(), depth);
        break;
    case Value::Type::MAP:
        ret << visitMap(v.asValueMap(), depth);
        break;
    case Value::Type::INT_KEY_MAP:
        ret << visitMap(v.asIntKeyMap(), depth);
        break;
    default:
        AXASSERT(false, "Invalid type!");
        break;
    }

    return ret.str();
}

std::string Value::getDescription() const
{
    std::string ret("\n");
    ret += visit(*this, 0);
    return ret;
}

void Value::clear()
{
    // Free memory the old value allocated
    switch (getTypeFamily())
    {
    case Type::INTEGER:
        _field.uint64Val = 0;
        break;
    case Type::FLOAT:
        _field.floatVal = 0.0f;
        break;
    case Type::DOUBLE:
        _field.doubleVal = 0.0;
        break;
    case Type::BOOLEAN:
        _field.boolVal = false;
        break;
    case Type::STRING:
        AX_SAFE_DELETE(_field.strVal);
        break;
    case Type::VECTOR:
        AX_SAFE_DELETE(_field.vectorVal);
        break;
    case Type::MAP:
        AX_SAFE_DELETE(_field.mapVal);
        break;
    case Type::INT_KEY_MAP:
        AX_SAFE_DELETE(_field.intKeyMapVal);
        break;
    default:
        break;
    }

    _type = Type::NONE;
}

void Value::reset(Type type)
{
    if (_type == type)
        return;

    clear();

    // Allocate memory for the new value
    switch (type)
    {
    case Type::STRING:
        _field.strVal = new std::string();
        break;
    case Type::VECTOR:
        _field.vectorVal = new ValueVector();
        break;
    case Type::MAP:
        _field.mapVal = new ValueMap();
        break;
    case Type::INT_KEY_MAP:
        _field.intKeyMapVal = new ValueMapIntKey();
        break;
    default:
        break;
    }

    _type = type;
}

}
