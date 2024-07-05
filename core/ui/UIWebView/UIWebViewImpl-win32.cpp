/****************************************************************************
 MIT License

 Portions copyright (c) 2017 Serge Zaitsev.
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

#if defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)

#    include "UIWebViewImpl-win32.h"
#    include "UIWebView.h"
#    include "UIWebViewCommon.h"
#    include "base/Director.h"
#    include "platform/FileUtils.h"
#    include "platform/GLView.h"
#    include "ui/UIHelper.h"
#    include "rapidjson/document.h"
#    include "rapidjson/stringbuffer.h"
#    include "rapidjson/writer.h"
#    include "base/Utils.h"

#    define WIN32_LEAN_AND_MEAN
#    include <Shlwapi.h>
#    include <codecvt>
#    include <utility>
#    include <stdlib.h>
#    include <Windows.h>

#    pragma comment(lib, "user32.lib")
#    pragma comment(lib, "Shlwapi.lib")

// Edge/Chromium headers and libs
#    include "WebView2.h"
#    pragma comment(lib, "ole32.lib")
#    pragma comment(lib, "oleaut32.lib")

#    include "platform/PlatformConfig.h"

#    include "ntcvt/ntcvt.hpp"

USING_NS_AX;
using namespace webview_common;

using msg_cb_t = std::function<void(std::string_view)>;

template <class ArgType>
static std::string getUriStringFromArgs(ArgType* args)
{
    if (args != nullptr)
    {
        LPWSTR uri;
        args->get_Uri(&uri);

        return ntcvt::from_chars(uri);
    }

    return {};
}

static double getDeviceScaleFactor()
{
    static double scale_factor = 1.0;
    static bool initialized    = false;

    if (!initialized)
    {
        // This value is safe to cache for the life time of the app since the user
        // must logout to change the DPI setting. This value also applies to all
        // screens.
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
        HDC screen_dc = ::GetDC(NULL);
        int dpi_x     = GetDeviceCaps(screen_dc, LOGPIXELSX);
        scale_factor  = static_cast<double>(dpi_x) / 96.0;
        ::ReleaseDC(NULL, screen_dc);
#    endif
        initialized = true;
    }

    return scale_factor;
}

// declarate
class Win32WebControl
{
public:
    Win32WebControl();

    bool createWebView(const std::function<bool(std::string_view)>& shouldStartLoading,
                       const std::function<void(std::string_view)>& didFinishLoading,
                       const std::function<void(std::string_view)>& didFailLoading,
                       const std::function<void(std::string_view)>& onJsCallback);
    void removeWebView();

    void setWebViewRect(const int left, const int top, const int width, const int height);
    void setJavascriptInterfaceScheme(std::string_view scheme);
    void loadHTMLString(std::string_view html, std::string_view baseURL);
    void loadURL(std::string_view url, bool cleanCachedData);
    void loadFile(std::string_view filePath);
    void stopLoading();
    void reload() const;
    bool canGoBack() const;
    bool canGoForward() const;
    void goBack() const;
    void goForward() const;
    void evaluateJS(std::string_view js);
    void setScalesPageToFit(const bool scalesPageToFit);
    void setWebViewVisible(const bool visible) const;
    void setBounces(bool bounces);
    void setOpacityWebView(float opacity);
    float getOpacityWebView() const;
    void setBackgroundTransparent();

private:
    ICoreWebView2* m_webview              = nullptr;
    ICoreWebView2Controller* m_controller = nullptr;
    HWND m_window{};
    POINT m_minsz       = POINT{0, 0};
    POINT m_maxsz       = POINT{0, 0};
    DWORD m_main_thread = GetCurrentThreadId();

    std::string m_jsScheme;
    bool _scalesPageToFit{};

    std::function<bool(std::string_view)> _shouldStartLoading;
    std::function<void(std::string_view)> _didFinishLoading;
    std::function<void(std::string_view)> _didFailLoading;
    std::function<void(std::string_view)> _onJsCallback;

    static bool s_isInitialized;
    static void lazyInit();

    bool embed(HWND wnd, bool debug, msg_cb_t cb)
    {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
        flag.test_and_set();

        char currentExePath[MAX_PATH];
        GetModuleFileNameA(NULL, currentExePath, MAX_PATH);
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
        const char* currentExeName = PathFindFileNameA(currentExePath);
#    else
        const char* currentExeName = "axmol-app";
#    endif

        /*
         * Note: New OS feature 'Beta: Use Unicode UTF-8 for worldwide language support' since win10/win11
         *   - OFF: GetACP() equal to current system locale, such as chinese simplified is 936, english is 437
         *   - ON: GetACP() always equal to 65001(UTF-8)
         * Remark:
         *   The macro CP_ACP for ntcvt::from_chars works for converting chraset from current code page(936,437,65001)
         * to utf-16
         */
        std::wstring userDataFolder  = ntcvt::from_chars(std::getenv("APPDATA"), CP_ACP);
        std::wstring currentExeNameW = ntcvt::from_chars(currentExeName, CP_ACP);

        HRESULT res = CreateCoreWebView2EnvironmentWithOptions(
            nullptr, (userDataFolder + L"/" + currentExeNameW).c_str(), nullptr,
            new webview2_com_handler(
                wnd, cb,
                [&](ICoreWebView2Controller* controller) {
                    m_controller = controller;
                    m_controller->get_CoreWebView2(&m_webview);
                    m_webview->AddRef();
                    flag.clear();
                },
                [this](std::string_view url) -> bool {
                    const auto scheme = url.substr(0, url.find_first_of(':'));
                    if (scheme == m_jsScheme)
                    {
                        if (_onJsCallback)
                        {
                            _onJsCallback(url);
                        }
                        return true;
                    }

                    if (_shouldStartLoading && !url.empty())
                    {
                        return _shouldStartLoading(url);
                    }
                    return true;
                },
                [this]() {
                    LPWSTR uri;
                    this->m_webview->get_Source(&uri);
                    std::string result = ntcvt::from_chars(uri);
                    if (_didFinishLoading)
                        _didFinishLoading(result);
                },
                [this]() {
                    LPWSTR uri;
                    this->m_webview->get_Source(&uri);
                    std::string result = ntcvt::from_chars(uri);
                    if (_didFailLoading)
                        _didFailLoading(result);
                },
                [this](std::string_view url) { loadURL(url, false); }));

        if (res != S_OK)
        {
            CoUninitialize();
            return false;
        }
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
        MSG msg = {};
        while (flag.test_and_set() && GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#    endif
        init("window.external={invoke:s=>window.chrome.webview.postMessage(s)}");
        return true;
    }

    void resize(HWND wnd)
    {
        if (m_controller == nullptr)
        {
            return;
        }
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
        RECT bounds;
        GetClientRect(wnd, &bounds);
        m_controller->put_Bounds(bounds);
#    endif
    }

    void navigate(std::string_view url)
    {
        auto wurl = ntcvt::from_chars(url.data());
        m_webview->Navigate(wurl.c_str());
    }

    void init(std::string_view js)
    {
        auto wjs = ntcvt::from_chars(js);
        m_webview->AddScriptToExecuteOnDocumentCreated(wjs.c_str(), nullptr);
    }

    void eval(std::string_view js)
    {
        auto wjs = ntcvt::from_chars(js);
        m_webview->ExecuteScript(wjs.c_str(), nullptr);
    }

    void on_message(std::string_view msg)
    {
        const auto seq  = jsonParse(msg, "id", 0);
        const auto name = jsonParse(msg, "method", 0);
        const auto args = jsonParse(msg, "params", 0);
        if (bindings.find(name) == bindings.end())
        {
            return;
        }
        const auto fn = bindings[name];
        (*fn->first)(seq, args, fn->second);
    }

    using binding_t     = std::function<void(std::string, std::string, void*)>;
    using binding_ctx_t = std::pair<binding_t*, void*>;
    std::map<std::string, binding_ctx_t*> bindings;

    class webview2_com_handler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
                                 public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
                                 public ICoreWebView2WebMessageReceivedEventHandler,
                                 public ICoreWebView2PermissionRequestedEventHandler,
                                 public ICoreWebView2NavigationStartingEventHandler,
                                 public ICoreWebView2NavigationCompletedEventHandler,
                                 public ICoreWebView2NewWindowRequestedEventHandler,
                                 public ICoreWebView2ContentLoadingEventHandler,
                                 public ICoreWebView2DOMContentLoadedEventHandler
    {
        using webview2_com_handler_cb_t = std::function<void(ICoreWebView2Controller*)>;

    public:
        webview2_com_handler(HWND hwnd,
                             msg_cb_t msgCb,
                             webview2_com_handler_cb_t cb,
                             std::function<bool(std::string_view)> navStartingCallback,
                             std::function<void()> navCompleteCallback,
                             std::function<void()> navErrorCallback,
                             std::function<void(std::string_view url)> loadUrlCallback)
            : m_window(hwnd)
            , m_msgCb(std::move(msgCb))
            , m_cb(std::move(cb))
            , m_navStartingCallback(std::move(navStartingCallback))
            , m_navCompleteCallback(std::move(navCompleteCallback))
            , m_navErrorCallback(std::move(navErrorCallback))
            , m_loadUrlCallback(std::move(loadUrlCallback))
        {}
        ULONG STDMETHODCALLTYPE AddRef() { return 1; }
        ULONG STDMETHODCALLTYPE Release() { return 1; }
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv) { return S_OK; }

        // ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Environment* env)
        {
            env->CreateCoreWebView2Controller(m_window, this);
            return S_OK;
        }

        // ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Controller* controller)
        {
            controller->AddRef();

            ICoreWebView2* webview;
            ::EventRegistrationToken token;
            controller->get_CoreWebView2(&webview);
            webview->add_WebMessageReceived(this, &token);
            webview->add_PermissionRequested(this, &token);
            webview->add_NavigationStarting(this, &token);
            webview->add_NavigationCompleted(this, &token);
            webview->add_ContentLoading(this, &token);
            webview->add_NewWindowRequested(this, &token);

            m_cb(controller);
            return S_OK;
        }

        // ICoreWebView2WebMessageReceivedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args)
        {
            LPWSTR message;
            args->TryGetWebMessageAsString(&message);

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wideCharConverter;
            m_msgCb(wideCharConverter.to_bytes(message));
            sender->PostWebMessageAsString(message);

            CoTaskMemFree(message);
            return S_OK;
        }

        // ICoreWebView2PermissionRequestedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2PermissionRequestedEventArgs* args)
        {
            COREWEBVIEW2_PERMISSION_KIND kind;
            args->get_PermissionKind(&kind);
            if (kind == COREWEBVIEW2_PERMISSION_KIND_CLIPBOARD_READ)
            {
                args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
            }
            return S_OK;
        }

        // ICoreWebView2NavigationStartingEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
        {
            if (m_navStartingCallback)
            {
                const auto uriString = getUriStringFromArgs(args);
                if (!m_navStartingCallback(uriString))
                {
                    args->put_Cancel(true);
                }
            }

            return S_OK;
        }

        // ICoreWebView2NavigationCompletedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args)
        {
            BOOL success;
            if (SUCCEEDED(args->get_IsSuccess(&success)) && success)
            {
                if (m_navCompleteCallback)
                {
                    m_navCompleteCallback();
                }
            }
            else
            {
                // example of how to get status error if required
                // COREWEBVIEW2_WEB_ERROR_STATUS status;
                // if (SUCCEEDED(args->get_WebErrorStatus(&status)))
                //{
                //}

                if (m_navErrorCallback)
                {
                    m_navErrorCallback();
                }
            }
            return S_OK;
        }

        // ICoreWebView2ContentLoadingEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2ContentLoadingEventArgs* args)
        {
            return S_OK;
        }

        // ICoreWebView2NewWindowRequestedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args)
        {
            args->put_Handled(true);
            return S_OK;
        }

        // ICoreWebView2NewWindowRequestedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs2* args)
        {
            args->put_Handled(true);

            if (m_loadUrlCallback)
            {
                const auto uriString = getUriStringFromArgs(args);
                m_loadUrlCallback(uriString);
            }
            args->put_Handled(true);
            return S_OK;
        }

        // ICoreWebView2DOMContentLoadedEventHandler
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2DOMContentLoadedEventArgs* args)
        {
            return S_OK;
        }

    private:
        HWND m_window;
        msg_cb_t m_msgCb;
        webview2_com_handler_cb_t m_cb;
        std::function<bool(std::string_view)> m_navStartingCallback;
        std::function<void()> m_navCompleteCallback;
        std::function<void()> m_navErrorCallback;
        std::function<void(std::string_view url)> m_loadUrlCallback;
    };
};

