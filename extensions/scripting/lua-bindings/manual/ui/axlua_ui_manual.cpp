/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#include "lua-bindings/manual/ui/axlua_ui_manual.hpp"
#include "lua-bindings/auto/axlua_ui_auto.hpp"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
#    include "lua-bindings/auto/axlua_video_auto.hpp"
#    include "lua-bindings/manual/ui/axlua_video_manual.hpp"
#    include "lua-bindings/auto/axlua_webview_auto.hpp"
#    include "lua-bindings/manual/ui/axlua_webview_manual.hpp"
#elif defined(_WIN32)
#    if defined(AX_ENABLE_MFMEDIA)
#        include "lua-bindings/auto/axlua_video_auto.hpp"
#        include "lua-bindings/manual/ui/axlua_video_manual.hpp"
#    endif
#    if defined(AX_ENABLE_MSEDGE_WEBVIEW2)
#        include "lua-bindings/auto/axlua_webview_auto.hpp"
#        include "lua-bindings/manual/ui/axlua_webview_manual.hpp"
#    endif
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    include "lua-bindings/auto/axlua_webview_auto.hpp"
#    include "lua-bindings/manual/ui/axlua_webview_manual.hpp"
#    if defined(AX_ENABLE_VLC_MEDIA)
#        include "lua-bindings/auto/axlua_video_auto.hpp"
#        include "lua-bindings/manual/ui/axlua_video_manual.hpp"
#    endif
#endif

#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"
#include "lua-bindings/manual/LuaValue.h"
#include "ui/CocosGUI.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "base/EventListenerFocus.h"

using namespace ui;

static int handleUIEvent(int handler, ax::Object* sender, int eventType)
{
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();

    stack->pushObject(sender, "ax.Object");
    stack->pushInt(eventType);

    stack->executeFunctionByHandler(handler, 2);
    stack->clean();

    return 0;
}

