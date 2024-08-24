/****************************************************************************
 Copyright (c) 2017 Serge Zaitsev
 Copyright (c) 2022 Steffen André Langnes
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

#include "UIWebViewImpl-linux.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

#    include <sys/stat.h>
#    include <chrono>  // chrono_literals
#    include <thread>  // this_thread::sleep_for
#    include <gtk/gtk.h>
#    include <webkit2/webkit2.h>

#    include "UIWebView.h"
#    include "base/Director.h"
#    include "platform/FileUtils.h"
#    include "platform/GLView.h"
#    include "ui/UIHelper.h"
#    include "UIWebViewCommon.h"

#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
#        include <X11/Xlib.h>
#        include <X11/Xatom.h>
#        include <gdk/gdkx.h>
#    endif

using namespace webview_common;

namespace webkit_dmabuf
{

// Get environment variable. Not thread-safe.
static inline std::string get_env(const std::string& name)
{
    auto* value = std::getenv(name.c_str());
    if (value)
    {
        return {value};
    }
    return {};
}

// Set environment variable. Not thread-safe.
static inline void set_env(const std::string& name, const std::string& value)
{
    ::setenv(name.c_str(), value.c_str(), 1);
}

// Checks whether the NVIDIA GPU driver is used based on whether the kernel
// module is loaded.
static inline bool is_using_nvidia_driver()
{
    struct ::stat buffer
    {};
    if (::stat("/sys/module/nvidia", &buffer) != 0)
    {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

// Checks whether the windowing system is Wayland.
static inline bool is_wayland_display()
{
    if (!get_env("WAYLAND_DISPLAY").empty())
    {
        return true;
    }
    if (get_env("XDG_SESSION_TYPE") == "wayland")
    {
        return true;
    }
    if (get_env("DESKTOP_SESSION").find("wayland") != std::string::npos)
    {
        return true;
    }
    return false;
}

// Checks whether the GDK X11 backend is used.
// See: https://docs.gdkWindow.org/gdk3/class.DisplayManager.html
static inline bool is_gdk_x11_backend()
{
#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
    auto* manager = gdk_display_manager_get();
    auto* display = gdk_display_manager_get_default_display(manager);
    return GDK_IS_X11_DISPLAY(display);
#    else
    return false;
#    endif
}

// Checks whether WebKit is affected by bug when using DMA-BUF renderer.
// Returns true if all of the following conditions are met:
//  - WebKit version is >= 2.42 (please narrow this down when there's a fix).
//  - Environment variables are empty or not set:
//    - WEBKIT_DISABLE_DMABUF_RENDERER
//  - Windowing system is not Wayland.
//  - GDK backend is X11.
//  - NVIDIA GPU driver is used.
static inline bool is_webkit_dmabuf_bugged()
{
    auto wk_major = webkit_get_major_version();
    auto wk_minor = webkit_get_minor_version();
    // TODO: Narrow down affected WebKit version when there's a fixed version
    auto is_affected_wk_version = wk_major == 2 && wk_minor >= 42;
    if (!is_affected_wk_version)
    {
        return false;
    }
    if (!get_env("WEBKIT_DISABLE_DMABUF_RENDERER").empty())
    {
        return false;
    }
    if (is_wayland_display())
    {
        return false;
    }
    if (!is_gdk_x11_backend())
    {
        return false;
    }
    if (!is_using_nvidia_driver())
    {
        return false;
    }
    return true;
}

// Applies workaround for WebKit DMA-BUF bug if needed.
// See WebKit bug: https://bugs.webkit.org/show_bug.cgi?id=261874
static inline void apply_webkit_dmabuf_workaround()
{
    if (!is_webkit_dmabuf_bugged())
    {
        return;
    }
    set_env("WEBKIT_DISABLE_DMABUF_RENDERER", "1");
}
}  // namespace webkit_dmabuf

static double getDeviceScaleFactor(GdkMonitor* monitor)
{
    static double scale_factor      = 1.0;
    static GdkMonitor* last_monitor = nullptr;

    if (last_monitor != monitor)
    {
        last_monitor = monitor;
        return (double)gdk_monitor_get_scale_factor(last_monitor);
    }

    return scale_factor;
}

static inline std::string create_init_script(const std::string& post_fn)
{
    auto js = std::string{} +
              "(function() {\n\
  'use strict';\n\
  function generateId() {\n\
    var crypto = window.crypto || window.msCrypto;\n\
    var bytes = new Uint8Array(16);\n\
    crypto.getRandomValues(bytes);\n\
    return Array.prototype.slice.call(bytes).map(function(n) {\n\
      return n.toString(16).padStart(2, '0');\n\
    }).join('');\n\
  }\n\
  var Webview = (function() {\n\
    var _promises = {};\n\
    function Webview_() {}\n\
    Webview_.prototype.post = function(message) {\n\
      return (" +
              post_fn +
              ")(message);\n\
    };\n\
    Webview_.prototype.call = function(method) {\n\
      var _id = generateId();\n\
      var _params = Array.prototype.slice.call(arguments, 1);\n\
      var promise = new Promise(function(resolve, reject) {\n\
        _promises[_id] = { resolve, reject };\n\
      });\n\
      this.post(JSON.stringify({\n\
        id: _id,\n\
        method: method,\n\
        params: _params\n\
      }));\n\
      return promise;\n\
    };\n\
    Webview_.prototype.onReply = function(id, status, result) {\n\
      var promise = _promises[id];\n\
      if (result !== undefined) {\n\
        try {\n\
          result = JSON.parse(result);\n\
        } catch {\n\
          promise.reject(new Error(\"Failed to parse binding result as JSON\"));\n\
          return;\n\
        }\n\
      }\n\
      if (status === 0) {\n\
        promise.resolve(result);\n\
      } else {\n\
        promise.reject(result);\n\
      }\n\
    };\n\
    Webview_.prototype.onBind = function(name) {\n\
      if (Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" already exists');\n\
      }\n\
      window[name] = (function() {\n\
        var params = [name].concat(Array.prototype.slice.call(arguments));\n\
        return Webview_.prototype.call.apply(this, params);\n\
      }).bind(this);\n\
    };\n\
    Webview_.prototype.onUnbind = function(name) {\n\
      if (!Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" does not exist');\n\
      }\n\
      delete window[name];\n\
    };\n\
    return Webview_;\n\
  })();\n\
  window.__webview__ = new Webview();\n\
})()";
    return js;
}

static void init_gtk_platform_with_display(Display* x11Display)
{
    static bool has_initiated = false;
    if (has_initiated)
    {
        return;
    }
    webkit_dmabuf::apply_webkit_dmabuf_workaround();

#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
    // this will instruct gdk to use XWayland layer on Wayland platforms
    if (webkit_dmabuf::is_wayland_display())
    {
        gdk_set_allowed_backends("x11");
    }
#    endif
    gtk_init(nullptr, nullptr);

    // instructing gdk display manager to use specified x11 display as default
    const char* display_string = DisplayString(x11Display);
    GdkDisplay* x11_gdk_display =
        display_string != nullptr ? gdk_display_open(display_string) : gdk_display_get_default();

    auto default_display_manager = gdk_display_manager_get();
    gdk_display_manager_set_default_display(default_display_manager, x11_gdk_display);

    has_initiated = true;
}

using ContinueUrlCallback_t = std::function<bool(std::string_view)>;
using GeneralUrlCallback_t  = std::function<void(std::string_view)>;
class GTKWebKit
{
public:
    GTKWebKit() : isXwayland(webkit_dmabuf::is_wayland_display())
    {
        m_X11Display      = static_cast<Display*>(ax::Director::getInstance()->getGLView()->getX11Display());
        m_ParentX11Window = reinterpret_cast<Window>(ax::Director::getInstance()->getGLView()->getX11Window());

        init_gtk_platform_with_display(m_X11Display);

        m_Window = gtk_window_new(GTK_WINDOW_POPUP);
        gtk_window_set_resizable(GTK_WINDOW(m_Window), true);
        g_signal_connect(G_OBJECT(m_Window), "destroy", G_CALLBACK(+[](GtkWidget*, gpointer arg) {
                             auto* w = static_cast<GTKWebKit*>(arg);
                             // Widget destroyed along with window.
                             w->m_WebView = nullptr;
                             w->m_Window  = nullptr;
                         }),
                         this);

#    if defined(AX_PLATFORM_LINUX_WAYLAND)
        AX_ASSERT(0 && "TODO: Implement for wayland..");
/*
 * Notes:
 *
 * Current Wayland(2024) there is no way to embed a foreign window.
 * Usually we need to create a native window turn it into a standalone
 * browser, place inside of the main window region and interact with it.
 *
 * - Why not just create a window with no title bar and maintain it's
 * position and size manually according to the main window? AFAIK
 * Wayland also doesn't support it, it doesn't even allow the process
 * which created the window to change those properties.
 *
 * - Why not just use XWayland layer do those and never use direct wayland
 * through GLFW or anything?
 * Picture quality and overall performance will not be as good as native
 * wayland usage.
 * Need do more research but it's just makes sense we won't be getting
 * native Wayland performance that's for sure. But the OpenGL drawing
 * part shouldn't be affected.
 *
 * Possible workarounds:
 * 1. Ditch GLFW for Linux platform, use EGL library directly or
 * use GTK+GDK to maintain the all the window handle stuff. Then use
 * Wayland subcompositor concept or https://wayland-book.com/xdg-shell-in-depth/popups.html
 * to draw the WebkitGtk surface. Some usage of subcompositor concept can be found:
 * https://github.com/ehmry/snes9x/
 *
 * 1.5: Or we may be can use some other GLFW like framework for Linux, which has
 * custom wayland compositor implemented and allows foreign parenting/reparenting.
 * Something like this: https://github.com/fltk/fltk, Not sure if they allow it.
 *
 * - Why this approach is kind of better? Well will be using direct Wayland
 * so some hdpi scaling(if not by fraction) would be kind of better than
 * XWayland and some other stuff probably won't be much noticable, but
 * hey we are removing one more non benificial abstraction layer(XWayland)
 * from the userend.
 *
 * 2. Write custom Wayland compositor(AFAIK it's not a WM but almost
 * acts like one) to allow draw foreign wl_surface...
 * Yes.. No.
 *
 * Bonus: We can go with the direct Wayland window managing + draw the
 * WebView using XWayland abstraction layer and place the window position
 * according to the main window's location. We're not doing exactly the same
 * for x11 case, X11 has something to make a window, a child window of another
 * window so the relative positon gets handled automatically by the WM.
 *
 * Why not use CEF or some other WebView implementations? It would be same
 * we would still need to move those around, which Wayland doesn't allow
 * from process context to do it without custom compositor.
 *
 */
