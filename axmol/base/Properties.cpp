/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2015-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the cocos2d-x project
 */

#include "axmol/base/Properties.h"

#include <string.h>

#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/math/Vec2.h"
#include "axmol/math/Vec3.h"
#include "axmol/math/Vec4.h"
#include "axmol/math/Mat4.h"
#include "axmol/math/Quaternion.h"
#include "axmol/base/text_utils.h"
#include "axmol/base/Data.h"
#include "axmol/tlx/charconv.hpp"
#include "axmol/tlx/split.hpp"

namespace ax
{

static std::string_view make_sv(const char* cstr)
{
    return cstr ? std::string_view{cstr} : std::string_view{};
}

//
// class Properties::InputStreamView
//
struct Properties::InputStreamView
{
    InputStreamView(char* data, size_t size) : _first(data), _last(data + size), _ptr(data) {}

    std::string_view readLineSV()
    {
        char* lineStart = _ptr;
        while (!eof() && *_ptr != '\n')
            _ptr++;
        return std::string_view(lineStart, _ptr - lineStart);
    }

    void skipWhiteSpace()
    {
        signed char c;
        do
        {
            c = readChar();
        } while (isspace(static_cast<unsigned char>(c)) && c != EOF);

        // If we are not at the end of the file, then since we found a
        // non-whitespace character, we put the cursor back in front of it.
        if (c != EOF)
        {
            if (!advance(-1))
            {
                AXLOGE("Failed to seek backwards one character after skipping whitespace.");
            }
        }
    }

    signed char readChar()
    {
        if (eof())
            return EOF;
        return *_ptr++;
    }
    bool advance(int offset)
    {
        if (!eof())
            _ptr += offset;
        return !eof();
    }
    bool eof() { return _ptr >= _last || _ptr < _first; }

    bool empty() const { return _first == _last; }

