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
#    include <atomic>
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

namespace ax
{

namespace network
{

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloadContextCURL

class DownloadContextCURL : public IDownloadContext, public std::enable_shared_from_this<DownloadContextCURL>
{
    static int _sSerialId;

    // if more than one task write to one file, cause file broken
    // so use a set to check this situation
    static std::mutex _sStoragePathSetMutex;
    static std::set<std::string> _sStoragePathSet;

public:
    int serialId;
    DownloaderCURL& owner;

    explicit DownloadContextCURL(DownloaderCURL& o) : serialId(++_sSerialId), owner(o)
    {
        AXLOGD("Construct DownloadContextCURL {}, serialId:{}", fmt::ptr(this), serialId);
    }

    virtual ~DownloadContextCURL()
    {
        AXLOGD("Destruct DownloadContextCURL {}, serialId:{}", fmt::ptr(this), serialId);

        if (_errCode != DownloadTask::ERROR_TASK_DUPLICATED)
        {
            auto lock = std::lock_guard<std::mutex>(_sStoragePathSetMutex);

            // if task destroyed unnormally, we should release WritenFileName stored in set.
            // Normally, this action should done when task finished.
            if (_tempFileName.length() && _sStoragePathSet.end() != _sStoragePathSet.find(_tempFileName))
            {
                DownloadContextCURL::_sStoragePathSet.erase(_tempFileName);
            }
        }

        _fs.reset();
        _fsMd5.reset();
    }

    bool init(std::string_view filename, std::string_view tempSuffix)
    {
        // data task
        if (filename.empty())
            return true;

        // file task
        _fileName     = filename;
        _tempFileName = filename;
        _tempFileName.append(tempSuffix);

        {
            auto lock = std::lock_guard<std::mutex>(_sStoragePathSetMutex);

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
        }

        return true;
    }

    bool onStart()
    {
        // data task
        if (_fileName.empty())
            return true;

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
                ::shutdown(this->_sockfd, SD_BOTH);
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
    
    void onSocketClosed(curl_socket_t fd) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (this->_sockfd != -1 && this->_sockfd == fd) {
            this->_sockfd = -1;
        }
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

        curl_easy_getinfo(_curl, CURLINFO_SPEED_DOWNLOAD_T, &_speed);

        return ret;
    }

private:
    friend class DownloaderCURL;

    // for lock object instance
    std::recursive_mutex _mutex;

    // header info
    int64_t _totalBytesExpected = -1; // some server may not send data size, so set it to -1

    curl_off_t _speed = 0;
    CURL* _curl = nullptr;
    curl_socket_t _sockfd = -1;  // store the sockfd to support cancel download manually
    bool _cancelled       = false;

    // progress
    bool _alreadyDownloaded = false;
    int64_t _transferOffset = 0;
    int64_t _bytesReceived = 0;
    int64_t _totalBytesReceived = 0;

    // error
    int _errCode = DownloadTask::ERROR_NO_ERROR;
    int _errCodeInternal = CURLE_OK;
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
};
int DownloadContextCURL::_sSerialId = 0;
std::mutex DownloadContextCURL::_sStoragePathSetMutex;
std::set<std::string> DownloadContextCURL::_sStoragePathSet;

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

    void addTask(std::shared_ptr<DownloadTask> task)
    {
        std::lock_guard<std::mutex> lock(_requestMutex);
        _requestQueue.emplace_back(task);
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
        }

        if (_thread.joinable())
            _thread.join();

        _processSet.clear();
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
    static size_t _outputDataCallbackProc(void* buffer, size_t size, size_t count, DownloadContextCURL* context)
    {
        // AXLOGD("    _outputDataCallbackProc: size({}), count({})", size, count);
        // If your callback function returns CURL_WRITEFUNC_PAUSE it will cause this transfer to become paused.
        return context->writeDataProc((unsigned char*)buffer, size, count);
    }

    static int _progressCallbackProc(DownloadTask* task,
                                     curl_off_t dltotal,
                                     curl_off_t dlnow,
                                     curl_off_t /*ultotal*/,
                                     curl_off_t /*ulnow*/)
    {
        if (!task)
            return -1;
        auto context = static_cast<DownloadContextCURL*>(task->_context.get());
        if (!context)
            return -1;
        if (context->_cancelled)
            return 1;
        if (context->_totalBytesExpected < 0 && dltotal > 0)
            context->_totalBytesExpected = dltotal + context->_transferOffset;
        if (dlnow > 0 && task->background)
        {
            auto& downloaderImpl = context->owner;
            downloaderImpl._updateTaskProgressInfo(*task);
            downloaderImpl.onTaskProgress(*task);
        }
        return 0;
    }