static int axlua_Widget_addTouchEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc     = 0;
    Widget* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.Widget", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<Widget*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Widget_addTouchEventListener'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addTouchEventListener(
            [=](ax::Object* ref, Widget::TouchEventType eventType) { handleUIEvent(handler, ref, (int)eventType); });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addTouchEventListener' function of Widget has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addTouchEventListener'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Widget_addClickEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc     = 0;
    Widget* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.Widget", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<Widget*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Widget_addClickEventListener'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addClickEventListener([=](ax::Object* sender) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(sender, "ax.Object");
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addClickEventListener' function of Widget has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addClickEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendWidget(lua_State* L)
{
    lua_pushstring(L, "axui.Widget");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addTouchEventListener", axlua_Widget_addTouchEventListener);
        tolua_function(L, "addClickEventListener", axlua_Widget_addClickEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_CheckBox_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc       = 0;
    CheckBox* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.CheckBox", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<CheckBox*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_CheckBox_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener(
            [=](ax::Object* ref, CheckBox::EventType eventType) { handleUIEvent(handler, ref, (int)eventType); });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of CheckBox has wrong number of arguments: %d, was expecting %d\n", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendCheckBox(lua_State* L)
{
    lua_pushstring(L, "axui.CheckBox");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_CheckBox_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_RadioButton_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc          = 0;
    RadioButton* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.RadioButton", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<RadioButton*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_RadioButton_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener([=](RadioButton* radioButton, RadioButton::EventType eventType) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(radioButton, "axui.RadioButton");
            stack->pushInt((int)eventType);

            stack->executeFunctionByHandler(handler, 2);
            stack->clean();
        });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of CheckBox has wrong number of arguments: %d, was expecting %d\n", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendRadioButton(lua_State* L)
{
    lua_pushstring(L, "axui.RadioButton");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_RadioButton_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_RadioButtonGroup_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc               = 0;
    RadioButtonGroup* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.RadioButtonGroup", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<RadioButtonGroup*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_RadioButtonGroup_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener([=](RadioButton* radioButton, int index, RadioButtonGroup::EventType eventType) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();

            stack->pushObject(radioButton, "axui.RadioButton");
            stack->pushInt(index);
            stack->pushInt((int)eventType);

            stack->executeFunctionByHandler(handler, 3);
            stack->clean();
        });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L,
               "'addEventListener' function of RadioButtonGroup has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendRadioButtonGroup(lua_State* L)
{
    lua_pushstring(L, "axui.RadioButtonGroup");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_RadioButtonGroup_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_Slider_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc     = 0;
    Slider* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.Slider", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<Slider*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Slider_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener(
            [=](ax::Object* ref, Slider::EventType eventType) { handleUIEvent(handler, ref, (int)eventType); });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of Slider has wrong number of arguments: %d, was expecting %d\n", argc,
               1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendSlider(lua_State* L)
{
    lua_pushstring(L, "axui.Slider");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_Slider_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_TextField_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc        = 0;
    TextField* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.TextField", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<TextField*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_TextField_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener(
            [=](ax::Object* ref, TextField::EventType eventType) { handleUIEvent(handler, ref, (int)eventType); });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of TextField has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendTextField(lua_State* L)
{
    lua_pushstring(L, "axui.TextField");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_TextField_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_PageView_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc       = 0;
    PageView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.PageView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<PageView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_PageView_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        auto pageViewHandler = [=](ax::Object* ref, PageView::EventType eventType) {
            handleUIEvent(handler, ref, (int)eventType);
        };
        self->addEventListener((PageView::ccPageViewCallback)pageViewHandler);

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of PageView has wrong number of arguments: %d, was expecting %d\n", argc,
               1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendPageView(lua_State* L)
{
    lua_pushstring(L, "axui.PageView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_PageView_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_ScrollView_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc         = 0;
    ScrollView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.ScrollView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ScrollView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ScrollView_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        self->addEventListener(
            [=](ax::Object* ref, ScrollView::EventType eventType) { handleUIEvent(handler, ref, (int)eventType); });

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of ScrollView has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendScrollView(lua_State* L)
{
    lua_pushstring(L, "axui.ScrollView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_ScrollView_addEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_ListView_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc       = 0;
    ListView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.ListView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ListView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ListView_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        auto listViewCallback = [=](ax::Object* ref, ListView::EventType eventType) {
            handleUIEvent(handler, ref, (int)eventType);
        };
        self->addEventListener((ui::ListView::ccListViewCallback)listViewCallback);

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addEventListener' function of ListView has wrong number of arguments: %d, was expecting %d\n", argc,
               1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static int axlua_ListView_addScrollViewEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc       = 0;
    ListView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.ListView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ListView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_ListView_addScrollViewEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));

        auto scrollViewCallback = [=](ax::Object* ref, ui::ScrollView::EventType eventType) {
            handleUIEvent(handler, ref, (int)eventType);
        };
        self->addEventListener((ui::ScrollView::ccScrollViewCallback)scrollViewCallback);

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(
        L, "'addScrollViewEventListener' function of ListView has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addScrollViewEventListener'.", &tolua_err);
    return 0;
#endif
}

static void extendListView(lua_State* L)
{
    lua_pushstring(L, "axui.ListView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", axlua_ListView_addEventListener);
        tolua_function(L, "addScrollViewEventListener", axlua_ListView_addScrollViewEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_LayoutParameter_setMargin(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc              = 0;
    LayoutParameter* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.LayoutParameter", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<LayoutParameter*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_LayoutParameter_setMargin'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        Margin margin;
        lua_pushstring(L, "left");
        lua_gettable(L, 2);
        margin.left = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "top");
        lua_gettable(L, 2);
        margin.top = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "right");
        lua_gettable(L, 2);
        margin.right = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "bottom");
        lua_gettable(L, 2);
        margin.bottom = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        self->setMargin(margin);
        return 0;
    }

    luaL_error(L, "'setMargin' function of LayoutParameter has wrong number of arguments: %d, was expecting %d\n", argc,
               1);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'setMargin'.", &tolua_err);
    return 0;
#endif
}

