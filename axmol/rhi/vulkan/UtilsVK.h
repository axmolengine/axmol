/****************************************************************************
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
#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/PixelBufferDesc.h"
#include <glad/vulkan.h>
#include <utility>

namespace ax::rhi::vk
{
/**
 * @addtogroup _vk
 * @{
 */

struct PixelFormatInfo
{
    VkFormat format;
    VkFormat fmtSrv;   // View format for SRV/UAV
    VkFormat fmtDsv;   // View format for DSV
    VkFormat fmtSrgb;  // fmtSrgb
};

inline namespace UtilsVK
{
VkFormat toVKFormat(PixelFormat pf);

const PixelFormatInfo* toVKFormatInfo(PixelFormat pf);
};  // namespace UtilsVK

/** @} */

}  // namespace ax::rhi::vk
