// Copyright (c) 2018-2019 HALX99.
// Copyright (c) 2020 c4games.com
#include "platform/CCPosixFileStream.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "base/ZipUtils.h"
#endif

#include <sys/stat.h>
#include <assert.h>

NS_CC_BEGIN

struct PXIoF {
    int(*read)(PXFileHandle& handle, void*, unsigned int);
    long(*seek)(PXFileHandle& handle, long, int);
    int(*close)(PXFileHandle& handle);
};

static int pfs_posix_open(const std::string& path, FileStream::Mode mode, PXFileHandle& handle)
{
    switch (mode) {
    case FileStream::Mode::READ:
        handle._fd = posix_open(path.c_str(), O_READ_FLAGS);
        break;
    case FileStream::Mode::WRITE:
        handle._fd = posix_open(path.c_str(), O_WRITE_FLAGS);
        break;
    case FileStream::Mode::APPEND:
        handle._fd = posix_open(path.c_str(), O_APPEND_FLAGS);
        break;
    default:
        handle._fd = -1;
    }
    return handle._fd;
}

// posix standard wrappers
static int pfs_posix_read(PXFileHandle& handle, void* buf, unsigned int size) { return static_cast<int>(posix_read(handle._fd, buf, size)); }
static long pfs_posix_seek(PXFileHandle& handle, long offst, int origin) { return posix_lseek(handle._fd, offst, origin); }
static int pfs_posix_close(PXFileHandle& handle) {
    int fd = handle._fd;
    if (fd != -1) {
        handle._fd = -1;
        return posix_close(fd);
    }
    return 0;
}
static PXIoF pfs_posix_iof = {
        pfs_posix_read,
        pfs_posix_seek,
        pfs_posix_close
};

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
// android AssetManager wrappers
static int pfs_asset_read(PXFileHandle& handle, void* buf, unsigned int size) { return AAsset_read(handle._asset, buf, size); }
static long pfs_asset_seek(PXFileHandle& handle, long offst, int origin) { return AAsset_seek(handle._asset, offst, origin); }
static int pfs_asset_close(PXFileHandle& handle) {
    if (handle._asset != nullptr) {
        AAsset_close(handle._asset);
        handle._asset = nullptr;
    }
    return 0;
}
static PXIoF pfs_asset_iof = {
        pfs_asset_read,
        pfs_asset_seek,
        pfs_asset_close
};

// android obb
static int pfs_obb_read(PXFileHandle& handle, void* buf, unsigned int size) { return FileUtilsAndroid::getObbFile()->zfread(&handle._zfs, buf, size); }
static long pfs_obb_seek(PXFileHandle& handle, long offset, int origin) { return FileUtilsAndroid::getObbFile()->zfseek(&handle._zfs, offset, origin); }
static int pfs_obb_close(PXFileHandle& handle) {
    FileUtilsAndroid::getObbFile()->zfclose(&handle._zfs);
    return 0;
}
static PXIoF pfs_obb_iof = {
        pfs_obb_read,
        pfs_obb_seek,
        pfs_obb_close
};
#endif

PosixFileStream::~PosixFileStream()
{
    internalClose();
}

bool PosixFileStream::open(const std::string& path, FileStream::Mode mode)
{
    _mode = mode;
    bool ok = false;
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
    ok = pfs_posix_open(path, mode, _handle) != -1;
#else // Android
    if (path[0] != '/') { // from package, always readonly
        std::string relativePath;
        size_t position = path.find("assets/");
        if (0 == position)
        {
            // "assets/" is at the beginning of the path and we don't want it
            relativePath = path.substr(sizeof("assets/") - 1);
        }
        else
        {
            relativePath = path;
        }

        auto obb = FileUtilsAndroid::getObbFile();
        ok = obb != nullptr && obb->zfopen(relativePath, &_handle._zfs);
        if (ok) {
            this->_iof = &pfs_obb_iof;
        }
        else {
            AAssetManager* asMgr = FileUtilsAndroid::getAssetManager();
            AAsset* asset = AAssetManager_open(asMgr, relativePath.c_str(), AASSET_MODE_UNKNOWN);
            ok = !!asset;
            if (ok) {
                _handle._asset = asset;
                // setup file read/seek/close at here
                this->_iof = &pfs_asset_iof;
            }
        }
    }
    else { // otherwise, as a absolutely path
        ok = pfs_posix_open(path, mode, _handle) != -1;
    }
#endif

    if (ok && !_iof)
        _iof = &pfs_posix_iof;

    return ok;
}

bool PosixFileStream::isReadOnly() const
{
    return _mode == FileStream::Mode::READ;
}

int PosixFileStream::internalClose()
{
    if (_iof) {
        int ret = _iof->close(_handle);
        reset();
        return ret;
    }
    return 0;
}

int PosixFileStream::close()
{
    return internalClose();
}

int PosixFileStream::seek(long offset, int origin)
{
    const auto result = _iof->seek(_handle, offset, origin); // this returns -1 for error, and resulting offset on success
    return result < 0 ? -1 : 0; // return 0 for success
}

int PosixFileStream::read(void* buf, unsigned int size)
{
    return _iof->read(_handle, buf, size);
}

int PosixFileStream::write(const void* buf, unsigned int size)
{
    return static_cast<int>(posix_write(_handle._fd, buf, size));
}

int PosixFileStream::tell()
{
    return static_cast<int>(_iof->seek(_handle, 0, SEEK_CUR));
}

bool PosixFileStream::isOpen() const
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
    return _handle._fd != -1;
#else
    return _handle._fd != -1 && _handle._asset != nullptr;
#endif
}

int PosixFileStream::length()
{
    const auto currentPos = tell();
    seek(0, SEEK_END);
    const auto length = tell();
    seek(currentPos, SEEK_SET);

    return length;
}

void PosixFileStream::reset()
{
    memset(&_handle, 0, sizeof(_handle));
    _handle._fd = -1;
    _iof = nullptr;
}

NS_CC_END
