/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Common.h"
#include "axmol/platform/StdC.h"
#include "ntcvt/ntcvt.hpp"

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle style)
{
    std::wstring wsMsg   = ntcvt::from_chars(msg);
    std::wstring wsTitle = ntcvt::from_chars(title);
    UINT flags           = MB_TOPMOST;

    // level
    if (bitmask::any(style, AlertStyle::IconError))
        flags |= MB_ICONERROR;
    else if (bitmask::any(style, AlertStyle::IconWarning))
        flags |= MB_ICONWARNING;
    else if (bitmask::any(style, AlertStyle::IconInfo))
        flags |= MB_ICONINFORMATION;

    // buttons
    if (bitmask::any(style, AlertStyle::OkCancel))
        flags |= MB_OKCANCEL;
    else if (bitmask::any(style, AlertStyle::YesNo))
        flags |= MB_YESNO;
    else if (bitmask::any(style, AlertStyle::YesNoCancel))
        flags |= MB_YESNOCANCEL;
    else if (bitmask::any(style, AlertStyle::Ok))
        flags |= MB_OK;

    ::MessageBoxW(nullptr, wsMsg.c_str(), wsTitle.c_str(), flags);

    return AlertResult::Ok;
}

}  // namespace ax
