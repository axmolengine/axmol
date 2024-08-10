/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#if !defined(__EMSCRIPTEN__)
#    include "network/Downloader-curl.h"

#    include <cinttypes>
#    include <set>

#    include <curl/curl.h>
#    include <thread>
#    include "base/Utils.h"
#    include "base/UTF8.h"
#    include "base/Director.h"
#    include "base/Scheduler.h"
#    include "platform/FileUtils.h"
#    include "network/Downloader.h"
#    include "platform/FileStream.h"
#    include "openssl/md5.h"
#    include "yasio/xxsocket.hpp"
#    include "yasio/thread_name.hpp"

// **NOTE**
// In the file:
// member function with suffix "Proc" designed called in DownloaderCURL::_threadProc
// member function without suffix designed called in main thread
// !!! Don't change the `long` type to `int32_t` at this file, because
// some curl variadic API require explicit number types, please refer to:
//   https://curl.se/libcurl/c/curl_easy_getinfo.html
//   https://curl.se/libcurl/c/curl_easy_setopt.html

#    define AX_CURL_POLL_TIMEOUT_MS 1000

enum
{
    kCheckSumStateSucceed = 1,
    kCheckSumStateFailed  = 1 << 1,
};

NS_AX_BEGIN

namespace network
{

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloadTaskCURL

class DownloadTaskCURL : public IDownloadTask
{
    static int _sSerialId;

    // if more than one task write to one file, cause file broken
    // so use a set to check this situation
    static std::set<std::string> _sStoragePathSet;

public:
    int serialId;
    DownloaderCURL& owner;

    DownloadTaskCURL(DownloaderCURL& o) : serialId(_sSerialId++), owner(o), _requestHeaders(nullptr)

    {
        _initInternal();
        AXLOGD("Construct DownloadTaskCURL {}", fmt::ptr(this));
    }

    virtual ~DownloadTaskCURL()
    {
        if (_errCode != DownloadTask::ERROR_TASK_DUPLICATED)
        {
            // if task destroyed unnormally, we should release WritenFileName stored in set.
            // Normally, this action should done when task finished.
            if (_tempFileName.length() && _sStoragePathSet.end() != _sStoragePathSet.find(_tempFileName))
            {
                DownloadTaskCURL::_sStoragePathSet.erase(_tempFileName);
            }
        }

        _fs.reset();
        _fsMd5.reset();

        if (_requestHeaders)
            curl_slist_free_all(_requestHeaders);

        AXLOGD("Destruct DownloadTaskCURL {}", fmt::ptr(this));
    }

    bool init(std::string_view filename, std::string_view tempSuffix)
    {
        if (0 == filename.length())
        {
            // data task
            _buf.reserve(CURL_MAX_WRITE_SIZE);
            return true;
        }

        // file task
        _fileName     = filename;
        _tempFileName = filename;
        _tempFileName.append(tempSuffix);

        if (_sStoragePathSet.end() != _sStoragePathSet.find(_tempFileName))
        {
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
        do
        {
            std::string dir;
            size_t found = _tempFileName.find_last_of("/\\");
            if (found == std::string::npos)
            {
                _errCode         = DownloadTask::ERROR_INVALID_PARAMS;
                _errCodeInternal = 0;
                _errDescription  = "Can't find dirname in storagePath.";
                break;
            }

            // ensure directory is exist
            dir = _tempFileName.substr(0, found + 1);
            if (!FileUtils::getInstance()->isDirectoryExistInternal(dir))
            {
                if (!FileUtils::getInstance()->createDirectories(dir))
                {
                    _errCode         = DownloadTask::ERROR_CREATE_DIR_FAILED;
                    _errCodeInternal = 0;
                    _errDescription  = "Can't create dir:";
                    _errDescription.append(dir);
                    break;
                }
            }
            // open file
            _fs = FileUtils::getInstance()->openFileStream(_tempFileName, IFileStream::Mode::APPEND);
            if (!_fs)
            {
                _errCode         = DownloadTask::ERROR_OPEN_FILE_FAILED;
                _errCodeInternal = 0;
                _errDescription  = "Can't open file:";
                _errDescription.append(_tempFileName);
                break;
            }
            _totalBytesReceived = _transferOffset = std::max<int64_t>(_fs->size(), 0);

            // init md5 state
            _checksumFileName = _tempFileName + ".digest";

            _fsMd5 = FileUtils::getInstance()->openFileStream(_checksumFileName, IFileStream::Mode::OVERLAPPED);
            if (!_fsMd5)
            {
                _errCode         = DownloadTask::ERROR_OPEN_FILE_FAILED;
                _errCodeInternal = 0;
                _errDescription  = "Can't open checksum file:";
                _errDescription.append(_checksumFileName);
                break;
            }

            _fsMd5->seek(0, SEEK_END);
            if (_fsMd5->tell() != sizeof(_md5State))
            {
                MD5_Init(&_md5State);
            }
            else
            {
                _fsMd5->seek(0, SEEK_SET);
                _fsMd5->read(&_md5State, sizeof(_md5State));
            }
            ret = true;
        } while (0);

        return ret;
    }

    void cancel() override
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (!_cancelled)
        {
            _cancelled = true;
            if (this->_sockfd != -1)
            {
                // may cause curl CURLE_SEND_ERROR(55) or CURLE_RECV_ERROR(56)
                if (::shutdown(this->_sockfd, SD_BOTH) == -1)
                    ::closesocket(this->_sockfd);
                this->_sockfd = -1;
            }
        }
    }

