/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev

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
#include "network/Downloader-wasm.h"
#include <emscripten.h>

using namespace std;

namespace ax { namespace network {

        static int sDownloaderCounter;

        struct DownloadContextEmscripten : public IDownloadContext
        {
            explicit DownloadContextEmscripten(emscripten_fetch_t* fetch_)
            :fetch(fetch_)
            {
                AXLOGD("Construct DownloadContextEmscripten: {}", fmt::ptr(this));
            }
            virtual  ~DownloadContextEmscripten()
            {
                AXLOGD("Destruct DownloadContextEmscripten: {}", fmt::ptr(this));
            }

            emscripten_fetch_t * fetch;
            shared_ptr<DownloadTask> task;    // reference to DownloadTask, when task finish, release
        };

        DownloaderEmscripten::DownloaderEmscripten(const DownloaderHints& hints)
        : _id(++sDownloaderCounter)
        , hints(hints)
        {
            AXLOGD("Construct DownloaderEmscripten: {}", fmt::ptr(this));
        }

        DownloaderEmscripten::~DownloaderEmscripten()
        {
            AXLOGD("Destruct DownloaderEmscripten: {}", fmt::ptr(this));
            for (auto iter = _taskMap.begin(); iter != _taskMap.end(); ++iter)
            {
                if(iter->second->fetch != NULL) {
                    emscripten_fetch_close(iter->second->fetch);
                }
            }
        }

        void DownloaderEmscripten::startTask(std::shared_ptr<DownloadTask>& task)
        {
            emscripten_fetch_attr_t attr;
            emscripten_fetch_attr_init(&attr);
            strcpy(attr.requestMethod, "GET");
            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
            if(task->storagePath.length() == 0) {
                attr.onsuccess = DownloaderEmscripten::onDataLoad;
            }else{
                attr.onsuccess = DownloaderEmscripten::onLoad;
            }
            attr.onprogress = DownloaderEmscripten::onProgress;
            attr.onerror = DownloaderEmscripten::onError;
            attr.timeoutMSecs = this->hints.timeoutInSeconds * 1000;
            emscripten_fetch_t *fetch = emscripten_fetch(&attr, task->requestURL.c_str());
            fetch->userData = this;

            auto context = new DownloadContextEmscripten(fetch);
            context->task = task;

            AXLOGD("DownloaderEmscripten::startTask fetch: {}", fmt::ptr(fetch));
            _taskMap.emplace(fetch, context);
        }

        void DownloaderEmscripten::onDataLoad(emscripten_fetch_t *fetch)
        {
            int64_t size = fetch->totalBytes;
            AXLOGD("DownloaderEmscripten::onDataLoad(fetch: {}, size: {})", fmt::ptr(fetch), size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(fetch);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onDataLoad can't find task with fetch: {}, size: {}", fmt::ptr(fetch), size);
                return;
            }
            auto context = iter->second;
            updateTaskProgressInfo(*context->task, fetch);
            std::vector<unsigned char> buf(reinterpret_cast<const uint8_t*>(fetch->data), reinterpret_cast<const uint8_t*>(fetch->data) + size);
            emscripten_fetch_close(fetch);
            context->fetch = fetch = NULL;

            downloader->_taskMap.erase(iter);
            downloader->onTaskFinish(*context->task,
                DownloadTask::ERROR_NO_ERROR,
                0,
                "",
                buf
            );

            context->task.reset();
        }