    char* _first;
    char* _last;
    char* _ptr;
};

// Utility functions (shared with SceneLoader).
/** @script{ignore} */
void calculateNamespacePath(std::string_view urlString,
                            std::string& fileString,
                            std::vector<std::string_view>& namespacePath);
/** @script{ignore} */
Properties* getPropertiesFromNamespacePath(Properties* properties, const std::vector<std::string_view>& namespacePath);

Properties::Properties() : _variables(nullptr), _dirPath(nullptr), _parent(nullptr)
{
    _properties.reserve(32);
}

Properties::Properties(const Properties& copy)
    : _namespace(copy._namespace)
    , _id(copy._id)
    , _parentID(copy._parentID)
    , _properties(copy._properties)
    , _variables(nullptr)
    , _dirPath(nullptr)
    , _parent(copy._parent)

{
    setDirectoryPath(copy._dirPath);

    for (const auto space : copy._namespaces)
    {
        _namespaces.emplace_back(new Properties(*space));
    }
    rewind();
}

Properties::Properties(InputStreamView* isv) : _variables(nullptr), _dirPath(nullptr), _parent(nullptr)
{
    readProperties(isv);
    rewind();
}

Properties::Properties(InputStreamView* isv,
                       std::string_view name,
                       std::string_view id,
                       std::string_view parentID,
                       Properties* parent)
    : _namespace(name), _variables(nullptr), _dirPath(nullptr), _parent(parent)
{
    if (!id.empty())
    {
        _id = id;
    }
    if (!parentID.empty())
    {
        _parentID = parentID;
    }
    readProperties(isv);
    rewind();
}

Properties* Properties::createNonRefCounted(std::string_view url)
{
    if (url.empty())
    {
        AXLOGE("Attempting to create a Properties object from an empty URL!");
        return nullptr;
    }

    // Calculate the file and full namespace path from the specified url.
    auto& urlString = url;
    std::string fileString;
    std::vector<std::string_view> namespacePath;
    calculateNamespacePath(urlString, fileString, namespacePath);

    // data will be released automatically when 'data' goes out of scope
    // so we pass data as weak pointer
    auto data = FileUtils::getInstance()->getDataFromFile(fileString);
    InputStreamView isv{(char*)data.data(), static_cast<size_t>(data.size())};
    Properties* properties = new Properties(&isv);
    properties->resolveInheritance();

    // Get the specified properties object.
    Properties* p = getPropertiesFromNamespacePath(properties, namespacePath);
    if (!p)
    {
        AXLOGW("Failed to load properties from url '{}'.", url);
        AX_SAFE_DELETE(properties);
        return nullptr;
    }

    // If the loaded properties object is not the root namespace,
    // then we have to clone it and delete the root namespace
    // so that we don't leak memory.
    if (p != properties)
    {
        p = p->clone();
        AX_SAFE_DELETE(properties);
    }
    // XXX
    //    p->setDirectoryPath(FileSystem::getDirectoryName(fileString));
    p->setDirectoryPath("");
    return p;
}

static std::string_view extractVariable(std::string_view str)
{
    if (str.size() > 3 && str[0] == '$' && str[1] == '{' && str.back() == '}')
        return str.substr(2, str.size() - 3);

    return std::string_view();
}

void Properties::readProperties(InputStreamView* isv)
{
    AXASSERT(!isv->empty(), "Invalid data");

    constexpr std::string_view multiLineCommentEnd   = "*/"sv;
    constexpr std::string_view multiLineCommentStart = "/*"sv;
    constexpr std::string_view singleLineComment     = "//"sv;

    std::string_view line;
    int c;
    bool comment = false;

    while (true)
    {
        // Skip whitespace at the start of lines
        isv->skipWhiteSpace();

        // Stop when we have reached the end of the file.
        if (isv->eof())
            break;

        // Read the next line as string_view
        line = isv->readLineSV();
        if (line.empty())
            continue;

        // Remove leading/trailing whitespace
        std::string_view trimmedLine = text_utils::trim(line);

        // Ignore comments
        if (comment)
        {
            // Check for end of multi-line comment at either start or end of line
            if (trimmedLine.starts_with(multiLineCommentEnd) || trimmedLine.ends_with(multiLineCommentEnd))
                comment = false;
        }
        else if (trimmedLine.starts_with(multiLineCommentStart))
        {
            // Start of multi-line comment (must be at start of line)
            comment = true;
        }
        else if (!trimmedLine.starts_with(singleLineComment))
        {
            // If an '=' appears on this line, parse it as a name/value pair.
            size_t equalPos = trimmedLine.find('=');
            if (equalPos != std::string_view::npos)
            {
                // Split into name and value parts
                std::string_view name  = text_utils::trim(trimmedLine.substr(0, equalPos));
                std::string_view value = text_utils::trim(trimmedLine.substr(equalPos + 1));

                if (name.empty())
                {
                    AXLOGE("Error parsing properties file: attribute without name.");
                    return;
                }

                // Is this a variable assignment?
                std::string_view varName = extractVariable(name);
                if (!varName.empty())
                {
                    setVariable(varName, value);
                }
                else
                {
                    // Normal name/value pair
                    _properties.emplace_back(Property(name, value));
                }
            }
            else
            {
                // This line might begin or end a namespace,
                // or it might be a key/value pair without '='.

                // Check for '{' on same line.
                size_t braceOpenPos = trimmedLine.find('{');

                // Check for inheritance: ':'
                size_t colonPos = trimmedLine.find(':');

                // Check if the line ends with '}', trimmedLine already has trailing whitespace removed
                const bool endsWithBrace = trimmedLine.ends_with('}');

                // Extract tokens without creating vector
                std::string_view name, id, parentID;

                // Find first token (name)
                size_t start = 0;
                size_t end   = trimmedLine.find_first_of(" \t", start);
                if (end == std::string_view::npos)
                {
                    name = trimmedLine.substr(start);
                }
                else
                {
                    name = trimmedLine.substr(start, end - start);

                    // Skip whitespace after name
                    start = trimmedLine.find_first_not_of(" \t"sv, end);
                    if (start != std::string_view::npos)
                    {
                        // Find second token (id or special character)
                        end = trimmedLine.find_first_of(" \t:{"sv, start);
                        if (end != std::string_view::npos)
                        {
                            id = trimmedLine.substr(start, end - start);

                            // Skip whitespace after id
                            start = trimmedLine.find_first_not_of(" \t"sv, end);
                        }
                        else
                        {
                            id    = trimmedLine.substr(start);
                            start = std::string_view::npos;
                        }
                    }
                }

                if (name.empty())
                {
                    AXLOGE("Error parsing properties file: failed to determine a valid token for line '{}'.",
                           trimmedLine);
                    return;
                }

                // Check if the name is just '}' (end of namespace)
                if (name == "}"sv)
                {
                    // End of namespace.
                    return;
                }

                // Handle inheritance (parentID) if ':' exists
                if (colonPos != std::string_view::npos && start != std::string_view::npos)
                {
                    // Find the ':' and get the token after it (parentID)
                    // Skip to after ':'
                    size_t colonPosInSubstr = trimmedLine.find(':', start);
                    if (colonPosInSubstr != std::string_view::npos)
                    {
                        start = colonPosInSubstr + 1;
                        // Skip whitespace after ':'
                        start = trimmedLine.find_first_not_of(" \t"sv, start);
                        if (start != std::string_view::npos)
                        {
                            // Get parentID (stop at whitespace or '{')
                            end = trimmedLine.find_first_of(" \t{"sv, start);
                            if (end != std::string_view::npos)
                            {
                                parentID = trimmedLine.substr(start, end - start);
                            }
                            else
                            {
                                parentID = trimmedLine.substr(start);
                            }
                        }
                    }
                }

                // Trim any trailing whitespace from id and parentID
                id       = text_utils::trim(id);
                parentID = text_utils::trim(parentID);

                // Check if this line contains a namespace definition
                bool hasBraceOnSameLine = (braceOpenPos != std::string_view::npos);

                if (hasBraceOnSameLine)
                {
                    // If the namespace ends on this line, seek back to right before the '}' character.
                    if (endsWithBrace)
                    {
                        if (!isv->advance(-1))
                        {
                            AXLOGE("Failed to seek back to before a '}}' character in properties file.");
                            return;
                        }
                        while (isv->readChar() != '}')
                        {
                            if (!isv->advance(-2))
                            {
                                AXLOGE("Failed to seek back to before a '}}' character in properties file.");
                                return;
                            }
                        }
                        if (!isv->advance(-1))
                        {
                            AXLOGE("Failed to seek back to before a '}}' character in properties file.");
                            return;
                        }
                    }

                    // Create new namespace.
                    Properties* space = new Properties(isv, name, id, parentID, this);
                    _namespaces.emplace_back(space);

                    // If the namespace ends on this line, seek to right after the '}' character.
                    if (endsWithBrace)
                    {
                        if (!isv->advance(1))
                        {
                            AXLOGE("Failed to seek to immediately after a '}}' character in properties file.");
                            return;
                        }
                    }
                }
                else
                {
                    // Find out if the next line starts with "{"
                    isv->skipWhiteSpace();
                    c = isv->readChar();
                    if (c == '{')
                    {
                        // Create new namespace.
                        Properties* space = new Properties(isv, name, id, parentID, this);
                        _namespaces.emplace_back(space);
                    }
                    else
                    {
                        // Back up from readChar()
                        if (!isv->advance(-1))
                            AXLOGE(
                                "Failed to seek backwards a single character after testing if the next line starts "
                                "with '{{'.");

                        // Store "name value" as a name/value pair, or even just "name".
                        _properties.emplace_back(Property(name, id));
                    }
                }
            }
        }
    }
}

Properties::~Properties()
{
    AX_SAFE_DELETE(_dirPath);
    for (size_t i = 0, count = _namespaces.size(); i < count; ++i)
    {
        AX_SAFE_DELETE(_namespaces[i]);
    }

    AX_SAFE_DELETE(_variables);
}

void Properties::resolveInheritance(std::string_view id)
{
    // Namespaces can be defined like so:
    // "name id : parentID { }"
    // This method merges data from the parent namespace into the child.

    // Get a top-level namespace.
    Properties* derived;
    if (!id.empty())
    {
        derived = getNamespace(id);
    }
    else
    {
        derived = getNextNamespace();
    }
    while (derived)
    {
        // If the namespace has a parent ID, find the parent.
        if (!derived->_parentID.empty())
        {
            Properties* parent = getNamespace(derived->_parentID);
            if (parent)
            {
                resolveInheritance(parent->getId());

                // Copy the child.
                Properties* overrides = new Properties(*derived);

                // Delete the child's data.
                for (size_t i = 0, count = derived->_namespaces.size(); i < count; i++)
                {
                    AX_SAFE_DELETE(derived->_namespaces[i]);
                }

                // Copy data from the parent into the child.
                derived->_properties = parent->_properties;
                derived->_namespaces.clear();
                std::vector<Properties*>::const_iterator itt;
                for (const auto space : parent->_namespaces)
                {
                    derived->_namespaces.emplace_back(new Properties(*space));
                }
                derived->rewind();

                // Take the original copy of the child and override the data copied from the parent.
                derived->mergeWith(overrides);

                // Delete the child copy.
                AX_SAFE_DELETE(overrides);
            }
        }

        // Resolve inheritance within this namespace.
        derived->resolveInheritance();

        // Get the next top-level namespace and check again.
        if (id.empty())
        {
            derived = getNextNamespace();
        }
        else
        {
            derived = nullptr;
        }
    }
}

void Properties::mergeWith(Properties* overrides)
{
    AXASSERT(overrides, "Invalid overrides");

    // Overwrite or add each property found in child.
    overrides->rewind();
    auto name = overrides->getNextProperty();
    while (!name.empty())
    {
        this->setString(name, overrides->getString());
        name = overrides->getNextProperty();
    }
    this->_propertiesItr = this->_properties.end();

    // Merge all common nested namespaces, add new ones.
    Properties* overridesNamespace = overrides->getNextNamespace();
    while (overridesNamespace)
    {
        bool merged = false;

        rewind();
        Properties* derivedNamespace = getNextNamespace();
        while (derivedNamespace)
        {
            if (derivedNamespace->getNamespace() == overridesNamespace->getNamespace() &&
                derivedNamespace->getId() == overridesNamespace->getId())
            {
                derivedNamespace->mergeWith(overridesNamespace);
                merged = true;
            }

            derivedNamespace = getNextNamespace();
        }

        if (!merged)
        {
            // Add this new namespace.
            Properties* newNamespace = new Properties(*overridesNamespace);

            this->_namespaces.emplace_back(newNamespace);
            this->_namespacesItr = this->_namespaces.end();
        }

        overridesNamespace = overrides->getNextNamespace();
    }
}

std::string_view Properties::getNextProperty()
{
    if (_propertiesItr == _properties.end())
    {
        // Restart from the beginning
        _propertiesItr = _properties.begin();
    }
    else
    {
        // Move to the next property
        ++_propertiesItr;
    }

    return _propertiesItr == _properties.end() ? std::string_view() : _propertiesItr->name;
}

Properties* Properties::getNextNamespace()
{
    if (_namespacesItr == _namespaces.end())
    {
        // Restart from the beginning
        _namespacesItr = _namespaces.begin();
    }
    else
    {
        ++_namespacesItr;
    }

    if (_namespacesItr != _namespaces.end())
    {
        Properties* ns = *_namespacesItr;
        return ns;
    }

    return nullptr;
}

void Properties::rewind()
{
    _propertiesItr = _properties.end();
    _namespacesItr = _namespaces.end();
}

Properties* Properties::getNamespace(std::string_view id, bool searchNames, bool recurse) const
{
    AXASSERT(!id.empty(), "invalid id");

    for (const auto& it : _namespaces)
    {
        Properties* p = it;
        if ((searchNames ? p->_namespace : p->_id) == id)
            return p;

        if (recurse)
        {
            // Search recursively.
            p = p->getNamespace(id, searchNames, true);
            if (p)
                return p;
        }
    }

    return nullptr;
}

std::string_view Properties::getNamespace() const
{
    return _namespace;
}

std::string_view Properties::getId() const
{
    return _id;
}

bool Properties::exists(std::string_view name) const
{
    if (name.empty())
        return false;

    for (const auto& itr : _properties)
    {
        if (itr.name == name)
            return true;
    }

    return false;
}

static bool isStringNumeric(std::string_view str)
{
    AXASSERT(!str.empty(), "invalid str");

    const char* ptr = str.data();

    // The first character may be '-'
    if (*ptr == '-')
        ptr++;

    // The first character after the sign must be a digit
    if (!isdigit(*ptr))
        return false;
    ptr++;

    // All remaining characters must be digits, with a single decimal (.) permitted
    unsigned int decimalCount = 0;
    while (*ptr)
    {
        if (!isdigit(*ptr))
        {
            if (*ptr == '.' && decimalCount == 0)
            {
                // Max of 1 decimal allowed
                decimalCount++;
            }
            else
            {
                return false;
            }
        }
        ptr++;
    }
    return true;
}

Properties::Type Properties::getType(std::string_view name) const
{
    auto value = getString(name);
    if (value.empty())
    {
        return Properties::NONE;
    }

    // Parse the value to determine the format
    unsigned int commaCount = 0;
    auto valuePtr           = value.data();
    while ((valuePtr = strchr(valuePtr, ',')))
    {
        valuePtr++;
        commaCount++;
    }

    switch (commaCount)
    {
    case 0:
        return isStringNumeric(value) ? Properties::NUMBER : Properties::STRING;
    case 1:
        return Properties::VECTOR2;
    case 2:
        return Properties::VECTOR3;
    case 3:
        return Properties::VECTOR4;
    case 15:
        return Properties::MATRIX;
    default:
        return Properties::STRING;
    }
}

std::string_view Properties::getString(std::string_view name, std::string_view defaultValue) const
{
    std::string_view value = ""sv;

    if (!name.empty())
    {
        // If 'name' is a variable, return the variable value
        std::string_view varName = extractVariable(name);
        if (!varName.empty())
        {
            return getVariable(varName, defaultValue);
        }

        for (const auto& itr : _properties)
        {
            if (itr.name == name)
            {
                value = itr.value;
                break;
            }
        }
    }
    else
    {
        // No name provided - get the value at the current iterator position
        if (_propertiesItr != _properties.end())
        {
            value = _propertiesItr->value;
        }
    }

    if (!value.empty())
    {
        // If the value references a variable, return the variable value
        std::string_view varName = extractVariable(value);
        if (!varName.empty())
            return getVariable(varName, defaultValue);

        return value;
    }

    return defaultValue;
}

bool Properties::setString(std::string_view name, std::string_view value)
{
    if (!name.empty())
    {
        for (auto&& itr : _properties)
        {
            if (itr.name == name)
            {
                // Update the first property that matches this name
                itr.value = value;
                return true;
            }
        }

        // There is no property with this name, so add one
        _properties.emplace_back(Property(name, value));
    }
    else
    {
        // If there's a current property, set its value
        if (_propertiesItr == _properties.end())
            return false;

        _propertiesItr->value = value;
    }

    return true;
}

bool Properties::getBool(std::string_view name, bool defaultValue) const
{
    auto valueString = getString(name);
    if (!valueString.empty())
    {
        return valueString == "true";
    }

    return defaultValue;
}

int Properties::getInt(std::string_view name) const
{
    auto valueString = getString(name);
    if (!valueString.empty())
    {
        int value{};
        auto [_, ec] = tlx::from_chars(valueString, value);
        if (ec != std::errc{})
        {
            AXLOGE("Error attempting to parse property '{}' as an integer.", name);
            return 0;
        }
        return value;
    }

    return 0;
}

float Properties::getFloat(std::string_view name) const
{
    auto valueString = getString(name);
    if (!valueString.empty())
    {
        float value{};
        auto [_, ec] = tlx::from_chars(valueString, value);
        if (ec != std::errc{})
        {
            AXLOGE("Error attempting to parse property '{}' as a float.", name);
            return 0.0f;
        }
        return value;
    }

    return 0.0f;
}

bool Properties::getMat4(std::string_view name, Mat4* out) const
{
    AXASSERT(out, "Invalid out");

    auto valueString = getString(name);
    if (!valueString.empty())
    {
        float m[16];
        auto [_, ec] = tlx::from_chars(valueString, ',', m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9],
                                       m[10], m[11], m[12], m[13], m[14], m[15]);

        if (ec != std::errc{})
        {
            AXLOGE("Error attempting to parse property '{}' as a matrix.", name);
            out->setIdentity();
            return false;
        }

        out->set(m);
        return true;
    }

