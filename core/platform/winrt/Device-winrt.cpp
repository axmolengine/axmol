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

#include "platform/PlatformConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)

#    include <dwrite.h>
#    include <d2d1.h>
#    include <wincodec.h>
#    include <Shlwapi.h>
#    include <shellapi.h>
#    include "ntcvt/ntcvt.hpp"
#    include "platform/Device.h"
#    include "platform/FileUtils.h"
#    include "platform/winrt/WinRTUtils.h"
#    include "platform/StdC.h"
#    include "platform/winrt/GLViewImpl-winrt.h"

#    include <winrt/Windows.Devices.Sensors.h>

using namespace winrt;
using namespace Windows::Graphics::Display;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;

#    if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
using namespace Windows::Phone::Devices::Notification;
#    endif  // (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)

NS_AX_BEGIN

template <typename T>
inline HRESULT CreateInstance(REFCLSID clsid, winrt::com_ptr<T>& ptr)
{
    ASSERT(!ptr);
    return CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, __uuidof(T),
                            reinterpret_cast<void**>(ptr.put()));
}

#    define ASSERT_HR(expression) winrt::check_hresult(expression)

class TextRenderer
{
    winrt::com_ptr<ID2D1Factory> _d2d1Factory;
    winrt::com_ptr<IDWriteFactory> _dwriteFactory;
    winrt::com_ptr<IWICImagingFactory> _imageFactory;

    winrt::com_ptr<IDWriteTextFormat> _textFormat;
    winrt::com_ptr<ID2D1RenderTarget> _renderTarget;
    winrt::com_ptr<ID2D1SolidColorBrush> _fillBrush;

    winrt::com_ptr<IWICBitmap> _wicBitmap;

public:
    TextRenderer()
    {
        ASSERT_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                      (::IUnknown**)_dwriteFactory.put()));

        D2D1_FACTORY_OPTIONS fo = {};