        void DownloaderEmscripten::onLoad(emscripten_fetch_t *fetch)
        {
            int64_t size = fetch->totalBytes;
            AXLOGD("DownloaderEmscripten::onLoad(fetch: {}, size: {})", fmt::ptr(fetch), size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(fetch);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onLoad can't find task with fetch: {}, size: {}", fmt::ptr(fetch), size);
                return;
            }
            auto context = iter->second;
            updateTaskProgressInfo(*context->task, fetch);
            vector<unsigned char> buf;
            downloader->_taskMap.erase(iter);

            string storagePath = context->task->storagePath;
            int errCode = DownloadTask::ERROR_NO_ERROR;
            int errCodeInternal = 0;
            string errDescription;
            do
            {
                auto util = FileUtils::getInstance();
                if (util->isFileExist(storagePath))
                {
                    if (false == util->removeFile(storagePath))
                    {
                        errCode = DownloadTask::ERROR_REMOVE_FILE_FAILED;
                        errCodeInternal = 0;
                        errDescription = "Can't remove old file: ";
                        errDescription.append(storagePath);
                        break;
                    }
                }

                string dir;
                size_t found = storagePath.find_last_of("/\\");
                if (found == string::npos)
                {
                    errCode = DownloadTask::ERROR_INVALID_PARAMS;
                    errCodeInternal = 0;
                    errDescription = "Can't find dirname in storagePath.";
                    break;
                }

                // ensure directory is exist
                dir = storagePath.substr(0, found + 1);
                if (false == util->isDirectoryExist(dir))
                {
                    if (false == util->createDirectories(dir))
                    {
                        errCode = DownloadTask::ERROR_CREATE_DIR_FAILED;
                        errCodeInternal = 0;
                        errDescription = "Can't create dir:";
                        errDescription.append(dir);
                        break;
                    }
                }

                // open file
                auto _fs = util->openFileStream(storagePath, IFileStream::Mode::WRITE);
                if (!_fs)
                {
                    errCode = DownloadTask::ERROR_OPEN_FILE_FAILED;
                    errCodeInternal = 0;
                    errDescription = "Can't open file:";
                    errDescription.append(storagePath);
                    break;
                }

                _fs->write(fetch->data, static_cast<unsigned int>(size));

            } while (0);
            emscripten_fetch_close(fetch);
            context->fetch = fetch = NULL;

            downloader->onTaskFinish(*context->task,
                errCode,
                errCodeInternal,
                errDescription,
                buf
            );
            context->task.reset();
        }

        void DownloaderEmscripten::onProgress(emscripten_fetch_t *fetch)
        {
            AXLOGD("DownloaderEmscripten::onProgress(fetch: {}, dlnow: {}, dltotal: {})", fmt::ptr(fetch), fetch->dataOffset, fetch->totalBytes);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(fetch);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onProgress can't find task with fetch: {}", fmt::ptr(fetch));
                return;
            }

            if (fetch->totalBytes == 0) {
                AXLOGD("DownloaderEmscripten::onProgress fetch totalBytes unknown, usually caused by unknown content-length header {}", fmt::ptr(fetch));
                return;
            }

            auto context = iter->second;
            updateTaskProgressInfo(*context->task, fetch);
            downloader->onTaskProgress(*context->task);
        }

        void DownloaderEmscripten::onError(emscripten_fetch_t *fetch)
        {
            AXLOGD("DownloaderEmscripten::onLoad(fetch: {})", fmt::ptr(fetch));
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(fetch);
            if (downloader->_taskMap.end() == iter)
            {
                emscripten_fetch_close(fetch);
                AXLOGD("DownloaderEmscripten::onLoad can't find task with fetch: {}", fmt::ptr(fetch));
                return;
            }
            auto context = iter->second;
            updateTaskProgressInfo(*context->task, fetch);
            vector<unsigned char> buf;
            downloader->_taskMap.erase(iter);
            downloader->onTaskFinish(*context->task,
                DownloadTask::ERROR_IMPL_INTERNAL,
                fetch->status,
                fetch->statusText,
                buf
            );

            emscripten_fetch_close(fetch);
            context->fetch = fetch = NULL;
            context->task.reset();
        }

        void DownloaderEmscripten::updateTaskProgressInfo(DownloadTask& task, emscripten_fetch_t *fetch)
        {
            task.progressInfo.bytesReceived      = fetch->numBytes;
            task.progressInfo.totalBytesReceived = fetch->dataOffset;
            task.progressInfo.totalBytesExpected = fetch->totalBytes;
        }
    }
}  // namespace ax::network
