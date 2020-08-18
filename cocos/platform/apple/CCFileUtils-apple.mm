/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#import <Foundation/Foundation.h>

#include "platform/apple/CCFileUtils-apple.h"

#include <ftw.h>

#include <string>
#include <stack>

#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"

#define DECLARE_GUARD (void)0 // std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)

NS_CC_BEGIN

struct FileUtilsApple::IMPL {
    IMPL(NSBundle* bundle):bundle_([NSBundle mainBundle]) {}
    void setBundle(NSBundle* bundle) {
        bundle_ = bundle;
    }
    NSBundle* getBundle() const {
        return bundle_;
    }
private:
    NSBundle* bundle_;
};

FileUtilsApple::FileUtilsApple() : pimpl_(new IMPL([NSBundle mainBundle])) {
}

FileUtilsApple::~FileUtilsApple() = default;

#if CC_FILEUTILS_APPLE_ENABLE_OBJC
void FileUtilsApple::setBundle(NSBundle* bundle) {
    pimpl_->setBundle(bundle);
}
#endif

#pragma mark - FileUtils

static NSFileManager* s_fileManager = [NSFileManager defaultManager];

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new (std::nothrow) FileUtilsApple();
        if(!s_sharedFileUtils->init())
        {
          delete s_sharedFileUtils;
          s_sharedFileUtils = nullptr;
          CCLOG("ERROR: Could not init CCFileUtilsApple");
        }
    }
    return s_sharedFileUtils;
}


std::string FileUtilsApple::getWritablePath() const
{
    DECLARE_GUARD;
    if (_writablePath.length())
    {
        return _writablePath;
    }

    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

bool FileUtilsApple::isFileExistInternal(const std::string& filePath) const
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
            file = filePath.substr(pos+1);
            path = filePath.substr(0, pos+1);
        }
        else
        {
            file = filePath;
        }

        NSString* fullpath = [pimpl_->getBundle() pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil) {
            ret = true;
        }
    }
    else
    {
        // Search path is an absolute path.
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:filePath.c_str()]]) {
            ret = true;
        }
    }

    return ret;
}

static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    auto ret = remove(fpath);
    if (ret)
    {
        log("Fail to remove: %s ",fpath);
    }

    return ret;
}

bool FileUtilsApple::removeDirectory(const std::string& path) const
{
    if (path.empty())
    {
        CCLOGERROR("Fail to remove directory, path is empty!");
        return false;
    }

    if (nftw(path.c_str(),unlink_cb, 64, FTW_DEPTH | FTW_PHYS))
        return false;
    else
        return true;
}

std::string FileUtilsApple::getPathForDirectory(const std::string &dir, const std::string &resolutionDiretory, const std::string &searchPath) const
{
    auto path = searchPath + resolutionDiretory + dir;
    
    if(!path.empty() && path[path.length() -1] == '/') {
        path.erase(path.end() - 1);
    }
    
    if(path[0] == '/')
    {
        BOOL isDir = false;
        if([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:dir.c_str()]
                               isDirectory:&isDir]) {
            return isDir ? path : "";
        }
    }
    else
    {
        NSString *fullpath = [pimpl_->getBundle() pathForResource:[NSString stringWithUTF8String:path.c_str()]
                                                           ofType:nil];
        if(fullpath != nil) {
            return [fullpath UTF8String];
        }
    }
    return "";
}


std::string FileUtilsApple::getFullPathForFilenameWithinDirectory(const std::string& directory, const std::string& filename) const
{
    if (directory[0] != '/')
    {
        NSString* fullpath = [pimpl_->getBundle() pathForResource:[NSString stringWithUTF8String:filename.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:directory.c_str()]];
        if (fullpath != nil) {
            return [fullpath UTF8String];
        }
    }
    else
    {
        std::string fullPath = directory+filename;
        // Search path is an absolute path.
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:fullPath.c_str()]]) {
            return fullPath;
        }
    }
    return "";
}

bool FileUtilsApple::createDirectory(const std::string& path) const
{
    CCASSERT(!path.empty(), "Invalid path");
    
    if (isDirectoryExist(path))
        return true;
    
    NSError* error;
    
    bool result = [s_fileManager createDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] withIntermediateDirectories:YES attributes:nil error:&error];
    
    if(!result && error != nil)
    {
        CCLOGERROR("Fail to create directory \"%s\": %s", path.c_str(), [error.localizedDescription UTF8String]);
    }
    
    return result;
}

NS_CC_END