    curl_socket_t openSocket(curlsocktype propose, curl_sockaddr* addr)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (!_cancelled)
        {
            this->_sockfd = ::socket(addr->family, addr->socktype, addr->protocol);
            return this->_sockfd;
        }
        return -1;
    }

    /*
    retval: 0. don't check, 1. check succeed, 2. check failed
    */
    int verifyFileIntegrity(std::string_view requiredsum, std::string* outsum = nullptr)
    {
        int status = 0;
        if (!requiredsum.empty())
        {
            std::string digest(16, '\0');
            auto state = _md5State;  // Excellent, make a copy, don't modify the origin state.
            MD5_Final((uint8_t*)&digest.front(), &state);
            auto checksum = utils::bin2hex(digest);
            status        = requiredsum == checksum ? kCheckSumStateSucceed : kCheckSumStateFailed;

            if (outsum != nullptr)
                *outsum = std::move(checksum);
        }
        return status;
    }

    void setErrorDesc(int code, int codeInternal, std::string&& desc)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _errCode         = code;
        _errCodeInternal = codeInternal;
        _errDescription  = std::move(desc);
    }

    size_t writeDataProc(unsigned char* buffer, size_t size, size_t count)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        size_t ret = 0;

        auto bytes_transferred = size * count;

        if (_fs)
        {
            ret = _fs->write(buffer, static_cast<unsigned int>(bytes_transferred));
        }
        else
        {
            ret          = bytes_transferred;
            auto cap     = _buf.capacity();
            auto bufSize = _buf.size();
            if (cap < bufSize + ret)
            {
                _buf.reserve(bufSize * 2);
            }
            _buf.insert(_buf.end(), buffer, buffer + ret);
        }
        if (ret > 0)
        {
            _bytesReceived += ret;
            _totalBytesReceived += ret;

            if (_fsMd5)
            {
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
    int64_t _totalBytesExpected;

    double _speed;
    CURL* _curl;
    curl_socket_t _sockfd = -1;  // store the sockfd to support cancel download manually
    bool _cancelled       = false;

    curl_slist* _requestHeaders;

    // progress
    int64_t _transferOffset;
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
    std::unique_ptr<IFileStream> _fs{};

    // calculate md5 in downloading time support
    std::unique_ptr<IFileStream> _fsMd5{};  // store md5 state realtime
    MD5state_st _md5State;

    void _initInternal()
    {
        _bytesReceived      = (0);
        _totalBytesReceived = (0);
        _totalBytesExpected = (-1);  // some server may not replay data size, so set it to -1
        _speed              = 0;
        _curl               = nullptr;
        _errCode            = (DownloadTask::ERROR_NO_ERROR);
        _errCodeInternal    = (CURLE_OK);
    }
};
int DownloadTaskCURL::_sSerialId;
std::set<std::string> DownloadTaskCURL::_sStoragePathSet;

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloaderCURL::Impl
// This class shared by DownloaderCURL and work thread.
class DownloaderCURL::Impl : public std::enable_shared_from_this<DownloaderCURL::Impl>
{
public:
    DownloaderHints hints;

    Impl()
    //        : _thread(nullptr)
    {
        AXLOGD("Construct DownloaderCURL::Impl {}", fmt::ptr(this));
    }

    ~Impl() { AXLOGD("Destruct DownloaderCURL::Impl {}", fmt::ptr(this)); }

    void addTask(std::shared_ptr<DownloadTask> task, DownloadTaskCURL* coTask)
    {
        int status = coTask->verifyFileIntegrity(task->checksum);

        if (status & kCheckSumStateSucceed || DownloadTask::ERROR_NO_ERROR != coTask->_errCode)
        {
            _owner->_onDownloadFinished(*task, status);
        }
        else
        {
            std::lock_guard<std::mutex> lock(_requestMutex);
            _requestQueue.emplace_back(task);
        }
    }

    void run()
    {
        std::lock_guard<std::mutex> lock(_threadMutex);

        if (_tasksFinished)
        {  // all tasks finished, make sure thread not joinable
            if (_thread.joinable())
                _thread.join();
            _tasksFinished = false;
        }

        if (!_thread.joinable())
            _thread = std::thread(&DownloaderCURL::Impl::_threadProc, this);
    }

    void stop()
    {  // make sure all task exit properly
        if (!_requestQueue.empty())
        {
            std::lock_guard<std::mutex> lock(_requestMutex);
            _requestQueue.clear();
        }

        if (!_processSet.empty())
        {
            std::lock_guard<std::mutex> lock(_processMutex);
            for (auto&& task : _processSet)
                task->cancel();
            _processSet.clear();
        }

        if (_thread.joinable())
            _thread.join();
    }

    bool stopped() const
    {
        std::lock_guard<std::mutex> lock(_threadMutex);
        return false == _thread.joinable() ? true : false;
    }

    // Gets non background task list
    void getProcessTasks(std::vector<std::shared_ptr<DownloadTask>>& outList)
    {
        std::lock_guard<std::mutex> lock(_processMutex);
        outList.reserve(_processSet.size());
        for (auto&& task : _processSet)
        {
            if (!task->background)
                outList.emplace_back(task);
        }
    }

    void getFinishedTasks(std::vector<std::shared_ptr<DownloadTask>>& outList)
    {
        std::lock_guard<std::mutex> lock(_finishedMutex);
        outList.reserve(_finishedQueue.size());
        outList.insert(outList.end(), _finishedQueue.begin(), _finishedQueue.end());
        _finishedQueue.clear();
    }

private:
    static size_t _outputDataCallbackProc(void* buffer, size_t size, size_t count, DownloadTaskCURL* coTask)
    {
        // AXLOGD("    _outputDataCallbackProc: size({}), count({})", size, count);
        // If your callback function returns CURL_WRITEFUNC_PAUSE it will cause this transfer to become paused.
        return coTask->writeDataProc((unsigned char*)buffer, size, count);
    }

    static int _progressCallbackProc(DownloadTask* task,
                                     curl_off_t dltotal,
                                     curl_off_t dlnow,
                                     curl_off_t /*ultotal*/,
                                     curl_off_t /*ulnow*/)
    {
        if (!task)
            return -1;
        auto coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());
        if (!coTask)
            return -1;
        if (coTask->_cancelled)
            return 1;
        if (coTask->_totalBytesExpected < 0 && dltotal > 0)
            coTask->_totalBytesExpected = dltotal + coTask->_transferOffset;
        if (dlnow > 0 && task->background)
        {
            auto& downloaderImpl = coTask->owner;
            downloaderImpl._updateTaskProgressInfo(*task);
            downloaderImpl.onTaskProgress(*task, downloaderImpl._transferDataToBuffer);
        }
        return 0;
    }

    static curl_socket_t _openSocketCallback(DownloadTaskCURL& pTask, curlsocktype propose, curl_sockaddr* addr)
    {
        return pTask.openSocket(propose, addr);
    }

    // this function designed call in work thread
    // the curl handle destroyed in _threadProc
    // handle inited for get header
    CURLcode _initCurlHandleProc(CURL* handle, std::shared_ptr<DownloadTask>& task)
    {
        DownloadTaskCURL* coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());

        /* Resolve host domain to ip */
        std::string internalURL = task->requestURL;
        // Curl_custom_setup(handle, internalURL, (void**)& coTask->_requestHeaders);

        // set url
        curl_easy_setopt(handle, CURLOPT_URL, internalURL.c_str());

        // set xferinfo progress func
        curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, _progressCallbackProc);
        curl_easy_setopt(handle, CURLOPT_XFERINFODATA, task.get());

        // set write func
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _outputDataCallbackProc);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, coTask);

        curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(handle, CURLOPT_OPENSOCKETFUNCTION, _openSocketCallback);
        curl_easy_setopt(handle, CURLOPT_OPENSOCKETDATA, coTask);
        curl_easy_setopt(handle, CURLOPT_HEADER, 0L);

        /** if server acceptRanges and local has part of file, we continue to download **/
        if (coTask->_totalBytesReceived > 0)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "%" PRId64 "-", coTask->_totalBytesReceived);
            curl_easy_setopt(handle, CURLOPT_RANGE, buf);
            curl_easy_setopt(handle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)coTask->_totalBytesReceived);
        }

        if (hints.timeoutInSeconds)
        {
            curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, hints.timeoutInSeconds);
        }

        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, 10L);

        if (task->cacertPath.empty())
        {
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
        }
        else
        {
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(handle, CURLOPT_CAINFO, task->cacertPath.c_str());
        }

        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 5L);

        coTask->_curl = handle;

        return CURLE_OK;
    }

    void _threadProc()
    {
        yasio::set_thread_name("axmol-dl");

        AXLOGD("++++DownloaderCURL::Impl::_threadProc begin {}", fmt::ptr(this));
        // the holder prevent DownloaderCURL::Impl class instance be destruct in main thread
        auto holder                        = this->shared_from_this();
        auto thisThreadId                  = std::this_thread::get_id();
        uint32_t countOfMaxProcessingTasks = this->hints.countOfMaxProcessingTasks;
        // init curl content
        CURLM* curlmHandle = curl_multi_init();
        std::unordered_map<CURL*, std::shared_ptr<DownloadTask>> coTaskMap;
        int runningHandles = 0;
        CURLMcode mcode    = CURLM_OK;
        int rc             = 0;  // select return code

        do
        {
            // check the thread should exit or not
            {
                std::lock_guard<std::mutex> lock(_threadMutex);
                // if the Impl stoped, this->_thread.reset will be called, thus _thread.get_id() not equal with
                // thisThreadId
                if (thisThreadId != this->_thread.get_id())
                {
                    break;
                }
            }

            if (runningHandles)
            {
                int nret = 0;
                rc       = curl_multi_poll(curlmHandle, nullptr, 0, AX_CURL_POLL_TIMEOUT_MS, &nret);
                if (rc < 0)
                {
                    AXLOGD("    _threadProc: select return unexpect code: {}", rc);
                }
            }

            if (!coTaskMap.empty())
            {
                mcode = CURLM_CALL_MULTI_PERFORM;
                while (CURLM_CALL_MULTI_PERFORM == mcode)
                {
                    mcode = curl_multi_perform(curlmHandle, &runningHandles);
                }
                if (CURLM_OK != mcode)
                {
                    break;
                }

                struct CURLMsg* m;
                do
                {
                    int msgq = 0;
                    m        = curl_multi_info_read(curlmHandle, &msgq);
                    if (m && (m->msg == CURLMSG_DONE))
                    {
                        CURL* curlHandle = m->easy_handle;
                        CURLcode errCode = m->data.result;

                        auto task = coTaskMap[curlHandle];

                        // remove from multi-handle
                        curl_multi_remove_handle(curlmHandle, curlHandle);
                        do
                        {
                            auto coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());
                            if (CURLE_OK != errCode)
                            {
                                std::string errorMsg = curl_easy_strerror(errCode);
                                if (errCode == CURLE_HTTP_RETURNED_ERROR) {
                                    long responeCode = 0;
                                    curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &responeCode);
                                    fmt::format_to(std::back_inserter(errorMsg), FMT_COMPILE(": {}"), responeCode);
                                }

                                coTask->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, errCode, std::move(errorMsg));
                                break;
                            }

                            // after get header info success
                            // wrapper.second->_totalBytesReceived inited by local file size
                            // if the local file size equal with the content size from header, the file has
                            // downloaded finish
                            if (coTask->_totalBytesReceived &&
                                coTask->_totalBytesReceived == coTask->_totalBytesExpected)
                            {
                                // the file has download complete
                                // break to move this task to finish queue
                                break;
                            }
                        } while (0);

                        curl_easy_cleanup(curlHandle);
                        AXLOGD("    _threadProc task clean cur handle :{} with errCode:{}", fmt::ptr(curlHandle),
                               static_cast<int>(errCode));

                        // remove from coTaskMap
                        coTaskMap.erase(curlHandle);

                        // remove from _processSet
                        {
                            std::lock_guard<std::mutex> lock(_processMutex);
                            if (_processSet.end() != _processSet.find(task))
                            {
                                _processSet.erase(task);
                            }
                        }

                        if (task->background)
                            _owner->_onDownloadFinished(*task);
                        else
                        {
                            std::lock_guard<std::mutex> lock(_finishedMutex);
                            _finishedQueue.emplace_back(task);
                        }
                    }
                } while (m);
            }

            // process tasks in _requestList
            while (true)
            {
                // Check for set task limit
                if (countOfMaxProcessingTasks && coTaskMap.size() >= countOfMaxProcessingTasks)
                    break;

                // get task wrapper from request queue
                std::shared_ptr<DownloadTask> task;
                {
                    std::lock_guard<std::mutex> lock(_requestMutex);
                    if (!_requestQueue.empty())
                    {
                        task = _requestQueue.front();
                        _requestQueue.pop_front();
                    }
                }

                // if request queue is empty, the wrapper.first is nullptr
                if (!task)
                {
                    break;
                }

                auto coTask = static_cast<DownloadTaskCURL*>(task->_coTask.get());
                // create curl handle from task and add into curl multi handle
                CURL* curlHandle = curl_easy_init();

                if (nullptr == curlHandle)
                {
                    coTask->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, 0, "Alloc curl handle failed.");
                    _owner->_onDownloadFinished(*task);
                    continue;
                }

                // init curl handle for get header info
                _initCurlHandleProc(curlHandle, task);

                // add curl handle to process list
                mcode = curl_multi_add_handle(curlmHandle, curlHandle);
                if (CURLM_OK != mcode)
                {
                    coTask->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, mcode, curl_multi_strerror(mcode));
                    _owner->_onDownloadFinished(*task);
                    continue;
                }

                AXLOGD("    _threadProc task create curl handle:{}", fmt::ptr(curlHandle));
                coTaskMap[curlHandle] = task;
                std::lock_guard<std::mutex> lock(_processMutex);
                _processSet.insert(task);
            }
        } while (!coTaskMap.empty());

        _tasksFinished = true;

        curl_multi_cleanup(curlmHandle);
        AXLOGD("----DownloaderCURL::Impl::_threadProc end");
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
DownloaderCURL::DownloaderCURL(const DownloaderHints& hints) : _impl(std::make_shared<Impl>()), _currTask(nullptr)
{
    AXLOGD("Construct DownloaderCURL {}", fmt::ptr(this));
    _impl->hints  = hints;
    _impl->_owner = this;

    _transferDataToBuffer = [this](void* buf, int64_t len) -> int64_t {
        DownloadTaskCURL& coTask = *_currTask;
        int64_t dataLen          = coTask._buf.size();
        if (len < dataLen)
        {
            return 0;
        }

        memcpy(buf, coTask._buf.data(), dataLen);
        coTask._buf.resize(0);
        return dataLen;
    };
}

