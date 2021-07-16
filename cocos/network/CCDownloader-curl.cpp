/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

 https://adxe.org

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

#include "network/CCDownloader-curl.h"

#include <cinttypes>
#include <set>

#include <curl/curl.h>
#include <thread>
#include "base/ccUtils.h"
#include "base/ccUTF8.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "platform/CCFileUtils.h"
#include "network/CCDownloader.h"
#include "platform/CCFileStream.h"
#include "openssl/md5.h"
#include "yasio/xxsocket.hpp"

// **NOTE**
// In the file:
// member function with suffix "Proc" designed called in DownloaderCURL::_threadProc
// member function without suffix designed called in main thread

#define CC_CURL_POLL_TIMEOUT_MS 50 // wait until DNS query done

enum {
    kCheckSumStateSucceed = 1,
    kCheckSumStateFailed  = 1 << 1,
};

namespace cocos2d {
namespace network {

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloadTaskCURL

class DownloadTaskCURL : public IDownloadTask {
    static int _sSerialId;

    // if more than one task write to one file, cause file broken
    // so use a set to check this situation
    static std::set<std::string> _sStoragePathSet;

public:
    int serialId;
    DownloaderCURL& owner;

    DownloadTaskCURL(DownloaderCURL& o)
        : serialId(_sSerialId++), owner(o), _requestHeaders(nullptr)

    {
        _initInternal();
        DLLOG("Construct DownloadTaskCURL %p", this);
    }

    virtual ~DownloadTaskCURL() {
        if (_errCode != DownloadTask::ERROR_TASK_DUPLICATED) {
            // if task destroyed unnormally, we should release WritenFileName stored in set.
            // Normally, this action should done when task finished.
            if (_tempFileName.length() && _sStoragePathSet.end() != _sStoragePathSet.find(_tempFileName)) {
                DownloadTaskCURL::_sStoragePathSet.erase(_tempFileName);
            }
        }

        _fs.reset();
        _fsMd5.reset();

        if (_requestHeaders)
            curl_slist_free_all(_requestHeaders);

        DLLOG("Destruct DownloadTaskCURL %p", this);
    }

    bool init(const std::string& filename, const std::string& tempSuffix) {
        if (0 == filename.length()) {
            // data task
            _buf.reserve(CURL_MAX_WRITE_SIZE);
            return true;
        }

        // file task
        _fileName     = filename;
        _tempFileName = filename;
        _tempFileName.append(tempSuffix);

        if (_sStoragePathSet.end() != _sStoragePathSet.find(_tempFileName)) {
            // there is another task uses this storage path
            _errCode         = DownloadTask::ERROR_TASK_DUPLICATED;
            _errCodeInternal = 0;
            _errDescription  = "More than one download file task write to same file:";
            _errDescription.append(_tempFileName);
            return false;
        }
        _sStoragePathSet.insert(_tempFileName);

        // open temp file handle for write
        bool ret = false;
        do {
            std::string dir;
            size_t found = _tempFileName.find_last_of("/\\");
            if (found == std::string::npos) {
                _errCode         = DownloadTask::ERROR_INVALID_PARAMS;
                _errCodeInternal = 0;
                _errDescription  = "Can't find dirname in storagePath.";
                break;
            }

            // ensure directory is exist
            dir = _tempFileName.substr(0, found + 1);
            if (!FileUtils::getInstance()->isDirectoryExistInternal(dir)) {
                if (!FileUtils::getInstance()->createDirectory(dir)) {
                    _errCode         = DownloadTask::ERROR_CREATE_DIR_FAILED;
                    _errCodeInternal = 0;
                    _errDescription  = "Can't create dir:";
                    _errDescription.append(dir);
                    break;
                }
            }
            // open file
            _fs = FileUtils::getInstance()->openFileStream(_tempFileName, FileStream::Mode::APPEND);
            if (!_fs) {
                _errCode         = DownloadTask::ERROR_OPEN_FILE_FAILED;
                _errCodeInternal = 0;
                _errDescription  = "Can't open file:";
                _errDescription.append(_tempFileName);
                break;
            }

            // init md5 state
            _checksumFileName = filename + ".chksum";

            _fsMd5 = FileUtils::getInstance()->openFileStream(_checksumFileName, FileStream::Mode::OVERLAPPED);
            _fsMd5->seek(0, SEEK_END);
            if (_fsMd5->tell() != sizeof(_md5State)) {
                MD5_Init(&_md5State);
            } else {
                _fsMd5->seek(0, SEEK_SET);
                _fsMd5->read(&_md5State, sizeof(_md5State));
            }
            ret = true;
        } while (0);

        return ret;
    }

