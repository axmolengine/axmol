// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md)
#include "platform/FileStream.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/FileUtils-android.h"
#    include "base/ZipUtils.h"
#endif

#include <sys/stat.h>
#include <assert.h>

#if defined(_WIN32)
#    include "ntcvt/ntcvt.hpp"
#else
#    include <unistd.h>
#    include <errno.h>
#    include <fcntl.h>
#    define O_SHARE_FLAGS             (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#    define O_READ_FLAGS              O_RDONLY, 0
#    define O_WRITE_FLAGS             O_CREAT | O_RDWR | O_TRUNC, O_SHARE_FLAGS
#    define O_APPEND_FLAGS            O_APPEND | O_CREAT | O_RDWR, O_SHARE_FLAGS
#    define O_OVERLAP_FLAGS           O_CREAT | O_RDWR, O_SHARE_FLAGS

#    define posix_open_cxx(path, ...) ::open(path.data(), ##__VA_ARGS__)
#    define posix_open                ::open
#    define posix_close               ::close
#    define posix_lseek               ::lseek
#    define posix_read                ::read
#    define posix_write               ::write
#    define posix_fd2fh(fd)           (fd)
#    define posix_ftruncate           ::ftruncate
#    if defined(__APPLE__) || defined(__EMSCRIPTEN__)
#        define posix_lseek64     ::lseek
#        define posix_ftruncate64 ::ftruncate
#    else
#        define posix_lseek64     ::lseek64
#        define posix_ftruncate64 ::ftruncate64
#    endif
#endif

namespace ax
{

struct PXIoF
{
    int (*read)(const PXFileHandle& fh, void*, unsigned int);
    int (*write)(const PXFileHandle& fh, const void*, unsigned int);
    int64_t (*seek)(const PXFileHandle& fh, int64_t, int);
    int (*close)(PXFileHandle& fh);
    int64_t (*size)(const PXFileHandle& fh);
    int (*resize)(const PXFileHandle& fh, int64_t);
};

static int64_t axrt_lowio_seek(const PXFileHandle& fh, int64_t offst, int origin);
struct __axrt_seek_guard
{
    __axrt_seek_guard(const PXFileHandle& fh)
        : place(axrt_lowio_seek(fh, 0, SEEK_CUR)), end(axrt_lowio_seek(fh, 0, SEEK_END)), fhh(fh)
    {}

    ~__axrt_seek_guard() { axrt_lowio_seek(fhh, place, SEEK_SET); }

    __axrt_seek_guard(__axrt_seek_guard const&)            = delete;
    __axrt_seek_guard& operator=(__axrt_seek_guard const&) = delete;

