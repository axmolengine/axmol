/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/Data.h"
#include "axmol/base/Logging.h"
#include "axmol/base/Macros.h"

namespace ax
{

const Data Data::Null;

Data::Data()
{
    AXLOGV("In the empty constructor of Data.");
}

Data::Data(Data&& other) : _impl(std::move(other._impl))
{
    AXLOGV("In the move constructor of Data.");
}

Data::Data(const Data& other) : _impl(other._impl)
{
    AXLOGV("In the copy constructor of Data.");
}

Data::~Data()
{
    AXLOGV("deallocing Data: {}", fmt::ptr(this));
    clear();
}

Data& Data::operator=(const Data& other)
{
    if (this != &other)
    {
        AXLOGV("In the copy assignment of Data.");
        _impl = other._impl;
    }
    return *this;
}

Data& Data::operator=(Data&& other)
{
    if (this != &other)
    {
        AXLOGV("In the move assignment of Data.");
        this->_impl = std::move(other._impl);
    }
    return *this;
}

bool Data::isNull() const
{
    return _impl.empty();
}

uint8_t* Data::getBytes() const
{
    return _impl.data();
}

ssize_t Data::getSize() const
{
    return _impl.size();
}

ssize_t Data::copy(const unsigned char* bytes, const ssize_t size)
{
    AXASSERT(size >= 0, "copy size should be non-negative");
    AXASSERT(bytes, "bytes should not be nullptr");
    if (size > 0 && bytes != _impl.data())
        _impl.assign(bytes, bytes + size);
    return _impl.size();
}

uint8_t* Data::resize(ssize_t size)
{
    _impl.resize(size);
    return this->data();
}

void Data::fastSet(uint8_t* bytes, const ssize_t size)
{
    AXASSERT(size >= 0, "fastSet size should be non-negative");
    // AXASSERT(bytes, "bytes should not be nullptr");
    _impl.attach_abi(bytes, size);
}

void Data::clear()
{
    _impl.clear();
}

uint8_t* Data::takeBuffer(ssize_t* size)
{
    auto buffer = getBytes();
    if (size)
        *size = getSize();
    return _impl.detach_abi();
}

}  // namespace ax
