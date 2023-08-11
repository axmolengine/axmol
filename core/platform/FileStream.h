// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 C4games Ltd
#pragma once

#include "platform/IFileStream.h"
#include "platform/PlatformConfig.h"
#include <string>
#include <functional>

#include "platform/PlatformMacros.h"

#if !defined(_WIN32)
#    include <unistd.h>
#    include <errno.h>
#    include <fcntl.h>
#    define O_READ_FLAGS O_RDONLY, 0
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
#    define posix_ftruncate ::ftruncate
#    if defined(__APPLE__)
#        define posix_lseek64 ::lseek
#        define posix_ftruncate64 ::ftruncate
#    else
#        define posix_lseek64 ::lseek64
#        define posix_ftruncate64 ::ftruncate64
#    endif
#endif

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include <android/asset_manager.h>
#endif

NS_AX_BEGIN

struct ZipEntryInfo;

union PXFileHandle
{
    void* value;
#if defined(_WIN32)
    struct
    {
        osfhnd_t osfh;  // underlying OS file HANDLE
        bool append;            // append mode?
    };
#else
    osfhnd_t osfh = -1;  // underlying OS file HANDLE
#endif
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    AAsset* aasset;
    ZipEntryInfo* zentry;
#endif
};

struct PXIoF;

class AX_DLL FileStream : public IFileStream
{
public:
    FileStream() = default;
    virtual ~FileStream();

    FileStream(const FileStream& other) = delete;

    FileStream(FileStream&& other) noexcept
        : IFileStream(std::move(other)), _handle(std::move(other._handle)), _iof(other._iof)
    {
        other.reset();
    }

    FileStream& operator=(const FileStream& other) = delete;

    FileStream& operator=(FileStream&& other) noexcept
    {
        if (this == &other)
            return *this;
        IFileStream::operator=(std::move(other));
        _handle = std::move(other._handle);
        _iof    = other._iof;

        other.reset();

        return *this;
    }

    bool open(std::string_view path, IFileStream::Mode mode) override;
    int close() override;

    int64_t seek(int64_t offset, int origin) override;
    int read(void* buf, unsigned int size) override;
    int write(const void* buf, unsigned int size) override;
    int64_t size() override;
    bool resize(int64_t size) override;
    bool isOpen() const override;

    osfhnd_t nativeHandle() const override;

private:
    int internalClose();
    void reset() { _iof = nullptr; }

    PXFileHandle _handle{};
    const PXIoF* _iof{nullptr};
};

NS_AX_END
