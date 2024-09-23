/****************************************************************************
Copyright (c) 2011      Laschweinski
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
#include "platform/linux/FileUtils-linux.h"
#include "platform/linux/Application-linux.h"
#include "platform/Common.h"
#include "base/Macros.h"
#include "base/UTF8.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

namespace ax
{

static std::string _checkPath(const char* path) {
    std::string ret;
    ret.resize(PATH_MAX - 1);
    int n = readlink(path, &ret.front(), PATH_MAX);
    if (n > 0) {
        ret.resize(n);
        return ret;
    }
    return std::string{};
}

std::string FileUtilsLinux::s_exeName;

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsLinux();
        if (!s_sharedFileUtils->init())
        {
            delete s_sharedFileUtils;
            s_sharedFileUtils = nullptr;
            AXLOGE("ERROR: Could not init FileUtilsLinux");
        }
    }
    return s_sharedFileUtils;
}

FileUtilsLinux::FileUtilsLinux() {}

bool FileUtilsLinux::init()
{
    // application path
    if (s_exeDir.empty()) {
        s_exeDir = _checkPath("/proc/self/exe");
        auto slash = s_exeDir.find_last_of('/');
        assert(slash != std::string::npos);
        s_exeName = s_exeDir.substr(slash + 1);
        s_exeDir.resize(slash + 1);
    }

    std::string workingDir = _checkPath("/proc/self/cwd");
    workingDir += '/';
    bool startedFromSelfLocation = workingDir == s_exeDir;
    if (!startedFromSelfLocation || !isDirectoryExistInternal(AX_CONTENT_DIR))
        _defaultResRootPath = workingDir;
    else
    {
        _defaultResRootPath.reserve(s_exeDir.size() + AX_CONTENT_DIR_LEN);
        _defaultResRootPath.append(s_exeDir).append(AX_CONTENT_DIR, AX_CONTENT_DIR_LEN);
    }

    // Set writable path to $XDG_CONFIG_HOME or ~/.config/<app name>/ if $XDG_CONFIG_HOME not exists.
    const char* xdg_config_path = getenv("XDG_CONFIG_HOME");
    std::string xdgConfigPath;
    if (xdg_config_path == NULL)
    {
        xdgConfigPath = getenv("HOME");
        xdgConfigPath += "/.config";
    }
    else
    {
        xdgConfigPath = xdg_config_path;
    }
    _writablePath = xdgConfigPath;
    _writablePath += "/";
    _writablePath += s_exeName;
    _writablePath += "/";

    bool ret = FileUtils::init();

    // make sure any path relative to exe dir can be found when app working directory location not exe path
    if (!startedFromSelfLocation)
        addSearchPath(s_exeDir);

    return ret;
}

string FileUtilsLinux::getWritablePath() const
{
    return getNativeWritableAbsolutePath();
}

std::string FileUtilsLinux::getNativeWritableAbsolutePath() const
{
    struct stat st;
    stat(_writablePath.c_str(), &st);
    if (!S_ISDIR(st.st_mode))
    {
        mkdir(_writablePath.c_str(), 0744);
    }

    return _writablePath;
}

bool FileUtilsLinux::isFileExistInternal(std::string_view path) const
{
    struct stat sts;
    return (stat(path.data(), &sts) == 0) && S_ISREG(sts.st_mode);
}

}
