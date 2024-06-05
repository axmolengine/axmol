#pragma once
#include "base/Data.h"

#include "Macros.h"
#include "Types.h"

NS_AX_BACKEND_BEGIN

struct PixelBufferDescriptor
{
    PixelBufferDescriptor()                             = default;
    PixelBufferDescriptor(const PixelBufferDescriptor&) = default;
    PixelBufferDescriptor(PixelBufferDescriptor&& rhs) noexcept
        : _data(std::move(rhs._data)), _width(rhs._width), _height(rhs._height)
    {}

    explicit operator bool() const { return !_data.isNull(); }

    Data _data;
    int _width  = 0;
    int _height = 0;
};

NS_AX_BACKEND_END