    out->setIdentity();
    return false;
}

bool Properties::getVec2(std::string_view name, Vec2* out) const
{
    return parseVec2(getString(name), out);
}

bool Properties::getVec3(std::string_view name, Vec3* out) const
{
    return parseVec3(getString(name), out);
}

bool Properties::getVec4(std::string_view name, Vec4* out) const
{
    return parseVec4(getString(name), out);
}

bool Properties::getQuaternionFromAxisAngle(std::string_view name, Quaternion* out) const
{
    return parseAxisAngle(getString(name), out);
}

bool Properties::getColor(std::string_view name, Color* out) const
{
    return parseColor(getString(name), out);
}

bool Properties::getPath(std::string_view name, std::string* path) const
{
    AXASSERT(!name.empty() && path, "Invalid name or path");
    auto valueString = getString(name);
    if (!valueString.empty())
    {
        if (FileUtils::getInstance()->isFileExist(valueString))
        {
            path->assign(valueString);
            return true;
        }
        else
        {
            const Properties* prop = this;
            while (prop != nullptr)
            {
                // Search for the file path relative to the bundle file
                const std::string* dirPath = prop->_dirPath;
                if (dirPath != nullptr && !dirPath->empty())
                {
                    std::string relativePath = *dirPath;
                    relativePath.append(valueString);
                    if (FileUtils::getInstance()->isFileExist(relativePath))
                    {
                        path->assign(relativePath);
                        return true;
                    }
                }
                prop = prop->_parent;
            }
        }
    }
    return false;
}

