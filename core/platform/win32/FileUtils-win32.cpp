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
#include "platform/win32/FileUtils-win32.h"
#include "platform/Common.h"
#include <Shlobj.h>
#include <cstdlib>
#include <regex>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

#include "ntcvt/ntcvt.hpp"

#define DECLARE_GUARD (void)0  // std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)

NS_AX_BEGIN

#define AX_MAX_PATH 512

// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
static std::string convertPathFormatToUnixStyle(const std::string_view& path)
{
    std::string ret{path};
    std::replace(ret.begin(), ret.end(), '\\', '/');
    return ret;
}

static std::string convertPathFormatToWinStyle(const std::string_view& path)
{
    std::string ret{path};
    std::replace(ret.begin(), ret.end(), '/', '\\');
    return ret;
}

static std::string _checkWorkingPath()
{
    WCHAR utf16Path[AX_MAX_PATH + 2] = {0};
    size_t nNum                      = GetCurrentDirectoryW(AX_MAX_PATH, utf16Path);
    utf16Path[nNum++]                = '\\';
    auto workingDir                     = ntcvt::from_chars(std::wstring_view{utf16Path, static_cast<size_t>(nNum)});
    std::replace(workingDir.begin(), workingDir.end(), '\\', '/');
    return workingDir;
}

static std::string _checkExePath()
{
    WCHAR utf16Path[AX_MAX_PATH + 1] = {0};
    size_t nNum                      = GetModuleFileNameW(NULL, utf16Path, AX_MAX_PATH);
    std::wstring_view u16pathsv{utf16Path, nNum};
    u16pathsv.remove_suffix(u16pathsv.length() - u16pathsv.find_last_of('\\') - 1);
    auto u8path = ntcvt::from_chars(u16pathsv);
    std::replace(u8path.begin(), u8path.end(), '\\', '/');
    return u8path;
}

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsWin32();
        if (!s_sharedFileUtils->init())
        {
            delete s_sharedFileUtils;
            s_sharedFileUtils = nullptr;
            AXLOGE("ERROR: Could not init FileUtilsWin32");
        }
    }
    return s_sharedFileUtils;
}

FileUtilsWin32::FileUtilsWin32() {}

bool FileUtilsWin32::init()
{
    if(s_exeDir.empty()) {
        s_exeDir = _checkExePath();
    }

    auto workingDir = _checkWorkingPath();
    bool startedFromSelfLocation = workingDir == s_exeDir;
    if (!startedFromSelfLocation || !isDirectoryExistInternal(AX_CONTENT_DIR))
        _defaultResRootPath = std::move(workingDir);
    else
    {
        _defaultResRootPath.reserve(s_exeDir.size() + AX_CONTENT_DIR_LEN);
        _defaultResRootPath.append(s_exeDir).append(AX_CONTENT_DIR, AX_CONTENT_DIR_LEN);
    }

    bool bRet = FileUtils::init();

    // make sure any path relative to exe dir can be found when app working directory location not exe path
    if (!startedFromSelfLocation)
        addSearchPath(s_exeDir);

    return bRet;
}

