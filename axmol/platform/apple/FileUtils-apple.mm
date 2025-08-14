/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#import <Foundation/Foundation.h>

#include "platform/apple/FileUtils-apple.h"

#include <ftw.h>

#include <string>
#include <stack>

#include "base/Director.h"
#include "platform/FileUtils.h"

namespace ax
{

struct FileUtilsApple::IMPL
{
    IMPL(NSBundle* bundle) : bundle_([NSBundle mainBundle]) {}
    void setBundle(NSBundle* bundle) { bundle_ = bundle; }
    NSBundle* getBundle() const { return bundle_; }

private:
    NSBundle* bundle_;
};

static std::string appendTrailingSlashToDir(std::string_view dir)
{
    auto result = std::string(dir);
    if (not result.empty() and result.back() != '/')
        result.push_back('/');
    return result;
}

FileUtilsApple::FileUtilsApple() : pimpl_(new IMPL([NSBundle mainBundle])) {}

FileUtilsApple::~FileUtilsApple() = default;

bool FileUtilsApple::init()
{
    _defaultResRootPath = appendTrailingSlashToDir([[[NSBundle mainBundle] resourcePath] UTF8String]);

    bool ret = FileUtils::init();
    return ret;
}

#if AX_FILEUTILS_APPLE_ENABLE_OBJC
void FileUtilsApple::setBundle(NSBundle* bundle)
{
    pimpl_->setBundle(bundle);
}
#endif

#pragma mark - FileUtils

static NSFileManager* s_fileManager = [NSFileManager defaultManager];

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsApple();
        if (!s_sharedFileUtils->init())
        {
            delete s_sharedFileUtils;
            s_sharedFileUtils = nullptr;
            AXLOGE("ERROR: Could not init FileUtilsApple");
        }
    }
    return s_sharedFileUtils;
}

std::string FileUtilsApple::getWritablePath() const
{
    return getNativeWritableAbsolutePath();
}

std::string FileUtilsApple::getNativeWritableAbsolutePath() const
{
    if (_writablePath.length())
    {
        return _writablePath;
    }

    // save to document folder
    NSArray* paths               = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    std::string strRet           = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

bool FileUtilsApple::isFileExistInternal(std::string_view filePath) const
{
    if (filePath.empty())
    {
        return false;
    }

    bool ret = false;

    if (filePath[0] != '/')
    {
        std::string path;
        std::string file;
        size_t pos = filePath.find_last_of("/");
        if (pos != std::string::npos)
        {
            file = filePath.substr(pos + 1);
            path = filePath.substr(0, pos + 1);
        }
        else
        {
            file = filePath;
        }

        NSString* fullpath = [pimpl_->getBundle() pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                           ofType:nil
                                                      inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil)
        {
            BOOL isDir = NO;
            if ([s_fileManager fileExistsAtPath:fullpath isDirectory:&isDir] && !isDir)
            {
                ret = true;
            }
        }
    }
    else
    {
        // Search path is an absolute path.
        BOOL isDir = NO;
        if ([s_fileManager fileExistsAtPath:[[NSString alloc] initWithBytes:filePath.data() length:filePath.size() encoding:NSUTF8StringEncoding] isDirectory:&isDir] && !isDir)
        {
            ret = true;
        }
    }

    return ret;
}

static int unlink_cb(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf)
{
    auto ret = remove(fpath);
    if (ret)
    {
        AXLOGE("Fail to remove: {} ", fpath);
    }

    return ret;
}

bool FileUtilsApple::removeDirectory(std::string_view path) const
{
    if (path.empty())
    {
        AXLOGE("Fail to remove directory, path is empty!");
        return false;
    }

    if (nftw(path.data(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS))
        return false;
    else
        return true;
}

std::string FileUtilsApple::getPathForDirectory(std::string_view dir,
                                                std::string_view searchPath) const
{
    std::string path{searchPath};
    path.append(dir);

    if (!path.empty() && path[path.length() - 1] == '/')
    {
        path.erase(path.end() - 1);
    }

    if (path[0] == '/')
    {
        BOOL isDir = NO;
        if ([s_fileManager fileExistsAtPath:[[NSString alloc] initWithBytes:path.data() length:path.size() encoding:NSUTF8StringEncoding] isDirectory:&isDir])
        {
            return appendTrailingSlashToDir(isDir ? path : "");
        }
    }
    else
    {
        NSString* fullpath = [pimpl_->getBundle() pathForResource:[NSString stringWithUTF8String:path.c_str()]
                                                           ofType:nil];
        if (fullpath != nil)
        {
            return appendTrailingSlashToDir([fullpath UTF8String]);
        }
    }
    return "";
}

std::string FileUtilsApple::getFullPathForFilenameWithinDirectory(std::string_view directory,
                                                                  std::string_view filename) const
{
    std::string ret;

    if (filename.empty())
        return ret;

    if (!directory.empty() && directory[0] == '/') {
        size_t pathSize = directory.size() + filename.size();
        if (directory.back() != '/')
            ++pathSize;
        ret.reserve(pathSize);
        ret += directory;
        ret += filename;
    }
    else
    { // Build full path for the file
        NSString* path = nil;
        if (!directory.empty()) {
            path = [pimpl_->getBundle() pathForResource:[[NSString alloc] initWithBytes:filename.data() length:filename.size() encoding:NSUTF8StringEncoding]
                                                       ofType:nil
                                                  inDirectory:[[NSString alloc] initWithBytes:directory.data() length:directory.size() encoding:NSUTF8StringEncoding]];
        } else {
            path = [pimpl_->getBundle() pathForResource:[[NSString alloc] initWithBytes:filename.data() length:filename.size() encoding:NSUTF8StringEncoding]
                                                       ofType:nil];
        }
        if (path != nil)
            ret = [path UTF8String];
    }

    // Check if there's a file at path
    BOOL isDir = NO;
    if (![s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:ret.c_str()] isDirectory:&isDir] || isDir)
    {
        ret.clear();
    }

    return ret;
}

bool FileUtilsApple::createDirectories(std::string_view path) const
{
    AXASSERT(!path.empty(), "Invalid path");

    if (isDirectoryExist(path))
        return true;

    NSError* error;

    bool result = [s_fileManager createDirectoryAtPath:[[NSString alloc] initWithBytes:path.data() length:path.size() encoding:NSUTF8StringEncoding]
                           withIntermediateDirectories:YES
                                            attributes:nil
                                                 error:&error];

    if (!result && error != nil)
    {
        AXLOGE("Fail to create directory \"{}\": {}", path, [error.localizedDescription UTF8String]);
    }

    return result;
}

}