NS_AX_BEGIN

namespace ui
{
WebViewImpl::WebViewImpl(WebView* webView) : _createSucceeded(false), _systemWebControl(nullptr), _webView(webView)
{
    _systemWebControl = new Win32WebControl();
    if (_systemWebControl == nullptr)
    {
        return;
    }

    _createSucceeded = _systemWebControl->createWebView(
        [this](std::string_view url) -> bool {
            const auto shouldStartLoading = _webView->getOnShouldStartLoading();
            if (shouldStartLoading != nullptr)
            {
                return shouldStartLoading(_webView, url);
            }
            return true;
        },
        [this](std::string_view url) {
            WebView::ccWebViewCallback didFinishLoading = _webView->getOnDidFinishLoading();
            if (didFinishLoading != nullptr)
            {
                didFinishLoading(_webView, url);
            }
        },
        [this](std::string_view url) {
            WebView::ccWebViewCallback didFailLoading = _webView->getOnDidFailLoading();
            if (didFailLoading != nullptr)
            {
                didFailLoading(_webView, url);
            }
        },
        [this](std::string_view url) {
            WebView::ccWebViewCallback onJsCallback = _webView->getOnJSCallback();
            if (onJsCallback != nullptr)
            {
                onJsCallback(_webView, url);
            }
        });
}

WebViewImpl::~WebViewImpl()
{
    if (_systemWebControl != nullptr)
    {
        _systemWebControl->removeWebView();
        delete _systemWebControl;
        _systemWebControl = nullptr;
    }
}

void WebViewImpl::loadData(const Data& data,
                           std::string_view MIMEType,
                           std::string_view encoding,
                           std::string_view baseURL)
{
    if (_createSucceeded)
    {
        const auto url = getDataURI(data, MIMEType);
        _systemWebControl->loadURL(url, false);
    }
}

void WebViewImpl::loadHTMLString(std::string_view string, std::string_view baseURL)
{
    if (_createSucceeded)
    {
        if (string.empty())
        {
            _systemWebControl->loadHTMLString("data:text/html," + utils::urlEncode("<html></html>"), baseURL);
            return;
        }

        const auto html = htmlFromUri(string);
        if (!html.empty())
        {
            _systemWebControl->loadHTMLString("data:text/html," + utils::urlEncode(html), baseURL);
        }
        else
        {
            _systemWebControl->loadHTMLString(string, baseURL);
        }
    }
}

void WebViewImpl::loadURL(std::string_view url, bool cleanCachedData)
{
    if (_createSucceeded)
    {
        _systemWebControl->loadURL(url, cleanCachedData);
    }
}

void WebViewImpl::loadFile(std::string_view fileName)
{
    if (_createSucceeded)
    {
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
        _systemWebControl->loadFile(fullPath);
    }
}

void WebViewImpl::stopLoading()
{
    if (_createSucceeded)
    {
        _systemWebControl->stopLoading();
    }
}

void WebViewImpl::reload()
{
    if (_createSucceeded)
    {
        _systemWebControl->reload();
    }
}

bool WebViewImpl::canGoBack()
{
    if (_createSucceeded)
    {
        return _systemWebControl->canGoBack();
    }
    return false;
}

bool WebViewImpl::canGoForward()
{
    if (_createSucceeded)
    {
        return _systemWebControl->canGoForward();
    }
    return false;
}

void WebViewImpl::goBack()
{
    if (_createSucceeded)
    {
        _systemWebControl->goBack();
    }
}

void WebViewImpl::goForward()
{
    if (_createSucceeded)
    {
        _systemWebControl->goForward();
    }
}

void WebViewImpl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    if (_createSucceeded)
    {
        _systemWebControl->setJavascriptInterfaceScheme(scheme);
    }
}

