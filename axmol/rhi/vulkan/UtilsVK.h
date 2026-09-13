/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/PixelBufferDesc.h"
#include <glad/vulkan.h>
#include <utility>

// Helper to log Vulkan error with context
#define VK_LOG_ERROR(mesg, detail) \
    AXLOGE("axmol vulkan: {} - {} in {} {} line {}", mesg, detail, __FILE__, __FUNCTION__, __LINE__)

#define VK_ABORT(mesg)          \
    do                          \
    {                           \
        VK_LOG_ERROR(mesg, ""); \
        abort();                \
    } while (false)

// Boolean expression: return false
#define VK_VERIFY_EXPR(expr, mesg)     \
    do                                 \
    {                                  \
        if (!(expr))                   \
        {                              \
            VK_LOG_ERROR(mesg, #expr); \
            return false;              \
        }                              \
    } while (false)

// Vulkan result: return false
#define VK_VERIFY(vkRet, mesg)                                                                       \
    do                                                                                               \
    {                                                                                                \
        static_assert(!std::is_same_v<bool, decltype(vkRet)>, "The type of vkRet must be VKResult"); \
        if ((vkRet) != VK_SUCCESS)                                                                   \
        {                                                                                            \
            VK_LOG_ERROR(mesg, fmt::format("error code {}", static_cast<int>(vkRet)));               \
            return false;                                                                            \
        }                                                                                            \
    } while (false)

// Boolean expression: abort
#define VK_REQUIRE_EXPR(expr, mesg)    \
    do                                 \
    {                                  \
        if (!(expr))                   \
        {                              \
            VK_LOG_ERROR(mesg, #expr); \
            abort();                   \
        }                              \
    } while (false)

// Vulkan result: abort
#define VK_REQUIRE(vkRet, mesg)                                                                      \
    do                                                                                               \
    {                                                                                                \
        static_assert(!std::is_same_v<bool, decltype(vkRet)>, "The type of vkRet must be VKResult"); \
        if ((vkRet) != VK_SUCCESS)                                                                   \
        {                                                                                            \
            VK_LOG_ERROR(mesg, fmt::format("error code {}", static_cast<int>(vkRet)));               \
            abort();                                                                                 \
        }                                                                                            \
    } while (false)

namespace ax::rhi::vk
{
/**
 * @addtogroup _vk
 * @{
 */

static constexpr auto SWAPCHAIN_IMAGE_USAGE_FLAGS =
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

struct PixelFormatInfo
{
    VkFormat format;
    VkFormat fmtSrv;   // View format for SRV/UAV
    VkFormat fmtDsv;   // View format for DSV
    VkFormat fmtSrgb;  // fmtSrgb
};

inline namespace UtilsVK
{
VkFormat toVkFormat(PixelFormat pf, bool useSrgb = false);

const PixelFormatInfo* toVkFormatInfo(PixelFormat pf);
};  // namespace UtilsVK

/** @} */

}  // namespace ax::rhi::vk