bool FileUtilsWin32::isDirectoryExistInternal(std::string_view dirPath) const
{
    uint32_t fAttrib = GetFileAttributesW(ntcvt::from_chars(dirPath).c_str());
    return (fAttrib != INVALID_FILE_ATTRIBUTES && (fAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileUtilsWin32::isFileExistInternal(std::string_view strFilePath) const
{
    DWORD attr = GetFileAttributesW(ntcvt::from_chars(strFilePath).c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

std::string FileUtilsWin32::getPathForFilename(std::string_view filename,
                                               std::string_view searchPath) const
{
    std::string unixFileName            = convertPathFormatToUnixStyle(filename);
    std::string unixSearchPath          = convertPathFormatToUnixStyle(searchPath);

    return FileUtils::getPathForFilename(unixFileName, unixSearchPath);
}

std::string FileUtilsWin32::getFullPathForFilenameWithinDirectory(std::string_view strDirectory,
                                                                  std::string_view strFilename) const
{
    std::string unixDirectory = convertPathFormatToUnixStyle(strDirectory);
    std::string unixFilename  = convertPathFormatToUnixStyle(strFilename);

    return FileUtils::getFullPathForFilenameWithinDirectory(unixDirectory, unixFilename);
}

int64_t FileUtilsWin32::getFileSize(std::string_view filepath) const
{
    if (filepath.empty())
        return -1;
    WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
    if (GetFileAttributesExW(ntcvt::from_chars(filepath).c_str(), GetFileExInfoStandard, &attrs) &&
        !(attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        return static_cast<long long>(attrs.nFileSizeHigh) << 32 | static_cast<unsigned long long>(attrs.nFileSizeLow);
    return -1;
}

std::string FileUtilsWin32::getWritablePath() const
{
    DECLARE_GUARD;
    return getNativeWritableAbsolutePath();
}

std::string FileUtilsWin32::getNativeWritableAbsolutePath() const
{
    DECLARE_GUARD;
    if (_writablePath.length())
    {
        return _writablePath;
    }

    // Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
    WCHAR full_path[AX_MAX_PATH + 1] = {0};
    ::GetModuleFileNameW(nullptr, full_path, AX_MAX_PATH + 1);

    // Debug app uses executable directory; Non-debug app uses local app data directory
    //#ifndef _DEBUG
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR* base_name = wcsrchr(full_path, '\\');
    std::wstring retPath;
    if (base_name)
    {
        WCHAR app_data_path[AX_MAX_PATH + 1];

        // Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
        {
            std::wstring ret(app_data_path);

            // Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application
            // Data\MyGame.exe
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

bool FileUtilsWin32::renameFile(std::string_view oldfullpath, std::string_view newfullpath) const
{
    AXASSERT(!oldfullpath.empty(), "Invalid path");
    AXASSERT(!newfullpath.empty(), "Invalid path");

    std::wstring _wNew = ntcvt::from_chars(newfullpath);
    std::wstring _wOld = ntcvt::from_chars(oldfullpath);

    if (FileUtils::getInstance()->isFileExist(newfullpath))
    {
        if (!DeleteFile(_wNew.c_str()))
        {
            AXLOGE("Fail to delete file {} !Error code is 0x{:x}", newfullpath, GetLastError());
        }
    }

    if (MoveFileW(_wOld.c_str(), _wNew.c_str()))
    {
        return true;
    }
    else
    {
        AXLOGE("Fail to rename file {} to {} !Error code is 0x{:x}", oldfullpath, newfullpath,
                   GetLastError());
        return false;
    }
}

bool FileUtilsWin32::renameFile(std::string_view path, std::string_view oldname, std::string_view name) const
{
    AXASSERT(!path.empty(), "Invalid path");
    std::string oldPath{path};
    oldPath += oldname;
    std::string newPath{path};
    newPath += name;

    std::regex pat("\\/");
    std::string _old = std::regex_replace(oldPath, pat, "\\");
    std::string _new = std::regex_replace(newPath, pat, "\\");

    return renameFile(_old, _new);
}

bool FileUtilsWin32::createDirectories(std::string_view dirPath) const
{
    AXASSERT(!dirPath.empty(), "Invalid path");

    if (isDirectoryExist(dirPath))
        return true;

    std::wstring path = ntcvt::from_chars(dirPath);

    bool fail = false;
    if ((GetFileAttributesW(path.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        axstd::splitpath_cb(
            &path.front(), [](wchar_t* ptr) { return *ptr != '\0'; },
            [&dirPath, &fail](const wchar_t* subpath) {
            auto attribs = GetFileAttributesW(subpath);
            if (attribs == INVALID_FILE_ATTRIBUTES)
            {
                BOOL ret = CreateDirectoryW(subpath, NULL);
                if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
                {
                    fail = true;
                    AXLOGE("Fail create directory {} !Error code is 0x{:x}", dirPath, GetLastError());
                }
            }
            else
                fail = !(attribs & FILE_ATTRIBUTE_DIRECTORY);

            return fail;
        });
    }
    return !fail;
}

bool FileUtilsWin32::removeFile(std::string_view filepath) const
{
    std::string win32path = convertPathFormatToWinStyle(filepath);

    if (DeleteFileW(ntcvt::from_chars(win32path).c_str()))
    {
        return true;
    }
    else
    {
        AXLOGE("Fail remove file {} !Error code is 0x{:x}", filepath, GetLastError());
        return false;
    }
}

bool FileUtilsWin32::removeDirectory(std::string_view dirPath) const
{
    std::string dirPathCopy{dirPath};
    if (!dirPath.empty() && dirPath[dirPath.length() - 1] != '/' && dirPath[dirPath.length() - 1] != '\\')
    {
        dirPathCopy.push_back('/');
    }
    std::wstring wpath = ntcvt::from_chars(dirPathCopy);
    std::wstring files = wpath + L"*.*";
    WIN32_FIND_DATA wfd;
    HANDLE search = FindFirstFileEx(files.c_str(), FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
    bool ret      = true;
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

NS_AX_END