    void cancel() override {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (!_cancelled) {
            _cancelled = true;
            if (this->_sockfd != -1) {
                ::shutdown(this->_sockfd, SD_BOTH); // may cause curl CURLE_SEND_ERROR(55) or CURLE_RECV_ERROR(56)
                this->_sockfd = -1;
            }
        }
    }

    curl_socket_t openSocket(curlsocktype propose, curl_sockaddr* addr) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (!_cancelled) {
            this->_sockfd = ::socket(addr->family, addr->socktype, addr->protocol);
            return this->_sockfd;
        }
        return -1;
    }

    /*
    retval: 0. don't check, 1. check succeed, 2. check failed
    */
    int checkFileMd5(const std::string& requiredsum, std::string* outsum = nullptr) {
        int status = 0;
        if (!requiredsum.empty()) {
            std::string digest(16, '\0');
            auto state = _md5State; // Excellent, make a copy, don't modify the origin state.
            MD5_Final((uint8_t*) &digest.front(), &state);
            auto checksum = utils::bin2hex(digest);
            status        = requiredsum == checksum ? kCheckSumStateSucceed : kCheckSumStateFailed;

            if (outsum != nullptr)
                *outsum = std::move(checksum);
        }
        return status;
    }

    void initProc() {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _initInternal();
    }

    void setErrorProc(int code, int codeInternal, const char* desc) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _errCode         = code;
        _errCodeInternal = codeInternal;
        _errDescription  = desc;
    }

    size_t writeDataProc(unsigned char* buffer, size_t size, size_t count) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        size_t ret = 0;

        auto bytes_transferred = size * count;

        if (_fs) {
            ret = _fs->write(buffer, bytes_transferred); // fwrite(buffer, size, count, _fp);
        } else {
            ret          = bytes_transferred;
            auto cap     = _buf.capacity();
            auto bufSize = _buf.size();
            if (cap < bufSize + ret) {
                _buf.reserve(bufSize * 2);
            }
            _buf.insert(_buf.end(), buffer, buffer + ret);
        }
        if (ret > 0) {
            _bytesReceived += ret;
            _totalBytesReceived += ret;

            if (_fsMd5) {
                ::MD5_Update(&_md5State, buffer, bytes_transferred);
                _fsMd5->seek(0, SEEK_SET);
                _fsMd5->write(&_md5State, sizeof(_md5State));
            }
        }

        curl_easy_getinfo(_curl, CURLINFO_SPEED_DOWNLOAD, &_speed);

        return ret;
    }

private:
    friend class DownloaderCURL;

    // for lock object instance
    std::recursive_mutex _mutex;

    // header info
    bool _acceptRanges;
    bool _headerAchieved;
    int64_t _totalBytesExpected;

    double _speed;
    CURL* _curl;
    curl_socket_t _sockfd = -1; // store the sockfd to support cancel download manually
    bool _cancelled       = false;

    std::string _header; // temp buffer for receive header string, only used in thread proc

    curl_slist* _requestHeaders;

    // progress
    int64_t _bytesReceived;
    int64_t _totalBytesReceived;

    // error
    int _errCode;
    int _errCodeInternal;
    std::string _errDescription;

    // for saving data
    std::string _fileName;
    std::string _tempFileName;
    std::string _checksumFileName;
    std::vector<unsigned char> _buf;
    std::unique_ptr<FileStream> _fs{};

    // calculate md5 in downloading time support
    std::unique_ptr<FileStream> _fsMd5{}; // store md5 state realtime
    MD5state_st _md5State;


    void _initInternal() {
        _acceptRanges       = (false);
        _headerAchieved     = (false);
        _bytesReceived      = (0);
        _totalBytesReceived = (0);
        _totalBytesExpected = (0);
        _speed              = 0;
        _curl               = nullptr;
        _errCode            = (DownloadTask::ERROR_NO_ERROR);
        _errCodeInternal    = (CURLE_OK);
        _header.resize(0);
        _header.reserve(384); // pre alloc header string buffer
    }
};
int DownloadTaskCURL::_sSerialId;
std::set<std::string> DownloadTaskCURL::_sStoragePathSet;

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloaderCURL::Impl
// This class shared by DownloaderCURL and work thread.
class DownloaderCURL::Impl : public std::enable_shared_from_this<DownloaderCURL::Impl> {
public:
    DownloaderHints hints;