static int axlua_LayoutParameter_getMargin(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc              = 0;
    LayoutParameter* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.LayoutParameter", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<LayoutParameter*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_LayoutParameter_getMargin'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        Margin margin = self->getMargin();

        lua_newtable(L);

        lua_pushstring(L, "left");
        lua_pushnumber(L, (lua_Number)margin.left);
        lua_rawset(L, -3);

        lua_pushstring(L, "top");
        lua_pushnumber(L, (lua_Number)margin.top);
        lua_rawset(L, -3);

        lua_pushstring(L, "right");
        lua_pushnumber(L, (lua_Number)margin.right);
        lua_rawset(L, -3);

        lua_pushstring(L, "bottom");
        lua_pushnumber(L, (lua_Number)margin.bottom);
        lua_rawset(L, -3);

        return 1;
    }

    luaL_error(L, "'getMargin' function of LayoutParameter has wrong number of arguments: %d, was expecting %d\n", argc,
               0);

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'getMargin'.", &tolua_err);
    return 0;
#endif
}

static void extendLayoutParameter(lua_State* L)
{
    lua_pushstring(L, "axui.LayoutParameter");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setMargin", axlua_LayoutParameter_setMargin);
        tolua_function(L, "getMargin", axlua_LayoutParameter_getMargin);
    }
    lua_pop(L, 1);
}

static int tolua_cocos2d_EditBox_registerScriptEditBoxHandler(lua_State* L)
{
    if (NULL == L)
        return 0;

    int argc      = 0;
    EditBox* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.EditBox", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EditBox*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'tolua_cocos2d_EditBox_registerScriptEditBoxHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        self->registerScriptEditBoxHandler(handler);
        return 0;
    }

    luaL_error(L, "%s function of EditBox  has wrong number of arguments: %d, was expecting %d\n",
               "axui.EditBox:registerScriptEditBoxHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_EditBox_registerScriptEditBoxHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_EditBox_unregisterScriptEditBoxHandler(lua_State* L)
{

    if (NULL == L)
        return 0;

    int argc      = 0;
    EditBox* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.EditBox", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EditBox*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'tolua_cocos2d_EditBox_unregisterScriptEditBoxHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        self->unregisterScriptEditBoxHandler();
        return 0;
    }

    luaL_error(L, "%s function of EditBox  has wrong number of arguments: %d, was expecting %d\n",
               "axui.EditBox:unregisterScriptEditBoxHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_EditBox_unregisterScriptEditBoxHandler'.", &tolua_err);
    return 0;
#endif
}

static void extendEditBox(lua_State* L)
{
    lua_pushstring(L, "axui.EditBox");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        lua_pushstring(L, "registerScriptEditBoxHandler");
        lua_pushcfunction(L, tolua_cocos2d_EditBox_registerScriptEditBoxHandler);
        lua_rawset(L, -3);
        lua_pushstring(L, "unregisterScriptEditBoxHandler");
        lua_pushcfunction(L, tolua_cocos2d_EditBox_unregisterScriptEditBoxHandler);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);
}

int register_all_ax_ui_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;
    extendWidget(L);
    extendCheckBox(L);
    extendRadioButton(L);
    extendRadioButtonGroup(L);
    extendSlider(L);
    extendTextField(L);
    extendPageView(L);
    extendScrollView(L);
    extendListView(L);
    extendLayoutParameter(L);
    extendEditBox(L);

    return 0;
}

static int toaxlua_EventListenerFocus_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L, 1, "ax.EventListenerFocus", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        ax::EventListenerFocus* tolua_ret = ax::EventListenerFocus::create();
        if (nullptr == tolua_ret)
            return 0;

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(L, ID, luaID, (void*)tolua_ret, "ax.EventListenerFocus");

        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerFocus:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'toaxlua_EventListenerFocus_create'.", &tolua_err);
    return 0;
#endif
}