void WebViewImpl::evaluateJS(std::string_view js)
{
    if (_createSucceeded)
    {
        _systemWebControl->evaluateJS(js);
    }
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit)
{
    if (_createSucceeded)
    {
        _systemWebControl->setScalesPageToFit(scalesPageToFit);
    }
}

void WebViewImpl::draw(Renderer* renderer, Mat4 const& transform, uint32_t flags)
{
    if (_createSucceeded && (flags & Node::FLAGS_TRANSFORM_DIRTY))
    {
        const auto uiRect = ax::ui::Helper::convertBoundingBoxToScreen(_webView);
        _systemWebControl->setWebViewRect(static_cast<int>(uiRect.origin.x), static_cast<int>(uiRect.origin.y),
                                          static_cast<int>(uiRect.size.width), static_cast<int>(uiRect.size.height));
    }
}

void WebViewImpl::setVisible(bool visible)
{
    if (_createSucceeded)
    {
        _systemWebControl->setWebViewVisible(visible);
    }
}

void WebViewImpl::setBounces(bool bounces)
{
    _systemWebControl->setBounces(bounces);
}

void WebViewImpl::setOpacityWebView(float opacity)
{
    _systemWebControl->setOpacityWebView(opacity);
}

float WebViewImpl::getOpacityWebView() const
{
    return _systemWebControl->getOpacityWebView();
}

