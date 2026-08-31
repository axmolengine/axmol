/****************************************************************************
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
#include "lua-bindings/adapters/base/axlua_base_adapter.h"
#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/ComponentLua.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "lua-bindings/runtime/LuaCallFunc.h"
#if defined(_MSC_VER) || defined(__MINGW32__)
#    include <WS2tcpip.h>
#else
#    include <sys/socket.h>
#endif

#include "axmol/2d/ActionCamera.h"
#include "axmol/2d/ActionCatmullRom.h"
#include "axmol/2d/ActionGrid.h"
#include "axmol/2d/ActionInterval.h"
#include "axmol/2d/AtlasNode.h"
#include "axmol/scene/Camera.h"
#include "axmol/2d/Menu.h"
#include "axmol/2d/MenuItem.h"
#include "axmol/2d/MotionStreak.h"
#include "axmol/2d/ParticleBatchNode.h"
#include "axmol/2d/ParticleSystem.h"
#include "axmol/scene/Scene.h"
#include "axmol/2d/SpriteBatchNode.h"
#include "axmol/2d/FastTMXLayer.h"
#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/renderer/RenderTexture.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/Properties.h"
#include "axmol/base/Scheduler.h"
#include "axmol/base/UserDefault.h"
#include "axmol/base/Utils.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/RenderView.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/renderer/RenderTexturePass.h"

#ifndef LUAJIT_VERSION
#    include <lspec.h>

class LuaStringBufferAdapter : public ResizableBuffer
{
    lua_State* _state;
    size_t _size{0};
    bool _filled{false};

public:
    bool is_filled() const { return _filled; }
    explicit LuaStringBufferAdapter(lua_State* state) : _state(state) {}
    void resize_and_overwrite(size_t num_of_bytes, std::function<size_t(void*, size_t)> op) override
    {
        _size = op(lua_pushistring(_state, num_of_bytes), num_of_bytes);

        _filled = true;
    }
    size_t size_in_bytes() const override { return _size; }
    static size_t count_element(size_t num_of_bytes) { return num_of_bytes; }
};

#endif

static int axlua_MenuItemImage_create(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.MenuItemImage", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    do
    {
        if (argc == 0)
        {
            MenuItemImage* returnValue = (MenuItemImage*)MenuItemImage::create();
            // Uncheck
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemImage");
            return 1;
        }
    } while (0);
    do
    {
        if (argc == 2)
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
                !axlua::adapter::is_string(luaState, 3, 0, &conversionError))
            {
                ok = false;
            }
#endif
            if (!ok)
            {
                ok = true;
                break;
            }
            auto normalImage           = axlua_tosv(luaState, 2);
            auto selectedImage         = axlua_tosv(luaState, 3);
            MenuItemImage* returnValue = (MenuItemImage*)MenuItemImage::create(normalImage, selectedImage);
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemImage");
            return 1;
        }
    } while (0);
    do
    {
        if (argc == 3)
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
                !axlua::adapter::is_string(luaState, 3, 0, &conversionError) ||
                !axlua::adapter::is_string(luaState, 4, 0, &conversionError))
            {
                goto argumentError;
                break;
            }
#endif
            auto normalImage   = axlua_tosv(luaState, 2);
            auto selectedImage = axlua_tosv(luaState, 3);
            auto disabledImage = axlua_tosv(luaState, 4);

            MenuItemImage* returnValue =
                (MenuItemImage*)MenuItemImage::create(normalImage, selectedImage, disabledImage);
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemImage");
            return 1;
        }
    } while (0);

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemImage:create", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItemImage_create'.\n", &conversionError);
#endif
    return 0;
}

// ComponentLua historically returns the table produced by its script module.
// The native helper communicates that value through the Lua stack and returns
// `void*`, which must never be exposed as a generic sol2 pointer userdata.
static int axlua_ComponentLua_getScriptObject(lua_State* luaState)
{
    if (luaState == nullptr)
        return 0;

    axlua::adapter::Error conversionError;
    if (lua_gettop(luaState) != 1 || !axlua::adapter::is_usertype(luaState, 1, "ax.ComponentLua", 0, &conversionError))
    {
        axlua::adapter::raise_error(luaState, "invalid arguments in function 'ax.ComponentLua:getScriptObject'",
                                    &conversionError);
        return 0;
    }

    auto* self = static_cast<ax::ComponentLua*>(axlua::adapter::to_usertype(luaState, 1, nullptr));
    return self != nullptr ? self->pushScriptObject(luaState) : 0;
}

static void extendComponentLua(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.ComponentLua");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
        axlua::adapter::set_function(luaState, "getScriptObject", axlua_ComponentLua_getScriptObject);
    lua_pop(luaState, 1);
}

static int axlua_MenuItemLabel_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.MenuItemLabel", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    if (1 == argc)
    {

#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_usertype(luaState, 2, "ax.Node", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        Node* label                = ((Node*)axlua::adapter::to_usertype(luaState, 2, 0));
        MenuItemLabel* returnValue = (MenuItemLabel*)MenuItemLabel::create(label);
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemLabel");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemLabel:create", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItemImage_create'.", &conversionError);
    return 0;
#endif
}

static int axlua_MenuItemFont_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.MenuItemFont", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    if (1 == argc)
    {

#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto value                = axlua_tosv(luaState, 2);
        MenuItemFont* returnValue = (MenuItemFont*)MenuItemFont::create(value);
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemFont");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemFont:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItemImage_create'.", &conversionError);
    return 0;
#endif
}

static int axlua_MenuItemSprite_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.MenuItemSprite", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc >= 2 && argc <= 3)
    {

#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_usertype(luaState, 2, "ax.Node", 0, &conversionError) ||
            !axlua::adapter::is_usertype(luaState, 3, "ax.Node", 0, &conversionError))
        {
            goto argumentError;
        }

        if (3 == argc && !axlua::adapter::is_usertype(luaState, 4, "ax.Node", 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        Node* normalSprite   = ((Node*)axlua::adapter::to_usertype(luaState, 2, 0));
        Node* selectedSprite = ((Node*)axlua::adapter::to_usertype(luaState, 3, 0));
        Node* disabledSprite = NULL;
        if (3 == argc)
        {
            disabledSprite = (Node*)axlua::adapter::to_usertype(luaState, 4, 0);
        }
        MenuItemSprite* returnValue =
            (MenuItemSprite*)MenuItemSprite::create(normalSprite, selectedSprite, disabledSprite);
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemSprite");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemSprite:create", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItemImage_create'.", &conversionError);
    return 0;
#endif
}

static int axlua_Menu_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Menu", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc > 0)
    {
        Vector<MenuItem*> items;
        int i = 1;
        while (i <= argc)
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_usertype(luaState, 1 + i, "ax.MenuItem", 0, &conversionError))
                goto argumentError;
#endif

            ax::MenuItem* item = static_cast<ax::MenuItem*>(axlua::adapter::to_usertype(luaState, 1 + i, NULL));
            if (NULL != item)
            {
                items.pushBack(item);
                ++i;
            }
        }
        ax::Menu* returnValue = ax::Menu::createWithArray(items);
        // UnCheck
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.Menu");
        return 1;
    }
    else if (argc == 0)
    {
        ax::Menu* returnValue = ax::Menu::create();
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.Menu");
        return 1;
    }

    luaL_error(luaState, "create wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Menu_create'.\n", &conversionError);
#endif
    return 0;
}

static int toaxlua_Menu_alignItemsInRows(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc   = 0;
    Menu* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Menu", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<Menu*>(axlua::adapter::to_usertype(luaState, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_Menu_alignItemsInRows'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc > 0)
    {
        ValueVector items;
        if (luavals_variadic_to_ccvaluevector(luaState, argc, &items))
        {
            self->alignItemsInRowsWithArray(items);
        }
        return 0;
    }

    luaL_error(luaState,
               "'alignItemsInRows' has wrong number of arguments in toaxlua_Menu_alignItemsInRows: %d, was "
               "expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'alignItemsInRows'.\n", &conversionError);
#endif
    return 0;
}

static int toaxlua_Menu_alignItemsInColumns(lua_State* luaState)
{

    if (nullptr == luaState)
        return 0;

    int argc   = 0;
    Menu* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Menu", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<Menu*>(axlua::adapter::to_usertype(luaState, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'toaxlua_Menu_alignItemsInColumns'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc > 0)
    {
        ValueVector items;
        if (luavals_variadic_to_ccvaluevector(luaState, argc, &items))
        {
            self->alignItemsInColumnsWithArray(items);
        }
        return 0;
    }

    luaL_error(luaState,
               "'alignItemsInColumns' has wrong number of arguments in toaxlua_Menu_alignItemsInColumns: %d, "
               "was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_Menu_alignItemsInColumns'.\n",
                                &conversionError);
#endif
    return 0;
}

static int axlua_MenuItemToggle_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.MenuItemToggle", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc >= 1)
    {
        MenuItemToggle* returnValue = MenuItemToggle::create();
        if (NULL == returnValue)
        {
            return 0;
        }

        for (int i = 0; i < argc; ++i)
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_usertype(luaState, i + 2, "ax.MenuItem", 0, &conversionError))
            {
                goto argumentError;
            }
#endif
            MenuItem* item = static_cast<MenuItem*>(axlua::adapter::to_usertype(luaState, i + 2, 0));
            returnValue->addSubItem(item);
        }
        returnValue->setSelectedIndex(0);

        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.MenuItemToggle");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemToggle:create", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItemToggle_create'.", &conversionError);
    return 0;
#endif
}

static int axlua_MenuItem_registerScriptTapHandler(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc      = 0;
    MenuItem* obj = nullptr;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.MenuItem", 0, &conversionError))
        goto argumentError;
#endif
    obj = static_cast<ax::MenuItemImage*>(axlua::adapter::to_usertype(luaState, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_MenuItem_registerScriptTapHandler'\n",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto callback = axlua::Callback<void(int, MenuItem*)>(luaState, 2);
        obj->setCallback([callback = std::move(callback)](Object* sender) mutable {
            auto* item = static_cast<MenuItem*>(sender);
            if (item)
                callback(item->getTag(), item);
        });
        return 0;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.MenuItem:registerScriptTapHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItem_registerScriptTapHandler'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_MenuItem_unregisterScriptTapHandler(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc      = 0;
    MenuItem* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.MenuItem", 0, &conversionError))
        goto argumentError;
#endif

    obj = static_cast<ax::MenuItemImage*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_MenuItem_unregisterScriptTapHandler'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (0 == argc)
    {
        obj->setCallback(nullptr);
        return 0;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.MenuItem:unregisterScriptTapHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_MenuItem_unregisterScriptTapHandler'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_Scheduler_scheduleScriptFunc(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc        = 0;
    Scheduler* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Scheduler", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::Scheduler*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_Scheduler_scheduleScriptFunc'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
            !axlua::adapter::is_boolean(luaState, 4, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        axlua::Callback<void(float)> callback(luaState, 2);
        float interval           = (float)axlua::adapter::to_number(luaState, 3, 0);
        bool paused              = (bool)axlua::adapter::to_boolean(luaState, 4, 0);
        unsigned int returnValue = (unsigned int)self->scheduleScriptFunc(callback.function(), interval, paused);
        lua_pushnumber(luaState, (lua_Number)returnValue);
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Scheduler:scheduleScriptFunc",
               argc, 3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Scheduler_scheduleScriptFunc'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_Scheduler_unscheduleScriptEntry(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc        = 0;
    Scheduler* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Scheduler", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::Scheduler*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_Scheduler_unscheduleScriptEntry'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_number(luaState, 2, 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        unsigned int scheduleScriptEntryID = ((unsigned int)axlua::adapter::to_number(luaState, 2, 0));
        self->unscheduleScriptEntry(scheduleScriptEntryID);
        return 0;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Scheduler:unscheduleScriptEntry", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Scheduler_unscheduleScriptEntry'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_RenderTexture_newImage(lua_State* luaState)
{
    int argc               = 0;
    ax::RenderTexture* obj = nullptr;
    bool ok                = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.RenderTexture", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::RenderTexture*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_RenderTexture_newImage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1 || argc == 2)
    {
#if _AX_DEBUG >= 1
        ok &= axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError);
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_RenderTexture_newImage'",
                                        nullptr);
            return 0;
        }
#endif
        axlua::Callback<void(ax::Image*)> callback(luaState, 2);
        auto imageCallback = [callback = std::move(callback)](RefPtr<ax::Image> image) mutable {
            callback(image.get());
        };

        obj->newImage(imageCallback);
        return 0;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.RenderTexture:newImage", argc,
               2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_RenderTexture_newImage'.", &conversionError);
#endif

    return 0;
}

int axlua_Sequence_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

    axlua::adapter::Error conversionError;

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Sequence", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc > 0)
    {
        Vector<FiniteTimeAction*> array;

        if (1 == argc && axlua::adapter::is_table(luaState, 2, 0, &conversionError))
        {
            luaval_to_axvector(luaState, 2, &array, "ax.Sequence:create");
        }
        else
        {
            int i = 1;
            while (i <= argc)
            {
#if _AX_DEBUG >= 1
                if (!axlua::adapter::is_usertype(luaState, 1 + i, "ax.FiniteTimeAction", 0, &conversionError))
                    goto argumentError;
#endif

                ax::FiniteTimeAction* item =
                    static_cast<ax::FiniteTimeAction*>(axlua::adapter::to_usertype(luaState, 1 + i, nullptr));
                if (nullptr != item)
                {
                    array.pushBack(item);
                }
                ++i;
            }
        }

        ax::Sequence* returnValue = ax::Sequence::create(array);
        // issue 2433 uncheck
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.Sequence");
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Sequence:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Sequence_create'.", &conversionError);
    return 0;
#endif
}

static int axlua_CallFunc_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.CallFunc", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1 || argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
            goto argumentError;
#endif

        if (argc == 2)
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_table(luaState, 3, 0, &conversionError))
                goto argumentError;
#endif
        }
        LuaCallFunc* returnValue = new LuaCallFunc();
        if (argc == 2)
        {
            sol::table extra(luaState, 3);
            axlua::Callback<void(Node*, sol::table)> callback(luaState, 2);
            returnValue->initWithFunction([callback = std::move(callback), extra = std::move(extra)](
                                              void*, Node* target) mutable { callback(target, extra); });
        }
        else
        {
            axlua::Callback<void(Node*)> callback(luaState, 2);
            returnValue->initWithFunction(
                [callback = std::move(callback)](void*, Node* target) mutable { callback(target); });
        }
        returnValue->autorelease();

        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.CallFunc");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CallFunc:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_CallFunc_create'.", &conversionError);
    return 0;
#endif
}

// Keep the historical two-number Node:getPosition() result while the
// generator exposes modern Vec2 returns for other APIs. The C++ overload
// taking float* is intentionally not generated because Lua cannot provide
// addressable scalar pointers.
static int axlua_Node_getPosition(lua_State* luaState)
{
    if (luaState == nullptr)
        return 0;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Node", 0, nullptr))
        return luaL_error(luaState, "invalid 'self' in function 'ax.Node:setAnchorPoint'");
    auto* node = static_cast<ax::Node*>(axlua::adapter::to_usertype(luaState, 1, nullptr));
    if (node == nullptr)
        return luaL_error(luaState, "invalid 'self' in function 'ax.Node:getPosition'");
    if (lua_gettop(luaState) != 1)
        return luaL_error(luaState, "ax.Node:getPosition expects no arguments");
    const auto& position = node->getPosition();
    lua_pushnumber(luaState, position.x);
    lua_pushnumber(luaState, position.y);
    return 2;
}

// setAnchorPoint historically accepted either a Vec2 table or two scalar
// coordinates. Keep that small Lua convenience overload without changing the
// public C++ API or reintroducing the old general-purpose wrappers.
static int axlua_Node_setAnchorPoint(lua_State* luaState)
{
    if (luaState == nullptr)
        return 0;
    auto* node = static_cast<ax::Node*>(axlua::adapter::to_usertype(luaState, 1, nullptr));
    if (node == nullptr)
        return luaL_error(luaState, "invalid 'self' in function 'ax.Node:setAnchorPoint'");

    const int argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        if (!lua_istable(luaState, 2))
            return luaL_error(luaState, "ax.Node:setAnchorPoint expects a Vec2 table or two numbers");
        ax::Vec2 point;
        if (!luaval_to_vec2(luaState, 2, &point, "ax.Node:setAnchorPoint"))
            return luaL_error(luaState, "invalid Vec2 table in function 'ax.Node:setAnchorPoint'");
        node->setAnchorPoint(point);
    }
    else if (argc == 2)
    {
        if (!lua_isnumber(luaState, 2) || !lua_isnumber(luaState, 3))
            return luaL_error(luaState, "ax.Node:setAnchorPoint expects two numbers");
        node->setAnchorPoint(
            ax::Vec2(static_cast<float>(lua_tonumber(luaState, 2)), static_cast<float>(lua_tonumber(luaState, 3))));
    }
    else
    {
        return luaL_error(luaState, "ax.Node:setAnchorPoint expects a Vec2 table or two numbers");
    }

    lua_settop(luaState, 1);
    return 1;
}

static int axlua_Node_enumerateChildren(lua_State* luaState)
{
    int argc      = 0;
    ax::Node* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Node", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Node*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Node_enumerateChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        auto name = axlua_tosv(luaState, 2);
        axlua::Callback<bool(Node*)> callback(luaState, 3);
        obj->enumerateChildren(name, [callback = std::move(callback)](Node* node) mutable { return callback(node); });
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "enumerateChildren", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Node_enumerateChildren'.", &conversionError);
#endif

    return 0;
}

int axlua_Node_setAdditionalTransform(lua_State* luaState)
{
    int argc      = 0;
    ax::Node* obj = nullptr;
    bool ok       = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Node", 0, &conversionError))
        goto argumentError;
#endif
    obj = (ax::Node*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Node_setAdditionalTransform'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;
    do
    {
        if (argc == 1)
        {
            ax::AffineTransform arg0;
            ok &= luaval_to_affinetransform(luaState, 2, &arg0, "ax.Node:setAdditionalTransform");

            if (!ok)
            {
                break;
            }
            obj->setAdditionalTransform(arg0);
            lua_settop(luaState, 1);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            ax::Mat4 arg0;
            ok &= luaval_to_mat4(luaState, 2, &arg0, "ax.Node:setAdditionalTransform");

            if (!ok)
            {
                break;
            }
            obj->setAdditionalTransform(&arg0);
            lua_settop(luaState, 1);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Node:setAdditionalTransform",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Node_setAdditionalTransform'.", &conversionError);
#endif

    return 0;
}

#if defined(AX_ENABLE_NAVMESH)
#    include "axmol/navmesh/NavMesh.h"
int axlua_Scene_setNavMesh(lua_State* luaState)
{
    int argc       = 0;
    ax::Scene* obj = nullptr;
    bool ok        = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Scene", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::Scene*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Scene_setNavMesh'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::NavMesh* arg0;

        ok &= luaval_to_object<ax::NavMesh>(luaState, 2, "ax.NavMesh", &arg0, "ax.Scene:setNavMesh");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Scene_setNavMesh'", nullptr);
            return 0;
        }
        obj->setNavMesh(arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:setNavMesh", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Scene_setNavMesh'.", &conversionError);
#    endif

    return 0;
}

int axlua_Scene_getNavMesh(lua_State* luaState)
{
    int argc       = 0;
    ax::Scene* obj = nullptr;
    bool ok        = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Scene", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::Scene*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Scene_getNavMesh'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Scene_getNavMesh'", nullptr);
            return 0;
        }
        ax::NavMesh* ret = obj->getNavMesh();
        object_to_luaval<ax::NavMesh>(luaState, "ax.NavMesh", (ax::NavMesh*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:getNavMesh", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Scene_getNavMesh'.", &conversionError);
#    endif

    return 0;
}

#endif  // #if defined(AX_ENABLE_NAVMESH)

static int axlua_Spawn_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;

    axlua::adapter::Error conversionError;
#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Spawn", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc > 0)
    {
        Vector<FiniteTimeAction*> array;
        int i = 1;

        if (1 == argc && axlua::adapter::is_table(luaState, 2, 0, &conversionError))
        {
            luaval_to_axvector(luaState, 2, &array, "ax.Spawn:create");
        }
        else
        {
            while (i <= argc)
            {
#if _AX_DEBUG >= 1
                if (!axlua::adapter::is_usertype(luaState, 1 + i, "ax.FiniteTimeAction", 0, &conversionError))
                    goto argumentError;
#endif

                ax::FiniteTimeAction* item =
                    static_cast<ax::FiniteTimeAction*>(axlua::adapter::to_usertype(luaState, 1 + i, NULL));
                if (NULL != item)
                {
                    array.pushBack(item);
                    ++i;
                }
            }
        }

        ax::Spawn* returnValue = ax::Spawn::create(array);
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.Spawn");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Spawn:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Spawn_create'.", &conversionError);
    return 0;
#endif
}

int lua_cocos2d_CardinalSplineBy_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.CardinalSplineBy", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 3)
    {
        double dur = 0.0;
        ok &= luaval_to_number(luaState, 2, &dur, "ax.CardinalSplineBy:create");
        if (!ok)
            return 0;

        int num       = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(luaState, 3, &arr, &num, "ax.CardinalSplineBy:create");
        if (!ok)
            return 0;

        double ten = 0.0;
        ok &= luaval_to_number(luaState, 4, &ten, "ax.CardinalSplineBy:create");
        if (!ok)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        if (num > 0)
        {
            PointArray* points = PointArray::create(num);

            if (NULL == points)
            {
                AX_SAFE_DELETE_ARRAY(arr);
                return 0;
            }

            for (int i = 0; i < num; i++)
            {
                points->addControlPoint(arr[i]);
            }

            AX_SAFE_DELETE_ARRAY(arr);
            CardinalSplineBy* returnValue = CardinalSplineBy::create((float)dur, points, (float)ten);
            if (NULL != returnValue)
            {
                axlua::adapter::push_object(luaState, (void*)returnValue, "ax.CardinalSplineBy");
                return 1;
            }
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CardinalSplineBy:create", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_cocos2d_CardinalSplineBy_create'.",
                                &conversionError);
    return 0;
#endif
}

int axlua_CatmullRomBy_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.CatmullRomBy", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 2)
    {
        double dur = 0.0;
        ok &= luaval_to_number(luaState, 2, &dur, "ax.CatmullRomBy:create");
        if (!ok)
            return 0;

        int num       = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(luaState, 3, &arr, &num, "ax.CatmullRomBy:create");
        if (!ok)
            return 0;

        if (num > 0)
        {
            PointArray* points = PointArray::create(num);

            if (NULL == points)
            {
                AX_SAFE_DELETE_ARRAY(arr);
                return 0;
            }

            for (int i = 0; i < num; i++)
            {
                points->addControlPoint(arr[i]);
            }

            AX_SAFE_DELETE_ARRAY(arr);
            CatmullRomBy* returnValue = CatmullRomBy::create((float)dur, points);
            if (NULL != returnValue)
            {
                axlua::adapter::push_object(luaState, (void*)returnValue, "ax.CatmullRomBy");
                return 1;
            }
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CatmullRomBy:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_CatmullRomBy_create'.", &conversionError);
    return 0;
#endif
}

int axlua_CatmullRomTo_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.CatmullRomTo", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 2)
    {
        double dur = 0.0;
        ok &= luaval_to_number(luaState, 2, &dur, "ax.CatmullRomTo:create");
        if (!ok)
            return 0;

        int num       = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(luaState, 3, &arr, &num, "ax.CatmullRomTo:create");
        if (!ok)
            return 0;

        if (num > 0)
        {
            PointArray* points = PointArray::create(num);

            if (NULL == points)
            {
                AX_SAFE_DELETE_ARRAY(arr);
                return 0;
            }

            for (int i = 0; i < num; i++)
            {
                points->addControlPoint(arr[i]);
            }

            AX_SAFE_DELETE_ARRAY(arr);
            CatmullRomTo* returnValue = CatmullRomTo::create((float)dur, points);
            if (NULL != returnValue)
            {
                axlua::adapter::push_object(luaState, (void*)returnValue, "ax.CatmullRomTo");
                return 1;
            }
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CatmullRomTo:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_CatmullRomTo_create'.", &conversionError);
    return 0;
#endif
}

int axlua_BezierBy_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.BezierBy", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 2)
    {
        double t = 0.0;
        ok &= luaval_to_number(luaState, 2, &t, "ax.BezierBy:create");
        if (!ok)
            return 0;

        int num       = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(luaState, 3, &arr, &num, "ax.BezierBy:create");
        if (!ok)
            return 0;

        if (num < 3)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        BezierConfig config;
        config.controlPoint_1 = arr[0];
        config.controlPoint_2 = arr[1];
        config.endPosition    = arr[2];
        AX_SAFE_DELETE_ARRAY(arr);

        BezierBy* returnValue = BezierBy::create((float)t, config);
        if (NULL != returnValue)
        {
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.BezierBy");
            return 1;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.BezierBy:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_BezierBy_create'.", &conversionError);
    return 0;
#endif
}

int axlua_BezierTo_create(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.BezierTo", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 2)
    {
        double t = 0.0;
        ok &= luaval_to_number(luaState, 2, &t, "ax.BezierTo:create");
        if (!ok)
            return 0;

        int num       = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(luaState, 3, &arr, &num, "ax.BezierTo:create");
        if (!ok)
            return 0;

        if (num < 3)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        BezierConfig config;
        config.controlPoint_1 = arr[0];
        config.controlPoint_2 = arr[1];
        config.endPosition    = arr[2];
        AX_SAFE_DELETE_ARRAY(arr);

        BezierTo* returnValue = BezierTo::create((float)t, config);
        if (NULL != returnValue)
        {
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.BezierTo");
            return 1;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.BezierTo:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_BezierTo_create'.", &conversionError);
    return 0;
#endif
}

static int toaxlua_DrawNode_drawPolygon(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc       = 0;
    DrawNode* self = nullptr;

    axlua::adapter::Error conversionError;
#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::DrawNode*>(axlua::adapter::to_usertype(luaState, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawPolygon'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (5 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 4, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 5, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 6, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        int size = lua_tointeger(luaState, 3);
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (int i = 0; i < size; i++)
            {
                lua_pushnumber(luaState, i + 1);
                lua_gettable(luaState, 2);
                if (!axlua::adapter::is_table(luaState, -1, 0, &conversionError))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto argumentError;
#endif
                }

                if (!luaval_to_vec2(luaState, lua_gettop(luaState), &points[i], "ax.DrawNode:drawPolygon"))
                {
                    lua_pop(luaState, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(luaState, 1);
            }

            ax::Color fillColor;
            if (!luaval_to_color(luaState, 4, &fillColor, "ax.DrawNode:drawPolygon"))
            {
                AX_SAFE_DELETE_ARRAY(points);
                return 0;
            }

            float borderWidth = (float)axlua::adapter::to_number(luaState, 5, 0);

            ax::Color borderColor;
            if (!luaval_to_color(luaState, 6, &borderColor, "ax.DrawNode:drawPolygon"))
            {
                AX_SAFE_DELETE_ARRAY(points);
                return 0;
            }

            self->drawPolygon(points, (int)size, fillColor, borderWidth, borderColor);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.DrawNode:drawPolygon", argc,
               5);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawPolygon'.", &conversionError);
    return 0;
#endif
}

int toaxlua_DrawNode_drawSolidPoly(lua_State* luaState)
{
    int argc           = 0;
    ax::DrawNode* self = nullptr;
    bool ok            = true;

    axlua::adapter::Error conversionError;

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::DrawNode*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawSolidPoly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        unsigned int size;
        luaval_to_int(luaState, 3, &size, "ax.DrawNode:drawSolidPoly");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(luaState, i + 1);
                lua_gettable(luaState, 2);
                if (!axlua::adapter::is_table(luaState, -1, 0, &conversionError))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto argumentError;
#endif
                }

                if (!luaval_to_vec2(luaState, lua_gettop(luaState), &points[i], "ax.DrawNode:drawSolidPoly"))
                {
                    lua_pop(luaState, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(luaState, 1);
            }

            ax::Color arg2;

            ok &= luaval_to_color(luaState, 4, &arg2, "ax.DrawNode:drawSolidPoly");
            if (!ok)
                return 0;
            self->drawSolidPoly(points, size, arg2);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawSolidPoly", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawSolidPoly'.", &conversionError);
#endif

    return 0;
}

int toaxlua_DrawNode_drawPoly(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc       = 0;
    DrawNode* self = nullptr;
    bool ok        = true;

    axlua::adapter::Error conversionError;

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ax::DrawNode*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawPoly'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 4)
    {
        unsigned int size;
        luaval_to_int(luaState, 3, &size, "ax.DrawNode:drawPoly");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(luaState, i + 1);
                lua_gettable(luaState, 2);
                if (!axlua::adapter::is_table(luaState, -1, 0, &conversionError))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto argumentError;
#endif
                }

                if (!luaval_to_vec2(luaState, lua_gettop(luaState), &points[i], "ax.DrawNode:drawPoly"))
                {
                    lua_pop(luaState, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(luaState, 1);
            }

            bool arg2;
            ax::Color arg3;

            ok &= luaval_to_boolean(luaState, 4, &arg2, "ax.DrawNode:drawPoly");

            ok &= luaval_to_color(luaState, 5, &arg3, "ax.DrawNode:drawPoly");
            if (!ok)
                return 0;

            self->drawPoly(points, size, arg2, arg3);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawPoly", argc, 4);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawPoly'.", &conversionError);
#endif

    return 0;
}

int toaxlua_DrawNode_drawCardinalSpline(lua_State* luaState)
{
    int argc           = 0;
    ax::DrawNode* self = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::DrawNode*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawCardinalSpline'",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 4)
    {
        int num       = 0;
        ax::Vec2* arr = NULL;
        if (!luaval_to_array_of_vec2(luaState, 2, &arr, &num, "ax.DrawNode:drawCardinalSpline"))
            return 0;
        PointArray* config = PointArray::create(num);
        if (NULL == config)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        for (int i = 0; i < num; i++)
        {
            config->addControlPoint(arr[i]);
        }
        AX_SAFE_DELETE_ARRAY(arr);

        double arg1;
        unsigned int arg2;
        ax::Color arg3;

        ok &= luaval_to_number(luaState, 3, &arg1, "ax.DrawNode:drawCardinalSpline");

        ok &= luaval_to_int(luaState, 4, &arg2, "ax.DrawNode:drawCardinalSpline");

        ok &= luaval_to_color(luaState, 5, &arg3, "ax.DrawNode:drawCardinalSpline");
        if (!ok)
            return 0;
        self->drawCardinalSpline(config, (float)arg1, arg2, arg3);
        return 0;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawCardinalSpline",
               argc, 4);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawCardinalSpline'.", &conversionError);
#endif

    return 0;
}

int toaxlua_DrawNode_drawCatmullRom(lua_State* luaState)
{
    int argc           = 0;
    ax::DrawNode* self = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::DrawNode*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawCatmullRom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        int num       = 0;
        ax::Vec2* arr = NULL;
        if (!luaval_to_array_of_vec2(luaState, 2, &arr, &num, "ax.DrawNode:drawCatmullRom"))
            return 0;
        PointArray* config = PointArray::create(num);
        if (NULL == config)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        for (int i = 0; i < num; i++)
        {
            config->addControlPoint(arr[i]);
        }
        AX_SAFE_DELETE_ARRAY(arr);

        unsigned int arg1;
        ax::Color arg2;

        ok &= luaval_to_int(luaState, 3, &arg1, "ax.DrawNode:drawCatmullRom");

        ok &= luaval_to_color(luaState, 4, &arg2, "ax.DrawNode:drawCatmullRom");
        if (!ok)
            return 0;
        self->drawCatmullRom(config, arg1, arg2);
        return 0;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawCatmullRom",
               argc, 3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawCatmullRom'.", &conversionError);
#endif

    return 0;
}

int toaxlua_DrawNode_drawPoints(lua_State* luaState)
{
    int argc           = 0;
    ax::DrawNode* self = nullptr;
    bool ok            = true;

    axlua::adapter::Error conversionError;

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.DrawNode", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::DrawNode*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_DrawNode_drawPoints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
        unsigned int size;
        luaval_to_int(luaState, 3, &size, "ax.DrawNode:drawPoints");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(luaState, i + 1);
                lua_gettable(luaState, 2);
                if (!axlua::adapter::is_table(luaState, -1, 0, &conversionError))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto argumentError;
#endif
                }

                if (!luaval_to_vec2(luaState, lua_gettop(luaState), &points[i], "ax.DrawNode:drawPoints"))
                {
                    lua_pop(luaState, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(luaState, 1);
            }

            ax::Color arg2;

            ok &= luaval_to_color(luaState, 4, &arg2, "ax.DrawNode:drawPoints");
            if (!ok)
                return 0;
            self->drawPoints(points, size, arg2);
            return 0;
        }
    }
    else if (argc == 4)
    {
        unsigned int size;
        luaval_to_int(luaState, 3, &size, "ax.DrawNode:drawPoints");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(luaState, i + 1);
                lua_gettable(luaState, 2);
                if (!axlua::adapter::is_table(luaState, -1, 0, &conversionError))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto argumentError;
#endif
                }

                if (!luaval_to_vec2(luaState, lua_gettop(luaState), &points[i], "ax.DrawNode:drawPoints"))
                {
                    lua_pop(luaState, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(luaState, 1);
            }

            float pointSize = (float)axlua::adapter::to_number(luaState, 4, 0);
            ax::Color color;
            ok &= luaval_to_color(luaState, 5, &color, "ax.DrawNode:drawPoints");
            if (!ok)
                return 0;
            self->drawPoints(points, size, pointSize, color);
            return 0;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawPoints", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_DrawNode_drawPoints'.", &conversionError);
#endif

    return 0;
}

static int toaxlua_LayerMultiplex_create(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;
    Vector<Node*> arg0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.LayerMultiplex", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc > 0)
    {
        if (luavals_variadic_to_axvector(luaState, argc, &arg0))
        {
            LayerMultiplex* returnValue = LayerMultiplex::createWithArray(arg0);
            axlua::adapter::push_object(luaState, (void*)returnValue, "ax.LayerMultiplex");
            return 1;
        }
        else
        {
            luaL_error(luaState, "error in toaxlua_LayerMultiplex_create \n");
            return 0;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.LayerMultiplex:create", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_LayerMultiplex_create'.", &conversionError);
    return 0;
#endif
}
int lua_ax_base_FileUtils_setSearchPaths(lua_State* luaState)
{
    int argc           = 0;
    ax::FileUtils* obj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.FileUtils", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::FileUtils*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_base_FileUtils_setSearchPaths'",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        std::vector<std::string_view> arg0;

        ok &= luaval_to_std_vector_string_view(luaState, 2, &arg0, "ax.FileUtils:setSearchPaths");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_base_FileUtils_setSearchPaths'", nullptr);
            return 0;
        }
        obj->setSearchPaths(arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FileUtils:setSearchPaths",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_base_FileUtils_setSearchPaths'.",
                                &conversionError);
#endif

    return 0;
}
static int toaxlua_FileUtils_getStringFromFile(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc        = 0;
    FileUtils* self = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.FileUtils", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<FileUtils*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'toaxlua_FileUtils_getStringFromFile'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
        std::string_view arg0;
        ok = luaval_to_std_string_view(luaState, 2, &arg0, "ax.FileUtils:getStringFromFile");
        if (ok)
        {
#ifndef LUAJIT_VERSION
            LuaStringBufferAdapter adapter(luaState);
            FileUtils::getInstance()->getContents(arg0, &adapter);
            if (!adapter.is_filled()) [[unlikely]]
                axlua::adapter::push_literal(luaState, "");
#else
            std::string content = FileUtils::getInstance()->getStringFromFile(arg0);

            lua_pushlstring(luaState, content.c_str(), content.size());
#endif
            return 1;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FileUtils:getStringFromFile",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_FileUtils_getStringFromFile'.",
                                &conversionError);
    return 0;
#endif
}

static int toaxlua_FileUtils_getDataFromFile(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc        = 0;
    FileUtils* self = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.FileUtils", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<FileUtils*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'toaxlua_FileUtils_getDataFromFile'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
        std::string_view arg0;
        ok = luaval_to_std_string_view(luaState, 2, &arg0, "ax.FileUtils:getDataFromFile");
        if (ok)
        {
#ifndef LUAJIT_VERSION
            LuaStringBufferAdapter adapter(luaState);
            FileUtils::getInstance()->getContents(arg0, &adapter);
            if (!adapter.is_filled()) [[unlikely]]
                lua_pushnil(luaState);
#else
            auto data = FileUtils::getInstance()->getDataFromFile(arg0);
            if (!data.isNull())
                lua_pushlstring(luaState, reinterpret_cast<const char*>(data.getBytes()),
                                static_cast<size_t>(data.getSize()));
            else
                lua_pushnil(luaState);
#endif
            return 1;
        }
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FileUtils:getDataFromFile",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_FileUtils_getDataFromFile'.", &conversionError);
    return 0;
#endif
}

static int toaxlua_UserDefault_getInstance(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.UserDefault", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (0 == argc)
    {
        UserDefault* returnValue = (UserDefault*)UserDefault::getInstance();
        axlua::adapter::push_usertype(luaState, (void*)returnValue, "ax.UserDefault");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.UserDefault:getInstance", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_UserDefault_getInstance'.", &conversionError);
    return 0;
#endif
}

static int toaxlua_Texture2D_setTexParameters(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc        = 0;
    Texture2D* self = nullptr;
    rhi::SamplerFilter arg1;
    rhi::SamplerFilter arg2;
    rhi::SamplerAddressMode arg3;
    rhi::SamplerAddressMode arg4;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Texture2D", 0, &conversionError))
        goto argumentError;
#endif

    self = (Texture2D*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'toaxlua_Texture2D_setTexParameters'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (4 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 4, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 5, 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        arg1 = (rhi::SamplerFilter)(int)axlua::adapter::to_number(luaState, 2, 0);
        arg2 = (rhi::SamplerFilter)(int)axlua::adapter::to_number(luaState, 3, 0);
        arg3 = (rhi::SamplerAddressMode)(int)axlua::adapter::to_number(luaState, 4, 0);
        arg4 = (rhi::SamplerAddressMode)(int)axlua::adapter::to_number(luaState, 5, 0);

        Texture2D::TexParams param{.minFilter = arg1, .magFilter = arg2, .sAddressMode = arg3, .tAddressMode = arg4};

        self->setTexParameters(param);

        return 0;
    }

    luaL_error(luaState, "'setTexParameters' function of Texture2D wrong number of arguments: %d, was expecting %d\n",
               argc, 4);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'setTexParameters'.", &conversionError);
    return 0;
#endif
}

static int toaxlua_SpriteBatchNode_getDescendants(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc                 = 0;
    ax::SpriteBatchNode* obj = nullptr;
    bool ok                  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.SpriteBatchNode", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::SpriteBatchNode*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'toaxlua_SpriteBatchNode_getDescendants'",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        std::vector<Sprite*> ret = obj->getDescendants();

        lua_newtable(luaState);

        if (ret.empty())
            return 1;

        auto iter      = ret.begin();
        int indexTable = 1;
        for (; iter != ret.end(); ++iter)
        {
            if (nullptr == *iter)
                continue;

            lua_pushnumber(luaState, (lua_Number)indexTable);
            axlua::adapter::push_object(luaState, (void*)(*iter), "ax.Sprite");
            lua_rawset(luaState, -3);
            (*iter)->retain();
            ++indexTable;
        }

        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.SpriteBatchNode:getDescendants", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_SpriteBatchNode_getDescendants'.",
                                &conversionError);
#endif
    return 0;
}

#if defined(AX_ENABLE_PHYSICS_3D)
#    include "axmol/physics/3d/PhysicsWorld3D.h"
int axlua_Scene_getPhysicsWorld3D(lua_State* luaState)
{
    int argc       = 0;
    ax::Scene* obj = nullptr;
    bool ok        = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Scene", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::Scene*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Scene_getPhysicsWorld3D'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Scene_getPhysicsWorld3D'",
                                        nullptr);
            return 0;
        }
        ax::PhysicsWorld3D* ret = obj->getPhysicsWorld3D();
        object_to_luaval<ax::PhysicsWorld3D>(luaState, "ax.PhysicsWorld3D", (ax::PhysicsWorld3D*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:getPhysicsWorld3D",
               argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Scene_getPhysicsWorld3D'.", &conversionError);
#    endif

    return 0;
}

#endif

int axlua_RenderTexturePass_setViewport(lua_State* luaState)
{
    int argc                   = 0;
    ax::RenderTexturePass* obj = nullptr;
    bool ok                    = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.RenderTexturePass", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::RenderTexturePass*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_base_RenderTexturePass_setViewport'",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        if (!lua_isnil(luaState, 2))
        {
            ax::rhi::RectI arg0;
            ok &= luaval_to_recti(luaState, 2, &arg0, "ax.RenderTexturePass:setViewport");
            if (!ok && !lua_isnil(luaState, 2))
            {
                axlua::adapter::raise_error(
                    luaState, "invalid arguments in function 'lua_ax_base_RenderTexturePass_setViewport'", nullptr);
                return 0;
            }
            obj->setViewport(arg0);
        }
        else
        {
            obj->setViewport(std::nullopt);
        }
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.RenderTexturePass:setViewport", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_base_RenderTexturePass_setViewport'.",
                                &conversionError);
#endif

    return 0;
}

static void extendScene(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Scene");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
#if defined(AX_ENABLE_PHYSICS_3D)
        axlua::adapter::set_function(luaState, "getPhysicsWorld3D", axlua_Scene_getPhysicsWorld3D);
#endif

#if defined(AX_ENABLE_NAVMESH)
        axlua::adapter::set_function(luaState, "setNavMesh", axlua_Scene_setNavMesh);
        axlua::adapter::set_function(luaState, "getNavMesh", axlua_Scene_getNavMesh);
#endif
    }
    lua_pop(luaState, 1);
}

static void extendTexture2D(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Texture2D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "setTexParameters");
        lua_pushcfunction(luaState, toaxlua_Texture2D_setTexParameters);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItem(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItem");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "registerScriptTapHandler");
        lua_pushcfunction(luaState, axlua_MenuItem_registerScriptTapHandler);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "unregisterScriptTapHandler");
        lua_pushcfunction(luaState, axlua_MenuItem_unregisterScriptTapHandler);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItemImage(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItemImage");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_MenuItemImage_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItemLabel(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItemLabel");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_MenuItemLabel_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItemFont(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItemFont");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_MenuItemFont_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItemSprite(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItemSprite");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_MenuItemSprite_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenuItemToggle(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.MenuItemToggle");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_MenuItemToggle_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendMenu(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Menu");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_Menu_create);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "alignItemsInRows");
        lua_pushcfunction(luaState, toaxlua_Menu_alignItemsInRows);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "alignItemsInColumns");
        lua_pushcfunction(luaState, toaxlua_Menu_alignItemsInColumns);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendNode(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Node");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getPosition", axlua_Node_getPosition);
        axlua::adapter::set_function(luaState, "setAnchorPoint", axlua_Node_setAnchorPoint);
        axlua::adapter::set_function(luaState, "enumerateChildren", axlua_Node_enumerateChildren);
        axlua::adapter::set_function(luaState, "setAdditionalTransform", axlua_Node_setAdditionalTransform);
    }
    lua_pop(luaState, 1);
}

static void extendScheduler(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Scheduler");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "scheduleScriptFunc");
        lua_pushcfunction(luaState, axlua_Scheduler_scheduleScriptFunc);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "unscheduleScriptEntry");
        lua_pushcfunction(luaState, axlua_Scheduler_unscheduleScriptEntry);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendRenderTexture(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.RenderTexture");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "newImage");
        lua_pushcfunction(luaState, axlua_RenderTexture_newImage);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendSequence(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Sequence");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_Sequence_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendCallFunc(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.CallFunc");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_CallFunc_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendSpawn(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Spawn");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_Spawn_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendCardinalSplineBy(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.CardinalSplineBy");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, lua_cocos2d_CardinalSplineBy_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendCatmullRomBy(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.CatmullRomBy");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_CatmullRomBy_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendCatmullRomTo(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.CatmullRomTo");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_CatmullRomTo_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendBezierBy(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.BezierBy");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_BezierBy_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendBezierTo(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.BezierTo");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, axlua_BezierTo_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendDrawNode(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.DrawNode");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "drawPolygon");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawPolygon);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "drawSolidPoly");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawSolidPoly);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "drawPoly");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawPoly);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "drawCardinalSpline");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawCardinalSpline);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "drawCatmullRom");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawCatmullRom);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "drawPoints");
        lua_pushcfunction(luaState, toaxlua_DrawNode_drawPoints);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendRenderTexturePass(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.RenderTexturePass");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "setViewport");
        lua_pushcfunction(luaState, axlua_RenderTexturePass_setViewport);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

int axlua_Sprite_initWithPolygon(lua_State* luaState)
{
    int argc        = 0;
    ax::Sprite* obj = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Sprite", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Sprite*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Sprite_initWithPolygon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::PolygonInfo* arg0;

        ok &= luaval_to_object<ax::PolygonInfo>(luaState, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:initWithPolygon");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Sprite_initWithPolygon'",
                                        nullptr);
            return 0;
        }
        bool ret = obj->initWithPolygon(*arg0);
        lua_pushboolean(luaState, (bool)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Sprite:initWithPolygon", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Sprite_initWithPolygon'.", &conversionError);
#endif
    return 0;
}

int axlua_Sprite_setPolygonInfo(lua_State* luaState)
{
    int argc        = 0;
    ax::Sprite* obj = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Sprite", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Sprite*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Sprite_setPolygonInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::PolygonInfo* arg0;

        ok &= luaval_to_object<ax::PolygonInfo>(luaState, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:setPolygonInfo");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Sprite_setPolygonInfo'",
                                        nullptr);
            return 0;
        }
        obj->setPolygonInfo(*arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Sprite:setPolygonInfo", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Sprite_setPolygonInfo'.", &conversionError);
#endif
    return 0;
}

int axlua_Sprite_create(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Sprite", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    do
    {
        if (argc == 1 && lua_type(luaState, 2) == LUA_TSTRING)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(arg0);
            object_to_luaval<ax::Sprite>(luaState, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            ax::Sprite* ret = ax::Sprite::create();
            object_to_luaval<ax::Sprite>(luaState, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1 && lua_type(luaState, 2) != LUA_TSTRING)
        {
            ax::PolygonInfo* arg0;
            ok &= luaval_to_object<ax::PolygonInfo>(luaState, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(*arg0);
            object_to_luaval<ax::Sprite>(luaState, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Rect arg1;
            ok &= luaval_to_rect(luaState, 3, &arg1, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(arg0, arg1);
            object_to_luaval<ax::Sprite>(luaState, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d", "ax.Sprite:create", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Sprite_create'.", &conversionError);
#endif
    return 0;
}

static void extendSprite(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Sprite");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "initWithPolygon", axlua_Sprite_initWithPolygon);
        axlua::adapter::set_function(luaState, "setPolygonInfo", axlua_Sprite_setPolygonInfo);
        axlua::adapter::set_function(luaState, "create", axlua_Sprite_create);
    }
    lua_pop(luaState, 1);
}

static void extendLayerMultiplex(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.LayerMultiplex");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "create");
        lua_pushcfunction(luaState, toaxlua_LayerMultiplex_create);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendFileUtils(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.FileUtils");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "getStringFromFile");
        lua_pushcfunction(luaState, toaxlua_FileUtils_getStringFromFile);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "getDataFromFile");
        lua_pushcfunction(luaState, toaxlua_FileUtils_getDataFromFile);
        lua_rawset(luaState, -3);

        axlua::adapter::push_literal(luaState, "setSearchPaths");
        lua_pushcfunction(luaState, lua_ax_base_FileUtils_setSearchPaths);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendUserDefault(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.UserDefault");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "getInstance");
        lua_pushcfunction(luaState, toaxlua_UserDefault_getInstance);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

static void extendSpriteBatchNode(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.SpriteBatchNode");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "getDescendants");
        lua_pushcfunction(luaState, toaxlua_SpriteBatchNode_getDescendants);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

namespace ax
{
AccelerationEventListener* LuaAccelerationEventListener::create(axlua::Callback<void(AccelerationEvent*)> callback)
{
    AccelerationEventListener* listener = new AccelerationEventListener();
    if (listener->init([callback = std::move(callback)](AccelerationEvent* event) mutable { callback(event); }))
    {
        listener->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(listener);
    }
    return listener;
}

CustomEventListener* LuaCustomEventListener::create(std::string_view eventName,
                                                    axlua::Callback<void(CustomEvent*)> callback)
{
    CustomEventListener* eventCustom = new CustomEventListener();
    if (eventCustom->init(eventName, [callback = std::move(callback)](CustomEvent* event) mutable { callback(event); }))
    {
        eventCustom->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(eventCustom);
    }
    return eventCustom;
}
}  // namespace ax

static int toaxlua_LuaAccelerationEventListener_create(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.AccelerationEventListener", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        axlua::Callback<void(ax::AccelerationEvent*)> callback(luaState, 2);
        ax::AccelerationEventListener* returnValue = ax::LuaAccelerationEventListener::create(std::move(callback));
        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.AccelerationEventListener");

        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.AccelerationEventListener:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_LuaAccelerationEventListener_create'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_LuaCustomEventListener_create(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.CustomEventListener", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto eventName = axlua_tosv(luaState, 2);
        axlua::Callback<void(ax::CustomEvent*)> callback(luaState, 3);
        ax::CustomEventListener* returnValue = LuaCustomEventListener::create(eventName, std::move(callback));

        axlua::adapter::push_object(luaState, (void*)returnValue, "ax.CustomEventListener");

        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CustomEventListener:create",
               argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_LuaCustomEventListener_create'.",
                                &conversionError);
    return 0;
#endif
}

static void extendCustomEventListener(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.CustomEventListener");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "create", axlua_LuaCustomEventListener_create);
    }
    lua_pop(luaState, 1);
}

static void extendAccelerationEventListener(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.AccelerationEventListener");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "create", toaxlua_LuaAccelerationEventListener_create);
    }
    lua_pop(luaState, 1);
}

static int toaxlua_ActionCamera_reverse(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc              = 0;
    ax::ActionCamera* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ActionCamera", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::ActionCamera*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'toaxlua_ActionCamera_reverse'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        ax::ActionCamera* reverse = obj->reverse();
        axlua::adapter::push_object(luaState, (void*)(reverse), "ax.ActionCamera");
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ActionCamera:reverse", argc,
               0);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_ActionCamera_reverse'.", &conversionError);
#endif
    return 0;
}

static void extendActionCamera(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.ActionCamera");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "reverse", toaxlua_ActionCamera_reverse);
    }
    lua_pop(luaState, 1);
}

static int toaxlua_GridAction_reverse(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc            = 0;
    ax::GridAction* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.GridAction", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::GridAction*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'toaxlua_GridAction_reverse'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        ax::GridAction* reverse = obj->reverse();
        axlua::adapter::push_object(luaState, (void*)(reverse), "ax.GridAction");
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.GridAction:reverse", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'toaxlua_GridAction_reverse'.", &conversionError);
#endif
    return 0;
}

static void extendGridAction(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.GridAction");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "reverse", toaxlua_GridAction_reverse);
    }
    lua_pop(luaState, 1);
}

static int axlua_Label_createWithTTF00(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "ax.Label", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc >= 2 && argc <= 4)
    {

#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(L, 4, 1, &conversionError) ||
            !axlua::adapter::is_number(L, 5, 1, &conversionError))
        {
            goto argumentError;
        }
#endif
        TTFConfig ttfConfig("");
        std::string_view text = ""sv;

        ok &= luaval_to_ttfconfig(L, 2, &ttfConfig, "ax.Label:createWithTTF");
        if (!ok)
            return 0;

        ok &= luaval_to_std_string_view(L, 3, &text, "ax.Label:createWithTTF");
        if (!ok)
            return 0;

        int alignment  = (int)axlua::adapter::to_number(L, 4, 1);
        int lineSize   = (int)axlua::adapter::to_number(L, 5, 0);
        ax::Label* ret = ax::Label::createWithTTF(ttfConfig, text, static_cast<TextHAlignment>(alignment), lineSize);
        axlua::adapter::push_object(L, (void*)ret, "ax.Label");
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Label:createWithTTF", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_Label_createWithTTF'.", &conversionError);
#endif
    return 0;
}

static int axlua_Label_createWithTTF01(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "ax.Label", 0, &conversionError))
        goto argumentError;

    argc = lua_gettop(L) - 1;

    if (argc >= 3 && argc <= 6)
    {
        if (!axlua::adapter::is_string(L, 2, 0, &conversionError) ||
            !axlua::adapter::is_string(L, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(L, 4, 0, &conversionError) ||
            !axlua::adapter::is_table(L, 5, 1, &conversionError) ||
            !axlua::adapter::is_number(L, 6, 1, &conversionError) ||
            !axlua::adapter::is_number(L, 7, 1, &conversionError))
        {
            goto argumentError;
        }
        else
        {
            std::string_view text     = axlua_tosv(L, 2);
            std::string_view fontFile = axlua_tosv(L, 3);
            float fontSize            = (float)axlua::adapter::to_number(L, 4, 0);
            ax::Size dimensions       = ax::Size::zero;
            if (lua_istable(L, 5))
            {
                luaval_to_size(L, 5, &dimensions, "ax.Label:createWithTTF");
            }
            TextHAlignment hAlignment = static_cast<TextHAlignment>((int)axlua::adapter::to_number(L, 6, 0));
            TextVAlignment vAlignment = static_cast<TextVAlignment>((int)axlua::adapter::to_number(L, 7, 0));

            ax::Label* ret = ax::Label::createWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment);

            axlua::adapter::push_object(L, (void*)ret, "ax.Label");
            return 1;
        }
    }

argumentError:
    return axlua_Label_createWithTTF00(L);
}

static void extendLabel(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Label");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "createWithTTF", axlua_Label_createWithTTF01);
    }
    lua_pop(luaState, 1);
}

static int axlua_TMXTiledMap_getPropertiesForGID(lua_State* luaState)
{
    int argc             = 0;
    ax::TMXTiledMap* obj = NULL;
    bool ok              = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.TMXTiledMap", 0, &conversionError))
        goto argumentError;
#endif
    obj = (ax::TMXTiledMap*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_TMXTiledMap_getPropertiesForGID'",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "ax.TMXTiledMap:getPropertiesForGID");

        if (!ok)
            return 0;
        ax::Value ret = obj->getPropertiesForGID(arg0);
        value_to_luaval(luaState, ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.TMXTiledMap:getPropertiesForGID", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_TMXTiledMap_getPropertiesForGID'.",
                                &conversionError);
#endif

    return 0;
}

static void extendTMXTiledMap(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.TMXTiledMap");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getPropertiesForGID", axlua_TMXTiledMap_getPropertiesForGID);
    }
    lua_pop(luaState, 1);
}

static int axlua_OrbitCamera_sphericalRadius(lua_State* luaState)
{
    int argc              = 0;
    ax::OrbitCamera* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.OrbitCamera", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::OrbitCamera*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_OrbitCamera_sphericalRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 3)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 4, 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        float newRadius = (float)axlua::adapter::to_number(luaState, 2, 0);
        float zenith    = (float)axlua::adapter::to_number(luaState, 3, 0);
        float azimuth   = (float)axlua::adapter::to_number(luaState, 4, 0);

        self->sphericalRadius(&newRadius, &zenith, &azimuth);

        lua_pushnumber(luaState, (lua_Number)newRadius);
        lua_pushnumber(luaState, (lua_Number)zenith);
        lua_pushnumber(luaState, (lua_Number)azimuth);

        return 3;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OrbitCamera:sphericalRadius",
               argc, 3);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_OrbitCamera_sphericalRadius'.", &conversionError);
#endif

    return 0;
}

static void extendOrbitCamera(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.OrbitCamera");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "sphericalRadius", axlua_OrbitCamera_sphericalRadius);
    }
    lua_pop(luaState, 1);
}

int axlua_TMXLayer_getTileGIDAt(lua_State* luaState)
{
    int argc          = 0;
    ax::TMXLayer* obj = nullptr;
    bool ok           = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.TMXLayer", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::TMXLayer*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_TMXLayer_getTileGIDAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(luaState, 2, &arg0, "ax.TMXLayer:getTileGIDAt");
        if (!ok)
            return 0;
        unsigned int ret = obj->getTileGIDAt(arg0);
        lua_pushnumber(luaState, (lua_Number)ret);
        lua_pushnumber(luaState, (lua_Number)0);
        return 2;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        int arg1;

        ok &= luaval_to_vec2(luaState, 2, &arg0, "ax.TMXLayer:getTileGIDAt");
        ok &= luaval_to_int(luaState, 3, &arg1, "ax.TMXLayer:getTileGIDAt");

        if (!ok)
            return 0;

        unsigned int ret = obj->getTileGIDAt(arg0, (ax::TMXTileFlags*)&arg1);
        lua_pushnumber(luaState, (lua_Number)ret);
        lua_pushnumber(luaState, (lua_Number)arg1);
        return 2;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TMXLayer:getTileGIDAt", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_TMXLayer_getTileGIDAt'.", &conversionError);
#endif

    return 0;
}

int axlua_TMXLayer_setTiles(lua_State* luaState)
{
    int argc          = 0;
    ax::TMXLayer* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.TMXLayer", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::TMXLayer*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_TMXLayer_setTiles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        uint32_t* arg0 = nullptr;
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 2, 0, &conversionError))
            goto argumentError;
#endif

        size_t len = lua_objlen(luaState, 2);
        if (len == 0)
        {
            luaL_error(luaState, "Table's len equal 0");
            return 0;
        }
        arg0 = new uint32_t[len];
        for (size_t i = 1; i <= len; i++)
        {
            arg0[i - 1] = (uint32_t)axlua::adapter::to_field_number(luaState, 2, i, 0);
        }

        obj->setTiles(arg0);

        AX_SAFE_DELETE_ARRAY(arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TMXLayer:setTiles", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_TMXLayer_setTiles'.", &conversionError);
#endif

    return 0;
}

static void extendTMXLayer(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.TMXLayer");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getTileGIDAt", axlua_TMXLayer_getTileGIDAt);
        axlua::adapter::set_function(luaState, "setTiles", axlua_TMXLayer_setTiles);
    }
    lua_pop(luaState, 1);
}

static int axlua_FastTMXLayer_getTileGIDAt(lua_State* luaState)
{
    int argc              = 0;
    ax::FastTMXLayer* obj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.FastTMXLayer", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::FastTMXLayer*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_FastTMXLayer_getTileGIDAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(luaState, 2, &arg0, "ax.FastTMXLayer:getTileGIDAt");
        if (!ok)
            return 0;
        int ret = obj->getTileGIDAt(arg0);
        lua_pushnumber(luaState, (lua_Number)ret);
        lua_pushnumber(luaState, (lua_Number)0);
        return 2;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        int arg1 = 0;

        ok &= luaval_to_vec2(luaState, 2, &arg0, "ax.FastTMXLayer:getTileGIDAt");
        ok &= luaval_to_int(luaState, 3, &arg1, "ax.FastTMXLayer:getTileGIDAt");

        if (!ok)
            return 0;

        unsigned int ret = obj->getTileGIDAt(arg0, (ax::TMXTileFlags*)&arg1);
        lua_pushnumber(luaState, (lua_Number)ret);
        lua_pushnumber(luaState, (lua_Number)arg1);
        return 2;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FastTMXLayer:getTileGIDAt",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_FastTMXLayer_getTileGIDAt'.", &conversionError);
#endif

    return 0;
}

static void extendFastTMXLayer(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.FastTMXLayer");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getTileGIDAt", axlua_FastTMXLayer_getTileGIDAt);
    }
    lua_pop(luaState, 1);
}

static int axlua_Application_getInstance(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Application", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 0)
    {
        auto* returnValue = ax::Application::getInstance();
        axlua::adapter::push_usertype(luaState, static_cast<void*>(returnValue), "ax.Application");
        return 1;
    }

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Application:getInstance",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Application_getInstance'.", &conversionError);
    return 0;
#endif
}

int axlua_Application_isIOS64bit(lua_State* luaState)
{
    int argc             = 0;
    ax::Application* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Application", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Application*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Application_isIOS64bit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        bool isIOS64bit                = false;
        Application::Platform platform = ax::Application::getInstance()->getTargetPlatform();
        if (Application::Platform::iOS == platform)
        {
#if defined(__LP64__)
            isIOS64bit = true;
#endif
        }

        lua_pushboolean(luaState, isIOS64bit);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Application:isIOS64bit", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Application_isIOS64bit'.", &conversionError);
#endif

    return 0;
}

int axlua_Application_is64BitMobileDevice(lua_State* luaState)
{
    int argc             = 0;
    ax::Application* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Application", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Application*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Application_is64BitMobileDevice'",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        bool is64BitMobileDevice       = false;
        Application::Platform platform = ax::Application::getInstance()->getTargetPlatform();
        if (Application::Platform::iOS == platform || Application::Platform::Android == platform)
        {
#if defined(__arm64__) || defined(__aarch64__)
            is64BitMobileDevice = true;
#endif
        }

        lua_pushboolean(luaState, is64BitMobileDevice);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.Application:is64BitMobileDevice", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Application_is64BitMobileDevice'.",
                                &conversionError);
#endif

    return 0;
}

static void extendApplication(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Application");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getInstance", axlua_Application_getInstance);
        axlua::adapter::set_function(luaState, "isIOS64bit", axlua_Application_isIOS64bit);
        axlua::adapter::set_function(luaState, "is64BitMobileDevice", axlua_Application_is64BitMobileDevice);
    }
    lua_pop(luaState, 1);
}

static int axlua_TextureCache_addImageAsync(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc           = 0;
    TextureCache* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.TextureCache", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<TextureCache*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_TextureCache_addImageAsync'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto configFilePath = axlua_tosv(luaState, 2);
        axlua::Callback<void(Texture2D*)> callback(luaState, 3);
        self->addImageAsync(configFilePath,
                            [callback = std::move(callback)](Texture2D* tex) mutable { callback(tex); });

        return 0;
    }

    luaL_error(luaState, "%s function of TextureCache has wrong number of arguments: %d, was expecting %d\n",
               "ax.TextureCache:addImageAsync", argc, 1);

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_TextureCache_addImageAsync'.", &conversionError);
#endif
    return 0;
}

static void extendTextureCache(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.TextureCache");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "addImageAsync", axlua_TextureCache_addImageAsync);
    }
    lua_pop(luaState, 1);
}

#if 0
int axlua_RenderView_getAllTouches(lua_State* luaState)
{
    int argc            = 0;
    ax::RenderView* obj = nullptr;
    bool ok             = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.RenderView", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::RenderView*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_RenderView_getAllTouches'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;

        std::vector<ax::Touch*> ret = obj->getAllTouches();
        lua_newtable(luaState);
        if (ret.empty())
            return 1;

        int index = 1;
        for (const auto& obj : ret)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(luaState, (lua_Number)index);
            axlua::adapter::push_object(luaState, (void*)obj, "ax.Touch");
            lua_rawset(luaState, -3);
            ++index;
        }

        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.RenderView:getAllTouches",
               argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_RenderView_getAllTouches'.", &conversionError);
#    endif

    return 0;
}

static void extendRenderView(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.RenderView");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getAllTouches", axlua_RenderView_getAllTouches);
    }
    lua_pop(luaState, 1);
}
#endif

int axlua_Camera_unproject(lua_State* luaState)
{
    int argc        = 0;
    ax::Camera* obj = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Camera", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::Camera*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_Camera_unproject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(luaState, 2, &arg0, "ax.Camera:unproject");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_Camera_unproject'", nullptr);
            return 0;
        }
        auto ret = obj->deprojectScreenToWorld(arg0);
        vec3_to_luaval(luaState, ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Camera:unproject", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Camera_unproject'.", &conversionError);
#endif

    return 0;
}

static void extendCamera(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Camera");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "unproject", axlua_Camera_unproject);
    }
    lua_pop(luaState, 1);
}

int axlua_Properties_createNonRefCounted(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.Properties", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.Properties:createNonRefCounted");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'axlua_Properties_createNonRefCounted'", nullptr);
            return 0;
        }
        ax::Properties* ret = ax::Properties::createNonRefCounted(arg0);
        object_to_luaval<ax::Properties>(luaState, "ax.Properties", (ax::Properties*)ret);
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ",
               "ax.Properties:createNonRefCounted", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Properties_createNonRefCounted'.",
                                &conversionError);
#endif
    return 0;
}

static int lua_collect_Properties(lua_State* luaState)
{
    ax::Properties* self = (ax::Properties*)axlua::adapter::to_usertype(luaState, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

static void extendProperties(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.Properties");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "createNonRefCounted", axlua_Properties_createNonRefCounted);
    }
    lua_pop(luaState, 1);

    luaL_getmetatable(luaState, "ax.Properties");
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, ".collector", lua_collect_Properties);
    }
    lua_pop(luaState, 1);
}

int axlua_get_PolygonInfo_rect(lua_State* luaState)
{
    ax::PolygonInfo* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PolygonInfo", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::PolygonInfo*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_get_PolygonInfo_rect'\n", nullptr);
        return 0;
    }
#endif
    rect_to_luaval(luaState, obj->getRect());
    return 1;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_get_PolygonInfo_rect'.", &conversionError);
    return 0;
#endif
}

int axlua_set_PolygonInfo_rect(lua_State* luaState)
{
    int argc              = 0;
    ax::PolygonInfo* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PolygonInfo", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::PolygonInfo*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_set_PolygonInfo_rect'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 2, 0, &conversionError))
            goto argumentError;
#endif
        Rect outRect;
        luaval_to_rect(luaState, 2, &outRect);
        self->setRect(outRect);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_set_PolygonInfo_rect'.", &conversionError);
    return 0;
#endif
}

int axlua_get_PolygonInfo_filename(lua_State* luaState)
{
    ax::PolygonInfo* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PolygonInfo", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::PolygonInfo*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_get_PolygonInfo_filename'\n", nullptr);
        return 0;
    }
#endif
    axlua::adapter::push_string_view(luaState, obj->getFilename());
    return 1;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_get_PolygonInfo_filename'.", &conversionError);
    return 0;
#endif
}

int axlua_set_PolygonInfo_filename(lua_State* luaState)
{
    int argc              = 0;
    ax::PolygonInfo* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PolygonInfo", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::PolygonInfo*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_set_PolygonInfo_filename'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
        std::string_view outFilename;
        luaval_to_std_string_view(luaState, 2, &outFilename);
        self->setFilename(outFilename);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_set_PolygonInfo_filename'.", &conversionError);
    return 0;
#endif
}

static int lua_collect_PolygonInfo(lua_State* luaState)
{
    ax::PolygonInfo* self = (ax::PolygonInfo*)axlua::adapter::to_usertype(luaState, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

int axlua_get_PipelineDescriptor_programState(lua_State* luaState)
{
    ax::PipelineDesc* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PipelineDesc", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::PipelineDesc*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == obj)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'self' in function 'axlua_get_PipelineDescriptor_programState'\n", nullptr);
        return 0;
    }
#endif
    axlua::adapter::push_usertype(luaState, obj->programState, "axr.ProgramState");
    return 1;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_get_PipelineDescriptor_programState'.",
                                &conversionError);
    return 0;
#endif
}

int axlua_set_PipelineDescriptor_programState(lua_State* luaState)
{
    int argc               = 0;
    ax::PipelineDesc* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.PipelineDesc", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::PipelineDesc*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'self' in function 'axlua_set_PipelineDescriptor_programState'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
        ax::rhi::ProgramState* programState = nullptr;
        luaval_to_object<ax::rhi::ProgramState>(luaState, 2, "axr.ProgramState", &programState,
                                                "axlua_set_PipelineDescriptor_programState");
        self->programState = programState;
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_set_PipelineDescriptor_programState'.",
                                &conversionError);
    return 0;
#endif
}

static void extendPolygonInfo(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.PolygonInfo");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_variable(luaState, "rect", axlua_get_PolygonInfo_rect, axlua_set_PolygonInfo_rect);
        axlua::adapter::set_variable(luaState, "filename", axlua_get_PolygonInfo_filename,
                                     axlua_set_PolygonInfo_filename);
    }
    lua_pop(luaState, 1);

    luaL_getmetatable(luaState, "ax.PolygonInfo");
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, ".collector", lua_collect_PolygonInfo);
    }
    lua_pop(luaState, 1);
}

static void extendPipelineDescriptor(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.PipelineDesc");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_variable(luaState, "programState", axlua_get_PipelineDescriptor_programState,
                                     axlua_set_PipelineDescriptor_programState);
    }
    lua_pop(luaState, 1);
}

static int axlua_rhi_ProgramState_getUniformLocation(lua_State* luaState)
{
    int argc                   = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok                    = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "axr.ProgramState", 0, &conversionError))
        goto argumentError;
#endif
    obj = (ax::rhi::ProgramState*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_rhi_ProgramState_getUniformLocation'",
                                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;
    do
    {
        if (argc == 1)
        {
            if (lua_isstring(luaState, -1))
            {
                std::string_view arg0;
                ok &= luaval_to_std_string_view(luaState, 2, &arg0, "axr.ProgramState:getUniformLocation");

                if (!ok)
                {
                    break;
                }
                ax::rhi::UniformLocation ret = obj->getUniformLocation(arg0);
                uniformLocation_to_luaval(luaState, ret);
                return 1;
            }
            else if (lua_isnumber(luaState, -1))
            {
                ax::rhi::Uniform arg0;
                ok &= luaval_to_int(luaState, 2, (int*)&arg0, "axr.ProgramState:getUniformLocation");

                if (!ok)
                {
                    break;
                }
                ax::rhi::UniformLocation ret = obj->getUniformLocation(arg0);
                uniformLocation_to_luaval(luaState, ret);
                return 1;
            }
        }
    } while (0);

    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "axr.ProgramState:getUniformLocation", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_rhi_ProgramState_getUniformLocation'.",
                                &conversionError);
#endif

    return 0;
}

static int axlua_ProgramState_setUniform(lua_State* luaState)
{
    bool ok                     = true;
    int argc                    = 0;
    ax::rhi::ProgramState* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "axr.ProgramState", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::rhi::ProgramState*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_ProgramState_setUniform'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (2 == argc)
    {
        ax::rhi::UniformLocation location;

        if (lua_isstring(luaState, 2))
        {
            location = self->getUniformLocation(lua_tostring(luaState, 2));
        }
        else if (lua_istable(luaState, 2))
        {
            ok &= luaval_to_uniformLocation(luaState, 2, location, "axr.ProgramState:setUniform");
        }

        if (ok & lua_istable(luaState, 3))
        {
            int len = lua_objlen(luaState, 3);
            std::vector<uint8_t> buffer(len);

            for (int i = 0; i < len; i++)
            {
                lua_rawgeti(luaState, 3, i + 1);
                buffer[i] = lua_tointeger(luaState, -1);
                lua_pop(luaState, 1);
            }

            self->setUniform(location, buffer.data(), buffer.size());
        }
        else
        {
            luaL_error(luaState, "`axlua_ProgramState_setUniform` argument invalidate");
        }
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_set_PipelineDescriptor_vertexLayout'.",
                                &conversionError);
    return 0;
#endif
}

static void extendProgramState(lua_State* luaState)
{
    // lua_pushstring(luaState, "axr.ProgramState");
    // lua_rawget(luaState, LUA_REGISTRYINDEX);

    axlua::adapter::begin_module(luaState, "axr");
    axlua::adapter::begin_module(luaState, "ProgramState");
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "setUniform", axlua_ProgramState_setUniform);
        axlua::adapter::set_function(luaState, "getUniformLocation", axlua_rhi_ProgramState_getUniformLocation);
    }
    axlua::adapter::end_module(luaState);
    axlua::adapter::end_module(luaState);
    // lua_pop(luaState, 1);
}

int axlua_AutoPolygon_generatePolygon(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "ax.AutoPolygon", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'",
                                        nullptr);
            return 0;
        }
        ax::PolygonInfo* ret = new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0));
        object_to_luaval<ax::PolygonInfo>(luaState, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
    if (argc == 2)
    {
        std::string_view arg0;
        ax::Rect arg1;
        ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(luaState, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'",
                                        nullptr);
            return 0;
        }
        ax::PolygonInfo* ret = new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1));
        object_to_luaval<ax::PolygonInfo>(luaState, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        ax::Rect arg1;
        double arg2;
        ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(luaState, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(luaState, 4, &arg2, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'",
                                        nullptr);
            return 0;
        }
        ax::PolygonInfo* ret = new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1, (float)arg2));
        object_to_luaval<ax::PolygonInfo>(luaState, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
    if (argc == 4)
    {
        std::string_view arg0;
        ax::Rect arg1;
        double arg2;
        double arg3;
        ok &= luaval_to_std_string_view(luaState, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(luaState, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(luaState, 4, &arg2, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(luaState, 5, &arg3, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'",
                                        nullptr);
            return 0;
        }
        ax::PolygonInfo* ret =
            new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1, (float)arg2, (float)arg3));
        object_to_luaval<ax::PolygonInfo>(luaState, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AutoPolygon:generatePolygon",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_AutoPolygon_generatePolygon'.", &conversionError);
#endif
    return 0;
}

static int lua_collect_AutoPolygon(lua_State* luaState)
{
    ax::AutoPolygon* self = (ax::AutoPolygon*)axlua::adapter::to_usertype(luaState, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

static void extendAutoPolygon(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.AutoPolygon");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "generatePolygon", axlua_AutoPolygon_generatePolygon);
    }
    lua_pop(luaState, 1);

    luaL_getmetatable(luaState, "ax.AutoPolygon");
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, ".collector", lua_collect_AutoPolygon);
    }
    lua_pop(luaState, 1);
}

// extern int axlua_Director_setStatsDisplay(lua_State* luaState);
// static void extendDirector(lua_State* luaState)
//{
//     lua_pushstring(luaState, "ax.Director");
//     lua_rawget(luaState, LUA_REGISTRYINDEX);
//     if (lua_istable(luaState, -1))
//     {
//         lua_pushstring(luaState, "setDisplayStats");
//         lua_pushcfunction(luaState, axlua_Director_setStatsDisplay);
//         lua_rawset(luaState, -3);
//     }
//     lua_pop(luaState, 1);
// }

int register_all_ax_adapter(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    extendRenderTexturePass(luaState);
    extendComponentLua(luaState);
    extendNode(luaState);
    extendScene(luaState);
    extendMenuItem(luaState);
    extendMenuItemImage(luaState);
    extendMenuItemLabel(luaState);
    extendMenuItemFont(luaState);
    extendMenuItemSprite(luaState);
    extendMenuItemToggle(luaState);
    extendMenu(luaState);
    extendScheduler(luaState);
    extendSequence(luaState);
    extendCallFunc(luaState);
    extendSpawn(luaState);
    extendCardinalSplineBy(luaState);
    extendCatmullRomBy(luaState);
    extendCatmullRomTo(luaState);
    extendBezierBy(luaState);
    extendBezierTo(luaState);
    extendDrawNode(luaState);
    extendSprite(luaState);
    extendLayerMultiplex(luaState);
    extendFileUtils(luaState);
    extendUserDefault(luaState);
    extendTexture2D(luaState);
    extendSpriteBatchNode(luaState);
    extendCustomEventListener(luaState);
    extendAccelerationEventListener(luaState);
    extendActionCamera(luaState);
    extendGridAction(luaState);
    extendPipelineDescriptor(luaState);
    extendProgramState(luaState);

    extendLabel(luaState);
    extendTMXTiledMap(luaState);
    extendOrbitCamera(luaState);
    extendTMXLayer(luaState);
    extendFastTMXLayer(luaState);
    extendApplication(luaState);
    extendTextureCache(luaState);
    extendCamera(luaState);
    extendProperties(luaState);
    extendAutoPolygon(luaState);
    extendPolygonInfo(luaState);
    extendRenderTexture(luaState);
    return 0;
}

static int axlua_utils_captureNode(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_usertype(luaState, 2, "ax.Node", 0, &conversionError) ||
        !axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Node* node = static_cast<Node*>(axlua::adapter::to_usertype(luaState, 2, nullptr));
        axlua::Callback<void(ax::Image*)> callback(luaState, 3);

        float scale = 1.0f;
        axlua::adapter::Error conversionError;
        if (axlua::adapter::is_number(luaState, 4, 0, &conversionError))
        {
            scale = axlua::adapter::to_number(luaState, 4, 1.0);
        }

        ax::utils::captureNode(
            node, [callback = std::move(callback)](RefPtr<Image> image) mutable { callback(image.get()); }, scale);

        return 0;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_captureScreen'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_captureScreen(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 3, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        axlua::Callback<void(bool, std::string_view)> callback(luaState, 2);
        auto fileName = axlua_tosv(luaState, 3);
        ax::utils::captureScreen([callback = std::move(callback)](bool succeed, std::string_view name) mutable {
            callback(succeed, name);
        }, fileName);

        return 0;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_captureScreen'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_findChildren(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_usertype(luaState, 2, "ax.Node", 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 3, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Node* node              = static_cast<Node*>(axlua::adapter::to_usertype(luaState, 2, nullptr));
        auto name                   = axlua_tosv(luaState, 3);
        std::vector<Node*> children = ax::utils::findChildren(*node, name);
        lua_newtable(luaState);
        int index = 1;
        for (const auto& obj : children)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(luaState, (lua_Number)index);
            axlua::adapter::push_object(luaState, (void*)obj, "ax.Node");
            lua_rawset(luaState, -3);
            ++index;
        }
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_findChildren'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_findChild(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Node", 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Node* node = static_cast<Node*>(axlua::adapter::to_usertype(luaState, 1, nullptr));
        auto name      = axlua_tosv(luaState, 2);
        auto obj       = ax::utils::findChild(node, name);
        axlua::adapter::push_object(luaState, (void*)obj, "ax.Node");
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_findChild'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_gettime(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        double time = ax::utils::gettime();
        lua_pushnumber(luaState, time);
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_gettime'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_getTimeInMilliseconds(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        long long time = ax::utils::getTimeInMilliseconds();
        lua_pushnumber(luaState, time);
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_getTimeInMilliseconds'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_getFileMD5Hash(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
        !axlua::adapter::is_number(luaState, 3, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        std::string_view filename = axlua_tosv(luaState, 2);
        uint32_t bufferSize       = axlua::adapter::to_number(luaState, 3, 0);
        std::string hexOutput     = ax::utils::getFileMD5Hash(filename, bufferSize);
        lua_pushlstring(luaState, hexOutput.c_str(), hexOutput.size());
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_getFileMD5Hash'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_getStringMD5Hash(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        std::string_view str  = axlua_tosv(luaState, 2);
        std::string hexOutput = ax::utils::getStringMD5Hash(str);

        lua_pushlstring(luaState, hexOutput.c_str(), hexOutput.size());
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_getStringMD5Hash'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_base64Encode(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        std::string_view str = axlua_tosv(luaState, 2);
        auto base64_str      = ax::utils::base64Encode(std::span{str});

        lua_pushlstring(luaState, base64_str.c_str(), base64_str.size());
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_base64Encode'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_base64Decode(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        std::string_view base64String = axlua_tosv(luaState, 2);

        auto plainText = utils::base64Decode(base64String);
        lua_pushlstring(luaState, reinterpret_cast<const char*>(plainText.data()), plainText.size());
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_base64Decode'.", &conversionError);
    return 0;
#endif
}

static int axlua_utils_getCascadeBoundingBox(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_usertype(luaState, 2, "ax.Node", 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Node* node = static_cast<Node*>(axlua::adapter::to_usertype(luaState, 2, nullptr));
        Rect box       = ax::utils::getCascadeBoundingBox(node);
        rect_to_luaval(luaState, box);
        return 1;
    }
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_utils_getCascadeBoundingBox'.", &conversionError);
    return 0;
#endif
}

int register_all_ax_module_adapter(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    axlua::adapter::open(luaState);
    axlua::adapter::module(luaState, "ax", 0);
    axlua::adapter::begin_module(luaState, "ax");
    axlua::adapter::module(luaState, "utils", 0);
    axlua::adapter::begin_module(luaState, "utils");
    axlua::adapter::set_function(luaState, "captureNode", axlua_utils_captureNode);
    axlua::adapter::set_function(luaState, "captureScreen", axlua_utils_captureScreen);
    axlua::adapter::set_function(luaState, "findChildren", axlua_utils_findChildren);
    axlua::adapter::set_function(luaState, "findChild", axlua_utils_findChild);
    axlua::adapter::set_function(luaState, "gettime", axlua_utils_gettime);
    axlua::adapter::set_function(luaState, "getTimeInMilliseconds", axlua_utils_getTimeInMilliseconds);
    axlua::adapter::set_function(luaState, "getStringMD5Hash", axlua_utils_getStringMD5Hash);
    axlua::adapter::set_function(luaState, "getFileMD5Hash", axlua_utils_getFileMD5Hash);
    axlua::adapter::set_function(luaState, "base64Encode", axlua_utils_base64Encode);
    axlua::adapter::set_function(luaState, "base64Decode", axlua_utils_base64Decode);
    axlua::adapter::set_function(luaState, "getCascadeBoundingBox", axlua_utils_getCascadeBoundingBox);
    axlua::adapter::end_module(luaState);
    axlua::adapter::end_module(luaState);

    return 0;
}

static int axlua_Mat4_getInversed(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Mat4 mat;
        bool ok = luaval_to_mat4(luaState, 1, &mat);
        if (ok)
        {
            mat4_to_luaval(luaState, mat.getInversed());
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'mat4_getInversed'.", &conversionError);
    return 0;
#endif
}

static int axlua_Mat4_transformVector(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 2 || argc == 3) /* Allow 3 arguments for compatibility with old version */
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 2, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 mat;
            ax::Vec4 vector;
            ax::Vec4 dst;
            ok &= luaval_to_mat4(luaState, 1, &mat);
            if (!ok)
                return 0;

            ok &= luaval_to_vec4(luaState, 2, &vector);
            if (!ok)
                return 0;

            mat.transformVector(vector, &dst);
            vec4_to_luaval(luaState, dst);
            return 1;
        }
    }
    else if (argc == 5 || argc == 6) /* Allow 6 arguments for compatibility with old version */
    {
        /*
         float x, float y, float z, float w, Vec3* dst
         */
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 4, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 5, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 mat;
            float x, y, z, w;
            ax::Vec3 dst;
            ok &= luaval_to_mat4(luaState, 1, &mat);
            if (!ok)
                return 0;

            x = axlua::adapter::to_number(luaState, 2, 0);
            y = axlua::adapter::to_number(luaState, 3, 0);
            z = axlua::adapter::to_number(luaState, 4, 0);
            w = axlua::adapter::to_number(luaState, 5, 0);

            mat.transformVector(x, y, z, w, &dst);
            vec3_to_luaval(luaState, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'mat4_transformVector'.", &conversionError);
    return 0;
#endif
}

static int axlua_Mat4_decompose(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;

    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        (!lua_isnil(luaState, 2) && !axlua::adapter::is_table(luaState, 2, 0, &conversionError)) ||
        (!lua_isnil(luaState, 3) && !axlua::adapter::is_table(luaState, 3, 0, &conversionError)) ||
        (!lua_isnil(luaState, 4) && !axlua::adapter::is_table(luaState, 4, 0, &conversionError)))
        goto argumentError;
    else
#endif
    {
        ax::Mat4 mat;
        ax::Vec3 scale;
        ax::Quat rotation;
        ax::Vec3 translation;
        bool ok = true;

        ok &= luaval_to_mat4(luaState, 1, &mat);
        if (!ok)
            return 0;

        if (lua_isnil(luaState, 2) && !lua_isnil(luaState, 3) && !lua_isnil(luaState, 4))
        {
            ok &= luaval_to_quaternion(luaState, 3, &rotation);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, &rotation, &translation);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            quaternion_to_luaval(luaState, rotation);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            vec3_to_luaval(luaState, translation);
            lua_rawset(luaState, -3);

            return 1;
        }

        if (lua_isnil(luaState, 2) && lua_isnil(luaState, 3) && !lua_isnil(luaState, 4))
        {
            ok &= luaval_to_vec3(luaState, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, nullptr, &translation);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            vec3_to_luaval(luaState, translation);
            lua_rawset(luaState, -3);

            return 1;
        }

        if (!lua_isnil(luaState, 2) && lua_isnil(luaState, 3) && !lua_isnil(luaState, 4))
        {
            ok &= luaval_to_vec3(luaState, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(&scale, nullptr, &translation);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            vec3_to_luaval(luaState, scale);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            vec3_to_luaval(luaState, translation);
            lua_rawset(luaState, -3);

            return 1;
        }

        if (!lua_isnil(luaState, 2) && lua_isnil(luaState, 3) && lua_isnil(luaState, 4))
        {
            ok &= luaval_to_vec3(luaState, 2, &scale);
            if (!ok)
                return 0;

            mat.decompose(&scale, nullptr, nullptr);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            vec3_to_luaval(luaState, scale);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            return 1;
        }

        if (!lua_isnil(luaState, 2) && !lua_isnil(luaState, 3) && lua_isnil(luaState, 4))
        {
            ok &= luaval_to_vec3(luaState, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_quaternion(luaState, 3, &rotation);
            if (!ok)
                return 0;

            mat.decompose(&scale, &rotation, nullptr);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            vec3_to_luaval(luaState, scale);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            quaternion_to_luaval(luaState, rotation);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            return 1;
        }

        if (lua_isnil(luaState, 2) && !lua_isnil(luaState, 3) && lua_isnil(luaState, 4))
        {
            ok &= luaval_to_quaternion(luaState, 3, &rotation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, &rotation, nullptr);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            quaternion_to_luaval(luaState, rotation);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            lua_pushnil(luaState);
            lua_rawset(luaState, -3);
        }

        if (!lua_isnil(luaState, 2) && !lua_isnil(luaState, 3) && !lua_isnil(luaState, 4))
        {
            ok &= luaval_to_vec3(luaState, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_quaternion(luaState, 3, &rotation);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(&scale, &rotation, &translation);

            lua_newtable(luaState);

            axlua::adapter::push_literal(luaState, "scale");
            vec3_to_luaval(luaState, scale);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "rotation");
            quaternion_to_luaval(luaState, rotation);
            lua_rawset(luaState, -3);

            axlua::adapter::push_literal(luaState, "translation");
            vec3_to_luaval(luaState, translation);
            lua_rawset(luaState, -3);

            return 1;
        }

        return 0;
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'mat4_decompose'.", &conversionError);
    return 0;
#endif
}

static int axlua_Vec3_cross(lua_State* luaState)
{
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 2, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec3 obj;
            ax::Vec3 v;

            bool ok = true;

            ok &= luaval_to_vec3(luaState, 1, &obj);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 2, &v);
            if (!ok)
                return 0;

            obj.cross(v);

            vec3_to_luaval(luaState, obj);
            return 1;
        }
    }
    else if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 3, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec3 v1;
            ax::Vec3 v2;
            ax::Vec3 dst;
            bool ok = true;

            ok &= luaval_to_vec3(luaState, 1, &v1);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 2, &v2);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 3, &dst);
            if (!ok)
                return 0;

            ax::Vec3::cross(v1, v2, &dst);

            vec3_to_luaval(luaState, dst);
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'MathUtil_gcrossVec3'.", &conversionError);
    return 0;
