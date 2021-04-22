// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 c4games.com
#pragma once

#include <string>

#include "platform/CCPlatformConfig.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class CC_DLL FileStream {
public:
    FileStream() = default;
    virtual ~FileStream() = default;

    enum class Mode {
        READ,
        WRITE,
        APPEND,
    };

    virtual int close() = 0;

    virtual int seek(long offset, int origin) = 0;
    virtual int read(void* buf, unsigned int size) = 0;

    virtual int write(const void* buf, unsigned int size) = 0;
    virtual int tell() = 0;
    virtual bool isOpen() const = 0;
};

NS_CC_END