    Impl()
    //        : _thread(nullptr)
    {
        DLLOG("Construct DownloaderCURL::Impl %p", this);
    }

    ~Impl() {
        DLLOG("Destruct DownloaderCURL::Impl %p", this);
    }

    void addTask(std::shared_ptr<DownloadTask> task, DownloadTaskCURL* coTask) {
        int status = coTask->checkFileMd5(task->checksum);

        if (status & kCheckSumStateSucceed || DownloadTask::ERROR_NO_ERROR != coTask->_errCode) {
            _owner->_onDownloadFinished(*task, status);
        } else {
            std::lock_guard<std::mutex> lock(_requestMutex);
            _requestQueue.push_back(task);
        }
    }

    void run() {
        std::lock_guard<std::mutex> lock(_threadMutex);

        if (_tasksFinished) { // all tasks finished, make sure thread not joinable
            if (_thread.joinable())
                _thread.join();
            _tasksFinished = false;
        }

        if (!_thread.joinable())
            _thread = std::thread(&DownloaderCURL::Impl::_threadProc, this);
    }

    void stop() { // make sure all task exit properly
        if (!_requestQueue.empty()) {
            std::lock_guard<std::mutex> lock(_requestMutex);
            _requestQueue.clear();
        }

        if (!_processSet.empty()) {
            std::lock_guard<std::mutex> lock(_processMutex);
            for (auto& task : _processSet)
                task->cancel();
            _processSet.clear();
        }

        if (_thread.joinable())
            _thread.join();
    }

    bool stopped() const {
        std::lock_guard<std::mutex> lock(_threadMutex);
        return false == _thread.joinable() ? true : false;
    }

    // Gets non background task list
    void getProcessTasks(std::vector<std::shared_ptr<DownloadTask>>& outList) {
        std::lock_guard<std::mutex> lock(_processMutex);
        outList.reserve(_processSet.size());
        for (auto& task : _processSet) {
            if (!task->background)
                outList.push_back(task);
        }
    }

    void getFinishedTasks(std::vector<std::shared_ptr<DownloadTask>>& outList) {
        std::lock_guard<std::mutex> lock(_finishedMutex);
        outList.reserve(_finishedQueue.size());
        outList.insert(outList.end(), _finishedQueue.begin(), _finishedQueue.end());
        _finishedQueue.clear();
    }

private:
    static size_t _outputHeaderCallbackProc(void* buffer, size_t size, size_t count, void* userdata) {
        int strLen = int(size * count);
        DLLOG("    _outputHeaderCallbackProc: %.*s", strLen, buffer);
        DownloadTaskCURL& coTask = *((DownloadTaskCURL*) (userdata));
        coTask._header.append((const char*) buffer, strLen);
        return strLen;
    }

    static size_t _outputDataCallbackProc(void* buffer, size_t size, size_t count, void* userdata) {
        //            DLLOG("    _outputDataCallbackProc: size(%ld), count(%ld)", size, count);
        DownloadTaskCURL* coTask = (DownloadTaskCURL*) userdata;

        // If your callback function returns CURL_WRITEFUNC_PAUSE it will cause this transfer to become paused.
        return coTask->writeDataProc((unsigned char*) buffer, size, count);
    }