DownloaderCURL::~DownloaderCURL()
{
    if (_scheduler)
    {
        _scheduler->unschedule(_schedulerKey, this);
        _scheduler->release();
    }
    _impl->stop();
    AXLOGD("Destruct DownloaderCURL {}", fmt::ptr(this));
}

void DownloaderCURL::startTask(std::shared_ptr<DownloadTask>& task)
{
    DownloadTaskCURL* coTask = new DownloadTaskCURL(*this);
    task->_coTask.reset(coTask);  // coTask auto managed by task
    if (coTask->init(task->storagePath, _impl->hints.tempFileNameSuffix))
    {
        AXLOGD("DownloaderCURL: createTask: Id({})", coTask->serialId);

        _impl->addTask(task, coTask);
        _impl->run();

        if (!task->background)
        {
            _lazyScheduleUpdate();
            _scheduler->resumeTarget(this);
        }
    }
    else
    {
        AXLOGE("DownloaderCURL createTask fail, error: {}, detail: {}", coTask->_errCode, coTask->_errDescription);
        task.reset();
    }
}

void DownloaderCURL::_lazyScheduleUpdate()
{
    if (!_scheduler)
    {
        _scheduler = Director::getInstance()->getScheduler();
        _scheduler->retain();

        char key[128];
        snprintf(key, sizeof(key), "DownloaderCURL(%p)", this);
        _schedulerKey = key;

        _scheduler->schedule(std::bind(&DownloaderCURL::_onUpdate, this, std::placeholders::_1), this, 0.1f, true,
                             _schedulerKey);
    }
}

