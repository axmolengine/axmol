/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
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

#include "platform/FileUtils.h"

#include <stack>
#include <sstream>
#include <algorithm>

#include "base/Data.h"
#include "base/Macros.h"
#include "base/Director.h"
#include "platform/SAXParser.h"
#include "platform/FileStream.h"

#ifdef MINIZIP_FROM_SYSTEM
#    include <minizip/unzip.h>
#else  // from our embedded sources
#    include "unzip.h"
#endif
#include <sys/stat.h>

#if defined(_WIN32)
#    include "ntcvt/ntcvt.hpp"
#    include "yasio/string_view.hpp"
#endif

#include "pugixml/pugixml.hpp"

#define DECLARE_GUARD (void)0

#include "base/filesystem.h"

#if defined(_WIN32)
inline stdfs::path toFspath(const std::string_view& pathSV)
{
    return stdfs::path{ntcvt::from_chars(pathSV)};
}
#else
inline stdfs::path toFspath(const std::string_view& pathSV)
{
    return stdfs::path{pathSV};
}
#endif

NS_AX_BEGIN

// Implement DictMaker

typedef enum
{
    SAX_NONE = 0,
    SAX_KEY,
    SAX_DICT,
    SAX_INT,
    SAX_REAL,
    SAX_STRING,
    SAX_ARRAY
} SAXState;

typedef enum
{
    SAX_RESULT_NONE = 0,
    SAX_RESULT_DICT,
    SAX_RESULT_ARRAY
} SAXResult;

class DictMaker : public SAXDelegator
{
public:
    SAXResult _resultType;
    ValueMap _rootDict;
    ValueVector _rootArray;

    std::string _curKey;    ///< parsed key
    std::string _curValue;  // parsed value
    SAXState _state;

    ValueMap* _curDict;
    ValueVector* _curArray;

    std::stack<ValueMap*> _dictStack;
    std::stack<ValueVector*> _arrayStack;
    std::stack<SAXState> _stateStack;

public:
    DictMaker() : _resultType(SAX_RESULT_NONE), _state(SAX_NONE), _curDict(nullptr), _curArray(nullptr) {}

    ~DictMaker() {}

