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

#include <string>
#include <string_view>
#include <vector>

#include "axmol/platform/PlatformDefine.h"

namespace ax
{

/**
 * CommandLineArgs
 *
 * - Default behavior: build and own UTF-8 strings from argv/wargv (safe).
 * - Provides a non-owning view builder for performance; caller must guarantee argv lifetime.
 * - This variant is intentionally lock-free: caller must ensure construction happens
 *   before any concurrent access.
 */
class AX_API CommandLineArgs
{
public:
    CommandLineArgs()  = default;
    ~CommandLineArgs() = default;

    CommandLineArgs(const CommandLineArgs&)            = delete;
    CommandLineArgs& operator=(const CommandLineArgs&) = delete;
    CommandLineArgs(CommandLineArgs&&)                 = delete;
    CommandLineArgs& operator=(CommandLineArgs&&)      = delete;

    // Safe owning builders (recommended)
    void buildFromArgv(int argc, char** argv);  // assume argv is UTF-8, copy into internal storage
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    void buildFromWargv(int argc, wchar_t** wargv);  // convert wide strings to UTF-8 and copy
    void buildFromCommandLine();                     // Windows: use GetCommandLineW + CommandLineToArgvW
#endif

    // Performance: non-owning views (caller must ensure argv lifetime >= this object's lifetime)
    void buildViewsFromArgv(int argc, char** argv) noexcept;

    // Clear stored data (no locking; caller must ensure no concurrent access)
    void clear() noexcept;

    // Accessors (no locking; safe only after construction is complete)
    const std::vector<std::string>& storage() const noexcept;
    const std::vector<std::string_view>& views() const noexcept;
    size_t count() const noexcept;
    bool empty() const noexcept;
    std::string_view operator[](size_t i) const noexcept;

    // Debug / introspection: whether this object owns internal storage
    bool ownsStorage() const noexcept;

private:
    void rebuildViews();

    std::vector<std::string> _storage;     // owned UTF-8 strings
    std::vector<std::string_view> _views;  // views into _storage or external argv
    bool _ownsStorage = false;             // indicates ownership of storage
};

}  // namespace ax
