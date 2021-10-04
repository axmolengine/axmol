/****************************************************************************
 MIT License
 
 Portions copyright (c) 2017 Serge Zaitsev
 
 https://adxe.org
 
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

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#include "UIWebViewImpl-win32.h"
#include "UIWebView.h"
#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"
#include "platform/CCGLView.h"
#include "base/base64.h"
#include "ui/UIHelper.h"

#define WIN32_LEAN_AND_MEAN
#include <Shlwapi.h>
#include <codecvt>
#include <utility>
#include <stdlib.h>
#include <Windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shlwapi.lib")

// Edge/Chromium headers and libs
#include "WebView2.h"
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

#include "platform/CCPlatformConfig.h"

using msg_cb_t = std::function<void(const std::string)>;

// Convert ASCII hex digit to a nibble (four bits, 0 - 15).
//
// Use unsigned to avoid signed overflow UB.
static inline unsigned char hex2nibble(unsigned char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    else if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    return 0;
}

// Convert ASCII hex string (two characters) to byte.
//
// E.g., "0B" => 0x0B, "af" => 0xAF.
static inline char hex2char(const char* p)
{
    return hex2nibble(p[0]) * 16 + hex2nibble(p[1]);
}

inline std::string url_encode(const std::string s)
{
    std::string encoded;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        auto c = s[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded = encoded + c;
        }
        else
        {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02x", c);
            encoded = encoded + hex;
        }
    }
    return encoded;
}

inline std::string url_decode(const std::string st)
{
    std::string decoded;
    const char* s = st.c_str();
    size_t length = strlen(s);
    for (unsigned int i = 0; i < length; i++)
    {
        if (s[i] == '%')
        {
            decoded.push_back(hex2char(s + i + 1));
            i = i + 2;
        }
        else if (s[i] == '+')
        {
            decoded.push_back(' ');
        }
        else
        {
            decoded.push_back(s[i]);
        }
    }
    return decoded;
}

inline std::string html_from_uri(const std::string& s)
{
    if (s.substr(0, 15) == "data:text/html,")
    {
        return url_decode(s.substr(15));
    }
    return "";
}

inline int json_parse_c(const char* s, size_t sz, const char* key, size_t keysz, const char** value, size_t* valuesz)
{
    enum
    {
        JSON_STATE_VALUE,
        JSON_STATE_LITERAL,
        JSON_STATE_STRING,
        JSON_STATE_ESCAPE,
        JSON_STATE_UTF8
    } state = JSON_STATE_VALUE;
    const char* k  = NULL;
    int index      = 1;
    int depth      = 0;
    int utf8_bytes = 0;

    if (key == NULL)
    {
        index = keysz;
        keysz = 0;
    }

    *value   = NULL;
    *valuesz = 0;

    for (; sz > 0; s++, sz--)
    {
        enum
        {
            JSON_ACTION_NONE,
            JSON_ACTION_START,
            JSON_ACTION_END,
            JSON_ACTION_START_STRUCT,
            JSON_ACTION_END_STRUCT
        } action        = JSON_ACTION_NONE;
        unsigned char c = *s;
        switch (state)
        {
            case JSON_STATE_VALUE:
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' || c == ':')
                {
                    continue;
                }
                else if (c == '"')
                {
                    action = JSON_ACTION_START;
                    state  = JSON_STATE_STRING;
                }
                else if (c == '{' || c == '[')
                {
                    action = JSON_ACTION_START_STRUCT;
                }
                else if (c == '}' || c == ']')
                {
                    action = JSON_ACTION_END_STRUCT;
                }
                else if (c == 't' || c == 'f' || c == 'n' || c == '-' || (c >= '0' && c <= '9'))
                {
                    action = JSON_ACTION_START;
                    state  = JSON_STATE_LITERAL;
                }
                else
                {
                    return -1;
                }
                break;
            case JSON_STATE_LITERAL:
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' || c == ']' || c == '}' || c == ':')
                {
                    state = JSON_STATE_VALUE;
                    s--;
                    sz++;
                    action = JSON_ACTION_END;
                }
                else if (c < 32 || c > 126)
                {
                    return -1;
                }  // fallthrough
            case JSON_STATE_STRING:
                if (c < 32 || (c > 126 && c < 192))
                {
                    return -1;
                }
                else if (c == '"')
                {
                    action = JSON_ACTION_END;
                    state  = JSON_STATE_VALUE;
                }
                else if (c == '\\')
                {
                    state = JSON_STATE_ESCAPE;
                }
                else if (c >= 192 && c < 224)
                {
                    utf8_bytes = 1;
                    state      = JSON_STATE_UTF8;
                }
                else if (c >= 224 && c < 240)
                {
                    utf8_bytes = 2;
                    state      = JSON_STATE_UTF8;
                }
                else if (c >= 240 && c < 247)
                {
                    utf8_bytes = 3;
                    state      = JSON_STATE_UTF8;
                }
                else if (c >= 128 && c < 192)
                {
                    return -1;
                }
                break;
            case JSON_STATE_ESCAPE:
                if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' ||
                    c == 'u')
                {
                    state = JSON_STATE_STRING;
                }
                else
                {
                    return -1;
                }
                break;
            case JSON_STATE_UTF8:
                if (c < 128 || c > 191)
                {
                    return -1;
                }
                utf8_bytes--;
                if (utf8_bytes == 0)
                {
                    state = JSON_STATE_STRING;
                }
                break;
            default:
                return -1;
        }

        if (action == JSON_ACTION_END_STRUCT)
        {
            depth--;
        }

        if (depth == 1)
        {
            if (action == JSON_ACTION_START || action == JSON_ACTION_START_STRUCT)
            {
                if (index == 0)
                {
                    *value = s;
                }
                else if (keysz > 0 && index == 1)
                {
                    k = s;
                }
                else
                {
                    index--;
                }
            }
            else if (action == JSON_ACTION_END || action == JSON_ACTION_END_STRUCT)
            {
                if (*value != NULL && index == 0)
                {
                    *valuesz = (size_t)(s + 1 - *value);
                    return 0;
                }
                else if (keysz > 0 && k != NULL)
                {
                    if (keysz == (size_t)(s - k - 1) && memcmp(key, k + 1, keysz) == 0)
                    {
                        index = 0;
                    }
                    else
                    {
                        index = 2;
                    }
                    k = NULL;
                }
            }
        }

        if (action == JSON_ACTION_START_STRUCT)
        {
            depth++;
        }
    }
    return -1;
}

inline std::string json_escape(std::string s)
{
    // TODO: implement
    return '"' + s + '"';
}

inline int json_unescape(const char* s, size_t n, char* out)
{
    int r = 0;
    if (*s++ != '"')
    {
        return -1;
    }
    while (n > 2)
    {
        char c = *s;
        if (c == '\\')
        {
            s++;
            n--;
            switch (*s)
            {
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '/':
                    c = '/';
                    break;
                case '\"':
                    c = '\"';
                    break;
                default:  // TODO: support unicode decoding
                    return -1;
            }
        }
        if (out != NULL)
        {
            *out++ = c;
        }
        s++;
        n--;
        r++;
    }
    if (*s != '"')
    {
        return -1;
    }
    if (out != NULL)
    {
        *out = '\0';
    }
    return r;
}

inline std::string json_parse(const std::string s, const std::string key, const int index)
{
    const char* value;
    size_t value_sz;
    if (key.empty())
    {
        json_parse_c(s.c_str(), s.length(), nullptr, index, &value, &value_sz);
    }
    else
    {
        json_parse_c(s.c_str(), s.length(), key.c_str(), key.length(), &value, &value_sz);
    }
    if (value != nullptr)
    {
        if (value[0] != '"')
        {
            return std::string(value, value_sz);
        }
        int n = json_unescape(value, value_sz, nullptr);
        if (n > 0)
        {
            char* decoded = new char[n + 1];
            json_unescape(value, value_sz, decoded);
            std::string result(decoded, n);
            delete[] decoded;
            return result;
        }
    }
    return "";
}

template <class ArgType>
static std::string getUriStringFromArgs(ArgType* args)
{
    if (args != nullptr)
    {
        LPWSTR uri;
        args->get_Uri(&uri);
        std::wstring ws(uri);
        std::string result = std::string(ws.begin(), ws.end());

        return result;
    }

    return {};
}

static std::string GetDataURI(const std::string& data, const std::string& mime_type)
{
    char* encodedData;
    cocos2d::base64Encode(reinterpret_cast<const unsigned char*>(data.data()), static_cast<unsigned>(data.size()), &encodedData);
    return "data:" + mime_type + ";base64," + url_encode(encodedData);
}

static double GetDeviceScaleFactor()
{
    static double scale_factor = 1.0;
    static bool initialized    = false;

    if (!initialized)
    {
        // This value is safe to cache for the life time of the app since the user
        // must logout to change the DPI setting. This value also applies to all
        // screens.
#    if _WIN32
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

    bool createWebView(
        const std::function<bool (const std::string &)> &shouldStartLoading,
        const std::function<void (const std::string &)> &didFinishLoading,
        const std::function<void (const std::string &)> &didFailLoading,
        const std::function<void (const std::string &)> &onJsCallback);
    void removeWebView();

    void setWebViewRect(const int left, const int top, const int width, const int height);
    void setJavascriptInterfaceScheme(const std::string &scheme);
    void loadHTMLString(const std::string &html, const std::string &baseURL);
    void loadURL(const std::string &url, bool cleanCachedData);
    void loadFile(const std::string &filePath);
    void stopLoading();
    void reload() const;
    bool canGoBack() const;
    bool canGoForward() const;
    void goBack() const;
    void goForward() const;
    void evaluateJS(const std::string &js);
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

    std::function<bool (const std::string &)> _shouldStartLoading;
    std::function<void (const std::string &)> _didFinishLoading;
    std::function<void (const std::string &)> _didFailLoading;
    std::function<void (const std::string &)> _onJsCallback;

    static bool s_isInitialized;
    static void lazyInit();

    static LPWSTR to_lpwstr(const std::string s)
    {
        const int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
        auto* ws = new wchar_t[n];
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, ws, n);
        return ws;
    }

    bool embed(HWND wnd, bool debug, msg_cb_t cb)
    {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
        flag.test_and_set();

        char currentExePath[MAX_PATH];
        GetModuleFileNameA(NULL, currentExePath, MAX_PATH);
        char* currentExeName = PathFindFileNameA(currentExePath);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wideCharConverter;
        std::wstring userDataFolder  = wideCharConverter.from_bytes(std::getenv("APPDATA"));
        std::wstring currentExeNameW = wideCharConverter.from_bytes(currentExeName);

        HRESULT res = CreateCoreWebView2EnvironmentWithOptions(
            nullptr, (userDataFolder + L"/" + currentExeNameW).c_str(), nullptr,
            new webview2_com_handler(wnd, cb, 
                    [&](ICoreWebView2Controller* controller) {
                        m_controller = controller;
                        m_controller->get_CoreWebView2(&m_webview);
                        m_webview->AddRef();
                        flag.clear();
                    },
                  [this](const std::string& url) -> bool {
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
                        std::wstring ws(uri);
                        const auto result = std::string(ws.begin(), ws.end());
                        if (_didFinishLoading)
                            _didFinishLoading(result);
                    },
                    [this]() {
                        LPWSTR uri;
                        this->m_webview->get_Source(&uri);
                        std::wstring ws(uri);
                        const auto result = std::string(ws.begin(), ws.end());
                        if (_didFailLoading)
                            _didFailLoading(result);
                    },
                    [this](const std::string& url) { loadURL(url, false); }
                )
        );

        if (res != S_OK)
        {
            CoUninitialize();
            return false;
        }
        MSG msg = {};
        while (flag.test_and_set() && GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        init("window.external={invoke:s=>window.chrome.webview.postMessage(s)}");
        return true;
    }

    void resize(HWND wnd)
    {
        if (m_controller == nullptr)
        {
            return;
        }
        RECT bounds;
        GetClientRect(wnd, &bounds);
        m_controller->put_Bounds(bounds);
    }

    void navigate(const std::string url)
    {
        auto wurl = to_lpwstr(url);
        m_webview->Navigate(wurl);
        delete[] wurl;
    }

    void init(const std::string js)
    {
        LPCWSTR wjs = to_lpwstr(js);
        m_webview->AddScriptToExecuteOnDocumentCreated(wjs, nullptr);
        delete[] wjs;
    }

    void eval(const std::string js)
    {
        LPCWSTR wjs = to_lpwstr(js);
        m_webview->ExecuteScript(wjs, nullptr);
        delete[] wjs;
    }
    
    void on_message(const std::string msg)
    {
        const auto seq  = json_parse(msg, "id", 0);
        const auto name = json_parse(msg, "method", 0);
        const auto args = json_parse(msg, "params", 0);
        if (bindings.find(name) == bindings.end())
        {
            return;
        }
        const auto fn = bindings[name];
        (*fn->first)(seq, args, fn->second);
    }

    using binding_t = std::function<void(std::string, std::string, void *)>;
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
                             std::function<bool(const std::string&)> navStartingCallback,
                             std::function<void()> navCompleteCallback,
                             std::function<void()> navErrorCallback,
                             std::function<void(const std::string& url)> loadUrlCallback
            )
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
            if (args->get_IsSuccess(&success) == 0 && success)
            {
                if (m_navCompleteCallback)
                {
                    m_navCompleteCallback();
                }
            }
            else
            {
                COREWEBVIEW2_WEB_ERROR_STATUS status;

                if (args->get_WebErrorStatus(&status) == 0)
                {
                    // CCLOG("")
                }

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
        std::function<bool(const std::string&)> m_navStartingCallback;
        std::function<void()> m_navCompleteCallback;
        std::function<void()> m_navErrorCallback;
        std::function<void(const std::string& url)> m_loadUrlCallback;
    };
};

namespace cocos2d {
    namespace ui {
        WebViewImpl::WebViewImpl(WebView *webView) : _createSucceeded(false), _systemWebControl(nullptr), _webView(webView)
        {
            _systemWebControl = new Win32WebControl();
            if (_systemWebControl == nullptr)
            {
                return;
            }

            _createSucceeded = _systemWebControl->createWebView(
                [this](const std::string &url)->bool {
                    const auto shouldStartLoading = _webView->getOnShouldStartLoading();
                    if (shouldStartLoading != nullptr)
                    {
                        return shouldStartLoading(_webView, url);
                    }
                    return true;
                },
                [this](const std::string &url) {
                    WebView::ccWebViewCallback didFinishLoading = _webView->getOnDidFinishLoading();
                    if (didFinishLoading != nullptr)
                    {
                        didFinishLoading(_webView, url);
                    }
                },
                [this](const std::string &url) {
                    WebView::ccWebViewCallback didFailLoading = _webView->getOnDidFailLoading();
                    if (didFailLoading != nullptr)
                    {
                        didFailLoading(_webView, url);
                    }
                },
                [this](const std::string &url) {
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

        void WebViewImpl::loadData(const Data &data, const std::string &MIMEType, const std::string &encoding, const std::string &baseURL)
        {
            if (_createSucceeded)
            {
                const std::string dataString(reinterpret_cast<char*>(data.getBytes()), static_cast<unsigned int>(data.getSize()));
                const auto url = GetDataURI(dataString, MIMEType);
                _systemWebControl->loadURL(url, false);
            }
        }

        void WebViewImpl::loadHTMLString(const std::string &string, const std::string &baseURL)
        {
            if (_createSucceeded)
            {
                if (string.empty())
                {
                    _systemWebControl->loadHTMLString(
                        "data:text/html," + url_encode("<html></html>"), baseURL);
                    return;
                }

                const auto html = html_from_uri(string);
                if (!html.empty())
                {
                    _systemWebControl->loadHTMLString("data:text/html," + url_encode(html), baseURL);
                }
                else
                {
                    _systemWebControl->loadHTMLString(string, baseURL);
                }
            }
        }

        void WebViewImpl::loadURL(const std::string &url, bool cleanCachedData)
        {
            if (_createSucceeded)
            {
                _systemWebControl->loadURL(url, cleanCachedData);
            }
        }

        void WebViewImpl::loadFile(const std::string &fileName)
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

        void WebViewImpl::setJavascriptInterfaceScheme(const std::string &scheme)
        {
            if (_createSucceeded)
            {
                _systemWebControl->setJavascriptInterfaceScheme(scheme);
            }
        }

        void WebViewImpl::evaluateJS(const std::string &js)
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

        void WebViewImpl::draw(Renderer *renderer, Mat4 const &transform, uint32_t flags)
        {
            if (_createSucceeded && (flags & Node::FLAGS_TRANSFORM_DIRTY))
            {
                const auto uiRect = cocos2d::ui::Helper::convertBoundingBoxToScreen(_webView);
                _systemWebControl->setWebViewRect(static_cast<int>(uiRect.origin.x), static_cast<int>(uiRect.origin.y),
                                                  static_cast<int>(uiRect.size.width),
                                                  static_cast<int>(uiRect.size.height));
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
    } // namespace ui
} //namespace cocos2d

//
// Implement Win32WebControl
//
bool Win32WebControl::s_isInitialized = false;

void Win32WebControl::lazyInit()
{
    // reset the main windows style so that its drawing does not cover the webview sub window
    auto hwnd  = cocos2d::Director::getInstance()->getOpenGLView()->getWin32Window();
    const auto style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style | WS_CLIPCHILDREN);

    CoInitialize(NULL);
}

Win32WebControl::Win32WebControl() :
    _shouldStartLoading(nullptr)
    , _didFinishLoading(nullptr)
    , _didFailLoading(nullptr)
{
    if (!s_isInitialized)
    {
        lazyInit();
    }
}

bool Win32WebControl::createWebView(
    const std::function<bool (const std::string &)> &shouldStartLoading,
    const std::function<void (const std::string &)> &didFinishLoading,
    const std::function<void (const std::string &)> &didFailLoading,
    const std::function<void (const std::string &)> &onJsCallback)
{
    bool ret = false;
    do
    {
        HWND hwnd = cocos2d::Director::getInstance()->getOpenGLView()->getWin32Window();
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.hInstance     = hInstance;
        wc.lpszClassName = L"webview";
        wc.hIcon         = nullptr;
        wc.hIconSm       = nullptr;
        //wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc   = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> int {
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
        m_window = CreateWindow(L"webview", L"", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hwnd, nullptr,
                                hInstance, nullptr);
        SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

        //SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
        ShowWindow(m_window, SW_SHOW);
        UpdateWindow(m_window);
        SetFocus(m_window);

        auto cb = [this](const std::string msg) { on_message(msg); };

        if (!embed(m_window, false, cb))
        {
            CCLOG("Cannot create edge chromium webview");
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
    _didFinishLoading = didFinishLoading;
    _didFailLoading = didFailLoading;
    _onJsCallback = onJsCallback;
    return ret;
}

void Win32WebControl::removeWebView()
{
    m_controller->Close();

    m_controller  = nullptr;
    m_webview     = nullptr;
}

void Win32WebControl::setWebViewRect(const int left, const int top, const int width, const int height)
{
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

    m_controller->put_ZoomFactor(_scalesPageToFit ? GetDeviceScaleFactor() : 1.0);
}

void Win32WebControl::setJavascriptInterfaceScheme(const std::string &scheme)
{
    m_jsScheme = scheme;
}

void Win32WebControl::loadHTMLString(const std::string &html, const std::string &baseURL)
{
    if (!html.empty())
    {
        const auto whtml = to_lpwstr(html);
        m_webview->NavigateToString(whtml);
        delete[] whtml;
    }
}

void Win32WebControl::loadURL(const std::string &url, bool cleanCachedData)
{
    if (cleanCachedData)
    {
        m_webview->CallDevToolsProtocolMethod(L"Network.clearBrowserCache", L"{}", nullptr);
    }
    navigate(url);
}

void Win32WebControl::loadFile(const std::string &filePath)
{
    auto fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);
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
    return m_webview->get_CanGoBack(&result) == 0 && result;
}

bool Win32WebControl::canGoForward() const
{
    BOOL result;
    return m_webview->get_CanGoForward(&result) == 0 && result;
}

void Win32WebControl::goBack() const
{
    m_webview->GoBack();
}

void Win32WebControl::goForward() const
{
    m_webview->GoForward();
}

void Win32WebControl::evaluateJS(const std::string &js)
{
    eval(js);
}

void Win32WebControl::setScalesPageToFit(const bool scalesPageToFit)
{
    _scalesPageToFit = scalesPageToFit;
    m_controller->put_ZoomFactor(_scalesPageToFit ? GetDeviceScaleFactor() : 1.0);
}

void Win32WebControl::setWebViewVisible(const bool visible) const
{
    ShowWindow(m_window, visible ? SW_SHOW : SW_HIDE);
    if (!visible)
    {
        // When the frame window is minimized set the browser window size to 0x0 to
        // reduce resource usage.
        SetWindowPos(m_window, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

void Win32WebControl::setBounces(bool bounces)
{
}

void Win32WebControl::setOpacityWebView(float opacity)
{
}

float Win32WebControl::getOpacityWebView() const
{
    return 1.0f;
}

void Win32WebControl::setBackgroundTransparent()
{
    ICoreWebView2Controller2* viewController2;
    if (SUCCEEDED(m_controller->QueryInterface(__uuidof(ICoreWebView2Controller2), reinterpret_cast<void**>(&viewController2))))
    {
        COREWEBVIEW2_COLOR color;
        viewController2->get_DefaultBackgroundColor(&color);
        color.A = 0; // Only supports 0 or 255. Other values not supported
        viewController2->put_DefaultBackgroundColor(color);
    }
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
