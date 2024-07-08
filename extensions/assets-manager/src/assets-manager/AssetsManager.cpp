/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "AssetsManager.h"

#include <thread>

#include "base/Director.h"
#include "base/Scheduler.h"
#include "base/UserDefault.h"
#include "network/Downloader.h"
#include "platform/FileUtils.h"

#ifdef MINIZIP_FROM_SYSTEM
#    include <minizip/unzip.h>
#else  // from our embedded sources
#    include "unzip.h"
#endif
#include <ioapi.h>

NS_AX_EXT_BEGIN

using namespace std;
USING_NS_AX;
using namespace ax::network;

#define KEY_OF_VERSION "current-version-code"
#define KEY_OF_DOWNLOADED_VERSION "downloaded-version-code"
#define TEMP_PACKAGE_FILE_NAME "cocos2dx-update-temp-package.zip"
#define BUFFER_SIZE 8192
#define MAX_FILENAME 512

class AssetManagerZipFileInfo
{
public:
    std::string zipFileName{};
};

// unzip overrides to support FileStream
long AssetManager_tell_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
        return -1;

    auto* fs = (FileStream*)stream;

    return fs->tell();
}

long AssetManager_seek_file_func(voidpf opaque, voidpf stream, uint32_t offset, int origin)
{
    if (stream == nullptr)
        return -1;

    auto* fs = (FileStream*)stream;

    return fs->seek(offset, origin) != -1 ? 0 : -1;  // must return 0 for success or -1 for error
}

voidpf AssetManager_open_file_func(voidpf opaque, const char* filename, int mode)
{
    FileStream::Mode fsMode;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
        fsMode = FileStream::Mode::READ;
    else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        fsMode = FileStream::Mode::APPEND;
    else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        fsMode = FileStream::Mode::WRITE;
    else
        return nullptr;

    return FileUtils::getInstance()->openFileStream(filename, fsMode).release();
}

voidpf AssetManager_opendisk_file_func(voidpf opaque, voidpf stream, uint32_t number_disk, int mode)
{
    if (stream == nullptr)
        return nullptr;

    const auto zipFileInfo   = static_cast<AssetManagerZipFileInfo*>(opaque);
    std::string diskFilename = zipFileInfo->zipFileName;

    const auto pos = diskFilename.rfind('.', std::string::npos);

    if (pos != std::string::npos && pos != 0)
    {
        const size_t bufferSize = 5;
        char extensionBuffer[bufferSize];
        snprintf(&extensionBuffer[0], bufferSize, ".z%02u", number_disk + 1);
        diskFilename.replace(pos, std::min((size_t)4, zipFileInfo->zipFileName.size() - pos), extensionBuffer);
        return AssetManager_open_file_func(opaque, diskFilename.c_str(), mode);
    }

    return nullptr;
}

uint32_t AssetManager_read_file_func(voidpf opaque, voidpf stream, void* buf, uint32_t size)
{
    if (stream == nullptr)
        return (uint32_t)-1;

    auto* fs = (FileStream*)stream;
    return fs->read(buf, size);
}

uint32_t AssetManager_write_file_func(voidpf opaque, voidpf stream, const void* buf, uint32_t size)
{
    if (stream == nullptr)
        return (uint32_t)-1;

    auto* fs = (FileStream*)stream;
    return fs->write(buf, size);
}

int AssetManager_close_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
        return -1;

    auto* fs          = (FileStream*)stream;
    const auto result = fs->close();  // 0 for success, -1 for error
    delete fs;
    return result;
}

// THis isn't supported by FileStream, so just check if the stream is null and open
int AssetManager_error_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
    {
        return -1;
    }

    auto* fs = (FileStream*)stream;

    if (fs->isOpen())
    {
        return 0;
    }

    return -1;
}
// End of Overrides

// Implementation of AssetsManager

