#pragma once
#include "axmol/base/Data.h"

#include "axmol/rhi/RHITypes.h"

namespace ax::rhi
{

struct PixelBufferDesc
{
    PixelBufferDesc()                       = default;
    PixelBufferDesc(const PixelBufferDesc&) = default;
    PixelBufferDesc(PixelBufferDesc&& rhs) noexcept
        : _data(std::move(rhs._data)), _width(rhs._width), _height(rhs._height)
    {}

    explicit operator bool() const { return !_data.isNull(); }

    Data _data;
    int _width  = 0;
    int _height = 0;
};

}  // namespace ax::rhi