    ValueMap dictionaryWithContentsOfFile(std::string_view fileName)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        AXASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootDict;
    }

    ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        AXASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(filedata, filesize);
        return _rootDict;
    }

    ValueVector arrayWithContentsOfFile(std::string_view fileName)
    {
        _resultType = SAX_RESULT_ARRAY;
        SAXParser parser;

        AXASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootArray;
    }

    void startElement(void* ctx, const char* name, const char** atts) override
    {
        const std::string sName(name);
        if (sName == "dict"sv)
        {
            if (_resultType == SAX_RESULT_DICT && _rootDict.empty())
            {
                _curDict = &_rootDict;
            }

            _state = SAX_DICT;

            SAXState preState = SAX_NONE;
            if (!_stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (SAX_ARRAY == preState)
            {
                // add a new dictionary into the array
                _curArray->emplace_back(Value(ValueMap()));
                _curDict = &(_curArray->rbegin())->asValueMap();
            }
            else if (SAX_DICT == preState)
            {
                // add a new dictionary into the pre dictionary
                AXASSERT(!_dictStack.empty(), "The state is wrong!");
                ValueMap* preDict = _dictStack.top();
                auto& curVal      = hlookup::set_item(*preDict, _curKey, Value(ValueMap()))->second;
                _curDict          = &curVal.asValueMap();
            }

            // record the dict state
            _stateStack.push(_state);
            _dictStack.push(_curDict);
        }
        else if (sName == "key"sv)
        {
            _state = SAX_KEY;
        }
        else if (sName == "integer"sv)
        {
            _state = SAX_INT;
        }
        else if (sName == "real"sv)
        {
            _state = SAX_REAL;
        }
        else if (sName == "string"sv)
        {
            _state = SAX_STRING;
        }
        else if (sName == "array"sv)
        {
            _state = SAX_ARRAY;

            if (_resultType == SAX_RESULT_ARRAY && _rootArray.empty())
            {
                _curArray = &_rootArray;
            }
            SAXState preState = SAX_NONE;
            if (!_stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (preState == SAX_DICT)
            {
                (*_curDict)[_curKey] = Value(ValueVector());
                _curArray            = &(*_curDict)[_curKey].asValueVector();
            }
            else if (preState == SAX_ARRAY)
            {
                AXASSERT(!_arrayStack.empty(), "The state is wrong!");
                ValueVector* preArray = _arrayStack.top();
                preArray->emplace_back(Value(ValueVector()));
                _curArray = &(_curArray->rbegin())->asValueVector();
            }
            // record the array state
            _stateStack.push(_state);
            _arrayStack.push(_curArray);
        }
        else
        {
            _state = SAX_NONE;
        }
    }

    void endElement(void* ctx, const char* name) override
    {
        SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string sName((char*)name);
        if (sName == "dict"sv)
        {
            _stateStack.pop();
            _dictStack.pop();
            if (!_dictStack.empty())
            {
                _curDict = _dictStack.top();
            }
        }
        else if (sName == "array"sv)
        {
            _stateStack.pop();
            _arrayStack.pop();
            if (!_arrayStack.empty())
            {
                _curArray = _arrayStack.top();
            }
        }
        else if (sName == "true"sv)
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->emplace_back(Value(true));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(true);
            }
        }
        else if (sName == "false"sv)
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->emplace_back(Value(false));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(false);
            }
        }
        else if (sName == "string"sv || sName == "integer"sv || sName == "real"sv)
        {
            if (SAX_ARRAY == curState)
            {
                if (sName == "string"sv)
                    _curArray->emplace_back(Value(_curValue));
                else if (sName == "integer"sv)
                    _curArray->emplace_back(Value(atoi(_curValue.c_str())));
                else
                    _curArray->emplace_back(Value(std::atof(_curValue.c_str())));
            }
            else if (SAX_DICT == curState)
            {
                if (sName == "string"sv)
                    (*_curDict)[_curKey] = Value(_curValue);
                else if (sName == "integer"sv)
                    (*_curDict)[_curKey] = Value(atoi(_curValue.c_str()));
                else
                    (*_curDict)[_curKey] = Value(std::atof(_curValue.c_str()));
            }

            _curValue.clear();
        }

        _state = SAX_NONE;
    }

    void textHandler(void* ctx, const char* ch, size_t len) override
    {
        if (_state == SAX_NONE)
        {
            return;
        }

        SAXState curState      = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string text = std::string((char*)ch, len);

        switch (_state)
        {
        case SAX_KEY:
            _curKey = text;
            break;
        case SAX_INT:
        case SAX_REAL:
        case SAX_STRING:
        {
            if (curState == SAX_DICT)
            {
                AXASSERT(!_curKey.empty(), "key not found : <integer/real>");
            }

            _curValue.append(text);
        }
        break;
        default:
            break;
        }
    }
};

ValueMap FileUtils::getValueMapFromFile(std::string_view filename) const
{
    const std::string fullPath = fullPathForFilename(filename);
    DictMaker tMaker;
    return tMaker.dictionaryWithContentsOfFile(fullPath);
}

ValueMap FileUtils::getValueMapFromData(const char* filedata, int filesize) const
{
    DictMaker tMaker;
    return tMaker.dictionaryWithDataOfFile(filedata, filesize);
}

ValueVector FileUtils::getValueVectorFromFile(std::string_view filename) const
{
    const std::string fullPath = fullPathForFilename(filename);
    DictMaker tMaker;
    return tMaker.arrayWithContentsOfFile(fullPath);
}

/*
 * forward statement
 */
static void generateElementForArray(const ValueVector& array, pugi::xml_node& parent);
static void generateElementForDict(const ValueMap& dict, pugi::xml_node& innerDict);

/*
 * Use pugixml to write plist files
 */
bool FileUtils::writeToFile(const ValueMap& dict, std::string_view fullPath) const
{
    return writeValueMapToFile(dict, fullPath);
}

bool FileUtils::writeValueMapToFile(const ValueMap& dict, std::string_view fullPath) const
{
    pugi::xml_document doc;
    doc.load_string(R"(<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist />)",
                    pugi::parse_full);

    auto rootEle = doc.document_element();

    generateElementForDict(dict, rootEle);
    std::stringstream ss;
    doc.save(ss, "  ");
    return writeStringToFile(ss.str(), fullPath);
}

bool FileUtils::writeValueVectorToFile(const ValueVector& vecData, std::string_view fullPath) const
{
    pugi::xml_document doc;
    doc.load_string(R"(<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist />)",
                    pugi::parse_full);

    auto rootEle = doc.document_element();
    generateElementForArray(vecData, rootEle);
    std::stringstream ss;
    doc.save(ss, "  ");
    return writeStringToFile(ss.str(), fullPath);
}

