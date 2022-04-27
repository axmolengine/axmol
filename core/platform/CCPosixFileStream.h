// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 C4games Ltd
#pragma once

#include "platform/CCFileStream.h"
#include "platform/CCPlatformConfig.h"
#include <string>
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#    include <io.h>
#    include <direct.h>
#else
#    include <unistd.h>
#    include <errno.h>
#endif
#include <fcntl.h>
#include <functional>

#include "platform/CCPlatformMacros.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#    include "platform/android/CCFileUtils-android.h"
#    include <jni.h>
#    include <android/asset_manager.h>
#    include <android/asset_manager_jni.h>
#    include "base/ZipUtils.h"
#endif

#if defined(_WIN32)
#    include "ntcvt/ntcvt.hpp"
#    define O_READ_FLAGS O_BINARY | O_RDONLY, S_IREAD
#    define O_WRITE_FLAGS O_CREAT | O_RDWR | O_BINARY | O_TRUNC, S_IWRITE | S_IREAD
#    define O_APPEND_FLAGS O_APPEND | O_CREAT | O_RDWR | O_BINARY, S_IWRITE | S_IREAD

#    define O_OVERLAP_FLAGS O_CREAT | O_RDWR | O_BINARY, S_IWRITE | S_IREAD
#    define posix_open_cxx(path, ...) ::_wopen(ntcvt::from_chars(path).c_str(), ##__VA_ARGS__)
#    define posix_open(path, ...) ::_wopen(ntcvt::from_chars(path).c_str(), ##__VA_ARGS__)
#    define posix_close ::_close
#    define posix_lseek ::_lseek
#    define posix_lseek64 ::_lseeki64
#    define posix_read ::_read
#    define posix_write ::_write
#    define posix_fd2fh(fd) reinterpret_cast<HANDLE>(_get_osfhandle(fd))
#    define posix_fsetsize(fd, size) ::_chsize(fd, size)
#else

#    if defined(__APPLE__)
#        define posix_lseek64 ::lseek
#    else
#        define posix_lseek64 ::lseek64
#    endif

#    define O_READ_FLAGS O_RDONLY
#    define O_WRITE_FLAGS O_CREAT | O_RDWR | O_TRUNC, S_IRWXU
#    define O_APPEND_FLAGS O_APPEND | O_CREAT | O_RDWR, S_IRWXU

#    define O_OVERLAP_FLAGS O_CREAT | O_RDWR, S_IRWXU
#    define posix_open_cxx(path, ...) ::open(path.data(), ##__VA_ARGS__)
#    define posix_open ::open
#    define posix_close ::close
#    define posix_lseek ::lseek
#    define posix_read ::read
#    define posix_write ::write
#    define posix_fd2fh(fd) (fd)
#    define posix_fsetsize(fd, size) (::ftruncate(fd, size), ::lseek(fd, 0, SEEK_SET))
#endif

NS_CC_BEGIN

struct UnzFileStream;
union PXFileHandle
{
    int _fd = -1;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    AAsset* _asset;
    ZipFileStream _zfs;
#endif
};

struct PXIoF;

class CC_DLL PosixFileStream : public FileStream
{
public:
    PosixFileStream() = default;
    virtual ~PosixFileStream();

    PosixFileStream(const PosixFileStream& other) = delete;

    PosixFileStream(PosixFileStream&& other) noexcept
        : FileStream(std::move(other)), _handle(std::move(other._handle)), _iof(other._iof)
    {
        other.reset();
    }

    PosixFileStream& operator=(const PosixFileStream& other) = delete;

    PosixFileStream& operator=(PosixFileStream&& other) noexcept
    {
        if (this == &other)
            return *this;
        FileStream::operator=(std::move(other));
        _handle             = std::move(other._handle);
        _iof                = other._iof;

        other.reset();

        return *this;
    }

    enum class Mode
    {
        READ,
        WRITE,
        APPEND,
    };

    bool open(std::string_view path, FileStream::Mode mode) override;
    int close() override;

    int seek(int64_t offset, int origin) override;
    int read(void* buf, unsigned int size) override;
    int write(const void* buf, unsigned int size) override;
    int64_t tell() override;
    int64_t size() override;
    bool isOpen() const override;

private:
    int internalClose();
    void reset();

    PXFileHandle _handle{};
    const PXIoF* _iof{};
};

NS_CC_END
