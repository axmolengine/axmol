/****************************************************************************
 Copyright (c) 2015-2016 cocos2d-x.org
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

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <vector>

#include "platform/PlatformMacros.h"

NS_AX_BEGIN

namespace network
{

class IDownloadTask;
class IDownloaderImpl;
class Downloader;
class DownloaderCURL;

class AX_DLL DownloadTask final
{
public:
    const static int ERROR_NO_ERROR            = 0;
    const static int ERROR_INVALID_PARAMS      = -1;
    const static int ERROR_OPEN_FILE_FAILED    = -2;
    const static int ERROR_IMPL_INTERNAL       = -3;
    const static int ERROR_TASK_DUPLICATED     = -4;
    const static int ERROR_CREATE_DIR_FAILED   = -5;
    const static int ERROR_REMOVE_FILE_FAILED  = -6;
    const static int ERROR_RENAME_FILE_FAILED  = -7;
    const static int ERROR_CHECK_SUM_FAILED    = -8;
    const static int ERROR_ORIGIN_FILE_MISSING = -9;

    std::string identifier;
    std::string requestURL;
    std::string storagePath;
    
    std::string cacertPath;

    struct
    {
        // progress
        int64_t totalBytesExpected = 0;
        int64_t bytesReceived      = 0;
        int64_t totalBytesReceived = 0;
        // speed
        double speedInBytes = 0;
    } mutable progressInfo;

    DownloadTask();
    DownloadTask(std::string_view srcUrl, std::string_view identifier);
    DownloadTask(std::string_view srcUrl,
                 std::string_view storagePath,
                 std::string_view checksum,  // currently is MD5
                 std::string_view identifier,
                 bool background,
                 std::string_view cacertPath);

    virtual ~DownloadTask();

    // Cancel the download, it's useful for ios platform switch wifi to 4g
    void cancel();

    std::string checksum;  // The MD5 checksum for check only when download finished.
    bool background;       // Does the task is background (all callback will invoke on downloader thread)

private:
    friend class Downloader;
    friend class DownloaderCURL;
    std::unique_ptr<IDownloadTask> _coTask;
};

class AX_DLL DownloaderHints
{
public:
    uint32_t countOfMaxProcessingTasks;
    uint32_t timeoutInSeconds;
    std::string tempFileNameSuffix;
};

class AX_DLL Downloader final
{
public:
    Downloader();
    Downloader(const DownloaderHints& hints);
    ~Downloader();

    std::function<void(const DownloadTask& task, std::vector<unsigned char>& data)> onDataTaskSuccess;

    std::function<void(const DownloadTask& task)> onFileTaskSuccess;

    std::function<void(const DownloadTask& task)> onTaskProgress;

    std::function<void(const DownloadTask& task, int errorCode, int errorCodeInternal, std::string_view errorStr)>
        onTaskError;

    void setOnFileTaskSuccess(const std::function<void(const DownloadTask& task)>& callback)
    {
        onFileTaskSuccess = callback;
    };

    void setOnTaskProgress(const std::function<void(const DownloadTask& task)>& callback)
    {
        onTaskProgress = callback;
    };

    void setOnTaskError(
        const std::function<
            void(const DownloadTask& task, int errorCode, int errorCodeInternal, std::string_view errorStr)>& callback)
    {
        onTaskError = callback;
    };

    std::shared_ptr<DownloadTask> createDownloadDataTask(std::string_view srcUrl, std::string_view identifier = "");

    std::shared_ptr<DownloadTask> createDownloadFileTask(std::string_view srcUrl,
                                                         std::string_view storagePath,
                                                         std::string_view identifier = "",
                                                         std::string_view checksum   = "",
                                                         bool background             = false,
                                                         std::string_view cacertPath = "");

private:
    std::unique_ptr<IDownloaderImpl> _impl;
};

}  // namespace network
NS_AX_END  // namespace ax