std::string_view Properties::getVariable(std::string_view name, std::string_view defaultValue) const
{
    if (name.empty())
        return defaultValue;

    // Search for variable in this Properties object
    if (_variables)
    {
        for (size_t i = 0, count = _variables->size(); i < count; ++i)
        {
            Property& prop = (*_variables)[i];
            if (prop.name == name)
                return prop.value;
        }
    }

    // Search for variable in parent Properties
    return _parent ? _parent->getVariable(name, defaultValue) : defaultValue;
}

void Properties::setVariable(std::string_view name, std::string_view value)
{
    AXASSERT(!name.empty(), "Invalid name");

    Property* prop = nullptr;

    // Search for variable in this Properties object and parents
    Properties* current = const_cast<Properties*>(this);
    while (current)
    {
        if (current->_variables)
        {
            for (size_t i = 0, count = current->_variables->size(); i < count; ++i)
            {
                Property* p = &(*current->_variables)[i];
                if (p->name == name)
                {
                    prop = p;
                    break;
                }
            }
        }
        current = current->_parent;
    }

    if (prop)
    {
        // Found an existing property, set it
        prop->value = value;
    }
    else
    {
        // Add a new variable with this name
        if (!_variables)
            _variables = new std::vector<Property>();
        _variables->emplace_back(name, value);
    }
}

