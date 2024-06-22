
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

#include "base/Logging.h"

#include "yasio/utils.hpp"
#include "fmt/color.h"

#if defined(_WIN32)
#    include "ntcvt/ntcvt.hpp"
#endif

#if defined(__ANDROID__)
#    include <android/log.h>
#endif

NS_AX_BEGIN

#if defined(_AX_DEBUG) && _AX_DEBUG == 1
static LogLevel s_logLevel = LogLevel::Debug;
#else
static LogLevel s_logLevel = LogLevel::Info;
#endif

static LogFmtFlag s_logFmtFlags = LogFmtFlag::Null;
static ILogOutput* s_logOutput  = nullptr;

AX_API void setLogLevel(LogLevel level)
{
    s_logLevel = level;
}

AX_API LogLevel getLogLevel()
{
    return s_logLevel;
}

AX_API void setLogFmtFlag(LogFmtFlag flags)
{
    s_logFmtFlags = flags;
}

AX_API void setLogOutput(ILogOutput* output)
{
    s_logOutput = output;
}

AX_API LogItem& preprocessLog(LogItem&& item)
{
    if (s_logFmtFlags != LogFmtFlag::Null)
    {
#if defined(_WIN32)
#    define xmol_getpid()       (uintptr_t)::GetCurrentProcessId()
#    define xmol_gettid()       (uintptr_t)::GetCurrentThreadId()
#    define localtime_r(utc, t) ::localtime_s(t, utc)
#else
#    define xmol_getpid() (uintptr_t)::getpid()
#    define xmol_gettid() (uintptr_t)::pthread_self()
#endif
        auto wptr              = item.prefix_buffer_;
        const auto buffer_size = sizeof(item.prefix_buffer_);
        auto& prefix_size      = item.prefix_size_;
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::Level | LogFmtFlag::Colored))
        {
            std::string_view levelName;
            switch (item.level_)
            {
            case LogLevel::Trace:
                levelName = "V/"sv;
                break;
            case LogLevel::Debug:
                levelName = "D/"sv;
                break;
            case LogLevel::Info:
                levelName = "I/"sv;
                break;
            case LogLevel::Warn:
                levelName = "W/"sv;
                break;
            case LogLevel::Error:
                levelName = "E/"sv;
                break;
            default:
                levelName = "?/"sv;
            }

#if !defined(__APPLE__) && !defined(__ANDROID__)
            if (bitmask::any(s_logFmtFlags, LogFmtFlag::Colored))
            {
                constexpr auto colorCodeOfLevel = [](LogLevel level) -> std::string_view {
                    switch (level)
                    {
                    case LogLevel::Verbose:
                        return "\x1b[37m"sv;
                    case LogLevel::Debug:
                        return "\x1b[36m"sv;
                    case LogLevel::Info:
                        return "\x1b[92m"sv;
                    case LogLevel::Warn:
                        return "\x1b[33m"sv;
                    case LogLevel::Error:
                        return "\x1b[31m"sv;
                    default:
                        return std::string_view{};
                    }
                };

                auto colorCode = colorCodeOfLevel(item.level_);
                if (!colorCode.empty())
                {
                    item.writePrefix(colorCode);
                    item.has_style_ = true;
                }
            }
#endif
            item.writePrefix(levelName);
        }
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::TimeStamp))
        {
            struct tm ts = {0};
            auto tv_msec = yasio::clock<yasio::system_clock_t>();
            auto tv_sec  = static_cast<time_t>(tv_msec / std::milli::den);
            localtime_r(&tv_sec, &ts);
            prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,
                                            "[{}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}]", ts.tm_year + 1900,
                                            ts.tm_mon + 1, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec,
                                            static_cast<int>(tv_msec % std::milli::den))
                               .size;
        }
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::ProcessId))
            prefix_size +=
                fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size, "[PID:{:x}]", xmol_getpid()).size;
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::ThreadId))
            prefix_size +=
                fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size, "[TID:{:x}]", xmol_gettid()).size;
    }
    return item;
}

AX_DLL void outputLog(LogItem& item, const char* tag)
{
    if (!s_logOutput) writeLog(item, tag);
    else s_logOutput->write(item, tag);
}

AX_DLL void writeLog(LogItem& item, const char* tag)
{
#if defined(__ANDROID__)
    int prio;
    switch (item.level_)
    {
    case LogLevel::Info:
        prio = ANDROID_LOG_INFO;
        break;
    case LogLevel::Warn:
        prio = ANDROID_LOG_WARN;
        break;
    case LogLevel::Error:
        prio = ANDROID_LOG_ERROR;
        break;
    default:
        prio = ANDROID_LOG_DEBUG;
    }
    struct trim_one_eol
    {
        explicit trim_one_eol(std::string& v) : value(v)
        {
            trimed = !v.empty() && v.back() == '\n';
            if (trimed)
                value.back() = '\0';
        }
        ~trim_one_eol()
        {
            if (trimed)
                value.back() = '\n';
        }
        operator const char* const() const { return value.c_str(); }
        std::string& value;
        bool trimed{false};
    };
    __android_log_print(prio, tag, "%s",
                        static_cast<const char*>(trim_one_eol{item.qualified_message_} + item.prefix_size_));
#else
    AX_UNUSED_PARAM(tag);
#    if defined(_WIN32)
    if (::IsDebuggerPresent())
        OutputDebugStringW(ntcvt::from_chars(item.message()).c_str());
#    endif

        // write normal color text to console
#    if defined(_WIN32)
    auto hStdout = ::GetStdHandle(item.level_ != LogLevel::Error ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    if (hStdout)
    {
        // print to console if possible
        // since we use win32 API, the ::fflush call doesn't required.
        // see: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-flushfilebuffers#return-value
        ::WriteFile(hStdout, item.qualified_message_.c_str(), static_cast<DWORD>(item.qualified_message_.size()),
                    nullptr, nullptr);
    }
#    else
    // Linux, Mac, iOS, etc
    auto outfp = item.level_ != LogLevel::Error ? stdout : stderr;
    auto outfd = ::fileno(outfp);
    ::fflush(outfp);
    ::write(outfd, item.qualified_message_.c_str(), item.qualified_message_.size());
#    endif
#endif
}

AX_API void print(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    auto message = StringUtils::vformat(format, args);
    va_end(args);

    if (!message.empty())
        outputLog(LogItem::vformat(FMT_COMPILE("{}{}\n"), preprocessLog(LogItem{LogLevel::Silent}), message),
                  "axmol debug info");
}

NS_AX_END