    static curl_socket_t _openSocketCallback(DownloadContextCURL* context, curlsocktype propose, curl_sockaddr* addr)
    {
        auto fd = context->openSocket(propose, addr);
        if (fd != -1)
            context->owner._impl->_contextMap.emplace(fd, context->weak_from_this());
        return fd;
    }
    
    static int _closeSocketCallback(DownloaderCURL::Impl* impl, curl_socket_t fd)
    {
        int ret = ::closesocket(fd);

        auto it = impl->_contextMap.find(fd);
        if (it != impl->_contextMap.end())
        {
            auto context = it->second.lock();
            if (context)
                static_cast<DownloadContextCURL*>(context.get())->onSocketClosed(fd);
            else
                AXLOGD("DownloadContextCURL was destoryed early than closesocket:fd:{}", fd);
            impl->_contextMap.erase(fd);
        }
        
        return ret;
    }

    // this function designed call in work thread
    // the curl handle destroyed in _threadProc
    // handle inited for get header
    CURLcode _initCurlHandleProc(CURL* handle, std::shared_ptr<DownloadTask>& task)
    {
        auto context = static_cast<DownloadContextCURL*>(task->_context.get());

        /* Resolve host domain to ip */
        std::string internalURL = task->requestURL;

        // set url
        curl_easy_setopt(handle, CURLOPT_URL, internalURL.c_str());

        // set xferinfo progress func
        curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, _progressCallbackProc);
        curl_easy_setopt(handle, CURLOPT_XFERINFODATA, task.get());

