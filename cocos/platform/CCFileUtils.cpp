/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
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

#include "platform/CCFileUtils.h"

#include <stack>
#include <sstream>

#include "base/CCData.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "platform/CCSAXParser.h"
//#include "base/ccUtils.h"
#include "platform/CCPosixFileStream.h"

#ifdef MINIZIP_FROM_SYSTEM
#include <minizip/unzip.h>
#else // from our embedded sources
#include "unzip.h"
#endif
#include <sys/stat.h>

#include "pugixml/pugixml.hpp"
#define DECLARE_GUARD (void)0

NS_CC_BEGIN

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
}SAXState;

typedef enum
{
    SAX_RESULT_NONE = 0,
    SAX_RESULT_DICT,
    SAX_RESULT_ARRAY
}SAXResult;

class DictMaker : public SAXDelegator
{
public:
    SAXResult _resultType;
    ValueMap _rootDict;
    ValueVector _rootArray;

    std::string _curKey;   ///< parsed key
    std::string _curValue; // parsed value
    SAXState _state;

    ValueMap*  _curDict;
    ValueVector* _curArray;

    std::stack<ValueMap*> _dictStack;
    std::stack<ValueVector*> _arrayStack;
    std::stack<SAXState>  _stateStack;

public:
    DictMaker()
        : _resultType(SAX_RESULT_NONE)
        , _state(SAX_NONE)
        , _curDict(nullptr)
        , _curArray(nullptr)
    {
    }

    ~DictMaker()
    {
    }

    ValueMap dictionaryWithContentsOfFile(const std::string& fileName)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootDict;
    }

    ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(filedata, filesize);
        return _rootDict;
    }

    ValueVector arrayWithContentsOfFile(const std::string& fileName)
    {
        _resultType = SAX_RESULT_ARRAY;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootArray;
    }

    void startElement(void *ctx, const char *name, const char **atts) override
    {
        const std::string sName(name);
        if( sName == "dict" )
        {
            if(_resultType == SAX_RESULT_DICT && _rootDict.empty())
            {
                _curDict = &_rootDict;
            }

            _state = SAX_DICT;

            SAXState preState = SAX_NONE;
            if (! _stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (SAX_ARRAY == preState)
            {
                // add a new dictionary into the array
                _curArray->push_back(Value(ValueMap()));
                _curDict = &(_curArray->rbegin())->asValueMap();
            }
            else if (SAX_DICT == preState)
            {
                // add a new dictionary into the pre dictionary
                CCASSERT(! _dictStack.empty(), "The state is wrong!");
                ValueMap* preDict = _dictStack.top();
                (*preDict)[_curKey] = Value(ValueMap());
                _curDict = &(*preDict)[_curKey].asValueMap();
            }

            // record the dict state
            _stateStack.push(_state);
            _dictStack.push(_curDict);
        }
        else if(sName == "key")
        {
            _state = SAX_KEY;
        }
        else if(sName == "integer")
        {
            _state = SAX_INT;
        }
        else if(sName == "real")
        {
            _state = SAX_REAL;
        }
        else if(sName == "string")
        {
            _state = SAX_STRING;
        }
        else if (sName == "array")
        {
            _state = SAX_ARRAY;

            if (_resultType == SAX_RESULT_ARRAY && _rootArray.empty())
            {
                _curArray = &_rootArray;
            }
            SAXState preState = SAX_NONE;
            if (! _stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (preState == SAX_DICT)
            {
                (*_curDict)[_curKey] = Value(ValueVector());
                _curArray = &(*_curDict)[_curKey].asValueVector();
            }
            else if (preState == SAX_ARRAY)
            {
                CCASSERT(! _arrayStack.empty(), "The state is wrong!");
                ValueVector* preArray = _arrayStack.top();
                preArray->push_back(Value(ValueVector()));
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

    void endElement(void *ctx, const char *name) override
    {
        SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string sName((char*)name);
        if( sName == "dict" )
        {
            _stateStack.pop();
            _dictStack.pop();
            if ( !_dictStack.empty())
            {
                _curDict = _dictStack.top();
            }
        }
        else if (sName == "array")
        {
            _stateStack.pop();
            _arrayStack.pop();
            if (! _arrayStack.empty())
            {
                _curArray = _arrayStack.top();
            }
        }
        else if (sName == "true")
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->push_back(Value(true));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(true);
            }
        }
        else if (sName == "false")
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->push_back(Value(false));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(false);
            }
        }
        else if (sName == "string" || sName == "integer" || sName == "real")
        {
            if (SAX_ARRAY == curState)
            {
                if (sName == "string")
                    _curArray->push_back(Value(_curValue));
                else if (sName == "integer")
                    _curArray->push_back(Value(atoi(_curValue.c_str())));
                else
                    _curArray->push_back(Value(std::atof(_curValue.c_str())));
            }
            else if (SAX_DICT == curState)
            {
                if (sName == "string")
                    (*_curDict)[_curKey] = Value(_curValue);
                else if (sName == "integer")
                    (*_curDict)[_curKey] = Value(atoi(_curValue.c_str()));
                else
                    (*_curDict)[_curKey] = Value(std::atof(_curValue.c_str()));
            }

            _curValue.clear();
        }

        _state = SAX_NONE;
    }

    void textHandler(void *ctx, const char *ch, size_t len) override
    {
        if (_state == SAX_NONE)
        {
            return;
        }

        SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string text = std::string((char*)ch,len);

        switch(_state)
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
                    CCASSERT(!_curKey.empty(), "key not found : <integer/real>");
                }

                _curValue.append(text);
            }
            break;
        default:
            break;
        }
    }
};

