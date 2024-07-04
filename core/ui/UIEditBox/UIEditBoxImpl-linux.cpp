/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2015 hanxi
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "ui/UIEditBox/UIEditBoxImpl-linux.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

#    include "ui/UIEditBox/UIEditBox.h"
#    include "2d/Label.h"
#    include "base/UTF8.h"

#include <dlfcn.h>

extern "C" {

typedef char gchar;
typedef int gint;
typedef gint gboolean;

typedef unsigned long gulong;

typedef enum
{
    G_CONNECT_DEFAULT = 0,
    G_CONNECT_AFTER   = 1 << 0,
    G_CONNECT_SWAPPED = 1 << 1
} GConnectFlags;

typedef enum
{
    GDK_WINDOW_TYPE_HINT_NORMAL,
    GDK_WINDOW_TYPE_HINT_DIALOG,
    GDK_WINDOW_TYPE_HINT_MENU, /* Torn off menu */
    GDK_WINDOW_TYPE_HINT_TOOLBAR,
    GDK_WINDOW_TYPE_HINT_SPLASHSCREEN,
    GDK_WINDOW_TYPE_HINT_UTILITY,
    GDK_WINDOW_TYPE_HINT_DOCK,
    GDK_WINDOW_TYPE_HINT_DESKTOP,
    GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU, /* A drop down menu (from a menubar) */
    GDK_WINDOW_TYPE_HINT_POPUP_MENU,    /* A popup menu (from right-click) */
    GDK_WINDOW_TYPE_HINT_TOOLTIP,
    GDK_WINDOW_TYPE_HINT_NOTIFICATION,
    GDK_WINDOW_TYPE_HINT_COMBO,
    GDK_WINDOW_TYPE_HINT_DND
} GdkWindowTypeHint;

typedef enum
{
    GTK_WIN_POS_NONE,
    GTK_WIN_POS_CENTER,
    GTK_WIN_POS_MOUSE,
    GTK_WIN_POS_CENTER_ALWAYS,
    GTK_WIN_POS_CENTER_ON_PARENT
} GtkWindowPosition;

typedef struct _GtkAdjustment GtkAdjustment;
typedef struct _GtkBuilder GtkBuilder;
typedef struct _GtkClipboard GtkClipboard;
typedef struct _GtkIconSet GtkIconSet;
typedef struct _GtkIconSource GtkIconSource;
typedef struct _GtkRcStyle GtkRcStyle;
typedef struct _GtkRequisition GtkRequisition;
typedef struct _GtkSelectionData GtkSelectionData;
typedef struct _GtkSettings GtkSettings;
typedef struct _GtkStyle GtkStyle;
typedef struct _GtkStyleContext GtkStyleContext;
typedef struct _GtkTooltip GtkTooltip;
typedef struct _GtkWidget GtkWidget;
typedef struct _GtkWidgetPath GtkWidgetPath;
typedef struct _GtkWindow GtkWindow;

typedef struct _GtkDialog GtkDialog;
typedef struct _GtkDialogPrivate GtkDialogPrivate;
typedef struct _GtkDialogClass GtkDialogClass;

typedef struct _GTypeClass GTypeClass;
typedef struct _GTypeInstance GTypeInstance;

typedef void* gpointer;

typedef struct _GtkContainer GtkContainer;

typedef struct _GtkEntry GtkEntry;

typedef struct _GClosure GClosure;

typedef struct _GMainContext GMainContext;
}

#    if defined(__has_attribute) && __has_attribute(__const__)
#        define G_GNUC_CONST __attribute__((__const__))
#    else
#        define G_GNUC_CONST
#    endif

// dynamic load gtk-3 APIs
typedef unsigned long GType;

typedef void (*GCallback)(void);
typedef void (*GClosureNotify)(gpointer data, GClosure* closure);

#define	G_CALLBACK(f)			 ((GCallback) (f))

static GType (*gtk_dialog_get_type)(void) G_GNUC_CONST;
static GType (*gtk_container_get_type)(void) G_GNUC_CONST;
static GType (*gtk_entry_get_type)(void) G_GNUC_CONST;
static GType (*gtk_window_get_type)(void) G_GNUC_CONST;