    static int _progressCallbackProc(
        void* ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded) {
        auto task = (DownloadTask*) ptr;
        if (!task || !task->background)
            return 0;
        auto& coTask = task->_coTask;
        if (coTask) {
            auto& downloaderImpl = static_cast<DownloadTaskCURL*>(coTask.get())->owner;
            downloaderImpl._updateTaskProgressInfo(*task);
            downloaderImpl.onTaskProgress(*task, downloaderImpl._transferDataToBuffer);
        }

        return 0;
    }

    static curl_socket_t _openSocketCallback(DownloadTaskCURL& pTask, curlsocktype propose, curl_sockaddr* addr) {
        return pTask.openSocket(propose, addr);
    }

    // this function designed call in work thread
    // the curl handle destroyed in _threadProc
    // handle inited for get header
    CURLcode _initCurlHandleProc(CURL* handle, std::shared_ptr<DownloadTask>& task, bool forContent = false) {
        DownloadTaskCURL* coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());

        /* Resolve host domain to ip */
        std::string internalURL = task->requestURL;
        // Curl_custom_setup(handle, internalURL, (void**)& coTask->_requestHeaders);

        // set url
        curl_easy_setopt(handle, CURLOPT_URL, internalURL.c_str());

        // set write func
        if (forContent) {
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _outputDataCallbackProc);
        } else {
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _outputHeaderCallbackProc);
        }
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, coTask);

        if (task->background) {
            curl_easy_setopt(handle, CURLOPT_NOPROGRESS, false);
            curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, task.get());
            curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, _progressCallbackProc);
        } else {
            curl_easy_setopt(handle, CURLOPT_NOPROGRESS, true);
        }
        curl_easy_setopt(handle, CURLOPT_FAILONERROR, true);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(handle, CURLOPT_OPENSOCKETFUNCTION, _openSocketCallback);
        curl_easy_setopt(handle, CURLOPT_OPENSOCKETDATA, coTask);

        if (forContent) {
            /** if server acceptRanges and local has part of file, we continue to download **/
            if (coTask->_acceptRanges && coTask->_totalBytesReceived > 0) {
                char buf[128];
                sprintf(buf, "%" PRId64 "-", coTask->_totalBytesReceived);
                curl_easy_setopt(handle, CURLOPT_RANGE, buf);
                curl_easy_setopt(handle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t) coTask->_totalBytesReceived);
            }
        } else {
            // get header options
            curl_easy_setopt(handle, CURLOPT_HEADER, 1);
            curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
        }

        //            if (!sProxy.empty())
        //            {
        //                curl_easy_setopt(curl, CURLOPT_PROXY, sProxy.c_str());
        //            }
        if (hints.timeoutInSeconds) {
            curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, hints.timeoutInSeconds);
        }

        static const long LOW_SPEED_LIMIT = 1;
        static const long LOW_SPEED_TIME  = 10;
        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);

        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);

        static const int MAX_REDIRS = 5;
        if (MAX_REDIRS) {
            curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, true);
            curl_easy_setopt(handle, CURLOPT_MAXREDIRS, MAX_REDIRS);
        }

        coTask->_curl = handle;

        return CURLE_OK;
    }

    // get header info, if success set handle to content download state
    bool _getHeaderInfoProc(CURL* handle, DownloadTaskCURL* coTask) {
        CURLcode rc = CURLE_OK;
        do {
            long httpResponseCode = 0;
            rc                    = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpResponseCode);
            if (CURLE_OK != rc) {
                break;
            }
            // Comment follow code to support ftp
            /*if (200 != httpResponseCode)
            {
                char buf[256] = {0};
                sprintf(buf
                        , "When request url(%s) header info, return unexcept http response code(%ld)"
                        , wrapper.first->requestURL.c_str()
                        , httpResponseCode);
                coTask.setErrorProc(DownloadTask::ERROR_IMPL_INTERNAL, CURLE_OK, buf);
            }*/

            //                curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
            //                curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &contentType);
            double contentLen = 0;
            rc                = curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLen);
            if (CURLE_OK != rc) {
                break;
            }

            // std::transform(coTask._header.begin(), coTask._header.end(), coTask._header.begin(), ::toupper);
            bool acceptRanges = true; // (string::npos != coTask._header.find("ACCEPT-RANGES")) ? true : false;

            // get current file size
            int64_t fileSize = 0;
            if (acceptRanges && coTask->_tempFileName.length()) {
                fileSize = FileUtils::getInstance()->getFileSize(coTask->_tempFileName);
            }

            // set header info to coTask
            std::lock_guard<std::recursive_mutex> lock(coTask->_mutex);
            coTask->_totalBytesExpected = (int64_t) contentLen;
            coTask->_acceptRanges       = acceptRanges;
            if (acceptRanges && fileSize > 0) {
                coTask->_totalBytesReceived = fileSize;
            }
            coTask->_headerAchieved = true;
        } while (0);

        if (CURLE_OK != rc) {
            coTask->setErrorProc(DownloadTask::ERROR_IMPL_INTERNAL, rc, curl_easy_strerror(rc));
        }
        return coTask->_headerAchieved;
    }

    void _threadProc() {
        DLLOG("++++DownloaderCURL::Impl::_threadProc begin %p", this);
        // the holder prevent DownloaderCURL::Impl class instance be destruct in main thread
        auto holder                        = this->shared_from_this();
        auto thisThreadId                  = std::this_thread::get_id();
        uint32_t countOfMaxProcessingTasks = this->hints.countOfMaxProcessingTasks;
        // init curl content
        CURLM* curlmHandle = curl_multi_init();
        std::unordered_map<CURL*, std::shared_ptr<DownloadTask>> coTaskMap;
        int runningHandles = 0;
        CURLMcode mcode    = CURLM_OK;
        int rc             = 0; // select return code

        do {
            // check the thread should exit or not
            {
                std::lock_guard<std::mutex> lock(_threadMutex);
                // if the Impl stoped, this->_thread.reset will be called, thus _thread.get_id() not equal with
                // thisThreadId
                if (thisThreadId != this->_thread.get_id()) {
                    break;
                }
            }

            if (runningHandles) {
                // get timeout setting from multi-handle
                long timeoutMS = -1;
                curl_multi_timeout(curlmHandle, &timeoutMS);

                if (timeoutMS < 0) {
                    timeoutMS = 1000;
                }

                /* get file descriptors from the transfers */
                fd_set fdread;
                fd_set fdwrite;
                fd_set fdexcep;
                int maxfd = -1;

                FD_ZERO(&fdread);
                FD_ZERO(&fdwrite);
                FD_ZERO(&fdexcep);

                mcode = curl_multi_fdset(curlmHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
                if (CURLM_OK != mcode) {
                    break;
                }

                // do wait action
                if (maxfd == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(CC_CURL_POLL_TIMEOUT_MS));
                    rc = 0;
                } else {
                    struct timeval timeout;

                    timeout.tv_sec  = timeoutMS / 1000;
                    timeout.tv_usec = (timeoutMS % 1000) * 1000;

                    rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
                }

                if (rc < 0) {
                    DLLOG("    _threadProc: select return unexpect code: %d", rc);
                }
            }

            if (!coTaskMap.empty()) {
                mcode = CURLM_CALL_MULTI_PERFORM;
                while (CURLM_CALL_MULTI_PERFORM == mcode) {
                    mcode = curl_multi_perform(curlmHandle, &runningHandles);
                }
                if (CURLM_OK != mcode) {
                    break;
                }

                struct CURLMsg* m;
                do {
                    int msgq = 0;
                    m        = curl_multi_info_read(curlmHandle, &msgq);
                    if (m && (m->msg == CURLMSG_DONE)) {
                        CURL* curlHandle = m->easy_handle;
                        CURLcode errCode = m->data.result;

                        auto task = coTaskMap[curlHandle];

                        // remove from multi-handle
                        curl_multi_remove_handle(curlmHandle, curlHandle);
                        bool reinited = false;
                        do {
                            auto coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());
                            if (CURLE_OK != errCode) {
                                coTask->setErrorProc(
                                    DownloadTask::ERROR_IMPL_INTERNAL, errCode, curl_easy_strerror(errCode));
                                break;
                            }

                            // if the task is content download task, cleanup the handle
                            if (coTask->_headerAchieved) {
                                break;
                            }

                            // the task is get header task
                            // first, we get info from response
                            if (!_getHeaderInfoProc(curlHandle, coTask)) {
                                // the error info has been set in _getHeaderInfoProc
                                break;
                            }

                            // after get header info success
                            // wrapper.second->_totalBytesReceived inited by local file size
                            // if the local file size equal with the content size from header, the file has
                            // downloaded finish
                            if (coTask->_totalBytesReceived
                                && coTask->_totalBytesReceived == coTask->_totalBytesExpected) {
                                // the file has download complete
                                // break to move this task to finish queue
                                break;
                            }
                            // reinit curl handle for download content
                            curl_easy_reset(curlHandle);
                            auto error = _initCurlHandleProc(curlHandle, task, true);
                            if (error != CURLE_OK) {
                                coTask->setErrorProc(
                                    DownloadTask::ERROR_IMPL_INTERNAL, error, curl_easy_strerror(error));
                                break;
                            }
                            mcode = curl_multi_add_handle(curlmHandle, curlHandle);
                            if (CURLM_OK != mcode) {
                                coTask->setErrorProc(
                                    DownloadTask::ERROR_IMPL_INTERNAL, mcode, curl_multi_strerror(mcode));
                                break;
                            }
                            reinited = true;
                        } while (0);

                        if (reinited) {
                            continue;
                        }
                        curl_easy_cleanup(curlHandle);
                        DLLOG("    _threadProc task clean cur handle :%p with errCode:%d", curlHandle, errCode);

                        // remove from coTaskMap
                        coTaskMap.erase(curlHandle);

                        // remove from _processSet
                        {
                            std::lock_guard<std::mutex> lock(_processMutex);
                            if (_processSet.end() != _processSet.find(task)) {
                                _processSet.erase(task);
                            }
                        }

                        if (task->background)
                            _owner->_onDownloadFinished(*task);
                        else {
                            std::lock_guard<std::mutex> lock(_finishedMutex);
                            _finishedQueue.push_back(task);
                        }
                    }
                } while (m);
            }

            // process tasks in _requestList
            auto size = coTaskMap.size();
            while (0 == countOfMaxProcessingTasks || size < countOfMaxProcessingTasks) {
                // get task wrapper from request queue
                std::shared_ptr<DownloadTask> task;
                {
                    std::lock_guard<std::mutex> lock(_requestMutex);
                    if (!_requestQueue.empty()) {
                        task = _requestQueue.front();
                        _requestQueue.pop_front();
                    }
                }

                // if request queue is empty, the wrapper.first is nullptr
                if (!task) {
                    break;
                }

                auto coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());
                coTask->initProc();

                // create curl handle from task and add into curl multi handle
                CURL* curlHandle = curl_easy_init();

                if (nullptr == curlHandle) {
                    coTask->setErrorProc(DownloadTask::ERROR_IMPL_INTERNAL, 0, "Alloc curl handle failed.");
                    _owner->_onDownloadFinished(*task);
                    continue;
                }

                // init curl handle for get header info
                _initCurlHandleProc(curlHandle, task);

                // add curl handle to process list
                mcode = curl_multi_add_handle(curlmHandle, curlHandle);
                if (CURLM_OK != mcode) {
                    coTask->setErrorProc(DownloadTask::ERROR_IMPL_INTERNAL, mcode, curl_multi_strerror(mcode));
                    _owner->_onDownloadFinished(*task);
                    continue;
                }

                DLLOG("    _threadProc task create curl handle:%p", curlHandle);
                coTaskMap[curlHandle] = task;
                std::lock_guard<std::mutex> lock(_processMutex);
                _processSet.insert(task);
            }
        } while (!coTaskMap.empty());

        _tasksFinished = true;

        curl_multi_cleanup(curlmHandle);
        DLLOG("----DownloaderCURL::Impl::_threadProc end");
    }

    std::thread _thread;
    std::atomic_bool _tasksFinished{};
    std::deque<std::shared_ptr<DownloadTask>> _requestQueue;
    std::set<std::shared_ptr<DownloadTask>> _processSet;
    std::deque<std::shared_ptr<DownloadTask>> _finishedQueue;

    mutable std::mutex _threadMutex;
    std::mutex _requestMutex;
    std::mutex _processMutex;
    std::mutex _finishedMutex;