ValueMap FileUtils::getValueMapFromFile(const std::string& filename) const
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

ValueVector FileUtils::getValueVectorFromFile(const std::string& filename) const
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
bool FileUtils::writeToFile(const ValueMap& dict, const std::string &fullPath) const
{
    return writeValueMapToFile(dict, fullPath);
}

bool FileUtils::writeValueMapToFile(const ValueMap& dict, const std::string& fullPath) const
{
    pugi::xml_document doc;
	doc.load_string(R"(<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist />)", pugi::parse_full);

	auto rootEle = doc.document_element();
	
    generateElementForDict(dict, rootEle);
    std::stringstream ss;
    doc.save(ss, "  ");
    return writeStringToFile(ss.str(), fullPath);
}

bool FileUtils::writeValueVectorToFile(const ValueVector& vecData, const std::string& fullPath) const
{
	pugi::xml_document doc;
	doc.load_string(R"(<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist />)", pugi::parse_full);

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
		auto node = parent.append_child("string");
		node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString().c_str());
    }
    // object is integer
    else if (value.getType() == Value::Type::INTEGER)
    {
		auto node = parent.append_child("integer");
		node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString().c_str());
    }
    // object is real
    else if (value.getType() == Value::Type::FLOAT || value.getType() == Value::Type::DOUBLE)
    {
		auto node = parent.append_child("real");
		node.append_child(pugi::xml_node_type::node_pcdata).set_value(value.asString().c_str());
    }
    //object is bool
    else if (value.getType() == Value::Type::BOOLEAN) {
		parent.append_child(value.asString().c_str());
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
    auto dictDS = parent.append_child("dict");
    for (const auto &iter : dict)
    {
		auto key = dictDS.append_child("key");
		key.append_child(pugi::xml_node_type::node_pcdata).set_value(iter.first.c_str());

        generateElementForObject(iter.second, dictDS);
    }
}

static void generateElementForArray(const ValueVector& array, pugi::xml_node& parent)
{
    auto arrayDS = parent.append_child("array");
    for(const auto &value : array) {
        generateElementForObject(value, arrayDS);
    }
}




// Implement FileUtils
FileUtils* FileUtils::s_sharedFileUtils = nullptr;

void FileUtils::destroyInstance()
{
    CC_SAFE_DELETE(s_sharedFileUtils);
}

void FileUtils::setDelegate(FileUtils *delegate)
{
    if (s_sharedFileUtils)
        delete s_sharedFileUtils;

    s_sharedFileUtils = delegate;
}

