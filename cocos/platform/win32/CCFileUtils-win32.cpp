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
#include "platform/win32/CCFileUtils-win32.h"
#include "platform/CCCommon.h"
#include "tinydir/tinydir.h"
#include <Shlobj.h>
#include <cstdlib>
#include <regex>
#include <sstream>

#include <sys/types.h>  
#include <sys/stat.h>  

#include "win32-specific/ntcvt/ntcvt.hpp"

using namespace std;

#define DECLARE_GUARD (void)0 // std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)

NS_CC_BEGIN

#define CC_MAX_PATH  512

// The root path of resources, the character encoding is UTF-8.
// UTF-8 is the only encoding supported by cocos2d-x API.
static std::string s_resourcePath = "";

// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
static inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    int len = ret.length();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

static void _checkPath()
{
    if (s_resourcePath.empty())
    {
        WCHAR utf16Path[CC_MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, utf16Path, CC_MAX_PATH - 1);
        WCHAR *pUtf16ExePath = &(utf16Path[0]);

        // We need only directory part without exe
        WCHAR *pUtf16DirEnd = wcsrchr(pUtf16ExePath, L'\\');

        auto utf8ExeDir = ntcvt::wcbs2a<std::string>(pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1);

        s_resourcePath = convertPathFormatToUnixStyle(utf8ExeDir);
    }
}

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsWin32();
        if(!s_sharedFileUtils->init())
        {
          delete s_sharedFileUtils;
          s_sharedFileUtils = nullptr;
          CCLOG("ERROR: Could not init CCFileUtilsWin32");
        }
    }
    return s_sharedFileUtils;
}

FileUtilsWin32::FileUtilsWin32()
{
}

bool FileUtilsWin32::init()
{
    DECLARE_GUARD;
    _checkPath();
    _defaultResRootPath = s_resourcePath + "Resources/";
    return FileUtils::init();
}

bool FileUtilsWin32::isDirectoryExistInternal(const std::string& dirPath) const
{
    unsigned long fAttrib = GetFileAttributesW(ntcvt::from_chars(dirPath).c_str());
    return (fAttrib != INVALID_FILE_ATTRIBUTES &&
        (fAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

long FileUtilsWin32::getFileSize(const std::string &filepath)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &fad))
    {
        return 0; // error condition, could call GetLastError to find out more
    }
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return (long)size.QuadPart;
}