#endif
}

static int axlua_Mat4_multiply(lua_State* luaState)
{
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
        !axlua::adapter::is_table(luaState, 2, 0, &conversionError))
        goto argumentError;
    else
#endif
    {
        ax::Mat4 mat1;
        bool ok = luaval_to_mat4(luaState, 1, &mat1);
        if (!ok)
            return 0;

        ax::Mat4 mat2;
        ok = luaval_to_mat4(luaState, 2, &mat2);
        if (!ok)
            return 0;

        ax::Mat4 ret = mat1 * mat2;
        mat4_to_luaval(luaState, ret);
        return 1;
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'mat4_multiply'.", &conversionError);
    return 0;
#endif
}

int axlua_Mat4_translate(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_table(luaState, 2, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 mat;
            ax::Vec3 vec3;
            ok &= luaval_to_mat4(luaState, 1, &mat);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(luaState, 2, &vec3);
            if (!ok)
                return 0;

            mat.translate(vec3);
            mat4_to_luaval(luaState, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Mat4_translate'.", &conversionError);
    return 0;
#endif
}

int axlua_Mat4_createRotationZ(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 2)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 2, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 mat;
            float angle;
            ok &= luaval_to_mat4(luaState, 1, &mat);
            if (!ok)
                return 0;
            angle = (float)lua_tonumber(luaState, 2);
            ax::Mat4::createRotationZ(angle, &mat);
            mat4_to_luaval(luaState, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Mat4_createRotationZ'.", &conversionError);
    return 0;
#endif
}

int axlua_Mat4_setIdentity(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 mat;
            ok &= luaval_to_mat4(luaState, 1, &mat);
            if (!ok)
                return 0;
            mat.setIdentity();
            mat4_to_luaval(luaState, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Mat4_setIdentity'.", &conversionError);
    return 0;
#endif
}

int axlua_Mat4_createTranslation(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 3)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_number(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 dst;
            float xTranslation = (float)lua_tonumber(luaState, 1);
            float yTranslation = (float)lua_tonumber(luaState, 2);
            float zTranslation = (float)lua_tonumber(luaState, 3);

            ax::Mat4::createTranslation(xTranslation, yTranslation, zTranslation, &dst);
            mat4_to_luaval(luaState, dst);
            return 1;
        }
    }
    else if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec3 translation;
            ax::Mat4 dst;

            ok &= luaval_to_vec3(luaState, 1, &translation, "ax.Mat4.createTranslation");
            if (!ok)
                return 0;

            ax::Mat4::createTranslation(translation, &dst);
            mat4_to_luaval(luaState, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Mat4_createTranslation'.", &conversionError);
    return 0;
#endif
}

int axlua_Mat4_createRotation(lua_State* luaState)
{
    bool ok  = true;
    int argc = lua_gettop(luaState);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Quat quat;
            ax::Mat4 dst;

            ok &= luaval_to_quaternion(luaState, 1, &quat, "ax.Mat4.createRotation");
            if (!ok)
                return 0;

            ax::Mat4::createRotation(quat, &dst);
            mat4_to_luaval(luaState, dst);
            return 1;
        }
    }
    else if (argc == 2)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(luaState, 1, 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 2, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec3 axis;
            ax::Mat4 dst;

            ok &= luaval_to_vec3(luaState, 1, &axis, "ax.Mat4.createRotation");
            if (!ok)
                return 0;

            float angle = (float)axlua::adapter::to_number(luaState, 2, 0);

            ax::Mat4::createRotation(axis, angle, &dst);
            mat4_to_luaval(luaState, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_Mat4_createRotation'.", &conversionError);
    return 0;
#endif
}

static int axlua_Vec2_new(lua_State* L)
{
    return vec2_to_luaval(L, Vec2{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2))});
}

