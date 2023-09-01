// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 C4games Ltd
#pragma once

#include "platform/FileStream.h"
#include "platform/PlatformConfig.h"
#include "base/posix_io.h"
#include <string>
#include <functional>

#include "platform/PlatformMacros.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/FileUtils-android.h"
#    include <jni.h>
#    include <android/asset_manager.h>
#    include <android/asset_manager_jni.h>
#    include "base/ZipUtils.h"
#endif

NS_AX_BEGIN

struct UnzFileStream;
union PXFileHandle
{
    int _fd = -1;
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    AAsset* _asset;
    ZipFileStream _zfs;
#endif
};

struct PXIoF;

class AX_DLL PosixFileStream : public FileStream
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
        _handle = std::move(other._handle);
        _iof    = other._iof;

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

    void* getNativeHandle() const override;

private:
    int internalClose();
    void reset();

    PXFileHandle _handle{};
    const PXIoF* _iof{};
};

NS_AX_END