void WebViewImpl::setBackgroundTransparent()
{
    _systemWebControl->setBackgroundTransparent();
}
}  // namespace ui
NS_AX_END  // namespace ax

//
// Implement Win32WebControl
//
bool Win32WebControl::s_isInitialized = false;

void Win32WebControl::lazyInit()
{
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    // reset the main windows style so that its drawing does not cover the webview sub window
    auto hwnd        = ax::Director::getInstance()->getGLView()->getWin32Window();
    const auto style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style | WS_CLIPCHILDREN);

    std::ignore = CoInitialize(NULL);
#    endif
}

Win32WebControl::Win32WebControl() : _shouldStartLoading(nullptr), _didFinishLoading(nullptr), _didFailLoading(nullptr)
{
    if (!s_isInitialized)
    {
        lazyInit();
    }
}

bool Win32WebControl::createWebView(const std::function<bool(std::string_view)>& shouldStartLoading,
                                    const std::function<void(std::string_view)>& didFinishLoading,
                                    const std::function<void(std::string_view)>& didFailLoading,
                                    const std::function<void(std::string_view)>& onJsCallback)
{
    bool ret = false;
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    do
    {
        HWND hwnd           = ax::Director::getInstance()->getGLView()->getWin32Window();
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.hInstance     = hInstance;
        wc.lpszClassName = L"webview";
        wc.hIcon         = nullptr;
        wc.hIconSm       = nullptr;
        // wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> int {
            auto w = (Win32WebControl*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (!w)
            {
                return DefWindowProc(hwnd, msg, wp, lp);
            }

            switch (msg)
            {
            case WM_SIZE:
                w->resize(hwnd);
                break;

            case WM_CLOSE:
                DestroyWindow(hwnd);
                break;

            case WM_GETMINMAXINFO:
            {
                auto lpmmi = (LPMINMAXINFO)lp;
                if (w == nullptr)
                {
                    return 0;
                }
                if (w->m_maxsz.x > 0 && w->m_maxsz.y > 0)
                {
                    lpmmi->ptMaxSize      = w->m_maxsz;
                    lpmmi->ptMaxTrackSize = w->m_maxsz;
                }
                if (w->m_minsz.x > 0 && w->m_minsz.y > 0)
                {
                    lpmmi->ptMinTrackSize = w->m_minsz;
                }
            }
            break;

            default:
                return DefWindowProc(hwnd, msg, wp, lp);
            }
            return 0;
        });
        RegisterClassEx(&wc);
        m_window = CreateWindow(L"webview", L"", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hwnd, nullptr, hInstance, nullptr);
        SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

        ShowWindow(m_window, SW_SHOW);
        UpdateWindow(m_window);
        SetFocus(m_window);

        auto cb = [this](std::string_view msg) { on_message(msg); };

        if (!embed(m_window, false, cb))
        {
            AXLOGD("Cannot create edge chromium webview");
            ret = false;
            break;
        }

        resize(m_window);
        ret = true;
    } while (0);

    if (!ret)
    {
        removeWebView();
    }

    _shouldStartLoading = shouldStartLoading;
    _didFinishLoading   = didFinishLoading;
    _didFailLoading     = didFailLoading;
    _onJsCallback       = onJsCallback;