AssetsManager::AssetsManager(const char* packageUrl /* =nullptr */,
                             const char* versionFileUrl /* =nullptr */,
                             const char* storagePath /* =nullptr */)
    : _storagePath(storagePath ? storagePath : "")
    , _version("")
    , _packageUrl(packageUrl ? packageUrl : "")
    , _versionFileUrl(versionFileUrl ? versionFileUrl : "")
    , _downloadedVersion("")
    , _downloader(new Downloader())
    , _connectionTimeout(0)
    , _delegate(nullptr)
    , _isDownloading(false)
    , _shouldDeleteDelegateWhenExit(false)
{
    checkStoragePath();
    // convert downloader error code to AssetsManager::ErrorCode
    _downloader->onTaskError = [this](const DownloadTask& /*task*/, int errorCode, int /*errorCodeInternal*/,
                                      std::string_view /*errorStr*/) {
        _isDownloading = false;

        if (nullptr == _delegate)
        {
            return;
        }
        auto err = (DownloadTask::ERROR_OPEN_FILE_FAILED == errorCode) ? ErrorCode::CREATE_FILE : ErrorCode::NETWORK;
        _delegate->onError(err);
    };

    // progress callback
    _downloader->onTaskProgress = [this](const DownloadTask& task) {
        if (FileUtils::getInstance()->getFileExtension(task.requestURL) != ".zip")
        {
            // get version progress don't report
            return;
        }

        if (nullptr == _delegate)
        {
            return;
        }

        int percent = task.progressInfo.totalBytesExpected
                          ? int(task.progressInfo.totalBytesReceived * 100 / task.progressInfo.totalBytesExpected)
                          : 0;
        _delegate->onProgress(percent);
        AXLOGD("downloading... {}%", percent);
    };

    // get version from version file when get data success
    _downloader->onDataTaskSuccess = [this](const DownloadTask& /*task*/, std::vector<unsigned char>& data) {
        // store version info to member _version
        const char* p = (char*)data.data();
        _version.insert(_version.end(), p, p + data.size());

        if (getVersion() == _version)
        {
            if (_delegate)
            {
                _delegate->onError(ErrorCode::NO_NEW_VERSION);
            }
            AXLOGD("there is not new version");
            // Set resource search path.
            setSearchPath();
            _isDownloading = false;
            return;
        }

        // start download new version assets
        // 1. Urls of package and version should be valid;
        // 2. Package should be a zip file.
        if (_versionFileUrl.empty() || _packageUrl.empty() ||
            FileUtils::getInstance()->getFileExtension(_packageUrl) != ".zip")
        {
            AXLOGD("no version file url, or no package url, or the package is not a zip file");
            _isDownloading = false;
            return;
        }

        // Is package already downloaded?
        _downloadedVersion = UserDefault::getInstance()->getStringForKey(keyOfDownloadedVersion().c_str());
        if (_downloadedVersion == _version)
        {
            downloadAndUncompress();
            return;
        }

        // start download;
        const string outFileName = _storagePath + TEMP_PACKAGE_FILE_NAME;
        _downloader->createDownloadFileTask(_packageUrl, outFileName);
    };

    // after download package, do uncompress operation
    _downloader->onFileTaskSuccess = [this](const DownloadTask& /*task*/) { downloadAndUncompress(); };
}

AssetsManager::~AssetsManager()
{
    if (_shouldDeleteDelegateWhenExit)
    {
        delete _delegate;
    }
    AX_SAFE_DELETE(_downloader);
}

void AssetsManager::checkStoragePath()
{
    if (!_storagePath.empty() && _storagePath[_storagePath.size() - 1] != '/')
    {
        _storagePath.push_back('/');
    }
}

// Multiple key names
static std::string keyWithHash(const char* prefix, std::string_view url)
{
    char buf[256];
    snprintf(buf, sizeof(buf), "%s%zd", prefix, std::hash<std::string_view>()(url));
    return buf;
}

// hashed version
std::string AssetsManager::keyOfVersion() const
{
    return keyWithHash(KEY_OF_VERSION, _packageUrl);
}

// hashed version
std::string AssetsManager::keyOfDownloadedVersion() const
{
    return keyWithHash(KEY_OF_DOWNLOADED_VERSION, _packageUrl);
}

bool AssetsManager::checkUpdate()
{
    if (_versionFileUrl.empty() || _isDownloading)
        return false;

    // Clear _version before assign new value.
    _version.clear();
    _isDownloading = true;
    _downloader->createDownloadDataTask(_versionFileUrl);
    return true;
}

