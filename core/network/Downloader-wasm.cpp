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

        struct DownloadTaskEmscripten : public IDownloadTask
        {
            explicit DownloadTaskEmscripten(unsigned int id_)
            :id(id_)
            ,bytesReceived(0)
            ,fetch(NULL)
            {
                AXLOGD("Construct DownloadTaskEmscripten: {}", fmt::ptr(this));
            }
            virtual  ~DownloadTaskEmscripten()
            {
                AXLOGD("Destruct DownloadTaskEmscripten: {}", fmt::ptr(this));
            }

            int bytesReceived;
            unsigned int id;
            emscripten_fetch_t * fetch;
            shared_ptr<const DownloadTask> task;    // reference to DownloadTask, when task finish, release
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

            DownloadTaskEmscripten *coTask = new DownloadTaskEmscripten(fetch->id);
            coTask->task = task;

            AXLOGD("DownloaderEmscripten::createCoTask id: {}", coTask->id);
            _taskMap.insert(make_pair(coTask->id, coTask));
        }

        void DownloaderEmscripten::onDataLoad(emscripten_fetch_t *fetch)
        {
            unsigned int taskId = fetch->id;
            uint64_t size = fetch->numBytes;
            AXLOGD("DownloaderEmscripten::onDataLoad(taskId: {}, size: {})", taskId, size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onDataLoad can't find task with id: {}, size: {}", taskId, size);
                return;
            }
            DownloadTaskEmscripten *coTask = iter->second;
            std::vector<unsigned char> buf(reinterpret_cast<const uint8_t*>(fetch->data), reinterpret_cast<const uint8_t*>(fetch->data) + size);
            emscripten_fetch_close(fetch);
            coTask->fetch = fetch = NULL;

            downloader->_taskMap.erase(iter);
            downloader->onTaskFinish(*coTask->task,
                DownloadTask::ERROR_NO_ERROR,
                0,
                "",
                buf
            );

            coTask->task.reset();
        }

        void DownloaderEmscripten::onLoad(emscripten_fetch_t *fetch)
        {
            unsigned int taskId = fetch->id;
            uint64_t size = fetch->numBytes;
            AXLOGD("DownloaderEmscripten::onLoad(taskId: {}, size: {})", taskId, size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onLoad can't find task with id: {}, size: {}", taskId, size);
                return;
            }
            DownloadTaskEmscripten *coTask = iter->second;
            vector<unsigned char> buf;
            downloader->_taskMap.erase(iter);

            string storagePath = coTask->task->storagePath;
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
                auto _fs = util->openFileStream(storagePath, IFileStream::Mode::READ);
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
            coTask->fetch = fetch = NULL;

            downloader->onTaskFinish(*coTask->task,
                errCode,
                errCodeInternal,
                errDescription,
                buf
            );
            coTask->task.reset();
        }

        void DownloaderEmscripten::onProgress(emscripten_fetch_t *fetch)
        {
            uint64_t dlTotal = fetch->totalBytes;
            uint64_t dlNow = fetch->dataOffset;
            unsigned int taskId = fetch->id;
            AXLOGD("DownloaderEmscripten::onProgress(taskId: {}, dlnow: {}, dltotal: {})", taskId, dlNow, dlTotal);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                AXLOGD("DownloaderEmscripten::onProgress can't find task with id: {}", taskId);
                return;
            }

            if (dlTotal == 0) {
                AXLOGD("DownloaderEmscripten::onProgress dlTotal unknown, usually caused by unknown content-length header {}", taskId);
                return;
            }

            DownloadTaskEmscripten *coTask = iter->second;
            coTask->bytesReceived = dlNow;
            downloader->onTaskProgress(*coTask->task);
        }

        void DownloaderEmscripten::onError(emscripten_fetch_t *fetch)
        {
            unsigned int taskId = fetch->id;
            AXLOGD("DownloaderEmscripten::onLoad(taskId: {})", taskId);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                emscripten_fetch_close(fetch);
                AXLOGD("DownloaderEmscripten::onLoad can't find task with id: {}", taskId);
                return;
            }
            DownloadTaskEmscripten *coTask = iter->second;
            vector<unsigned char> buf;
            downloader->_taskMap.erase(iter);
            downloader->onTaskFinish(*coTask->task,
                DownloadTask::ERROR_IMPL_INTERNAL,
                fetch->status,
                fetch->statusText,
                buf
            );

            emscripten_fetch_close(fetch);
            coTask->fetch = fetch = NULL;
            coTask->task.reset();
        }
    }
}  // namespace ax::network