#    endif
    return ret;
}

void Win32WebControl::removeWebView()
{
    m_controller->Close();

    m_controller = nullptr;
    m_webview    = nullptr;
}

void Win32WebControl::setWebViewRect(const int left, const int top, const int width, const int height)
{
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    if (m_controller == nullptr)
    {
        return;
    }

    RECT r;
    r.left   = left;
    r.top    = top;
    r.right  = left + width;
    r.bottom = top - height;
    AdjustWindowRect(&r, WS_CHILD | WS_VISIBLE, 0);
    SetWindowPos(m_window, nullptr, left, top, width, height, SWP_NOZORDER);

    m_controller->put_ZoomFactor(_scalesPageToFit ? getDeviceScaleFactor() : 1.0);
#    endif
}

void Win32WebControl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    m_jsScheme = scheme;
}

void Win32WebControl::loadHTMLString(std::string_view html, std::string_view baseURL)
{
    if (!html.empty())
    {
        const auto whtml = ntcvt::from_chars(html);
        m_webview->NavigateToString(whtml.c_str());
    }
}

void Win32WebControl::loadURL(std::string_view url, bool cleanCachedData)
{
    if (cleanCachedData)
    {
        m_webview->CallDevToolsProtocolMethod(L"Network.clearBrowserCache", L"{}", nullptr);
    }
    navigate(url);
}

