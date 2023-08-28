/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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


#include "platform/FileUtils.h"
#include "network/Downloader-emscripten.h"
#include <emscripten.h>

using namespace std;

namespace ax { namespace network {

        static int sDownloaderCounter;

        struct DownloadTaskEmscripten : public IDownloadTask
        {
            DownloadTaskEmscripten(unsigned int id)
            :id(id)
            ,bytesReceived(0)
            ,fetch(NULL)
            {
                DLLOG("Construct DownloadTaskEmscripten: %p", this);
            }
            virtual  ~DownloadTaskEmscripten()
            {
                DLLOG("Destruct DownloadTaskEmscripten: %p", this);
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
            DLLOG("Construct DownloaderEmscripten: %p", this);
        }

        DownloaderEmscripten::~DownloaderEmscripten()
        {
            DLLOG("Destruct DownloaderEmscripten: %p", this);
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
            
            DLLOG("DownloaderEmscripten::createCoTask id: %d", coTask->id);
            _taskMap.insert(make_pair(coTask->id, coTask));
        }

        void DownloaderEmscripten::onDataLoad(emscripten_fetch_t *fetch)
        {
            unsigned int taskId = fetch->id;
            uint64_t size = fetch->numBytes;
            DLLOG("DownloaderEmscripten::onDataLoad(taskId: %d, size: %d)", taskId, size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                DLLOG("DownloaderEmscripten::onDataLoad can't find task with id: %i, size: %i", taskId, size);
                return;
            }
            DownloadTaskEmscripten *coTask = iter->second;
            std::vector<unsigned char> buf((unsigned char*)fetch->data, (unsigned char*)fetch->data + size);
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
            DLLOG("DownloaderEmscripten::onLoad(taskId: %i, size: %i)", taskId, size);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                DLLOG("DownloaderEmscripten::onLoad can't find task with id: %i, size: %i", taskId, size);
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
                    if (false == util->createDirectory(dir))
                    {
                        errCode = DownloadTask::ERROR_CREATE_DIR_FAILED;
                        errCodeInternal = 0;
                        errDescription = "Can't create dir:";
                        errDescription.append(dir);
                        break;
                    }
                }

                // open file
                auto _fs = util->openFileStream(storagePath, FileStream::Mode::READ);
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
            DLLOG("DownloaderEmscripten::onProgress(taskId: %i, dlnow: %d, dltotal: %d)", taskId, dlNow, dlTotal);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                DLLOG("DownloaderEmscripten::onProgress can't find task with id: %i", taskId);
                return;
            }

            if (dlTotal == 0) {
                DLLOG("DownloaderEmscripten::onProgress dlTotal unknown, usually caused by unknown content-length header %i", taskId);
                return;
            }

            DownloadTaskEmscripten *coTask = iter->second;
            function<int64_t(void*, int64_t)> transferDataToBuffer; // just a placeholder
            int dl = dlNow - coTask->bytesReceived;
            coTask->bytesReceived = dlNow;
            downloader->onTaskProgress(*coTask->task, transferDataToBuffer);
        }

        void DownloaderEmscripten::onError(emscripten_fetch_t *fetch)
        {
            unsigned int taskId = fetch->id;
            DLLOG("DownloaderEmscripten::onLoad(taskId: %i)", taskId);
            DownloaderEmscripten* downloader = reinterpret_cast<DownloaderEmscripten*>(fetch->userData);
            auto iter = downloader->_taskMap.find(taskId);
            if (downloader->_taskMap.end() == iter)
            {
                emscripten_fetch_close(fetch);
                DLLOG("DownloaderEmscripten::onLoad can't find task with id: %i", taskId);
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
}  // namespace cocos2d::network