static int axlua_Vec3_new(lua_State* L)
{
    return vec3_to_luaval(L, Vec3{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)),
                                  static_cast<float>(lua_tonumber(L, 3))});
}

static int axlua_Vec4_new(lua_State* L)
{
    return vec4_to_luaval(L, Vec4{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)),
                                  static_cast<float>(lua_tonumber(L, 3)), static_cast<float>(lua_tonumber(L, 4))});
}

int register_all_ax_math_adapter(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    axlua::adapter::module(luaState, nullptr, 0);
    axlua::adapter::begin_module(luaState, nullptr);
    axlua::adapter::set_function(luaState, "mat4_getInversed", axlua_Mat4_getInversed);
    axlua::adapter::set_function(luaState, "mat4_transformVector", axlua_Mat4_transformVector);
    axlua::adapter::set_function(luaState, "mat4_decompose", axlua_Mat4_decompose);
    axlua::adapter::set_function(luaState, "mat4_multiply", axlua_Mat4_multiply);
    axlua::adapter::set_function(luaState, "mat4_translate", axlua_Mat4_translate);
    axlua::adapter::set_function(luaState, "mat4_createRotationZ", axlua_Mat4_createRotationZ);
    axlua::adapter::set_function(luaState, "mat4_setIdentity", axlua_Mat4_setIdentity);
    axlua::adapter::set_function(luaState, "mat4_createTranslation", axlua_Mat4_createTranslation);
    axlua::adapter::set_function(luaState, "mat4_createRotation", axlua_Mat4_createRotation);
    axlua::adapter::set_function(luaState, "vec3_cross", axlua_Vec3_cross);
    axlua::adapter::set_function(luaState, "vec2_new", axlua_Vec2_new);
    axlua::adapter::set_function(luaState, "vec3_new", axlua_Vec3_new);
    axlua::adapter::set_function(luaState, "vec4_new", axlua_Vec4_new);
    axlua::adapter::end_module(luaState);
    return 0;
}