void DownloaderCURL::_onUpdate(float)
{
    std::vector<std::shared_ptr<DownloadTask>> tasks;

    // update processing tasks
    _impl->getProcessTasks(tasks);
    for (auto&& task : tasks)
    {
        DownloadTaskCURL& coTask = static_cast<DownloadTaskCURL&>(*task->_coTask);
        std::lock_guard<std::recursive_mutex> lock(coTask._mutex);
        if (coTask._bytesReceived)
        {
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
    if (_impl->stopped())
    {
        if (_scheduler)
            _scheduler->pauseTarget(this);
    }

    for (auto&& task : tasks)
    {
        _onDownloadFinished(*task);
    }
}

void DownloaderCURL::_updateTaskProgressInfo(DownloadTask& task, int64_t totalExpected)
{
    auto& coTask                         = static_cast<DownloadTaskCURL&>(*task._coTask);
    task.progressInfo.bytesReceived      = coTask._bytesReceived;
    task.progressInfo.totalBytesReceived = coTask._totalBytesReceived;
    task.progressInfo.totalBytesExpected = totalExpected < 0 ? coTask._totalBytesExpected : totalExpected;
    task.progressInfo.speedInBytes       = coTask._speed;
}

void DownloaderCURL::_onDownloadFinished(DownloadTask& task, int checkState)
{
    auto& coTask = static_cast<DownloadTaskCURL&>(*task._coTask);

    // if there is bytesReceived, call progress update first
    if (coTask._bytesReceived)
    {
        _currTask = &coTask;
        _updateTaskProgressInfo(task);
        onTaskProgress(task, _transferDataToBuffer);
        coTask._bytesReceived = 0;
        _currTask             = nullptr;
    }

    // if file task, close file handle and rename file if needed
    if (coTask._fs)
    {
        do
        {
            auto pFileUtils = FileUtils::getInstance();
            coTask._fs.reset();
            coTask._fsMd5.reset();

            if (checkState & kCheckSumStateSucceed)  // No need download
            {
                auto fsOrigin = pFileUtils->openFileStream(coTask._fileName, IFileStream::Mode::READ);
                if (fsOrigin)
                {
                    fsOrigin->seek(0, SEEK_END);
                    _updateTaskProgressInfo(task, fsOrigin->tell());
                    coTask._errCode         = DownloadTask::ERROR_NO_ERROR;
                    coTask._errCodeInternal = 0;
                    coTask._errDescription  = "";

                    pFileUtils->removeFile(coTask._tempFileName);

                    onTaskProgress(task, _transferDataToBuffer);

                    fsOrigin = nullptr;
                }
                else
                {
                    coTask._errCode         = DownloadTask::ERROR_ORIGIN_FILE_MISSING;
                    coTask._errCodeInternal = 0;
                    coTask._errDescription  = "Check file md5 succeed, but the origin file is missing!";
                    pFileUtils->removeFile(coTask._checksumFileName);
                    pFileUtils->removeFile(coTask._tempFileName);
                }

                break;
            }

            if (coTask._fileName.empty() || coTask._errCode != DownloadTask::ERROR_NO_ERROR)
            {
                if (coTask._errCodeInternal == CURLE_RANGE_ERROR)
                {
                    // If CURLE_RANGE_ERROR, means the server not support resume from download.
                    pFileUtils->removeFile(coTask._checksumFileName);
                    pFileUtils->removeFile(coTask._tempFileName);
                }
                break;
            }

            // if file already exist, remove it
            if (pFileUtils->isFileExistInternal(coTask._fileName))
            {
                if (!pFileUtils->removeFile(coTask._fileName))
                {
                    coTask._errCode         = DownloadTask::ERROR_REMOVE_FILE_FAILED;
                    coTask._errCodeInternal = errno;
                    coTask._errDescription  = "Can't remove old file: ";
                    coTask._errDescription.append(coTask._fileName);
                    break;
                }
            }

            // Try check sum with md5 digest
            std::string realMd5;
            if (coTask.verifyFileIntegrity(task.checksum, &realMd5) & kCheckSumStateFailed)
            {
                coTask._errCode         = DownloadTask::ERROR_CHECK_SUM_FAILED;
                coTask._errCodeInternal = 0;
                coTask._errDescription =
                    fmt::format("Check file: {} md5 failed, required:{}, real:{}", coTask._fileName,
                                        task.checksum, realMd5);

                pFileUtils->removeFile(coTask._checksumFileName);
                pFileUtils->removeFile(coTask._tempFileName);
                break;
            }

            // Rename file work fine.
            if (pFileUtils->renameFile(coTask._tempFileName, coTask._fileName))
            {
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
    AXLOGD("    DownloaderCURL: finish Task: Id({})", coTask.serialId);
}

}  // namespace network
NS_AX_END  // namespace ax

#endif
