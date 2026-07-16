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
#include "axmol/platform/CommandLineArgs.h"

#if defined(_WIN32)
#    include <windows.h>
#    include "ntcvt/ntcvt.hpp"
#endif

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include <shellapi.h>
#endif

namespace ax
{

void CommandLineArgs::rebuildViews()
{
    _views.clear();
    _views.reserve(_storage.size());
    for (const auto& s : _storage)
        _views.emplace_back(s);
}

void CommandLineArgs::buildFromArgv(int argc, tchar_t** argv)
{
    // Caller must ensure no concurrent access during construction
    _storage.clear();
    _views.clear();
    if (argc <= 0)
    {
        _ownsStorage = true;
        return;
    }
    _storage.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i)
    {
#if defined(_WIN32) && defined(_UNICODE)
        _storage.emplace_back(argv[i] ? ntcvt::from_chars(argv[i]) : std::string{});
#else
        _storage.emplace_back(argv[i] ? argv[i] : "");
#endif
    }
    rebuildViews();
    _ownsStorage = true;
}

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
void CommandLineArgs::buildFromCommandLine()
{
    // Caller must ensure no concurrent access during construction
    int argc        = 0;
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv)
    {
        clear();
        return;
    }
    _storage.clear();
    _storage.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i)
    {
        _storage.emplace_back(ntcvt::from_chars(wargv[i]));
    }
    LocalFree(wargv);
    rebuildViews();
    _ownsStorage = true;
}
#endif

void CommandLineArgs::buildViewsFromArgv(int argc, char** argv) noexcept
{
    // Non-owning: views point to external argv memory. Caller must guarantee lifetime.
    _storage.clear();  // relinquish ownership
    _views.clear();
    if (argc <= 0)
    {
        _ownsStorage = false;
        return;
    }
    _views.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i)
    {
        _views.emplace_back(argv[i] ? argv[i] : "");
    }
    _ownsStorage = false;
}

void CommandLineArgs::clear() noexcept
{
    _storage.clear();
    _views.clear();
    _ownsStorage = false;
}

const std::vector<std::string>& CommandLineArgs::storage() const noexcept
{
    return _storage;
}

const std::vector<std::string_view>& CommandLineArgs::views() const noexcept
{
    return _views;
}
size_t CommandLineArgs::count() const noexcept
{
    return _views.size();
}

bool CommandLineArgs::empty() const noexcept
{
    return _views.empty();
}

std::string_view CommandLineArgs::operator[](size_t i) const noexcept
{
    return _views[i];
}

bool CommandLineArgs::ownsStorage() const noexcept
{
    return _ownsStorage;
}

}  // namespace ax
