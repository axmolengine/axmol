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

#include "base/bitmask.h"
#include "platform/PlatformMacros.h"

#include "fmt/compile.h"

NS_AX_BEGIN

enum class LogLevel
{
    Verbose,
    Trace = Verbose,
    Debug,
    Info,
    Warn,
    Error,
    Silent /* only for setLogLevel(); must be last */
};

enum class LogFmtFlag
{
    Null,
    Level     = 1,
    TimeStamp = 1 << 1,
    ProcessId = 1 << 2,
    ThreadId  = 1 << 3,
    Colored   = 1 << 4,
    Full      = Level | TimeStamp | ProcessId | ThreadId | Colored,
};
AX_ENABLE_BITMASK_OPS(LogFmtFlag);

class LogItem
{
    friend AX_API LogItem& preprocessLog(LogItem&& logItem);
    friend AX_API void writeLog(LogItem& item, const char* tag);

public:
    static constexpr auto COLOR_PREFIX_SIZE    = 5;                      // \x1b[00m
    static constexpr auto COLOR_QUALIFIER_SIZE = COLOR_PREFIX_SIZE + 3;  // \x1b[m

    explicit LogItem(LogLevel lvl) : level_(lvl) {}
    LogItem(const LogItem&) = delete;

    LogLevel level() const { return level_; }

    std::string_view message() const
    {
        return has_style_ ? std::string_view{qualified_message_.data() + COLOR_PREFIX_SIZE,
                                             qualified_message_.size() - COLOR_QUALIFIER_SIZE}
                          : std::string_view{qualified_message_};
    }

    template <typename _FmtType, typename... _Types>
    inline static LogItem& vformat(_FmtType&& fmt, LogItem& item, _Types&&... args)
    {
        item.qualified_message_ =
            fmt::format(std::forward<_FmtType>(fmt), std::string_view{item.prefix_buffer_, item.prefix_size_},
                        std::forward<_Types>(args)...);

        item.qualifier_size_ = item.prefix_size_;

        auto old_size = item.qualified_message_.size();
        if (item.has_style_)
            item.qualified_message_.append("\x1b[m"sv);
        item.qualifier_size_ += (item.qualified_message_.size() - old_size);
        return item;
    }

private:
    void writePrefix(std::string_view data)
    {
        memcpy(prefix_buffer_ + prefix_size_, data.data(), data.size());
        prefix_size_ += data.size();
    }
    LogLevel level_;
    bool has_style_{false};
    size_t prefix_size_{0};     // \x1b[00mD/[2024-02-29 00:00:00.123][PID:][TID:]
    size_t qualifier_size_{0};  // prefix_size_ + \x1b[m (optional) + \n
    std::string qualified_message_;
    char prefix_buffer_[128];
};

class ILogOutput
{
public:
    virtual ~ILogOutput() {}
    virtual void write(LogItem& item, const char* tag) = 0;
};

/* @brief control log level */
AX_API void setLogLevel(LogLevel level);
AX_API LogLevel getLogLevel();

/* @brief control log prefix format */
AX_API void setLogFmtFlag(LogFmtFlag flags);

/* @brief set log output */
AX_API void setLogOutput(ILogOutput* output);

/* @brief internal use */
AX_API LogItem& preprocessLog(LogItem&& logItem);

/* @brief internal use */
AX_API void outputLog(LogItem& item, const char* tag);
AX_API void writeLog(LogItem& item, const char* tag);

template <typename _FmtType, typename... _Types>
inline void printLogT(_FmtType&& fmt, LogItem& item, _Types&&... args)
{
    if (item.level() >= getLogLevel())
        outputLog(LogItem::vformat(std::forward<_FmtType>(fmt), item, std::forward<_Types>(args)...), "axmol");
}

#define AXLOG_WITH_LEVEL(level, fmtOrMsg, ...) \
    ax::printLogT(FMT_COMPILE("{}" fmtOrMsg "\n"), ax::preprocessLog(ax::LogItem{level}), ##__VA_ARGS__)

#define AXLOGV(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Verbose, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGD(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Debug, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGI(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Info, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGW(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Warn, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGE(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Error, fmtOrMsg, ##__VA_ARGS__)

#define AXLOGT AXLOGV

/**
 @brief Output Debug message.
 */
/* AX_DEPRECATED_ATTRIBUTE*/ AX_API void print(const char* format, ...) AX_FORMAT_PRINTF(1, 2);  // use AXLOGD instead

NS_AX_END