int register_all_ax_shaders_adapter(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;
#define set_lua_field(field)                                     \
    do                                                           \
    {                                                            \
        lua_pushlstring(luaState, #field, sizeof(#field) - 1);   \
        lua_pushlstring(luaState, field.data(), field.length()); \
        lua_rawset(luaState, -3);                                \
    } while (false)

    axlua::adapter::open(luaState);
    axlua::adapter::module(luaState, "ax", 0);
    axlua::adapter::begin_module(luaState, "ax");
    set_lua_field(positionColor_vs);
    set_lua_field(positionColor_fs);
    set_lua_field(positionTexture_vs);
    set_lua_field(positionTexture_fs);
    set_lua_field(positionTextureColor_vs);
    set_lua_field(positionTextureColor_fs);
    set_lua_field(positionTextureColorAlphaTest_fs);
    set_lua_field(positionTextureGray_fs);
    set_lua_field(positionTextureGrayAlpha_fs);
    set_lua_field(label_normal_fs);
    set_lua_field(label_outline_fs);
    set_lua_field(label_distanceNormal_fs);
    set_lua_field(label_distanceOutline_fs);
    set_lua_field(label_distanceGlow_fs);
    set_lua_field(positionColorLengthTexture_vs);
    set_lua_field(positionColorLengthTexture_fs);
    set_lua_field(positionColorTextureAsPointsize_vs);
    set_lua_field(position_vs);
    set_lua_field(layer_radialGradient_vs);
    set_lua_field(layer_radialGradient_fs);
    set_lua_field(grayScale_fs);
    set_lua_field(positionUColor_vs);
    set_lua_field(dualSampler_fs);
    set_lua_field(dualSampler_gray_fs);
    set_lua_field(cameraClear_vs);
    set_lua_field(cameraClear_fs);
    set_lua_field(quadTexture_fs);
    set_lua_field(quadTexture_vs);
    set_lua_field(quadColor_fs);
    set_lua_field(quadColor_vs);
    set_lua_field(hsv_fs);
    set_lua_field(dualSampler_hsv_fs);
    set_lua_field(videoTextureYUY2_fs);
    set_lua_field(videoTextureNV12_fs);
    set_lua_field(videoTextureI420_fs);
    set_lua_field(posUVColor2D_vs);
    set_lua_field(lineColor_fs);
    set_lua_field(lineColor_vs);
    set_lua_field(color_fs);
    set_lua_field(colorNormal_fs);
    set_lua_field(colorNormalTexture_fs);
    set_lua_field(colorTexture_fs);
    set_lua_field(particleTexture_fs);
    set_lua_field(particleColor_fs);
    set_lua_field(particle_vs);
    set_lua_field(positionNormalTexture_vs);
    set_lua_field(skinPositionNormalTexture_vs);
    set_lua_field(unlit_vs);
    set_lua_field(unlit_instance_vs);
    set_lua_field(skinPositionTexture_vs);
    set_lua_field(skybox_fs);
    set_lua_field(skybox_vs);
    set_lua_field(terrain_fs);
    set_lua_field(terrain_vs);
    set_lua_field(colorNormalTexture_fs_1);
    set_lua_field(positionNormalTexture_vs_1);
    set_lua_field(skinPositionNormalTexture_vs_1);
    axlua::adapter::end_module(luaState);
    return 0;
}

static int axlua_bytearray_vec2(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec2 arg;
            ok &= luaval_to_vec2(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_vec3(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec3 arg;
            ok &= luaval_to_vec3(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_vec4(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Vec4 arg;
            ok &= luaval_to_vec4(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_mat4(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            ax::Mat4 arg;
            ok &= luaval_to_mat4(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_int(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_number(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            int arg;
            ok &= luaval_to_int(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_float(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_number(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            float arg;
            ok &= luaval_to_float(L, 1, &arg, "axlua_bytearray_template");
            if (!ok)
                return 0;

            lua_pop(L, 1);
            lua_newtable(L);
            uint8_t* bytes = (uint8_t*)&arg;
            for (auto idx = 0; idx < sizeof(arg); idx++)
            {
                lua_pushnumber(L, bytes[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_template'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_intv(lua_State* L)
{
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            int size = lua_objlen(L, 1);
            std::vector<uint8_t> arg(size * sizeof(int));

            int e;
            int* p = (int*)arg.data();

            for (auto idx = 0; idx < size; idx++)
            {
                lua_rawgeti(L, 1, idx + 1);
                luaval_to_int(L, -1, &e, "axlua_bytearray_elev");
                p[idx] = e;
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
            lua_newtable(L);
            for (size_t idx = 0; idx < arg.size(); idx++)
            {
                lua_pushnumber(L, arg[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_elev'.", &conversionError);
    return 0;
#endif
}

static int axlua_bytearray_floatv(lua_State* L)
{
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!axlua::adapter::is_table(L, 1, 0, &conversionError))
            goto argumentError;
        else
#endif
        {
            int size = lua_objlen(L, 1);
            std::vector<uint8_t> arg(size * sizeof(float));

            float e;
            float* p = (float*)arg.data();

            for (auto idx = 0; idx < size; idx++)
            {
                lua_rawgeti(L, 1, idx + 1);
                luaval_to_float(L, -1, &e, "axlua_bytearray_elev");
                p[idx] = e;
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
            lua_newtable(L);
            for (auto idx = 0; idx < arg.size(); idx++)
            {
                lua_pushnumber(L, arg[idx]);
                lua_rawseti(L, 1, idx + 1);
            }
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_bytearray_elev'.", &conversionError);
    return 0;
#endif
}

int register_all_ax_bytearray_adapter(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    axlua::adapter::module(luaState, "ax", 0);
    axlua::adapter::begin_module(luaState, "ax");
    axlua::adapter::module(luaState, "bytearray", 0);
    axlua::adapter::begin_module(luaState, "bytearray");
    axlua::adapter::set_function(luaState, "from_vec2", axlua_bytearray_vec2);
    axlua::adapter::set_function(luaState, "from_vec3", axlua_bytearray_vec3);
    axlua::adapter::set_function(luaState, "from_vec4", axlua_bytearray_vec4);
    axlua::adapter::set_function(luaState, "from_mat4", axlua_bytearray_mat4);
    axlua::adapter::set_function(luaState, "from_int", axlua_bytearray_int);
    axlua::adapter::set_function(luaState, "from_float", axlua_bytearray_float);
    axlua::adapter::set_function(luaState, "from_intv", axlua_bytearray_intv);
    axlua::adapter::set_function(luaState, "from_floatv", axlua_bytearray_floatv);
    axlua::adapter::end_module(luaState);
    axlua::adapter::end_module(luaState);
    return 0;
}
