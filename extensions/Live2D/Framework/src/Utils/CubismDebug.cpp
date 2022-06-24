/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismDebug.hpp"
#include "CubismFramework.hpp"
#include <stdio.h>
#include <stdarg.h>

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Utils {

void CubismDebug::Print(CubismFramework::Option::LogLevel logLevel, const csmChar* format, ...)
{
    // オプションで設定されたログ出力レベルを下回る場合はログに出さない
    if (logLevel < CubismFramework::GetLoggingLevel())
        return;

    const Core::csmLogFunction logPrint = CubismFramework::CoreLogFunction;

    if (!logPrint)
        return;

    csmChar buffer[256];
    va_list va;
    va_start(va, format);
#ifdef _WINDOWS
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va);
#else
    vsnprintf(buffer, sizeof(buffer), format, va);
#endif
    va_end(va);

    logPrint(buffer);
}

void CubismDebug::DumpBytes(CubismFramework::Option::LogLevel logLevel, const csmUint8* data, csmInt32 length)
{
    for (csmInt32 i = 0; i < length; i++)
    {
        if (i % 16 == 0 && i > 0) Print(logLevel, "\n");
        else if (i % 8 == 0 && i > 0) Print(logLevel, "  ");
        Print(logLevel, "%02X ", (data[i] & 0xFF));
    }

    Print(logLevel, "\n");
}

}}}}

//------------ LIVE2D NAMESPACE ------------