FileUtils::FileUtils()
    : _writablePath("")
{
}

FileUtils::~FileUtils()
{
}

bool FileUtils::writeStringToFile(const std::string& dataStr, const std::string& fullPath) const
{
    return FileUtils::writeBinaryToFile(dataStr.c_str(), dataStr.size(), fullPath);
}

void FileUtils::writeStringToFile(std::string dataStr, const std::string& fullPath, std::function<void(bool)> callback) const
{
    performOperationOffthread([fullPath](const std::string& dataStrIn) -> bool {
        return FileUtils::getInstance()->writeStringToFile(dataStrIn, fullPath);
    }, std::move(callback),std::move(dataStr));
}

bool FileUtils::writeDataToFile(const Data& data, const std::string& fullPath) const
{
    return FileUtils::writeBinaryToFile(data.getBytes(), data.getSize(), fullPath);
}

void FileUtils::writeDataToFile(Data data, const std::string& fullPath, std::function<void(bool)> callback) const
{
    performOperationOffthread([fullPath](const Data& dataIn) -> bool {
        return FileUtils::getInstance()->writeDataToFile(dataIn, fullPath);
    }, std::move(callback), std::move(data));
}

bool FileUtils::writeBinaryToFile(const void* data, size_t dataSize, const std::string& fullPath)
{
    CCASSERT(!fullPath.empty() && dataSize > 0, "Invalid parameters.");

    auto* fileUtils = FileUtils::getInstance();
    do
    {
        auto* fileStream = fileUtils->openFileStream(fullPath, FileStream::Mode::WRITE);
        // Read the file from hardware
        CC_BREAK_IF(!fileStream);

        fileStream->write(data, dataSize);
        delete fileStream;
        return true;
    } while (0);

    return false;
}

bool FileUtils::init()
{
    DECLARE_GUARD;
    _searchPathArray.push_back(_defaultResRootPath);
    _searchResolutionsOrderArray.emplace_back("");
    return true;
}

void FileUtils::purgeCachedEntries()
{
    DECLARE_GUARD;
    _fullPathCache.clear();
    _fullPathCacheDir.clear();
}

std::string FileUtils::getStringFromFile(const std::string& filename) const
{
    std::string s;
    getContents(filename, &s);
    return s;
}

void FileUtils::getStringFromFile(const std::string &path, std::function<void (std::string)> callback) const
{
    // Get the full path on the main thread, to avoid the issue that FileUtil's is not
    // thread safe, and accessing the fullPath cache and searching the search paths is not thread safe
    auto fullPath = fullPathForFilename(path);
    performOperationOffthread([fullPath]() -> std::string {
        return FileUtils::getInstance()->getStringFromFile(fullPath);
    }, std::move(callback));
}

Data FileUtils::getDataFromFile(const std::string& filename) const
{
    Data d;
    getContents(filename, &d);
    return d;
}

void FileUtils::getDataFromFile(const std::string& filename, std::function<void(Data)> callback) const
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread([fullPath]() -> Data {
        return FileUtils::getInstance()->getDataFromFile(fullPath);
    }, std::move(callback));
}



FileUtils::Status FileUtils::getContents(const std::string& filename, ResizableBuffer* buffer) const
{
    if (filename.empty())
        return Status::NotExists;

    auto fileUtils = FileUtils::getInstance();

    const auto fullPath = fileUtils->fullPathForFilename(filename);

    auto* fileStream = fileUtils->openFileStream(fullPath, FileStream::Mode::READ);
    if (!fileStream)
        return Status::OpenFailed;

    if (fileStream->seek(0, SEEK_END) != 0)
    {
        delete fileStream;
        return Status::ObtainSizeFailed;
    }

    const auto size = fileStream->tell();
    if (size <= 0)
    {
        delete fileStream;
        return Status::OK;
    }

    if (size > ULONG_MAX)
    {
        delete fileStream;
        return Status::TooLarge;
    }

    buffer->resize(size);

    fileStream->seek(0, SEEK_SET);

    const auto sizeRead = fileStream->read(buffer->buffer(), size);
    if (sizeRead < size) {
        buffer->resize(sizeRead);
        delete fileStream;
        return Status::ReadFailed;
    }

    delete fileStream;
    return Status::OK;
}