Properties* Properties::clone()
{
    Properties* p = new Properties();

    p->_namespace     = _namespace;
    p->_id            = _id;
    p->_parentID      = _parentID;
    p->_properties    = _properties;
    p->_propertiesItr = p->_properties.end();
    p->setDirectoryPath(_dirPath);

    for (size_t i = 0, count = _namespaces.size(); i < count; i++)
    {
        AXASSERT(_namespaces[i], "Invalid namespace");
        Properties* child = _namespaces[i]->clone();
        p->_namespaces.emplace_back(child);
        child->_parent = p;
    }
    p->_namespacesItr = p->_namespaces.end();

    return p;
}

void Properties::setDirectoryPath(const std::string* path)
{
    if (path)
    {
        setDirectoryPath(*path);
    }
    else
    {
        AX_SAFE_DELETE(_dirPath);
    }
}

void Properties::setDirectoryPath(std::string_view path)
{
    if (_dirPath == nullptr)
    {
        _dirPath = new std::string(path);
    }
    else
    {
        _dirPath->assign(path);
    }
}

void calculateNamespacePath(std::string_view urlString,
                            std::string& fileString,
                            std::vector<std::string_view>& namespacePath)
{
    // If the url references a specific namespace within the file,
    // calculate the full namespace path to the final namespace.
    size_t loc = urlString.rfind('#');
    if (loc != std::string::npos)
    {
        fileString               = urlString.substr(0, loc);
        auto namespacePathString = urlString.substr(loc + 1);
        while ((loc = namespacePathString.find('/')) != std::string::npos)
        {
            namespacePath.emplace_back(namespacePathString.substr(0, loc));
            namespacePathString = namespacePathString.substr(loc + 1);
        }
        namespacePath.emplace_back(namespacePathString);
    }
    else
    {
        fileString = urlString;
    }
}