static void (*gtk_widget_destroy)(GtkWidget* widget);
static void (*gtk_init)(int* argc, char*** argv);
static GtkWidget* (*gtk_dialog_new)(void);
static GtkWidget* (*gtk_entry_new)(void);
static GtkWidget* (*gtk_dialog_get_content_area)(GtkDialog* dialog);
static void (*gtk_container_add)(GtkContainer* container, GtkWidget* widget);
static GtkWidget* (*gtk_dialog_add_button)(GtkDialog* dialog, const gchar* button_text, gint response_id);
static void (*gtk_entry_set_text)(GtkEntry* entry, const gchar* text);
static void (*gtk_window_set_keep_above)(GtkWindow* window, gboolean setting);
static gulong (*g_signal_connect_data)(gpointer instance,
                                       const gchar* detailed_signal,
                                       GCallback c_handler,
                                       gpointer data,
                                       GClosureNotify destroy_data,
                                       GConnectFlags connect_flags);
static void (*gtk_window_set_type_hint)(GtkWindow* window, GdkWindowTypeHint hint);
static void (*gtk_window_set_position)(GtkWindow* window, GtkWindowPosition position);
static void (*gtk_widget_show_all)(GtkWidget* widget);
static gint (*gtk_dialog_run)(GtkDialog* dialog);
static const gchar* (*gtk_entry_get_text)(GtkEntry* entry);
static gboolean (*g_main_context_iteration)(GMainContext* context, gboolean may_block);
static GTypeInstance* (*g_type_check_instance_cast)(GTypeInstance* instance, GType iface_type);

/* --- implementation bits --- */
#    if defined(G_DISABLE_CAST_CHECKS) || defined(__OPTIMIZE__)
#        define _G_TYPE_CIC(ip, gt, ct) ((ct*)(void*)ip)
#        define _G_TYPE_CCC(cp, gt, ct) ((ct*)(void*)cp)
#    else
#        define _G_TYPE_CIC(ip, gt, ct) ((ct*)(void*)g_type_check_instance_cast((GTypeInstance*)ip, gt))
#        define _G_TYPE_CCC(cp, gt, ct) ((ct*)(void*)g_type_check_class_cast((GTypeClass*)cp, gt))
#    endif

#    define G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type) (_G_TYPE_CIC((instance), (g_type), c_type))
#    define G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type) (_G_TYPE_CIC((instance), (g_type), c_type))

#    define GTK_TYPE_DIALOG                                      (gtk_dialog_get_type())
#    define GTK_DIALOG(obj)                                      (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_DIALOG, GtkDialog))
#    define GTK_DIALOG_CLASS(klass)                              (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_DIALOG, GtkDialogClass))
#    define GTK_IS_DIALOG(obj)                                   (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_DIALOG))
#    define GTK_IS_DIALOG_CLASS(klass)                           (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_DIALOG))
#    define GTK_DIALOG_GET_CLASS(obj)                            (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_DIALOG, GtkDialogClass))

#    define GTK_TYPE_CONTAINER                                   (gtk_container_get_type())
#    define GTK_CONTAINER(obj)                                   (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_CONTAINER, GtkContainer))
#    define GTK_CONTAINER_CLASS(klass)                           (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_CONTAINER, GtkContainerClass))
#    define GTK_IS_CONTAINER(obj)                                (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_CONTAINER))
#    define GTK_IS_CONTAINER_CLASS(klass)                        (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_CONTAINER))
#    define GTK_CONTAINER_GET_CLASS(obj)                         (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_CONTAINER, GtkContainerClass))

#    define GTK_TYPE_ENTRY                                       (gtk_entry_get_type())
#    define GTK_ENTRY(obj)                                       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_ENTRY, GtkEntry))
#    define GTK_ENTRY_CLASS(klass)                               (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_ENTRY, GtkEntryClass))
#    define GTK_IS_ENTRY(obj)                                    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_ENTRY))
#    define GTK_IS_ENTRY_CLASS(klass)                            (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_ENTRY))
#    define GTK_ENTRY_GET_CLASS(obj)                             (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_ENTRY, GtkEntryClass))

#    define GTK_TYPE_WINDOW                                      (gtk_window_get_type())
#    define GTK_WINDOW(obj)                                      (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_WINDOW, GtkWindow))
#    define GTK_WINDOW_CLASS(klass)                              (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_WINDOW, GtkWindowClass))
#    define GTK_IS_WINDOW(obj)                                   (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_WINDOW))
#    define GTK_IS_WINDOW_CLASS(klass)                           (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_WINDOW))
#    define GTK_WINDOW_GET_CLASS(obj)                            (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_WINDOW, GtkWindowClass))

