/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
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
#include "platform/PlatformConfig.h"
#include "platform/winrt/GLViewImpl-winrt.h"
#include "base/Director.h"
#include <algorithm>
#include "platform/FileUtils.h"
#include "platform/winrt/WinRTUtils.h"
#include "platform/Application.h"

#include "pugixml/pugixml.hpp"
#include "fmt/format.h"

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.System.UserProfile.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace Windows::UI::Core;
using namespace Windows::Foundation;

/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/

namespace ax
{

// sharedApplication pointer
Application* Application::sm_pSharedApplication = nullptr;

////////////////////////////////////////////////////////////////////////////////
// implement Application
////////////////////////////////////////////////////////////////////////////////

// sharedApplication pointer
Application* s_pSharedApplication = nullptr;

Application::Application() : m_openURLDelegate(nullptr)
{
    m_nAnimationInterval.QuadPart = 0;
    AX_ASSERT(!sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = nullptr;
}

int Application::run()
{
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    QueryPerformanceCounter(&m_nLast);

    GLViewImpl::sharedGLView()->Run();
    return 0;
}

bool Application::frameStep(const std::function<bool()>& onFrame)
{
    QueryPerformanceCounter(&m_nNow);
    const auto interval = m_nNow.QuadPart - m_nLast.QuadPart;
    if (interval > m_nAnimationInterval.QuadPart)
    {
        m_nLast.QuadPart = m_nNow.QuadPart;
        return onFrame();
    }
    else
    {
        // The precision of timer on Windows is set to highest (1ms) by 'timeBeginPeriod' from above code,
        // but it's still not precise enough. For example, if the precision of timer is 1ms,
        // Sleep(3) may make a sleep of 2ms or 4ms. Therefore, we subtract 1ms here to make Sleep time shorter.
        // If 'waitMS' is equal or less than 1ms, don't sleep and run into next loop to
        // boost CPU to next frame accurately.
        const auto waitMS =
            static_cast<LONG>((m_nAnimationInterval.QuadPart - interval) * 1000LL / m_nFreq.QuadPart - 1L);
        if (waitMS > 1L)
            Sleep(waitMS);

        return true;
    }
}

void Application::setAnimationInterval(float interval)
{
    QueryPerformanceFrequency(&m_nFreq);
    m_nAnimationInterval.QuadPart = (LONGLONG)(interval * m_nFreq.QuadPart);
}

// void Application::setAnimationInterval(float interval, SetIntervalReason reason)
//{
//     setAnimationInterval(interval);
// }

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    AX_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

const char* Application::getCurrentLanguageCode()
{
    static std::string code = "en";
    auto languages          = Windows::System::UserProfile::GlobalizationPreferences::Languages();
    code                    = PlatformStringToString(languages.GetAt(0));
    return code.c_str();
}

LanguageType Application::getCurrentLanguage()
{
    const char* code = getCurrentLanguageCode();

    return utils::getLanguageTypeByISO2(code);
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::WinUWP;
}

std::string Application::getVersion()
{
    auto package   = winrt::Windows::ApplicationModel::Package::Current();
    auto packageId = package.Id();
    auto version   = packageId.Version();

    return fmt::format("{0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision);
}

bool Application::openURL(std::string_view url)
{
    auto dispatcher = ax::GLViewImpl::sharedGLView()->getDispatcher();
    dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, DispatchedHandler([url]() {
                                  auto uri = Windows::Foundation::Uri(PlatformStringFromString(url));
                                  Windows::System::Launcher::LaunchUriAsync(uri);
                              }));
    return true;
}

void Application::setStartupScriptFilename(const std::string& startupScriptFile)
{
    m_startupScriptFilename = startupScriptFile;
    std::replace(m_startupScriptFilename.begin(), m_startupScriptFilename.end(), '\\', '/');
}

}