void FileUtils::writeValueMapToFile(ValueMap dict, const std::string& fullPath, std::function<void(bool)> callback) const
{
    
    performOperationOffthread([fullPath](const ValueMap& dictIn) -> bool {
        return FileUtils::getInstance()->writeValueMapToFile(dictIn, fullPath);
    }, std::move(callback), std::move(dict));
}

void FileUtils::writeValueVectorToFile(ValueVector vecData, const std::string& fullPath, std::function<void(bool)> callback) const
{
    performOperationOffthread([fullPath] (const ValueVector& vecDataIn) -> bool {
        return FileUtils::getInstance()->writeValueVectorToFile(vecDataIn, fullPath);
    }, std::move(callback), std::move(vecData));
}

std::string FileUtils::getNewFilename(const std::string &filename) const
{
    std::string newFileName;
    
    DECLARE_GUARD;

    // in Lookup Filename dictionary ?
    auto iter = _filenameLookupDict.find(filename);

    if (iter == _filenameLookupDict.end())
    {
        newFileName = filename;
    }
    else
    {
        newFileName = iter->second.asString();
    }
    return newFileName;
}

std::string FileUtils::getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath) const
{
    std::string file = filename;
    std::string file_path = "";
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos)
    {
        file_path = filename.substr(0, pos+1);
        file = filename.substr(pos+1);
    }

    // searchPath + file_path + resourceDirectory
    std::string path = searchPath;
    path += file_path;
    path += resolutionDirectory;

    path = getFullPathForFilenameWithinDirectory(path, file);

    return path;
}

std::string FileUtils::getPathForDirectory(const std::string &dir, const std::string &resolutionDiretory, const std::string &searchPath) const
{
    return searchPath + resolutionDiretory + dir;
}

std::string FileUtils::fullPathForFilename(const std::string &filename) const
{
    
    DECLARE_GUARD;

    if (filename.empty())
    {
        return "";
    }

    /*
    * As we know, this function 'fullPathForFilename' will be calling when load any file, how to ensure this thread safe
    * Step:
    *    a. call this function at main-thread always
    *    b. use the really fullPath to call getStringFromFile/getDataFromFile at sub-thread
    *    c. then this function will call again with really fullPath
    *    d. then isAbsolutePath avoid to access _fullPathCache _fullPathCache concurrent
    */
    if (isAbsolutePath(filename))
    {
        return filename;
    }

    // Already Cached ?
    auto cacheIter = _fullPathCache.find(filename);
    if(cacheIter != _fullPathCache.end())
    {
        return cacheIter->second;
    }

    // Get the new file name.
    const std::string newFilename( getNewFilename(filename) );

    std::string fullpath;

    for (const auto& searchIt : _searchPathArray)
    {
        for (const auto& resolutionIt : _searchResolutionsOrderArray)
        {
            fullpath = this->getPathForFilename(newFilename, resolutionIt, searchIt);

            if (!fullpath.empty())
            {
                // Using the filename passed in as key.
                _fullPathCache.emplace(filename, fullpath);
                return fullpath;
            }

        }
    }

    if(isPopupNotify()){
        CCLOG("cocos2d: fullPathForFilename: No file found at %s. Possible missing file.", filename.c_str());
    }

    // The file wasn't found, return empty string.
    return "";
}