#    endif
    }

    ~GTKWebKit()
    {
        if (m_WebView)
        {
            gtk_widget_destroy(GTK_WIDGET(m_WebView));
            m_WebView = nullptr;
        }

        if (m_Window)
        {
            g_signal_handlers_disconnect_by_data(GTK_WINDOW(m_Window), this);
            gtk_window_close(GTK_WINDOW(m_Window));
            m_Window = nullptr;
        }

        if (m_InitScript)
        {
            webkit_user_script_unref(m_InitScript);
            m_InitScript = nullptr;
        }

        depleteRunLoopEventQueue();
    }

    bool createWebView(const ContinueUrlCallback_t& shouldStartLoading,
                       const GeneralUrlCallback_t& didFinishLoading,
                       const GeneralUrlCallback_t didFailLoading,
                       const GeneralUrlCallback_t& onJsCallback)
    {
        if (m_Window == nullptr)
            return false;

        m_WebView = webkit_web_view_new();
        if (m_WebView == nullptr)
            return false;

        WebKitUserContentManager* manager = m_UserContentManager =
            webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_WebView));
        if (manager == nullptr)
            return false;

        g_signal_connect(manager, "script-message-received::__webview__",
                         G_CALLBACK(+[](WebKitUserContentManager*, WebKitJavascriptResult* r, gpointer arg) {
                             auto* w = static_cast<GTKWebKit*>(arg);
                             // webkit2 2.22+
                             JSCValue* value = webkit_javascript_result_get_js_value(r);
                             char* s         = jsc_value_to_string(value);
                             w->onMessage(s);
                             g_free(s);
                         }),
                         this);
        webkit_user_content_manager_register_script_message_handler(manager, "__webview__");
        initScript("function(message){return window.webkit.messageHandlers.__webview__.postMessage(message);}");

        m_ShouldStartLoading = shouldStartLoading;
        m_DidFinishLoading   = didFinishLoading;
        m_DidFailLoading     = didFailLoading;
        m_OnJsCallback       = onJsCallback;
        g_signal_connect(m_WebView, "load-changed",
                         G_CALLBACK(+[](WebKitWebView* webView, WebKitLoadEvent loadEvent, gpointer userData) {
                             auto thiz = static_cast<GTKWebKit*>(userData);
                             switch (loadEvent)
                             {
                             case WEBKIT_LOAD_COMMITTED:
                             {
                                 std::string_view uri = webkit_web_view_get_uri(webView);
                                 const auto scheme    = uri.substr(0, uri.find_first_of(':'));

                                 if (scheme == thiz->m_JsScheme)
                                 {
                                     thiz->m_OnJsCallback(uri);
                                     return;
                                 }

                                 if (!thiz->m_ShouldStartLoading(uri))
                                     webkit_web_view_stop_loading(webView);
                                 break;
                             }
                             case WEBKIT_LOAD_FINISHED:
                             {
                                 std::string_view uri = webkit_web_view_get_uri(webView);
                                 thiz->m_DidFinishLoading(uri);
                                 break;
                             }
                             }
                         }),
                         this);

        g_signal_connect(m_WebView, "load-failed",
                         G_CALLBACK(+[](WebKitWebView* webView, WebKitLoadEvent /* loadEvent */, gchar* failingUri,
                                        gpointer /* error */, gpointer userData) {
                             auto thiz = static_cast<GTKWebKit*>(userData);
                             thiz->m_DidFailLoading(failingUri);
                         }),
                         this);
        gtk_container_add(GTK_CONTAINER(m_Window), GTK_WIDGET(m_WebView));
        gtk_widget_show(GTK_WIDGET(m_WebView));

        WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_WebView));
        if (settings == nullptr)
            return false;

        webkit_settings_set_javascript_can_access_clipboard(settings, true);

        gtk_widget_grab_focus(GTK_WIDGET(m_WebView));
        gtk_widget_show_all(m_Window);

        return embed();
    }

    void update()
    {
        auto director = ax::Director::getInstance();
        if (!director->isValid() || m_WebViewHidden)
            return;

        // Xwayland child repaint issue...
        // Another reason to support direct wayland.
        // Related issues:
        // https://bugzilla.redhat.com/show_bug.cgi?id=1896119
        // https://gitlab.gnome.org/GNOME/mutter/-/issues/1577
        if (isXwayland)
        {
            XReparentWindow(m_X11Display, m_ChildX11Window, m_ParentX11Window, (int)m_LastPos.x, (int)m_LastPos.y);
        }

        // Can be removed if axmol ever integrates with gtk/gdk for linux
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
    }

    void setWebViewRect(int x, int y, int width, int height)
    {
        AXLOGI("Pos: {}, {}, Size: w:{}, h:{}", x, y, width, height);

        gtk_window_resize(GTK_WINDOW(m_Window), width, height);
        gtk_window_move(GTK_WINDOW(m_Window), x, y);

        setScalesPageToFit(m_ScalesPageToFit);

        m_LastPos  = ax::Vec2((float)x, (float)y);
        m_LastSize = ax::Vec2((float)width, (float)height);
    }

    void loadHTMLString(std::string_view html, std::string_view baseURL)
    {
        webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_WebView), html.data(), baseURL.data());
    }

    void loadFile(std::string_view filePath)
    {
        auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
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

    void loadURL(std::string_view url, bool cleanCachedData)
    {
        if (cleanCachedData)
        {
            webkit_web_context_clear_cache(webkit_web_view_get_context(WEBKIT_WEB_VIEW(m_WebView)));
        }
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_WebView), url.data());
    }

    void stopLoading() { webkit_web_view_stop_loading(WEBKIT_WEB_VIEW(m_WebView)); }

    void reload()
    {
        // Note: there is also `webkit_web_view_reload` which uses cached data
        webkit_web_view_reload_bypass_cache(WEBKIT_WEB_VIEW(m_WebView));
    }

    bool canGoBack() { return webkit_web_view_can_go_back(WEBKIT_WEB_VIEW(m_WebView)); }

    bool canGoForward() { return webkit_web_view_can_go_forward(WEBKIT_WEB_VIEW(m_WebView)); }

    void goBack() { webkit_web_view_go_back(WEBKIT_WEB_VIEW(m_WebView)); }

    void goForward() { webkit_web_view_go_forward(WEBKIT_WEB_VIEW(m_WebView)); }

    void setJavascriptInterfaceScheme(std::string_view scheme) { m_JsScheme = scheme; }

    void evaluateJS(std::string_view js)
    {
        if (webkit_web_view_get_uri(WEBKIT_WEB_VIEW(m_WebView)) == nullptr)
            return;

        // webkit2 2.40+
        webkit_web_view_evaluate_javascript(WEBKIT_WEB_VIEW(m_WebView), js.data(), static_cast<gssize>(js.size()),
                                            nullptr, nullptr, nullptr, nullptr, nullptr);
    }

    void setScalesPageToFit(bool scalesPageToFit)
    {
        m_ScalesPageToFit = scalesPageToFit;

        // get current monitor
        auto* manager = gdk_display_manager_get();
        auto* display = gdk_display_manager_get_default_display(manager);
        m_Monitor     = gdk_display_get_monitor_at_window(display, gtk_widget_get_window(m_Window));

        webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(m_WebView),
                                       m_ScalesPageToFit ? getDeviceScaleFactor(m_Monitor) : 1.0);
    }

    void setWebViewVisible(bool visible)
    {
        using namespace std::chrono_literals;
        if (visible && m_WebViewHidden)
        {
            XMapWindow(m_X11Display, m_ChildX11Window);
            XReparentWindow(m_X11Display, m_ChildX11Window, m_ParentX11Window, (int)m_LastPos.x, (int)m_LastPos.y);

            std::this_thread::sleep_for(1000ms);

            XSync(m_X11Display, false);
            XFlush(m_X11Display);
        }
        else if (!m_WebViewHidden && !visible)
        {
            XUnmapWindow(m_X11Display, m_ChildX11Window);

            std::this_thread::sleep_for(1000ms);

            XSync(m_X11Display, false);
            XFlush(m_X11Display);
        }

        m_WebViewHidden = !visible;
    }

    void setBounces(bool) {}

    void setOpacityWebView(double opacity)
    {
        gtk_widget_set_opacity(GTK_WIDGET(m_WebView), opacity);
        gtk_widget_set_opacity(GTK_WIDGET(m_Window), opacity);
    }

    double getOpacityWebView() { return gtk_widget_get_opacity(GTK_WIDGET(m_WebView)); }

    void setBackgroundTransparent() {}

