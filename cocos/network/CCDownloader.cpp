/****************************************************************************
 Copyright (c) 2015-2016 cocos2d-x.org
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

#include "network/CCDownloader.h"

#include "network/CCDownloader-curl.h"
#define DownloaderImpl DownloaderCURL

namespace cocos2d {
namespace network {

DownloadTask::DownloadTask() {
    DLLOG("Construct DownloadTask %p", this);
}

DownloadTask::DownloadTask(const std::string& srcUrl, const std::string& identifier) {
    this->requestURL = srcUrl;
    this->identifier = identifier;
}

DownloadTask::DownloadTask(const std::string& srcUrl, const std::string& storagePath, const std::string& checksum,
    const std::string& identifier) {
    this->requestURL  = srcUrl;
    this->storagePath = storagePath;
    this->checksum    = checksum;
    this->identifier  = identifier;
}

DownloadTask::~DownloadTask() {
    DLLOG("Destruct DownloadTask %p", this);
}


void DownloadTask::cancel() {
    if (_coTask)
        _coTask->cancel();
}

////////////////////////////////////////////////////////////////////////////////
//  Implement Downloader
Downloader::Downloader() : Downloader(DownloaderHints{6, 45, ".tmp"}) {}

Downloader::Downloader(const DownloaderHints& hints) {
    DLLOG("Construct Downloader %p", this);
    _impl.reset(new DownloaderImpl(hints));
    _impl->onTaskProgress = [this](const DownloadTask& task,
                                std::function<int64_t(void* buffer, int64_t len)>& /*transferDataToBuffer*/) {
        if (onTaskProgress) {
            onTaskProgress(task);
        }
    };

    _impl->onTaskFinish = [this](const DownloadTask& task, int errorCode, int errorCodeInternal,
                              const std::string& errorStr, std::vector<unsigned char>& data) {
        if (DownloadTask::ERROR_NO_ERROR != errorCode) {
            if (onTaskError) {
                onTaskError(task, errorCode, errorCodeInternal, errorStr);
            }
            return;
        }

        // success callback
        if (task.storagePath.length()) {
            if (onFileTaskSuccess) {
                onFileTaskSuccess(task);
            }
        } else {
            // data task
            if (onDataTaskSuccess) {
                onDataTaskSuccess(task, data);
            }
        }
    };
}

Downloader::~Downloader() {
    DLLOG("Destruct Downloader %p", this);
}

std::shared_ptr<DownloadTask> Downloader::createDownloadDataTask(
    const std::string& srcUrl, const std::string& identifier /* = ""*/) {
    auto task = std::make_shared<DownloadTask>(srcUrl, identifier);

    do {
        if (srcUrl.empty()) {
            if (onTaskError) {
                onTaskError(*task, DownloadTask::ERROR_INVALID_PARAMS, 0, "URL or is empty.");
            }
            task.reset();
            break;
        }
        task->_coTask.reset(_impl->createCoTask(task));
    } while (0);

    return task;
}

std::shared_ptr<DownloadTask> Downloader::createDownloadFileTask(const std::string& srcUrl,
    const std::string& storagePath, const std::string& identifier, const std::string& md5checksum) {
    auto task = std::make_shared<DownloadTask>(srcUrl, storagePath, md5checksum, identifier);
    do {
        if (srcUrl.empty() || storagePath.empty()) {
            if (onTaskError) {
                onTaskError(*task, DownloadTask::ERROR_INVALID_PARAMS, 0, "URL or storage path is empty.");
            }
            task.reset();
            break;
        }
        task->_coTask.reset(_impl->createCoTask(task));
    } while (0);

    return task;
}

// std::string Downloader::getFileNameFromUrl(const std::string& srcUrl)
//{
//    // Find file name and file extension
//    std::string filename;
//    unsigned long found = srcUrl.find_last_of("/\\");
//    if (found != std::string::npos)
//        filename = srcUrl.substr(found+1);
//    return filename;
//}

} // namespace network
} // namespace cocos2d