bool FileUtilsWin32::isFileExistInternal(const std::string& strFilePath) const
{
    DECLARE_GUARD;
    if (strFilePath.empty())
    {
        return false;
    }

    std::string strPath = strFilePath;
    if (!isAbsolutePath(strPath))
    { // Not absolute path, add the default root path at the beginning.
        strPath.insert(0, _defaultResRootPath);
    }

    DWORD attr = GetFileAttributesW(ntcvt::from_chars(strPath).c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileUtilsWin32::isAbsolutePath(const std::string& strPath) const
{
    if (   (strPath.length() > 2
        && ( (strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z') )
        && strPath[1] == ':') || (strPath[0] == '/' && strPath[1] == '/'))
    {
        return true;
    }
    return false;
}


FileUtils::Status FileUtilsWin32::getContents(const std::string& filename, ResizableBuffer* buffer) const
{
    if (filename.empty())
        return FileUtils::Status::NotExists;

    // read the file from hardware
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);

    HANDLE fileHandle = ::CreateFileW(ntcvt::from_chars(fullPath).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return FileUtils::Status::OpenFailed;

	DWORD hi;
    auto size = ::GetFileSize(fileHandle, &hi);
	if (hi > 0)
	{
		::CloseHandle(fileHandle);
		return FileUtils::Status::TooLarge;
	}
    // don't read file content if it is empty
    if (size == 0)
    {
        ::CloseHandle(fileHandle);
        return FileUtils::Status::OK;
    }

    buffer->resize(size);
    DWORD sizeRead = 0;
    BOOL successed = ::ReadFile(fileHandle, buffer->buffer(), size, &sizeRead, nullptr);
    ::CloseHandle(fileHandle);

    if (!successed) {
		CCLOG("Get data from file(%s) failed, error code is %s", filename.data(), std::to_string(::GetLastError()).data());
		buffer->resize(sizeRead);
		return FileUtils::Status::ReadFailed;
    }
    return FileUtils::Status::OK;
}

std::string FileUtilsWin32::getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath) const
{
    std::string unixFileName = convertPathFormatToUnixStyle(filename);
    std::string unixResolutionDirectory = convertPathFormatToUnixStyle(resolutionDirectory);
    std::string unixSearchPath = convertPathFormatToUnixStyle(searchPath);

    return FileUtils::getPathForFilename(unixFileName, unixResolutionDirectory, unixSearchPath);
}

std::string FileUtilsWin32::getFullPathForFilenameWithinDirectory(const std::string& strDirectory, const std::string& strFilename) const
{
    std::string unixDirectory = convertPathFormatToUnixStyle(strDirectory);
    std::string unixFilename = convertPathFormatToUnixStyle(strFilename);

    return FileUtils::getFullPathForFilenameWithinDirectory(unixDirectory, unixFilename);
}

void FileUtilsWin32::listFilesRecursively(const std::string& dirPath, std::vector<std::string> *files) const
{
    std::string fullpath = fullPathForFilename(dirPath);
    if (isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
        std::wstring fullpathstr = ntcvt::from_chars(fullpath);

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
                std::string fileName = ntcvt::from_chars(file.name);

                if (fileName != "." && fileName != "..")
                {
                    std::string filepath = ntcvt::from_chars(file.path);
                    if (file.is_dir)
                    {
                        filepath.push_back('/');
                        files->push_back(filepath);
                        listFilesRecursively(filepath, files);
                    }
                    else
                    {
                        files->push_back(filepath);
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

int64_t FileUtilsWin32::getFileSize(const std::string &filepath) const
{
    if (filepath.empty())
        return -1;
    WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
    if (GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &attrs) &&
        !(attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        return static_cast<long long>(attrs.nFileSizeHigh) << 32 |
        static_cast<unsigned long long>(attrs.nFileSizeLow);
    return -1;
}

std::vector<std::string> FileUtilsWin32::listFiles(const std::string& dirPath) const
{
    std::string fullpath = fullPathForDirectory(dirPath);
    std::vector<std::string> files;
    if (isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
        std::wstring fullpathstr = ntcvt::from_chars(fullpath);

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

                std::string fileName = ntcvt::from_chars(file.name);
                if (fileName != "." && fileName != "..")
                {
                    std::string filepath = ntcvt::from_chars(file.path);
                    if (file.is_dir)
                    {
                        filepath.push_back('/');
                    }
                    files.push_back(filepath);
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

string FileUtilsWin32::getWritablePath() const
{
    DECLARE_GUARD;
    if (_writablePath.length())
    {
        return _writablePath;
    }

    // Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
    WCHAR full_path[CC_MAX_PATH + 1] = { 0 };
    ::GetModuleFileNameW(nullptr, full_path, CC_MAX_PATH + 1);

    // Debug app uses executable directory; Non-debug app uses local app data directory
//#ifndef _DEBUG
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR *base_name = wcsrchr(full_path, '\\');
    wstring retPath;
    if(base_name)
    {
        WCHAR app_data_path[CC_MAX_PATH + 1];

        // Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
        {
            wstring ret(app_data_path);

            // Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
            ret += base_name;

            // Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
            ret = ret.substr(0, ret.rfind(L"."));

            ret += L"\\";

            // Create directory
            if (SUCCEEDED(SHCreateDirectoryExW(nullptr, ret.c_str(), nullptr)))
            {
                retPath = ret;
            }
        }
    }
    if (retPath.empty())
//#endif // not defined _DEBUG
    {
        // If fetching of local app data directory fails, use the executable one
        retPath = full_path;

        // remove xxx.exe
        retPath = retPath.substr(0, retPath.rfind(L"\\") + 1);
    }

    return convertPathFormatToUnixStyle(ntcvt::from_chars(retPath));
}

bool FileUtilsWin32::renameFile(const std::string &oldfullpath, const std::string& newfullpath) const
{
    CCASSERT(!oldfullpath.empty(), "Invalid path");
    CCASSERT(!newfullpath.empty(), "Invalid path");

    std::wstring _wNew = ntcvt::from_chars(newfullpath);
    std::wstring _wOld = ntcvt::from_chars(oldfullpath);

    if (FileUtils::getInstance()->isFileExist(newfullpath))
    {
        if (!DeleteFile(_wNew.c_str()))
        {
            CCLOGERROR("Fail to delete file %s !Error code is 0x%x", newfullpath.c_str(), GetLastError());
        }
    }

    if (MoveFileW(_wOld.c_str(), _wNew.c_str()))
    {
        return true;
    }
    else
    {
        CCLOGERROR("Fail to rename file %s to %s !Error code is 0x%x", oldfullpath.c_str(), newfullpath.c_str(), GetLastError());
        return false;
    }
}

bool FileUtilsWin32::renameFile(const std::string &path, const std::string &oldname, const std::string &name) const
{
    CCASSERT(!path.empty(), "Invalid path");
    std::string oldPath = path + oldname;
    std::string newPath = path + name;

    std::regex pat("\\/");
    std::string _old = std::regex_replace(oldPath, pat, "\\");
    std::string _new = std::regex_replace(newPath, pat, "\\");

    return renameFile(_old, _new);
}

bool FileUtilsWin32::createDirectory(const std::string& dirPath) const
{
    CCASSERT(!dirPath.empty(), "Invalid path");

    if (isDirectoryExist(dirPath))
        return true;

    std::wstring path = ntcvt::from_chars(dirPath);

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of(L"/\\", start);
    std::wstring subpath;
    std::vector<std::wstring> dirs;

    if (found != std::wstring::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found + 1;
            found = path.find_first_of(L"/\\", start);
            if (found == std::wstring::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    if ((GetFileAttributesW(path.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        subpath = L"";
        for (unsigned int i = 0, size = dirs.size(); i < size; ++i)
        {
            subpath += dirs[i];

            std::string utf8Path = ntcvt::from_chars(subpath);
            if (!isDirectoryExist(utf8Path))
            {
                BOOL ret = CreateDirectoryW(subpath.c_str(), NULL);
                if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
                {
                    CCLOGERROR("Fail create directory %s !Error code is 0x%x", utf8Path.c_str(), GetLastError());
                    return false;
                }
            }
        }
    }
    return true;
}

bool FileUtilsWin32::removeFile(const std::string &filepath) const
{
    std::regex pat("\\/");
    std::string win32path = std::regex_replace(filepath, pat, "\\");

    if (DeleteFileW(ntcvt::from_chars(win32path).c_str()))
    {
        return true;
    }
    else
    {
        CCLOGERROR("Fail remove file %s !Error code is 0x%x", filepath.c_str(), GetLastError());
        return false;
    }
}

bool FileUtilsWin32::removeDirectory(const std::string& dirPath) const
{
    std::string dirPathCopy = dirPath;
    if (dirPath.length() > 0 && dirPath[dirPath.length() - 1] != '/' && dirPath[dirPath.length() - 1] != '\\')
    {
        dirPathCopy.push_back('/');
    }
    std::wstring wpath = ntcvt::from_chars(dirPathCopy);
    std::wstring files = wpath + L"*.*";
    WIN32_FIND_DATA wfd;
    HANDLE  search = FindFirstFileEx(files.c_str(), FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
    bool ret = true;
    if (search != INVALID_HANDLE_VALUE)
    {
        BOOL find = true;
        while (find)
        {
            // Need check string . and .. for delete folders and files begin name.
            std::wstring fileName = wfd.cFileName;
            if (fileName != L"." && fileName != L"..")
            {
                std::wstring temp = wpath + wfd.cFileName;
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    temp += '/';
                    ret = ret && this->removeDirectory(ntcvt::from_chars(temp));
                }
                else
                {
                    SetFileAttributesW(temp.c_str(), FILE_ATTRIBUTE_NORMAL);
                    ret = ret && DeleteFileW(temp.c_str());
                }
            }
            find = FindNextFile(search, &wfd);
        }
        FindClose(search);
    }
    if (ret && RemoveDirectoryW(wpath.c_str()))
    {
        return true;
    }
    return false;
}

NS_CC_END