void AssetsManager::downloadAndUncompress()
{
    std::thread([this]() {
        do
        {
            // Uncompress zip file.
            if (!uncompress())
            {
                Director::getInstance()->getScheduler()->runOnAxmolThread([&, this] {
                    UserDefault::getInstance()->setStringForKey(this->keyOfDownloadedVersion().c_str(), "");
                    UserDefault::getInstance()->flush();
                    if (this->_delegate)
                        this->_delegate->onError(ErrorCode::UNCOMPRESS);
                });
                break;
            }

            Director::getInstance()->getScheduler()->runOnAxmolThread([&, this] {
                // Record new version code.
                UserDefault::getInstance()->setStringForKey(this->keyOfVersion().c_str(), this->_version);

                // Unrecord downloaded version code.
                UserDefault::getInstance()->setStringForKey(this->keyOfDownloadedVersion().c_str(), "");
                UserDefault::getInstance()->flush();

                // Set resource search path.
                this->setSearchPath();

                // Delete unloaded zip file.
                string zipfileName = this->_storagePath + TEMP_PACKAGE_FILE_NAME;
                if (remove(zipfileName.c_str()) != 0)
                {
                    AXLOGD("can not remove downloaded zip file {}", zipfileName);
                }

                if (this->_delegate)
                    this->_delegate->onSuccess();
            });

        } while (0);

        _isDownloading = false;
    }).detach();
}

void AssetsManager::update()
{
    // all operation in checkUpdate, nothing need to do
    // keep this function for compatibility
}

bool AssetsManager::uncompress()
{
    // Open the zip file
    string outFileName = _storagePath + TEMP_PACKAGE_FILE_NAME;

    zlib_filefunc_def_s zipFunctionOverrides;
    fillZipFunctionOverrides(zipFunctionOverrides);

    AssetManagerZipFileInfo zipFileInfo;
    zipFileInfo.zipFileName = outFileName;

    zipFunctionOverrides.opaque = &zipFileInfo;

    // Open the zip file
    unzFile zipfile = unzOpen2(outFileName.c_str(), &zipFunctionOverrides);
    if (!zipfile)
    {
        AXLOGD("can not open downloaded zip file {}", outFileName);
        return false;
    }

    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        AXLOGD("can not read file global info of {}", outFileName);
        unzClose(zipfile);
        return false;
    }

    // Buffer to hold data read from the zip file
    char readBuffer[BUFFER_SIZE];

    AXLOGD("start uncompressing");

    // Loop to extract all files.
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        // Get info about current file.
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile, &fileInfo, fileName, MAX_FILENAME, nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            AXLOGD("can not read file info");
            unzClose(zipfile);
            return false;
        }

        const string fullPath = _storagePath + fileName;

        // Check if this entry is a directory or a file.
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength - 1] == '/')
        {
            // Entry is a directory, so create it.
            // If the directory exists, it will failed silently.
            if (!FileUtils::getInstance()->createDirectory(fullPath))
            {
                AXLOGD("can not create directory {}", fullPath);
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            // There are not directory entry in some case.
            // So we need to test whether the file directory exists when uncompressing file entry
            //, if does not exist then create directory
            const string fileNameStr(fileName);

            size_t startIndex = 0;

            size_t index = fileNameStr.find('/', startIndex);

            while (index != std::string::npos)
            {
                const string dir = _storagePath + fileNameStr.substr(0, index);

                auto fsOut = FileUtils::getInstance()->openFileStream(dir, FileStream::Mode::READ);
                if (!fsOut)
                {
                    if (!FileUtils::getInstance()->createDirectory(dir))
                    {
                        AXLOGD("can not create directory {}", dir);
                        unzClose(zipfile);
                        return false;
                    }
                    else
                    {
                        AXLOGD("create directory {}", dir);
                    }
                }
                else
                {
                    fsOut.reset();
                }

                startIndex = index + 1;

                index = fileNameStr.find('/', startIndex);
            }

            // Entry is a file, so extract it.

            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                AXLOGD("can not open file {}", fileName);
                unzClose(zipfile);
                return false;
            }

            // Create a file to store current file.
            auto fsOut = FileUtils::getInstance()->openFileStream(fullPath, FileStream::Mode::WRITE);
            if (!fsOut)
            {
                AXLOGD("can not open destination file {}", fullPath);
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }

            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0)
                {
                    AXLOGD("can not read zip file {}, error code is {}", fileName, error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    fsOut.reset();
                    return false;
                }

                if (error > 0)
                {
                    fsOut->write(readBuffer, error);
                }
            } while (error > 0);

            fsOut.reset();
        }

        unzCloseCurrentFile(zipfile);

        // Goto next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                AXLOGD("can not read next file");
                unzClose(zipfile);
                return false;
            }
        }
    }

    AXLOGD("end uncompressing");
    unzClose(zipfile);

    return true;
}