#    ifdef _DEBUG
        fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#    endif

        ASSERT_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, _d2d1Factory.put()));

        ASSERT_HR(CreateInstance(CLSID_WICImagingFactory, _imageFactory));
    }

    ~TextRenderer() {}

    bool setFont(const FontDefinition& textDefinition)
    {
        bool bRet = false;
        do
        {
            std::wstring wfontName = ntcvt::from_chars(textDefinition._fontName);

            // italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
            const DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
            // bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
            const DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

            ASSERT_HR(_dwriteFactory->CreateTextFormat(wfontName.c_str(), nullptr, fontWeight, fontStyle,
                                                       DWRITE_FONT_STRETCH_NORMAL, textDefinition._fontSize, L"",
                                                       _textFormat.put()));

            if (textDefinition._alignment == TextHAlignment::CENTER)
                _textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

            if (textDefinition._vertAlignment == TextVAlignment::CENTER)
                _textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            return static_cast<bool>(_textFormat);

            bRet = true;
        } while (0);
        return bRet;
    }

    SIZE sizeWithText(const wchar_t* text,
                      int nLen,
                      DWORD dwFmt,
                      LONG nWidthLimit,
                      LONG nHeightLimit,
                      bool enableWrap,
                      int overflow)
    {
        SIZE tRet = {0};
        do
        {
            HRESULT hr = S_OK;
            winrt::com_ptr<IDWriteTextLayout> pTextLayout;
            const float maxWidth  = nWidthLimit == 0 ? 16384.0f : static_cast<float>(nWidthLimit);
            const float maxHeight = nHeightLimit == 0 ? 16384.0f : static_cast<float>(nWidthLimit);

            // layout
            if (!enableWrap)
                _textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

            enum Overflow
            {
                /**
                 * In SHRINK mode, the font size will change dynamically to adapt the content size.
                 */
                SHRINK = 2,
            };
            if (overflow == Overflow::SHRINK)
            {
                DWRITE_TRIMMING trimOptions{DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_WORD, 0, 0};
                _textFormat->SetTrimming(&trimOptions, nullptr);
            }
            // Create a text layout
            hr = _dwriteFactory->CreateTextLayout(text, static_cast<UINT32>(nLen), _textFormat.get(), maxWidth,
                                                  maxHeight, pTextLayout.put());

            if (SUCCEEDED(hr))
            {
                // Get text size
                DWRITE_TEXT_METRICS textMetrics;
                hr = pTextLayout->GetMetrics(&textMetrics);
                auto metrics =
                    D2D1::SizeF(ceil(textMetrics.widthIncludingTrailingWhitespace), ceil(textMetrics.height));
                tRet.cx = static_cast<LONG>(metrics.width);
                tRet.cy = static_cast<LONG>(metrics.height);
            }

        } while (0);

        return tRet;
    }

    bool prepareBitmap(int nWidth, int nHeight, const FontDefinition& textDef)
    {
        // release bitmap
        // Create a pixel format and initial its format
        // and alphaMode fields.
        // https://docs.microsoft.com/en-gb/windows/win32/direct2d/supported-pixel-formats-and-alpha-modes#supported-formats-for-id2d1devicecontext
        D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        props.pixelFormat                   = pixelFormat;

        ASSERT_HR(_imageFactory->CreateBitmap(nWidth, nHeight, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad,
                                              _wicBitmap.put()));

        ASSERT_HR(_d2d1Factory->CreateWicBitmapRenderTarget(_wicBitmap.get(), props, _renderTarget.put()));

        // CreateDeviceResources();
        ASSERT_HR(_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), _fillBrush.put()));
        return true;
    }

    int drawText(std::string_view text, SIZE& tSize, Device::TextAlign eAlign, const FontDefinition& textDefinition)
    {
        int nRet            = 0;
        wchar_t* pwszBuffer = nullptr;
        wchar_t* fixedText  = nullptr;
        do
        {
            AX_BREAK_IF(text.empty());

            DWORD dwFmt = DT_WORDBREAK;
            if (!textDefinition._enableWrap)
            {
                dwFmt |= DT_SINGLELINE;
            }
            DWORD dwHoriFlag = (int)eAlign & 0x0f;
            DWORD dwVertFlag = ((int)eAlign & 0xf0) >> 4;

            switch (dwHoriFlag)
            {
            case 1:  // left
                dwFmt |= DT_LEFT;
                break;
            case 2:  // right
                dwFmt |= DT_RIGHT;
                break;
            case 3:  // center
                dwFmt |= DT_CENTER;
                break;
            }

            int nLen = static_cast<int>(text.length());
            // utf-8 to utf-16
            int nBufLen = nLen + 1;
            pwszBuffer  = new wchar_t[nBufLen];
            AX_BREAK_IF(!pwszBuffer);

            memset(pwszBuffer, 0, sizeof(wchar_t) * nBufLen);
            nLen = MultiByteToWideChar(CP_UTF8, 0, text.data(), nLen, pwszBuffer, nBufLen);

            if (strchr(text.data(), '&'))
            {
                fixedText      = new wchar_t[nLen * 2 + 1];
                int fixedIndex = 0;
                for (int index = 0; index < nLen; ++index)
                {
                    if (pwszBuffer[index] == '&')
                    {
                        fixedText[fixedIndex]     = '&';
                        fixedText[fixedIndex + 1] = '&';
                        fixedIndex += 2;
                    }
                    else
                    {
                        fixedText[fixedIndex] = pwszBuffer[index];
                        fixedIndex += 1;
                    }
                }
                fixedText[fixedIndex] = '\0';
                nLen                  = fixedIndex;
            }

            SIZE newSize;
            if (fixedText)
            {
                newSize = sizeWithText(fixedText, nLen, dwFmt, tSize.cx, tSize.cy, textDefinition._enableWrap,
                                       textDefinition._overflow);
            }
            else
            {
                newSize = sizeWithText(pwszBuffer, nLen, dwFmt, tSize.cx, tSize.cy, textDefinition._enableWrap,
                                       textDefinition._overflow);
            }

            D2D1_RECT_F rcText = {0};
            // if content width is 0, use text size as content size
            if (tSize.cx <= 0)
            {
                tSize         = newSize;
                rcText.right  = newSize.cx;
                rcText.bottom = newSize.cy;
            }
            else
            {

                LONG offsetX = 0;
                LONG offsetY = 0;
                rcText.right = newSize.cx;  // store the text width to rectangle

                // calculate text horizontal offset
                if (1 != dwHoriFlag                                      // and text isn't align to left
                    && newSize.cx < tSize.cx)                            // and text's width less then content width,
                {                                                        // then need adjust offset of X.
                    offsetX = (2 == dwHoriFlag) ? tSize.cx - newSize.cx  // align to right
                                                : (tSize.cx - newSize.cx) / 2;  // align to center
                }

                // if content height is 0, use text height as content height
                // else if content height less than text height, use content height to draw text
                if (tSize.cy <= 0)
                {
                    tSize.cy = newSize.cy;
                    dwFmt |= DT_NOCLIP;
                    rcText.bottom = newSize.cy;  // store the text height to rectangle
                }
                else if (tSize.cy < newSize.cy)
                {
                    // content height larger than text height need, clip text to rect
                    rcText.bottom = tSize.cy;
                }
                else
                {
                    rcText.bottom = newSize.cy;  // store the text height to rectangle

                    // content larger than text, need adjust vertical position
                    dwFmt |= DT_NOCLIP;

                    // calculate text vertical offset
                    offsetY = (2 == dwVertFlag)   ? tSize.cy - newSize.cy        // align to bottom
                              : (3 == dwVertFlag) ? (tSize.cy - newSize.cy) / 2  // align to middle
                                                  : 0;                           // align to top
                }

                if (offsetX || offsetY)
                {
                    // OffsetRect(&rcText, offsetX, offsetY);
                    rcText.left += offsetX;
                    rcText.right += offsetY;
                    rcText.bottom += offsetY;
                    rcText.top += offsetY;
                }
            }

            AX_BREAK_IF(!prepareBitmap(tSize.cx, tSize.cy, textDefinition));

            // draw text
            _renderTarget->BeginDraw();

            _renderTarget->DrawText(fixedText ? (LPCTSTR)fixedText : pwszBuffer, nLen, _textFormat.get(), &rcText,
                                    _fillBrush.get());
            _renderTarget->EndDraw();

            nRet = true;
        } while (0);
        AX_SAFE_DELETE_ARRAY(pwszBuffer);
        delete[] fixedText;

        return nRet;
    }

    void copyPixels(uint8_t* out, size_t outLen, int rowPitch)
    {
        _wicBitmap->CopyPixels(NULL, static_cast<UINT>(rowPitch), static_cast<UINT>(outLen), out);
    }