#    define g_signal_connect(instance, detailed_signal, c_handler, data) \
        g_signal_connect_data((instance), (detailed_signal), (c_handler), (data), NULL, (GConnectFlags)0)

// destroy dialog when lost focus
static void dialogFocusOutCallback(GtkWidget* widget, gpointer user_data)
{
    gtk_widget_destroy(widget);
}

#define GTK_DLSYM(func)  do { \
    func = reinterpret_cast<decltype(func)>(dlsym(gtk3, #func)); \
    if (!func) throw std::runtime_error(#func); \
} while(0)

static bool load_gtk3() {
    static bool s_loaded = false;
    static void* gtk3 = 0;
    
    if(!s_loaded) {
        s_loaded = true;

        gtk3 = dlopen("libgtk-3.so", RTLD_LAZY | RTLD_LOCAL);
        if(gtk3) {
            try {
                GTK_DLSYM(gtk_dialog_get_type);
                GTK_DLSYM(gtk_container_get_type);
                GTK_DLSYM(gtk_entry_get_type);
                GTK_DLSYM(gtk_window_get_type);
                GTK_DLSYM(gtk_widget_destroy);
                GTK_DLSYM(gtk_init);
                GTK_DLSYM(gtk_dialog_new);
                GTK_DLSYM(gtk_entry_new);
                GTK_DLSYM(gtk_dialog_get_content_area);
                GTK_DLSYM(gtk_container_add);
                GTK_DLSYM(gtk_dialog_add_button);
                GTK_DLSYM(gtk_entry_set_text);
                GTK_DLSYM(gtk_window_set_keep_above);
                GTK_DLSYM(g_signal_connect_data);
                GTK_DLSYM(gtk_window_set_type_hint);
                GTK_DLSYM(gtk_window_set_position);
                GTK_DLSYM(gtk_widget_show_all);
                GTK_DLSYM(gtk_dialog_run);
                GTK_DLSYM(gtk_entry_get_text);
                GTK_DLSYM(g_main_context_iteration);
                GTK_DLSYM(g_type_check_instance_cast);
            } catch(const std::exception& ex) {
                AXLOGE("load gtk function: {} fail", ex.what());
                dlclose(gtk3);
                gtk3 = nullptr;
            }
        }
    }

    return !!gtk3;
}

static bool LinuxInputBox(std::string& entryLine)
{
    if(!load_gtk3()) {
        throw std::runtime_error("missing library gtk3");
    }

    bool didChange = false;
    GtkWidget* dialog;
    GtkWidget* entry;
    GtkWidget* contentArea;

    gtk_init(0, NULL);
    dialog      = gtk_dialog_new();
    entry       = gtk_entry_new();
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    gtk_container_add(GTK_CONTAINER(contentArea), entry);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", 0);
    gtk_entry_set_text(GTK_ENTRY(entry), entryLine.c_str());

    g_signal_connect(dialog, "focus-out-event", G_CALLBACK(dialogFocusOutCallback), NULL);
    gtk_window_set_keep_above(GTK_WINDOW(dialog), true);
    gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_MENU);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(dialog);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    switch (result)
    {
    case 0:
        entryLine = gtk_entry_get_text(GTK_ENTRY(entry));
        didChange = true;
        break;
    default:
        // AXLOGD("Undefined. Perhaps dialog was closed");
        break;
    }

    gtk_widget_destroy(dialog);
    while (g_main_context_iteration(NULL, false))
        ;
    return didChange;
}

NS_AX_BEGIN

namespace ui
{

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return new EditBoxImplLinux(pEditBox);
}

EditBoxImplLinux::EditBoxImplLinux(EditBox* pEditText) : EditBoxImplCommon(pEditText) {}

EditBoxImplLinux::~EditBoxImplLinux() {}

bool EditBoxImplLinux::isEditing()
{
    return false;
}

void EditBoxImplLinux::nativeOpenKeyboard()
{
    std::string text = this->getText();
    bool didChange   = LinuxInputBox(text);
    if (didChange)
    {
        this->editBoxEditingDidEnd(text);
    }
}

}  // namespace ui

NS_AX_END

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX) */