static void cloneFocusHandler(const EventListenerFocus* src, EventListenerFocus* dst)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler =
        ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, ScriptHandlerMgr::HandlerType::EVENT_FOCUS);
    if (0 != handler)
    {
        int newscriptHandler =
            ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler,
                                                          ScriptHandlerMgr::HandlerType::EVENT_FOCUS);
        dst->onFocusChanged = [=](ui::Widget* widgetLostFocus, ui::Widget* widgetGetFocus) {
            auto stack   = LuaEngine::getInstance()->getLuaStack();
            lua_State* L = stack->getLuaState();
            int id       = (widgetLostFocus) ? (int)widgetLostFocus->_ID : -1;
            int* luaID   = (widgetLostFocus) ? &widgetLostFocus->_luaID : nullptr;
            toluafix_pushusertype_object(L, id, luaID, (void*)widgetLostFocus, "axui.Widget");
            id    = (widgetGetFocus) ? (int)widgetGetFocus->_ID : -1;
            luaID = (widgetGetFocus) ? &widgetGetFocus->_luaID : nullptr;
            toluafix_pushusertype_object(L, id, luaID, (void*)widgetGetFocus, "axui.Widget");
            stack->executeFunctionByHandler(handler, 2);
        };
    }
}

static int toaxlua_EventListenerFocus_clone(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    EventListenerFocus* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.EventListenerFocus", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerFocus*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'toaxlua_EventListenerFocus_clone'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        ax::EventListenerFocus* tolua_ret = ax::EventListenerFocus::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneFocusHandler(self, tolua_ret);

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(L, ID, luaID, (void*)tolua_ret, "ax.EventListenerFocus");

        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerFocus:clone", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'toaxlua_EventListenerFocus_clone'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_EventListenerFocus_registerScriptHandler(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    EventListenerFocus* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.EventListenerFocus", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerFocus*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'toaxlua_EventListenerFocus_registerScriptHandler'\n", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(L, 2, 0);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler,
                                                          ScriptHandlerMgr::HandlerType::EVENT_FOCUS);

        self->onFocusChanged = [=](ui::Widget* widgetLostFocus, ui::Widget* widgetGetFocus) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            auto Ls    = stack->getLuaState();
            int id     = (widgetLostFocus) ? (int)widgetLostFocus->_ID : -1;
            int* luaID = (widgetLostFocus) ? &widgetLostFocus->_luaID : nullptr;
            toluafix_pushusertype_object(Ls, id, luaID, (void*)widgetLostFocus, "axui.Widget");
            id    = (widgetGetFocus) ? (int)widgetGetFocus->_ID : -1;
            luaID = (widgetGetFocus) ? &widgetGetFocus->_luaID : nullptr;
            toluafix_pushusertype_object(Ls, id, luaID, (void*)widgetGetFocus, "axui.Widget");
            stack->executeFunctionByHandler(handler, 2);
        };
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerFocus:registerScriptHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'toaxlua_EventListenerFocus_registerScriptHandler'.", &tolua_err);
    return 0;
#endif
}

static void extendEventListenerFocusEvent(lua_State* L)
{
    lua_pushstring(L, "ax.EventListenerFocus");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", toaxlua_EventListenerFocus_create);
        tolua_function(L, "registerScriptHandler", toaxlua_EventListenerFocus_registerScriptHandler);
        tolua_function(L, "clone", toaxlua_EventListenerFocus_clone);
    }
    lua_pop(L, 1);
}

int register_ui_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_ui(L);
        register_all_ax_ui_manual(L);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
        register_all_ax_video(L);
        register_all_ax_video_manual(L);
        register_all_ax_webview(L);
        register_all_ax_webview_manual(L);
#elif defined(_WIN32)
#    if defined(AX_ENABLE_MFMEDIA)
        register_all_ax_video(L);
        register_all_ax_video_manual(L);
#    endif
#    if defined(AX_ENABLE_MSEDGE_WEBVIEW2)
        register_all_ax_webview(L);
        register_all_ax_webview_manual(L);
#    endif
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
        register_all_ax_webview(L);
        register_all_ax_webview_manual(L);
#    if defined(AX_ENABLE_VLC_MEDIA)
        register_all_ax_video(L);
        register_all_ax_video_manual(L);
#    endif
#endif
        extendEventListenerFocusEvent(L);
    }
    lua_pop(L, 1);

    return 1;
}
