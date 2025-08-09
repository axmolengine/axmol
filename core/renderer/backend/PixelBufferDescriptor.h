#pragma once
#include "base/Data.h"

#include "BaseDefs.h"

namespace ax::backend {

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

}