    const PXFileHandle& fhh;
    int64_t place;
    int64_t end;
};

static inline int __axrt_errno_or(int ec)
{
    auto value = errno;
    if (!value)
        errno = value = ec;
    return value;
}

#if defined(_WIN32)
#    define __axrt_truncate_file(fh, size) ::SetEndOfFile(fh)

#    if !defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#        define __axrt_create_file(path, omode, smode, cflags, fflags, fattrs) \
            CreateFileW(path, omode, smode, nullptr, cflags, fflags | fattrs, nullptr)
#    else
static HANDLE __axrt_create_file(LPCWSTR path, DWORD omode, DWORD smode, DWORD cflags, DWORD fflags, DWORD fattrs)
{
    if (!(fflags | fattrs))
        return CreateFile2(path, omode, smode, cflags, nullptr);
    else
    {
        CREATEFILE2_EXTENDED_PARAMETERS params = {0};
        params.dwSize                          = sizeof(params);
        params.dwFileFlags                     = fflags;
        params.dwFileAttributes                = fattrs;
        return CreateFile2(path, omode, smode, cflags, &params);
    }
}
#    endif

#    define _LOWIO_SH_DENYNO (FILE_SHARE_READ | FILE_SHARE_WRITE)
static bool axrt_lowio_open(std::string_view path, IFileStream::Mode mode, PXFileHandle& fh)
{
    static constexpr DWORD __axrt_open_mode2options[][5] = {
        // readonly
        {GENERIC_READ, _LOWIO_SH_DENYNO, OPEN_EXISTING, 0, 0},
        // write
        {GENERIC_READ | GENERIC_WRITE, _LOWIO_SH_DENYNO, CREATE_ALWAYS, 0, FILE_ATTRIBUTE_NORMAL},
        // append
        {GENERIC_READ | GENERIC_WRITE, _LOWIO_SH_DENYNO, OPEN_ALWAYS, 0, FILE_ATTRIBUTE_NORMAL},
        // overlap
        {GENERIC_READ | GENERIC_WRITE, _LOWIO_SH_DENYNO, OPEN_ALWAYS, 0, FILE_ATTRIBUTE_NORMAL},
    };
    auto wFileName  = ntcvt::from_chars(path);
    const auto opts = __axrt_open_mode2options[(int)mode & 0b11];
    fh.osfh         = __axrt_create_file(wFileName.c_str(), opts[0], opts[1], opts[2], opts[3], opts[4]);
    if (fh.osfh != INVALID_HANDLE_VALUE)
    {
        fh.append = mode == IFileStream::Mode::APPEND;
        return true;
    }
    return false;
}

static int axrt_lowio_read(const PXFileHandle& fh, void* buf, unsigned int size)
{
    DWORD dwBytesRead = 0;
    if (::ReadFile(fh.osfh, buf, size, &dwBytesRead, nullptr))
        return static_cast<int>(dwBytesRead);
    errno = EACCES;
    return -1;
}
static int axrt_lowio_write(const PXFileHandle& fh, const void* buf, unsigned int size)
{
    if (fh.append)
        ::SetFilePointer(fh.osfh, 0, 0, FILE_END);
    DWORD dwBytesWrite = 0;
    if (::WriteFile(fh.osfh, buf, size, &dwBytesWrite, nullptr))
        return static_cast<int>(dwBytesWrite);
    errno = EACCES;
    return -1;
}
static int64_t axrt_lowio_seek(const PXFileHandle& fh, int64_t offset, int origin)
{
    LARGE_INTEGER seekpos, newpos;
    seekpos.QuadPart = offset;
    if (SetFilePointerEx(fh.osfh, seekpos, &newpos, origin))
        return newpos.QuadPart;
    errno = EINVAL;
    return -1;
}
static int64_t axrt_lowio_size(const PXFileHandle& fh)
{
    LARGE_INTEGER endpos;
    return ::GetFileSizeEx(fh.osfh, &endpos) ? endpos.QuadPart : 0;
}
static int axrt_lowio_close(PXFileHandle& fh)
{
    if (fh.osfh != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fh.osfh);
        fh.osfh = INVALID_HANDLE_VALUE;
    }
    return 0;
}
#else
#    define __axrt_truncate_file(fh, size) (posix_ftruncate64(fh, size) == 0)
static bool axrt_lowio_open(std::string_view path, IFileStream::Mode mode, PXFileHandle& fh)
{
    static constexpr int __axrt_open_mode2options[][3] = {
        {O_READ_FLAGS},
        {O_WRITE_FLAGS},
        {O_APPEND_FLAGS},
        {O_OVERLAP_FLAGS},
    };
    const auto opts = __axrt_open_mode2options[(int)mode & 0b11];
    fh.osfh         = posix_open_cxx(path, opts[0], opts[1]);
    return fh.osfh != -1;
}

static int axrt_lowio_read(const PXFileHandle& fh, void* buf, unsigned int size)
{
    return static_cast<int>(posix_read(fh.osfh, buf, size));
}
static int axrt_lowio_write(const PXFileHandle& fh, const void* buf, unsigned int size)
{
    return static_cast<int>(posix_write(fh.osfh, buf, size));
}
static int64_t axrt_lowio_seek(const PXFileHandle& fh, int64_t offset, int origin)
{
    return posix_lseek64(fh.osfh, offset, origin);
}
static int64_t axrt_lowio_size(const PXFileHandle& fh)
{
    // Get current file position and seek to end
    __axrt_seek_guard seek_guard(fh);
    return seek_guard.end != -1 ? seek_guard.end : 0;
}
static int axrt_lowio_close(PXFileHandle& fh)
{
    int fd = fh.osfh;
    if (fd != -1)
    {
        fh.osfh = -1;
        return posix_close(fd);
    }
    return 0;
}
#endif

static int axrt_lowio_resize(const PXFileHandle& fh, int64_t size)
{
    // Get current file position and seek to end
    __axrt_seek_guard seek_guard(fh);
    if (seek_guard.place == -1 || seek_guard.end == -1)
    {
        // EINVAL otherwise (ex: too large of a offset)
        return __axrt_errno_or(EINVAL);
    }

    // move pointer to new sizes
    int64_t const new_end = axrt_lowio_seek(fh, size, SEEK_SET);
    if (new_end == -1)
        return errno;

    if (!__axrt_truncate_file(fh.osfh, size))
        return __axrt_errno_or(EACCES);

    // Shorten the file by truncating it, ensure restore place don't grater than new file size
    int64_t const extend = size - seek_guard.end;
    if (extend < 0 && seek_guard.place > new_end)
        seek_guard.place = new_end;

    return 0;
}

static const PXIoF axrt_lowio_iof = {axrt_lowio_read,  axrt_lowio_write, axrt_lowio_seek,
                                     axrt_lowio_close, axrt_lowio_size,  axrt_lowio_resize};

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID

