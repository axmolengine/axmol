/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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
#include "axmol/platform/Device.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/StdC.h"
#include "ntcvt/ntcvt.hpp"

namespace ax
{
namespace
{
struct ScopedClipboard
{
    ScopedClipboard() : _ok(::OpenClipboard(nullptr)) {}
    ~ScopedClipboard()
    {
        if (_ok)
            ::CloseClipboard();
    }
    ScopedClipboard(const ScopedClipboard&)            = delete;
    ScopedClipboard& operator=(const ScopedClipboard&) = delete;

    explicit operator bool() const { return !!_ok; }

private:
    BOOL _ok;
};
}  // namespace

void Device::getClipboardText(std::function<void(std::string_view)> callback)
{
    if (!callback)
        return;
    ScopedClipboard clipboard;
    if (!clipboard)
    {
        callback(std::string_view{});
        return;
    }

    std::string result;
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData)
    {
        LPCWSTR pwsz = static_cast<LPCWSTR>(GlobalLock(hData));
        if (pwsz)
        {
            result = ntcvt::from_chars(pwsz, CP_UTF8);
            GlobalUnlock(hData);
        }
    }

    callback(result);
}

void Device::setClipboardText(std::string_view text)
{
    // Convert to wide (UTF-16)
    ScopedClipboard clipboard;
    if (!clipboard)
        return;

    // Empty clipboard first
    if (!EmptyClipboard() || text.empty())
    {
        return;
    }

    // Allocate global memory for the wide string including null terminator
    int cch = ::MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), nullptr, 0);
    if (cch <= 0)
    {
        return;
    }
    size_t bytes  = (cch + 1) * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hGlob)
    {
        return;
    }

    void* pGlob = GlobalLock(hGlob);
    if (!pGlob)
    {
        GlobalFree(hGlob);
        return;
    }

    ::MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), static_cast<wchar_t*>(pGlob), cch);
    static_cast<wchar_t*>(pGlob)[cch] = L'\0';
    GlobalUnlock(hGlob);

    // Set clipboard data as CF_UNICODETEXT
    if (!SetClipboardData(CF_UNICODETEXT, hGlob))
        GlobalFree(hGlob);
}

void Device::clearClipboard()
{
    ScopedClipboard clipboard;
    if (clipboard)
        EmptyClipboard();
}

int Device::getDPI()
{
    static int dpi = -1;
    if (dpi == -1)
    {
        HDC hScreenDC = GetDC(nullptr);
        int PixelsX   = GetDeviceCaps(hScreenDC, HORZRES);
        int MMX       = GetDeviceCaps(hScreenDC, HORZSIZE);
        ReleaseDC(nullptr, hScreenDC);
        dpi = (int)(254.0f * PixelsX / MMX / 10);
    }
    return dpi;
}

float Device::getPixelRatio()
{
    const HDC dc    = GetDC(nullptr);
    const auto xdpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);
    return xdpi / 96.0f;
}

void Device::setAccelerometerEnabled(bool isEnabled) {}

void Device::setAccelerometerInterval(float interval) {}

void Device::setKeepScreenOn(bool value)
{
    AX_UNUSED_PARAM(value);
}

void Device::vibrate(float duration)
{
    AX_UNUSED_PARAM(duration);
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style)
{
    AX_UNUSED_PARAM(style);
}

void Device::impactOccurred(ImpactFeedbackStyle style)
{
    AX_UNUSED_PARAM(style);
}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType type)
{
    AX_UNUSED_PARAM(type);
}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged() {}

}  // namespace ax
