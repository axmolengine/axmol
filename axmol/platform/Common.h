/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once
/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformMacros.h"
#include "axmol/tlx/bitmask.hpp"
#include <string_view>

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

enum AlertStyle : unsigned int
{
    // Icon types (low bits)
    IconInfo    = 0x0001,
    IconWarning = 0x0002,
    IconError   = 0x0004,
    IconDebug   = 0x0008,

    // Button sets (middle bits)
    Ok          = 0x0100,
    OkCancel    = 0x0200,
    YesNo       = 0x0400,
    YesNoCancel = 0x0800,

    // Behavior flags (high bits)
    RequireSync = 0x10000  ///< Enforce synchronous (blocking) behavior
};
AX_ENABLE_BITMASK_OPS(AlertStyle)

enum class AlertResult
{
    None,
    Ok,
    Cancel,
    Yes,
    No
};

/** @brief Display a modal alert dialog with configurable icon and buttons.
 *
 * @param msg         The message text to display, in UTF-8 encoding.
 * @param title       The dialog title text, in UTF-8 encoding.
 * @param style       Bitwise OR combination of icon type and button set. See ::AlertStyle.
 * @param requireSync When true, enforces synchronous (blocking) behavior for this call.
 *                    On platforms where the default is non-blocking (e.g., UWP), this will
 *                    block the calling thread until the dialog is closed.
 *                    On platforms where the default is already blocking (e.g., Win32),
 *                    this flag has no effect on execution but serves as an explicit
 *                    indication that the caller expects synchronous behavior.
 * @return            The button pressed by the user. See ::AlertResult.
 */
AlertResult AX_DLL showAlert(std::string_view msg, std::string_view title, AlertStyle style = AlertStyle::Ok);

/**
@brief Enum the language type supported now
*/
enum class LanguageType
{
    ENGLISH = 0,
    CHINESE,
    CHINESE_TRADITIONAL,
    FRENCH,
    ITALIAN,
    GERMAN,
    SPANISH,
    DUTCH,
    RUSSIAN,
    KOREAN,
    JAPANESE,
    HUNGARIAN,
    PORTUGUESE,
    ARABIC,
    NORWEGIAN,
    POLISH,
    TURKISH,
    UKRAINIAN,
    ROMANIAN,
    BULGARIAN,
    BELARUSIAN
};

// END of platform group
/// @}

}  // namespace ax

/// @endcond
