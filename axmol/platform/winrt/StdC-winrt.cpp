/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/platform/StdC.h"

namespace ax
{
int AX_DLL gettimeofday(struct timeval* val, struct timezone*)
{
    if (val)
    {
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);

        struct tm tTm;
        tTm.tm_year  = wtm.wYear - 1900;
        tTm.tm_mon   = wtm.wMonth - 1;
        tTm.tm_mday  = wtm.wDay;
        tTm.tm_hour  = wtm.wHour;
        tTm.tm_min   = wtm.wMinute;
        tTm.tm_sec   = wtm.wSecond;
        tTm.tm_isdst = -1;

        val->tv_sec  = (long)mktime(&tTm);  // time_t is 64-bit on win32
        val->tv_usec = wtm.wMilliseconds * 1000;
    }
    return 0;
}
}  // namespace ax