public:
    DownloaderCURL* _owner = nullptr;
};


////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloaderCURL
DownloaderCURL::DownloaderCURL(const DownloaderHints& hints) : _impl(std::make_shared<Impl>()), _currTask(nullptr) {
    DLLOG("Construct DownloaderCURL %p", this);
    _impl->hints  = hints;
    _impl->_owner = this;

    _transferDataToBuffer = [this](void* buf, int64_t len) -> int64_t {
        DownloadTaskCURL& coTask = *_currTask;
        int64_t dataLen          = coTask._buf.size();
        if (len < dataLen) {
            return 0;
        }

        memcpy(buf, coTask._buf.data(), dataLen);
        coTask._buf.resize(0);
        return dataLen;
    };
}

DownloaderCURL::~DownloaderCURL() {
    if (_scheduler) {
        _scheduler->unschedule(_schedulerKey, this);
        _scheduler->release();
    }
    _impl->stop();
    DLLOG("Destruct DownloaderCURL %p", this);
}

void DownloaderCURL::startTask(std::shared_ptr<DownloadTask>& task) {
    DownloadTaskCURL* coTask = new (std::nothrow) DownloadTaskCURL(*this);
    coTask->init(task->storagePath, _impl->hints.tempFileNameSuffix);
    task->_coTask.reset(coTask); // coTask auto managed by task

    DLLOG("    DownloaderCURL: createTask: Id(%d)", coTask->serialId);

    _impl->addTask(task, coTask);
    _impl->run();

    if (!task->background) {
        _lazyScheduleUpdate();
        _scheduler->resumeTarget(this);
    }
}