static void generateElementForObject(const Value& value, pugi::xml_node& parent)
{
    // object is String
    if (value.getType() == Value::Type::STRING)
    {
        auto node = parent.append_child("string"sv);
        node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString());
    }
    // object is integer
    else if (value.getType() == Value::Type::INTEGER)
    {
        auto node = parent.append_child("integer"sv);
        node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString());
    }
    // object is real
    else if (value.getType() == Value::Type::FLOAT || value.getType() == Value::Type::DOUBLE)
    {
        auto node = parent.append_child("real"sv);
        node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString());
    }
    // object is bool
    else if (value.getType() == Value::Type::BOOLEAN)
    {
        parent.append_child(value.asString());
    }
    // object is Array
    else if (value.getType() == Value::Type::VECTOR)
        generateElementForArray(value.asValueVector(), parent);
    // object is Dictionary
    else if (value.getType() == Value::Type::MAP)
        generateElementForDict(value.asValueMap(), parent);
}

static void generateElementForDict(const ValueMap& dict, pugi::xml_node& parent)
{
    auto dictDS = parent.append_child("dict"sv);
    for (const auto& iter : dict)
    {
        auto key = dictDS.append_child("key"sv);
        key.append_child(pugi::xml_node_type::node_pcdata).set_value(iter.first);

        generateElementForObject(iter.second, dictDS);
    }
}

static void generateElementForArray(const ValueVector& array, pugi::xml_node& parent)
{
    auto arrayDS = parent.append_child("array"sv);
    for (const auto& value : array)
    {
        generateElementForObject(value, arrayDS);
    }
}

// Implement FileUtils
FileUtils* FileUtils::s_sharedFileUtils = nullptr;

void FileUtils::destroyInstance()
{
    AX_SAFE_DELETE(s_sharedFileUtils);
}

void FileUtils::setDelegate(FileUtils* delegate)
{
    if (s_sharedFileUtils)
        delete s_sharedFileUtils;

    s_sharedFileUtils = delegate;
}

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
std::string FileUtils::s_exeDir;
#endif

FileUtils::FileUtils() : _writablePath() {}

FileUtils::~FileUtils() {}

bool FileUtils::writeStringToFile(std::string_view dataStr, std::string_view fullPath) const
{
    return FileUtils::writeBinaryToFile(dataStr.data(), dataStr.size(), fullPath);
}

void FileUtils::writeStringToFile(std::string dataStr,
                                  std::string_view fullPath,
                                  std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{fullPath}](std::string_view dataStrIn) -> bool {
            return FileUtils::getInstance()->writeStringToFile(dataStrIn, path);
        },
        std::move(callback), std::move(dataStr));
}

bool FileUtils::writeDataToFile(const Data& data, std::string_view fullPath) const
{
    return FileUtils::writeBinaryToFile(data.getBytes(), data.getSize(), fullPath);
}

void FileUtils::writeDataToFile(Data data, std::string_view fullPath, std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{fullPath}](const Data& dataIn) -> bool {
            return FileUtils::getInstance()->writeDataToFile(dataIn, path);
        },
        std::move(callback), std::move(data));
}

bool FileUtils::writeBinaryToFile(const void* data, size_t dataSize, std::string_view fullPath)
{
    AXASSERT(!fullPath.empty() && dataSize > 0, "Invalid parameters.");

    auto* fileUtils = FileUtils::getInstance();
    do
    {
        auto fileStream = fileUtils->openFileStream(fullPath, IFileStream::Mode::WRITE);
        // Read the file from hardware
        AX_BREAK_IF(!fileStream);

        bool ok = fileStream->write(data, static_cast<unsigned int>(dataSize)) == dataSize;
        return ok;
    } while (0);

    return false;
}

bool FileUtils::init()
{
    DECLARE_GUARD;
    _searchPathArray.emplace_back(_defaultResRootPath);
    return true;
}

void FileUtils::purgeCachedEntries()
{
    DECLARE_GUARD;
    _fullPathCache.clear();
    _fullPathCacheDir.clear();
}

std::string FileUtils::getStringFromFile(std::string_view filename) const
{
    std::string s;
    getContents(filename, &s);
    return s;
}

void FileUtils::getStringFromFile(std::string_view path, std::function<void(std::string)> callback) const
{
    // Get the full path on the main thread, to avoid the issue that FileUtil's is not
    // thread safe, and accessing the fullPath cache and searching the search paths is not thread safe
    auto fullPath = fullPathForFilename(path);
    performOperationOffthread(
        [path = std::string{fullPath}]() -> std::string { return FileUtils::getInstance()->getStringFromFile(path); },
        std::move(callback));
}