std::string FileUtils::fullPathForDirectory(const std::string &dir) const
{
    DECLARE_GUARD;

    if (dir.empty())
    {
        return "";
    }

    if (isAbsolutePath(dir))
    {
        return dir;
    }

    // Already Cached ?
    auto cacheIter = _fullPathCacheDir.find(dir);
    if(cacheIter != _fullPathCacheDir.end())
    {
        return cacheIter->second;
    }
    std::string longdir = dir;
    std::string fullpath;

    if(longdir[longdir.length() - 1] != '/')
    {
        longdir +="/";
    }

    const std::string newdirname( getNewFilename(longdir) );
    
    for (const auto& searchIt : _searchPathArray)
    {
        for (const auto& resolutionIt : _searchResolutionsOrderArray)
        {
            fullpath = this->getPathForDirectory(newdirname, resolutionIt, searchIt);
            if (!fullpath.empty() && isDirectoryExistInternal(fullpath))
            {
                // Using the filename passed in as key.
                _fullPathCacheDir.emplace(dir, fullpath);
                return fullpath;
            }

        }
    }

    if(isPopupNotify()){
        CCLOG("cocos2d: fullPathForDirectory: No directory found at %s. Possible missing directory.", dir.c_str());
    }

    // The file wasn't found, return empty string.
    return "";
}

std::string FileUtils::fullPathFromRelativeFile(const std::string &filename, const std::string &relativeFile) const
{
    return relativeFile.substr(0, relativeFile.rfind('/')+1) + getNewFilename(filename);
}

void FileUtils::setSearchResolutionsOrder(const std::vector<std::string>& searchResolutionsOrder)
{
    DECLARE_GUARD;

    if (_searchResolutionsOrderArray == searchResolutionsOrder)
    {
        return;
    }

    bool existDefault = false;

    _fullPathCache.clear();
    _fullPathCacheDir.clear();
    _searchResolutionsOrderArray.clear();
    for(const auto& iter : searchResolutionsOrder)
    {
        std::string resolutionDirectory = iter;
        if (!existDefault && resolutionDirectory == "")
        {
            existDefault = true;
        }

        if (!resolutionDirectory.empty() && resolutionDirectory[resolutionDirectory.length()-1] != '/')
        {
            resolutionDirectory += "/";
        }

        _searchResolutionsOrderArray.push_back(resolutionDirectory);
    }

    if (!existDefault)
    {
        _searchResolutionsOrderArray.push_back("");
    }
}

void FileUtils::addSearchResolutionsOrder(const std::string &order,const bool front)
{
    
    DECLARE_GUARD;

    std::string resOrder = order;
    if (!resOrder.empty() && resOrder[resOrder.length()-1] != '/')
        resOrder.push_back('/');

    if (front) {
        _searchResolutionsOrderArray.insert(_searchResolutionsOrderArray.begin(), resOrder);
    } else {
        _searchResolutionsOrderArray.push_back(resOrder);
    }
}

const std::vector<std::string> FileUtils::getSearchResolutionsOrder() const
{
    DECLARE_GUARD;
    return _searchResolutionsOrderArray;
}

const std::vector<std::string> FileUtils::getSearchPaths() const
{
    DECLARE_GUARD;
    return _searchPathArray;
}

const std::vector<std::string> FileUtils::getOriginalSearchPaths() const
{
    DECLARE_GUARD;
    return _originalSearchPaths;
}

void FileUtils::setWritablePath(const std::string& writablePath)
{
    DECLARE_GUARD;
    _writablePath = writablePath;
}

const std::string FileUtils::getDefaultResourceRootPath() const
{
    DECLARE_GUARD;
    return _defaultResRootPath;
}

