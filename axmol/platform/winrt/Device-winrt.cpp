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

#include "axmol/platform/PlatformConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)

#    include <dwrite.h>
#    include <d2d1.h>
#    include <wincodec.h>
#    include <Shlwapi.h>
#    include <shellapi.h>
#    include <winstring.h>
#    include "ntcvt/ntcvt.hpp"
#    include "axmol/platform/StdC.h"
#    include "axmol/platform/Device.h"
#    include "axmol/platform/FileUtils.h"
#    include "axmol/platform/winrt/WinRTUtils.h"
#    include "axmol/platform/winrt/RenderView-winrt.h"
#    include <winrt/Windows.Devices.Sensors.h>
#    include <winrt/Windows.ApplicationModel.DataTransfer.h>
#    include <winrt/Windows.ApplicationModel.Core.h>
#    include <atomic>

using namespace winrt;
using namespace Windows::Graphics::Display;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::DataTransfer;

#    if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
using namespace Windows::Phone::Devices::Notification;
#    endif  // (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)

namespace ax
{

namespace
{
std::atomic<float> s_dpi{96.0f};
}

void Device::getClipboardText(std::function<void(std::string_view)> callback)
{
    if (!callback)
        return;

    try
    {
        DataPackageView view = Clipboard::GetContent();
        if (!view)
            return callback(std::string_view{});

        hstring hs = view.GetTextAsync().get();
        callback(ntcvt::from_chars(std::wstring_view{hs.c_str(), hs.size()}, CP_UTF8));
    }
    catch (hresult_error const&)
    {
        return callback(std::string_view{});
    }
}

void Device::setClipboardText(std::string_view text)
{
    DataPackage dp;
    dp.RequestedOperation(DataPackageOperation::Copy);
    dp.SetText(winrt::to_hstring(text));

    auto dispatcher = CoreApplication::MainView().CoreWindow().Dispatcher();
    dispatcher.RunAsync(CoreDispatcherPriority::Normal, [dp]() mutable {
        try
        {
            Clipboard::SetContent(dp);
        }
        catch (hresult_error const& e)
        {}
    });
}

void Device::clearClipboard()
{
    try
    {
        Clipboard::Clear();
    }
    catch (hresult_error const&)
    {}
}

int Device::getDPI()
{
    return static_cast<int>(s_dpi.load(std::memory_order_acquire));
}

void Device::setDPI(float dpi)
{
    s_dpi.store(dpi, std::memory_order_release);
}

float Device::getPixelRatio()
{
    return Device::getDPI() / 96.0f;
}

static Accelerometer sAccelerometer = nullptr;

void Device::setAccelerometerEnabled(bool isEnabled)
{
    static winrt::event_token sToken;
    static bool sEnabled = false;

    // we always need to reset the accelerometer
    if (sAccelerometer)
    {
        sAccelerometer.ReadingChanged(sToken);
        sAccelerometer = nullptr;
        sEnabled       = false;
    }

    if (isEnabled)
    {
        sAccelerometer = Accelerometer::GetDefault();

        if (sAccelerometer == nullptr)
        {
            // It's not a friendly experience and may cause crash.
            // MessageBox("This device does not have an accelerometer.","Alert");
            AXLOGI("This device does not have an accelerometer.");
            return;
        }

        setAccelerometerInterval(0.0f);
        sEnabled = true;

        sToken =
            sAccelerometer.ReadingChanged([](Accelerometer const& a, AccelerometerReadingChangedEventArgs const& e) {
            if (!sEnabled)
            {
                return;
            }

            auto&& reading = e.Reading();
            ax::Acceleration acc;
            acc.x         = reading.AccelerationX();
            acc.y         = reading.AccelerationY();
            acc.z         = reading.AccelerationZ();
            acc.timestamp = 0;

            auto orientation = RenderView::sharedRenderView()->getDeviceOrientation();

            if (isWindowsPhone())
            {
                switch (orientation)
                {
                case DisplayOrientations::Portrait:
                    acc.x = reading.AccelerationX();
                    acc.y = reading.AccelerationY();
                    break;

                case DisplayOrientations::Landscape:
                    acc.x = -reading.AccelerationY();
                    acc.y = reading.AccelerationX();
                    break;

                case DisplayOrientations::PortraitFlipped:
                    acc.x = -reading.AccelerationX();
                    acc.y = reading.AccelerationY();
                    break;

                case DisplayOrientations::LandscapeFlipped:
                    acc.x = reading.AccelerationY();
                    acc.y = -reading.AccelerationX();
                    break;

                default:
                    acc.x = reading.AccelerationX();
                    acc.y = reading.AccelerationY();
                    break;
                }
            }
            else  // Windows Store App
            {
                // from http://msdn.microsoft.com/en-us/library/windows/apps/dn440593
                switch (orientation)
                {
                case DisplayOrientations::Portrait:
                    acc.x = reading.AccelerationY();
                    acc.y = -reading.AccelerationX();
                    break;

                case DisplayOrientations::Landscape:
                    acc.x = reading.AccelerationX();
                    acc.y = reading.AccelerationY();
                    break;

                case DisplayOrientations::PortraitFlipped:
                    acc.x = -reading.AccelerationY();
                    acc.y = reading.AccelerationX();
                    break;

                case DisplayOrientations::LandscapeFlipped:
                    acc.x = -reading.AccelerationX();
                    acc.y = -reading.AccelerationY();
                    break;

                default:
                    acc.x = reading.AccelerationY();
                    acc.y = -reading.AccelerationX();
                    break;
                }
            }

            // std::shared_ptr<ax::InputEvent> event(new AccelerometerEvent(acc));
            // ax::RenderView::sharedRenderView()->QueueEvent(event);

            Director::getInstance()->postTask([acc]() {
                // std::shared_ptr<ax::InputEvent> event(new AccelerometerEvent(acc));
                // ax::RenderView::sharedRenderView()->QueueEvent(event);
                ax::AccelerationEvent accEvent(acc);
                InputSystem::getInstance()->dispatchEvent(&accEvent);
            });
        });
    }
}

void Device::setAccelerometerInterval(float interval)
{
    if (sAccelerometer)
    {
        try
        {
            int minInterval = sAccelerometer.MinimumReportInterval();
            int reqInterval = (int)interval;
            sAccelerometer.ReportInterval(reqInterval < minInterval ? minInterval : reqInterval);
        }
        catch (winrt::hresult_error const& /*ex*/)
        {
            AXLOGW("Device::setAccelerometerInterval not supported on this device");
        }
    }
    else
    {
        AXLOGW("Device::setAccelerometerInterval: accelerometer not enabled.");
    }
}

void Device::setKeepScreenOn(bool /*value*/) {}

void Device::vibrate(float duration)
{
#    if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    Windows::Foundation::TimeSpan timespan;
    // A time period expressed in 100-nanosecond units, see
    // https://msdn.microsoft.com/en-us/library/windows/apps/windows.foundation.timespan.aspx The duration is limited to
    // a maximum of 5 seconds, see
    // https://msdn.microsoft.com/en-us/library/windows/apps/windows.phone.devices.notification.vibrationdevice.aspx
    timespan.Duration = std::min(static_cast<int>(duration * 10000), 50000);

    VibrationDevice ^ testVibrationDevice = VibrationDevice::GetDefault();
    testVibrationDevice->Vibrate(timespan);
#    endif  // (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle /*style*/) {}

void Device::impactOccurred(ImpactFeedbackStyle /*style*/) {}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType /*type*/) {}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged() {}

int Device::getDisplayRefreshRate()
{
    return 60;
}

void Device::setPreferredOrientation(Device::Orientation /*orientation*/) {}

Device::Orientation Device::getPreferredOrientation()
{
    return Orientation::Unknown;
}

Device::OrientationMask Device::getSupportedOrientations()
{
    return OrientationMask::All;
}

Device::Orientation Device::getPhysicalOrientation()
{
    return Orientation::Unknown;
}

Device::Orientation Device::resolveOrientation()
{
    return Orientation::Unknown;
}

}  // namespace ax

#endif  // (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