Properties* getPropertiesFromNamespacePath(Properties* properties, const std::vector<std::string_view>& namespacePath)
{
    // If the url references a specific namespace within the file,
    // return the specified namespace or notify the user if it cannot be found.
    if (!namespacePath.empty())
    {
        size_t size = namespacePath.size();
        properties->rewind();
        Properties* iter = properties->getNextNamespace();
        for (size_t i = 0; i < size;)
        {
            while (true)
            {
                if (iter == nullptr)
                {
                    AXLOGW("Failed to load properties object from url.");
                    return nullptr;
                }

                if (iter->getId() == namespacePath[i])
                {
                    if (i != size - 1)
                    {
                        properties = iter->getNextNamespace();
                        iter       = properties;
                    }
                    else
                        properties = iter;

                    i++;
                    break;
                }

                iter = properties->getNextNamespace();
            }
        }

        return properties;
    }
    else
        return properties;
}

bool Properties::parseVec2(std::string_view str, Vec2* out)
{
    if (!str.empty())
    {
        float x, y;
        if (tlx::from_chars(str, ',', x, y).ec == std::errc{})
        {
            if (out)
                out->set(x, y);
            return true;
        }
        else
        {
            AXLOGE("Error attempting to parse property as a two-dimensional vector: {}", str);
        }
    }

    if (out)
        out->set(0.0f, 0.0f);
    return false;
}

