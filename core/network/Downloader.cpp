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

#include "network/Downloader.h"

#if EMSCRIPTEN
#    include "network/Downloader-wasm.h"
#    define DownloaderImpl DownloaderEmscripten
#else
#    include "network/Downloader-curl.h"
#    define DownloaderImpl DownloaderCURL
#endif

#include <ctype.h>
#include <algorithm>

namespace ax
{

namespace network
{

DownloadTask::DownloadTask()
{
    AXLOGD("Construct DownloadTask {}", fmt::ptr(this));
}

DownloadTask::DownloadTask(std::string_view srcUrl, std::string_view identifier)
{
    this->requestURL = srcUrl;
    this->identifier = identifier;
    this->background = false;
}

DownloadTask::DownloadTask(std::string_view srcUrl,
                           std::string_view storagePath,
                           std::string_view checksum,
                           std::string_view identifier,
                           bool background,
                           std::string_view cacertPath)
{
    this->requestURL  = srcUrl;
    this->storagePath = storagePath;
    this->checksum    = checksum;
    this->identifier  = identifier;
    this->background  = background;
    this->cacertPath  = cacertPath;
    if (!this->checksum.empty())
        std::transform(this->checksum.begin(), this->checksum.end(), this->checksum.begin(), ::tolower);
}

DownloadTask::~DownloadTask()
{
    AXLOGD("Destruct DownloadTask {}", fmt::ptr(this));
}

void DownloadTask::cancel()
{
    if (_coTask)
        _coTask->cancel();
}

////////////////////////////////////////////////////////////////////////////////
//  Implement Downloader
Downloader::Downloader() : Downloader(DownloaderHints{6, 45, ".tmp"}) {}

Downloader::Downloader(const DownloaderHints& hints)
{
    AXLOGD("Construct Downloader {}", fmt::ptr(this));
    _impl.reset(new DownloaderImpl(hints));
    _impl->onTaskProgress = [this](const DownloadTask& task) {
        if (onTaskProgress)
        {
            onTaskProgress(task);
        }
    };

    _impl->onTaskFinish = [this](const DownloadTask& task, int errorCode, int errorCodeInternal,
                                 std::string_view errorStr, std::vector<unsigned char>& data) {
        if (DownloadTask::ERROR_NO_ERROR != errorCode)
        {
            if (onTaskError)
            {
                onTaskError(task, errorCode, errorCodeInternal, errorStr);
            }
            return;
        }

        // success callback
        if (task.storagePath.length())
        {
            if (onFileTaskSuccess)
            {
                onFileTaskSuccess(task);
            }
        }
        else
        {
            // data task
            if (onDataTaskSuccess)
            {
                onDataTaskSuccess(task, data);
            }
        }
    };
}

Downloader::~Downloader()
{
    AXLOGD("Destruct Downloader {}", fmt::ptr(this));
}

std::shared_ptr<DownloadTask> Downloader::createDownloadDataTask(std::string_view srcUrl,
                                                                 std::string_view identifier /* = ""*/)
{
    auto task = std::make_shared<DownloadTask>(srcUrl, identifier);

    do
    {
        if (srcUrl.empty())
        {
            if (onTaskError)
            {
                onTaskError(*task, DownloadTask::ERROR_INVALID_PARAMS, 0, "URL or is empty.");
            }
            task.reset();
            break;
        }
        _impl->startTask(task);
    } while (0);

    return task;
}

std::shared_ptr<DownloadTask> Downloader::createDownloadFileTask(std::string_view srcUrl,
                                                                 std::string_view storagePath,
                                                                 std::string_view identifier,
                                                                 std::string_view md5checksum,
                                                                 bool background,
                                                                 std::string_view cacertPath)
{
    auto task = std::make_shared<DownloadTask>(srcUrl, storagePath, md5checksum, identifier, background, cacertPath);
    do
    {
        if (srcUrl.empty() || storagePath.empty())
        {
            if (onTaskError)
            {
                onTaskError(*task, DownloadTask::ERROR_INVALID_PARAMS, 0, "URL or storage path is empty.");
            }
            task.reset();
            break;
        }
        _impl->startTask(task);
    } while (0);

    return task;
}

// std::string Downloader::getFileNameFromUrl(std::string_view srcUrl)
//{
//    // Find file name and file extension
//    std::string filename;
//    unsigned int32_t found = srcUrl.find_last_of("/\\");
//    if (found != std::string::npos)
//        filename = srcUrl.substr(found+1);
//    return filename;
//}

}  // namespace network
}  // namespace ax