private:
    void initScript(const std::string& js)
    {
        m_InitScript = webkit_user_script_new(js.c_str(), WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                                              WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, nullptr, nullptr);

        webkit_user_content_manager_add_script(m_UserContentManager, m_InitScript);
    }

    bool embed()
    {
        gtk_widget_realize(m_Window);

        m_ChildX11Window = gdk_x11_window_get_xid(gtk_widget_get_window(m_Window));
        if (m_ChildX11Window == None)
            return false;

        XReparentWindow(m_X11Display, m_ChildX11Window, m_ParentX11Window, 0, 0);

        XMapWindow(m_X11Display, m_ChildX11Window);
        XFlush(m_X11Display);
        XSync(m_X11Display, false);

        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }

        return true;
    }

    using dispatch_fn_t = std::function<void()>;
    void dispatch_in_gtk(std::function<void()> f)
    {
        g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)([](void* f) -> int {
                            (*static_cast<dispatch_fn_t*>(f))();
                            return G_SOURCE_REMOVE;
                        }),
                        new std::function<void()>(f), [](void* f) { delete static_cast<dispatch_fn_t*>(f); });
    }

    void onMessage(std::string_view message)
    {
        // if axmol UIWebView ever support bindings for different callbacks from
        // javascript land...
    }

    void depleteRunLoopEventQueue()
    {
        bool done = false;
        dispatch_in_gtk([&] { done = true; });
        while (!done)
        {
            gtk_main_iteration();
        }
    }