Data FileUtils::getDataFromFile(std::string_view filename) const
{
    Data d;
    getContents(filename, &d);
    return d;
}

void FileUtils::getDataFromFile(std::string_view filename, std::function<void(Data)> callback) const
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread(
        [path = std::string{fullPath}]() -> Data { return FileUtils::getInstance()->getDataFromFile(path); },
        std::move(callback));
}

FileUtils::Status FileUtils::getContents(std::string_view filename, ResizableBuffer* buffer) const
{
    if (filename.empty())
        return Status::NotExists;

    auto fileUtils = FileUtils::getInstance();

    const auto fullPath = fileUtils->fullPathForFilename(filename);

    FileStream fileStream;
    fileStream.open(fullPath, IFileStream::Mode::READ);
    if (!fileStream)
        return Status::OpenFailed;

    const auto size = fileStream.size();
    if (size < 0)
    {
        return Status::ObtainSizeFailed;
    }

    if (size > ULONG_MAX)
    {
        return Status::TooLarge;
    }

    buffer->resize((size_t)size);
    const auto sizeRead = fileStream.read(buffer->buffer(), (unsigned)size);
    if (sizeRead < size)
    {
        buffer->resize(sizeRead);
        return Status::ReadFailed;
    }

    return Status::OK;
}

void FileUtils::writeValueMapToFile(ValueMap dict, std::string_view fullPath, std::function<void(bool)> callback) const
{

    performOperationOffthread(
        [path = std::string{fullPath}](const ValueMap& dictIn) -> bool {
            return FileUtils::getInstance()->writeValueMapToFile(dictIn, path);
        },
        std::move(callback), std::move(dict));
}

void FileUtils::writeValueVectorToFile(ValueVector vecData,
                                       std::string_view fullPath,
                                       std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{fullPath}](const ValueVector& vecDataIn) -> bool {
            return FileUtils::getInstance()->writeValueVectorToFile(vecDataIn, path);
        },
        std::move(callback), std::move(vecData));
}

std::string FileUtils::getPathForFilename(std::string_view filename, std::string_view searchPath) const
{
    auto file                  = filename;
    std::string_view file_path = hlookup::empty_sv;
    size_t pos                 = filename.find_last_of('/');
    if (pos != std::string::npos)
    {
        file_path = filename.substr(0, pos + 1);
        file      = filename.substr(pos + 1);
    }

    // searchPath + file_path + resourceDirectory
    std::string path{searchPath};
    path += file_path;

    path = getFullPathForFilenameWithinDirectory(path, file);

    return path;
}

std::string FileUtils::getPathForDirectory(std::string_view dir, std::string_view searchPath) const
{
    return std::string{searchPath}.append(dir);
}

std::string FileUtils::fullPathForFilename(std::string_view filename) const
{

    DECLARE_GUARD;

    if (filename.empty())
    {
        return "";
    }

    /*
     * As we know, this function 'fullPathForFilename' will be calling when load any file, how to ensure this thread
     * safe Step: a. call this function at main-thread always b. use the really fullPath to call
     * getStringFromFile/getDataFromFile at sub-thread c. then this function will call again with really fullPath d.
     * then isAbsolutePath avoid to access _fullPathCache _fullPathCache concurrent
     */
    if (isAbsolutePath(filename))
    {
        return std::string{filename};
    }

    // Already Cached ?
    auto cacheIter = _fullPathCache.find(filename);
    if (cacheIter != _fullPathCache.end())
    {
        return cacheIter->second;
    }

    std::string fullpath;

    for (const auto& searchIt : _searchPathArray)
    {
        fullpath = this->getPathForFilename(filename, searchIt);

        if (!fullpath.empty())
        {
            // Using the filename passed in as key.
            _fullPathCache.emplace(filename, fullpath);
            return fullpath;
        }
    }

    if (isPopupNotify())
    {
        AXLOG("axmol: fullPathForFilename: No file found at %s. Possible missing file.", filename.data());
    }

    // The file wasn't found, return empty string.
    return std::string{};
}

