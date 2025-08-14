// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md)
#pragma once

#include "platform/IFileStream.h"
#include "platform/PlatformConfig.h"
#include <string>
#include <functional>

#include "platform/PlatformMacros.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include <android/asset_manager.h>
#endif

namespace ax
{

struct ZipEntryInfo;

union PXFileHandle
{
    void* value;
#if defined(_WIN32)
    struct
    {
        osfhnd_t osfh;  // underlying OS file HANDLE
        bool append;    // append mode?
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
    using IFileStream::Mode;

    FileStream() = default;
    ~FileStream() override;

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

    int64_t seek(int64_t offset, int origin) const override;
    int read(void* buf, unsigned int size) const override;
    int write(const void* buf, unsigned int size) const override;
    int64_t size() const override;
    int64_t tell() const override;
    bool resize(int64_t size) const override;
    bool isOpen() const override;

    osfhnd_t nativeHandle() const override;

private:
    int internalClose();
    void reset() { _iof = nullptr; }

    PXFileHandle _handle{};
    const PXIoF* _iof{nullptr};
};

}