bool Properties::parseVec3(std::string_view str, Vec3* out)
{
    if (!str.empty())
    {
        float x, y, z;
        if (tlx::from_chars(str, ',', x, y, z).ec == std::errc{})
        {
            if (out)
                out->set(x, y, z);
            return true;
        }
        else
        {
            AXLOGE("Error attempting to parse property as a three-dimensional vector: {}", str);
        }
    }

    if (out)
        out->set(0.0f, 0.0f, 0.0f);
    return false;
}

bool Properties::parseVec4(std::string_view str, Vec4* out)
{
    if (!str.empty())
    {
        float x, y, z, w;
        if (tlx::from_chars(str, ',', x, y, z, w).ec == std::errc{})
        {
            if (out)
                out->set(x, y, z, w);
            return true;
        }
        else
        {
            AXLOGE("Error attempting to parse property as a four-dimensional vector: {}", str);
        }
    }

    if (out)
        out->set(0.0f, 0.0f, 0.0f, 0.0f);
    return false;
}

bool Properties::parseAxisAngle(std::string_view str, Quaternion* out)
{
    if (!str.empty())
    {
        float x, y, z, theta;
        if (tlx::from_chars(str, ',', x, y, z, theta).ec == std::errc{})
        {
            if (out)
                out->set(Vec3(x, y, z), MATH_DEG_TO_RAD(theta));
            return true;
        }
        else
        {
            AXLOGE("Error attempting to parse property as an axis-angle rotation: {}", str);
        }
    }

    if (out)
        out->set(0.0f, 0.0f, 0.0f, 1.0f);
    return false;
}

bool Properties::parseColor(std::string_view str, Color* out)
{
    if (!str.empty())
    {
        if (str.length() == 9 && str[0] == '#')
        {
            // Read the string into an int as hex.
            unsigned int color;
            auto [_, ec] = tlx::from_chars(str.substr(1), color, 16);
            if (ec == std::errc{})
            {
                if (out)
                    out->set(Color::fromHex(color));
                return true;
            }
            else
            {
                // Invalid format
                AXLOGE("Error attempting to parse property as an RGBA color: {}", str);
            }
        }
        else
        {
            // Not a color string.
            AXLOGW("Error attempting to parse property as an RGBA color (not specified as a color string): {}", str);
        }
    }

    if (out)
        out->set(0.0f, 0.0f, 0.0f, 0.0f);
    return false;
}

}  // namespace ax