std::string FileUtils::fullPathForDirectory(std::string_view dir) const
{
    DECLARE_GUARD;

    auto result = std::string();

    if (dir.empty())
    {
        // result is ""
    }
    else if (isAbsolutePath(dir))
    {
        result = dir;
    }
    else
    {
        // Already Cached ?
        auto cacheIter = _fullPathCacheDir.find(dir);
        if (cacheIter != _fullPathCacheDir.end())
        {
            result = cacheIter->second;
        }
        else
        {
            std::string longdir{dir};

            if (longdir[longdir.length() - 1] != '/')
            {
                longdir += "/";
            }

            for (const auto& searchIt : _searchPathArray)
            {
                auto fullpath = this->getPathForDirectory(longdir, searchIt);
                if (!fullpath.empty() && isDirectoryExistInternal(fullpath))
                {
                    // Using the filename passed in as key.
                    _fullPathCacheDir.emplace(dir, fullpath);
                    result = fullpath;
                    break;
                }
            }

            if (result.empty() && isPopupNotify())
            {
                AXLOG("axmol: fullPathForDirectory: No directory found at %s. Possible missing directory.", dir.data());
            }
        }
    }

    if (!result.empty() && result.back() != '/')
    {
        result += '/';
    }

    return result;
}

std::string FileUtils::fullPathFromRelativeFile(std::string_view filename, std::string_view relativeFile) const
{
    return std::string{relativeFile.substr(0, relativeFile.rfind('/') + 1)}.append(filename);
}

const std::vector<std::string>& FileUtils::getSearchPaths() const
{
    DECLARE_GUARD;
    return _searchPathArray;
}

const std::vector<std::string>& FileUtils::getOriginalSearchPaths() const
{
    DECLARE_GUARD;
    return _originalSearchPaths;
}

void FileUtils::setWritablePath(std::string_view writablePath)
{
    DECLARE_GUARD;
    _writablePath = writablePath;
}

const std::string& FileUtils::getDefaultResourceRootPath() const
{
    DECLARE_GUARD;
    return _defaultResRootPath;
}

void FileUtils::setDefaultResourceRootPath(std::string_view path)
{
    DECLARE_GUARD;
    if (_defaultResRootPath != path)
    {
        _fullPathCache.clear();
        _fullPathCacheDir.clear();
        _defaultResRootPath = path;
        if (!_defaultResRootPath.empty() && _defaultResRootPath[_defaultResRootPath.length() - 1] != '/')
        {
            _defaultResRootPath += '/';
        }

        // Updates search paths
        setSearchPaths(_originalSearchPaths);
    }
}

void FileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
    DECLARE_GUARD;
    bool existDefaultRootPath = false;
    _originalSearchPaths      = searchPaths;

    _fullPathCache.clear();
    _fullPathCacheDir.clear();
    _searchPathArray.clear();

    for (const auto& path : _originalSearchPaths)
    {
        std::string prefix;
        std::string fullPath;

        if (!isAbsolutePath(path))
        {  // Not an absolute path
            prefix = _defaultResRootPath;
        }
        fullPath = prefix + path;
        if (!path.empty() && path[path.length() - 1] != '/')
        {
            fullPath += "/";
        }
        if (!existDefaultRootPath && path == _defaultResRootPath)
        {
            existDefaultRootPath = true;
        }
        _searchPathArray.emplace_back(fullPath);
    }

    if (!existDefaultRootPath)
    {
        // AXLOG("Default root path doesn't exist, adding it.");
        _searchPathArray.emplace_back(_defaultResRootPath);
    }
}

void FileUtils::addSearchPath(std::string_view searchpath, const bool front)
{
    DECLARE_GUARD;
    std::string path;
    if (!isAbsolutePath(searchpath))
        path = _defaultResRootPath;

    path.append(searchpath);
    if (!path.empty() && path[path.length() - 1] != '/')
    {
        path += "/";
    }

#ifdef AX_NO_DUP_SEARCH_PATH
    auto it = std::find(_searchPathArray.begin(), _searchPathArray.end(), path);
    if (it != _searchPathArray.end())
    {
        _searchPathArray.erase(it);

        auto itOrigin = std::find(_originalSearchPaths.begin(), _originalSearchPaths.end(), std::string{searchpath});
        if (itOrigin != _originalSearchPaths.end())
        {
            _originalSearchPaths.erase(itOrigin);
        }
    }
#endif  // AX_NO_DUP_SEARCH_PATH

    if (front)
    {
        _originalSearchPaths.insert(_originalSearchPaths.begin(), std::string{searchpath});
        _searchPathArray.insert(_searchPathArray.begin(), std::move(path));
    }
    else
    {
        _originalSearchPaths.emplace_back(std::string{searchpath});
        _searchPathArray.emplace_back(std::move(path));
    }
}