        // set write func
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _outputDataCallbackProc);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, context);

        curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(handle, CURLOPT_OPENSOCKETFUNCTION, _openSocketCallback);
        curl_easy_setopt(handle, CURLOPT_OPENSOCKETDATA, context);
        
        curl_easy_setopt(handle, CURLOPT_CLOSESOCKETFUNCTION, _closeSocketCallback);
        curl_easy_setopt(handle, CURLOPT_CLOSESOCKETDATA, this);
        
        curl_easy_setopt(handle, CURLOPT_HEADER, 0L);

        /** if server acceptRanges and local has part of file, we continue to download **/
        if (context->_totalBytesReceived > 0)
        {
            char buf[128];
            snprintf(buf, sizeof(buf), "%" PRId64 "-", context->_totalBytesReceived);
            curl_easy_setopt(handle, CURLOPT_RANGE, buf);
            curl_easy_setopt(handle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)context->_totalBytesReceived);
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

        context->_curl = handle;

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
        std::unordered_map<CURL*, std::shared_ptr<DownloadTask>> taskMap;
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

            if (!taskMap.empty())
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

                        auto task = taskMap[curlHandle];

                        /* clean underlaying execution
                        * !!!Note 
                        * the closesocket callback may not invoke immidiately after remove & cleanup handle,
                        * and may invoking delay at next curl_multi_perform or curl_multi_cleanup
                        */

                        // remove from multi-handle
                        curl_multi_remove_handle(curlmHandle, curlHandle);
                        auto context = static_cast<DownloadContextCURL*>(task->_context.get());
                        do
                        {
                            if (CURLE_OK != errCode)
                            {
                                std::string errorMsg = curl_easy_strerror(errCode);
                                if (errCode == CURLE_HTTP_RETURNED_ERROR) {
                                    long responeCode = 0;
                                    curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &responeCode);
                                    fmt::format_to(std::back_inserter(errorMsg), FMT_COMPILE(": {}"), responeCode);
                                }

                                context->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, errCode, std::move(errorMsg));
                                break;
                            }

                            // after get header info success
                            // wrapper.second->_totalBytesReceived inited by local file size
                            // if the local file size equal with the content size from header, the file has
                            // downloaded finish
                            if (context->_totalBytesReceived &&
                                context->_totalBytesReceived == context->_totalBytesExpected)
                            {
                                // the file has download complete
                                // break to move this task to finish queue
                                break;
                            }
                        } while (0);

                        curl_easy_cleanup(curlHandle);
                        AXLOGD("    _threadProc task clean cur handle :{} with errCode:{}, serialId:{}", fmt::ptr(curlHandle),
                               static_cast<int>(errCode), context->serialId);

                        // remove from taskMap
                        taskMap.erase(curlHandle);

                        // remove from _processSet
                        {
                            std::lock_guard<std::mutex> lock(_processMutex);
                            if (_processSet.end() != _processSet.find(task))
                            {
                                _processSet.erase(task);
                            }
                        }

                        finishTask(task);
                    }
                } while (m);
            }

            // process tasks in _requestList
            while (true)
            {
                // Check for set task limit
                if (countOfMaxProcessingTasks && taskMap.size() >= countOfMaxProcessingTasks)
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

                auto context = static_cast<DownloadContextCURL*>(task->_context.get());

                // Init task, open file handles, etc
                if (!context->onStart())
                {
                    finishTask(task);
                    continue;
                }

                // Check if the file has already been downloaded
                int status = context->verifyFileIntegrity(task->checksum);
                if (status & kCheckSumStateSucceed || DownloadTask::ERROR_NO_ERROR != context->_errCode)
                {
                    context->_alreadyDownloaded = status & kCheckSumStateSucceed;
                    finishTask(task);
                    continue;
                }

                // create curl handle from task and add into curl multi handle
                CURL* curlHandle = curl_easy_init();

                if (nullptr == curlHandle)
                {
                    context->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, 0, "Alloc curl handle failed.");
                    finishTask(task);
                    continue;
                }

                // init curl handle for get header info
                _initCurlHandleProc(curlHandle, task);

                // add curl handle to process list
                mcode = curl_multi_add_handle(curlmHandle, curlHandle);
                if (CURLM_OK != mcode)
                {
                    context->setErrorDesc(DownloadTask::ERROR_IMPL_INTERNAL, mcode, curl_multi_strerror(mcode));
                    finishTask(task);
                    continue;
                }

                AXLOGD("    _threadProc task create curl handle:{}", fmt::ptr(curlHandle));
                taskMap[curlHandle] = task;
                std::lock_guard<std::mutex> lock(_processMutex);
                _processSet.insert(task);
            }
        } while (!taskMap.empty());

        _tasksFinished = true;

        curl_multi_cleanup(curlmHandle);
        AXLOGD("----DownloaderCURL::Impl::_threadProc end");
    }

    void finishTask(std::shared_ptr<DownloadTask>& task)
    {
        if (task->background)
        {
            _owner->_onDownloadFinished(*task);
        }
        else
        {
            std::lock_guard<std::mutex> lock(_finishedMutex);
            _finishedQueue.emplace_back(task);
        }
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

    // only access in download thread
    std::map<curl_socket_t, std::weak_ptr<IDownloadContext>> _contextMap;

public:
    DownloaderCURL* _owner = nullptr;
};

