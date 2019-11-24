// Copyright (c) 2018-2019 HALX99, TODO: may rename this file to PXFileStream
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
#endif

NS_CC_BEGIN

struct UnzFileStream;
union PXFileHandle {
    int _fd = -1;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    AAsset* _asset;
    UnzFileStream* _uzfs;
#endif
};

struct PXIoF;

class CC_DLL PXFileStream {
public:
    PXFileStream();
    ~PXFileStream();

    enum Mode {
        kModeReadOnly,
        kModeWrite,
        kModeAppend,
    };

    bool open(const std::string& path, int mode = kModeReadOnly);
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