std::string FileUtils::getFullPathForFilenameWithinDirectory(std::string_view directory,
                                                             std::string_view filename) const
{
    // get directory+filename, safely adding '/' as necessary
    std::string ret{directory};
    if (!directory.empty() && directory[directory.size() - 1] != '/')
    {
        ret += '/';
    }
    ret += filename;
    // if the file doesn't exist, return an empty string

    if (isFileExistInternal(ret))
        return ret;
    return std::string{};
}

bool FileUtils::isFileExist(std::string_view filename) const
{
    if (isAbsolutePath(filename))
    {
        return isFileExistInternal(filename);
    }
    else
    {
        std::string fullpath = fullPathForFilename(filename);
        return !fullpath.empty();
    }
}

void FileUtils::isFileExist(std::string_view filename, std::function<void(bool)> callback) const
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread(
        [path = std::string{fullPath}]() -> bool { return FileUtils::getInstance()->isFileExist(path); },
        std::move(callback));
}

bool FileUtils::isAbsolutePath(std::string_view path) const
{
    return isAbsolutePathInternal(path);
}

bool FileUtils::isAbsolutePathInternal(std::string_view path)
{
    const char* raw = path.data();
#if defined(_WIN32)
    // see also: https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file?redirectedfrom=MSDN
    return ((path.length() > 2 && ((raw[0] >= 'a' && raw[0] <= 'z') || (raw[0] >= 'A' && raw[0] <= 'Z')) &&
             raw[1] == ':')                         // Normal absolute path
            || cxx20::starts_with(path, R"(\\?\)")  // Win32 File Namespaces for Long Path
            || cxx20::starts_with(path, R"(\\.\)")  // Win32 Device Namespaces for device
            || (raw[0] == '/' || raw[0] == '\\')    // Current disk drive
    );
#else
    return (raw[0] == '/');
#endif
}

bool FileUtils::isDirectoryExist(std::string_view dirPath) const
{
    AXASSERT(!dirPath.empty(), "Invalid path");

    DECLARE_GUARD;

    if (isAbsolutePath(dirPath))
    {
        return isDirectoryExistInternal(dirPath);
    }
    else
    {
        auto fullPath = fullPathForDirectory(dirPath);
        return !fullPath.empty();
    }
}

void FileUtils::isDirectoryExist(std::string_view fullPath, std::function<void(bool)> callback) const
{
    AXASSERT(isAbsolutePath(fullPath), "Async isDirectoryExist only accepts absolute file paths");
    performOperationOffthread(
        [path = std::string{fullPath}]() -> bool { return FileUtils::getInstance()->isDirectoryExist(path); },
        std::move(callback));
}

void FileUtils::createDirectory(std::string_view dirPath, std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{dirPath}]() -> bool { return FileUtils::getInstance()->createDirectory(path); },
        std::move(callback));
}

void FileUtils::removeDirectory(std::string_view dirPath, std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{dirPath}]() -> bool { return FileUtils::getInstance()->removeDirectory(path); },
        std::move(callback));
}

void FileUtils::removeFile(std::string_view filepath, std::function<void(bool)> callback) const
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread(
        [path = std::string{fullPath}]() -> bool { return FileUtils::getInstance()->removeFile(path); },
        std::move(callback));
}

void FileUtils::renameFile(std::string_view path,
                           std::string_view oldname,
                           std::string_view name,
                           std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [path = std::string{path}, oldname = std::string{oldname}, name = std::string{name}]() -> bool {
            return FileUtils::getInstance()->renameFile(path, oldname, name);
        },
        std::move(callback));
}

void FileUtils::renameFile(std::string_view oldfullpath,
                           std::string_view newfullpath,
                           std::function<void(bool)> callback) const
{
    performOperationOffthread(
        [oldpath = std::string{oldfullpath}, newpath = std::string{newfullpath}]() {
        return FileUtils::getInstance()->renameFile(oldpath, newpath);
        },
        std::move(callback));
}

void FileUtils::getFileSize(std::string_view filepath, std::function<void(int64_t)> callback) const
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread([path = std::string{fullPath}]() { return FileUtils::getInstance()->getFileSize(path); },
                              std::move(callback));
}

void FileUtils::listFilesAsync(std::string_view dirPath, std::function<void(std::vector<std::string>)> callback) const
{
    auto fullPath = fullPathForDirectory(dirPath);
    performOperationOffthread([path = std::string{fullPath}]() { return FileUtils::getInstance()->listFiles(path); },
                              std::move(callback));
}