private:
    const bool isXwayland;

    Display* m_X11Display;
    Window m_ParentX11Window;
    Window m_ChildX11Window;
    GtkWidget* m_Window;
    GtkWidget* m_WebView;
    GdkMonitor* m_Monitor;
    WebKitUserScript* m_InitScript;
    WebKitUserContentManager* m_UserContentManager;

    std::string m_JsScheme;
    bool m_ScalesPageToFit;
    bool m_WebViewHidden;

    ContinueUrlCallback_t m_ShouldStartLoading;
    GeneralUrlCallback_t m_DidFinishLoading;
    GeneralUrlCallback_t m_DidFailLoading;
    GeneralUrlCallback_t m_OnJsCallback;

    ax::Vec2 m_LastPos;
    ax::Vec2 m_LastSize;
};

namespace ax
{

namespace ui
{

WebViewImpl::WebViewImpl(WebView* webView) : _createSucceeded(false), _gtkWebKit(nullptr), _webView(webView)
{
    _gtkWebKit       = new GTKWebKit();
    _createSucceeded = _gtkWebKit->createWebView(
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
    if (_gtkWebKit != nullptr)
    {
        delete _gtkWebKit;
        _gtkWebKit = nullptr;
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
        _gtkWebKit->loadURL(url, false);
    }
}

void WebViewImpl::loadHTMLString(std::string_view string, std::string_view baseURL)
{
    if (_createSucceeded)
    {
        if (string.empty())
        {
            _gtkWebKit->loadHTMLString("data:text/html," + utils::urlEncode("<html></html>"), baseURL);
            return;
        }

        const auto html = htmlFromUri(string);
        if (!html.empty())
        {
            _gtkWebKit->loadHTMLString("data:text/html," + utils::urlEncode(html), baseURL);
        }
        else
        {
            _gtkWebKit->loadHTMLString(string, baseURL);
        }
    }
}

void WebViewImpl::loadURL(std::string_view url, bool cleanCachedData)
{
    if (_createSucceeded)
    {
        _gtkWebKit->loadURL(url, cleanCachedData);
    }
}

void WebViewImpl::loadFile(std::string_view fileName)
{
    if (_createSucceeded)
    {
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
        _gtkWebKit->loadFile(fullPath);
    }
}

void WebViewImpl::stopLoading()
{
    if (_createSucceeded)
    {
        _gtkWebKit->stopLoading();
    }
}

void WebViewImpl::reload()
{
    if (_createSucceeded)
    {
        _gtkWebKit->reload();
    }
}

bool WebViewImpl::canGoBack()
{
    if (_createSucceeded)
    {
        return _gtkWebKit->canGoBack();
    }
    return false;
}

bool WebViewImpl::canGoForward()
{
    if (_createSucceeded)
    {
        return _gtkWebKit->canGoForward();
    }
    return false;
}

void WebViewImpl::goBack()
{
    if (_createSucceeded)
    {
        _gtkWebKit->goBack();
    }
}

void WebViewImpl::goForward()
{
    if (_createSucceeded)
    {
        _gtkWebKit->goForward();
    }
}

void WebViewImpl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    if (_createSucceeded)
    {
        _gtkWebKit->setJavascriptInterfaceScheme(scheme);
    }
}

void WebViewImpl::evaluateJS(std::string_view js)
{
    if (_createSucceeded)
    {
        _gtkWebKit->evaluateJS(js);
    }
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit)
{
    if (_createSucceeded)
    {
        _gtkWebKit->setScalesPageToFit(scalesPageToFit);
    }
}

void WebViewImpl::draw(Renderer* renderer, Mat4 const& transform, uint32_t flags)
{
    if (!_createSucceeded)
    {
        return;
    }
    if (flags & Node::FLAGS_TRANSFORM_DIRTY)
    {
        const auto uiRect = ax::ui::Helper::convertBoundingBoxToScreen(_webView);
        _gtkWebKit->setWebViewRect(static_cast<int>(uiRect.origin.x), static_cast<int>(uiRect.origin.y),
                                   static_cast<int>(uiRect.size.width), static_cast<int>(uiRect.size.height));
    }

    _gtkWebKit->update();
}

void WebViewImpl::setVisible(bool visible)
{
    if (_createSucceeded)
    {
        _gtkWebKit->setWebViewVisible(visible);
    }
}

void WebViewImpl::setBounces(bool bounces)
{
    _gtkWebKit->setBounces(bounces);
}

void WebViewImpl::setOpacityWebView(float opacity)
{
    _gtkWebKit->setOpacityWebView((double)opacity);
}

float WebViewImpl::getOpacityWebView() const
{
    return (float)_gtkWebKit->getOpacityWebView();
}

void WebViewImpl::setBackgroundTransparent()
{
    _gtkWebKit->setBackgroundTransparent();
}

}  // namespace ui

}

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