void DownloaderCURL::_lazyScheduleUpdate() {
    if (!_scheduler) {
        _scheduler = Director::getInstance()->getScheduler();
        _scheduler->retain();

        char key[128];
        sprintf(key, "DownloaderCURL(%p)", this);
        _schedulerKey = key;

        _scheduler->schedule(
            std::bind(&DownloaderCURL::_onUpdate, this, std::placeholders::_1), this, 0.1f, true, _schedulerKey);
    }
}

void DownloaderCURL::_onUpdate(float) {
    std::vector<std::shared_ptr<DownloadTask>> tasks;

    // update processing tasks
    _impl->getProcessTasks(tasks);
    for (auto& task : tasks) {
        DownloadTaskCURL& coTask = static_cast<DownloadTaskCURL&>(*task->_coTask);
        std::lock_guard<std::recursive_mutex> lock(coTask._mutex);
        if (coTask._bytesReceived) {
            _currTask = &coTask;
            _updateTaskProgressInfo(*task);
            onTaskProgress(*task, _transferDataToBuffer);
            _currTask             = nullptr;
            coTask._bytesReceived = 0;
        }
    }
    tasks.clear();

    // update finished tasks
    _impl->getFinishedTasks(tasks);
    if (_impl->stopped()) {
        if (_scheduler)
            _scheduler->pauseTarget(this);
    }

    for (auto& task : tasks) {
        _onDownloadFinished(*task);
    }
}

