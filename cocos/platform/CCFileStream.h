// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 c4games.com
#pragma once

#include "platform/CCPlatformConfig.h"
#include <string>
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <errno.h>
#endif
#include <fcntl.h>
#include <functional>

#include "platform/CCPlatformMacros.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform/android/CCFileUtils-android.h"
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "base/ZipUtils.h"
#endif

NS_CC_BEGIN

struct UnzFileStream;
union PXFileHandle {
    int _fd = -1;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    AAsset* _asset;
    ZipFileStream _zfs;
#endif
};

struct PXIoF;

class CC_DLL FileStream {
public:
    FileStream();
    ~FileStream();

    enum class Mode {
        READ,
        WRITE,
        APPEND,
    };

    bool open(const std::string& path, Mode mode);
    int close();

    int seek(long offset, int origin);
    int read(void* buf, unsigned int size);

    int write(const void* buf, unsigned int size);

    operator bool() const;

private:
   PXFileHandle _handle;
   const PXIoF* _iof;
};

NS_CC_END