void AssetsManager::setSearchPath()
{
    vector<string> searchPaths    = FileUtils::getInstance()->getSearchPaths();
    vector<string>::iterator iter = searchPaths.begin();
    searchPaths.insert(iter, _storagePath);
    FileUtils::getInstance()->setSearchPaths(searchPaths);
}

const char* AssetsManager::getPackageUrl() const
{
    return _packageUrl.c_str();
}

void AssetsManager::setPackageUrl(const char* packageUrl)
{
    _packageUrl = packageUrl;
}

const char* AssetsManager::getStoragePath() const
{
    return _storagePath.c_str();
}

void AssetsManager::setStoragePath(const char* storagePath)
{
    _storagePath = storagePath;
    checkStoragePath();
}

const char* AssetsManager::getVersionFileUrl() const
{
    return _versionFileUrl.c_str();
}

void AssetsManager::setVersionFileUrl(const char* versionFileUrl)
{
    _versionFileUrl = versionFileUrl;
}

std::string_view AssetsManager::getVersion()
{
    return UserDefault::getInstance()->getStringForKey(keyOfVersion().data());
}

void AssetsManager::deleteVersion()
{
    UserDefault::getInstance()->setStringForKey(keyOfVersion().c_str(), "");
}

void AssetsManager::setDelegate(AssetsManagerDelegateProtocol* delegate)
{
    _delegate = delegate;
}

void AssetsManager::setConnectionTimeout(unsigned int timeout)
{
    _connectionTimeout = timeout;
}

unsigned int AssetsManager::getConnectionTimeout()
{
    return _connectionTimeout;
}

AssetsManager* AssetsManager::create(const char* packageUrl,
                                     const char* versionFileUrl,
                                     const char* storagePath,
                                     ErrorCallback errorCallback,
                                     ProgressCallback progressCallback,
                                     SuccessCallback successCallback)
{
    class DelegateProtocolImpl : public AssetsManagerDelegateProtocol
    {
    public:
        DelegateProtocolImpl(ErrorCallback& aErrorCallback,
                             ProgressCallback& aProgressCallback,
                             SuccessCallback& aSuccessCallback)
            : errorCallback(aErrorCallback), progressCallback(aProgressCallback), successCallback(aSuccessCallback)
        {}

        virtual void onError(AssetsManager::ErrorCode errorCode) { errorCallback(int(errorCode)); }
        virtual void onProgress(int percent) { progressCallback(percent); }
        virtual void onSuccess() { successCallback(); }

    private:
        ErrorCallback errorCallback;
        ProgressCallback progressCallback;
        SuccessCallback successCallback;
    };

    auto* manager  = new AssetsManager(packageUrl, versionFileUrl, storagePath);
    auto* delegate = new DelegateProtocolImpl(errorCallback, progressCallback, successCallback);
    manager->setDelegate(delegate);
    manager->_shouldDeleteDelegateWhenExit = true;
    manager->autorelease();
    return manager;
}

void AssetsManager::fillZipFunctionOverrides(zlib_filefunc_def_s& zipFunctionOverrides)
{
    zipFunctionOverrides.zopen_file     = AssetManager_open_file_func;
    zipFunctionOverrides.zopendisk_file = AssetManager_opendisk_file_func;
    zipFunctionOverrides.zread_file     = AssetManager_read_file_func;
    zipFunctionOverrides.zwrite_file    = AssetManager_write_file_func;
    zipFunctionOverrides.ztell_file     = AssetManager_tell_file_func;
    zipFunctionOverrides.zseek_file     = AssetManager_seek_file_func;
    zipFunctionOverrides.zclose_file    = AssetManager_close_file_func;
    zipFunctionOverrides.zerror_file    = AssetManager_error_file_func;
    zipFunctionOverrides.opaque         = nullptr;
}

NS_AX_EXT_END
