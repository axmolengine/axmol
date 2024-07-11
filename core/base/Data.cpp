/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 
 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/Data.h"
#include "base/Logging.h"

NS_AX_BEGIN

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
    (void)_impl.release_pointer();  // forget internal pointer
    _impl.attach_abi(bytes, size);
}

void Data::clear()
{
    _impl.clear();
    _impl.shrink_to_fit();
}

uint8_t* Data::takeBuffer(ssize_t* size)
{
    auto buffer = getBytes();
    if (size)
        *size = getSize();
    return _impl.release_pointer();
}

NS_AX_END
