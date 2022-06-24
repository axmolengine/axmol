/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

#ifdef CSM_DEBUG
#include "assert.h"
#define CSM_ASSERT(expr)    assert(expr)
#else
#define CSM_ASSERT(expr)
#endif

#define CubismLogPrint(level, fmt, ...)         Live2D::Cubism::Framework::Utils::CubismDebug::Print(level,  "[CSM]" fmt, ## __VA_ARGS__)
#define CubismLogPrintln(level, fmt, ...)       CubismLogPrint(level, fmt "\n", ## __VA_ARGS__)

#if CSM_LOG_LEVEL <= CSM_LOG_LEVEL_VERBOSE
#define CubismLogVerbose(fmt, ...)    CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Verbose, "[V]" fmt, ## __VA_ARGS__)
#define CubismLogDebug(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Debug,   "[D]" fmt, ## __VA_ARGS__)
#define CubismLogInfo(fmt, ...)       CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Info,    "[I]" fmt, ## __VA_ARGS__)
#define CubismLogWarning(fmt, ...)    CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Warning, "[W]" fmt, ## __VA_ARGS__)
#define CubismLogError(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Error,   "[E]" fmt, ## __VA_ARGS__)
#elif CSM_LOG_LEVEL == CSM_LOG_LEVEL_DEBUG
#define CubismLogVerbose(fmt, ...)
#define CubismLogDebug(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Debug,   "[D]" fmt, ## __VA_ARGS__)
#define CubismLogInfo(fmt, ...)       CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Info,    "[I]" fmt, ## __VA_ARGS__)
#define CubismLogWarning(fmt, ...)    CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Warning, "[W]" fmt, ## __VA_ARGS__)
#define CubismLogError(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Error,   "[E]" fmt, ## __VA_ARGS__)
#elif CSM_LOG_LEVEL == CSM_LOG_LEVEL_INFO
#define CubismLogVerbose(fmt, ...)
#define CubismLogDebug(fmt, ...)
#define CubismLogInfo(fmt, ...)       CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Info,    "[I]" fmt, ## __VA_ARGS__)
#define CubismLogWarning(fmt, ...)    CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Warning, "[W]" fmt, ## __VA_ARGS__)
#define CubismLogError(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Error,   "[E]" fmt, ## __VA_ARGS__)
#elif CSM_LOG_LEVEL == CSM_LOG_LEVEL_WARNING
#define CubismLogVerbose(fmt, ...)
#define CubismLogDebug(fmt, ...)
#define CubismLogInfo(fmt, ...)
#define CubismLogWarning(fmt, ...)    CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Warning, "[W]" fmt, ## __VA_ARGS__)
#define CubismLogError(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Error,   "[E]" fmt, ## __VA_ARGS__)
#elif CSM_LOG_LEVEL == CSM_LOG_LEVEL_ERROR
#define CubismLogVerbose(fmt, ...)
#define CubismLogDebug(fmt, ...)
#define CubismLogInfo(fmt, ...)
#define CubismLogWarning(fmt, ...)
#define CubismLogError(fmt, ...)      CubismLogPrintln(Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Error,   "[E]" fmt, ## __VA_ARGS__)
#else
#define CubismLogVerbose(fmt, ...)
#define CubismLogDebug(fmt, ...)
#define CubismLogInfo(fmt, ...)
#define CubismLogWarning(fmt, ...)
#define CubismLogError(fmt, ...)
#endif

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {
template<class T>
class csmVector;
}}}

namespace Live2D { namespace Cubism { namespace Framework { namespace Utils {

/**
 * @brief   デバッグ用ユーティリティクラス。<br>
 *           ログの出力、バイトのダンプなど
 */
class CubismDebug
{
public:
    /**
     *@brief    ログを出力する。第一引数にログレベルを設定する。<br>
     *           CubismFramework::Initialize()時にオプションで設定されたログ出力レベルを下回る場合はログに出さない。
     *
     *@param    logLevel ->  ログレベルの設定
     *@param    format   ->  書式付き文字列
     *@param    ...      ->  可変長引数
     *
     */
    static void Print(CubismFramework::Option::LogLevel logLevel, const csmChar* format, ...);

    /**
     *@brief    データから指定した長さだけダンプ出力する。<br>
     *           CubismFramework::Initialize()時にオプションで設定されたログ出力レベルを下回る場合はログに出さない。
     *
     *@param    logLevel ->  ログレベルの設定
     *@param    data     ->  ダンプするデータ
     *@param    length   ->  ダンプする長さ
     *
     */
    static void DumpBytes(CubismFramework::Option::LogLevel logLevel, const csmUint8* data, csmInt32 length);

private:

    /**
     *  privateコンストラクタ
     */
    CubismDebug();

};
}}}}

//------------ LIVE2D NAMESPACE ------------