private:
    friend class Image;
};

static TextRenderer& sharedTextRenderer()
{
    static TextRenderer s_BmpDC;
    return s_BmpDC;
}

int Device::getDPI()
{
    return ax::GLViewImpl::sharedGLView()->GetDPI();
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

        sToken = sAccelerometer.ReadingChanged(
                [](Accelerometer const& a, AccelerometerReadingChangedEventArgs const& e) {
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

            auto orientation = GLViewImpl::sharedGLView()->getDeviceOrientation();

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

            std::shared_ptr<ax::InputEvent> event(new AccelerometerEvent(acc));
            ax::GLViewImpl::sharedGLView()->QueueEvent(event);
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

Data Device::getTextureDataForText(std::string_view text,
                                   const FontDefinition& textDefinition,
                                   TextAlign align,
                                   int& width,
                                   int& height,
                                   bool& hasPremultipliedAlpha)
{
    Data ret;
    do
    {
        auto& textRenderer = sharedTextRenderer();

        if (!textRenderer.setFont(textDefinition))
        {
            AXLOGW("Can't found font({}), use system default", textDefinition._fontName);
        }

        // draw text
        // does changing to SIZE here affects the font size by rounding from float?
        SIZE size = {(LONG)textDefinition._dimensions.width, (LONG)textDefinition._dimensions.height};
        AX_BREAK_IF(!textRenderer.drawText(text, size, align, textDefinition));

        int dataLen            = size.cx * size.cy * 4;
        unsigned char* dataBuf = (unsigned char*)malloc(sizeof(unsigned char) * dataLen);
        AX_BREAK_IF(!dataBuf);

        width  = static_cast<int>(size.cx);
        height = static_cast<int>(size.cy);

        const int bpp   = 32;
        size_t rowPitch = (width * bpp + 7) / 8;
        textRenderer.copyPixels(dataBuf, dataLen, rowPitch);

        ret.fastSet(dataBuf, dataLen);
        hasPremultipliedAlpha = false;
    } while (0);

    return ret;
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

NS_AX_END

#endif  // (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
