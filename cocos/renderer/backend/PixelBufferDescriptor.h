#pragma once
#include "base/CCData.h"

#include "Macros.h"
#include "Types.h"

CC_BACKEND_BEGIN

struct PixelBufferDescriptor
{
    PixelBufferDescriptor() = default;
    PixelBufferDescriptor(const PixelBufferDescriptor&) = default;
    PixelBufferDescriptor(PixelBufferDescriptor&& rhs) : _data(std::move(rhs._data)), _width(rhs._width), _height(rhs._height) {
        
    }
    
    explicit operator bool() const { return !_data.isNull(); }
    
    Data _data;
    int _width = 0;
    int _height = 0;
};

CC_BACKEND_END