void DownloaderCURL::_updateTaskProgressInfo(DownloadTask& task, int64_t totalExpected) {
    auto& coTask                         = static_cast<DownloadTaskCURL&>(*task._coTask);
    task.progressInfo.bytesReceived      = coTask._bytesReceived;
    task.progressInfo.totalBytesReceived = coTask._totalBytesReceived;
    task.progressInfo.totalBytesExpected = totalExpected < 0 ? coTask._totalBytesExpected : totalExpected;
    task.progressInfo.speedInBytes       = coTask._speed;
}

void DownloaderCURL::_onDownloadFinished(DownloadTask& task, int checkState) {
    auto& coTask = static_cast<DownloadTaskCURL&>(*task._coTask);

    // if there is bytesReceived, call progress update first
    if (coTask._bytesReceived) {
        _currTask = &coTask;
        _updateTaskProgressInfo(task);
        onTaskProgress(task, _transferDataToBuffer);
        coTask._bytesReceived = 0;
        _currTask             = nullptr;
    }

    // if file task, close file handle and rename file if needed
    if (coTask._fs) {
        do {
            auto pFileUtils = FileUtils::getInstance();
            coTask._fs.reset();
            coTask._fsMd5.reset();

            if (checkState & kCheckSumStateSucceed) // No need download
            {
                auto fsOrigin = pFileUtils->openFileStream(coTask._fileName, FileStream::Mode::READ);
                if (fsOrigin) {
                    fsOrigin->seek(0, SEEK_END);
                    _updateTaskProgressInfo(task, fsOrigin->tell());
                    coTask._errCode         = DownloadTask::ERROR_NO_ERROR;
                    coTask._errCodeInternal = 0;
                    coTask._errDescription  = "";

                    pFileUtils->removeFile(coTask._tempFileName);

                    onTaskProgress(task, _transferDataToBuffer);

                    fsOrigin = nullptr;
                } else {
                    coTask._errCode         = DownloadTask::ERROR_ORIGIN_FILE_MISSING;
                    coTask._errCodeInternal = 0;
                    coTask._errDescription  = "Check file md5 succeed, but the origin file is missing!";
                    pFileUtils->removeFile(coTask._checksumFileName);
                    pFileUtils->removeFile(coTask._tempFileName);
                }

                break;
            }

            if (coTask._fileName.empty() || DownloadTask::ERROR_NO_ERROR != coTask._errCode) {
                if (coTask._errCodeInternal == CURLE_RANGE_ERROR) {
                    // If CURLE_RANGE_ERROR, means the server not support resume from download.
                    pFileUtils->removeFile(coTask._checksumFileName);
                    pFileUtils->removeFile(coTask._tempFileName);
                }
                break;
            }

            // if file already exist, remove it
            if (pFileUtils->isFileExistInternal(coTask._fileName)) {
                if (!pFileUtils->removeFile(coTask._fileName)) {
                    coTask._errCode         = DownloadTask::ERROR_REMOVE_FILE_FAILED;
                    coTask._errCodeInternal = errno;
                    coTask._errDescription  = "Can't remove old file: ";
                    coTask._errDescription.append(coTask._fileName);
                    break;
                }
            }


            // Try check sum with md5 digest
            std::string realMd5;
            if (coTask.checkFileMd5(task.checksum, &realMd5) & kCheckSumStateFailed) {
                coTask._errCode         = DownloadTask::ERROR_CHECK_SUM_FAILED;
                coTask._errCodeInternal = 0;
                coTask._errDescription  = StringUtils::format("Check file: %s md5 failed, required:%s, real:%s",
                    coTask._fileName.c_str(), task.checksum.c_str(), realMd5.c_str());

                pFileUtils->removeFile(coTask._checksumFileName);
                pFileUtils->removeFile(coTask._tempFileName);
                break;
            }

            // Rename file work fine.
            if (pFileUtils->renameFile(coTask._tempFileName, coTask._fileName)) {
                // success, remove storage from set
                DownloadTaskCURL::_sStoragePathSet.erase(coTask._tempFileName);
                break;
            }

            // failed
            coTask._errCode         = DownloadTask::ERROR_RENAME_FILE_FAILED;
            coTask._errCodeInternal = 0;
            coTask._errDescription  = "Can't renamefile from: ";
            coTask._errDescription.append(coTask._tempFileName);
            coTask._errDescription.append(" to: ");
            coTask._errDescription.append(coTask._fileName);
        } while (0);
    }

    // needn't lock coTask here, because tasks has removed form _impl
    onTaskFinish(task, coTask._errCode, coTask._errCodeInternal, coTask._errDescription, coTask._buf);
    DLLOG("    DownloaderCURL: finish Task: Id(%d)", coTask.serialId);
}

} // namespace network
} // namespace cocos2d