void FileUtils::setDefaultResourceRootPath(const std::string& path)
{
    DECLARE_GUARD;
    if (_defaultResRootPath != path)
    {
        _fullPathCache.clear();
        _fullPathCacheDir.clear();
        _defaultResRootPath = path;
        if (!_defaultResRootPath.empty() && _defaultResRootPath[_defaultResRootPath.length()-1] != '/')
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
    _originalSearchPaths = searchPaths;

    _fullPathCache.clear();
    _fullPathCacheDir.clear();
    _searchPathArray.clear();

    for (const auto& path : _originalSearchPaths)
    {
        std::string prefix;
        std::string fullPath;

        if (!isAbsolutePath(path))
        { // Not an absolute path
            prefix = _defaultResRootPath;
        }
        fullPath = prefix + path;
        if (!path.empty() && path[path.length()-1] != '/')
        {
            fullPath += "/";
        }
        if (!existDefaultRootPath && path == _defaultResRootPath)
        {
            existDefaultRootPath = true;
        }
        _searchPathArray.push_back(fullPath);
    }

    if (!existDefaultRootPath)
    {
        //CCLOG("Default root path doesn't exist, adding it.");
        _searchPathArray.push_back(_defaultResRootPath);
    }
}

void FileUtils::addSearchPath(const std::string &searchpath,const bool front)
{
    DECLARE_GUARD;
    std::string prefix;
    if (!isAbsolutePath(searchpath))
        prefix = _defaultResRootPath;

    std::string path = prefix + searchpath;
    if (!path.empty() && path[path.length()-1] != '/')
    {
        path += "/";
    }

    if (front) {
        _originalSearchPaths.insert(_originalSearchPaths.begin(), searchpath);
        _searchPathArray.insert(_searchPathArray.begin(), path);
    } else {
        _originalSearchPaths.push_back(searchpath);
        _searchPathArray.push_back(path);
    }
}

void FileUtils::setFilenameLookupDictionary(const ValueMap& filenameLookupDict)
{
    DECLARE_GUARD;
    _fullPathCache.clear();
    _fullPathCacheDir.clear();
    _filenameLookupDict = filenameLookupDict;
}

void FileUtils::loadFilenameLookupDictionaryFromFile(const std::string &filename)
{
    const std::string fullPath = fullPathForFilename(filename);
    if (!fullPath.empty())
    {
        ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
        if (!dict.empty())
        {
            ValueMap& metadata =  dict["metadata"].asValueMap();
            int version = metadata["version"].asInt();
            if (version != 1)
            {
                CCLOG("cocos2d: ERROR: Invalid filenameLookup dictionary version: %d. Filename: %s", version, filename.c_str());
                return;
            }
            setFilenameLookupDictionary( dict["filenames"].asValueMap());
        }
    }
}

std::string FileUtils::getFullPathForFilenameWithinDirectory(const std::string& directory, const std::string& filename) const
{
    // get directory+filename, safely adding '/' as necessary
    std::string ret = directory;
    if (directory.size() && directory[directory.size()-1] != '/'){
        ret += '/';
    }
    ret += filename;
    // if the file doesn't exist, return an empty string
    if (!isFileExistInternal(ret)) {
        ret = "";
    }
    return ret;
}

bool FileUtils::isFileExist(const std::string& filename) const
{
    if (isAbsolutePath(filename))
    {
        return isFileExistInternal(filename);
    }
    else
    {
        std::string fullpath = fullPathForFilename(filename);
        if (fullpath.empty())
            return false;
        else
            return true;
    }
}

void FileUtils::isFileExist(const std::string& filename, std::function<void(bool)> callback) const
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->isFileExist(fullPath);
    }, std::move(callback));
}

bool FileUtils::isAbsolutePath(const std::string& path) const
{
    return (path[0] == '/');
}

bool FileUtils::isDirectoryExist(const std::string& dirPath) const
{
    CCASSERT(!dirPath.empty(), "Invalid path");
    
    DECLARE_GUARD;

    if (isAbsolutePath(dirPath))
    {
        return isDirectoryExistInternal(dirPath);
    } else {
        auto fullPath = fullPathForDirectory(dirPath);
        return !fullPath.empty();
    }
}

void FileUtils::isDirectoryExist(const std::string& fullPath, std::function<void(bool)> callback) const
{
    CCASSERT(isAbsolutePath(fullPath), "Async isDirectoryExist only accepts absolute file paths");
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->isDirectoryExist(fullPath);
    }, std::move(callback));
}

void FileUtils::createDirectory(const std::string& dirPath, std::function<void(bool)> callback) const
{
    performOperationOffthread([dirPath]() -> bool {
        return FileUtils::getInstance()->createDirectory(dirPath);
    }, std::move(callback));
}

void FileUtils::removeDirectory(const std::string& dirPath, std::function<void(bool)> callback) const
{
    performOperationOffthread([dirPath]() -> bool {
        return FileUtils::getInstance()->removeDirectory(dirPath);
    }, std::move(callback));
}