void FileUtils::listFilesRecursivelyAsync(std::string_view dirPath,
                                          std::function<void(std::vector<std::string>)> callback) const
{
    auto fullPath = fullPathForDirectory(dirPath);
    performOperationOffthread(
        [path = std::string{fullPath}]() {
        std::vector<std::string> retval;
        FileUtils::getInstance()->listFilesRecursively(path, &retval);
        return retval;
        },
        std::move(callback));
}

std::unique_ptr<IFileStream> FileUtils::openFileStream(std::string_view filePath, IFileStream::Mode mode) const
{
    FileStream fs;
    return fs.open(filePath, mode) ? std::make_unique<FileStream>(std::move(fs)) : nullptr;
}

/* !!!Notes for c++fs
 a. ios: require ios 13.0+, currently use ghc as workaround in lower ios 13.0- devices
 b. android: require ndk-r22+
*/
std::vector<std::string> FileUtils::listFiles(std::string_view dirPath) const
{
    const auto fullPath = fullPathForDirectory(dirPath);
    auto fsPath         = toFspath(fullPath);
    if (!stdfs::is_directory(fsPath))
    {
        return {};
    }
    std::vector<std::string> files = {};
    for (const auto& entry : stdfs::directory_iterator(fsPath))
    {
        const auto isDir = entry.is_directory();
        if (isDir || entry.is_regular_file())
        {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
            /*
             * Because the object memory model of std::u8string is identical to std::string
             * so we use force cast to std::string without `memory alloc & copy`, the ASM code will be:
             *   00F03204  lea         eax,[ebp-28h]
             *   00F03207  lea         ecx,[edi+20h]
             *   00F0320A  push        eax
             *   008E320B  call        std::filesystem::path::u8string (08E1C40h)
             *   008E3210  mov         esi,eax
             *   008E3212  mov         byte ptr [ebp-4],6
             */
            auto pathU8Str = entry.path().u8string();
            auto& pathStr  = *reinterpret_cast<std::string*>(&pathU8Str);
            std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
#else
            std::string pathStr = entry.path().string();
#endif
            if (isDir)
                pathStr += '/';
            files.emplace_back(std::move(pathStr));
        }
    }
    return files;
}

void FileUtils::listFilesRecursively(std::string_view dirPath, std::vector<std::string>* files) const
{
    const auto fullPath = fullPathForDirectory(dirPath);
    auto fsPath         = toFspath(fullPath);
    if (!stdfs::is_directory(fsPath))
    {
        return;
    }
    for (const auto& entry : stdfs::recursive_directory_iterator(fsPath))
    {
        const auto isDir = entry.is_directory();
        if (isDir || entry.is_regular_file())
        {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
            auto pathU8Str = entry.path().u8string();
            auto& pathStr  = *reinterpret_cast<std::string*>(&pathU8Str);
            std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
#else
            std::string pathStr = entry.path().string();
#endif
            if (isDir)
                pathStr += '/';
            files->emplace_back(std::move(pathStr));
        }
    }
}

#if defined(_WIN32)
// windows os implement should override in platform specific FileUtiles class
bool FileUtils::isDirectoryExistInternal(std::string_view dirPath) const
{
    AXASSERT(false, "FileUtils not support isDirectoryExistInternal");
    return false;
}

bool FileUtils::createDirectory(std::string_view path) const
{
    AXASSERT(false, "FileUtils not support createDirectory");
    return false;
}

bool FileUtils::removeDirectory(std::string_view path) const
{
    AXASSERT(false, "FileUtils not support removeDirectory");
    return false;
}

bool FileUtils::removeFile(std::string_view path) const
{
    AXASSERT(false, "FileUtils not support removeFile");
    return false;
}

bool FileUtils::renameFile(std::string_view oldfullpath, std::string_view newfullpath) const
{
    AXASSERT(false, "FileUtils not support renameFile");
    return false;
}

bool FileUtils::renameFile(std::string_view path, std::string_view oldname, std::string_view name) const
{
    AXASSERT(false, "FileUtils not support renameFile");
    return false;
}

int64_t FileUtils::getFileSize(std::string_view filepath) const
{
    AXASSERT(false, "getFileSize should be override by platform FileUtils");
    return 0;
}

#else
// default implements for unix like os
#    include <sys/types.h>
#    include <errno.h>
#    include <dirent.h>