////////////////////////////////////////////////////////////////////////////////
//  Implementation DownloaderCURL
DownloaderCURL::DownloaderCURL(const DownloaderHints& hints) : _impl(std::make_shared<Impl>())
{
    AXLOGD("Construct DownloaderCURL {}", fmt::ptr(this));
    _impl->hints  = hints;
    _impl->_owner = this;
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
    auto context   = std::make_shared<DownloadContextCURL>(*this);
    task->_context = context;
    if (context->init(task->storagePath, _impl->hints.tempFileNameSuffix))
    {
        AXLOGD("DownloaderCURL: startTask: Id({})", context->serialId);

        _impl->addTask(task);
        _impl->run();

        if (!task->background)
        {
            _lazyScheduleUpdate();
            _scheduler->resumeTarget(this);
        }
    }
    else
    {
        AXLOGE("DownloaderCURL startTask fail, error: {}, detail: {}", context->_errCode, context->_errDescription);
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
        auto context = static_cast<DownloadContextCURL*>(task->_context.get());
        std::lock_guard<std::recursive_mutex> lock(context->_mutex);
        if (context->_bytesReceived)
        {
            _updateTaskProgressInfo(*task);
            onTaskProgress(*task);
            context->_bytesReceived = 0;
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
    auto context                         = static_cast<DownloadContextCURL*>(task._context.get());
    task.progressInfo.bytesReceived      = context->_bytesReceived;
    task.progressInfo.totalBytesReceived = context->_totalBytesReceived;
    task.progressInfo.totalBytesExpected = totalExpected < 0 ? context->_totalBytesExpected : totalExpected;
    task.progressInfo.speedInBytes       = context->_speed;
}

void DownloaderCURL::_onDownloadFinished(DownloadTask& task)
{
    auto context = static_cast<DownloadContextCURL*>(task._context.get());

    // if there is bytesReceived, call progress update first
    if (context->_bytesReceived)
    {
        _updateTaskProgressInfo(task);
        onTaskProgress(task);
        context->_bytesReceived = 0;
    }

    // if file task, close file handle and rename file if needed
    if (context->_fs)
    {
        do
        {
            auto pFileUtils = FileUtils::getInstance();
            context->_fs.reset();
            context->_fsMd5.reset();

            if (context->_alreadyDownloaded)  // No need to download
            {
                auto fsOrigin = pFileUtils->openFileStream(context->_fileName, IFileStream::Mode::READ);
                if (fsOrigin)
                {
                    fsOrigin->seek(0, SEEK_END);
                    _updateTaskProgressInfo(task, fsOrigin->tell());
                    context->_errCode         = DownloadTask::ERROR_NO_ERROR;
                    context->_errCodeInternal = 0;
                    context->_errDescription  = "";

                    pFileUtils->removeFile(context->_tempFileName);

                    onTaskProgress(task);

                    fsOrigin = nullptr;
                }
                else
                {
                    context->_errCode         = DownloadTask::ERROR_ORIGIN_FILE_MISSING;
                    context->_errCodeInternal = 0;
                    context->_errDescription  = "Check file md5 succeed, but the origin file is missing!";
                    pFileUtils->removeFile(context->_checksumFileName);
                    pFileUtils->removeFile(context->_tempFileName);
                }

                break;
            }

            if (context->_fileName.empty() || context->_errCode != DownloadTask::ERROR_NO_ERROR)
            {
                if (context->_errCodeInternal == CURLE_RANGE_ERROR)
                {
                    // If CURLE_RANGE_ERROR, means the server not support resume from download.
                    pFileUtils->removeFile(context->_checksumFileName);
                    pFileUtils->removeFile(context->_tempFileName);
                }
                break;
            }

            // if file already exist, remove it
            if (pFileUtils->isFileExistInternal(context->_fileName))
            {
                if (!pFileUtils->removeFile(context->_fileName))
                {
                    context->_errCode         = DownloadTask::ERROR_REMOVE_FILE_FAILED;
                    context->_errCodeInternal = errno;
                    context->_errDescription  = "Can't remove old file: ";
                    context->_errDescription.append(context->_fileName);
                    break;
                }
            }

            // Try check sum with md5 digest
            std::string realMd5;
            if (context->verifyFileIntegrity(task.checksum, &realMd5) & kCheckSumStateFailed)
            {
                context->_errCode         = DownloadTask::ERROR_CHECK_SUM_FAILED;
                context->_errCodeInternal = 0;
                context->_errDescription =
                    fmt::format("Check file: {} md5 failed, required:{}, real:{}", context->_fileName,
                                        task.checksum, realMd5);

                pFileUtils->removeFile(context->_checksumFileName);
                pFileUtils->removeFile(context->_tempFileName);
                break;
            }

            // Rename file work fine.
            if (pFileUtils->renameFile(context->_tempFileName, context->_fileName))
            {
                auto lock = std::lock_guard<std::mutex>(DownloadContextCURL::_sStoragePathSetMutex);

                // success, remove storage from set
                DownloadContextCURL::_sStoragePathSet.erase(context->_tempFileName);
                break;
            }

            // failed
            context->_errCode         = DownloadTask::ERROR_RENAME_FILE_FAILED;
            context->_errCodeInternal = 0;
            context->_errDescription  = "Can't renamefile from: ";
            context->_errDescription.append(context->_tempFileName);
            context->_errDescription.append(" to: ");
            context->_errDescription.append(context->_fileName);
        } while (0);
    }

    // needn't lock coTask here, because tasks has removed form _impl
    onTaskFinish(task, context->_errCode, context->_errCodeInternal, context->_errDescription, context->_buf);
    AXLOGD("    DownloaderCURL: finish Task: Id({})", context->serialId);
}

}  // namespace network
}  // namespace ax

#endif