void FileUtils::removeFile(const std::string &filepath, std::function<void (bool)> callback) const
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->removeFile(fullPath);
    }, std::move(callback));
}

void FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name, std::function<void(bool)> callback) const
{
    performOperationOffthread([path, oldname, name]() -> bool {
        return FileUtils::getInstance()->renameFile(path, oldname, name);
    }, std::move(callback));
                                
}

void FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath, std::function<void(bool)> callback) const
{
    performOperationOffthread([oldfullpath, newfullpath]() {
        return FileUtils::getInstance()->renameFile(oldfullpath, newfullpath);
    }, std::move(callback));
}

void FileUtils::getFileSize(const std::string &filepath, std::function<void(int64_t)> callback) const
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread([fullPath]() {
        return FileUtils::getInstance()->getFileSize(fullPath);
    }, std::move(callback));
}

void FileUtils::listFilesAsync(const std::string& dirPath, std::function<void(std::vector<std::string>)> callback) const
{
    auto fullPath = fullPathForDirectory(dirPath);
    performOperationOffthread([fullPath]() {
        return FileUtils::getInstance()->listFiles(fullPath);
    }, std::move(callback));
}

void FileUtils::listFilesRecursivelyAsync(const std::string& dirPath, std::function<void(std::vector<std::string>)> callback) const
{
    auto fullPath = fullPathForDirectory(dirPath);
    performOperationOffthread([fullPath]() {
        std::vector<std::string> retval;
        FileUtils::getInstance()->listFilesRecursively(fullPath, &retval);
        return retval;
    }, std::move(callback));
}

FileStream* FileUtils::openFileStream(const std::string& filePath, FileStream::Mode mode)
{
    PosixFileStream fs;

    if (fs.open(filePath, mode))
    {
        return new PosixFileStream(std::move(fs)); // PosixFileStream is the default implementation
    }

    return nullptr;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
// windows os implement should override in platform specific FileUtiles class
bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
    CCASSERT(false, "FileUtils not support isDirectoryExistInternal");
    return false;
}

bool FileUtils::createDirectory(const std::string& path) const
{
    CCASSERT(false, "FileUtils not support createDirectory");
    return false;
}

bool FileUtils::removeDirectory(const std::string& path) const
{
    CCASSERT(false, "FileUtils not support removeDirectory");
    return false;
}

bool FileUtils::removeFile(const std::string &path) const
{
    CCASSERT(false, "FileUtils not support removeFile");
    return false;
}

bool FileUtils::renameFile(const std::string &oldfullpath, const std::string& newfullpath) const
{
    CCASSERT(false, "FileUtils not support renameFile");
    return false;
}

bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name) const
{
    CCASSERT(false, "FileUtils not support renameFile");
    return false;
}

int64_t FileUtils::getFileSize(const std::string &filepath) const
{
    CCASSERT(false, "getFileSize should be override by platform FileUtils");
    return 0;
}

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) const
{
    CCASSERT(false, "FileUtils not support listFiles");
    return std::vector<std::string>();
}

void FileUtils::listFilesRecursively(const std::string& dirPath, std::vector<std::string> *files) const
{
    CCASSERT(false, "FileUtils not support listFilesRecursively");
    return;
}

#else
#include "tinydir/tinydir.h"
// default implements for unix like os
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

// android doesn't have ftw.h
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
#include <ftw.h>
#endif

bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool FileUtils::createDirectory(const std::string& path) const
{
    CCASSERT(!path.empty(), "Invalid path");

    if (isDirectoryExist(path))
        return true;

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string subpath;
    std::vector<std::string> dirs;

    if (found != std::string::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found+1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    DIR *dir = NULL;

    // Create path recursively
    subpath = "";
    for (const auto& iter : dirs)
    {
        subpath += iter;
        dir = opendir(subpath.c_str());

        if (!dir)
        {
            // directory doesn't exist, should create a new one

            int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
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
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
    {
        int rv = remove(fpath);
        
        if (rv)
            perror(fpath);
        
        return rv;
    }
#endif
}

bool FileUtils::removeDirectory(const std::string& path) const
{
#if !defined(CC_TARGET_OS_TVOS)

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    if (nftw(path.c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS) == -1)
        return false;
    else
        return true;
#else
    std::string command = "rm -r ";
    // Path may include space.
    command += "\"" + path + "\"";
    if (system(command.c_str()) >= 0)
        return true;
    else
        return false;
#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)

#else
    return false;
#endif // !defined(CC_TARGET_OS_TVOS)
}

bool FileUtils::removeFile(const std::string &path) const
{
    if (remove(path.c_str())) {
        return false;
    } else {
        return true;
    }
}

bool FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath) const
{
    CCASSERT(!oldfullpath.empty(), "Invalid path");
    CCASSERT(!newfullpath.empty(), "Invalid path");

    int errorCode = rename(oldfullpath.c_str(), newfullpath.c_str());

    if (0 != errorCode)
    {
        CCLOGERROR("Fail to rename file %s to %s !Error code is %d", oldfullpath.c_str(), newfullpath.c_str(), errorCode);
        return false;
    }
    return true;
}

bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name) const
{
    CCASSERT(!path.empty(), "Invalid path");
    std::string oldPath = path + oldname;
    std::string newPath = path + name;

    return this->renameFile(oldPath, newPath);
}

int64_t FileUtils::getFileSize(const std::string &filepath) const
{
    CCASSERT(!filepath.empty(), "Invalid path");

    std::string fullpath = filepath;
    if (!isAbsolutePath(filepath))
    {
        fullpath = fullPathForFilename(filepath);
        if (fullpath.empty())
            return 0;
    }

    struct stat info;
    // Get data associated with "crt_stat.c":
    int result = ::stat(fullpath.c_str(), &info);

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

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) const
{
    std::vector<std::string> files;
    std::string fullpath = fullPathForDirectory(dirPath);
    if (!fullpath.empty() && isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
        std::string fullpathstr = fullpath;

        if (tinydir_open(&dir, &fullpathstr[0]) != -1)
        {
            while (dir.has_next)
            {
                tinydir_file file;
                if (tinydir_readfile(&dir, &file) == -1)
                {
                    // Error getting file
                    break;
                }
                std::string filepath = file.path;

                if(strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
                {
                    if (file.is_dir)
                        filepath.push_back('/');

                    files.push_back(std::move(filepath));
                }

                if (tinydir_next(&dir) == -1)
                {
                    // Error getting next file
                    break;
                }
            }
        }
        tinydir_close(&dir);
    }
    return files;
}

void FileUtils::listFilesRecursively(const std::string& dirPath, std::vector<std::string> *files) const
{
    std::string fullpath = fullPathForDirectory(dirPath);
    if (isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
        std::string fullpathstr = fullpath;

        if (tinydir_open(&dir, &fullpathstr[0]) != -1)
        {
            while (dir.has_next)
            {
                tinydir_file file;
                if (tinydir_readfile(&dir, &file) == -1)
                {
                    // Error getting file
                    break;
                }
                
                if(strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
                {
                    std::string filepath = file.path;
                    if (file.is_dir)
                    {
                        filepath.push_back('/');
                        files->push_back(filepath);
                        listFilesRecursively(filepath, files);
                    }
                    else
                    {
                        files->push_back(std::move(filepath));
                    }
                }

                if (tinydir_next(&dir) == -1)
                {
                    // Error getting next file
                    break;
                }
            }
        }
        tinydir_close(&dir);
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

std::string FileUtils::getFileExtension(const std::string& filePath) const
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

std::string FileUtils::getFileShortName(const std::string& filePath)
{
    std::string fileExtension;
    size_t pos = filePath.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return filePath.substr(pos + 1);
    }

    return filePath;
}

void FileUtils::valueMapCompact(ValueMap& /*valueMap*/) const
{
}

void FileUtils::valueVectorCompact(ValueVector& /*valueVector*/) const
{
}

NS_CC_END