static int axrt_dummy_write(const PXFileHandle& /*fh*/, const void* /*buf*/, unsigned int /*size*/)
{
    return -1;
}
static int axrt_dummy_resize(const PXFileHandle& /*fh*/, int64_t /*newsize*/)
{
    return -1;
}

// android AssetManager wrappers
static int axrt_asset_read(const PXFileHandle& fh, void* buf, unsigned int size)
{
    return AAsset_read(fh.aasset, buf, size);
}
static int64_t axrt_asset_seek(const PXFileHandle& fh, int64_t offset, int origin)
{
    return AAsset_seek(fh.aasset, offset, origin);
}
static int64_t axrt_asset_size(const PXFileHandle& fh)
{
    return AAsset_getLength64(fh.aasset);
}
static int axrt_asset_close(PXFileHandle& fh)
{
    if (fh.aasset != nullptr)
    {
        AAsset_close(fh.aasset);
        fh.aasset = nullptr;
    }
    return 0;
}
static const PXIoF axrt_asset_iof = {axrt_asset_read,  axrt_dummy_write, axrt_asset_seek,
                                     axrt_asset_close, axrt_asset_size,  axrt_dummy_resize};

// android obb
static int axrt_obb_read(const PXFileHandle& fh, void* buf, unsigned int size)
{
    return FileUtilsAndroid::getObbFile()->vread(fh.zentry, buf, size);
}
static int64_t axrt_obb_seek(const PXFileHandle& fh, int64_t offset, int origin)
{
    return FileUtilsAndroid::getObbFile()->vseek(fh.zentry, offset, origin);
}
static int64_t axrt_obb_size(const PXFileHandle& fh)
{
    return FileUtilsAndroid::getObbFile()->vsize(fh.zentry);
}
static int axrt_obb_close(PXFileHandle& fh)
{
    FileUtilsAndroid::getObbFile()->vclose(fh.zentry);
    return 0;
}
static const PXIoF axrt_obb_iof = {axrt_obb_read,  axrt_dummy_write, axrt_obb_seek,
                                   axrt_obb_close, axrt_obb_size,    axrt_dummy_resize};
#endif

FileStream::~FileStream()
{
    internalClose();
}

bool FileStream::open(std::string_view path, IFileStream::Mode mode)
{
#if AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID
    if (axrt_lowio_open(path, mode, _handle))
        this->_iof = &axrt_lowio_iof;
#else  // Android
    if (path[0] != '/')
    {  // from package, always readonly
        size_t position = path.find("assets/");
        // "assets/" is at the beginning of the path and we don't want it
        const auto relativePath = 0 == position ? path.substr(sizeof("assets/") - 1) : path;

        auto obb = FileUtilsAndroid::getObbFile();
        if (obb && (_handle.zentry = obb->vopen(relativePath)))
        {
            this->_iof = &axrt_obb_iof;
        }
        else
        {
            AAssetManager* asMgr = FileUtilsAndroid::getAssetManager();
            _handle.aasset       = AAssetManager_open(asMgr, relativePath.data(), AASSET_MODE_UNKNOWN);
            if (_handle.aasset)
                this->_iof = &axrt_asset_iof;
        }
    }
    else
    {  // otherwise, as a absolutely path
        if (axrt_lowio_open(path, mode, _handle))
            this->_iof = &axrt_lowio_iof;
    }
#endif

    return !!this->_iof;
}

osfhnd_t FileStream::nativeHandle() const
{
    return _iof == &axrt_lowio_iof ? _handle.osfh : (osfhnd_t)-1;
}

int FileStream::internalClose()
{
    if (_iof)
    {
        int ret = _iof->close(_handle);
        _iof    = nullptr;
        return ret;
    }
    return 0;
}

int FileStream::close()
{
    return internalClose();
}

int64_t FileStream::seek(int64_t offset, int origin) const
{
    assert(_iof);
    return _iof->seek(_handle, offset, origin);
}

int FileStream::read(void* buf, unsigned int size) const
{
    assert(_iof);
    return _iof->read(_handle, buf, size);
}

int FileStream::write(const void* buf, unsigned int size) const
{
    assert(_iof);
    return _iof->write(_handle, buf, size);
}

int64_t FileStream::size() const
{
    assert(_iof);
    return _iof->size(_handle);
}

int64_t FileStream::tell() const
{
    assert(_iof);
    return _iof->seek(_handle, 0, SEEK_CUR);
}

bool FileStream::resize(int64_t size) const
{
    assert(_iof);
    return _iof->resize(_handle, size) == 0;
}

bool FileStream::isOpen() const
{
    return !!_iof;
}

}