// android doesn't have ftw.h
#    if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID)
#        include <ftw.h>
#    endif

bool FileUtils::isDirectoryExistInternal(std::string_view dirPath) const
{
    struct stat st;
    if (stat(dirPath.data(), &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool FileUtils::createDirectory(std::string_view path) const
{
    AXASSERT(!path.empty(), "Invalid path");

    if (isDirectoryExist(path))
        return true;

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string_view subpath;
    std::vector<std::string> dirs;

    if (found != std::string::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.emplace_back(std::string{subpath});
            start = found + 1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos)
            {
                if (start < path.length())
                {
                    dirs.emplace_back(std::string{path.substr(start)});
                }
                break;
            }
        }
    }

    DIR* dir = NULL;

    // Create path recursively
    std::string strSubpath;
    for (const auto& iter : dirs)
    {
        strSubpath += iter;
        dir = opendir(strSubpath.c_str());

        if (!dir)
        {
            // directory doesn't exist, should create a new one

            int ret = mkdir(strSubpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret != 0 && (errno != EEXIST))
            {
                // current directory can not be created, sub directories can not be created too
                // should return
                return false;
            }
        }
        else
        {
            // directory exists, should close opened dir
            closedir(dir);
        }
    }
    return true;
}

namespace
{
#    if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID)
int unlink_cb(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}
#    endif
}  // namespace

bool FileUtils::removeDirectory(std::string_view path) const
{
#    if !defined(AX_TARGET_OS_TVOS)

#        if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID)
    if (nftw(path.data(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS) == -1)
        return false;
    else
        return true;
#        else
    std::string command = "rm -r \""s;
    // Path may include space.
    command.append(path).append("\"", 1);
    if (system(command.c_str()) >= 0)
        return true;
    else
        return false;
#        endif  // (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID)

#    else
    return false;
#    endif  // !defined(AX_TARGET_OS_TVOS)
}

bool FileUtils::removeFile(std::string_view path) const
{
    if (remove(path.data()))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool FileUtils::renameFile(std::string_view oldfullpath, std::string_view newfullpath) const
{
    AXASSERT(!oldfullpath.empty(), "Invalid path");
    AXASSERT(!newfullpath.empty(), "Invalid path");

    int errorCode = rename(oldfullpath.data(), newfullpath.data());

    if (0 != errorCode)
    {
        AXLOGERROR("Fail to rename file %s to %s !Error code is %d", oldfullpath.data(), newfullpath.data(), errorCode);
        return false;
    }
    return true;
}

bool FileUtils::renameFile(std::string_view path, std::string_view oldname, std::string_view name) const
{
    AXASSERT(!path.empty(), "Invalid path");
    std::string oldPath{path};
    oldPath += oldname;
    std::string newPath{path};
    newPath += name;

    return this->renameFile(oldPath, newPath);
}

int64_t FileUtils::getFileSize(std::string_view filepath) const
{
    AXASSERT(!filepath.empty(), "Invalid path");

    std::string_view path;
    std::string fullpath;
    if (!isAbsolutePath(filepath))
    {
        fullpath = fullPathForFilename(filepath);
        if (fullpath.empty())
            return -1;
        path = fullpath;
    }
    else
        path = filepath;

    struct stat info;
    // Get data associated with "crt_stat.c":
    int result = ::stat(path.data(), &info);

    // Check if statistics are valid:
    if (result != 0)
    {
        // Failed
        return -1;
    }
    else
    {
        return info.st_size;
    }
}

#endif

//////////////////////////////////////////////////////////////////////////
// Notification support when getFileData from invalid file path.
//////////////////////////////////////////////////////////////////////////
static bool s_popupNotify = true;

void FileUtils::setPopupNotify(bool notify)
{
    s_popupNotify = notify;
}

bool FileUtils::isPopupNotify() const
{
    return s_popupNotify;
}

std::string FileUtils::getFileExtension(std::string_view filePath) const
{
    std::string fileExtension;
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = filePath.substr(pos, filePath.length());

        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    }

    return fileExtension;
}

std::string FileUtils::getFileShortName(std::string_view filePath)
{
    // std::string fileExtension;
    size_t pos = filePath.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return std::string{filePath.substr(pos + 1)};
    }

    return std::string{filePath};
}

void FileUtils::valueMapCompact(ValueMap& /*valueMap*/) const {}

void FileUtils::valueVectorCompact(ValueVector& /*valueVector*/) const {}

NS_AX_END