void Win32WebControl::loadFile(std::string_view filePath)
{
    auto fullPath = ax::FileUtils::getInstance()->fullPathForFilename(filePath);
    if (fullPath.find("file:///") != 0)
    {
        if (fullPath[0] == '/')
        {
            fullPath = "file://" + fullPath;
        }
        else
        {
            fullPath = "file:///" + fullPath;
        }
    }
    loadURL(fullPath, false);
}

void Win32WebControl::stopLoading()
{
    m_webview->Stop();
}

void Win32WebControl::reload() const
{
    m_webview->Reload();
}

bool Win32WebControl::canGoBack() const
{
    BOOL result;
    return SUCCEEDED(m_webview->get_CanGoBack(&result)) && result;
}

bool Win32WebControl::canGoForward() const
{
    BOOL result;
    return SUCCEEDED(m_webview->get_CanGoForward(&result)) && result;
}

void Win32WebControl::goBack() const
{
    m_webview->GoBack();
}

void Win32WebControl::goForward() const
{
    m_webview->GoForward();
}

void Win32WebControl::evaluateJS(std::string_view js)
{
    eval(js);
}

void Win32WebControl::setScalesPageToFit(const bool scalesPageToFit)
{
    _scalesPageToFit = scalesPageToFit;
    m_controller->put_ZoomFactor(_scalesPageToFit ? getDeviceScaleFactor() : 1.0);
}

void Win32WebControl::setWebViewVisible(const bool visible) const
{
#    if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    ShowWindow(m_window, visible ? SW_SHOW : SW_HIDE);
    if (!visible)
    {
        // When the frame window is minimized set the browser window size to 0x0 to
        // reduce resource usage.
        SetWindowPos(m_window, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
#    endif
}

void Win32WebControl::setBounces(bool bounces) {}

void Win32WebControl::setOpacityWebView(float opacity) {}

float Win32WebControl::getOpacityWebView() const
{
    return 1.0f;
}

void Win32WebControl::setBackgroundTransparent()
{
    ICoreWebView2Controller2* viewController2;
    if (SUCCEEDED(m_controller->QueryInterface(__uuidof(ICoreWebView2Controller2),
                                               reinterpret_cast<void**>(&viewController2))))
    {
        COREWEBVIEW2_COLOR color;
        viewController2->get_DefaultBackgroundColor(&color);
        color.A = 0;  // Only supports 0 or 255. Other values not supported
        viewController2->put_DefaultBackgroundColor(color);
    }
}

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
