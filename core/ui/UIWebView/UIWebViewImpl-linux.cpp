/****************************************************************************
 Copyright (c) 2017 Serge Zaitsev
 Copyright (c) 2022 Steffen André Langnes
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#    include <cstdlib>
#    include <gtk/gtk.h>
#    include <webkit2/webkit2.h>
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <gdk/gdkx.h>

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
// See: https://docs.gtk.org/gdk3/class.DisplayManager.html
static inline bool is_gdk_x11_backend()
{
#    ifdef GDK_WINDOWING_X11
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

class GTKWebKit {
public:
    GTKWebKit* create()
    {
        if (webkit_dmabuf::is_wayland_display()) {
            // init wayland gdk..
        }
        else {
            // init x11 gdk...
        }
        m_WebView = webkit_web_view_new();
        WebKitUserContentManager *manager = m_user_content_manager =
            webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_WebView));

        // init script

        gtk_container_add(GTK_CONTAINER(m_Window), GTK_WIDGET(m_WebView));
        gtk_widget_show(GTK_WIDGET(m_WebView));

        WebKitSettings *settings =
            webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_webview));
        webkit_settings_set_javascript_can_access_clipboard(settings
    }

private:
    GtkWidget* m_Window;
    GtkWidget* m_WebView;
};

NS_AX_BEGIN

namespace ui
{

WebViewImpl::WebViewImpl(WebView* webView) : _createSucceeded(false), _systemWebControl(nullptr), _webView(webView)
{
    webkit_dmabuf::apply_webkit_dmabuf_workaround();

}

}  // namespace ui

NS_AX_END

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
